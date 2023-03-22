
#include "guile18_scheme.hpp"

SCM blackbox_tag;

SCM &texmacs::guile_blackbox_tag() {
    return blackbox_tag;
}

void texmacs::register_scheme_factory(SchemeFactory *factory);
void texmacs::registerGuile18() {
    register_scheme_factory(new Guile18Factory);
}