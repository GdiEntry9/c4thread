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
 
#if !defined (c4condition_included_MOECMKS)
#define c4condition_included_MOECMKS

#include "c4.h"

/*
 * source not mine.
 * see Algorithm 8c in pthread-win32's README.CV
 * see linuxthreads 's condvar.c
 *
 * algorithm reference
 *    :http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
 *    :http://groups.google.com/group/comp.programming.threads/browse_frm/thread/1692bdec8040ba40/e7a5f9d40e86503a
 *    :https://www.microsoft.com/en-us/research/publication/implementing-condition-variables-with-conditions/
 *    :http://birrell.org/andrew/papers/ImplementingCVs.pdf
 */

#if defined (__cplusplus)  /** __cplusplus */
extern "C" {
#endif  /** __cplusplus */

c4impl c4error c4condition_signal (c4condition condition);
c4impl c4error c4condition_broadcast (c4condition condition);
c4impl c4error c4condition_wait (c4condition condition, c4mutex ind_lock);
c4impl c4error c4condition_timedwait (c4condition condition, c4mutex ind_lock, uint32 time);
c4impl c4error c4condition_destroy (c4condition condition);
c4impl c4error c4condition_init (c4condition *condition);

#if defined (__cplusplus)  /** __cplusplus */
}
#endif  /** __cplusplus */

#endif