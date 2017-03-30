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
 
#if !defined (c4coroutine_included_MOECMKS)
#define c4coroutine_included_MOECMKS

#include "c4.h"

#if defined (__cplusplus)  /** __cplusplus */
extern "C" {
#endif  /** __cplusplus */

/*
 * c4routine is parallel coroutine.(but also supports nesting)
 * It is dangerous to create another coroutine within parent-coroutine.
 * this will make the logic and analysis very confusing.
 * c4coroutine_kill recursively delete all the child-coroutine of the current coroutine, 
 * which is easy to access illegally, It is very dangerous. Please bear in mind
 */
#if defined (__X86_32_) || defined (__X86_WOW64_)
#  define C4COroutine_EOF_MAGIC_HEAD1 (~0x5F375A86)
#  define C4COroutine_EOF_MAGIC_HEAD2 (~0x5F3759DF)
#if defined (__X86_64_)
#  define C4COroutine_EOF_MAGIC_HEAD1 (~0x5FE6EC85E7DE30DA)
#  define C4COroutine_EOF_MAGIC_HEAD2 (~0x5FE6EB50C7B537AA)
#endif 

/*
 * To maintain the order and predictability of the call parameters, 
 * use a variable parameter macro to declare coroutine
 */
#define c4coroutine_statement(coroutine,...) \
   c4error c4callc coroutine(__VA_ARGS__,...)
   
/*
 * We insert the end tag at the end of the call, 
 * partition custom parameter interval.
 * this is especially useful in some platforms that do 
 * not support the ebp/esp hybrid maintenance stack environment
 */ 
#define c4coroutine_create( /* c4coroutine **/ coroutine, \
                             /* lean32 */ care_fpu,          \
                             /* void **/ routine,...)       \
c4coroutine_create_unused ( (c4coroutine *) (coroutine),  \
                             (lean32) (care_fpu),            \
                             (void *) (routine), __VA_ARGS__, C4COroutine_EOF_MAGIC_HEAD1, C4COroutine_EOF_MAGIC_HEAD2 \
						               )
/*
 * it is important to ask the compiler to allow parameter floating points to follow the calling rules of the integer parameters.
 * Otherwise, the whole coroutine can not work, please confirm this before use
 */
c4impl c4error c4callc c4coroutine_create_unused (c4coroutine *coroutine, lean32 care_fpu, void *routine, ...);
c4impl c4error c4callc c4coroutine_exec (c4coroutine coroutine);
c4impl c4error c4callc c4coroutine_status (c4coroutine coroutine, lean32 *is_running);
c4impl c4error c4callc c4coroutine_kill (c4coroutine coroutine);
c4impl c4error c4callc c4coroutine_host (c4coroutine coroutine, c4thread *thread);
c4impl c4error c4callc c4coroutine_hostid (c4coroutine coroutine, c4tid *tid);
c4impl c4error c4callc c4coroutine_current (c4coroutine *coroutine);
c4impl c4error c4callc c4coroutine_nums (uint32 *nums);
c4impl c4error c4callc c4coroutine_link (void **link); /* link element idx0:level, idx1:coroutine (all void *)*/
c4impl c4error c4callc c4coroutine_spinlock (c4spin *spin);
c4impl c4error c4callc c4coroutine_nums_ex (c4tid tid, uint32 *nums);
c4impl c4error c4callc c4coroutine_link_ex (c4tid tid, void **link); /* link element idx0:level, idx1:coroutine (all void *)*/
c4impl c4error c4callc c4coroutine_spinlock_ex (c4tid tid, c4spin *spin);

#if defined (__cplusplus)  /** __cplusplus */
}
#endif  /** __cplusplus */

#endif