// win32 structured exception handling local rountine test4
#include <windows.h>
#include <stdio.h> 

int main()
{
  int *AV_ERROR = NULL;
    _try
    {
        *AV_ERROR = 0;
    }
    _except( EXCEPTION_EXECUTE_HANDLER )
    {
        *AV_ERROR = 0;
    }
    // imm complier error...
    _except( EXCEPTION_EXECUTE_HANDLER )
    {
        *AV_ERROR = 0;
    }
    return 0;
}