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
#include "tmscm.hpp"

#include <s7.hpp>
#include <iostream>
#include <unordered_map>

s7_pointer s7_blackbox_to_string (s7_scheme *sc, s7_pointer args);
s7_pointer s7_free_blackbox (s7_scheme *sc, s7_pointer obj);
s7_pointer s7_mark_blackbox (s7_scheme *sc, s7_pointer obj);
s7_pointer s7_blackbox_is_equal(s7_scheme *sc, s7_pointer args);

class s7_tmscheme : public abstract_scheme {
    friend class S7Factory;

public:
    s7_tmscheme();

    ~s7_tmscheme() {
        s7_free(mInstance);
    }

    // disable copy constructor and assignment operator
    s7_tmscheme(const s7_tmscheme&) = delete;
    s7_tmscheme& operator=(const s7_tmscheme&) = delete;

    /// TSCM Methods ///////////////////////////////////////////////////

    tmscm blackbox_to_tmscm(blackbox b) final {
        auto obj = s7_make_c_object (mInstance, blackboxTag(), (void*) (tm_new<blackbox> (b)));
        return s7_tmscm::mk(mInstance, obj);
    }

    tmscm tmscm_null() final {
        return s7_tmscm::mk(mInstance, s7_nil(mInstance));
    }

    tmscm tmscm_true() final {
        return s7_tmscm::mk(mInstance, s7_t(mInstance));
    }

    tmscm tmscm_false() final {
        return s7_tmscm::mk(mInstance, s7_f(mInstance));
    }

    tmscm bool_to_tmscm(bool b) final {
        return s7_tmscm::mk(mInstance, s7_make_boolean(mInstance, b));
    }

    tmscm int_to_tmscm(int64_t i) final {
        return s7_tmscm::mk(mInstance, s7_make_integer(mInstance, i));
    }

    tmscm long_to_tmscm(long l) final {
        return s7_tmscm::mk(mInstance, s7_make_integer(mInstance, l));
    }

    tmscm double_to_tmscm(double i) final {
        return s7_tmscm::mk(mInstance, s7_make_real(mInstance, i));
    }

    tmscm string_to_tmscm(string s) final {
        c_string _s (s);
        s7_pointer r = s7_make_string(mInstance, _s);
        return s7_tmscm::mk(mInstance, r);
    }

    tmscm symbol_to_tmscm(string s) final {
        c_string _s (s);
        s7_pointer r = s7_make_symbol(mInstance, _s);
        return s7_tmscm::mk(mInstance, r);
    }

    tmscm tmscm_unspefied() final {
        return s7_tmscm::mk(mInstance, s7_unspecified(mInstance));
    }

    tmscm eval_scheme_file(string name) {
        c_string _file (name);
        s7_pointer r = s7_load_with_environment(mInstance, _file, mUserEnv);
        if (r == nullptr) {
            std::cerr << "Error while loading " << _file << std::endl;
            return tmscm_unspefied();
        }
        return s7_tmscm::mk(mInstance, r);
        // s7_pointer r = s7_load(mInstance, _file);
        // return s7_tmscm::mk(mInstance, s7_eval(mInstance, r, {}));
    }

    tmscm eval_scheme(string s) {
        c_string _s (s);
        s7_pointer result= s7_eval_c_string_with_environment(mInstance, _s, mUserEnv);
        return s7_tmscm::mk(mInstance, result);
    }

    s7_pointer _call_scheme_args(s7_pointer fun, s7_pointer args) {
        return s7_call(mInstance, fun, args);
    }

    s7_pointer _call_scheme_args(s7_pointer fun, std::vector<s7_pointer> args) {

        s7_pointer l = s7_nil(mInstance);
        for (int i = args.size() - 1; i>=0; i--) {
            l = s7_cons(mInstance, args[i], l);
        }
        return s7_call(mInstance, fun, l);
    }

    tmscm call_scheme_args(tmscm fun, std::vector<tmscm> _args) {
        s7_pointer fun_scm = tmscm_cast<s7_tmscm>(fun)->getSCM();
       // assert(s7_is_function(fun_scm) || s7_is_procedure(fun_scm) || s7_is_macro(mInstance, fun_scm));
        std::vector<s7_pointer> args;
        for (auto& arg : _args) {
            args.push_back(tmscm_cast<s7_tmscm>(arg)->getSCM());
        }
        return s7_tmscm::mk(mInstance, _call_scheme_args(fun_scm, args));
    }

    void install_procedure(string name, std::function<tmscm(abstract_scheme*,tmscm)> fun, int numArgs, int numOptional) final;

    string scheme_dialect() {
        return "s7";
    }

    std::function<s7_pointer(s7_scheme*, s7_pointer)> &getFunction(int id) {
        return mProxyFunctionHolder[id];
    }

    int blackboxTag() const {
        return mBlackboxTag;
    }

private:
    s7_scheme *mInstance;
    std::vector<std::function<tmscm(abstract_scheme*,tmscm)>> mFunctionHolder;
    std::vector<std::function<s7_pointer(s7_scheme*, s7_pointer)>> mProxyFunctionHolder;
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

void registerS7();

#endif

