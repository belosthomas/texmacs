
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

    bool tmscm_is_blackbox (SCM t) {
        return SCM_BLACKBOXP (t);
    }

    blackbox tmscm_to_blackbox (SCM blackbox_smob) {
        return *((blackbox*) SCM_CDR (blackbox_smob));
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

