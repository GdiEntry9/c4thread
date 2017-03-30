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
 
#if !defined (c4mcs_spin_included_MOECMKS)
#define c4mcs_spin_included_MOECMKS

/*
 * !!! mcs-mcs_spin may not suitable for use on a single core machine.
 *
 * not mine, see:
 *
 * https://www.quora.com/How-does-an-MCS-lock-work
 * http://www.ituring.com.cn/article/42394
 * http://www.ibm.com/developerworks/cn/linux/l-cn-mcsspinlock/
 */
#include "c4.h"

#if defined (__cplusplus)  /** __cplusplus */
extern "C" {
#endif  /** __cplusplus */

struct c4mcs_spin_node_ {
  struct c4mcs_spin_node_ *level;
  uint64 recursion_depth;
  uintptr locked; 
}; 

c4impl c4error c4mcs_spin_unlock (c4mcs_spin mcs_spin);
c4impl c4error c4mcs_spin_unlock_root (c4mcs_spin mcs_spin);
c4impl c4error c4mcs_spin_destroy (c4mcs_spin mcs_spin);
c4impl c4error c4mcs_spin_lock (c4mcs_spin mcs_spin);
c4impl c4error c4mcs_spin_trylock (c4mcs_spin mcs_spin);
c4impl c4error c4mcs_spin_getowner (c4mcs_spin mcs_spin, c4tid *tid);
c4impl c4error c4mcs_spin_init (c4mcs_spin *mcs_spin, lean32 recursive);
c4impl c4error c4mcs_spin_is_recursive (c4mcs_spin mcs_spin, lean32 *recursive);
c4impl c4error c4mcs_spin_is_vailed (c4mcs_spin mcs_spin);

#if defined (__cplusplus)  /** __cplusplus */
}
#endif  /** __cplusplus */

#endif