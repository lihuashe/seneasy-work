
#ifndef XH_H
#define XH_H

#if defined(_WIN32)         
    /* Do not include XH51 specific files when building for PC host */
#elif defined(__unix)       
    /* Do not include XH51 specific files when building for PC host */
#elif defined(__APPLE__)    
    /* Do not include XH51 specific files when building for PC host */
#else

    /* Family selection for family includes. */
    #if defined (XH51)
#include "XH51.h"
#include "XH51_bitfields.h"
#include "XH51_deprecated.h"
    #else
        #error "Device family must be defined. See xh.h."
    #endif /* XH51 */

    #include "compiler_abstraction.h"

#endif /* _WIN32 || __unix || __APPLE__ */

#endif /* XH_H */

