//// Test win32sem init status.max value is 0.

#include <windows.h>

void main(void)
{
  HANDLE hSem = CreateSemaphore(NULL, 0, 0, NULL);
  if (hSem == NULL) {
    // result EAX?=?00000057[dec 87] from VS2012.
    // ---- MSDN result
    // 87 (0x57) ERROR_INVALID_PARAMETER
    // The parameter is incorrect.
    // ---- reference
    // see https://msdn.microsoft.com/en-us/library/ms681381(v=vs.85).aspx
    // see https://msdn.microsoft.com/en-us/library/ms681382(v=vs.85).aspx

    DWORD dLastError = GetLastError();   
  }
}