#ifndef SCHEME_GUILE30_TMSCM_HPP
#define SCHEME_GUILE30_TMSCM_HPP

#include "Scheme/tmscm.hpp"
#include <libguile.hpp>

namespace texmacs {

    scm_t_bits &guile_blackbox_tag();
    
    class guile_scheme;

    class guile_tmscm : public abstract_tmscm {

    public:
        template<typename ...Args>
        static inline tmscm mk(Args &&... args) {
            return make_tmscm<guile_tmscm>(std::forward<Args>(args)...);
        }

        guile_tmscm(guile_scheme *scheme, SCM scm);

        ~guile_tmscm() override;

        void mark();

        void unmark();

        SCM getSCM() const;

        tmscm null();

        void set_car(tmscm b) override;

        void set_cdr(tmscm b) override;

        bool is_equal(tmscm o2) override;

        bool is_null() override;

        bool is_pair() override;

        bool is_list() override;

        bool is_bool() override;

        bool is_int() override;

        bool is_double() override;

        bool is_string() override;

        bool is_symbol() override;

        tmscm cons(tmscm obj2) override;

        tmscm car() override;

        tmscm cdr() override;

        tmscm caar() override;

        tmscm cadr() override;

        tmscm cdar() override;

        tmscm cddr() override;

        tmscm caddr() override;

        tmscm cadddr() override;

        bool is_blackbox() override;

        blackbox to_blackbox() override;

        bool to_bool() override;

        int to_int() override;

        double to_double() override;

        string to_string() override;

        string to_symbol() override;

        int hash() override;

        string type();

    private:
        guile_scheme *mScheme;
        SCM mSCM;
        SCM mHandle{};
        std::string mType;
    };

}

#endif