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
 
#if !defined (c4error_included_MOECMKS)
#define c4error_included_MOECMKS

#include "attribute.h"

#define C4_FAIL 0xFFFFFFFF
#define C4_SFAIL 0xFFFFFFFE
#define C4_OK 0
#define C4_INVAL 1
#define C4_NOMEM 2
#define C4_BADMEM 3
#define C4_BUSY 4
#define C4_TIMEOUT 5
#define C4_DEADLK 6
#define C4_PERM 7
#define C4_NEQUAL 8
#define C4_KFAIL 9
#define C4_OVERFLOW 10 
#define C4_NOSYS 11 
#define C4_LERR 12 
#define C4_IRREV 13
#define C4_IPARAM 14 
#define C4_PARAM 15
#define C4_DESTROY 16
#define C4_NONE 17 
#define C4_INTER 18 
#define C4_CPARAM 19
#define C4_INTR 20 
#define C4_UNKWN 21 

#define c4error int32

#define C4SET_ERROR(v_dummy_symbols, v2_dummy_symbols) \
                    (v_dummy_symbols)= (v2_dummy_symbols)

#endif