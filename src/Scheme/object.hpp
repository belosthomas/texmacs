#ifndef TEXMACS_SCHEME_OBJECT_HPP
#define TEXMACS_SCHEME_OBJECT_HPP

#include <memory>
#include <vector>

#include "tm_configure.hpp"
#include "blackbox.hpp"
#include "array.hpp"
#include "widget.hpp"
#include "promise.hpp"
#include "tree.hpp"
#include "drd_mode.hpp"
#include "tree_search.hpp"
#include "modification.hpp"
#include "patch.hpp"
#include "tmscm.hpp"

class object {

public:
    object() {

    }

    object(tmscm obj) : handle(obj) {

    }

    template <typename T> object(T obj);

    inline ~object() = default;

    inline int hash() {
        return handle->hash();
    }

    inline bool as_bool () {
        tmscm b= to_tmscm();
        if (!b->is_bool()) return false;
        return b->to_bool();
    }
    
    inline int as_int () {
        tmscm i= to_tmscm();
        if (!i->is_int()) return 0;
        return i->to_int();
    }
    
    inline double as_double () {
        tmscm x= to_tmscm();
        if (!x->is_double()) return 0.0;
        return x->to_double();
    }
    
    inline string as_string () {
        tmscm s= to_tmscm();
        if (!s->is_string()) return "";
        return s->to_string();
    }
    
    inline string as_symbol () {
        tmscm s= to_tmscm();
        if (!s->is_symbol()) return "";
        return s->to_symbol();
    }
    
    inline tree as_tree () {
        tmscm t= to_tmscm();
        if (!t->is_tree()) return tree ();
        return t->to_tree();
    }
    
    inline scheme_tree as_tmscm_tree () {
        tmscm t= to_tmscm();
        return t->to_scheme_tree();
    }
    
    inline list<string> as_list_string () {
        tmscm l= to_tmscm();
        if (!l->is_list_string()) return list<string> ();
        return l->to_list_string();
    }
    
    inline list<tree> as_list_tree () {
        tmscm l= to_tmscm();
        if (!l->is_list_tree()) return list<tree> ();
        return l->to_list_tree();
    }
    
    inline path as_path () {
        tmscm t= to_tmscm();
        if (!t->is_path()) return path ();
        return t->to_path();
    }
    
    inline array<object> as_array_object () {
        TM_ASSERT (handle->is_list(), "list expected");
        object obj = *this;
        array<object> ret;
        while (!obj.is_null()) {
            ret << obj.car();
            obj = obj.cdr();
        }
        return ret;
    }
    
    inline url as_url () {
        tmscm t= to_tmscm();
        if (!t->is_url()) return url ("");
        return t->to_url();
    }
    
    inline array<double> as_array_double () {
        tmscm t= to_tmscm();
        TM_ASSERT (t->is_array_double(), "array<double> expected");
        return t->to_array_double();    
    }
    
    inline modification as_modification () {
        tmscm m= to_tmscm();
        if (!m->is_modification())
            return mod_assign (path (), "");
        return m->to_modification();
    }
    
    inline patch as_patch () {
        tmscm p= to_tmscm();
        if (!p->is_patch())
            return patch (array<patch> ());
        return p->to_patch();
    }
    
    inline widget as_widget () {
        tmscm w= to_tmscm();
        if (!w->is_widget()) return widget ();
        return w->to_widget();
    }
    
    inline tmscm to_tmscm() {
        return handle;
    }

    inline bool is_null () {
        return to_tmscm()->is_null();
    }

    inline bool is_list () {
        return to_tmscm()->is_list();
    }

    inline bool is_bool () {
        return to_tmscm()->is_bool();
    }

    inline bool is_int () {
        return to_tmscm()->is_int();
    }

    inline bool is_double () {
        return to_tmscm()->is_double();
    }

    inline bool is_string () {
        return to_tmscm()->is_string();
    }

    inline bool is_symbol () {
        return to_tmscm()->is_symbol();
    }

    inline bool is_tree () {
        return to_tmscm()->is_tree();
    }

    inline bool is_path () {
        return to_tmscm()->is_path();
    }

    inline bool is_url () {
        return to_tmscm()->is_url();
    }

    inline bool is_array_double () {
        return to_tmscm()->is_array_double();
    }

    inline bool is_widget () {
        return to_tmscm()->is_widget();
    }

    inline bool is_patch () {
        return to_tmscm()->is_patch();
    }

    inline bool is_modification () {
        return to_tmscm()->is_modification();
    }

    inline object cons (object obj2) {
        return {to_tmscm()->cons(obj2.to_tmscm())};
    }

    inline object car () {
        return {to_tmscm()->car()};
    }

    inline object cdr () {
        return {to_tmscm()->cdr()};
    }

    inline object caar () {
        return {to_tmscm()->caar()};
    }

    inline object cdar () {
        return {to_tmscm()->cdar()};
    }

    inline object cadr () {
        return {to_tmscm()->cadr()};
    }

    inline object cddr () {
        return {to_tmscm()->cddr()};
    }

    inline object caddr () {
        return {to_tmscm()->caddr()};
    }

    inline object cadddr () {
        return {to_tmscm()->cadddr()};
    }

    inline bool operator == (object obj) {
        return to_tmscm()->is_equal(obj.to_tmscm());
    }

    inline bool operator != (object obj) {
        return !to_tmscm()->is_equal(obj.to_tmscm());
    }

private:
    tmscm handle;

};

#endif