#include "s7_tmscm.hpp"

#include <string>

texmacs::s7_tmscm::s7_tmscm(s7_scheme *sc, s7_pointer scm) : mScheme(sc), mSCM(scm) {
    if (mSCM == nullptr) {
        mSCM = s7_nil(mScheme);
    }
    string t = type();
    mType = std::string(t.data(), N(t));
    s7_gc_protect(mScheme, mSCM);
}

texmacs::s7_tmscm::~s7_tmscm() {
}

s7_pointer &texmacs::s7_tmscm::getSCM() {
    return mSCM;
}

int texmacs::s7_tmscm::hash() {
    return std::hash<void *>()(mSCM.get());
}

tmscm texmacs::s7_tmscm::null() {
    return s7_tmscm::mk(mScheme, s7_nil(mScheme));
}

void texmacs::s7_tmscm::set_car(tmscm b) {
    s7_set_car(mSCM, tmscm_cast<s7_tmscm>(b)->getSCM());
}

void texmacs::s7_tmscm::set_cdr(tmscm b) {
    s7_set_cdr(mSCM, tmscm_cast<s7_tmscm>(b)->getSCM());
}

bool texmacs::s7_tmscm::is_equal(tmscm o2) {
    return s7_is_equal(mScheme, mSCM, tmscm_cast<s7_tmscm>(o2)->getSCM());
}

bool texmacs::s7_tmscm::is_null() {
    return s7_is_null(mScheme, mSCM) || (s7_is_list(mScheme, mSCM) && s7_list_length(mScheme, mSCM) == 0);
}

bool texmacs::s7_tmscm::is_pair() {
    return s7_is_pair(mSCM);
}

bool texmacs::s7_tmscm::is_list() {
    return s7_is_list(mScheme, mSCM);
}

bool texmacs::s7_tmscm::is_bool() {
    return s7_is_boolean(mSCM);
}

bool texmacs::s7_tmscm::is_int() {
    return s7_is_integer(mSCM);
}

bool texmacs::s7_tmscm::is_double() {
    return s7_is_real(mSCM);
}

bool texmacs::s7_tmscm::is_string() {
    return s7_is_string(mSCM);
}

bool texmacs::s7_tmscm::is_symbol() {
    return s7_is_symbol(mSCM);
}

bool texmacs::s7_tmscm::is_function() {
    return s7_is_function(mSCM);
}

tmscm texmacs::s7_tmscm::cons(tmscm obj2) {
    return s7_tmscm::mk(mScheme, s7_cons(mScheme, mSCM, tmscm_cast<s7_tmscm>(obj2)->getSCM()));
}

tmscm texmacs::s7_tmscm::car() {
    return s7_tmscm::mk(mScheme, s7_car(mSCM));
}

tmscm texmacs::s7_tmscm::cdr() {
    return s7_tmscm::mk(mScheme, s7_cdr(mSCM));
}

tmscm texmacs::s7_tmscm::caar() {
    return s7_tmscm::mk(mScheme, s7_caar(mSCM));
}

tmscm texmacs::s7_tmscm::cadr() {
    return s7_tmscm::mk(mScheme, s7_cadr(mSCM));
}

tmscm texmacs::s7_tmscm::cdar() {
    return s7_tmscm::mk(mScheme, s7_cdar(mSCM));
}

tmscm texmacs::s7_tmscm::cddr() {
    return s7_tmscm::mk(mScheme, s7_cddr(mSCM));
}

tmscm texmacs::s7_tmscm::caddr() {
    return s7_tmscm::mk(mScheme, s7_caddr(mSCM));
}

tmscm texmacs::s7_tmscm::cadddr() {
    return s7_tmscm::mk(mScheme, s7_cadddr(mSCM));
}

blackbox texmacs::s7_tmscm::to_blackbox() {
    assert(is_blackbox());
    return *((blackbox *) s7_c_object_value(mSCM));
}

bool texmacs::s7_tmscm::to_bool() {
    assert(is_bool());
    return s7_boolean(mScheme, mSCM);
}

int texmacs::s7_tmscm::to_int() {
    assert(is_int());
    return s7_integer(mSCM);
}

double texmacs::s7_tmscm::to_double() {
    assert(is_double());
    return s7_real(mSCM);
}

string texmacs::s7_tmscm::to_string() {
    if (is_list() || is_pair()) {
        list < string > l = to_list_string();
        string res;
        for (int i = 0; i < N(l); i++) {
            res = res * l[i];
        }
        return res;
    }
    assert(is_string());
    size_t len_r = s7_string_length(mSCM);
    const char *_r = s7_string(mSCM);
    string r(_r, len_r);
    // free ((void*)_r); // do not free the string
    return r;
}

string texmacs::s7_tmscm::to_symbol() {
    assert(is_symbol());
    const char *_r = s7_symbol_name(mSCM);
    string r(_r);
    // free ((void*)_r); // do not free the string
    return r;
}

string texmacs::s7_tmscm::type() {
    string type = "";
    if (s7_is_valid(mScheme, mSCM)) {
        type = "valid";
    } else {
        type = "invalid";
    }
    if (s7_is_unspecified(mScheme, mSCM)) {
        type = type * "/unspecified";
    }
    if (s7_is_boolean(mSCM)) {
        type = type * "/bool";
    }
    if (s7_is_integer(mSCM)) {
        type = type * "/int";
    }
    if (s7_is_real(mSCM)) {
        type = type * "/double";
    }
    if (s7_is_string(mSCM)) {
        type = type * "/string";
    }
    if (s7_is_symbol(mSCM)) {
        type = type * "/symbol";
    }
    if (s7_is_pair(mSCM)) {
        type = type * "/pair";
    }
    if (s7_is_list(mScheme, mSCM)) {
        type = type * "/list";
    }
    if (s7_is_proper_list(mScheme, mSCM)) {
        type = type * "/proper_list";
    }
    if (s7_is_null(mScheme, mSCM)) {
        type = type * "/null";
    }
    if (s7_is_function(mSCM)) {
        type = type * "/function";
    }
    if (s7_is_procedure(mSCM)) {
        type = type * "/procedure";
    }
    if (s7_is_c_pointer(mSCM)) {
        type = type * "/c-pointer";
    }
    if (s7_is_macro(mScheme, mSCM)) {
        type = type * "/macro";
    }
    if (s7_is_character(mSCM)) {
        type = type * "/character";
    }
    if (s7_is_number(mSCM)) {
        type = type * "/number";
    }
    if (s7_is_rational(mSCM)) {
        type = type * "/rational";
    }
    if (s7_is_ratio(mSCM)) {
        type = type * "/ratio";
    }
    if (s7_is_random_state(mSCM)) {
        type = type * "/random-state";
    }
    if (s7_is_complex(mSCM)) {
        type = type * "/complex";
    }
    if (s7_is_vector(mSCM)) {
        type = type * "/vector";
    }
    if (s7_is_float_vector(mSCM)) {
        type = type * "/float-vector";
    }
    if (s7_is_int_vector(mSCM)) {
        type = type * "/int-vector";
    }
    if (s7_is_byte_vector(mSCM)) {
        type = type * "/byte-vector";
    }
    if (s7_is_hash_table(mSCM)) {
        type = type * "/hash-table";
    }
    if (s7_is_input_port(mScheme, mSCM)) {
        type = type * "/input-port";
    }
    if (s7_is_output_port(mScheme, mSCM)) {
        type = type * "/output-port";
    }
    if (s7_is_syntax(mSCM)) {
        type = type * "/syntax";
    }
    if (s7_is_keyword(mSCM)) {
        type = type * "/keyword";
    }
    if (s7_is_openlet(mSCM)) {
        type = type * "/openlet";
    }
    if (s7_is_dilambda(mSCM)) {
        type = type * "/dilambda";
    }
    if (s7_is_iterator(mSCM)) {
        type = type * "/iterator";
    }
    if (s7_is_c_object(mSCM)) {
        type = type * "/c-object";
    }
    return type;
}