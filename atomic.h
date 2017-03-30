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
 
#if !defined (c4atomic_included_MOECMKS)
#define c4atomic_included_MOECMKS

#include <intrin.h>
#include <windows.h>

/*
 * If the processor is allowed on the x86,
 * it will be translated into pause instruction.
 */
#define cpu_relax _mm_pause 

/*
 * memory read/write/access barrier.
 */
#define _m_load_barrier _mm_lfence
#define _m_write_barrier _mm_sfence
#define _m_access_barrier _mm_mfence

/* TODO: cmpxchg16 on x64 */
#define _store8(p)        InterlockedExchange8(    (volatile CHAR *)(p), (CHAR)(v))
#define _gradual8(p,v)    InterlockedExchangeAdd8( (volatile CHAR *)(p), (CHAR)(v))
#define _cas8(p,v,c)      InterlockedCompareExchange8((volatile CHAR *)(p), (CHAR)(v), (CHAR)(c))
#define _or8(p,v)         InterlockedOr8((volatile CHAR *)(p),(CHAR)(v))
#define _xor8(p,v)        InterlockedXor8((volatile CHAR *)(p),(CHAR)(v))
#define _and8(p,v)        InterlockedAnd8((volatile CHAR *)(p),(CHAR)(v))
#define _dec8(p)          InterlockedExchangeAdd8((volatile CHAR *)(p),-1)
#define _inc8(p)          InterlockedExchangeAdd8((volatile CHAR *)(p),1)
#define _fetch8(p)        InterlockedExchangeAdd8((volatile CHAR *)(p),0)
#define _store16(p)      _InterlockedExchange16(    (volatile SHORT *)(p), (SHORT)(v))
#define _gradual16 (p,v) _InterlockedExchangeAdd16( (volatile SHORT *)(p), (SHORT)(v))
#define _cas16(p,v,c)    _InterlockedCompareExchange16((volatile SHORT *)(p), (SHORT)(v), (SHORT)(c))
#define _or16(p,v)        InterlockedOr16((volatile SHORT *)(p),(SHORT)(v))
#define _xor16(p,v)       InterlockedXor16((volatile SHORT *)(p),(SHORT)(v))
#define _and16(p,v)       InterlockedAnd16((volatile SHORT *)(p),(SHORT)(v))
#define _dec16(p)        _InterlockedExchangeAdd16((volatile SHORT *)(p),-1)
#define _inc16(p)        _InterlockedExchangeAdd16((volatile SHORT *)(p),1)
#define _fetch16(p)      _InterlockedExchangeAdd16((volatile SHORT *)(p),0)
#define _store32(p,v)     InterlockedExchange(    (volatile LONG *)(p), (LONG)(v))
#define _gradual32(p,v)   InterlockedExchangeAdd( (volatile LONG *)(p), (LONG)(v))
#define _cas32(p,v,c)     InterlockedCompareExchange((volatile LONG *)(p), (LONG)(v), (LONG)(c))
#define _or32(p,v)        InterlockedOr((volatile LONG *)(p),(LONG)(v))
#define _xor32(p,v)       InterlockedXor((volatile LONG *)(p),(LONG)(v))
#define _and32(p,v)       InterlockedAnd((volatile LONG *)(p),(LONG)(v))
#define _dec32(p)         InterlockedExchangeAdd( (volatile LONG *)(p), (LONG)(-1))
#define _inc32(p)         InterlockedExchangeAdd( (volatile LONG *)(p), (LONG)(1))
#define _fetch32(p)       InterlockedExchangeAdd( (volatile LONG *)(p), (LONG)(0))
#define _store64(p,v)     InterlockedExchange64(    (volatile LONGLONG *)(p), (LONGLONG)(v))
#define _gradual64(p,v)   InterlockedExchangeAdd64( (volatile LONGLONG *)(p), (LONGLONG)(v))
#define _cas64(p,v,c)     InterlockedCompareExchange64((volatile LONGLONG *)(p), (LONGLONG)(v), (LONGLONG)(c))
#define _or64(p,v)        InterlockedOr64((volatile LONG LONG*)(p),(LONG LONG)(v))
#define _xor64(p,v)       InterlockedXor64((volatile LONG LONG*)(p),(LONG LONG)(v))
#define _and64(p,v)       InterlockedAnd64((volatile LONG LONG*)(p),(LONG LONG)(v))
#define _dec64(p)         InterlockedExchangeAdd64( (volatile LONGLONG *)(p), (LONGLONG)(-1))
#define _inc64(p)         InterlockedExchangeAdd64( (volatile LONGLONG *)(p), (LONGLONG)(1))
#define _fetch64(p)       InterlockedExchangeAdd64( (volatile LONGLONG *)(p), (LONGLONG)(0))
#define _p_store(p,v)     InterlockedExchangePointer (    (volatile PVOID **)(p), (PVOID)(v))
#define _p_cas(p,v,c)     InterlockedCompareExchangePointer(    (volatile PVOID **)(p), (PVOID)(v), (PVOID)(c))

#endif