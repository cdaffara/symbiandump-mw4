/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef WTF_MathExtras_h
#define WTF_MathExtras_h

#include <math.h>
#include <time.h>

#if COMPILER(MSVC)

#include "kjs/operations.h"
#include "kjs/value.h"
#include <xmath.h>
#include <limits>

#if HAVE(FLOAT_H)
#include <float.h>
#endif

#endif

#ifndef M_PI
const double piDouble = 3.14159265358979323846;
const float piFloat = 3.14159265358979323846f;
#else
const double piDouble = M_PI;
const float piFloat = static_cast<float>(M_PI);
#endif

#ifndef M_PI_4
const double piOverFourDouble = 0.785398163397448309616;
const float piOverFourFloat = 0.785398163397448309616f;
#else
const double piOverFourDouble = M_PI_4;
const float piOverFourFloat = static_cast<float>(M_PI_4);
#endif

#if COMPILER(MSVC)

inline bool isinf(double num) { return !_finite(num) && !_isnan(num); }
inline bool isnan(double num) { return !!_isnan(num); }
inline long lround(double num) { return static_cast<long>(num > 0 ? num + 0.5 : ceil(num - 0.5)); }
inline long lroundf(float num) { return static_cast<long>(num > 0 ? num + 0.5f : ceilf(num - 0.5f)); }
inline double round(double num) { return num > 0 ? floor(num + 0.5) : ceil(num - 0.5); }
inline float roundf(float num) { return num > 0 ? floorf(num + 0.5f) : ceilf(num - 0.5f); }
inline bool signbit(double num) { return _copysign(1.0, num) < 0; }

inline double nextafter(double x, double y) { return _nextafter(x, y); }
inline float nextafterf(float x, float y) { return x > y ? x - FLT_EPSILON : x + FLT_EPSILON; }

inline double copysign(double x, double y) { return _copysign(x, y); }
inline int isfinite(double x) { return _finite(x); }

// Work around a bug in Win, where atan2(+-infinity, +-infinity) yields NaN instead of specific values.
inline double wtf_atan2(double x, double y)
{
    static double posInf = std::numeric_limits<double>::infinity();
    static double negInf = -std::numeric_limits<double>::infinity();

    double result = KJS::NaN;

    if (x == posInf && y == posInf)
        result = piOverFourDouble;
    else if (x == posInf && y == negInf)
        result = 3 * piOverFourDouble;
    else if (x == negInf && y == posInf)
        result = -piOverFourDouble;
    else if (x == negInf && y == negInf)
        result = -3 * piOverFourDouble;
    else
        result = ::atan2(x, y);

    return result;
}

// Work around a bug in the Microsoft CRT, where fmod(x, +-infinity) yields NaN instead of x.
inline double wtf_fmod(double x, double y) { return (!isinf(x) && isinf(y)) ? x : fmod(x, y); }

#define fmod(x, y) wtf_fmod(x, y)

#define atan2(x, y) wtf_atan2(x, y)

#if defined(_CRT_RAND_S)
// Initializes the random number generator.
inline void wtf_random_init()
{
    // No need to initialize for rand_s.
}

// Returns a pseudo-random number in the range [0, 1).
inline double wtf_random()
{
    unsigned u;
    rand_s(&u);

    return static_cast<double>(u) / (static_cast<double>(UINT_MAX) + 1.0);
}
#endif // _CRT_RAND_S

#else

// Initializes the random number generator.
inline void wtf_random_init()
{
    srand(static_cast<unsigned>(time(0)));
}

// Returns a pseudo-random number in the range [0, 1).
inline double wtf_random()
{
    return static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) + 1.0);
}

#endif // #if COMPILER(MSVC)

#if PLATFORM(SYMBIAN)

#define FLT_EPSILON 1.19209290E-07F /* b**(1-p) */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif  //  M_PI

#ifndef M_PI_4
#define M_PI_4 0.785398163397448309616
#endif  //  M_PI_4

#include <e32math.h>

#define ceilf ceil
#define fmodf fmod
#define floorf floor
inline long lround(double num) { return static_cast<long>(num > 0 ? num + 0.5 : ceil(num - 0.5)); }
inline float lroundf(float num)     { return num > 0 ? num + 0.5f : ceilf(num - 0.5f); }
inline float roundf(float num)      { return num > 0 ? floorf(num + 0.5f) : ceilf(num - 0.5f); }
inline int isfinite(double x) { return Math::IsFinite(x); }
inline bool signbit(double num) { return copysign(1.0, num) < 0; }
inline float nextafterf(float x, float y) { return x > y ? x - FLT_EPSILON : x + FLT_EPSILON; }

//tot:taken from mruntime
typedef union
{
  double value;
  struct
  {
    unsigned int msw;
    unsigned int lsw;
  } parts;
} ieee_double_shape_type;

#define EXTRACT_WORDS(ix0,ix1,d) \
do {                             \
  ieee_double_shape_type ew_u;   \
  ew_u.value = (d);              \
  (ix0) = ew_u.parts.msw;        \
  (ix1) = ew_u.parts.lsw;        \
} while (0)

#define INSERT_WORDS(d,ix0,ix1)  \
do {                             \
  ieee_double_shape_type iw_u;   \
  iw_u.parts.msw = (ix0);        \
  iw_u.parts.lsw = (ix1);        \
  (d) = iw_u.value;              \
} while (0)

inline double nextafter(double x, double y)
{
    volatile double t;
    int hx,hy,ix,iy;
    unsigned int lx,ly;

    EXTRACT_WORDS(hx,lx,x);
    EXTRACT_WORDS(hy,ly,y);
    ix = hx&0x7fffffff;     /* |x| */
    iy = hy&0x7fffffff;     /* |y| */

    if(((ix>=0x7ff00000)&&((ix-0x7ff00000)|lx)!=0) ||   /* x is nan */
       ((iy>=0x7ff00000)&&((iy-0x7ff00000)|ly)!=0))     /* y is nan */
       return x+y;
    if(x==y) return y;      /* x=y, return y */
    if((ix|lx)==0) {            /* x == 0 */
        INSERT_WORDS(x,hy&0x80000000,1);    /* return +-minsubnormal */
        t = x*x;
        if(t==x) return t; else return x;   /* raise underflow flag */
    }
    if(hx>=0) {             /* x > 0 */
        if(hx>hy||((hx==hy)&&(lx>ly))) {    /* x > y, x -= ulp */
        if(lx==0) hx -= 1;
        lx -= 1;
        } else {                /* x < y, x += ulp */
        lx += 1;
        if(lx==0) hx += 1;
        }
    } else {                /* x < 0 */
        if(hy>=0||hx>hy||((hx==hy)&&(lx>ly))){/* x < y, x -= ulp */
        if(lx==0) hx -= 1;
        lx -= 1;
        } else {                /* x > y, x += ulp */
        lx += 1;
        if(lx==0) hx += 1;
        }
    }
    hy = hx&0x7ff00000;
    if(hy>=0x7ff00000) return x+x;  /* overflow  */
    if(hy<0x00100000) {     /* underflow */
        t = x*x;
        if(t!=x) {      /* raise underflow flag */
            INSERT_WORDS(y,hx,lx);
        return y;
        }
    }
    INSERT_WORDS(x,hx,lx);
    return x;
}

inline float atan2f(float x, float y)
{
    return ((float)atan2((double)x, (double)y));
}

inline float cosf(float x)
{
    return ((float)cos((double)x));
}

inline float sinf(float x)
{
    return ((float)sin((double)x));
}


#endif // #if PLATFORM(SYMBIAN)

#endif // #ifndef WTF_MathExtras_h
