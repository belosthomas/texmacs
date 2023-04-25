#include "guile18_tmscm.hpp"
#include "guile18_scheme.hpp"


std::unordered_map<SCM, int> guile_tmscm_number_of_references;

void texmacs::guile_tmscm::mark() {
    if (guile_tmscm_number_of_references.find(mSCM) == guile_tmscm_number_of_references.end()) {
        guile_tmscm_number_of_references[mSCM] = 0;
    }
    if (guile_tmscm_number_of_references[mSCM] == 0) {
        scm_gc_protect_object(mSCM);
    }
    guile_tmscm_number_of_references[mSCM]++;
    // scm_gc_protect_object(mSCM);
}

void texmacs::guile_tmscm::unmark() {
    guile_tmscm_number_of_references[mSCM]--;
    if (guile_tmscm_number_of_references[mSCM] == 0) {
        scm_gc_unprotect_object(mSCM);
    }
}

texmacs::guile_tmscm::guile_tmscm(guile_scheme *scheme, SCM scm) : mScheme(scheme), mSCM(scm) {
    string t = type();
    mType = std::string(t.data(), N(t));
    mark();
}

texmacs::guile_tmscm::~guile_tmscm() {
    unmark();
}

SCM texmacs::guile_tmscm::getSCM() const {
    return mSCM;
}

tmscm texmacs::guile_tmscm::null() {
    return guile_tmscm::mk(mScheme, scm_list_n(SCM_UNDEFINED));
}

void texmacs::guile_tmscm::set_car(tmscm b) {
    SCM_SETCAR(mSCM, tmscm_cast<guile_tmscm>(b)->getSCM());
}

void texmacs::guile_tmscm::set_cdr(tmscm b) {
    SCM_SETCDR(mSCM, tmscm_cast<guile_tmscm>(b)->getSCM());
}

bool texmacs::guile_tmscm::is_equal(tmscm o2) {
    return scm_is_true (scm_equal_p(mSCM, tmscm_cast<guile_tmscm>(o2)->getSCM()));
}

bool texmacs::guile_tmscm::is_null() {
    return scm_is_null (mSCM);
}

bool texmacs::guile_tmscm::is_pair() {
    return scm_is_pair(mSCM);
}

bool texmacs::guile_tmscm::is_list() {
    return scm_is_true(scm_list_p(mSCM));
}

bool texmacs::guile_tmscm::is_bool() {
    return scm_is_bool(mSCM);
}

bool texmacs::guile_tmscm::is_int() {
    return scm_is_integer(mSCM);
}

bool texmacs::guile_tmscm::is_double() {
    return scm_is_real(mSCM);
}

bool texmacs::guile_tmscm::is_string() {
    return scm_is_string(mSCM);
}

bool texmacs::guile_tmscm::is_symbol() {
    return scm_is_symbol(mSCM);
}

tmscm texmacs::guile_tmscm::cons(tmscm obj2) {
    return guile_tmscm::mk(mScheme,scm_cons(mSCM, tmscm_cast<guile_tmscm>(obj2)->getSCM()));
}

tmscm texmacs::guile_tmscm::car() {
    return guile_tmscm::mk(mScheme,SCM_CAR (mSCM));
}

tmscm texmacs::guile_tmscm::cdr() {
    return guile_tmscm::mk(mScheme,SCM_CDR (mSCM));
}

tmscm texmacs::guile_tmscm::caar() {
    return guile_tmscm::mk(mScheme,SCM_CAAR (mSCM));
}

tmscm texmacs::guile_tmscm::cadr() {
    return guile_tmscm::mk(mScheme,SCM_CADR (mSCM));
}

tmscm texmacs::guile_tmscm::cdar() {
    return guile_tmscm::mk(mScheme,SCM_CDAR (mSCM));
}

tmscm texmacs::guile_tmscm::cddr() {
    return guile_tmscm::mk(mScheme,SCM_CDDR (mSCM));
}

tmscm texmacs::guile_tmscm::caddr() {
    return guile_tmscm::mk(mScheme,SCM_CADDR (mSCM));
}

tmscm texmacs::guile_tmscm::cadddr() {
    return guile_tmscm::mk(mScheme,SCM_CADDDR(mSCM));
}

bool texmacs::guile_tmscm::is_blackbox() {
    return (SCM_NIMP (mSCM) && (((scm_t_bits)SCM_CAR (mSCM)) == guile_blackbox_tag()));
}

blackbox texmacs::guile_tmscm::to_blackbox() {
    return *((blackbox *) SCM_CDR (mSCM));
}

bool texmacs::guile_tmscm::to_bool() {
    return scm_to_bool(mSCM);
}

int texmacs::guile_tmscm::to_int() {
    return scm_to_int64(mSCM);
}

double texmacs::guile_tmscm::to_double() {
    return scm_to_double(mSCM);
}

string texmacs::guile_tmscm::to_string() {
    size_t len_r;
    char *_r = scm_to_locale_stringn(mSCM, &len_r);
    string r(_r, len_r);
    free(_r);
    return r;
}

string texmacs::guile_tmscm::to_symbol() {
    size_t len_r;
    char *_r = scm_to_locale_stringn(scm_symbol_to_string(mSCM), &len_r);
    string r(_r, len_r);
    free(_r);
    return r;
}

int texmacs::guile_tmscm::hash() {
    return scm_ihash(mSCM, std::numeric_limits<int>::max(), NULL);
}

string texmacs::guile_tmscm::type() {
    string type = "";
    if (scm_is_null(mSCM)) {
        type = "null";
        return type;
    } else {
        type = "not null";
    }
    if (scm_is_integer(mSCM)) {
        type = type * "/int";
    }
    if (scm_is_real(mSCM)) {
        type = type * "/double";
    }
    if (scm_is_string(mSCM)) {
        type = type * "/string:'" * to_string() * "'";
    }
    if (scm_is_symbol(mSCM)) {
        type = type * "/symbol";
    }
    if (scm_is_pair(mSCM)) {
        type = type * "/pair";
    }
    if (scm_is_true(scm_list_p(mSCM))) {
        type = type * "/true";
    }
    if (scm_is_false(scm_list_p(mSCM))) {
        type = type * "/false";
    }
    if (scm_is_bool(mSCM)) {
        type = type * "/bool";
    }
    if (SCM_NIMP (mSCM)) {
        type = type * "/nimp";
    }
    if (SCM_IMP (mSCM)) {
        type = type * "/imp";
    }
    return type;
}