
#include "guile18_scheme.hpp"

scm_t_bits blackbox_tag;

scm_t_bits &texmacs::guile_blackbox_tag() {
    return blackbox_tag;
}

void texmacs::register_scheme_factory(SchemeFactory *factory);

void texmacs::registerGuile18() {
    register_scheme_factory(new Guile18Factory);
}

#define SCM_BLACKBOXP(t) (SCM_NIMP (t) && (((scm_t_bits)SCM_CAR (t)) == blackbox_tag))
#define SCM_ENABLE_EXCEPTION_RETHROW 1

bool tmscm_is_blackbox(SCM t) {
    return SCM_BLACKBOXP (t);
}

blackbox tmscm_to_blackbox(SCM blackbox_smob) {
    return *((blackbox *) SCM_CDR (blackbox_smob));
}

static SCM mark_blackbox(SCM blackbox_smob) {
    (void) blackbox_smob;
    return SCM_BOOL_F;
}

static size_t free_blackbox(SCM blackbox_smob) {
    blackbox *ptr = (blackbox *) SCM_CDR (blackbox_smob);
    tm_delete(ptr);
    return 0;
}

int print_blackbox(SCM blackbox_smob, SCM port, scm_print_state *pstate) {
    scm_puts("blackbox", port);
    return 1;
}

static SCM cmp_blackbox(SCM t1, SCM t2) {
    return scm_from_bool(tmscm_to_blackbox(t1) == tmscm_to_blackbox(t2));
}

void texmacs::initialize_smobs() {
    blackbox_tag = scm_make_smob_type("blackbox", 0);
    scm_set_smob_mark(blackbox_tag, mark_blackbox);
    scm_set_smob_free(blackbox_tag, free_blackbox);
    scm_set_smob_print(blackbox_tag, print_blackbox);
    scm_set_smob_equalp(blackbox_tag, cmp_blackbox);
}

texmacs::guile_scheme::guile_scheme() : mThread() {
    mThread.addToLaunchQueue([]() {
        initialize_smobs();
    });
}

tmscm texmacs::guile_scheme::blackbox_to_tmscm(blackbox b) {
    std::promise<tmscm> promise;
    mThread.addToLaunchQueue([&promise, b, this]() {
        try {
            SCM blackbox_smob;
            SET_SMOB (blackbox_smob, (void *) (tm_new<blackbox>(b)), (SCM) guile_blackbox_tag());
            promise.set_value(guile_tmscm::mk(this, blackbox_smob));
        } catch (std::exception &e) {
#if SCM_ENABLE_EXCEPTION_RETHROW
            promise.set_exception(std::current_exception());
#else
            promise.set_value(tmscm_null());
#endif
        }
    });
    return promise.get_future().get();
}

tmscm texmacs::guile_scheme::tmscm_null() {
    return guile_tmscm::mk(this, scm_list_n(SCM_UNDEFINED));
}

tmscm texmacs::guile_scheme::tmscm_true() {
    return guile_tmscm::mk(this, SCM_BOOL_T);
}

tmscm texmacs::guile_scheme::tmscm_false() {
    return guile_tmscm::mk(this, SCM_BOOL_F);
}

tmscm texmacs::guile_scheme::bool_to_tmscm(bool b) {
    return guile_tmscm::mk(this, scm_from_bool(b));
}

tmscm texmacs::guile_scheme::int_to_tmscm(int i) {
    return guile_tmscm::mk(this, scm_from_int32((int32_t) i));
}

tmscm texmacs::guile_scheme::long_to_tmscm(long l) {
    return guile_tmscm::mk(this, scm_from_int32((int32_t) l));
}

tmscm texmacs::guile_scheme::double_to_tmscm(double i) {
    return guile_tmscm::mk(this, scm_from_double(i));
}

tmscm texmacs::guile_scheme::string_to_tmscm(string s) {
    c_string _s(s);
    SCM r = scm_from_locale_stringn(_s, N(s));
    return guile_tmscm::mk(this, r);
}

tmscm texmacs::guile_scheme::symbol_to_tmscm(string s) {
    c_string _s(s);
    SCM r = scm_from_locale_symbol(_s);
    return guile_tmscm::mk(this, r);
}

tmscm texmacs::guile_scheme::tmscm_unspefied() {
    return guile_tmscm::mk(this, SCM_UNSPECIFIED);
}

tmscm texmacs::guile_scheme::eval_scheme_file(string name) {
    std::promise<tmscm> promise;
    mThread.addToLaunchQueue([&promise, &name, this]() {
        try {
            c_string _file(name);
            promise.set_value(guile_tmscm::mk(this, scm_c_primitive_load(_file)));
        } catch (...) {
#if SCM_ENABLE_EXCEPTION_RETHROW
            promise.set_exception(std::current_exception());
#else
            promise.set_value(tmscm_null());
#endif
        }
    });
    return promise.get_future().get();
}

tmscm texmacs::guile_scheme::eval_scheme(string s) {
    std::promise<tmscm> promise;
    mThread.addToLaunchQueue([&promise, s, this]() {
        try {
            c_string _s(s);
            SCM result = scm_c_eval_string(_s);
            promise.set_value(guile_tmscm::mk(this, result));
        } catch (...) {
#if SCM_ENABLE_EXCEPTION_RETHROW
            promise.set_exception(std::current_exception());
#else
            promise.set_value(tmscm_null());
#endif
        }
    });
    return promise.get_future().get();
}

SCM texmacs::guile_scheme::_call_scheme_args(SCM fun, std::vector<SCM> args) {
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
            for (auto it = args.rbegin(); it != args.rend(); ++it) {
                l = scm_cons(*it, l);
            }
            return scm_apply_0(fun, l);
        }
    }
}

tmscm texmacs::guile_scheme::call_scheme_args(tmscm fun, std::vector<tmscm> _args) {
    std::promise<tmscm> promise;
    mThread.addToLaunchQueue([&promise, fun, _args, this]() {
        try {
            SCM fun_scm = tmscm_cast<guile_tmscm>(fun)->getSCM();
            std::vector<SCM> args;
            for (auto &arg: _args) {
                args.push_back(tmscm_cast<guile_tmscm>(arg)->getSCM());
            }
            promise.set_value(guile_tmscm::mk(this, _call_scheme_args(fun_scm, args)));
        } catch (...) {
#if SCM_ENABLE_EXCEPTION_RETHROW
            promise.set_exception(std::current_exception());
#else
            promise.set_value(tmscm_null());
#endif
        }
    });
    return promise.get_future().get();
}

void texmacs::guile_scheme::install_procedure(string name, std::function<tmscm(abstract_scheme *, tmscm)> fun, int numArgs, int numOptional) {
    assert(numOptional == 0); // not implemented
    mThread.addToLaunchQueue([this, name, fun, numArgs]() {
        c_string _name(name);
        register_callback(&*_name, [this, fun](SCM args) {
            return tmscm_cast<guile_tmscm>(fun(this, guile_tmscm::mk(this, args)))->getSCM();
        }, numArgs);
    });
}

texmacs::abstract_scheme *texmacs::Guile18Factory::make_scheme() {
    return new guile_scheme();
}

std::string texmacs::Guile18Factory::name() {
    return "Guile++";
}