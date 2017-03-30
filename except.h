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
 
#if !defined (c4except_included_MOECMKS)
#define c4except_included_MOECMKS

#include "context.h"
#include <windows.h>

struct c4except_ {
  int32 except_code;
  int32 unused[3];
  struct c4context except_; /* context in which an exception occurs */
  struct c4context *repair_; /* null:continue, else:set context */
};

/*
 * except code
 */
#define ACCESS_VIOLATION EXCEPTION_ACCESS_VIOLATION
#define ARRAY_BOUNDS_EXCEEDED EXCEPTION_ARRAY_BOUNDS_EXCEEDED
/* #define BREAKPOINT EXCEPTION_BREAKPOINT */
#define DATATYPE_MISALIGNMENT EXCEPTION_DATATYPE_MISALIGNMENT
#define FLT_DENORMAL_OPERAND EXCEPTION_FLT_DENORMAL_OPERAND
#define FLT_DIVIDE_BY_ZERO EXCEPTION_FLT_DIVIDE_BY_ZERO
#define FLT_INEXACT_RESULT EXCEPTION_FLT_INEXACT_RESULT
#define FLT_INVALID_OPERATION EXCEPTION_FLT_INVALID_OPERATION
#define FLT_OVERFLOW EXCEPTION_FLT_OVERFLOW
#define FLT_STACK_CHECK EXCEPTION_FLT_STACK_CHECK
#define FLT_UNDERFLOW EXCEPTION_FLT_UNDERFLOW
/* #define GUARD_PAGE EXCEPTION_GUARD_PAGE */
#define ILLEGAL_INSTRUCTION EXCEPTION_ILLEGAL_INSTRUCTION
#define IN_PAGE_ERROR EXCEPTION_IN_PAGE_ERROR
#define INT_DIVIDE_BY_ZERO EXCEPTION_INT_DIVIDE_BY_ZERO
#define INT_OVERFLOW EXCEPTION_INT_OVERFLOW
/* #define INVALID_DISPOSITION EXCEPTION_INVALID_DISPOSITION */
/* #define INVALID_HANDLE EXCEPTION_INVALID_HANDLE */
#define NONCONTINUABLE_EXCEPTION EXCEPTION_NONCONTINUABLE_EXCEPTION
/* #define PRIV_INSTRUCTION EXCEPTION_PRIV_INSTRUCTION */
/* #define SINGLE_STEP EXCEPTION_SINGLE_STEP */
#define STACK_OVERFLOW EXCEPTION_STACK_OVERFLOW

/** I suggest you use MSVC or INTEL-XE 2015 to compile 
 */

#if defined (__INTEL_ICL_) || defined (__MSVC_CL_)
#  define __c4exception_base(expression) __except (expression)
#  define __c4constructor      __try
#  define __c4exception(filter) __except \
                (GetExceptionCode() == filter ? EXCEPTION_EXECUTE_HANDLER \
                              : EXCEPTION_CONTINUE_SEARCH)
#  define __c4force_catch       __except(EXCEPTION_EXECUTE_HANDLER)
#elif defined (__GNU_C_)
#  define __c4exception_base(expression) if (false) /** dummy */
#  define __c4constructor      if (true) /** dummy */
#  define __c4exception(filter) if (false) /** dummy */
#  define __c4force_catch      if (false) /** dummy */
#else 
#  error unsupported compiler!
#endif 

#endif