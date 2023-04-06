#ifndef SCHEME_GUILE30_GUILETM_HPP
#define SCHEME_GUILE30_GUILETM_HPP

#include <mutex>
#include <thread>
#include <deque>
#include <functional>
#include <future>

#include "Scheme/abstract_scheme.hpp"
#include "guile18_tmscm.hpp"
#include "guile_thread.hpp"

#include <utility>


#define SET_SMOB(smob,data,type)   \
SCM_NEWSMOB (smob, SCM_UNPACK (type), data);


namespace texmacs {

    scm_t_bits &guile_blackbox_tag();

    void initialize_smobs();

    class guile_scheme : public abstract_scheme {

    public:
        guile_scheme();

        // disable copy constructor and assignment operator
        guile_scheme(const guile_scheme &) = delete;

        guile_scheme &operator=(const guile_scheme &) = delete;

        tmscm blackbox_to_tmscm(blackbox b) final;

        tmscm tmscm_null() final;

        tmscm tmscm_true() final;

        tmscm tmscm_false() final;

        tmscm bool_to_tmscm(bool b) final;

        tmscm int_to_tmscm(int i) final;

        tmscm long_to_tmscm(long l) final;

        tmscm double_to_tmscm(double i) final;

        tmscm string_to_tmscm(string s) final;

        tmscm symbol_to_tmscm(string s) final;

        tmscm tmscm_unspefied() final;

        tmscm eval_scheme_file(string name) final;

        tmscm eval_scheme(string s) final;

        static SCM _call_scheme_args(SCM fun, std::vector<SCM> args);

        tmscm call_scheme_args(tmscm fun, std::vector<tmscm> _args) final;

        void install_procedure(string name, std::function<tmscm(abstract_scheme *, tmscm)> fun, int numArgs, int numOptional) final;

        inline string scheme_dialect() {
            return "guile-c";
        }

        inline SCM *unsafe_objectstack() const {
            return mThread.unsafe_objectstack();
        }

    private:
        guile_thread mThread;

    };

    class Guile18Factory : public SchemeFactory {

    public:
        abstract_scheme *make_scheme() final;

        std::string name() final;

    };

    void registerGuile18();

}

#endif

