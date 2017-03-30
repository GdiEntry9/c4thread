// win32 structured exception handling local rountine test1
#include <windows.h>
#include <stdio.h> 

// easy context for debug code.
struct CONTEXT_ {
  LONG ebp, esp, eip;
};
// exception struct. for test 
struct _ExceptionNode {
  struct _ExceptionNode *previous; 
  void *rountine;
  struct CONTEXT_ *ctx_;
};

#define UNWIND_HISTORY_TABLE_SIZE 12

// dummy NT-dispatcher's struct.
typedef struct _RUNTIME_FUNCTION_ {
  DWORD BeginAddress;
  DWORD EndAddress;
  DWORD UnwindInfoAddress;
} RUNTIME_FUNCTION_, *PRUNTIME_FUNCTION_;

// dummy NT-dispatcher's struct.
typedef struct _UNWIND_HISTORY_TABLE_ENTRY_ {
  DWORD64 ImageBase;
  DWORD64 Gp;
  PRUNTIME_FUNCTION_ FunctionEntry;
} UNWIND_HISTORY_TABLE_ENTRY_, *PUNWIND_HISTORY_TABLE_ENTRY_;

// dummy NT-dispatcher's struct.
typedef struct _UNWIND_HISTORY_TABLE_ {
  DWORD Count;
  BYTE  LocalHint;
  BYTE  GlobalHint;
  BYTE  Search;
  BYTE  Once;
  DWORD64 LowAddress;
  DWORD64 HighAddress;
  UNWIND_HISTORY_TABLE_ENTRY_ Entry[UNWIND_HISTORY_TABLE_SIZE];
} UNWIND_HISTORY_TABLE_, *PUNWIND_HISTORY_TABLE_;

// dummy NT-dispatcher's context.
struct __DISPATCHER_CONTEXT {
  DWORD ControlPc;
  DWORD ImageBase;
  PRUNTIME_FUNCTION_ FunctionEntry;
  DWORD EstablisherFrame;
  DWORD TargetPc;
  PCONTEXT ContextRecord;
  PEXCEPTION_ROUTINE LanguageHandler;
  PVOID HandlerData;
  PUNWIND_HISTORY_TABLE_ HistoryTable;
  DWORD ScopeIndex;
  BOOLEAN ControlPcIsUnwound;
  PBYTE  NonVolatileRegisters;
  DWORD Reserved;
};
// method for get CONTEXT_
__declspec(naked) void __cdecl _GetCurrentContext(struct CONTEXT_ *pContext)
{
  // arg1 - esp + 4
  // upper eip - esp
  __asm mov eax, [esp+4] 
  __asm mov [eax+0], ebp 
  __asm lea edx, [esp+8]
  __asm mov [eax+4], edx 
  __asm mov ecx, [esp+0]
  __asm mov [eax+8], ecx 
  __asm pop edx 
  __asm jmp ecx 
}
// method same as name.
void _DumpRegister(struct CONTEXT_ *pContext)
{
  static char szBuffer[1024];
  ZeroMemory(szBuffer, sizeof(szBuffer));
  sprintf(szBuffer, "dump registers status\nEBP:%08X ESP:%08X EIP:%08X\n"
        , pContext->ebp, pContext->esp, pContext->eip);
  // Output string.
  OutputDebugStringA(szBuffer);
}
// fot sysrem
void _DumpSystemRegister(struct _CONTEXT *pContext)
{
  static char szBuffer[1024];
  ZeroMemory(szBuffer, sizeof(szBuffer));
  sprintf(szBuffer, "dump system registers status\nEBP:%08X ESP:%08X EIP:%08X\n"
        , pContext->Ebp, pContext->Esp, pContext->Eip);
  // Output string.
  OutputDebugStringA(szBuffer);
}
// method for set CONTEXT_
__declspec(naked) void __cdecl _SetCurrentContext(struct CONTEXT_ *pContext)
{
  __asm mov eax, [esp+4] 
  __asm mov ebp, [eax+0]
  __asm mov esp, [eax+4]
  __asm mov eax, [eax+8]
  __asm jmp eax 
}
// method get SEH head.
void _GetSehListHeadNodePtr(struct _ExceptionNode **ppEnode)
{
  __asm mov eax, fs:[0]
  __asm mov ecx, [ppEnode]
  __asm mov [ecx], eax 
}
// method set SEH head.
void _SetSehListHeadNode(struct _ExceptionNode *pEnode)
{
  __asm mov eax, pEnode
  __asm mov fs:[0], eax 
}

// default callback.
EXCEPTION_DISPOSITION
__cdecl 
_except_handler(struct _EXCEPTION_RECORD *ExceptionRecord,
                struct _ExceptionNode *ExceptLink,
                struct _CONTEXT *ContextRecord,
                struct __DISPATCHER_CONTEXT *DispatcherContext)
{ 
  MessageBoxA(NULL, "Test", "SEH", MB_OK);
printf("---------SEH-Rountine----------\n");
  ContextRecord->Ebp = ExceptLink->ctx_->ebp;
  ContextRecord->Esp = ExceptLink->ctx_->esp;
  ContextRecord->Eip = ExceptLink->ctx_->eip;

	return ExceptionContinueExecution; 
} 
// std entry.
int main()
{ 
  struct _ExceptionNode enode;
  struct _ExceptionNode *enode2;
  struct _ExceptionNode *enode3;
  struct CONTEXT_ ctx_;
  
  int *AV_ERROR = NULL;
  void *eip_ = &ctx_.eip;
  
  _GetSehListHeadNodePtr(&enode2);
  _GetCurrentContext(&ctx_);
  _DumpRegister(&ctx_);
  
  // get label address 
  __asm movd xmm0, eax 
  __asm movd xmm1, edx 
  __asm lea eax, [ctx_.eip]
  __asm mov edx, __safe_place
  __asm mov [eax], edx 
  __asm movd eax, xmm0
  __asm movd edx, xmm1
  
  enode.rountine = _except_handler;
  enode.previous = enode2;
  enode.ctx_ = &ctx_;
  
  _SetSehListHeadNode(&enode);
  _GetSehListHeadNodePtr(&enode3);

  *AV_ERROR = 0;
__safe_place:
  _GetCurrentContext(&ctx_);
  _DumpRegister(&ctx_);
  // TODO:unstall hook ?
	return 0; 
} 