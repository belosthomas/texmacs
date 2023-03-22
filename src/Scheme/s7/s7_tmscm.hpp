/******************************************************************************
* MODULE     : s7/tmscm.hpp
* DESCRIPTION: Abstract interface for the manipulation of scheme objects
* COPYRIGHT  : Liza Belos
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#ifndef SCHEME_GUILE30_TMSCM_HPP
#define SCHEME_GUILE30_TMSCM_HPP

#include "Scheme/tmscm.hpp"

#include <s7.hpp>

namespace texmacs {

    class s7_tmscm : public abstract_tmscm {

    public:
        template<typename ...Args>
        static tmscm mk(Args &&... args) {
            return make_tmscm<s7_tmscm>(std::forward<Args>(args)...);
        }

        s7_tmscm(s7_scheme *sc, s7_pointer scm) : mScheme(sc), mSCM(scm) {
            if (mSCM == nullptr) {
                mSCM = s7_nil(mScheme);
            }
            string t = type();
            mType = std::string(t.data(), N(t));
            s7_gc_protect(mScheme, mSCM);
        }

        ~s7_tmscm() {
        }

        s7_pointer &getSCM() {
            return mSCM;
        }

        int hash() override {
            return std::hash<void *>()(mSCM.get());
        }

        tmscm null() override {
            return s7_tmscm::mk(mScheme, s7_nil(mScheme));
        }

        void set_car(tmscm b) override {
            s7_set_car(mSCM, tmscm_cast<s7_tmscm>(b)->getSCM());
        }

        void set_cdr(tmscm b) override {
            s7_set_cdr(mSCM, tmscm_cast<s7_tmscm>(b)->getSCM());
        }

        bool is_equal(tmscm o2) override {
            return s7_is_equal(mScheme, mSCM, tmscm_cast<s7_tmscm>(o2)->getSCM());
        }

        bool is_null() override {
            return s7_is_null(mScheme, mSCM) || (s7_is_list(mScheme, mSCM) && s7_list_length(mScheme, mSCM) == 0);
        }

        bool is_pair() override {
            return s7_is_pair(mSCM);
        }

        bool is_list() override {
            return s7_is_list(mScheme, mSCM);
        }

        bool is_bool() override {
            return s7_is_boolean(mSCM);
        }

        bool is_int() override {
            return s7_is_integer(mSCM);
        }

        bool is_double() override {
            return s7_is_real(mSCM);
        }

        bool is_string() override {
            return s7_is_string(mSCM);
        }

        bool is_symbol() override {
            return s7_is_symbol(mSCM);
        }

        bool is_function() {
            return s7_is_function(mSCM);
        }

        tmscm cons(tmscm obj2) override {
            return s7_tmscm::mk(mScheme, s7_cons(mScheme, mSCM, tmscm_cast<s7_tmscm>(obj2)->getSCM()));
        }

        tmscm car() override {
            return s7_tmscm::mk(mScheme, s7_car(mSCM));
        }

        tmscm cdr() override {
            return s7_tmscm::mk(mScheme, s7_cdr(mSCM));
        }

        tmscm caar() override {
            return s7_tmscm::mk(mScheme, s7_caar(mSCM));
        }

        tmscm cadr() override {
            return s7_tmscm::mk(mScheme, s7_cadr(mSCM));
        }

        tmscm cdar() override {
            return s7_tmscm::mk(mScheme, s7_cdar(mSCM));
        }

        tmscm cddr() override {
            return s7_tmscm::mk(mScheme, s7_cddr(mSCM));
        }

        tmscm caddr() override {
            return s7_tmscm::mk(mScheme, s7_caddr(mSCM));
        }

        tmscm cadddr() override {
            return s7_tmscm::mk(mScheme, s7_cadddr(mSCM));
        }

        bool is_blackbox() override;

        blackbox to_blackbox() override {
            assert(is_blackbox());
            return *((blackbox *) s7_c_object_value(mSCM));
        }

        bool to_bool() override {
            assert(is_bool());
            return s7_boolean(mScheme, mSCM);
        }

        int to_int() override {
            assert(is_int());
            return s7_integer(mSCM);
        }

        double to_double() override {
            assert(is_double());
            return s7_real(mSCM);
        }

        string to_string() override {
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

        string to_symbol() override {
            assert(is_symbol());
            const char *_r = s7_symbol_name(mSCM);
            string r(_r);
            // free ((void*)_r); // do not free the string
            return r;
        }

        string type() {
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

    private:
        s7_scheme *mScheme;
        s7_pointer mSCM;
        std::string mType;
    };

}

#endif