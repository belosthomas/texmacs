#ifndef SCHEME_GUILE30_GUILETM_HPP
#define SCHEME_GUILE30_GUILETM_HPP

#include <mutex>
#include <thread>
#include <deque>
#include <functional>
#include <future>

#include "Scheme/abstract_scheme.hpp"
#include "guile18_tmscm.hpp"

#include <libguile.hpp>
#include <utility>

SCM &guile_blackbox_tag();

#define SET_SMOB(smob,data,type)   \
SCM_NEWSMOB (smob, SCM_UNPACK (type), data);

template <typename T, int C> class thread_safe_queue
{
public:
    thread_safe_queue() {
        mSize = 0;
        mPushIndex = 0;
        mPopIndex = 0;
    }

    inline T *getPushElement() {
        if (mIsDestroyed) {
            return &mElements[mPushIndex];
        }
        std::unique_lock<std::mutex> lock(mMutex);
        while (mSize == C) {
            mConditionVariable.wait_for(lock, std::chrono::milliseconds(10));
            if (mIsDestroyed) {
                return &mElements[mPushIndex];
            }
        }
        T *element = &mElements[mPushIndex];
        return element;
    }

    inline void notifyPush() {
        mPushIndex = (mPushIndex + 1) % C;
        mSize++;
        mConditionVariable.notify_all();
    }

    inline T *getPopElement()
    {
        if (mIsDestroyed) {
            mElements[mPopIndex] = T();
            return &mElements[mPopIndex];
        }
        std::unique_lock<std::mutex> lock(mMutex);
        while (mSize == 0) {
            mConditionVariable.wait_for(lock, std::chrono::milliseconds(10));
            if (mIsDestroyed) {
                mElements[mPopIndex] = T();
                return &mElements[mPopIndex];
            }
        }
        T *element = &mElements[mPopIndex];
        return element;
    }

    inline void notifyPop() {
        mPopIndex = (mPopIndex + 1) % C;
        mSize--;
        mConditionVariable.notify_all();
    }

    inline void destroy() {
        mIsDestroyed = true;
        mConditionVariable.notify_all();
    }

    inline bool isDestroyed() {
        return mIsDestroyed;
    }

    inline int getCurrentSize() {
        return mSize;
    }

private:
    T mElements[C];
    std::atomic<int> mSize;
    int mPushIndex, mPopIndex;
    std::mutex mMutex;
    std::condition_variable mConditionVariable;
    std::atomic<bool> mIsDestroyed = false;
};

class guile_thread {
public:
    guile_thread() {
        mThread = std::thread(&guile_thread::init, this);
    }

    void init() {
        guile_init_mutex.lock();
        guile_init_self = this;
        scm_use_embedded_ice9();
        scm_with_guile (guile_thread::c_run, nullptr);
    }

    static void* c_run(void* data) {
        return guile_init_self->run();
    }

    void *run() {
        guile_init_mutex.unlock();
        while (mRunning) {
            std::function<void()> f = *mQueue.getPopElement();
            if (mQueue.isDestroyed()) {
                break;
            }
            mQueue.notifyPop();
            f();
        }
        return nullptr;
    }

    void run(std::function<void()> f) {
        if (mThread.get_id() == std::this_thread::get_id()) {
            f();
            return;
        }
        *mQueue.getPushElement() = std::move(f);
        mQueue.notifyPush();
    }

private:
    std::thread mThread;
    thread_safe_queue<std::function<void()>, 10> mQueue;
    std::atomic<bool> mRunning;

    static guile_thread *guile_init_self;
    static std::mutex guile_init_mutex;
};

class guile_scheme : public abstract_scheme {
    
public:
    guile_scheme() : mThread() {
        string init = "(read-set! keywords 'prefix)\n"
                      "(read-enable 'positions)\n"
                      "(debug-enable 'debug)\n"
                      "(debug-enable 'backtrace)\n";
        eval_scheme(init);
    }

    tmscm blackbox_to_tmscm(blackbox b) final {
        std::promise<tmscm> promise;
        mThread.run([&promise, b]() {
            SCM blackbox_smob;
            SET_SMOB (blackbox_smob, (void*) (tm_new<blackbox> (b)), (SCM) guile_blackbox_tag());
            promise.set_value(guile_tmscm::mk(blackbox_smob));
        });
        return promise.get_future().get();
    }

    tmscm tmscm_null() final {
        return guile_tmscm_null::mk();
    }

    tmscm tmscm_true() final {
        return guile_tmscm::mk(SCM_BOOL_T, false);
    }

    tmscm tmscm_false() final {
        return guile_tmscm::mk(SCM_BOOL_F, false);
    }

    tmscm bool_to_tmscm(bool b) final {
        return guile_tmscm::mk(scm_from_bool(b));
    }

    tmscm int_to_tmscm(int i) final {
        return guile_tmscm::mk(scm_from_int32((int32_t)i));
    }

    tmscm long_to_tmscm(long l) final {
        return guile_tmscm::mk(scm_from_int32((int32_t)l));
    }

    tmscm double_to_tmscm(double i) final {
        return guile_tmscm::mk(scm_from_double (i));
    }

    tmscm string_to_tmscm(string s) final {
        c_string _s (s);
#ifdef DEBUG_ON
        if (! scm_busy) {
#endif
        SCM r= scm_from_locale_stringn(_s, N(s));
        return guile_tmscm::mk(r);
#ifdef DEBUG_ON
        } else return guile_tmscm::mk(SCM_BOOL_F);
#endif
    }

    tmscm symbol_to_tmscm(string s) final {
        c_string _s (s);
        SCM r= scm_from_locale_symbol (_s);
        return guile_tmscm::mk(r);
    }

    tmscm tmscm_unspefied() final {
        return guile_tmscm::mk(SCM_UNSPECIFIED, false);
    }

    tmscm eval_scheme_file(string name) final {
        std::promise<tmscm> promise;
        mThread.run([&promise, name]() {
            c_string _file (name);
            promise.set_value(guile_tmscm::mk(scm_c_primitive_load (_file)));
        });
        return promise.get_future().get();
    }

    tmscm eval_scheme(string s) final {
        std::promise<tmscm> promise;
        mThread.run([&promise, s]() {
            c_string _s(s);
            SCM result = scm_c_eval_string(_s);
            promise.set_value(guile_tmscm::mk(result));
        });
        return promise.get_future().get();
    }

    static SCM _call_scheme_args(SCM fun, std::vector<SCM> args) {
        switch (args.size()) {
            case 0: return scm_call_0(fun);
            case 1: return scm_call_1(fun, args[0]);
            case 2: return scm_call_2(fun, args[0], args[1]);
            case 3: return scm_call_3(fun, args[0], args[1], args[2]);
            default:
            {
                SCM l = scm_list_n(SCM_UNDEFINED);
                for (int i = args.size(); i>=0; i--) {
                    l = scm_cons(args[i], l);
                }
                return scm_apply_0(args[0], l);
            }
        }
    }

    tmscm call_scheme_args(tmscm fun, std::vector<tmscm> _args) final {
        std::promise<tmscm> promise;
        mThread.run([&promise, fun, _args]() {
            SCM fun_scm = tmscm_cast<guile_tmscm>(fun)->getSCM();
            std::vector<SCM> args;
            for (auto& arg : _args) {
                args.push_back(tmscm_cast<guile_tmscm>(arg)->getSCM());
            }
            return guile_tmscm::mk(_call_scheme_args(fun_scm, args));
        });
        return promise.get_future().get();
    }

    void install_procedure(string name, std::function<tmscm(abstract_scheme*,tmscm)> fun, int numArgs, int numOptional) final {
        assert(numOptional == 0); // not implemented
        mThread.run([this, name, fun, numArgs]() {
            c_string _name(name);
            register_callback(&*_name, [this, fun](SCM args) {
                return tmscm_cast<guile_tmscm>(fun(this, guile_tmscm::mk(args)))->getSCM();
            }, numArgs);
        });
    }

    string scheme_dialect() {
        return "guile-c";
    }

private:
    guile_thread mThread;


};

class Guile18Factory : public SchemeFactory {

public:
    abstract_scheme *make_scheme() final {
        return new guile_scheme();
    }

    virtual std::string name() {
        return "Guile++";
    }

};

void registerGuile18();

#endif

