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
 
#if !defined (c4thread_included_MOECMKS)
#define c4thread_included_MOECMKS

#include "c4.h"

#if defined (__cplusplus)  /** __cplusplus */
extern "C" {
#endif  /** __cplusplus */

/*
 * Thread cancel state.
 */
enum C4THREAD_CANCEL_STATE {
  C4THREAD_CANCEL_ENABLE,
  C4THREAD_CANCEL_DISABLE
};

/*
 * Thread cancel type.
 */
enum C4THREAD_CANCEL_TYPE {
  C4THREAD_CANCEL_ASYNCHRONOUS,
  C4THREAD_CANCEL_DEFFERED
};

/*
 * c4thread_callback_.
 */
struct c4thread_callback_ {
  void *param;
  void *param_except_;
  void (c4callc *routine)(c4thread, void * );
  void (c4callc *except_)(c4thread, void *, struct c4except_ * );
};

/*
 * c4thread link
 */
struct c4thread_link {
  c4thread thread;
  struct c4thread_link *level;
};

c4impl c4error c4global_initialize (void);
c4impl c4error c4global_uninitialize (void);
/* c4impl */ void c4callc selfcontext_get (struct c4context *context); /* same as longjmp */
/* c4impl */ void c4callc selfcontext_set (struct c4context *context); /* same as setjmp */
c4impl c4error c4thread_nativehandle (c4thread thread, void **handle);
c4impl c4error c4thread_enumthread (struct c4thread_link **link);
c4impl c4error c4thread_getcontext (c4thread thread, struct c4context *context);
/* c4impl */  c4error c4thread_getcontext2 (c4thread thread, struct c4context *context);
c4impl c4error c4thread_setcontext (c4thread thread, struct c4context *context);
c4impl c4error c4thread_getaffinitie (c4thread thread, uint64 *mask);
c4impl c4error c4thread_setaffinitie (c4thread thread, uint64 mask);
c4impl c4error c4thread_getaffinitie_Z (uint64 *mask);
c4impl c4error c4thread_GetStartup (c4thread *thread);
c4impl c4error c4thread_startup (c4thread thread, uint32 *count);
c4impl c4error c4thread_suspend (c4thread thread, uint32 *count);
c4impl c4error c4thread_exit (c4error exitcode);
c4impl c4error c4thread_testcancel (void);
// c4impl c4error c4thread_asyncio (struct c4thread_callback_ *procedure, struct c4thread_callback_ *callback, lean32 cbcall_byself);
c4impl c4error c4thread_cleanup_push (struct c4thread_callback_ *procedure);
c4impl c4error c4thread_cleanup_pop (int32 level);
c4impl c4error c4thread_cancel (c4thread thread);
c4impl c4error c4thread_setcancelstate (c4thread thread, enum C4THREAD_CANCEL_STATE cs);
c4impl c4error c4thread_setcanceltype (c4thread thread, enum C4THREAD_CANCEL_TYPE ct);
c4impl c4error c4thread_setdetach (c4thread thread);
c4impl c4error c4thread_setjoin (c4thread thread); 
c4impl c4error c4thread_timedjoin (c4thread thread, uint32 time); /* cancel-point */
c4impl c4error c4thread_join (c4thread thread); /* cancel-point */
c4impl c4error c4thread_current (c4thread *thread);
c4impl c4error c4thread_currentid (c4tid *tid);
c4impl c4error c4thread_create (c4thread *thread, lean32 interrupt, int32 stacksize, c4tid *tid,
                   struct c4thread_callback_ *procedure
);

#if defined (__cplusplus)  /** __cplusplus */
}
#endif  /** __cplusplus */

#endif