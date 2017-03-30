// APC callback will suspend other waiting threads???
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <intrin.h>

int ws = 0;

unsigned int __stdcall ThreadProcNodone (void *param)
{
  while (ws == 0)
  {
  }
  return 0;
}
VOID CALLBACK APCProc(
  _In_ ULONG_PTR dwParam
  ){
          ws = 1;
    while (1)
    {
    }
  }
unsigned int __stdcall ThreadProcWait (/* void *param*/ HANDLE handle)
{
  printf ("result:%08X", WaitForSingleObjectEx (handle, INFINITE, TRUE));
  printf ("result:%08X", WaitForSingleObjectEx (handle, INFINITE, TRUE));
  return 0;
}
int main (void)
{
  HANDLE s[3] = { (HANDLE) -1, (HANDLE) -1, (HANDLE) -1 };
  s[0] = (HANDLE) _beginthreadex (NULL, 0, ThreadProcNodone, NULL, 0, NULL);
  if (s[0] == (HANDLE) -1)
  goto cleanup;
  s[1] = (HANDLE) _beginthreadex (NULL, 0, ThreadProcWait, s[0], 0, NULL);
  s[2] = (HANDLE) _beginthreadex (NULL, 0, ThreadProcWait, s[0], 0, NULL);
  if (s[1] == (HANDLE) -1 || s[2] == (HANDLE) -1)
    goto cleanup;
  else 
  {
      QueueUserAPC (APCProc, s[1], 0);

    while (1)
    {
    }
  }
cleanup:
  if ( s[0] != (HANDLE) -1) CloseHandle (s[0]);
  if ( s[1] != (HANDLE) -1) CloseHandle (s[1]);
  if ( s[2] != (HANDLE) -1) CloseHandle (s[2]);
    
    printf ("error to create thread");
    return -1;
}


