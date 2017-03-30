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
 
#if !defined (c4mutex_included_MOECMKS)
#define c4mutex_included_MOECMKS

#include "c4.h"

#if defined (__cplusplus)  /** __cplusplus */
extern "C" {
#endif  /** __cplusplus */

c4impl c4error c4mutex_unlock (c4mutex mutex);
c4impl c4error c4mutex_unlock_root (c4mutex mutex);
c4impl c4error c4mutex_destroy (c4mutex mutex);
c4impl c4error c4mutex_lock (c4mutex mutex);
c4impl c4error c4mutex_trylock (c4mutex mutex);
c4impl c4error c4mutex_timedlock (c4mutex mutex, uint32 time);
c4impl c4error c4mutex_getowner (c4mutex mutex, c4tid *tid);
c4impl c4error c4mutex_init (c4mutex *mutex, lean32 recursive);
c4impl c4error c4mutex_is_recursive (c4mutex mutex, lean32 *recursive);
c4impl c4error c4mutex_is_vailed (c4mutex mutex);

#if defined (__cplusplus)  /** __cplusplus */
}
#endif  /** __cplusplus */

#endif