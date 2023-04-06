/******************************************************************************
* MODULE     : s7/scheme.hpp
* DESCRIPTION: Abstract interface for the manipulation of scheme objects
* COPYRIGHT  : Liza Belos
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#ifndef SCHEME_GUILE30_GUILETM_HPP
#define SCHEME_GUILE30_GUILETM_HPP

#include "Scheme/abstract_scheme.hpp"
#include "s7_tmscm.hpp"

#include <s7.hpp>
#include <iostream>
#include <unordered_map>

namespace texmacs {

    s7_pointer s7_blackbox_to_string(s7_scheme *sc, s7_pointer args);

    s7_pointer s7_free_blackbox(s7_scheme *sc, s7_pointer obj);

    s7_pointer s7_mark_blackbox(s7_scheme *sc, s7_pointer obj);

    s7_pointer s7_blackbox_is_equal(s7_scheme *sc, s7_pointer args);

    class s7_tmscheme : public abstract_scheme {
        friend class S7Factory;

    public:
        s7_tmscheme();

        ~s7_tmscheme() final;

        // disable copy constructor and assignment operator
        s7_tmscheme(const s7_tmscheme &) = delete;

        s7_tmscheme &operator=(const s7_tmscheme &) = delete;

        tmscm blackbox_to_tmscm(blackbox b) final;

        tmscm tmscm_null() final;

        tmscm tmscm_true() final;

        tmscm tmscm_false() final;

        tmscm bool_to_tmscm(bool b) final;

        tmscm int_to_tmscm(int i) final;

        tmscm long_to_tmscm(long l) final;

        tmscm double_to_tmscm(double i) final;

        tmscm string_to_tmscm(string s) final;

        tmscm symbol_to_tmscm(string s) final;

        tmscm tmscm_unspefied() final;

        tmscm eval_scheme_file(string name);

        tmscm eval_scheme(string s);

        s7_pointer _call_scheme_args(s7_pointer fun, s7_pointer args);

        s7_pointer _call_scheme_args(s7_pointer fun, std::vector<s7_pointer> args);

        tmscm call_scheme_args(tmscm fun, std::vector<tmscm> _args);

        void install_procedure(string name, std::function<tmscm(abstract_scheme *, tmscm)> fun, int numArgs, int numOptional) final;

        string scheme_dialect() {
            return "s7";
        }

        std::function<s7_pointer(s7_scheme *, s7_pointer)> &getFunction(int id) {
            return mProxyFunctionHolder[id];
        }

        int blackboxTag() const {
            return mBlackboxTag;
        }

    private:
        s7_scheme *mInstance;
        std::vector<std::function<tmscm(abstract_scheme *, tmscm)>> mFunctionHolder;
        std::vector<std::function<s7_pointer(s7_scheme *, s7_pointer)>> mProxyFunctionHolder;
        int mBlackboxTag = 0;
        s7_pointer mUserEnv;
    };

    class S7Factory : public SchemeFactory {

    public:
        abstract_scheme *make_scheme() final;

        virtual std::string name() {
            return "S7";
        }

    };

}


#endif

