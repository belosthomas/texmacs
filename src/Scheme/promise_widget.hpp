#ifndef SCHEME_PROMISE_WIDGET_HPP
#define SCHEME_PROMISE_WIDGET_HPP

#include "promise.hpp"
#include "scheme.hpp"
#include "tmscm.hpp"
#include "object.hpp"

class object_promise_widget_rep: public promise_rep<widget> {
    object obj;
public:
    object_promise_widget_rep (object obj2): obj (obj2) {}
    tm_ostream& print (tm_ostream& out) {
        return out << obj;
    }
    widget eval () {
        tmscm result= scheme().call_scheme(obj.to_tmscm());
        if (result->is_widget()) return result->to_widget();
        else {
            TM_FAILED ("widget expected");
            return glue_widget ();
        }
    }
};

inline bool tmscm_is_promise_widget (tmscm t) {
    return (t->is_blackbox() && (type_box (t->to_blackbox()) == type_helper<promise<widget>>::id));
}

inline promise<widget> tmscm_to_promise_widget (tmscm t) {
    return open_box<promise<widget>>(t->to_blackbox());
}

promise<widget> as_promise_widget (object o) {
    return tm_new<object_promise_widget_rep>(o);
}


tmscm promise_widget_to_tmscm (promise<widget> o) {
    return scheme().blackbox_to_tmscm (close_box<promise<widget>> (o));
}

#endif