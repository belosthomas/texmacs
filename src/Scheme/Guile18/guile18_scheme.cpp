
#include "guile18_scheme.hpp"
#include <QDebug>
#include <iostream>

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

void guile_error(const char *message) {
    // throw a C++ exception
    throw std::runtime_error(message);
}

void guile_log_function(const char *cmsg, int len) {
    std::string msg(cmsg, len);
    std::cout << msg;
}