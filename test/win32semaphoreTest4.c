// release-sem test for MSVC.
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <intrin.h>

void main(void)
{
  HANDLE hSem = CreateSemaphore(NULL, 0, 1, NULL);
  if (hSem == NULL) {
    // CreateSem failed.
    MessageBox(NULL, TEXT("CreateSem Failed."), TEXT("fatal-error"), MB_ICONERROR);
    return ;    
  }
  // exec overflow 
  {
    LONG lOld=0;
    
    if (ReleaseSemaphore(hSem, -1, &lOld) == FALSE)
    {
      // Print the value of lOld
      printf("lOld Value:%d", lOld);
    }
  }
  // destroy.
  if (hSem != NULL)
  {
    CloseHandle(hSem);
  }
}