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
 
#if !defined (c4setup_included_MOECMKS)
#define c4setup_included_MOECMKS

/*
 * !!! lack of SEH support for GCC.
 */
#if defined (_MSC_VER)
#  define __MSVC_CL_
#elif defined (__ICC) || defined (__INTEL_COMPILER)
#  define __INTEL_ICL_
#elif defined (__GNUC__) || defined (__GNUG__)
#  define __GNU_C_
#else 
#  error unsupported compiler
#endif 

/*
 * WOW64 program, need to manually add _WOW64 macros
 */
#if defined (_WIN64)
#  define __X86_64_
#elif defined (_WOW64)
#  define __X86_WOW64_
#elif defined (_WIN32) || defined (WIN32)
#  define __X86_32_
#elif defined (__linux) || defined (__linux__)
#  error temporarily not supported
#else 
#  error only be compiled under windows and linux
#endif 

/*
 * assert debug
 */
#if defined (DEBUG) || defined (_DEBUG)
#  define __C4_DEBUG_
#endif

/*
 * library usage
 */
/* #define __C4_INLINE_COMPILE */
/* #define __C4_STATIC_LIBRARAY_EXPORT */
#define __C4_DYNAMIC_LIBRARAY_EXPORT
/* #define __C4_STATIC_LIBRARAY_IMPORT */
/* #define __C4_DYNAMIC_LIBRARAY_IMPORT*/

#endif