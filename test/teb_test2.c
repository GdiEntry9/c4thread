// fast fetch tid/pid ? [only intel's icl]
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <intrin.h>

void main(void)
{  
  DWORD64 tis__ = GetCurrentThreadId();
  DWORD64 tis_2 = 0;
  DWORD64 pis__ = GetCurrentProcessId();
  DWORD64 pis_2 = 0;
  
  __asm mov rax, dword ptr fs:[0x48]
  __asm mov dword ptr[tis_2], rax 
  __asm mov rax, dword ptr fs:[0x40]
  __asm mov dword ptr[pis_2], rax 
  
}