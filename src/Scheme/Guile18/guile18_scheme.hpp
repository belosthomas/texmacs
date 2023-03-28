#ifndef SCHEME_GUILE30_GUILETM_HPP
#define SCHEME_GUILE30_GUILETM_HPP

#include <mutex>
#include <thread>
#include <deque>
#include <functional>
#include <future>

#include "Scheme/abstract_scheme.hpp"
#include "guile18_tmscm.hpp"
#include "guile_thread.hpp"

#include <utility>


#define SET_SMOB(smob,data,type)   \
SCM_NEWSMOB (smob, SCM_UNPACK (type), data);


namespace texmacs {

    scm_t_bits &guile_blackbox_tag();

    void initialize_smobs();

    class guile_scheme : public abstract_scheme {

    public:
        guile_scheme() : mThread() {
            mThread.run([]() {
                initialize_smobs();
            });
        }

        tmscm blackbox_to_tmscm(blackbox b) final {
            std::promise<tmscm> promise;
            mThread.run([&promise, b]() {
                try {
                    SCM blackbox_smob;
                    SET_SMOB (blackbox_smob, (void *) (tm_new<blackbox>(b)), (SCM) guile_blackbox_tag());
                    promise.set_value(guile_tmscm::mk(blackbox_smob));
                } catch (std::exception &e) {
                    promise.set_exception(std::current_exception());
                }
            });
            return promise.get_future().get();
        }

        tmscm tmscm_null() final {
            return guile_tmscm_null::mk();
        }

        tmscm tmscm_true() final {
            return guile_tmscm::mk(SCM_BOOL_T);
        }

        tmscm tmscm_false() final {
            return guile_tmscm::mk(SCM_BOOL_F);
        }

        tmscm bool_to_tmscm(bool b) final {
            return guile_tmscm::mk(scm_from_bool(b));
        }

        tmscm int_to_tmscm(int i) final {
            return guile_tmscm::mk(scm_from_int32((int32_t) i));
        }

        tmscm long_to_tmscm(long l) final {
            return guile_tmscm::mk(scm_from_int32((int32_t) l));
        }

        tmscm double_to_tmscm(double i) final {
            return guile_tmscm::mk(scm_from_double(i));
        }

        tmscm string_to_tmscm(string s) final {
            c_string _s(s);
            SCM r = scm_from_locale_stringn(_s, N(s));
            return guile_tmscm::mk(r);
        }

        tmscm symbol_to_tmscm(string s) final {
            c_string _s(s);
            SCM r = scm_from_locale_symbol(_s);
            return guile_tmscm::mk(r);
        }

        tmscm tmscm_unspefied() final {
            return guile_tmscm::mk(SCM_UNSPECIFIED);
        }

        tmscm eval_scheme_file(string name) final {
            std::promise<tmscm> promise;
            mThread.run([&promise, &name]() {
                try {
                    c_string _file(name);
                    promise.set_value(guile_tmscm::mk(scm_c_primitive_load(_file)));
                } catch (...) {
                    promise.set_exception(std::current_exception());
                }
            });
            return promise.get_future().get();
        }

        tmscm eval_scheme(string s) final {
            std::promise<tmscm> promise;
            mThread.run([&promise, s]() {
                try {
                    c_string _s(s);
                    SCM result = scm_c_eval_string(_s);
                    promise.set_value(guile_tmscm::mk(result));
                } catch (...) {
                    promise.set_exception(std::current_exception());
                }
            });
            return promise.get_future().get();
        }

        static SCM _call_scheme_args(SCM fun, std::vector<SCM> args) {
            switch (args.size()) {
                case 0:
                    return scm_call_0(fun);
                case 1:
                    return scm_call_1(fun, args[0]);
                case 2:
                    return scm_call_2(fun, args[0], args[1]);
                case 3:
                    return scm_call_3(fun, args[0], args[1], args[2]);
                default: {
                    SCM l = scm_list_n(SCM_UNDEFINED);
                    for (int i = args.size(); i >= 0; i--) {
                        l = scm_cons(args[i], l);
                    }
                    return scm_apply_0(args[0], l);
                }
            }
        }

        tmscm call_scheme_args(tmscm fun, std::vector<tmscm> _args) final {
            std::promise<tmscm> promise;
            mThread.run([&promise, fun, _args]() {
                try {
                    SCM fun_scm = tmscm_cast<guile_tmscm>(fun)->getSCM();
                    std::vector<SCM> args;
                    for (auto &arg: _args) {
                        args.push_back(tmscm_cast<guile_tmscm>(arg)->getSCM());
                    }
                    promise.set_value(guile_tmscm::mk(_call_scheme_args(fun_scm, args)));
                } catch (...) {
                    promise.set_exception(std::current_exception());
                }
            });
            return promise.get_future().get();
        }

        void install_procedure(string name, std::function<tmscm(abstract_scheme *, tmscm)> fun, int numArgs,
                               int numOptional) final {
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

}

#endif

