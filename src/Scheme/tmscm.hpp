/******************************************************************************
* MODULE     : tmscm.hpp
* DESCRIPTION: Abstract interface for the manipulation of scheme objects
* COPYRIGHT  : Liza Belos
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/


#ifndef SCHEME_TMSCM_HPP
#define SCHEME_TMSCM_HPP

#include <memory>
#include <cassert>

#include "convert.hpp"
#include "widget.hpp"
#include "modification.hpp"
#include "patch.hpp"

#define TMSCM_ARG1 1
#define TMSCM_ARG2 2
#define TMSCM_ARG3 3
#define TMSCM_ARG4 4
#define TMSCM_ARG5 5
#define TMSCM_ARG6 6
#define TMSCM_ARG7 7
#define TMSCM_ARG8 8
#define TMSCM_ARG9 9
#define TMSCM_ARG10 10

class patch;


typedef array<int> array_int;
typedef array<string> array_string;
typedef array<tree> array_tree;
typedef array<url> array_url;
typedef array<patch> array_patch;
typedef array<path> array_path;
typedef array<widget> array_widget;
typedef array<double> array_double;
typedef array<array<double> > array_array_double;
typedef array<array<array<double> > > array_array_array_double;


class abstract_tmscm;
using tmscm = std::shared_ptr<abstract_tmscm>;

class abstract_tmscm {

public:
    virtual ~abstract_tmscm() = default;

    virtual int hash() = 0;

    virtual tmscm null() = 0;

    virtual void set_car(tmscm b) = 0;

    virtual void set_cdr(tmscm b) = 0;

    virtual bool is_equal(tmscm o2) = 0;

    virtual bool is_null() = 0;

    virtual bool is_pair() = 0;

    virtual bool is_list() = 0;

    virtual bool is_bool() = 0;

    virtual bool is_int() = 0;

    virtual bool is_double() = 0;

    virtual bool is_string() = 0;

    virtual bool is_symbol() = 0;

    virtual tmscm cons(tmscm obj2) = 0;

    virtual tmscm car() = 0;

    virtual tmscm cdr() = 0;

    virtual tmscm caar() = 0;

    virtual tmscm cadr() = 0;

    virtual tmscm cdar() = 0;

    virtual tmscm cddr() = 0;

    virtual tmscm caddr() = 0;

    virtual tmscm cadddr() = 0;

    virtual bool is_blackbox() = 0;

    virtual blackbox to_blackbox() = 0;

    virtual bool to_bool() = 0;

    virtual int to_int() = 0;

    virtual double to_double() = 0;

    virtual string to_string() = 0;

    virtual string to_symbol() = 0;

    virtual bool is_widget () {
        return (is_blackbox() &&
                (type_box (to_blackbox()) == type_helper<widget>::id));
    }

    virtual widget to_widget () {
        return open_box<widget> (to_blackbox());
    }

    virtual bool is_list_string () {
        if (is_null()) return true;
        else return is_pair() && car()->is_string() && cdr()->is_list_string();
    }

    virtual bool is_list_tree () {
        if (is_null()) return true;
        else return is_pair() && car()->is_tree() && cdr()->is_list_tree();
    }

    virtual bool is_path () {
        if (is_null()) return true;
        else return car()->is_int() && cdr()->is_path();
    }

    virtual bool is_url () {
        return (is_blackbox() && (type_box (to_blackbox()) == type_helper<url>::id)) || (is_string());
    }

    virtual bool is_array_double () {
        if (is_null()) return true;
        else return is_pair() && car()->is_double() && cdr()->is_array_double();
    }

    virtual bool is_modification () {
        return (is_blackbox() && (type_box (to_blackbox()) == type_helper<modification>::id)) || (is_string());
    }

    virtual bool is_patch () {
        return (is_blackbox() && (type_box (to_blackbox()) == type_helper<patch>::id)) || (is_string());
    }



    virtual tree to_content () {
        if (is_string()) return to_string();
        if (is_tree()) return to_tree();
        if (is_pair()) {
            if (!car()->is_symbol()) return "?";
            tree t (make_tree_label (car()->to_symbol()));
            tmscm p = cdr();
            while (!p->is_null()) {
                t << p->car()->to_content();
                p= p->cdr();
            }
            return t;
        }
        return "?";
    }

    virtual list<string> to_list_string () {
        if (is_null()) {
            return list<string>();
        }
        if (is_pair()) {
            return list<string>(car()->to_string(), cdr()->to_string());
        }
        assert(is_list());
        return list<string>(car()->to_string(), cdr()->to_list_string());
    }

    virtual list<tree> to_list_tree () {
        if (is_null()) return list<tree>();
        return list<tree>(car()->to_tree(), cdr()->to_list_tree());
    }

    virtual path to_path () {
        if (is_null()) return path ();
        else return path (car()->to_int(), cdr()->to_path());
    }

    virtual url to_url () {
        if (is_string())
#ifdef OS_MINGW
            return url_system (to_string());
#else
            return to_string();
#endif
        return open_box<url>(to_blackbox());
    }

    virtual array<double> to_array_double () {
        array<double> a;
        if (is_null()) return a;
        a << car()->to_double();
        auto p = cdr();
        while (!p->is_null()) {
            a << ((double)p->car()->to_double());
            p= p->cdr();
        }
        return a;
    }

    virtual modification to_modification () {
        return open_box<modification> (to_blackbox());
    }

    virtual patch to_patch () {
        return open_box<patch> (to_blackbox());
    }

    virtual scheme_tree to_scheme_tree () {
        if (is_list()) {
            tree t (TUPLE);
            if (is_null()) return t;
            t << car()->to_scheme_tree();
            tmscm p = cdr();
            while (!p->is_null()) {
                t << p->car()->to_scheme_tree();
                p= p->cdr();
            }
            return t;
        }
        if (is_symbol()) return to_symbol();
        if (is_string()) return scm_quote (to_string());
        //if (is_string()) return "\"" * to_string() * "\"";
        if (is_int()) return as_string ((int) to_int());
        if (is_bool()) return (to_bool()? string ("#t"): string ("#f"));
        if (is_tree()) return tree_to_scheme_tree (to_tree());
        return "?";
    }

    virtual inline tree_label to_tree_label () {
        string s = to_symbol();
        return make_tree_label(s);
    }

    virtual inline bool is_tree() {
        return (is_blackbox() && (type_box(to_blackbox()) == type_helper<tree>::id));
    }

    virtual inline tree to_tree () {
        return open_box<tree>(to_blackbox());
    }

    virtual inline bool is_content() {
        if (is_string() || is_tree()) return true;
        else if (!is_pair() || !car()->is_symbol()) return false;

        for (tmscm p= cdr(); !p->is_null(); p = p->cdr()) {
            if (!p->car()->is_content()) return false;
        }
        return true;
    }

    virtual inline bool is_observer () {
        return (is_blackbox() &&
                (type_box (to_blackbox()) == type_helper<observer>::id));
    }

    virtual inline observer to_observer () {
        return open_box<observer>(to_blackbox());
    }

    virtual inline bool is_command () {
        return (is_blackbox() &&
                (type_box (to_blackbox()) == type_helper<command>::id));
    }

    virtual inline command to_command () {
        return open_box<command> (to_blackbox());
    }

    virtual inline bool is_table_string_string () {
        if (is_null()) return true;
        else if (!is_pair()) return false;
        else {
            tmscm f= car();
            return is_pair() && car()->is_string() && cdr()->is_string() && cdr()->is_table_string_string();
        }
    }

    virtual inline hashmap<string,string> to_table_string_string () {
        hashmap<string,string> t;
        if (is_null()) return t;
        t(car()->to_string()) = cdr()->to_string();
        tmscm p= cdr();
        while (!p->is_null()) {
            tmscm n = p->car();
            t(p->car()->to_string()) = p->cdr()->to_string();
            p = p->cdr();
        }
        return t;
    }

    virtual inline bool is_array_int () {
        if (is_null()) return true;
        else return is_pair() && car()->is_int() && cdr()->is_array_int();
    }

    virtual inline array<int> to_array_int () {
        array<int> a;
        if (is_null()) return a;
        a << (int)car()->to_int();
        tmscm p = cdr();
        while (!p->is_null()) {
            a << (int)p->car()->to_int();
            p = p->cdr();
        }
        return a;
    }

    virtual inline bool is_array_string() {
        if (is_null()) return true;
        else return is_pair() && car()->is_string() && cdr()->is_array_string();
    }

    virtual inline bool is_array_array_double () {
        if (is_null()) return true;
        else return is_pair() && car()->is_array_double() && cdr()->is_array_array_double();
    }

    virtual inline array<array_double> to_array_array_double () {
        array<array_double> a;
        if (is_null()) return a;
        a << car()->to_array_double();
        tmscm p = cdr();
        while (!is_null()) {
            a << p->car()->to_array_double();
            p = p->cdr();
        }
        return a;
    }

    virtual inline bool is_array_array_array_double () {
        if (is_null()) return true;
        else return is_pair() && car()->is_array_array_double() && cdr()->is_array_array_array_double();
    }

    virtual inline array<array_array_double> to_array_array_array_double () {
        array<array_array_double> a;
        if (is_null()) return a;
        a << car()->to_array_array_double();
        tmscm p = cdr();
        while (!p->is_null()) {
            a << p->car()->to_array_array_double();
            p = p->cdr();
        }
        return a;
    }

    virtual inline array<string> to_array_string () {
        array<string> a;
        if (is_null()) return a;
        a << car()->to_string();
        tmscm p = cdr();
        while (!p->is_null()) {
            a << p->car()->to_string();
            p = p->cdr();
        }
        return a;
    }

    virtual inline bool is_array_tree () {
        if (is_null()) return true;
        else return is_pair() && car()->is_tree() && cdr()->is_array_tree();
    }

    virtual inline array<tree> to_array_tree () {
        array<tree> a;
        if (is_null()) return a;
        a << car()->to_tree();
        tmscm p = cdr();
        while (!p->is_null()) {
            a << p->car()->to_tree();
            p = p->cdr();
        }
        return a;
    }

    virtual inline bool is_array_widget () {
        if (is_null()) return true;
        else return is_pair() && car()->is_widget() && cdr()->is_array_widget();
    }

    virtual inline array<widget> to_array_widget () {
        array<widget> a;
        if (is_null()) return a;
        a << car()->to_widget();
        tmscm p = cdr();
        while (!p->is_null()) {
            a << p->car()->to_widget();
            p = p->cdr();
        }
        return a;
    }

    virtual inline bool is_array_url () {
        if (is_null()) return true;
        else return is_pair() && car()->is_url() && cdr()->is_array_url();
    }

    virtual inline array<url> to_array_url () {
        array<url> a;
        if (is_null()) return a;
        a << car()->to_url();
        tmscm p = cdr();
        while (!p->is_null()) {
            a << p->car()->to_url();
            p = p->cdr();
        }
        return a;
    }

    virtual inline bool is_array_patch () {
        if (is_null()) return true;
        else return is_pair() && car()->is_patch() && cdr()->is_array_patch();
    }

    virtual inline array<patch> to_array_patch () {
        array<patch> a;
        if (is_null()) return a;
        a << car()->to_patch();
        tmscm p = cdr();
        while (!p->is_null()) {
            a << p->car()->to_patch();
            p = p->cdr();
        }
        return a;
    }

    virtual inline bool is_array_path () {
        if (is_null()) return true;
        else return is_pair() && car()->is_path() && cdr()->is_array_path();
    }

    virtual inline array<path> to_array_path () {
        array<path> a;
        if (is_null()) return a;
        a << car()->to_path();
        tmscm p = cdr();
        while (!p->is_null()) {
            a << p->car()->to_path();
            p = p->cdr();
        }
        return a;
    }

};


template <typename T, typename ...Args> inline tmscm make_tmscm(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T> inline std::shared_ptr<T> tmscm_cast (tmscm p) {
    return std::dynamic_pointer_cast<T>(p);
}

#endif