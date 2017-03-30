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
 
#if !defined (c4objptr_included_MOECMKS)
#define c4objptr_included_MOECMKS

#include "c4.h"

/*
 * code/decode table.
 */
#if defined (__cplusplus)  /* __cplusplus */
extern "C" intptr obja16[16];
#else 
extern intptr obja16[16];
#endif  /* __cplusplus */

/* 
 * get objptr.
 */
c4finline void *
c4setop (void *address) {
  
  intptr idx = ( ( ( (intptr) address) - 13) % 16) & 15;
  intptr dce = obja16[idx] ^ (uintptr)address;
  
  return (void *)((dce & -16) | idx);
}

/* 
 * set objptr.
 */
c4finline void *
c4getop (void *address) {
  
  intptr idx = 15 & (intptr) address;
  intptr dce = obja16[idx] ^ (intptr) address;
  
  return (void *)(dce & -16);
}

#if defined (__cplusplus)  /* __cplusplus */
}
#endif   /* __cplusplus */

#endif