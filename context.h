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
 
#if !defined (c4context_included_MOECMKS)
#define c4context_included_MOECMKS

/*
 * !!! Only applicable to Windows platform
 * CPU architecture knowledge, please refer to the IA manual
 */
#include "attribute.h"

/* 
 * we always run code on protect mode. isn't it ? 
 * Intel® 64 and IA-32 Architectures Software Developer’s Manual::Volume 1::CHAPTER 8
 * 8.1.10 Saving the x87 FPU’s State with FSTENV/FNSTENV and FSAVE/FNSAVE
 */
struct fpu_387pmode {
  int32 cw;
  int32 sw;
  int32 tw;
  int32 ip;
  int32 ips_10_0opcode;
  int32 dp;
  int32 ds;
  int8 reg[80]; /* R0~R7 */
};

/* 
 * Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 2A:Instruction Set Reference, A-M
 * ::CHAPTER 3::FXSAVE—Save x87 FPU, MMX Technology, and SSE State
 * Table 3-53. Non-64-bit-Mode Layout of FXSAVE and FXRSTOR Memory Region
 */
struct fpu_wow64 {
  int16 _387cw;
  int16 _387sw;
  int8 _387tw;
  int8 reserved_5;
  int16 _387op;
  int32 _387ip;
  int16 _387cs;
  int16 reserved_14_15;
  int32 _387dp; 
  int16 _387ds;
  int16 reserved_6_7_onbase16byte;
  int32 _sse_csr;
  int32 _sse_csr_mask;
  int32 _mmx387_reg[32];
  int32 _sse_reg[32];
  int32 reserved_288[12];
  int32 available_464[12];
};

/* 
 * Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 2A:Instruction Set Reference, A-M
 * ::CHAPTER 3::FXSAVE—Save x87 FPU, MMX Technology, and SSE State
 * Table 3-56. Layout of the 64-bit-mode FXSAVE64 Map (requires REX.W = 1)
 */
struct fpu_x64 {
  int16 _387cw;
  int16 _387sw;
  int8 _387tw;
  int8 reserved_5;
  int16 _387op;
  int64 _387ip;
  int64 _387dp;
  int32 _sse_csr;
  int32 _sse_csr_mask;
  int32 _mmx387_reg[32];
  int32 _sse_reg[64];
  int32 reserved_416[12];
  int32 available_464[12];
};

/* normal register collect, only use base */
struct int_386r {
  int32 esp;
  int32 ebp;
  int32 eip;
  int32 esi;
  int32 edi;
  int32 ebx;
  int32 psw;
  int32 eax; /* scratch */ 
  int32 ecx; /* scratch */ 
  int32 edx; /* scratch */ 
};

/*
 * due to x64 function call, we are not very good at handling some registers 
 * When calling c4thread_getcontext with its own thread, ignore rcx, rdx
 */
struct int_x64r {
  int64 rbx;
  int64 rsp;
  int64 rbp; /* use rbp in debug mode. */
  int64 rsi;
  int64 rdi;
  int64 rip;
  int32 psw;
  int64 r12;
  int64 r13;
  int64 r14;
  int64 r15;
  int64 rax; /* scratch */ 
  int64 rcx; /* scratch */ 
  int64 rdx; /* scratch */ 
  int64 r8; /* scratch */ 
  int64 r9; /* scratch */ 
  int64 r10; /* scratch */ 
  int64 r11; /* scratch */ 
};
// RCX, RDX, R8, and R9
struct c4context {
  
 /* 
  * !!! missing protection for xmm registers on x86-32 
  * !!! missing protection for avx registers on x86-64
  *
  * the same applies to mingw. Please note.
  *
  * also see:
  *   https://msdn.microsoft.com/en-us/library/ms235286.aspx
  *   http://stackoverflow.com/questions/7902903/gcc-x64-function-calling
  */
# if defined (__X86_32_)
  struct fpu_387pmode fpu;
# elif defined (__X86_WOW64_)
  struct fpu_wow64 fpu;
# elif defined (__X86_64_)
  struct fpu_x64 fpu;
# endif 

# if defined (__X86_32_) || defined (__X86_WOW64_)
  struct int_386r cpu;
# elif defined (__X86_64_)
  struct int_x64r cpu;
# endif
};

#endif