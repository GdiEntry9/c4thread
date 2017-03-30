// Is the handle in thread wait a manual event or an automatic event?
// we will startup 3 new threads, test WaitForSingleObject.
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <intrin.h>

unsigned int __stdcall ThreadProcNodone (void *param)
{
  // nodone 
  return 0;
}
unsigned int __stdcall ThreadProcWait (/* void *param*/ HANDLE handle)
{
  printf ("result:%08X", WaitForSingleObject (handle, INFINITE));
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
    while (1)
    {
    }
cleanup:
  if ( s[0] != (HANDLE) -1) CloseHandle (s[0]);
  if ( s[1] != (HANDLE) -1) CloseHandle (s[1]);
  if ( s[2] != (HANDLE) -1) CloseHandle (s[2]);
    
    printf ("error to create thread");
    return -1;
}


