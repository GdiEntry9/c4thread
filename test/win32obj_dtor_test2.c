// win32obj dtor test2
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <intrin.h>

static HANDLE evtGate_ = NULL;
static HANDLE mtxTest_ = NULL;
static HANDLE cThreadHandle = NULL;

int __cdecl ThreadProc(void *pRaram) 
{
    WaitForSingleObject (mtxTest_, INFINITE);
    SetEvent (evtGate_);
    MessageBoxA (NULL, "RETURN","RETURN", MB_ICONERROR);
    return 0;
}

void main(void)
{  
    evtGate_ = CreateEvent (NULL, FALSE, FALSE, NULL);
    mtxTest_ = CreateMutex (NULL, TRUE, NULL);
    
    if ( evtGate_ && mtxTest_ ) {
      
     if ( - 1 == (uintptr_t)(cThreadHandle = (HANDLE) _beginthread (ThreadProc, 0, NULL) )) {
       goto ll;
     }
     Sleep (3000);
     SuspendThread (cThreadHandle);
       ReleaseMutex(mtxTest_); CloseHandle (mtxTest_);mtxTest_ = NULL;
       ResumeThread (cThreadHandle);
      WaitForSingleObject (evtGate_, INFINITE);
      goto ll;
      
    } else {
      ll:
        if ( evtGate_ ) CloseHandle ( evtGate_);
        if ( mtxTest_ ) CloseHandle ( mtxTest_);
          MessageBoxA(NULL, "ERROR", "ERROR", MB_ICONERROR);
          return ;
    }
}