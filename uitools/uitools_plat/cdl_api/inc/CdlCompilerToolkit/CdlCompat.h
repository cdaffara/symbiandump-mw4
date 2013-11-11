// Cdlcompat.h

#ifndef CDLCOMPAT_H
#define CDLCOMPAT_H

/* _MSC_VER for checking VC6, so that we can keep this library still compilable using `tools'. */
#if (defined(__MSVCRT__) || defined(_MSC_VER))
#define CDL_W32
#endif

// path separator
/* #ifdef CDL_W32 */
/* #define PATHSEP "\\" */
/* #define PATHSEPCHAR '\\' */
/* #else */
#define PATHSEP "/"
#define PATHSEPCHAR '/'
/* #endif */

const char FORWARDSLASH = '/';
const char BACKSLASH = '\\';

#endif
