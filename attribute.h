/*-
 * Copyright (c) 2016 moecmks
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
 
#if !defined (c4attribute_included_MOECMKS)
#define c4attribute_included_MOECMKS

#if defined (__cplusplus)  /** __cplusplus */
extern "C" {
#endif  /** __cplusplus */

#include "setup.h"
#include <assert.h>

#if !defined (__cplusplus)
#  ifndef null_DEFINED
#    define null ((void *)0)
#  endif
#else 
#  ifndef null_DEFINED
#    define null 0
#  endif
#endif

#if defined (__MSVC_CL_) || defined (__INTEL_ICL_) /* MSVC/ICC starting... */
#  include "external/C99/stdint.h"
#  include "external/C99/stdbool.h"

#  define c4finline static  __forceinline
#  define c4dinline         __declspec(noinline)
#  define c4callstd         __stdcall
#  define c4callc           __cdecl
#  define c4tls             __declspec(thread)
#  define c4align(x)        __declspec(align(x))
#elif defined (__GNU_C_) /* MSVC/ICC end... GNUC starting */
#  include <stdint.h>
#  include <stdbool.h>

#  define c4finline static            __attribute__((always_inline))
#  define c4dinline                   __attribute__((noinline))
#  define c4callstd                   __attribute__((stdcall))
#  define c4callc                     __attribute__((cdecl))
#  define c4tls                       __thread
#  define c4align(x)                  __attribute__((aligned(x)))
#else /* unsupported */
#  error unsupported compiler! 
#endif 

#if defined (__MSVC_CL_) || defined (__INTEL_ICL_) /* MSVC/ICC starting... */
#  include "external/memio/malloc.h" /* in visual studio, use DLmalloc */

#  define c4malloc dlmalloc 
#  define c4free   dlfree 
#  define c4calloc dlcalloc 
#  define c4memalign dlmemalign
#elif defined (__GNU_C_) /* MSVC/ICC end... GNUC starting */
#  include <malloc.h> /* in GNUC, use default malloc */

#  define c4malloc malloc  
#  define c4free free 
#  define c4calloc calloc 
#  define c4memalign memalign
#else /* unsupported */
#  error unsupported compiler! 
#endif 

#if !defined (TSDC75_DEFINED)
#  define uint8 uint8_t /* unsigned BYTE */
#  define uint16 uint16_t /* unsigned    WORD */
#  define uint32 uint32_t /* unsigned   DWORD */
#  define uint64 uint64_t /* unsigned  QDWORD */
#  define uintptr uintptr_t /*unsigned  POINTER */
#  define int8 int8_t /* signed  BYTE */
#  define int16 int16_t /* signed   WORD */
#  define int32 int32_t /* signed  DWORD */
#  define int64 int64_t /* signed QDWORD */
#  define intptr intptr_t /* signed POINTER */
#endif 

/* fixed length bool. */ 
#define lean32 int32 

#ifndef FALSE 
#  define FALSE 0
#endif 

#ifndef TRUE 
#  define TRUE 1
#endif 
 
#if defined (__C4_DYNAMIC_LIBRARAY_EXPORT) /* #1 export dynamic library */
#     if defined (__MSVC_CL_) || defined (__INTEL_ICL_) /* MSVC/ICC starting... */
#         define c4impl     __declspec(dllexport)          
#     elif defined (__GNU_C_) /* MSVC/ICC end... GNUC starting */
#         define c4impl      __attribute__((dllexport))        
#     else 
#     error unsupported compiler! 
#     endif 
#elif defined (__C4_DYNAMIC_LIBRARAY_IMPORT) /* #2 import dynamic library */
#     if defined (__MSVC_CL_) || defined (__INTEL_ICL_) /* MSVC/ICC starting... */
#         define c4impl     __declspec(dllimport)         
#     elif defined (__GNU_C_) /* MSVC/ICC end... GNUC starting */
#         define c4impl     __attribute__((dllimport))          
#     else 
#     error unsupported compiler! 
#     endif 
#elif defined (__C4_INLINE_COMPILE) || defined (__C4_STATIC_LIBRARAY_IMPORT) || defined (__C4_STATIC_LIBRARAY_EXPORT)
#     if defined (__MSVC_CL_) || defined (__INTEL_ICL_) /* MSVC/ICC starting... */
#         define c4impl          
#     elif defined (__GNU_C_) /* MSVC/ICC end... GNUC starting */
#         define c4impl             
#     else 
#     error unsupported compiler! 
#     endif 
#else
#  error please specify the form of the library
#endif
 
#if defined (__cplusplus)  /** __cplusplus */
}
#endif  /** __cplusplus */

#endif