/******************************************************************************
* MODULE     : abstract_scheme.hpp
* DESCRIPTION: Abstract interface for the manipulation of scheme objects
* COPYRIGHT  : Liza Belos
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#ifndef ABSTRACT_SCHEME_HPP
#define ABSTRACT_SCHEME_HPP

#include "scheme_config.hpp"

#include <memory>
#include <vector>
#include <string>
#include <functional>

#include "tmscm.hpp"
#include "object.hpp"

#include "iterator.hpp"
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
#include "new_view.hpp"


namespace texmacs {

    class abstract_scheme {

    public:
        virtual ~abstract_scheme() = default;

        virtual tmscm blackbox_to_tmscm(blackbox b) = 0;

        /// TSCM Methods that HAVE to be implemented from the derived classes //////////////////////////////////////////////
        virtual tmscm tmscm_null() = 0;

        virtual tmscm tmscm_true() = 0;

        virtual tmscm tmscm_false() = 0;

        virtual tmscm bool_to_tmscm(bool b) = 0;

        virtual tmscm int_to_tmscm(int i) = 0;

        virtual tmscm long_to_tmscm(long l) = 0;

        virtual tmscm double_to_tmscm(double i) = 0;

        virtual tmscm string_to_tmscm(string s) = 0;

        virtual tmscm symbol_to_tmscm(string s) = 0;

        virtual tmscm tmscm_unspefied() = 0;


        /// Eval Scheme Method that HAVE to be implemented from the derived classes ////////////////////////////////////////

        virtual tmscm eval_scheme_file(string name) = 0;

        virtual tmscm eval_scheme(string s) = 0;

        virtual tmscm call_scheme_args(tmscm fun, std::vector<tmscm> args) = 0;

        virtual void install_procedure(string name, std::function<tmscm(abstract_scheme *, tmscm)> fun, int numArgs, int numOptional) = 0;

        virtual string scheme_dialect() = 0;

        /// Eval Scheme Method that doesn't have to be implemented from the derived classes ////////////////////////////////


        template<typename... Args>
        tmscm call_scheme(tmscm fun, Args... args) {
            std::vector<tmscm> args_vec = {args...};
            return call_scheme_args(fun, args_vec);
        }


        virtual object tree_to_stree(scheme_tree t) {
            return call("tree->stree", object(scheme_tree_to_tmscm(t)));
        }

        virtual tree stree_to_tree(object obj) {
            return call("stree->tree", obj).as_tree();
        }

        tree content_to_tree(object obj) {
            return obj.to_tmscm()->to_content();
        }

        object scheme_cmd(const char *s) {
            return eval("(lambda () " * string(s) * ")");
        }

        object scheme_cmd(string s) {
            return eval("(lambda () " * s * ")");
        }

        object scheme_cmd(object cmd) {
            cmd = cmd.cons(null_object());
            cmd = null_object().cons(cmd);
            cmd = eval("'lambda").cons(cmd);
            return eval(cmd);
        }

        /******************************************************************************
        * Conversions to functional objects
        ******************************************************************************/

        inline array<tmscm> array_lookup(array<object> a) {
            const int n = N(a);
            array<tmscm> tmscm(n);
            int i;
            for (i = 0; i < n; i++) tmscm[i] = a[i].to_tmscm();
            return tmscm;
        }


        /******************************************************************************
        * Evaluation and function calls
        ******************************************************************************/

        object eval(const char *expr) {
            return {eval_scheme(expr)};
        }

        object eval(string expr) {
            return {eval_scheme(expr)};
        }

        object eval(object expr) {
            return call("eval", expr);
        }

        object eval_secure(string expr) {
            return eval("(wrap-eval-secure " * expr * ")");
        }

        object eval_file(string name) {
            return {eval_scheme_file(name)};
        }

        bool exec_file(url u) {
            object ret = eval_file(materialize(u));
            return ret != unspecified_object();
        }

        template<typename... Args>
        inline object call(const char *fun) {
            std::vector<tmscm> args_vec;
            return {call_scheme(eval_scheme(fun), args_vec)};
        }

        template<typename... Args>
        inline object call(string fun) {
            std::vector<tmscm> args_vec;
            return {call_scheme(eval_scheme(fun), args_vec)};
        }

        template<typename... Args>
        inline object call(object fun) {
            std::vector<tmscm> args_vec;
            return {call_scheme(fun.to_tmscm(), args_vec)};
        }

        template<typename... Args>
        inline object call(const char *fun, Args... args) {
            std::vector<tmscm> args_vec;
            for (auto arg: {args...}) {
                args_vec.push_back(arg.to_tmscm());
            }
            return {call_scheme(eval_scheme(fun), args_vec)};
        }

        template<typename... Args>
        inline object call(string fun, Args... args) {
            std::vector<tmscm> args_vec;
            for (auto arg: {args...}) {
                args_vec.push_back(arg.to_tmscm());
            }
            return {call_scheme(eval_scheme(fun), args_vec)};
        }

        template<typename... Args>
        inline object call(object fun, Args... args) {
            std::vector<tmscm> args_vec;
            for (auto arg: {args...}) {
                args_vec.push_back(object_to_tmscm(arg));
            }
            return {call_scheme(fun.to_tmscm(), args_vec)};
        }

        /******************************************************************************
        * User preferences
        ******************************************************************************/


        void notify_preferences_booted();

        void set_preference(string var, string val);

        void notify_preference(string var);

        string get_preference(string var, string def = "default");

        /******************************************************************************
        * Delayed evaluation
        ******************************************************************************/

        void
        exec_delayed(object cmd) {
            delayed_queue << cmd;
            start_queue << (((time_t) texmacs_time()) - 1000000000);
        }

        void
        exec_delayed_pause(object cmd) {
            delayed_queue << cmd;
            start_queue << ((time_t) texmacs_time());
        }

        void
        exec_pending_commands() {
            array<object> a = delayed_queue;
            array<time_t> b = start_queue;
            delayed_queue = array<object>(0);
            start_queue = array<time_t>(0);
            int i, n = N(a);
            for (i = 0; i < n; i++) {
                time_t now = (time_t) texmacs_time();
                if ((now - b[i]) >= 0) {
                    object obj = call(a[i]);
                    if (obj.is_int() && (now - b[i] < 1000000000)) {
                        //cout << "pause= " << obj << "\n";
                        delayed_queue << a[i];
                        start_queue << (now + obj.as_int());
                    }
                } else {
                    delayed_queue << a[i];
                    start_queue << b[i];
                }
            }
        }

        void
        clear_pending_commands() {
            delayed_queue = array<object>(0);
            start_queue = array<time_t>(0);
        }


        /******************************************************************************
        * Protected evaluation
        ******************************************************************************/

        void protected_call(object cmd);

        virtual object null_object() {
            return {tmscm_null()};
        }

        virtual object unspecified_object() {
            return {tmscm_unspefied()};
        }

        virtual object symbol_object(string s) {
            return {symbol_to_tmscm(s)};
        }

        virtual object string_to_object(string s) {
            return call("string->object", object(string_to_tmscm(s)));
        }

        virtual string object_to_string(object obj) {
            return call("object->string", obj).as_string();
        }

        /// These TMSCM Conversion doesn't have to be redefined in the derived classes.

        virtual inline tmscm tree_to_tmscm(tree o) {
            return blackbox_to_tmscm(close_box<tree>(o));
        }

        virtual tmscm list_string_to_tmscm(list<string> l) {
            if (is_nil(l)) return tmscm_null();
            return string_to_tmscm(l->item)->cons(list_string_to_tmscm(l->next));
        }

        virtual tmscm list_tree_to_tmscm(list<tree> l) {
            if (is_nil(l)) return tmscm_null();
            return tree_to_tmscm(l->item)->cons(list_tree_to_tmscm(l->next));
        }

        virtual tmscm path_to_tmscm(path p) {
            if (is_nil(p)) return tmscm_null();
            else return int_to_tmscm(p->item)->cons(path_to_tmscm(p->next));

        }

        virtual tmscm url_to_tmscm(url u) {
            return blackbox_to_tmscm(close_box<url>(u));
        }

        virtual tmscm array_double_to_tmscm(array<double> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = double_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm modification_to_tmscm(modification m) {
            return blackbox_to_tmscm(close_box<modification>(m));
        }

        virtual tmscm patch_to_tmscm(patch p) {
            return blackbox_to_tmscm(close_box<patch>(p));
        }

        virtual tmscm scheme_tree_to_tmscm(scheme_tree t) {
            if (is_atomic(t)) {
                string s = t->label;
                if (s == "#t") return tmscm_true();
                if (s == "#f") return tmscm_false();
                if (is_int(s)) return int_to_tmscm(as_int(s));
                if (is_quoted(s))
                    return string_to_tmscm(scm_unquote(s));
                //if ((N(s)>=2) && (s[0]=='\42') && (s[N(s)-1]=='\42'))
                //return string_to_tmscm (s (1, N(s)-1));
                if (N(s) >= 1 && s[0] == '\'') return symbol_to_tmscm(s(1, N(s)));
                return symbol_to_tmscm(s);
            } else {
                int i;
                tmscm p = tmscm_null();
                for (i = N(t) - 1; i >= 0; i--) {
                    p = scheme_tree_to_tmscm(t[i])->cons(p);
                }
                return p;
            }
        }

        virtual inline tmscm tree_label_to_tmscm(tree_label l) {
            string s = as_string(l);
            return symbol_to_tmscm(s);
        }

        virtual tmscm observer_to_tmscm(observer o) {
            return blackbox_to_tmscm(close_box<observer>(o));
        }

        virtual tmscm widget_to_tmscm(widget o) {
            return blackbox_to_tmscm(close_box<widget>(o));
        }

        virtual tmscm command_to_tmscm(command o) {
            return blackbox_to_tmscm(close_box<command>(o));
        }

        virtual tmscm table_string_string_to_tmscm(hashmap<string, string> t) {
            tmscm p = tmscm_null();
            iterator<string> it = iterate(t);
            while (it->busy()) {
                string s = it->next();
                tmscm n = string_to_tmscm(s)->cons(string_to_tmscm(t[s]));
                p = n->cons(p);
            }
            return p;
        }

        virtual tmscm array_int_to_tmscm(array<int> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = int_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm array_array_double_to_tmscm(array<array_double> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = array_double_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm array_array_array_double_to_tmscm(array<array_array_double> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = array_array_double_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm array_string_to_tmscm(array<string> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = string_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm array_tree_to_tmscm(array<tree> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = tree_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm array_widget_to_tmscm(array<widget> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = widget_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm array_url_to_tmscm(array<url> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = url_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm array_patch_to_tmscm(array<patch> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = patch_to_tmscm(a[i])->cons(p);
            return p;
        }

        virtual tmscm array_path_to_tmscm(array<path> a) {
            int i, n = N(a);
            tmscm p = tmscm_null();
            for (i = n - 1; i >= 0; i--) p = path_to_tmscm(a[i])->cons(p);
            return p;
        }


    private:
        bool preferences_ok = false;
        static array<object> delayed_queue;
        static array<time_t> start_queue;

    };


    class SchemeFactory {

    public:
        virtual ~SchemeFactory() {};

        virtual abstract_scheme *make_scheme() = 0;

        virtual std::string name() = 0;

    };

    void register_scheme_factory(SchemeFactory *factory);

    std::vector<std::string> get_scheme_factories();

    abstract_scheme *make_scheme(std::string name);

    void register_all_scheme();

#ifdef SCHEME_DECL
    SCHEME_DECL
#endif

}

#endif