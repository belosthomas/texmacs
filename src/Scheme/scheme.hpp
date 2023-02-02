
/******************************************************************************
* MODULE     : scheme.hpp
* DESCRIPTION: Abstract interface for the manipulation of scheme objects
* COPYRIGHT  : Rewritten by Liza Belos / (C) 1999  Joris van der Hoeven
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#ifndef SCHEME_HH
#define SCHEME_HH

#include "abstract_scheme.hpp"

void initialize_scheme ();

#define SINGLE_SCHEME_INSTANCE 1
#if SINGLE_SCHEME_INSTANCE

extern std::unique_ptr<abstract_scheme> global_scheme;

inline abstract_scheme &scheme() {
    return *global_scheme;
}

inline void use_scheme(std::string scheme_name) {
    global_scheme = std::unique_ptr<abstract_scheme>(make_scheme(scheme_name));
}

inline tm_ostream& operator << (tm_ostream& out, object obj) {
    out.flush ();
    if (out == cout) scheme().call ("write", obj);
    else if (out == cerr) scheme().call ("write-err", obj);
    else FAILED ("not yet implemented");
    scheme().call ("force-output");
    return out;
}

/// OBJECT FUNCTIONS
inline object object_from (object o) {
    return o;
}

inline object object_from (bool b) {
    return {scheme().bool_to_tmscm(b)};
}

inline object object_from (int i) {
    return {scheme().int_to_tmscm(i)};
}

inline object object_from (double x) {
    return {scheme().double_to_tmscm(x)};
}

inline object object_from (const char* s) {
    return {scheme().string_to_tmscm(s)};
}

inline object object_from (string s) {
    return {scheme().string_to_tmscm(s)};
}

inline object object_from (tree t) {
    return {scheme().tree_to_tmscm(t)};
}

inline object object_from (list<string> l) {
    return {scheme().list_string_to_tmscm(l)};
}

inline object object_from (list<tree> l) {
    return {scheme().list_tree_to_tmscm(l)};
}

inline object object_from (path p) {
    return {scheme().path_to_tmscm(p)};
}

inline object object_from (url u) {
    return {scheme().url_to_tmscm(u)};
}

inline object object_from (array<double> a) {
    return {scheme().array_double_to_tmscm(a)};
}

inline object object_from (modification m) {
    return {scheme().modification_to_tmscm(m)};
}

inline object object_from (patch p) {
    return {scheme().patch_to_tmscm(p)};
}

inline object object_from(array<object> objlist) {
    object obj = objlist[N(objlist) - 1];
    for (int i = N(objlist) - 2; i >= 0; i--) {
        obj = objlist[i].cons(obj);
    }
    return obj;
}

inline object object_from(std::vector<object> objlist) {
    object obj = scheme().null_object();
    for (ssize_t i = objlist.size() - 1; i >= 0; i--) {
        obj = objlist[i].cons(obj);
    }
    return obj;
}

inline void objects_from (std::vector<object> &obj) {

}

template <typename T, typename... Args> inline void objects_from (std::vector<object> &obj, T t, Args... args) {
    obj.push_back(object_from(t));
    objects_from(obj, args...);
}

inline object null_object () {
    return {scheme().tmscm_null()};
}

template <typename T> inline object list_object (T obj) {
    return object_from(obj).cons(null_object());
}

template <typename T, typename ...Args> inline object list_object (T obj, Args... args) {
    return object_from(obj).cons(list_object(std::forward<Args>(args)...));
}

template <typename T> object::object(T obj) {
    *this = object_from(obj);
}

inline object symbol_object (string s) {
    return {scheme().symbol_to_tmscm(s)};
}

inline object cons (object obj1, object obj2) {
    return obj1.cons(obj2);
}

inline object car (object obj) {
    return obj.car();
}

inline object cdr (object obj)  {
    return obj.cdr();
}

inline object caar (object obj)  {
    return obj.caar();
}

inline object cdar (object obj)  {
    return obj.cdar();
}

inline object cadr (object obj) {
    return obj.cadr();
}

inline object cddr (object obj) {
    return obj.cddr();
}

inline object caddr (object obj) {
    return obj.caddr();
}

inline object cadddr (object obj)  {
    return obj.cadddr();
}

inline bool is_null (object obj)  {
    return obj.is_null();
}

inline bool is_list (object obj)  {
    return obj.is_list();
}

inline bool is_bool (object obj)  {
    return obj.is_bool();
}

inline bool is_int (object obj)  {
    return obj.is_int();
}

inline bool is_double (object obj)  {
    return obj.is_double();
}

inline bool is_string (object obj)  {
    return obj.is_string();
}

inline bool is_symbol (object obj)  {
    return obj.is_symbol();
}

inline bool is_tree (object obj)  {
    return obj.is_tree();
}

inline bool is_path (object obj)  {
    return obj.is_path();
}

inline bool is_url (object obj)  {
    return obj.is_url();
}

inline bool is_array_double (object obj)  {
    return obj.is_array_double();
}

inline bool is_modification (object obj)  {
    return obj.is_modification();
}

inline bool is_patch (object obj)  {
    return obj.is_patch();
}

inline bool is_widget (object obj) {
    return obj.is_widget();
}

inline bool as_bool (object obj)  {
    return obj.as_bool();
}

inline int as_int (object obj)  {
    return obj.as_int();
}

inline double as_double (object obj) {
    return obj.as_double();
}

inline string as_string (object obj) {
    return obj.as_string();
}

inline string as_symbol (object obj) {
    return obj.as_symbol();
}

inline tree as_tree (object obj) {
    return obj.as_tree();
}

inline scheme_tree as_scheme_tree (object obj) {
    return obj.to_tmscm()->to_scheme_tree();
}

inline list<string> as_list_string (object obj) {
    return obj.as_list_string();
}

inline list<tree> as_list_tree (object obj) {
    return obj.as_list_tree();
}

inline path as_path (object obj) {
    return obj.as_path();
}

inline array<object> as_array_object (object obj) {
    return obj.as_array_object();
}

inline url as_url (object obj) {
    return obj.as_url();
}

inline array<double> as_array_double (object obj) {
    return obj.as_array_double();
}

inline modification as_modification (object obj) {
    return obj.as_modification();
}

inline patch as_patch (object obj) {
    return obj.as_patch();
}

inline widget as_widget (object obj) {
    return obj.as_widget();
}

inline object tree_to_stree (tree t) {
    return scheme().tree_to_stree(t);
}

inline tree   stree_to_tree (object obj) {
    return scheme().stree_to_tree(obj);
}

inline tree   content_to_tree (object obj) {
    return scheme().content_to_tree(obj);
}

inline object string_to_object (string s) {
    return scheme().string_to_object(s);
}

inline string object_to_string (object obj) {
    return scheme().object_to_string(obj);
}

inline object scheme_cmd (const char* s) {
    return scheme().eval ("(lambda () " * string (s) * ")");
}

inline object scheme_cmd (string s) {
    return scheme().eval ("(lambda () " * s * ")");
}

inline object scheme_cmd (object cmd) {
    cmd= cons (cmd, null_object ());
    cmd= cons (null_object (), cmd);
    cmd= cons (scheme().eval ("'lambda"), cmd);
    return scheme().eval (cmd);
}

inline void notify_preferences_booted () {
    return scheme().notify_preferences_booted();
}


inline void set_preference (string var, string val) {
    return scheme().set_preference(var, val);
}

void notify_preference (string var);

inline string get_preference (string var, string def= "default") {
    return scheme().get_preference(var, def);
}

inline object eval (const char* expr) {
    return scheme().eval(expr);
}

inline object eval (string expr) {
    return scheme().eval(expr);
}

inline object eval (object expr) {
    return scheme().eval(expr);
}

inline object eval_secure (string expr) {
    return scheme().eval_secure(expr);
}

inline object eval_file (string name) {
    return scheme().eval_file(name);
}

inline bool   exec_file (url u) {
    return scheme().exec_file(u);
}

void   exec_delayed (object cmd);
void   exec_delayed_pause (object cmd);
void   exec_pending_commands ();
void   clear_pending_commands ();

inline void   protected_call (object cmd) {
    return scheme().protected_call(cmd);
}

template <typename... Args> inline object call(const char* fun) {
    std::vector<tmscm> args_vec;
    return {scheme().call_scheme (scheme().eval_scheme(fun), args_vec)};
}

template <typename... Args> inline object call(string fun) {
    std::vector<tmscm> args_vec;
    return {scheme().call_scheme (scheme().eval_scheme(fun), args_vec)};
}

template <typename... Args> inline object call(object fun) {
    std::vector<tmscm> args_vec;
    return {scheme().call_scheme (fun.to_tmscm(), args_vec)};
}

template <typename... Args> inline object call(const char* fun, Args... args) {
    std::vector<object> objs;
    objects_from(objs, args...);
    std::vector<tmscm> args_vec;
    for (auto arg : objs) {
        args_vec.push_back(arg.to_tmscm());
    }
    return {scheme().call_scheme (scheme().eval_scheme(fun), args_vec)};
}

template <typename... Args> inline object call(string fun, Args... args) {
    std::vector<object> objs;
    objects_from(objs, args...);
    std::vector<tmscm> args_vec;
    for (auto arg : objs) {
        args_vec.push_back(arg.to_tmscm());
    }
    return {scheme().call_scheme (scheme().eval_scheme(fun), args_vec)};
}

template <typename... Args> inline object call(object fun, Args... args) {
    std::vector<object> objs;
    objects_from(objs, args...);
    std::vector<tmscm> args_vec;
    for (auto arg : objs) {
        args_vec.push_back(arg.to_tmscm());
    }
    return {scheme().call_scheme (fun.to_tmscm(), args_vec)};
}


inline tmscm  object_to_tmscm  (object o) {
    return o.to_tmscm();
}

inline object tmscm_to_object (tmscm  obj) {
    return {obj};
}

template <int requiredArgs, typename FN> inline void install_procedure (const char* name, FN fn) {
    if constexpr (requiredArgs == 0) {
        scheme().install_procedure(name, [fn](abstract_scheme*, tmscm args) {
            return fn();
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 1) {
        scheme().install_procedure(name, [fn](abstract_scheme*, tmscm args) {
            return fn(args->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 2) {
        scheme().install_procedure(name, [fn](abstract_scheme*, tmscm args) {
            return fn(args->car(), args->cdr()->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 3) {
        scheme().install_procedure(name, [fn](abstract_scheme*, tmscm args) {
            return fn(args->car(), args->cdr()->car(), args->cdr()->cdr()->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 4) {
        scheme().install_procedure(name, [fn](abstract_scheme*, tmscm args) {
            return fn(args->car(), args->cdr()->car(), args->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 5) {
        scheme().install_procedure(name, [fn](abstract_scheme *, tmscm args) {
            return fn(args->car(), args->cdr()->car(), args->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 6) {
        scheme().install_procedure(name, [fn](abstract_scheme *, tmscm args) {
            return fn(args->car(), args->cdr()->car(), args->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 7) {
        scheme().install_procedure(name, [fn](abstract_scheme *, tmscm args) {
            return fn(args->car(), args->cdr()->car(), args->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 8) {
        scheme().install_procedure(name, [fn](abstract_scheme *, tmscm args) {
            return fn(args->car(), args->cdr()->car(), args->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 9) {
        scheme().install_procedure(name, [fn](abstract_scheme *, tmscm args) {
            return fn(args->car(), args->cdr()->car(), args->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car());
        }, requiredArgs, 0);
    }
    else if constexpr (requiredArgs == 10) {
        scheme().install_procedure(name, [fn](abstract_scheme *, tmscm args) {
            return fn(args->car(), args->cdr()->car(), args->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car(),
                      args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car(), args->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->cdr()->car());
        }, requiredArgs, 0);
    }
    else {
        static_assert(requiredArgs <= 10, "Too many arguments");
    }
}

#define tmscm_install_procedure(name, fn, requiredArgs, unuseda, unusedb) install_procedure<requiredArgs>(name, fn)


class object_command_rep: public command_rep {
    object obj;
public:
    object_command_rep (object obj2): obj (obj2) {}
    void apply () {
        (void) scheme().call_scheme (object_to_tmscm (obj));
    }

    void apply (object args) {
        array<tmscm> a = scheme().array_lookup (as_array_object (args));
        std::vector<tmscm> v (N(a));
        for (int i=0; i<N(a); i++)
            v[i]= a[i];
        (void) scheme().call_scheme_args (object_to_tmscm (obj), v);
    }
    tm_ostream& print (tm_ostream& out) { return out << obj; }
};

inline command as_command (object obj) {
    return tm_new<object_command_rep> (obj);
}

#endif
#endif // defined SCHEME_HH
