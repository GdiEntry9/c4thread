// win64 structured exception handling local rountine test1
#include <windows.h>
#include <stdio.h> 

// method get SEH head.
void _GetSehListHeadNodePtr(struct _ExceptionNode **ppEnode)
{
  __asm mov rax, gs:[0]
  __asm mov rcx, [ppEnode]
  __asm mov [rcx], rax
}
// method set SEH head.
void _SetSehListHeadNode(struct _ExceptionNode *pEnode)
{
  __asm mov rax, pEnode
  __asm mov gs:[0], rax 
}

// default callback.
EXCEPTION_DISPOSITION
__cdecl 
_except_handler(void *ExceptionRecord,
                void *ExceptLink,
                void *ContextRecord,
                void *DispatcherContext)
{ 
  printf ( "success entry SEH...");

	return ExceptionContinueExecution; 
} 
// std entry.
int main()
{ 

  // TODO:unstall hook ?
	return 0; 
} 