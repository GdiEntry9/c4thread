// --------------------------------------------------------------------------
// BlockOverflowInterlockedTest for MSVC.
// --------------------------------------------------------------------------
// What call BlockOverflowInterlockedTest?
// We know that when we have access to the same block of memory, 
// the function of the atom is always kept in the atomic sequence.
// suppose we have the following requirements.
// we need to access two parts of the memory cross. in x86 mode.
//
//  address:0x00000000              0x00000004
//       BYTE-array-A[4]          BYTE-array-B[7]
//             |           ___________
//             |          |           |
//             |          |           |
//            A0          |           B0          0x00000004
//            A1          |           B1          0x00000005
//            A2          |           B2          0x00000006
//            A3          |           B3          0x00000007
//             |__________|           B4          0x00000008
//                                    B5          0x00000009
//                                    B6          0x0000000A
//
// every time we visit a DWORD, A3-B2 and B2-B5 each DWORD a access.
// whether the two visit will allow the B2 to maintain synchronization
// It is clear that this is not aligned, in three basic memory chunk
// --------------------------------------------------------------------------
             
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <intrin.h>

#define  THREAD_MAX 16 

static BOOL G_bSignalQuitThread = FALSE;
static HANDLE G_hThread[THREAD_MAX] = {0};
struct { BYTE A[4]; BYTE B[7]; };

int __stdcall A3TOB2(void *pRaram) 
{
  while (G_bSignalQuitThread == FALSE) 
  {
    union { BYTE ST[4];
      LONGLONG BM;
    } FD; FD.BM = InterlockedIncrement64(&A[3]);
    printf("A3TOB2 phase-current count:%d\n", (unsigned int)(FD.ST[3]));
  }
  return 0;
}
int __stdcall B2TOB5(void *pRaram) 
{
  while (G_bSignalQuitThread == FALSE) 
  {
    union { BYTE ST[4];
      LONGLONG BM;
    } FD; FD.BM = InterlockedIncrement64(&B[2]);
    printf("B2TOB5 phase-current count:%d\n", (unsigned int)(FD.ST[0]));
  }
  return 0;
}
