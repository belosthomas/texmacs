/******************************************************************************
* MODULE     : s7/scheme.cpp
* DESCRIPTION: Abstract interface for the manipulation of scheme objects
* COPYRIGHT  : Liza Belos
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#include <QCoreApplication>

#include "s7_tmscheme.hpp"

std::unordered_map<s7_scheme*, s7_tmscheme*> *s7_scheme_map = nullptr;

void registerS7() {
    register_scheme_factory(new S7Factory);
}

void s7_tmscheme::install_procedure(string name, std::function<tmscm(abstract_scheme*,tmscm)> fun, int numArgs, int numOptional) {

    mFunctionHolder.emplace_back(fun);
    int index = mFunctionHolder.size() - 1;

    mProxyFunctionHolder.emplace_back([this, index](s7_scheme *sc, s7_pointer args) {
        return tmscm_cast<s7_tmscm>(mFunctionHolder[index](this, s7_tmscm::mk(mInstance, args)))->getSCM();
    });

    c_string _name(name);
    s7_pointer ptr = s7_define_safe_function(mInstance, _name, mProxyFunctionHolder.back(), numArgs, numOptional, false, "Documentation is not available");

}

abstract_scheme *S7Factory::make_scheme() {
    s7_tmscheme *scheme = new s7_tmscheme();
    if (s7_scheme_map == nullptr) {
        s7_scheme_map = new std::unordered_map<s7_scheme*, s7_tmscheme*>();
    }
    (*s7_scheme_map)[scheme->mInstance] = scheme;
    return scheme;
}

bool s7_tmscm::is_blackbox() {
    return s7_is_c_object(mSCM) && s7_c_object_type(mSCM) == s7_scheme_map->at(mScheme)->blackboxTag();
}

s7_pointer s7_blackbox_to_string (s7_scheme *sc, s7_pointer args)
{
    auto scheme = s7_scheme_map->at(sc);
    tmscm tmscm_args = s7_tmscm::mk(sc, args);

    tmscm blackbox_smob = tmscm_args->car();

    string s = "<blackbox>";
    int type_ = type_box (blackbox_smob->to_blackbox()) ;
    if (type_ == type_helper<tree>::id) {
        tree t = blackbox_smob->to_tree();
        s= "<tree " * tree_to_texmacs (t) * ">";
    }
    else if (type_ == type_helper<observer>::id) {
        s= "<observer>";
    }
    else if (type_ == type_helper<widget>::id) {
        s= "<widget>";
    }
    else if (type_ == type_helper<promise<widget> >::id) {
        s= "<promise-widget>";
    }
    else if (type_ == type_helper<command>::id) {
        s= "<command>";
    }
    else if (type_ == type_helper<url>::id) {
        url u = tmscm_args->to_url();
        s= "<url " * as_string (u) * ">";
    }
    else if (type_ == type_helper<modification>::id) {
        s= "<modification>";
    }
    else if (type_ == type_helper<patch>::id) {
        s= "<patch>";
    }

    return tmscm_cast<s7_tmscm>(scheme->string_to_tmscm(s))->getSCM();
}

s7_pointer s7_free_blackbox (s7_scheme *sc, s7_pointer obj)
{
    blackbox *ptr = (blackbox *) s7_c_object_value (obj);
    tm_delete (ptr);

    // Don't remove this, segmentation error could happen :)
    return (NULL);
}

s7_pointer s7_mark_blackbox (s7_scheme *sc, s7_pointer obj)
{
    return (NULL);
}

s7_pointer s7_blackbox_is_equal(s7_scheme *sc, s7_pointer args)
{
    auto scheme = s7_scheme_map->at(sc);

    s7_pointer p1, p2;
    p1 = s7_car (args);
    p2 = s7_cadr (args);
    if (p1 == p2)
        return s7_t (sc);
    if ((!s7_is_c_object (p2)) ||
        (s7_c_object_type (p2) != scheme->blackboxTag()))
        return s7_f (sc);

    return tmscm_cast<s7_tmscm>(scheme->bool_to_tmscm(s7_tmscm::mk(sc, p1)->to_blackbox() == s7_tmscm::mk(sc, p2)->to_blackbox()))->getSCM();
}

s7_pointer g_current_time (s7_scheme *sc, s7_pointer args)
{
    s7_int res;

#ifdef HAVE_GETTIMEOFDAY
    struct timeval tp;
    gettimeofday (&tp, NULL);
    res = tp.tv_sec;
#else
    timeb tb;
  ftime (&tb);
  res = tb.time;
#endif

    return s7_make_integer(sc, res);
}

s7_pointer g_getpid (s7_scheme *sc, s7_pointer args)
{
    return(s7_make_integer(sc, (s7_int)QCoreApplication::applicationPid()));
}

s7_tmscheme::s7_tmscheme() {
    mInstance = s7_init();
    mBlackboxTag = s7_make_c_type (mInstance, "blackbox");
    s7_c_type_set_gc_free (mInstance, mBlackboxTag, s7_free_blackbox);
    s7_c_type_set_gc_mark (mInstance, mBlackboxTag, s7_mark_blackbox);
    s7_c_type_set_is_equal (mInstance, mBlackboxTag, s7_blackbox_is_equal);
    s7_c_type_set_to_string (mInstance, mBlackboxTag, s7_blackbox_to_string);

    mUserEnv = s7_inlet (mInstance, s7_nil (mInstance));
    //s7_gc_on (mInstance, false);
    s7_gc_protect (mInstance, mUserEnv);

    s7_define(mInstance, mUserEnv, s7_make_symbol (mInstance, "current-time"),
              s7_make_typed_function (mInstance, "current-time", g_current_time, 0, 0,
                                      false, "current-time", NULL));

    s7_define(mInstance, mUserEnv, s7_make_symbol(mInstance, "getpid"),
              s7_make_typed_function(mInstance, "getpid", g_getpid, 0, 0,
                                     false, "int getpid(void)",
                                     s7_make_signature2(mInstance, 2, s7_make_symbol(mInstance, "integer?"), s7_t(mInstance))));

    install_procedure("get-user-login", [](abstract_scheme *sc, tmscm args) {
        return sc->string_to_tmscm("test");
    }, 0, 0);

    install_procedure("get-user-name", [](abstract_scheme *sc, tmscm args) {
        return sc->string_to_tmscm("test");
    }, 0, 0);


}