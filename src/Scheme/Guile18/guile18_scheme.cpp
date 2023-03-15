
#include "guile18_scheme.hpp"

SCM blackbox_tag;

guile_thread *guile_thread::guile_init_self;
std::mutex guile_thread::guile_init_mutex;

SCM &guile_blackbox_tag() {
    return blackbox_tag;
}

void register_scheme_factory(SchemeFactory *factory);
void registerGuile18() {
    register_scheme_factory(new Guile18Factory);
}