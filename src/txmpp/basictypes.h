/*
 * libjingle
 * Copyright 2004--2005, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products 
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TXMPP_BASICTYPES_H_
#define _TXMPP_BASICTYPES_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "constructormagic.h"

#ifndef INT_TYPES_DEFINED
#define INT_TYPES_DEFINED
#ifdef COMPILER_MSVC
typedef __int64 int64;
#else
typedef long long int64;
#endif /* COMPILER_MSVC */
#if defined(__GNUC__) && defined(__LP64__)
// __LP64__ means that long is 64 bits and int is 32 bits, so we must use int
// for int32.
typedef int int32;
#else
// On our other platforms int is still 32 bits, but int32 is canonically defined
// as long (also 32 bits), so we do that to avoid conflicts with system headers.
typedef long int32;
#endif
typedef short int16;
typedef char int8;

#ifdef COMPILER_MSVC
typedef unsigned __int64 uint64;
typedef __int64 int64;
#define INT64_C(x) x ## I64
#define UINT64_C(x) x ## UI64
#define INT64_F "I64"
#else
typedef unsigned long long uint64;
typedef long long int64;
#define INT64_C(x) x ## LL
#define UINT64_C(x) x ## ULL
#define INT64_F "ll"
#endif /* COMPILER_MSVC */
#if defined(__GNUC__) && defined(__LP64__)
// See comment above on definition of int32.
typedef unsigned int uint32;
#else
// See comment above on definition of int32.
typedef unsigned long uint32;
#endif
typedef unsigned short uint16;
typedef unsigned char uint8;
#endif  // INT_TYPES_DEFINED

#ifdef WIN32
typedef int socklen_t;
#endif

namespace txmpp {
  template<class T> inline T _min(T a, T b) { return (a > b) ? b : a; }
  template<class T> inline T _max(T a, T b) { return (a < b) ? b : a; }

  // For wait functions that take a number of milliseconds, kForever indicates
  // unlimited time.
  const int kForever = -1;
}

#ifdef WIN32
#define alignof(t) __alignof(t)
#else  // !WIN32
#define alignof(t) __alignof__(t)
#endif  // !WIN32
#define IS_ALIGNED(p, t) (0==(reinterpret_cast<uintptr_t>(p) & (alignof(t)-1)))

#ifndef UNUSED
#define UNUSED(x) Unused(static_cast<const void *>(&x))
#define UNUSED2(x,y) Unused(static_cast<const void *>(&x)); Unused(static_cast<const void *>(&y))
#define UNUSED3(x,y,z) Unused(static_cast<const void *>(&x)); Unused(static_cast<const void *>(&y)); Unused(static_cast<const void *>(&z))
#define UNUSED4(x,y,z,a) Unused(static_cast<const void *>(&x)); Unused(static_cast<const void *>(&y)); Unused(static_cast<const void *>(&z)); Unused(static_cast<const void *>(&a))
#define UNUSED5(x,y,z,a,b) Unused(static_cast<const void *>(&x)); Unused(static_cast<const void *>(&y)); Unused(static_cast<const void *>(&z)); Unused(static_cast<const void *>(&a)); Unused(static_cast<const void *>(&b))
inline void Unused(const void *) { }
#endif // UNUSED

#if defined(__GNUC__)
#define GCC_ATTR(x) __attribute__ ((x))
#else  // !__GNUC__
#define GCC_ATTR(x)
#endif  // !__GNUC__

#endif  // _TXMPP_BASICTYPES_H_
