// fast fetch tid/pid ?
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <intrin.h>

void main(void)
{  
  DWORD tis__ = GetCurrentThreadId();
  DWORD tis_2 = 0;
  DWORD pis__ = GetCurrentProcessId();
  DWORD pis_2 = 0;
  
  __asm mov eax, dword ptr fs:[0x24]
  __asm mov dword ptr[tis_2], eax 
  __asm mov eax, dword ptr fs:[0x20]
  __asm mov dword ptr[pis_2], eax 
  
}