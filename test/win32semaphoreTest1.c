//// Test sem overflow's action. 

#include <windows.h>

int main(void)
{
  HANDLE hSem = CreateSemaphore(NULL, 0, 7, NULL);
  if (hSem == NULL) {
    // CreateSem failed.
    MessageBox(NULL, TEXT("CreateSem Failed."), TEXT("fatal-error"), MB_ICONERROR);
    return -1;    
  }
  // exec overflow 
  {
    LONG lOld=0;
    
    if (ReleaseSemaphore(hSem, 9, &lOld) == FALSE)
    {
      // failed. as you wish.
      // result EAX?=?0000012A[dec 298] from VS2012.
      // ---- MSDN result
      // 298 (0x12A) ERROR_TOO_MANY_POSTS
      // Too many posts were made to a semaphore.
      // ---- reference
      // see https://msdn.microsoft.com/en-us/library/ms681381(v=vs.85).aspx
      // see https://msdn.microsoft.com/en-us/library/ms681382(v=vs.85).aspx

      DWORD dLastError = GetLastError();
    }
  }
  // destroy.
  if (hSem != NULL)
  {
    CloseHandle(hSem);
  }
  return 0;
}