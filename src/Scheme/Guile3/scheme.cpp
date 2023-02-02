
#include "scheme.hpp"

SCM blackbox_tag;

SCM &guile_blackbox_tag() {
    return blackbox_tag;
}

void register_scheme_factory(SchemeFactory *factory);
void registerGuile30() {
    register_scheme_factory(new Guile30Factory);
}