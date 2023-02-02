#ifndef SCHEME_GUILE30_GUILETM_HPP
#define SCHEME_GUILE30_GUILETM_HPP

#include "Scheme/abstract_scheme.hpp"
#include "tmscm.hpp"

extern "C" {
    #include <guile/3.0/libguile.h>
}

SCM &guile_blackbox_tag();

#define SET_SMOB(smob,data,type)   \
SCM_NEWSMOB (smob, SCM_UNPACK (type), data);

class guile_scheme : public abstract_scheme {
    
public:
    guile_scheme() {
        scm_init_guile();
    }

    /// TSCM Methods ///////////////////////////////////////////////////

    tmscm blackbox_to_tmscm(blackbox b) final {
        SCM blackbox_smob;
        SET_SMOB (blackbox_smob, (void*) (tm_new<blackbox> (b)), (SCM) guile_blackbox_tag());
        return guile_tmscm::mk(blackbox_smob);
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

    tmscm int_to_tmscm(int64_t i) final {
        return guile_tmscm::mk(scm_from_int64((int64_t)i));
    }

    tmscm long_to_tmscm(long l) final {
        return guile_tmscm::mk(scm_from_int64((int64_t)l));
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

    tmscm eval_scheme_file(string name) {
        c_string _file (name);
        return guile_tmscm::mk(scm_c_primitive_load (_file));
    }

    tmscm eval_scheme(string s) {
#ifdef DEBUG_ON
        if (!scm_busy) {
#endif
        c_string _s (s);
        SCM result= scm_c_eval_string(_s);
        return guile_tmscm::mk(result);
#ifdef DEBUG_ON
        } else return SCM_BOOL_F;
#endif
    }

    SCM _call_scheme_args(SCM fun, std::vector<SCM> args) {
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

    tmscm call_scheme_args(tmscm fun, std::vector<tmscm> _args) {
        SCM fun_scm = tmscm_cast<guile_tmscm>(fun)->getSCM();
        std::vector<SCM> args;
        for (auto& arg : _args) {
            args.push_back(tmscm_cast<guile_tmscm>(arg)->getSCM());
        }
        return guile_tmscm::mk(_call_scheme_args(fun_scm, args));
    }

    void tmscm_install_procedure_impl(string name, void *func, int args, int p0, int p1) final {
        c_string _name(name);
        scm_c_define_gsubr (_name, args, p0, p1, (scm_t_subr)func);
    }

    string scheme_dialect() {
        return "guile-d";
    }


};

class Guile30Factory : public SchemeFactory {

public:
    abstract_scheme *make_scheme() final {
        return new guile_scheme();
    }

    virtual std::string name() {
        return "Guile 3.0";
    }

};

void registerGuile30();

#endif

