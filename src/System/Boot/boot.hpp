
/******************************************************************************
* MODULE     : boot.hpp
* DESCRIPTION: manipulation of TeX font files
* COPYRIGHT  : (C) 1999  Joris van der Hoeven
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#ifndef BOOT_H
#define BOOT_H

#include "url.hpp"
#include "scheme.hpp"

extern tree texmacs_settings;
extern int  install_status;
extern bool use_which;
extern bool use_locate;
extern bool headless_mode;

string get_setting (string var, string def= "");
void   set_setting (string var, string val);
void   get_old_settings (string s);
void   init_upgrade ();
void   init_texmacs ();
void   init_plugins ();
void   setup_texmacs ();
void   release_boot_lock ();

scheme_tree plugin_list ();

void   load_user_preferences ();
void   save_user_preferences ();

extern bool user_prefs_modified;
extern hashmap<string,string> user_prefs;

inline bool has_user_preference (string var) {
    return user_prefs->contains (var);
}

inline void set_user_preference (string var, string val) {
    if (val == "default") user_prefs->reset (var);
    else user_prefs (var)= val;
    user_prefs_modified= true;
    notify_preference (var);
}

inline void reset_user_preference (string var) {
    user_prefs->reset (var);
    user_prefs_modified= true;
    notify_preference (var);
}

inline string get_user_preference (string var, string val = "") {
    if (user_prefs->contains (var)) return user_prefs[var];
    else return val;
}

#endif // defined BOOT_H
