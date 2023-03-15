#ifndef SCHEME_GUILE30_TMSCM_HPP
#define SCHEME_GUILE30_TMSCM_HPP

#include "Scheme/tmscm.hpp"
#include <libguile.hpp>

SCM &guile_blackbox_tag();

class guile_tmscm : public abstract_tmscm {

public:
    template <typename ...Args> static tmscm mk(Args&&... args) {
        return make_tmscm<guile_tmscm>(std::forward<Args>(args)...);
    }

    guile_tmscm(SCM scm, bool protect = true) : mSCM(scm), mProtect(protect) {
        if (mProtect) {
            scm_gc_protect_object(mSCM);
        }
    }

    ~guile_tmscm() {
        if (mProtect) {
            scm_gc_unprotect_object(mSCM);
        }
    }

    SCM getSCM() const {
        return mSCM;
    }

    tmscm null() override;

    void set_car(tmscm b) override {
        SCM_SETCAR(mSCM,tmscm_cast<guile_tmscm>(b)->getSCM());
    }

    void set_cdr(tmscm b) override {
        SCM_SETCDR(mSCM,tmscm_cast<guile_tmscm>(b)->getSCM());
    }

    bool is_equal(tmscm o2) override {
        return scm_is_true (scm_equal_p(mSCM, tmscm_cast<guile_tmscm>(o2)->getSCM()));
    }

    bool is_null() override {
        return scm_is_null (mSCM);
    }

    bool is_pair() override {
        return scm_is_pair (mSCM);
    }

    bool is_list() override {
        return scm_is_true(scm_list_p(mSCM));
    }

    bool is_bool() override {
        return scm_is_bool (mSCM);
    }

    bool is_int() override {
        return scm_is_integer(mSCM);
    }

    bool is_double() override {
        return scm_is_real(mSCM);
    }

    bool is_string() override {
        return scm_is_string(mSCM);
    }

    bool is_symbol() override {
        return scm_is_symbol(mSCM);
    }

    tmscm cons(tmscm obj2) override {
        return guile_tmscm::mk(scm_cons(mSCM, tmscm_cast<guile_tmscm>(obj2)->getSCM()));
    }

    tmscm car() override {
        return guile_tmscm::mk(SCM_CAR (mSCM));
    }

    tmscm cdr() override {
        return guile_tmscm::mk(SCM_CDR (mSCM));
    }

    tmscm caar() override {
        return guile_tmscm::mk(SCM_CAAR (mSCM));
    }

    tmscm cadr() override {
        return guile_tmscm::mk(SCM_CADR (mSCM));
    }

    tmscm cdar() override {
        return guile_tmscm::mk(SCM_CDAR (mSCM));
    }

    tmscm cddr() override {
        return guile_tmscm::mk(SCM_CDDR (mSCM));
    }

    tmscm caddr() override {
        return guile_tmscm::mk(SCM_CADDR (mSCM));
    }

    tmscm cadddr() override {
        return guile_tmscm::mk(SCM_CADDDR(mSCM));
    }

    bool is_blackbox() override {
        return (SCM_NIMP (mSCM) && ((SCM_CAR (mSCM)) == guile_blackbox_tag()));
    }

    blackbox to_blackbox() override {
        return *((blackbox*) SCM_CDR (mSCM));
    }

    bool to_bool() override {
        return scm_to_bool (mSCM);
    }

    int to_int() override {
        return scm_to_int64(mSCM);
    }

    double to_double() override {
        return scm_to_double (mSCM);
    }

    string to_string() override {
        size_t len_r;
        char* _r= scm_to_locale_stringn(mSCM, &len_r);
        string r (_r, len_r);
        free (_r);
        return r;
    }

    string to_symbol() override {
        size_t len_r;
        char* _r=  scm_to_locale_stringn(scm_symbol_to_string(mSCM),&len_r);
        string r (_r, len_r);
        free (_r);
        return r;
    }

    int hash() {
        return scm_ihash(mSCM, std::numeric_limits<int>::max());
    }

private:
    SCM mSCM;
    bool mProtect;
};

class guile_tmscm_null : public guile_tmscm {

public:
    guile_tmscm_null() : guile_tmscm(scm_list_n(SCM_UNDEFINED), false) {

    }

    template<typename ...Args>
    static tmscm mk() {
        return make_tmscm<guile_tmscm_null>();
    }

    bool is_null() override {
        return true;
    }

    bool is_pair() override {
        return false;
    }

    bool is_list() override {
        return false;
    }

    bool is_bool() override {
        return false;
    }

    bool is_int() override {
        return false;
    }

    bool is_double() override {
        return false;
    }

    bool is_string() override {
        return false;
    }

    bool is_symbol() override {
        return false;
    }


};

tmscm guile_tmscm::null() {
    return guile_tmscm_null::mk();
}

#endif