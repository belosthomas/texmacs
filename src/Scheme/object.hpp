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

    virtual ~object() = default;

    virtual int hash() {
        return handle->hash();
    }

    virtual bool as_bool () {
        tmscm b= to_tmscm();
        if (!b->is_bool()) return false;
        return b->to_bool();
    }
    
    virtual int as_int () {
        tmscm i= to_tmscm();
        if (!i->is_int()) return 0;
        return i->to_int();
    }
    
    virtual double as_double () {
        tmscm x= to_tmscm();
        if (!x->is_double()) return 0.0;
        return x->to_double();
    }
    
    virtual string as_string () {
        tmscm s= to_tmscm();
        if (!s->is_string()) return "";
        return s->to_string();
    }
    
    virtual string as_symbol () {
        tmscm s= to_tmscm();
        if (!s->is_symbol()) return "";
        return s->to_symbol();
    }
    
    virtual tree as_tree () {
        tmscm t= to_tmscm();
        if (!t->is_tree()) return tree ();
        return t->to_tree();
    }
    
    virtual scheme_tree as_tmscm_tree () {
        tmscm t= to_tmscm();
        return t->to_scheme_tree();
    }
    
    virtual list<string> as_list_string () {
        tmscm l= to_tmscm();
        if (!l->is_list_string()) return list<string> ();
        return l->to_list_string();
    }
    
    virtual list<tree> as_list_tree () {
        tmscm l= to_tmscm();
        if (!l->is_list_tree()) return list<tree> ();
        return l->to_list_tree();
    }
    
    virtual path as_path () {
        tmscm t= to_tmscm();
        if (!t->is_path()) return path ();
        return t->to_path();
    }
    
    virtual array<object> as_array_object () {
        TM_ASSERT (handle->is_list(), "list expected");
        object obj = *this;
        array<object> ret;
        while (!obj.is_null()) {
            ret << obj.car();
            obj = obj.cdr();
        }
        return ret;
    }
    
    virtual url as_url () {
        tmscm t= to_tmscm();
        if (!t->is_url()) return url ("");
        return t->to_url();
    }
    
    virtual array<double> as_array_double () {
        tmscm t= to_tmscm();
        TM_ASSERT (t->is_array_double(), "array<double> expected");
        return t->to_array_double();    
    }
    
    virtual modification as_modification () {
        tmscm m= to_tmscm();
        if (!m->is_modification())
            return mod_assign (path (), "");
        return m->to_modification();
    }
    
    virtual patch as_patch () {
        tmscm p= to_tmscm();
        if (!p->is_patch())
            return patch (array<patch> ());
        return p->to_patch();
    }
    
    virtual widget as_widget () {
        tmscm w= to_tmscm();
        if (!w->is_widget()) return widget ();
        return w->to_widget();
    }
    
    virtual tmscm to_tmscm() {
        return handle;
    }

    virtual bool is_null () { return to_tmscm()->is_null(); }
    virtual bool is_list () { return to_tmscm()->is_list(); }
    virtual bool is_bool () { return to_tmscm()->is_bool(); }
    virtual bool is_int () { return to_tmscm()->is_int(); }
    virtual bool is_double () { return to_tmscm()->is_double(); }
    virtual bool is_string () { return to_tmscm()->is_string(); }
    virtual bool is_symbol () { return to_tmscm()->is_symbol(); }
    virtual bool is_tree () { return to_tmscm()->is_tree(); }
    virtual bool is_path () { return to_tmscm()->is_path(); }
    virtual bool is_url () { return to_tmscm()->is_url(); }
    virtual bool is_array_double () { return to_tmscm()->is_array_double(); }
    virtual bool is_widget () { return to_tmscm()->is_widget(); }
    virtual bool is_patch () { return to_tmscm()->is_patch(); }
    virtual bool is_modification () { return to_tmscm()->is_modification(); }


    object cons (object obj2) {
        return {to_tmscm()->cons(obj2.to_tmscm())};
    }

    object car () {
        return {to_tmscm()->car()};
    }
    
    object cdr () {
        return {to_tmscm()->cdr()};
    }
    
    object caar () {
        return {to_tmscm()->caar()};
    }
    
    object cdar () {
        return {to_tmscm()->cdar()};
    }
    
    object cadr () {
        return {to_tmscm()->cadr()};
    }
    
    object cddr () {
        return {to_tmscm()->cddr()};
    }
    
    object caddr () {
        return {to_tmscm()->caddr()};
    }
    
    object cadddr () {
        return {to_tmscm()->cadddr()};
    }

    bool operator == (object obj) {
        return to_tmscm()->is_equal(obj.to_tmscm());
    }

    bool operator != (object obj) {
        return !to_tmscm()->is_equal(obj.to_tmscm());
    }

private:
    tmscm handle;

};

#endif