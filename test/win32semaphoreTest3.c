// sync test1 for MSVC.
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

int 
__stdcall
SyncCheckProc(void *pRaram) 
{
    while (G_bSignalQuitThread == FALSE) 
    {
        LONG i = 0;
        WaitForSingleObject(G_hSem, INFINITE);
        
        for (i = 0; i != GARRAY_MAX; i++)
        {
            G_Array[i] += 5;
        }

        // easy cmp.
        {
            INT32 in, out;
            for (out = 0; out != GARRAY_MAX; out++)
            for (in = 0; in != GARRAY_MAX; in++)
            {
                if (G_Array[out] != G_Array[in])
                 printf("not equal\n");
            }
        }
        
        // Win32Sem Release.
        ReleaseSemaphore(G_hSem, 1, &i);
    }
    return 0;
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