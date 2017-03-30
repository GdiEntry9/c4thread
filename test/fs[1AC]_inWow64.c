// each thread has a private fs:[1AC] block ??
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <intrin.h>

#define  GARRAY_MAX 32
#define  THREAD_MAX 16 
#define  WIN32SEM_MAXCOUNT 0x7FFFFFF
#define  WIN32SEM_INITCOUNT 0x7FF000

static INT32 G_Array[GARRAY_MAX] = {0};
static BOOL G_bSignalQuitThread = FALSE;
static HANDLE G_hSem = NULL;
static HANDLE G_hThread[THREAD_MAX] = {0};
static LONG fs1AC = 0;

int 
__stdcall
SyncCheckProc(void *pRaram) 
{
    __asm mov eax, fs:[0x1AC]
    __asm cmp eax, -3 
    __asm je __softInterrupt
    return 0;
    __softInterrupt:
    __asm int 3
}

void main(void)
{  
    //  Create Sem.
    G_hSem = CreateSemaphore(NULL, WIN32SEM_INITCOUNT, WIN32SEM_MAXCOUNT, NULL);
    if (G_hSem == NULL) {
        // CreateSem failed.   
        MessageBox(NULL, TEXT("CreateSem Failed."), TEXT("fatal-error"), MB_ICONERROR);
        return -1;    
    }  
    __asm mov dword ptr fs:[0x1AC], -3 
    
    //  Create Thread.
    {
        INT32 i = 0;
        for (i = 0; i != THREAD_MAX; i++) 
        {
            G_hThread[i] = _beginthreadex(NULL, 0, SyncCheckProc, NULL, 0, NULL);
            if (G_hThread[i] == NULL) {
                // _beginthreadex failed.   
                MessageBox(NULL, TEXT("CreateThread Failed."), TEXT("fatal-error"), MB_ICONERROR);
                goto __WaitThreadObj;
            }
        }
    }
    __asm mov eax, dword ptr fs:[0x1AC]
    __asm mov [fs1AC], eax 
    
    while ((GetAsyncKeyState('Q') & 0x8000) == 0)
    {
        Sleep(1); // Switch thread. reduce CPU call.
    }

    // Quit Testthread.
    __WaitThreadObj:
    {
        INT32 i = 0;
        
        // Set SiganlQuit.
        G_bSignalQuitThread = TRUE;
        
        for (i = 0; i != THREAD_MAX; i++) 
        {
            if (G_hThread[i]!= NULL) {
                // WaitMTObj.close handle.
                WaitForSingleObject(G_hThread[i], INFINITE);
                CloseHandle(G_hThread[i]);
            }
        }
    }
    
    // ReleaseWin32Sem.KernelObj.
    if (G_hSem != NULL)
    {
        CloseHandle(G_hSem);
    }
}