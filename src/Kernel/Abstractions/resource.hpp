
/******************************************************************************
* MODULE     : resource.hpp
* COPYRIGHT  : (C) 1999  Joris van der Hoeven
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H
#include "string.hpp"
#include "hashmap.hpp"

template<class T> struct rep {
  string res_name;
  inline rep<T> (string res_name2):res_name (res_name2) {
    T::instances (res_name)= static_cast<pointer>(this); }
  inline virtual ~rep<T> () {
    T::instances -> reset (res_name); }
};

template<class R> class resource_ptr {
protected:
  ~resource_ptr() {};
public:
  R* rep;
  static inline hashmap<string,pointer> instances = nullptr;
  inline R* operator ->()  { return rep; }
};

#define RESOURCE(PTR)                               \
class PTR##_rep;                                    \
class PTR : public resource_ptr<PTR##_rep> {       \
    public:                                        \
  inline PTR (PTR##_rep* rep2= NULL) { rep=rep2; }  \
  inline PTR (string s) { rep=(PTR##_rep*) instances [s]; } \
  inline ~PTR() {}                                  \
}

#define RESOURCE_CODE(PTR) //\
//template<> hashmap<string,pointer> resource_ptr<PTR##_rep>::instances;

template<class R>
inline bool is_nil (const resource_ptr<R>& res) { return res.rep == NULL; }

template<class R>
tm_ostream& operator << (tm_ostream& out, const resource_ptr<R>& t);

#define make(T,s,im) ((T::instances -> contains (s))? T(s): T(im))

template<class T>
tm_ostream& operator << (tm_ostream& out, const resource_ptr<T>& t) {
  return out << t->res_name;
}

#endif // RESOURCE_H
