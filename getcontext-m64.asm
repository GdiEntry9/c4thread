;/*-
;* Copyright (c) 2016 moecmks
;* All rights reserved.
;*
;* Redistribution and use in source and binary forms, with or without
;* modification, are permitted provided that the following conditions
;* are met:
;* 1. Redistributions of source code must retain the above copyright
;*    notice, this list of conditions and the following disclaimer.
;* 2. Redistributions in binary form must reproduce the above copyright
;*    notice, this list of conditions and the following disclaimer in the
;*    documentation and/or other materials provided with the distribution.
;*
;* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
;* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;* ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
;* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
;* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
;* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
;* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
;* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
;* SUCH DAMAGE.
;*
;*/

.code

;***************************************************************
;* selfcontext_get-m64         
;***************************************************************
selfcontext_get proc C 
             option prologue:none, epilogue:none
   
  push  rdx ; rdx save.
  push  rax ; rax save.
  mov   rax, [rsp+0+16] ; get rip
;assume edx:ptr c4context_
  lea   rdx, [rsp+8+16] ; get rsp
  mov   [rcx+512+8*0], rbx ; store rbx
  mov   [rcx+512+8*1], rdx ; store rsp
  mov   [rcx+512+8*2], rsi ; store rsi 
  mov   [rcx+512+8*3], rdi ; store rdi 
  mov   [rcx+512+8*4], rax ; store rip
  lahf                     ; get psw.
  mov   [rcx+512+8*5],  ah ; store psw
  mov   [rcx+512+8*6], r12 ; store r12
  mov   [rcx+512+8*7], r13 ; store r13
  mov   [rcx+512+8*8], r14 ; store r14
  mov   [rcx+512+8*9], r15 ; store r15
  mov   [rcx+512+8*12], r8 ; store r8
  mov   [rcx+512+8*13], r9 ; store r9
  mov   [rcx+512+8*14], r10 ; store r10
  mov   [rcx+512+8*15], r11 ; store r11
  lahf                  ; get psw.
  mov   [edx+108+4*6], ah ; store psw.
  fsave [edx]        ; store current fpu context.
  pop   dword ptr[edx+108+4*7]; store eax.
  pop   dword ptr[edx+108+4*8]; store ecx.
  pop   dword ptr[edx+108+4*9]; store edx.
  ret 

selfcontext_get endp

;***************************************************************
;* selfcontext_set-m64  
;***************************************************************
selfcontext_set proc C 
             option prologue:none, epilogue:none
             
assume	fs:nothing   
  mov   edx, [esp+4] ; get c4context.
  mov   eax, [edx+108+4*2] ; get eip 
  mov   fs:[0700h], eax ; store to TIB's Reserved for user application
  frstor [edx]        ; set current fpu context.
  mov   esp, [edx+108+4*0] ; set esp
  mov   ebp, [edx+108+4*1] ; set ebp 
  mov   esi, [edx+108+4*3] ; set esi 
  mov   edi, [edx+108+4*4] ; set edi 
  mov   ebx, [edx+108+4*5] ; set ebx
  mov    ah, [edx+108+4*6] ; get psw
  sahf                     ; set psw 
  mov   eax, [edx+108+4*7] ; set eax
  mov   ecx, [edx+108+4*8] ; set ecx
  mov   edx, [edx+108+4*9] ; set edx
  jmp   dword ptr fs:[0700h] ; set eip. take off!!!

selfcontext_set endp

;***************************************************************
;* getcontext-m64         
;***************************************************************
c4thread_getcontext proc C 
             option prologue:none, epilogue:none
   
  push  edx ; edx save. 
  push  ecx ; ecx save. 
  push  eax ; eax save.
  mov   eax, [esp+0+12] ; get eip
;assume edx:ptr c4context_
  mov   edx, [esp+8+12] ; get c4context.
  lea   ecx, [esp+12+12] ; get esp
  mov   [edx+108+4*0], ecx ; store esp
  mov   [edx+108+4*1], ebp ; store ebp 
  mov   [edx+108+4*2], eax ; store eip 
  mov   [edx+108+4*3], esi ; store esi 
  mov   [edx+108+4*4], edi ; store edi 
  mov   [edx+108+4*5], ebx ; store ebx
  lahf                  ; get psw.
  mov   [edx+108+4*6], ah ; store psw.
  fsave [edx]        ; store current fpu context.
  pop   dword ptr[edx+108+4*7]; store eax.
  pop   dword ptr[edx+108+4*8]; store ecx.
  pop   dword ptr[edx+108+4*9]; store edx.
extrn   c4thread_getcontext2:proc
  jmp   c4thread_getcontext2 ; call main. 

c4thread_getcontext endp

    end