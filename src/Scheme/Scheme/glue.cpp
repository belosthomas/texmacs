
/******************************************************************************
* MODULE     : glue.cpp
* DESCRIPTION: Glue for linking TeXmacs commands to scheme
* COPYRIGHT  : (C) 1999-2011  Joris van der Hoeven and Massimiliano Gubinelli
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#include "Scheme/scheme.hpp"

#include "promise.hpp"
#include "tree.hpp"
#include "drd_mode.hpp"
#include "tree_search.hpp"
#include "modification.hpp"
#include "patch.hpp"

#include "boxes.hpp"
#include "editor.hpp"
#include "universal.hpp"
#include "convert.hpp"
#include "file.hpp"
#include "locale.hpp"
#include "iterator.hpp"
#include "Freetype/tt_tools.hpp"
#include "Database/database.hpp"
#include "Sqlite3/sqlite3.hpp"
#include "Updater/tm_updater.hpp"

tmscm 
blackboxP (tmscm t) {
  bool b= t->is_blackbox();
  return scheme().bool_to_tmscm(b);
}

#if 0
template<class T> tmscm box_to_tmscm (T o) {
  return blackbox_to_tmscm (close_box<T> (o)); }
template<class T> T tmscm_to_box (tmscm obj) { 
  return open_box<T>(obj->to_blackbox());  }
template<class T> tmscm cmp_box (tmscm o1, tmscm o2) { 
  return bool_to_tmscm (tmscm_to_box<T> (o1) == tmscm_to_box<T> (o2)); }
template<class T> tmscm boxP (tmscm t) {
  bool b= t->is_blackbox() && 
          (type_box (blackboxvalue(t)) == type_helper<T>::id);
  return bool_to_tmscm (b);
}
#endif

/******************************************************************************
* Miscellaneous routines for use by glue only
******************************************************************************/

string original_path;

string
get_original_path () {
  return original_path;
}

string
texmacs_version (string which) {
  if (which == "tgz") return TM_DEVEL;
  if (which == "rpm") return TM_DEVEL_RELEASE;
  if (which == "stgz") return TM_STABLE;
  if (which == "srpm") return TM_STABLE_RELEASE;
  if (which == "devel") return TM_DEVEL;
  if (which == "stable") return TM_STABLE;
  if (which == "devel-release") return TM_DEVEL_RELEASE;
  if (which == "stable-release") return TM_STABLE_RELEASE;
  if (which == "revision") return TEXMACS_REVISION;
  return TEXMACS_VERSION;
}

void
set_fast_environments (bool b) {
  enable_fastenv= b;
}

void
win32_display (string s) {
  cout << s;
  cout.flush ();
}

void
tm_output (string s) {
  cout << s;
  cout.flush ();
}

void
tm_errput (string s) {
  cerr << s;
  cerr.flush ();
}

void
cpp_error () {
  //char *np= 0; *np= 1;
  FAILED ("an error occurred");
}

array<int>
get_bounding_rectangle (tree t) {
  editor ed= get_current_editor ();
  rectangle wr= ed -> get_window_extents ();
  path p= reverse (obtain_ip (t));
  selection sel= ed->search_selection (p * start (t), p * end (t));
  SI sz= ed->get_pixel_size ();
  double sf= ((double) sz) / 256.0;
  rectangle selr= least_upper_bound (sel->rs) / sf;
  rectangle r= translate (selr, wr->x1, wr->y2);
  array<int> ret;
  ret << (r->x1) << (r->y1) << (r->x2) << (r->y2);
  //ret << (r->x1/PIXEL) << (r->y1/PIXEL) << (r->x2/PIXEL) << (r->y2/PIXEL);
  return ret;
}

bool
supports_native_pdf () {
#ifdef PDF_RENDERER
  return true;
#else
  return false;
#endif
}

bool
supports_ghostscript () {
#ifdef USE_GS
  return true;
#else
  return false;
#endif
}

bool
is_busy_versioning () {
  return busy_versioning;
}

array<SI>
get_screen_size () {
  array<SI> r;
  SI w, h;
  gui_root_extents (w, h);
  r << w << h;
  return r;
}

/******************************************************************************
* Redirections
******************************************************************************/

void
cout_buffer () {
  cout.buffer ();
}

string
cout_unbuffer () {
  return cout.unbuffer ();
}

/******************************************************************************
* Basic assertions
******************************************************************************/

#define TMSCM_ASSERT(a, b, c, d)

#define TMSCM_ASSERT_STRING(s,arg,rout) \
TMSCM_ASSERT (s->is_string(), s, arg, rout)
#define TMSCM_ASSERT_BOOL(flag,arg,rout) \
TMSCM_ASSERT (flag->is_bool(), flag, arg, rout)
#define TMSCM_ASSERT_INT(i,arg,rout) \
TMSCM_ASSERT (i->is_int(), i, arg, rout);
#define TMSCM_ASSERT_DOUBLE(i,arg,rout) \
  TMSCM_ASSERT (i->is_double(), i, arg, rout);
//TMSCM_ASSERT (SCM_REALP (i), i, arg, rout);
#define TMSCM_ASSERT_URL(u,arg,rout) \
TMSCM_ASSERT (u->is_url() || u->is_string(), u, arg, rout)
#define TMSCM_ASSERT_MODIFICATION(m,arg,rout) \
TMSCM_ASSERT (m->is_modification(), m, arg, rout)
#define TMSCM_ASSERT_PATCH(p,arg,rout) \
TMSCM_ASSERT (p->is_patch(), p, arg, rout)
#define TMSCM_ASSERT_BLACKBOX(t,arg,rout) \
TMSCM_ASSERT (t->is_blackbox(), t, arg, rout)
#define TMSCM_ASSERT_SYMBOL(s,arg,rout) \
  TMSCM_ASSERT (s->is_symbol (), s, arg, rout)
//TMSCM_ASSERT (SCM_NFALSEP (tmscm_symbol_p (s)), s, arg, rout)

#define TMSCM_ASSERT_OBJECT(a,b,c)
// no check

/******************************************************************************
* Tree labels
******************************************************************************/

#define TMSCM_ASSERT_TREE_LABEL(p,arg,rout) TMSCM_ASSERT_SYMBOL(p,arg,rout)

tmscm 
tree_label_to_tmscm (tree_label l) {
  string s= as_string (l);
  return scheme().symbol_to_tmscm (s);
}

tree_label
tmscm_to_tree_label (tmscm p) {
  string s= p->to_symbol();
  return make_tree_label (s);
}

/******************************************************************************
* Trees
******************************************************************************/

#define TMSCM_ASSERT_TREE(t,arg,rout) TMSCM_ASSERT (tmscm_is_tree (t), t, arg, rout)


bool
tmscm_is_tree (tmscm u) {
  return (u->is_blackbox() && 
         (type_box (u->to_blackbox()) == type_helper<tree>::id));
}

tmscm 
tree_to_tmscm (tree o) {
  return scheme().blackbox_to_tmscm (close_box<tree> (o));
}

tree
tmscm_to_tree (tmscm obj) {
  return open_box<tree>(obj->to_blackbox());
}

tmscm 
treeP (tmscm t) {
  bool b= t->is_blackbox() && 
          (type_box (t->to_blackbox()) == type_helper<tree>::id);
  return scheme().bool_to_tmscm (b);
}

tree
coerce_string_tree (string s) {
  return s;
}

string
coerce_tree_string (tree t) {
  return as_string (t);
}

tree
tree_ref (tree t, int i) {
  return t[i];
}

tree
tree_set (tree t, int i, tree u) {
  t[i]= u;
  return u;
}

tree
tree_range (tree t, int i, int j) {
  return t(i,j);
}

tree
tree_append (tree t1, tree t2) {
  return t1 * t2;
}

bool
tree_active (tree t) {
  path ip= obtain_ip (t);
  return is_nil (ip) || last_item (ip) != DETACHED;
}

tree
tree_child_insert (tree t, int pos, tree x) {
  //cout << "t= " << t << "\n";
  //cout << "x= " << x << "\n";
  int i, n= N(t);
  tree r (t, n+1);
  for (i=0; i<pos; i++) r[i]= t[i];
  r[pos]= x;
  for (i=pos; i<n; i++) r[i+1]= t[i];
  return r;
}

/******************************************************************************
* Document modification routines
******************************************************************************/

extern tree the_et;

tree
tree_assign (tree r, tree t) {
  path ip= copy (obtain_ip (r));
  if (ip_attached (ip)) {
    assign (reverse (ip), copy (t));
    return subtree (the_et, reverse (ip));
  }
  else {
    assign (r, copy (t));
    return r;
  }
}

tree
tree_insert (tree r, int pos, tree t) {
  path ip= copy (obtain_ip (r));
  if (ip_attached (ip)) {
    insert (reverse (path (pos, ip)), copy (t));
    return subtree (the_et, reverse (ip));
  }
  else {
    insert (r, pos, copy (t));
    return r;
  }
}

tree
tree_remove (tree r, int pos, int nr) {
  path ip= copy (obtain_ip (r));
  if (ip_attached (ip)) {
    remove (reverse (path (pos, ip)), nr);
    return subtree (the_et, reverse (ip));
  }
  else {
    remove (r, pos, nr);
    return r;
  }
}

tree
tree_split (tree r, int pos, int at) {
  path ip= copy (obtain_ip (r));
  if (ip_attached (ip)) {
    split (reverse (path (at, pos, ip)));
    return subtree (the_et, reverse (ip));
  }
  else {
    split (r, pos, at);
    return r;
  }
}

tree
tree_join (tree r, int pos) {
  path ip= copy (obtain_ip (r));
  if (ip_attached (ip)) {
    join (reverse (path (pos, ip)));
    return subtree (the_et, reverse (ip));
  }
  else {
    join (r, pos);
    return r;
  }
}

tree
tree_assign_node (tree r, tree_label op) {
  path ip= copy (obtain_ip (r));
  if (ip_attached (ip)) {
    assign_node (reverse (ip), op);
    return subtree (the_et, reverse (ip));
  }
  else {
    assign_node (r, op);
    return r;
  }
}

tree
tree_insert_node (tree r, int pos, tree t) {
  path ip= copy (obtain_ip (r));
  if (ip_attached (ip)) {
    insert_node (reverse (path (pos, ip)), copy (t));
    return subtree (the_et, reverse (ip));
  }
  else {
    insert_node (r, pos, copy (t));
    return r;
  }
}

tree
tree_remove_node (tree r, int pos) {
  path ip= copy (obtain_ip (r));
  if (ip_attached (ip)) {
    remove_node (reverse (path (pos, ip)));
    return subtree (the_et, reverse (ip));
  }
  else {
    remove_node (r, pos);
    return r;
  }
}

/******************************************************************************
* Scheme trees
******************************************************************************/

#define TMSCM_ASSERT_SCHEME_TREE(p,arg,rout)

tmscm 
scheme_tree_to_tmscm (scheme_tree t) {
  if (is_atomic (t)) {
    string s= t->label;
    if (s == "#t") return scheme().tmscm_true ();
    if (s == "#f") return scheme().tmscm_false ();
    if (is_int (s)) return scheme().int_to_tmscm (as_int (s));
    if (is_quoted (s))
      return scheme().string_to_tmscm (scm_unquote (s));
    //if ((N(s)>=2) && (s[0]=='\42') && (s[N(s)-1]=='\42'))
    //return string_to_tmscm (s (1, N(s)-1));
    if (N(s) >= 1 && s[0] == '\'') return scheme().symbol_to_tmscm (s (1, N(s)));
    return scheme().symbol_to_tmscm (s);
  }
  else {
    int i;
    tmscm p= scheme().tmscm_null ();
    for (i=N(t)-1; i>=0; i--)
      p= scheme_tree_to_tmscm (t[i])->cons(p);
    return p;
  }
}

scheme_tree
tmscm_to_scheme_tree (tmscm p) {
  if (p->is_list ()) {
    tree t (TUPLE);
    while (!p->is_null()) {
      t << tmscm_to_scheme_tree (p->car());
      p= p->cdr();
    }
    return t;
  }
  if (p->is_symbol ()) return p->to_symbol();
  if (p->is_string()) return scm_quote (p->to_string());
  //if (p->is_string()) return "\"" * tmscm_to_string (p) * "\"";
  if (p->is_int()) return as_string ((int)p->to_int());
  if (p->is_bool()) return (p->to_bool()? string ("#t"): string ("#f"));
  if (tmscm_is_tree (p)) return tree_to_scheme_tree (tmscm_to_tree (p));
  return "?";
}

/******************************************************************************
* Content
******************************************************************************/

bool
tmscm_is_content (tmscm p) {
  if (p->is_string() || tmscm_is_tree (p)) return true;
  else if (!p->is_pair() || !p->car()->is_symbol()) return false;
  else {
    for (p= p->cdr(); !p->is_null(); p= p->cdr())
      if (!tmscm_is_content (p->car())) return false;
    return true;
  }
}

#define content tree
#define TMSCM_ASSERT_CONTENT(p,arg,rout) \
   TMSCM_ASSERT (tmscm_is_content (p), p, arg, rout)
#define content_to_tmscm tree_to_tmscm

tree
tmscm_to_content (tmscm p) {
  if (p->is_string()) return p->to_string();
  if (tmscm_is_tree (p)) return tmscm_to_tree (p);
  if (p->is_pair()) {
    if (!p->car()->is_symbol()) return "?";
    tree t (make_tree_label (p->car()->to_symbol()));
    p= p->cdr();
    while (!p->is_null()) {
      t << tmscm_to_content (p->car());
      p= p->cdr();
    }
    return t;
  }
  return "?";
}

tmscm 
contentP (tmscm t) {
  bool b= tmscm_is_content (t);
  return scheme().bool_to_tmscm (b);
}

/******************************************************************************
* Paths
******************************************************************************/

bool
tmscm_is_path (tmscm p) {
  if (p->is_null()) return true;
  else return p->car()->is_int() && p->cdr()->is_path();
}

#define TMSCM_ASSERT_PATH(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_path (p), p, arg, rout)

tmscm 
path_to_tmscm (path p) {
  if (is_nil (p)) return scheme().tmscm_null();
  else return scheme().int_to_tmscm (p->item)->cons(path_to_tmscm (p->next));
}

path
tmscm_to_path (tmscm p) {
  if (p->is_null()) return path ();
  else return path ((int) p->car()->to_int(), tmscm_to_path (p->cdr()));
}


/******************************************************************************
* Observers
******************************************************************************/

#define TMSCM_ASSERT_OBSERVER(o,arg,rout) \
TMSCM_ASSERT (tmscm_is_observer (o), o, arg, rout)


bool
tmscm_is_observer (tmscm o) {
  return (o->is_blackbox() &&
         (type_box (o->to_blackbox()) == type_helper<observer>::id));
}

tmscm 
observer_to_tmscm (observer o) {
  return scheme().blackbox_to_tmscm (close_box<observer> (o));
}

static observer
tmscm_to_observer (tmscm obj) {
  return open_box<observer>(obj->to_blackbox());
}

tmscm 
observerP (tmscm t) {
  bool b= t->is_blackbox() && 
  (type_box (t->to_blackbox()) == type_helper<observer>::id);
  return scheme().bool_to_tmscm (b);
}


/******************************************************************************
* Widgets
******************************************************************************/

#define TMSCM_ASSERT_WIDGET(o,arg,rout) \
TMSCM_ASSERT (tmscm_is_widget (o), o, arg, rout)

bool
tmscm_is_widget (tmscm u) {
  return (u->is_blackbox() &&
         (type_box (u->to_blackbox()) == type_helper<widget>::id));
}


static tmscm 
widget_to_tmscm (widget o) {
  return scheme().blackbox_to_tmscm (close_box<widget> (o));
}

widget
tmscm_to_widget (tmscm o) {
  return open_box<widget> (o->to_blackbox());
}

/******************************************************************************
* Commands
******************************************************************************/

#define TMSCM_ASSERT_COMMAND(o,arg,rout) \
TMSCM_ASSERT (tmscm_is_command (o), o, arg, rout)

bool
tmscm_is_command (tmscm u) {
  return (u->is_blackbox() && 
      (type_box (u->to_blackbox()) == type_helper<command>::id));
}

static tmscm 
command_to_tmscm (command o) {
  return scheme().blackbox_to_tmscm (close_box<command> (o));
}

static command
tmscm_to_command (tmscm o) {
  return open_box<command> (o->to_blackbox());
}

/******************************************************************************
*  Widget Factory
******************************************************************************/

#include "promise_widget.hpp"
typedef promise<widget> promise_widget;

#define TMSCM_ASSERT_PROMISE_WIDGET(o,arg,rout) \
TMSCM_ASSERT (tmscm_is_promise_widget (o), o, arg, rout)


/******************************************************************************
* Urls
******************************************************************************/

bool
tmscm_is_url (tmscm u) {
  return (u->is_blackbox()
              && (type_box (u->to_blackbox()) == type_helper<url>::id))
         || (u->is_string());
}

tmscm 
url_to_tmscm (url u) {
  return scheme().blackbox_to_tmscm (close_box<url> (u));
}

url
tmscm_to_url (tmscm obj) {
  if (obj->is_string())
#ifdef OS_MINGW
    return url_system (obj->to_string());
#else
  return obj->to_string();
#endif
  return open_box<url> (obj->to_blackbox());
}

tmscm 
urlP (tmscm t) {
  bool b= tmscm_is_url (t);
  return scheme().bool_to_tmscm (b);
}

url url_concat (url u1, url u2) { return u1 * u2; }
url url_or (url u1, url u2) { return u1 | u2; }
void string_save (string s, url u) { (void) save_string (u, s); }
string string_load (url u) {
  string s; (void) load_string (u, s, false); return s; }
void string_append_to_file (string s, url u) { (void) append_string (u, s); }
url url_ref (url u, int i) { return u[i]; }

/******************************************************************************
* Modification
******************************************************************************/

bool
tmscm_is_modification (tmscm m) {
  return (m->is_blackbox() &&
	  (type_box (m->to_blackbox()) == type_helper<modification>::id))
    || (m->is_string());
}

tmscm 
modification_to_tmscm (modification m) {
  return scheme().blackbox_to_tmscm (close_box<modification> (m));
}

modification
tmscm_to_modification (tmscm obj) {
  return open_box<modification> (obj->to_blackbox());
}

tmscm 
modificationP (tmscm t) {
  bool b= tmscm_is_modification (t);
  return scheme().bool_to_tmscm (b);
}

tree
var_apply (tree& t, modification m) {
  apply (t, copy (m));
  return t;
}

tree
var_clean_apply (tree& t, modification m) {
  return clean_apply (t, copy (m));
}

/******************************************************************************
* Patch
******************************************************************************/

bool
tmscm_is_patch (tmscm p) {
  return (p->is_blackbox() &&
	  (type_box (p->to_blackbox()) == type_helper<patch>::id))
    || (p->is_string());
}

tmscm 
patch_to_tmscm (patch p) {
  return scheme().blackbox_to_tmscm (close_box<patch> (p));
}

patch
tmscm_to_patch (tmscm obj) {
  return open_box<patch> (obj->to_blackbox());
}

tmscm 
patchP (tmscm t) {
  bool b= tmscm_is_patch (t);
  return scheme().bool_to_tmscm (b);
}

patch
branch_patch (array<patch> a) {
  return patch (true, a);
}

tree
var_clean_apply (tree t, patch p) {
  return clean_apply (copy (p), t);
}

tree
var_apply (tree& t, patch p) {
  apply (copy (p), t);
  return t;
}

/******************************************************************************
* Table types
******************************************************************************/

typedef hashmap<string,string> table_string_string;

bool
tmscm_is_table_string_string (tmscm p) {
  if (p->is_null()) return true;
  else if (!p->is_pair()) return false;
  else {
    tmscm f= p->car();
    return f->is_pair() &&
    f->car()->is_string() &&
    f->cdr()->is_string() &&
    tmscm_is_table_string_string (p->cdr());
  }
}

#define TMSCM_ASSERT_TABLE_STRING_STRING(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_table_string_string (p), p, arg, rout)

tmscm 
table_string_string_to_tmscm (hashmap<string,string> t) {
  tmscm p= scheme().tmscm_null ();
  iterator<string> it= iterate (t);
  while (it->busy ()) {
    string s= it->next ();
    tmscm n= scheme().string_to_tmscm (s)->cons(scheme().string_to_tmscm (t[s]));
    p= n->cons(p);
  }
  return p;
}

hashmap<string,string>
tmscm_to_table_string_string (tmscm p) {
  hashmap<string,string> t;
  while (!p->is_null()) {
    tmscm n= p->car();
    t (n->car()->to_string()) = n->cdr()->to_string();
    p= p->cdr();
  }
  return t;
}

#define tmscm_is_solution tmscm_is_table_string_string
#define TMSCM_ASSERT_SOLUTION(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_solution(p), p, arg, rout)
#define solution_to_tmscm table_string_string_to_tmscm
#define tmscm_to_solution tmscm_to_table_string_string

/******************************************************************************
* Several array types
******************************************************************************/

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

static bool
tmscm_is_array_int (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    p->car()->is_int() &&
    tmscm_is_array_int (p->cdr());
}

#define TMSCM_ASSERT_ARRAY_INT(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_int (p), p, arg, rout)

/* static */ tmscm 
array_int_to_tmscm (array<int> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null ();
  for (i=n-1; i>=0; i--) p= scheme().int_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<int>
tmscm_to_array_int (tmscm p) {
  array<int> a;
  while (!p->is_null()) {
    a << ((int) p->car()->to_int());
    p= p->cdr();
  }
  return a;
}

static bool
tmscm_is_array_string (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() && 
     p->car()->is_string() &&
    tmscm_is_array_string (p->cdr());
}


/* static */ bool
tmscm_is_array_double (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    p->car()->is_double() &&
    tmscm_is_array_double (p->cdr());
}

#define TMSCM_ASSERT_ARRAY_DOUBLE(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_double (p), p, arg, rout)

/* static */ tmscm 
array_double_to_tmscm (array<double> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null();
  for (i=n-1; i>=0; i--) p= scheme().double_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<double>
tmscm_to_array_double (tmscm p) {
  array<double> a;
  while (!p->is_null()) {
    a << (double)p->car()->to_double();
    p= p->cdr();
  }
  return a;
}

static bool
tmscm_is_array_array_double (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    tmscm_is_array_double (p->car()) &&
    tmscm_is_array_array_double (p->cdr());
}

#define TMSCM_ASSERT_ARRAY_ARRAY_DOUBLE(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_array_double (p), p, arg, rout)

/* static */ tmscm 
array_array_double_to_tmscm (array<array_double> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null ();
  for (i=n-1; i>=0; i--) p= array_double_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<array_double>
tmscm_to_array_array_double (tmscm p) {
  array<array_double> a;
  while (!p->is_null()) {
    a << ((array_double) tmscm_to_array_double (p->car()));
    p= p->cdr();
  }
  return a;
}

static bool
tmscm_is_array_array_array_double (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    tmscm_is_array_array_double (p->car()) &&
    tmscm_is_array_array_array_double (p->cdr());
}

#define TMSCM_ASSERT_ARRAY_ARRAY_ARRAY_DOUBLE(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_array_array_double (p), p, arg, rout)

/* static */ tmscm 
array_array_array_double_to_tmscm (array<array_array_double> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null ();
  for (i=n-1; i>=0; i--) p= array_array_double_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<array_array_double>
tmscm_to_array_array_array_double (tmscm p) {
  array<array_array_double> a;
  while (!p->is_null()) {
    a << ((array_array_double) tmscm_to_array_array_double (p->car()));
    p= p->cdr();
  }
  return a;
}

void register_glyph (string s, array_array_array_double gl);
string recognize_glyph (array_array_array_double gl);



#define TMSCM_ASSERT_ARRAY_STRING(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_string (p), p, arg, rout)

/* static */ tmscm 
array_string_to_tmscm (array<string> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null ();
  for (i=n-1; i>=0; i--) p=  scheme().string_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<string>
tmscm_to_array_string (tmscm p) {
  array<string> a;
  while (!p->is_null()) {
    a << p->car()->to_string();
    p= p->cdr();
  }
  return a;
}

static bool
tmscm_is_array_tree (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() && 
    tmscm_is_tree (p->car()) &&
    tmscm_is_array_tree (p->cdr());
}

#define TMSCM_ASSERT_ARRAY_TREE(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_tree (p), p, arg, rout)

/* static */ tmscm 
array_tree_to_tmscm (array<tree> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null();
  for (i=n-1; i>=0; i--) p= tree_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<tree>
tmscm_to_array_tree (tmscm p) {
  array<tree> a;
  while (!p->is_null()) {
    a << tmscm_to_tree (p->car());
    p= p->cdr();
  }
  return a;
}

static bool
tmscm_is_array_widget (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    tmscm_is_widget (p->car()) &&
    tmscm_is_array_widget (p->cdr());
}

#define TMSCM_ASSERT_ARRAY_WIDGET(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_widget (p), p, arg, rout)

/* static */ tmscm 
array_widget_to_tmscm (array<widget> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null();
  for (i=n-1; i>=0; i--) p= widget_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<widget>
tmscm_to_array_widget (tmscm p) {
  array<widget> a;
  while (!p->is_null()) {
    a << tmscm_to_widget (p->car());
    p= p->cdr();
  }
  return a;
}

static bool
tmscm_is_array_url (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    tmscm_is_url (p->car()) &&
    tmscm_is_array_url (p->cdr());
}


#define TMSCM_ASSERT_ARRAY_URL(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_url (p), p, arg, rout)

/* static */ tmscm 
array_url_to_tmscm (array<url> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null ();
  for (i=n-1; i>=0; i--) p= url_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<url>
tmscm_to_array_url (tmscm p) {
  array<url> a;
  while (!p->is_null()) {
    a << tmscm_to_url (p->car());
    p= p->cdr();
  }
  return a;
}

static bool
tmscm_is_array_patch (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    tmscm_is_patch (p->car()) &&
    tmscm_is_array_patch (p->cdr());
}


#define TMSCM_ASSERT_ARRAY_PATCH(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_patch (p), p, arg, rout)

/* static */ tmscm 
array_patch_to_tmscm (array<patch> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null ();
  for (i=n-1; i>=0; i--) p=  patch_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<patch>
tmscm_to_array_patch (tmscm p) {
  array<patch> a;
  while (!p->is_null()) {
    a << tmscm_to_patch (p->car());
    p= p->cdr();
  }
  return a;
}

static bool
tmscm_is_array_path (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    tmscm_is_path (p->car()) &&
    tmscm_is_array_path (p->cdr());
}

#define TMSCM_ASSERT_ARRAY_PATH(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_array_path (p), p, arg, rout)

/* static */ tmscm 
array_path_to_tmscm (array<path> a) {
  int i, n= N(a);
  tmscm p= scheme().tmscm_null ();
  for (i=n-1; i>=0; i--) p= path_to_tmscm (a[i])->cons(p);
  return p;
}

/* static */ array<path>
tmscm_to_array_path (tmscm p) {
  array<path> a;
  while (!p->is_null()) {
    a << tmscm_to_path (p->car());
    p= p->cdr();
  }
  return a;
}

/******************************************************************************
* List types
******************************************************************************/

typedef list<string> list_string;

bool
tmscm_is_list_string (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    p->car()->is_string() &&
    tmscm_is_list_string (p->cdr());
}

#define TMSCM_ASSERT_LIST_STRING(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_list_string (p), p, arg, rout)

tmscm 
list_string_to_tmscm (list_string l) {
  if (is_nil (l)) return scheme().tmscm_null();
  return scheme().string_to_tmscm (l->item)->cons(list_string_to_tmscm (l->next));
}

list_string
tmscm_to_list_string (tmscm p) {
  if (p->is_null()) return list_string ();
  return list_string ( p->car()->to_string(), tmscm_to_list_string (p->cdr()));
}

typedef list<tree> list_tree;

bool
tmscm_is_list_tree (tmscm p) {
  if (p->is_null()) return true;
  else return p->is_pair() &&
    tmscm_is_tree (p->car()) &&
    tmscm_is_list_tree (p->cdr());
}

#define TMSCM_ASSERT_LIST_TREE(p,arg,rout) \
TMSCM_ASSERT (tmscm_is_list_tree (p), p, arg, rout)

tmscm 
list_tree_to_tmscm (list_tree l) {
  if (is_nil (l)) return scheme().tmscm_null();
  return tree_to_tmscm (l->item)->cons(list_tree_to_tmscm (l->next));
}

list_tree
tmscm_to_list_tree (tmscm p) {
  if (p->is_null()) return list_tree ();
  return list_tree (tmscm_to_tree (p->car()),tmscm_to_list_tree (p->cdr()));
}

/******************************************************************************
* Gluing
******************************************************************************/

#include "server.hpp"
#include "tm_window.hpp"
#include "boot.hpp"
#include "connect.hpp"
#include "convert.hpp"
#include "file.hpp"
#include "image_files.hpp"
#include "web_files.hpp"
#include "sys_utils.hpp"
#include "client_server.hpp"
#include "analyze.hpp"
#include "wencoding.hpp"
#include "base64.hpp"
#include "tree_traverse.hpp"
#include "tree_analyze.hpp"
#include "tree_correct.hpp"
#include "tree_modify.hpp"
#include "tree_math_stats.hpp"
#include "tm_frame.hpp"
#include "Concat/concater.hpp"
#include "converter.hpp"
#include "tm_timer.hpp"
#include "Metafont/tex_files.hpp"
#include "Freetype/tt_file.hpp"
#include "LaTeX_Preview/latex_preview.hpp"
#include "Bibtex/bibtex.hpp"
#include "Bibtex/bibtex_functions.hpp"
#include "link.hpp"
#include "dictionary.hpp"
#include "patch.hpp"
#include "packrat.hpp"
#include "new_style.hpp"
#include "persistent.hpp"

#include "promise_widget.hpp"

#include "../Glue/glue_basic.cpp"
#include "../Glue/glue_editor.cpp"
#include "../Glue/glue_server.cpp"

void
initialize_glue () {
  tmscm_install_procedure ("tree?", treeP, 1, 0, 0);
  tmscm_install_procedure ("tm?", contentP, 1, 0, 0);
  tmscm_install_procedure ("observer?", observerP, 1, 0, 0);
  tmscm_install_procedure ("url?", urlP, 1, 0, 0);
  tmscm_install_procedure ("modification?", modificationP, 1, 0, 0);
  tmscm_install_procedure ("patch?", patchP, 1, 0, 0);
  tmscm_install_procedure ("blackbox?", blackboxP, 1, 0, 0);
  
  initialize_glue_basic ();
  initialize_glue_editor ();
  initialize_glue_server ();
}
