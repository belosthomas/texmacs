
/******************************************************************************
* MODULE     : sys_utils.cpp
* DESCRIPTION: file handling
* COPYRIGHT  : (C) 1999-2016  Joris van der Hoeven, Denis Raux
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#include "sys_utils.hpp"
#include "file.hpp"
#include "tree.hpp"
#include "parse_string.hpp"

#ifdef WIN32
#include "Qt/qt_sys_utils.hpp"
#include "Windows/mingw_sys_utils.hpp"
#else
#include "Unix/unix_sys_utils.hpp"
#endif

#include <QApplication>

int script_status = 1;

/******************************************************************************
* System functions
******************************************************************************/

int
system (string s, string& result, string& error) {
#if WIN32
  int r= qt_system (s, result, error);
#else
  int r= unix_system (s, result, error);
#endif
  return r;
}

int
system (string s, string& result) {
#if WIN32
  int r= qt_system (s, result);
#else
  int r= unix_system (s, result);
#endif
  return r;
}

int
system (string s) {
  if (DEBUG_STD) debug_shell << s << "\n";
  if (DEBUG_VERBOSE) {
    string result;
    int r= system (s, result);
    debug_shell << result;
    return r;
  }
  else {
#if WIN32
    // if (starts (s, "convert ")) return 1;
    return qt_system (s);
#else
    return unix_system (s);
#endif
  }
}

string
eval_system (string s) {
  string result;
  (void) system (s, result);
  return result;
}

string
var_eval_system (string s) {
  string r= eval_system (s);
  while ((N(r)>0) && (r[N(r)-1]=='\n' || r[N(r)-1]=='\r')) r= r (0,N(r)-1);
  return r;
}

string
get_env (string var) {
  c_string _var (var);
  QByteArray _ret= qgetenv (_var);
  if (_ret.isNull() || _ret.isEmpty()) {
    if (var == "PWD") return get_env ("HOME");
    return "";
  }
  string ret (_ret.data(), _ret.size());
  return ret;
  // do not delete _ret !
}

void
set_env (string var, string with) {
#if defined(STD_SETENV) && !WIN32
  c_string _var  (var);
  c_string _with (with);
  setenv (_var, _with, 1);
#else
  c_string _var  (var);
  c_string _with (with);
  (void) qputenv (&*_var, &*_with);
  // do not delete _varw !!!
  // -> known memory leak, but solution more complex than it is worth
#endif
}

url
get_texmacs_path () {
  string tmpath= get_env ("TEXMACS_PATH");
    //FIXME: Why is this?
  while ((N(tmpath)>0) && (tmpath [N(tmpath) - 1] == '/'))
    tmpath= tmpath (0, N(tmpath)-1);
  return url_system (tmpath);
}

url
get_texmacs_home_path () {
  url path= get_env ("TEXMACS_HOME_PATH");
  if (path == "")
    path= url_system ("$HOME/.TeXmacs");
  return path;
}

array<string>
evaluate_system (array<string> arg,
		 array<int> fd_in, array<string> in,
		 array<int> fd_out) {
  array<string> out (N(fd_out));
  array<string*> ptr (N(fd_out));
  for (int i= 0; i < N(fd_out); i++) ptr[i]= &(out[i]);
#ifdef WIN32
  int ret= mingw_system (arg, fd_in, in, fd_out, ptr);
#else
  int ret= unix_system (arg, fd_in, in, fd_out, ptr);
#endif
  return append (as_string (ret), out);
}


string 
get_printing_default () {
#if WIN32
  url embedded ("$TEXMACS_PATH/bin/SumatraPDF.exe");
  if (exists (embedded))
    return sys_concretize (embedded) * " -print-dialog -exit-when-done";
  else return "";
#else
  return "lp";
#endif
}

class PrintCap {
private:
  string prt_cmd;
  bool blank;
public:	
  PrintCap (): blank (true) {};
  friend string get_printing_cmd ();
  friend void set_printing_cmd (string);
} print_cap;

string
get_printing_cmd () {
  if (print_cap.blank) {
    print_cap.prt_cmd= get_printing_default ();
    print_cap.blank= false;
  }
  return print_cap.prt_cmd;
}

void
set_printing_cmd (string cmd) {
  print_cap.prt_cmd= cmd;
  print_cap.blank= false;
}

bool
has_printing_cmd () {
  static bool has= get_printing_cmd () != "";
  return has;
}
