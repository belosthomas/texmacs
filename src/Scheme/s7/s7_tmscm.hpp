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
#include <string>

namespace texmacs {

    class s7_tmscm : public abstract_tmscm {

    public:
        template<typename ...Args>
        static tmscm mk(Args &&... args) {
            return make_tmscm<s7_tmscm>(std::forward<Args>(args)...);
        }

        s7_tmscm(s7_scheme *sc, s7_pointer scm);

        ~s7_tmscm();

        s7_pointer &getSCM();

        int hash() override;

        tmscm null() override;

        void set_car(tmscm b) override;

        void set_cdr(tmscm b) override;

        bool is_equal(tmscm o2) override;

        bool is_null() override;

        bool is_pair() override;

        bool is_list() override;

        bool is_bool() override;

        bool is_int() override;

        bool is_double() override;

        bool is_string() override;

        bool is_symbol() override;

        bool is_function();

        tmscm cons(tmscm obj2) override;

        tmscm car() override;

        tmscm cdr() override;

        tmscm caar() override;

        tmscm cadr() override;

        tmscm cdar() override;

        tmscm cddr() override;

        tmscm caddr() override;

        tmscm cadddr() override;

        bool is_blackbox() override;

        blackbox to_blackbox() override;

        bool to_bool() override;

        int to_int() override;

        double to_double() override;

        string to_string() override;

        string to_symbol() override;

        string type();

    private:
        s7_scheme *mScheme;
        s7_pointer mSCM;
        std::string mType;
    };

}

#endif