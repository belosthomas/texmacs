//
// Created by lizab on 28/03/2023.
//

#ifndef TEXMACS_PLATFORMDEPENDANT_HPP
#define TEXMACS_PLATFORMDEPENDANT_HPP

#include <stdint.h>

#ifndef WIN32
#include <sys/resource.h>
#else
#include <windows.h>
#endif

namespace texmacs {

    inline void setStackLimit(int64_t limit) {

        #ifndef WIN32
        struct rlimit rlim;
        getrlimit(RLIMIT_STACK, &rlim);
        rlim.rlim_cur = limit;
        setrlimit(RLIMIT_STACK, &rlim);
        #else
        ULONG dwStackSize = limit;
        SetThreadStackGuarantee(&dwStackSize);
        #endif

    }

}

#endif //TEXMACS_PLATFORMDEPENDANT_HPP
