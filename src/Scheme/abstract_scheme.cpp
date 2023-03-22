/******************************************************************************
* MODULE     : abstract_scheme.cpp
* DESCRIPTION: Abstract interface for the manipulation of scheme objects
* COPYRIGHT  : Liza Belos
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#include "abstract_scheme.hpp"

#include "editor.hpp"
#include "boot.hpp"

#include <iostream>
#include <unordered_map>

std::unique_ptr<texmacs::abstract_scheme> global_scheme;

void texmacs::abstract_scheme::protected_call (object cmd) {
#ifdef USE_EXCEPTIONS
    try {
#endif
        get_current_editor()->before_menu_action ();
        call (cmd);
        get_current_editor()->after_menu_action ();
#ifdef USE_EXCEPTIONS
    }
    catch (string s) {
        get_current_editor()->cancel_menu_action ();
    }
    handle_exceptions ();
#endif
}

void texmacs::abstract_scheme::notify_preferences_booted () {
    preferences_ok= true;
}

void texmacs::abstract_scheme::set_preference (string var, string val) {
    if (!preferences_ok) set_user_preference (var, val);
    else (void) call ("set-preference", object(string_to_tmscm(var)), object(string_to_tmscm(val)));
}

void texmacs::abstract_scheme::notify_preference (string var) {
    if (preferences_ok) (void) call ("notify-preference", object(string_to_tmscm(var)));
}

string texmacs::abstract_scheme::get_preference (string var, string def) {
    //std::cout << "get_preference: " << std::string(var.data(), N(var)) << " " << std::string(def.data(), N(def)) << std::endl;
    if (!preferences_ok)
        return get_user_preference (var, def);
    else {
        string pref= call ("get-preference", object(string_to_tmscm(var))).as_string();
        if (pref == "default") return def; else return pref;
    }
}

std::unordered_map<std::string, texmacs::SchemeFactory*> scheme_factories;

void texmacs::register_scheme_factory(SchemeFactory *factory) {
    scheme_factories[factory->name()] = factory;
}

std::vector<std::string> texmacs::get_scheme_factories() {
    std::vector<std::string> names;
    for (auto &factory : scheme_factories) {
        names.push_back(factory.first);
    }
    return names;
}

texmacs::abstract_scheme *texmacs::make_scheme(std::string name) {
    auto factory = scheme_factories.find(name);
    if (factory == scheme_factories.end()) {
        return nullptr;
    }
    return factory->second->make_scheme();
}

void texmacs::register_all_scheme() {
#ifdef SCHEME_REGI
    SCHEME_REGI
#endif
}
