#include "scheme.hpp"
#include "Glue/glue.hpp"

std::unique_ptr<abstract_scheme> global_scheme;

void notify_preference (string var) {
    return scheme().notify_preference(var);
}

void
initialize_scheme () {

    initialize_glue ();
    //object_stack = scm_lookup_string ("object-stack");

    // uncomment to have a guile repl available at startup
    //	gh_repl(guile_argc, guile_argv);
    //scm_shell (guile_argc, guile_argv);

}

