/*-
 * Copyright (c) 2016 moecmks
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "thread.h"
#include "mutex.h"
#include <process.h>

/*
 * msgbox error simple wraps/some check mutex. 
 */
#  define MSG_E2(tExT__, cApTiOn__) \
  MessageBoxA     ( null, tExT__, cApTiOn__, MB_ICONERROR)

/*
 * fear .....
 */
#  define c4mutex_unlock_(oMuTeX__, iN_sOmE_mEtHoD__) \
  do {         \
    if (c4mutex_unlock (oMuTeX__) != C4_OK) \
      MSG_E2 ("c4mutex unlock failed.", iN_sOmE_mEtHoD__); \
  } while (0)

/*
 * for c4global_initialize .....
 */
#  define MSG_E3(tExT__) \
  do {         \
      MSG_E2 (tExT__, "c4global_initialize"); \
      goto cleanup0; \
  } while (0); else ++ cleanup_depth

/* 
 * thread exit mode
 */
enum C4THREAD_EXIT_MODE {
  C4THREAD_EXIT_MODE_NORMAL = 0,
  C4THREAD_EXIT_MODE_CALLEXIT,
  C4THREAD_EXIT_MODE_CALLCANEL 
};

/* 
 * thread status 
 */
enum C4THREAD_STATUS {
  C4THREAD_STATUS_FREE = 0,
  C4THREAD_STATUS_BUSY
};

/* 
 * thread cleanup-stack
 */
static struct c4thread_cleanup_ {
  struct c4thread_cleanup_ *level;
  struct c4thread_callback_ *callback;
};

/*
 * c4thread struct.
 */
typedef struct c4thread_ {
  struct c4context destr_ctx, except_ctx2; /* aligned 16 bytes for fxrstor/fxsave */
  struct c4except_ except_;
  struct c4thread_callback_ procedure;
  struct c4thread_cleanup_ cleanup_stack;
  HANDLE cancel_event, handle; /* manual reset event. */
  c4mutex lock; /* no-cancel-able lock. */
  c4tid tid; /* c4thread's tid */
  c4tid join_tid; /* join c4thread's tid */
  int32 join_entry; /* for thread join. */
  int64 cpuset; 
  int32 cancelPending; /* cancel Queue count */
  intptr obj_g_uuid; /* object unique identifier */
  lean32 startUpThread, detachable; /* assert startup-thread/detach attriubute */
  int32 join_mask;  /* 0:joinable 1:another exist -1:will kill*/
  lean32 link_disable;
  CONTEXT ex_ctx;
  enum C4THREAD_EXIT_MODE exmode; /* see thread exit mode */
  enum C4THREAD_STATUS status; /* see thread status  */
  enum C4THREAD_CANCEL_TYPE cancel_type;
  enum C4THREAD_CANCEL_STATE cancel_state;
} c4thread_s, *c4threadImp;

/* 
 * thread single link.
 */

static struct c4thread_link_ {
  struct c4thread_link_ *level;
  struct c4thread_ *thread;
};

/*
 * thread create's params.
 */
struct _c4thread_params {
  lean32 context_init;
  lean32 interrupt_onstart;
  c4threadImp host;
  c4threadImp imp;
  c4tid tid;
  struct c4thread_link_ *previous, *current;
  struct c4thread_link_ *link;
  struct c4thread_ *std;
  HANDLE host_echo, interrupt;
};

/* 
 * global thread data.
 */
static struct _kthread_ {
  uint64 cpuset_Z; /* cpuset max count. */
  uint64 process_identifier; /* current process id */ 
  struct c4thread_link_ *list; /* . */
  struct c4thread_link_ *StartupLink; 
  HANDLE process_handle; /* current process handle. */
  CRITICAL_SECTION lock; /* global lock, protect list */
} _kthread;

/*
 * default c4thread's callback settings.
 */
static 
c4error c4callc clfu (c4thread thread, void *param) { return C4_OK; }
static 
c4error c4callc clfu_except_ (c4thread thread, void *param_except_, struct c4except_ *except_) { return C4_OK; }
static 
const struct c4thread_callback_ edclfu_ = { null, null, clfu, clfu_except_ };

/**
 * Extract context from the current context
 *
 * @param  c4context structure
 * @param  current context structure pointer
 */

c4finline 
#if defined (__X86_WOW64_)
void collect_context (struct c4context *context_, WOW64_CONTEXT *win32_context_) {
#else 
void collect_context (struct c4context *context_, CONTEXT *win32_context_) {
#endif 
#if defined (__X86_WOW64_) || defined (__X86_32_)
  context_->cpu.esp = win32_context_->Esp;
  context_->cpu.ebp = win32_context_->Ebp;
  context_->cpu.eip = win32_context_->Eip;
  context_->cpu.esi = win32_context_->Esi;
  context_->cpu.edi = win32_context_->Edi;
  context_->cpu.ebx = win32_context_->Ebx;
  context_->cpu.psw = win32_context_->EFlags;
  context_->cpu.eax = win32_context_->Eax;
  context_->cpu.ecx = win32_context_->Ecx;
  context_->cpu.edx = win32_context_->Edx;
#elif defined (__X86_64_)
  context_->cpu.rsp = win32_context_->Rsp;
  context_->cpu.rbp = win32_context_->Rbp;
  context_->cpu.rip = win32_context_->Rip;
  context_->cpu.rsi = win32_context_->Rsi;
  context_->cpu.rdi = win32_context_->Rdi;
  context_->cpu.rbx = win32_context_->Rbx;
  context_->cpu.psw = win32_context_->EFlags;
  context_->cpu.rax = win32_context_->Rax;
  context_->cpu.rcx = win32_context_->Rcx;
  context_->cpu.rdx = win32_context_->Rdx;
  context_->cpu.r8 = win32_context_->R8;
  context_->cpu.r9 = win32_context_->R9;
  context_->cpu.r10 = win32_context_->R10;
  context_->cpu.r11 = win32_context_->R11;
  context_->cpu.r12 = win32_context_->R12;
  context_->cpu.r13 = win32_context_->R13;
  context_->cpu.r14 = win32_context_->R14;
  context_->cpu.r15 = win32_context_->R15;
#endif 

#if defined (__X86_WOW64_) || defined (__X86_64_)
  memcpy ( & context_->fpu, & win32_context_->ExtendedRegisters, sizeof (struct fpu_x64 ));
#elif defined (__X86_32_)
  memcpy ( & context_->fpu, & win32_context_->FloatSave,         sizeof (struct fpu_387pmode ));
#endif
}

/**
 * Extract context from the win32 context
 *
 * @param  c4context structure
 * @param  win32 context structure pointer
 */

c4finline 
void collect_context2 (struct c4context *context_, struct _CONTEXT *win32_context_) {
  
#if defined (__X86_WOW64_) || defined (__X86_32_)
  context_->cpu.esp = win32_context_->Esp;
  context_->cpu.ebp = win32_context_->Ebp;
  context_->cpu.eip = win32_context_->Eip;
  context_->cpu.esi = win32_context_->Esi;
  context_->cpu.edi = win32_context_->Edi;
  context_->cpu.ebx = win32_context_->Ebx;
  context_->cpu.psw = win32_context_->EFlags;
  context_->cpu.eax = win32_context_->Eax;
  context_->cpu.ecx = win32_context_->Ecx;
  context_->cpu.edx = win32_context_->Edx;
#elif defined (__X86_64_)
  context_->cpu.rsp = win32_context_->Rsp;
  context_->cpu.rbp = win32_context_->Rbp;
  context_->cpu.rip = win32_context_->Rip;
  context_->cpu.rsi = win32_context_->Rsi;
  context_->cpu.rdi = win32_context_->Rdi;
  context_->cpu.rbx = win32_context_->Rbx;
  context_->cpu.psw = win32_context_->EFlags;
  context_->cpu.rax = win32_context_->Rax;
  context_->cpu.rcx = win32_context_->Rcx;
  context_->cpu.rdx = win32_context_->Rdx;
  context_->cpu.r8 = win32_context_->R8;
  context_->cpu.r9 = win32_context_->R9;
  context_->cpu.r10 = win32_context_->R10;
  context_->cpu.r11 = win32_context_->R11;
  context_->cpu.r12 = win32_context_->R12;
  context_->cpu.r13 = win32_context_->R13;
  context_->cpu.r14 = win32_context_->R14;
  context_->cpu.r15 = win32_context_->R15;
#endif 

#if defined (__X86_WOW64_) || defined (__X86_64_)
  memcpy ( & context_->fpu, & win32_context_->ExtendedRegisters, sizeof (struct fpu_x64 ));
#elif defined (__X86_32_)
  memcpy ( & context_->fpu, & win32_context_->FloatSave,         sizeof (struct fpu_387pmode ));
#endif
}

/**
 * Extract current context from the  c4context
 *
 * @param  current context structure pointer
 * @param  c4context structure
 */
c4finline 
#if defined (__X86_WOW64_)
void collect_win32context (struct _WOW64_CONTEXT *win32_context_, struct c4context *context_) {
#else 
void collect_win32context (struct _CONTEXT *win32_context_, struct c4context *context_) {
#endif 

#if defined (__X86_WOW64_) || defined (__X86_32_)
  win32_context_->Esp = context_->cpu.esp;
  win32_context_->Ebp = context_->cpu.ebp;
  win32_context_->Eip = context_->cpu.eip;
  win32_context_->Esi = context_->cpu.esi;
  win32_context_->Edi = context_->cpu.edi;
  win32_context_->Ebx = context_->cpu.ebx;
  win32_context_->EFlags = context_->cpu.psw;
  win32_context_->Eax = context_->cpu.eax;
  win32_context_->Ecx = context_->cpu.ecx;
  win32_context_->Edx = context_->cpu.edx;
#elif defined (__X86_64_)
  win32_context_->Rsp = context_->cpu.rsp;
  win32_context_->Rbp = context_->cpu.rbp;
  win32_context_->Rip = context_->cpu.rip;
  win32_context_->Rsi = context_->cpu.rsi;
  win32_context_->Rdi = context_->cpu.rdi;
  win32_context_->Rbx = context_->cpu.rbx;
  win32_context_->EFlags = context_->cpu.psw;
  win32_context_->Rax = context_->cpu.rax;
  win32_context_->Rcx = context_->cpu.rcx;
  win32_context_->Rdx = context_->cpu.rdx;
  win32_context_->R8 = context_->cpu.r8;
  win32_context_->R9 = context_->cpu.r9;
  win32_context_->R10 = context_->cpu.r10;
  win32_context_->R11 = context_->cpu.r11;
  win32_context_->R12 = context_->cpu.r12;
  win32_context_->R13 = context_->cpu.r13;
  win32_context_->R14 = context_->cpu.r14;
  win32_context_->R15 = context_->cpu.r15;
#endif

#if defined (__X86_WOW64_) || defined (__X86_64_)
  memcpy ( & win32_context_->ExtendedRegisters, & context_->fpu, sizeof (struct fpu_x64 ));
#elif defined (__X86_32_)
  memcpy ( & win32_context_->FloatSave,         & context_->fpu, sizeof (struct fpu_387pmode ));
#endif
}


/**
 * extract some basic information from the win32 exception structure
 * 
 * @param  c4except structure
 * @param  win32 except structure pointer
 */

c4finline 
void collect_except (struct c4except_ *except_, struct _EXCEPTION_POINTERS *win32_except_) {
  
  except_->except_code = win32_except_->ExceptionRecord->ExceptionCode;
  except_->repair_ = null;
  collect_context2 (& except_->except_, win32_except_->ContextRecord);
}

/**
 * exec thread callback.
 *
 * @param  c4thread_callback_ object.
 * 
 * @remark BUG.BUG.BUG.!!!
 */
static  
void _callback (struct c4thread_callback_ *clfu) {
  
  static /* volatile */ c4tls struct _EXCEPTION_POINTERS *win32exception_p_ = null;
  static /* volatile */ c4tls struct c4thread_callback_ *clfu_ = null;
  static /* volatile */ c4tls struct c4except_ *except_ = null;

  __c4constructor {
    except_ = & ( (struct c4thread_ *)c4getop (tec))->except_;
    clfu_ = clfu;    
    clfu_->routine ( (struct c4thread_ *)tec, clfu_->param);
  } __c4exception_base ( ( (win32exception_p_ = GetExceptionInformation()) != null) ? 
                                  EXCEPTION_EXECUTE_HANDLER
                                 :EXCEPTION_EXECUTE_HANDLER) {
    __rep: 
    __c4constructor {
      collect_except ( except_, win32exception_p_);
      clfu_->except_ ((struct c4thread_ *)tec, clfu_->param_except_, except_);
      
      if (except_->repair_ != null)
        selfcontext_set (except_->repair_);
    } __c4exception_base ( ( (win32exception_p_ = GetExceptionInformation()) != null) ? 
                              EXCEPTION_EXECUTE_HANDLER
                             :EXCEPTION_EXECUTE_HANDLER) {
        goto __rep;
    }
  } 
}

/**
 * Get c4context from current platform
 *
 * @param  c4thread object.
 * @param  c4context object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:object has been destroyed
 */
c4error 
c4callc c4thread_getcontext2 (c4thread thread, struct c4context *context) {
  
  __c4constructor {
    
# if defined (__X86_32_) || defined (__X86_64_)
    struct _CONTEXT win32_context;
# elif defined (__X86_WOW64_)
    struct _WOW64_CONTEXT win32_context;
# endif 
    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);
    
    /* hold self */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;
    
    /* self ?*/
    if (threadImp->tid == & _g_tid)
      goto cleanup;
    
# if defined (__X86_WOW64_) || defined (__X86_64_)
    win32_context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_EXTENDED_REGISTERS;
# elif defined (__X86_32_)
    win32_context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_FLOATING_POINT;
# endif 

# if defined (__X86_32_) || defined (__X86_64_)
    if ( GetThreadContext (threadImp->handle, & win32_context) == FALSE)
# elif defined (__X86_WOW64_)
    if ( Wow64GetThreadContext (threadImp->handle, & win32_context) == FALSE)
# endif 
      error_ = C4_KFAIL;
    else
      collect_context (context, & win32_context);
cleanup:
    c4mutex_unlock_ (threadImp->lock, "c4thread_getcontext");
    return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Set current platform context from c4context
 *
 * @param  c4thread object.
 * @param  c4context object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:object has been destroyed
 */
c4impl 
c4error c4thread_setcontext (c4thread thread, struct c4context *context) {
  
  __c4constructor {
    
# if defined (__X86_32_) || defined (__X86_64_)
    struct _CONTEXT win32_context;
# elif defined (__X86_WOW64_)
    struct _WOW64_CONTEXT win32_context;
# endif 
    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);

    /* hold self. */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;

    /* self ? */
    if (threadImp->tid == & _g_tid) {
      /* 
       * !!!FIXME:At this moment may produce synchronization BUG
       */
      c4mutex_unlock_ (threadImp->lock, "c4thread_setcontext");
      selfcontext_set (context);
      return C4_OK;
    }
/* TODO:lock?? */
    collect_win32context (& win32_context, context);

# if defined (__X86_WOW64_) || defined (__X86_64_)
    win32_context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_EXTENDED_REGISTERS;
# elif defined (__X86_32_)
    win32_context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_FLOATING_POINT;
# endif 

# if defined (__X86_32_) || defined (__X86_64_)
    if ( SetThreadContext (threadImp->handle, & win32_context) == FALSE)
# elif defined (__X86_WOW64_)
    if ( Wow64SetThreadContext (threadImp->handle, & win32_context) == FALSE)
# endif 
      error_ = C4_KFAIL;
/* cleanup: */
    c4mutex_unlock_ (threadImp->lock, "c4thread_setcontext");
    return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Returns the maximum number of CPU masks
 *
 * @param  mask  accecpt.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_PARAM:param is illegal memory
 */
c4impl /* FIXME:CPU nuclear number limit, int64 ext*/
c4error c4thread_getaffinitie_Z (uint64 *mask) {
  
  __c4constructor {
    
    *mask = _kthread.cpuset_Z;
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_PARAM;
  }
}

/**
 * Gets the c4thread object to start the thread
 *
 * @param  c4thread accecpt.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_NONE:main thread does not exist.
 */
c4impl 
c4error c4thread_GetStartup (c4thread *thread) {
  
  __c4constructor {
    
     c4thread thread_ = _kthread.StartupLink->thread;
     c4threadImp threadImp = c4getop (thread_);
     c4obj_check_thread (threadImp);
     
     if (threadImp->startUpThread == FALSE )
       return C4_NONE;
     
     *thread = thread_;
     return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * resume a pending thread
 *
 * @param  c4thread accecpt.
 * @param  count accecpt.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_NONE:main thread does not exist.
 *          C4_PERM:try to hang their own thread
 */
c4impl 
c4error c4thread_startup (c4thread thread, uint32 *count) {
  
  __c4constructor {
    
    int32 count_;
    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);

    /* hold self */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;

    /* self ? */
    if (threadImp->tid == & _g_tid) {
      c4mutex_unlock_ (threadImp->lock, "c4thread_startup");
      return C4_PERM;
    }
    
/* TODO:lock?? */
# if defined (__X86_32_) || defined (__X86_64_)
    if ( (count_ = ResumeThread (threadImp->handle)) == (DWORD) -1)
# elif defined (__X86_WOW64_)
    if ( (count_ = ResumeThread (threadImp->handle)) == (DWORD) -1)
# endif 
      error_ = C4_KFAIL;
    else if (count != null)
      *count = count_;
/* cleanup: */
    c4mutex_unlock_ (threadImp->lock, "c4thread_startup");
    return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Suspend a thread
 *
 * @param  c4thread accecpt.
 * @param  count accecpt.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_NONE:main thread does not exist.
 *          C4_PERM:try to hang their own thread
 */
c4impl 
c4error c4thread_suspend (c4thread thread, uint32 *count) {
  
  __c4constructor {

    int32 count_;
    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);

    /* hold self */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;

    /* self ? */
    if (threadImp->tid == & _g_tid) {
      c4mutex_unlock_ (threadImp->lock, "c4thread_suspend");
      return C4_PERM;
    }
/* TODO:lock?? */
# if defined (__X86_32_) || defined (__X86_64_)
    if ( (count_ = SuspendThread (threadImp->handle)) == (DWORD) -1)
# elif defined (__X86_WOW64_)
    if ( (count_ = Wow64SuspendThread (threadImp->handle)) == (DWORD) -1)
# endif 
      error_ = C4_KFAIL;
    else if (count != null)
      *count = count_;
/* cleanup: */
    c4mutex_unlock_ (threadImp->lock, "c4thread_suspend");
    return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/** 
 * setdetach/setattach wraps.
 *
 * @param  c4thread .
 * @param  assert attach attriubute. 
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_PERM:try to hang their own thread
 *          C4_BUSY:temporarily unable to respond to demand
 */
c4finline 
c4error c4thread_attachInline (c4thread thread, lean32 detachable) {
  
  __c4constructor {
    
    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);

    /* hold self */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;

    if (threadImp->status == C4THREAD_STATUS_BUSY)
      error_ = C4_BUSY;
    else if (threadImp->link_disable != FALSE)
      error_ = C4_BUSY;
    else
      threadImp->detachable = !! detachable;

    c4mutex_unlock_ (threadImp->lock, "c4thread_attachInline");
    return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/** 
 * set a thread detach
 */
c4impl 
c4error c4thread_setdetach (c4thread thread) {
  return c4thread_attachInline (thread, TRUE);
}

/** 
 * set a thread wait-able
 */
c4impl 
c4error c4thread_setjoin (c4thread thread) {
  return c4thread_attachInline (thread, FALSE);
}

/**
 * C4thread_callback version of memcpy.(internal use, no-lock)
 * 
 * @param  target 
 * @param  source
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return C4_BADMEM
 */

c4error 
c4thread_setc_ (struct c4thread_callback_ *target, struct c4thread_callback_ *source) {
  
  __c4constructor {
    
    struct c4thread_callback_ target_, *sourcep_ = (source != null) ? source : & edclfu_;

    /* copy temp. if sourcep_ points to an invalid memory, then BADMEM can be returned directly here */
    memcpy ( & target_, sourcep_, sizeof (struct c4thread_callback_));
    
    /* set default callback. again */
    if ( target_.routine == null )
       target_.routine = clfu;
    else if ( target_.except_ == null )
       target_.except_ = clfu_except_;
     
    /* set callback */
    memcpy (target, & target_, sizeof (struct c4thread_callback_) );
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Get current c4thread object.
 */
c4impl 
c4error c4thread_current (c4thread *thread) {
  
  *thread = tec;
  return C4_OK;
}

/**
 * Get current c4thread object.
 */
c4impl 
c4error c4thread_currenttid (c4tid *tid) {
  
  *tid = & _g_tid;
  return C4_OK;
}

/**
 * thread cleanup routine.(internal use)
 */
static void cleanup_s2 (c4threadImp imp, lean32 docall) {
  
  /* List head as stack top */
  /* LIFO call-cleanup */
  
  lean32 do_call = !!docall;
  struct c4thread_cleanup_ *stkp = _p_store ( & imp->cleanup_stack.level, null);
  struct c4thread_cleanup_ *stkt;/* = imp->cleanup_stack.level;*/

  for (; stkp != null; stkp = stkt) {
    /* save next nodde ptr .**/
    stkt = stkp->level;
    if (do_call != FALSE)
    _callback ( stkp->callback);
    /* free cleanup node. and callback's heap space**/
    c4free (stkp->callback);
    c4free (stkp);
  }
}

/**
 * Global management callback.
 */
static 
c4dinline unsigned int c4callstd __thmgr_c ( struct _c4thread_params *__params) {
  
  struct _c4thread_params params;
  memcpy (& params, __params, sizeof (params));

  /* we set default thread mask. **/
  if ( SetThreadAffinityMask (__params->imp->handle, (DWORD_PTR) _kthread.cpuset_Z) == 0)
    MSG_E2 ("SetThreadAffinityMask failed", "__thmgr_c");

  /*
   * we set up the exit routine. add the current thread node to the global 
   * thread management chain and unlock the host block
   */
_start:
  if ( params.context_init == FALSE )
    goto _getaddrlabel; 
  
    /*
     * hold global-lock before visit it.
     */
  EnterCriticalSection ( & _kthread.lock);
      /* append item to global-list head.*/
    params.link->level = _kthread.list->level;
  _kthread.list->level = params.link;
    /*
     * unlock global-lock.
     */  
  LeaveCriticalSection ( & _kthread.lock);
     /*
     * set thread environment chunk 
     */
  if ( params.imp->startUpThread == FALSE ) {
    tec = params.std;
  }
     /* unlock host blocking.*/
  if ( SetEvent (params.host_echo) == FALSE) { 
    /* If the sending signal fails, the host thread will deadlock */
    MSG_E2 ("signal host_echo fail, host thread will pretend-dead", "__thmgr_callback");
               /* TODO:release .in fact, Basic will not fail, But if that happens, That may be in big trouble*/
  }
  
  /*
   * thread context has already been set.
   * we test whether the thread is to be suspended in the initial state
   */
  if ( params.interrupt_onstart != FALSE) {
     /* fall into waiting.(i.e. non-cancelable maybe return failed. but harmless )*/
    WaitForSingleObject (params.interrupt, INFINITE);
    CloseHandle ( _p_store (& params.interrupt, null));
  } 
  
  /* callback */
  _callback (& params.imp->procedure);  
  /* set normal exit flags, lock*/
  c4mutex_lock (params.imp->lock);
  params.imp->status = C4THREAD_STATUS_BUSY;
  params.imp->exmode = C4THREAD_EXIT_MODE_NORMAL;
  /* uunlocK */
  c4mutex_unlock_ (params.imp->lock, "Global management callback");
     /* 
     * into destructor.
     */
    goto _dtorlabel; 
    
_getaddrlabel:
  __params->tid = & _g_tid;
  
  /* 
   * fillc4tid, collect exit's context
   */
  selfcontext_get (& params.imp->destr_ctx);
  
  if ( _store32 (& params.context_init, TRUE) == FALSE)
    goto _start;
    
_dtorlabel:

     /*
      * thread is about to usher in her death.
      * for detach-thread, we immediately destroy the current thread data structure
      */ 
      c4mutex_unlock_root (params.imp->lock);
      if ( params.imp->detachable == FALSE) {
        /*
         * nodone in join-thread phase.
         */
         return 0;
      }
      
      if ( params.imp->detachable == TRUE && params.imp->exmode != C4THREAD_EXIT_MODE_NORMAL) {
        /* cleanup routine for c4thread_exit/c4thread_cancel  */
        cleanup_s2 (params.imp, TRUE);
      }
               /*  destroy thread handle, do it*/
        CloseHandle (params.imp->handle);
         /* 
         * wait thread lock.last time(i.e. non-cancelable)
         */
      c4mutex_lock (params.imp->lock);
      
         /* get global-lock. */
      EnterCriticalSection ( & _kthread.lock);
         /* 
         * foreach global-list delete current thread chunk.
         */
      
  for (params.current = (params.previous = _kthread.list)->level; 
       params.current; 
       params.current = (params.previous = params.current)->level) {
         
   if (params.current->thread == params.std) {
      /* find match, remove it, unlock.
       */
       params.previous->level = params.current->level;
       /* _m_access_barrier(); */
       
         /* geiveuppppp global-lock*/
      LeaveCriticalSection ( & _kthread.lock);
       
                /* destroy attach handle and c4mutex sync-lock*/

       c4mutex_destroy ( params.imp->lock);
       CloseHandle ( params.imp->cancel_event);
        
       /* final release c4thread's data struct, game is over. */
       c4free (params.imp);
       return 0;
   }
  }
  
    /* assert exist */
    MSG_E2 ("we not find goal thread's data struct ", "__thmgr_callback");

  
      return 0;
  
}

/**
 * add cleanup-routine into current thread's cleanup-stack
 * 
 * @param  c4thread_callback_
 * 
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_NOMEM:biggest possibility is not enough heap space.
 *          C4_BADMEM:illegal memory operation
 */ 
c4impl 
c4error c4thread_cleanup_push (struct c4thread_callback_ *procedure) {
  
  __c4constructor {

    c4threadImp threadImp = c4getop ( (struct c4thread_ *) tec);
    struct c4thread_cleanup_ *node = c4malloc (sizeof (struct c4thread_cleanup_));
    struct c4thread_callback_ *llb = c4malloc (sizeof (struct c4thread_callback_));
    /* c4obj_check_thread (threadImp); */
    
    if (node 
            && llb )
            {
              if ( c4thread_setc_ (llb, procedure) != C4_OK)
              {
                /* params. errors **/
                c4free (llb);
                c4free (node);
                
                return C4_BADMEM;
              }
              
              /* insert current thread 's cleanup-stack */
              node->level = threadImp->cleanup_stack.level;
              node->callback = llb;
              
              threadImp->cleanup_stack.level = node;
              return C4_OK;
            }
            
            if (llb != null)                c4free (llb);
            if (node != null)    c4free (node);
            
            return C4_NOMEM;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * remove cleanup-routine into current thread's cleanup-stack top
 * 
 * @param  if != 0, then remove and exec, otherwise only remove
 * 
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_NONE:No elements are available in the cleanup stack
 *
 * @bug     synchronization with cancel
 */ 
c4impl 
c4error c4thread_cleanup_pop (int32 level) {
  
  /* List head as stack top */
  /* LIFO call-cleanup */
  
  lean32 do_call = !!level;
  struct c4thread_cleanup_ *stkp = ((c4threadImp)tec)->cleanup_stack.level;
  struct c4thread_cleanup_ *stkt;/* = imp->cleanup_stack.level;*/

  for (; stkp != null; stkp = stkt) {
    /* save next nodde ptr .**/
    stkt = stkp->level;
    if (do_call != FALSE)
    _callback ( stkp->callback);
    /* free cleanup node. and callback's heap space**/
    c4free (stkp->callback);
    c4free (stkp);
    /* link next */
    ((c4threadImp)tec)->cleanup_stack.level = stkt; 
    return C4_OK;
  }
  return C4_NONE;
}

/**
 * Get thread CPU affinitie
 * 
 * @param  c4thread accecpt 
 * @param  mask accecpt
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:host thread is invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:host thread has been destroyed
 */
c4impl 
c4error c4thread_getaffinitie (c4thread thread, uint64 *mask) {

  __c4constructor {

    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);

    /* check memory vailed. */
    *mask = 0;

    /* hold self */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;

    *mask = threadImp->cpuset;
/* cleanup: */
    c4mutex_unlock_ (threadImp->lock, "c4thread_getaffinitie");
    return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Set thread CPU affinitie
 * 
 * @param  c4thread attempt 
 * @param  mask attempt
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:host thread is invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:host thread has been destroyed
 */
c4impl 
c4error c4thread_setaffinitie (c4thread thread, uint64 mask) {

  __c4constructor {

    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);

    /* hold self */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;

    if ( SetThreadAffinityMask (threadImp->handle, (DWORD_PTR)mask) == 0)
      MSG_E2 ("SetThreadAffinityMask fail", "c4thread_setaffinitie");
    else 
      threadImp->cpuset = mask;
/* cleanup: */
    c4mutex_unlock_ (threadImp->lock, "c4thread_setaffinitie");
    return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}




c4impl c4error c4thread_join (c4thread thread);




/**
 * Env init ...
 */ 
c4impl 
c4error c4global_initialize (void) {

  static lean32 fi = FALSE;
  static lean32 success_infi = FALSE;
  static int32 cleanup_depth = 0;
  struct c4thread_ *p = null;
  struct c4thread_link_ *s = null;
  static int64 _qword_ptemp;

  if (fi == TRUE && success_infi == FALSE)
    return C4_FAIL;
  if (fi == TRUE && success_infi == TRUE)
    return C4_OK;
  if (fi == FALSE && success_infi == TRUE)
    return C4_UNKWN;

  /* check memory vaile. **/
  if ( (s = c4malloc (sizeof (struct c4thread_link_)) ) == null)
    MSG_E3 ("c4thread_link_ alloc fail."); // L0

  if ( (p = c4memalign (16, sizeof (c4thread_s)) ) == null) 
    MSG_E3 ("c4thread chunk alloc fail."); // L1

  /* set init-link**/
  s->level = null;
  s->thread = p;

  p->obj_g_uuid = obj_g_thread;
  p->handle = (HANDLE) -1;
  p->detachable = FALSE;
  p->cancel_state = C4THREAD_CANCEL_ENABLE;
  p->cancel_type = C4THREAD_CANCEL_DEFFERED;
  p->exmode = C4THREAD_EXIT_MODE_NORMAL;
  p->status = C4THREAD_STATUS_FREE;
  p->cancel_event = CreateEvent ( null, TRUE, FALSE, null);
  p->link_disable = FALSE;
  p->tid = null;
  p->join_tid = null;
  p->join_entry = 0;
  p->join_mask = 0;
  p->cpuset = _kthread.cpuset_Z;
  p->startUpThread = TRUE;

  /* check cancel_evnet **/
  if (p->cancel_event == null) // L2
    MSG_E3 ( "create StartupThread's cancel-event fail.");

  /* create c4sync lock. */
  if ( c4mutex_init (& p->lock, TRUE) != C4_OK) // L3
    MSG_E3 ( "create StartupThread's lock fail.");

  /* init GLOBAL lock..*/
  if ( InitializeCriticalSectionAndSpinCount ( & _kthread.lock, 4000) == FALSE) // L4
    MSG_E3 ( "InitializeCriticalSectionAndSpinCount fail");

  /* copy startup-thread handle */
  if ( DuplicateHandle(   GetCurrentProcess(),
                          GetCurrentThread(),
                          GetCurrentProcess(),
                        & p->handle,
                          0,
                          FALSE,
                          DUPLICATE_SAME_ACCESS) == FALSE ) // L5
    MSG_E3 ( "DuplicateHandle[StartupThreadHandle] fail");

  /* copy process handle */
  if ( DuplicateHandle(   GetCurrentProcess(),
                          GetCurrentProcess(),
                          GetCurrentProcess(),
                        & _kthread.process_handle,
                          0,
                          FALSE,
                          DUPLICATE_SAME_ACCESS) == FALSE ) // L6
    MSG_E3 ( "DuplicateHandle[CurrentProcessHandle] fail");

  /* we get system mask. **/
  if ( GetProcessAffinityMask (p->handle, (PDWORD_PTR)& _qword_ptemp, (PDWORD_PTR)& _kthread.cpuset_Z) == FALSE) // L7
    MSG_E3 ( "GetProcessAffinityMask fail");

  /* we set default thread mask. **/
  if ( SetThreadAffinityMask (p->handle, (DWORD_PTR)_kthread.cpuset_Z) == FALSE) // L8
    MSG_E3 ( "SetThreadAffinityMask fail");

  /* get process id */
  _kthread.process_identifier = GetCurrentProcessId();

  /* insert is.t */
  _kthread.StartupLink = c4setop(s);
  _kthread.list->level = c4setop(s);

  fi = FALSE;
  success_infi = TRUE;

  return C4_OK;

cleanup0:

  switch (cleanup_depth) { 
  case 9:
  case 8:
  case 7:  // L7 dec process handle ref.
    CloseHandle (_kthread.process_handle);
  case 6:  // L6 dec thread handle  ref.
    CloseHandle (p->handle);
  case 5:  // L5 deal global-lock release .
    DeleteCriticalSection (& _kthread.lock);
  case 4:  // L4 deal c4mutex-release.
    c4mutex_destroy (p->lock);
  case 3:  // L3 deal cancel-event.
    CloseHandle (p->cancel_event);
  case 2:  // L2 deal th-thhunk memory;
    c4free (p);
  case 1:  // L1 deal link memory.
    c4free (s);
  case 0:  // L0 base clear..
    _kthread.cpuset_Z = 0;
    _kthread.list->level = null;
    _kthread.list->thread = null;
    _kthread.process_handle = (HANDLE) -1;
    _kthread.StartupLink = null;

    fi = FALSE;
    success_infi = FALSE;
  default:
    return C4_FAIL; 
  }
}

/**
 * Env uninitialize ...
 */ 
c4impl 
c4error c4global_uninitialize (void) {

  c4threadImp p = c4getop(_kthread.StartupLink->thread);

  CloseHandle (_kthread.process_handle);
  CloseHandle (p->handle);
  DeleteCriticalSection (& _kthread.lock);
  c4mutex_destroy (p->lock);
  CloseHandle (p->cancel_event);

  c4free (p);
  c4free (_kthread.StartupLink);

  _kthread.cpuset_Z = 0;
  _kthread.list->level = null;
  _kthread.list->thread = null;
  _kthread.process_handle = (HANDLE) -1;
  _kthread.StartupLink = null;

  return C4_OK;
}


/**
 * Create a thread. 
 * 
 * @param  c4thread accecpt 
 * @param  indicates whether an interrupt occurs at the initial 
 * @param  thread stack space, the default argument is passed to 0
 * @param  c4tid accecpt (nocheck, please note.)
 * @param  c4thread_callback_::procedure (null indicates the default callback)
 * 
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:host thread is invailed object.
 *          C4_NOMEM:biggest possibility is not enough heap space.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:host thread has been destroyed
 *          C4_PARAM:call paramter errors.
 */ 

c4impl 
c4error c4thread_create (c4thread *thread, lean32 interrupt, int32 stacksize, c4tid *tid,
                   struct c4thread_callback_ *procedure
)
{   
  __c4constructor {

    struct _c4thread_params params;
    c4error error_;
    lean32 initMtx = FALSE; /* only for GCC.*/
    params.host = c4getop ( (struct c4thread_ *) tec);
    c4obj_check_thread (params.host);

    ZeroMemory ( &params, sizeof (params));
    
    /* hold self */
    if ( (error_ = c4mutex_lock (params.host->lock)) != C4_OK)
      return error_;
    
    /*
     * alloc link and thread chunk. 
     */
    if ( (params.link = c4malloc ( sizeof(struct c4thread_link_))) == null
     ||  (params.imp = c4memalign (16, sizeof(struct c4thread_))) == null) {
      error_ = C4_NOMEM;
      goto cleanup;
    } else {
      params.link->level = null;
      params.link->thread = params.std = c4setop (params.imp);
    }

   /* 
    * fill thread init params.
    */
    params.imp->obj_g_uuid = obj_g_thread;
    params.imp->handle = (HANDLE) -1;
    params.imp->detachable = FALSE;
    params.imp->cancel_state = C4THREAD_CANCEL_ENABLE;
    params.imp->cancel_type = C4THREAD_CANCEL_DEFFERED;
    params.imp->exmode = C4THREAD_EXIT_MODE_NORMAL;
    params.imp->status = C4THREAD_STATUS_FREE;
    params.imp->cancel_event = CreateEvent ( null, FALSE, FALSE, null);
    params.imp->link_disable = FALSE;
    params.imp->tid = null;
    params.imp->join_tid = null;
    params.imp->join_entry = 0;
    params.imp->join_mask = 0;
    params.imp->cpuset = _kthread.cpuset_Z;
    params.imp->startUpThread = FALSE;
    params.host_echo = CreateEvent ( null, TRUE, FALSE, null);
    params.interrupt = (!!interrupt == TRUE) ? CreateEvent ( null, TRUE, FALSE, null) : (HANDLE) -1;
    params.interrupt_onstart = !!interrupt;

    /* 
     * check local sync obj.
     */
    if ( C4_OK != c4mutex_init (& params.imp->lock, TRUE)
      || params.host_echo == null 
      || params.interrupt == null 
      || params.imp->cancel_event == null )
       goto cleanup;
    else 
       initMtx = TRUE;
    /*
     * callback settings. 
     */
    if ( C4_OK != c4thread_setc_ (& params.imp->procedure, procedure)) {
       error_ = C4_PARAM;
       goto cleanup;
     }
     
    /*
     * create thread use MSCRT-runtime method.
     */
    if ( (params.imp->handle = (HANDLE) _beginthreadex (null, 
            stacksize, __thmgr_c, & params, 0, null)) == (HANDLE) -1) 
       goto cleanup;
         
    /*
     * The thread at the MSCRT level has been created successfully,
     * but this does not mean that the c4thread created successfully.
     * We also need to do the following:
     *
     * !!1:Wait for the new thread to initialize the internal object
     *     in the __thmgr_callback, and then send the signal to notify
     *     the host thread 
     *
     * !!2:check, deal errors and some cleanup/misc things..
     */ 

    /*
     * wait new thread global-init complete.
     * Here we are simple, in the event of an error when a pop-up msgbox
     */  
    if ( WaitForSingleObject ( params.host_echo, INFINITE) != WAIT_OBJECT_0)
       MSG_E2 ("wait host_echo fail", "c4thread_create");
    else 
       CloseHandle ( _p_store (& params.host_echo, null));

    if ( params.interrupt_onstart == TRUE) {
#   if defined (__X86_WOW64_)
        if (Wow64SuspendThread (params.imp->handle) == (DWORD) -1)
#   elif defined (__X86_32_) || defined (__X86_64_)
        if (SuspendThread (params.imp->handle) == (DWORD) -1)
#   endif  
                {
                  /* suspend fail */
                  MSG_E2 ("suspendthread fail", "c4thread_create");
                }
                  /* signal child-thread */
                  if ( SetEvent (params.interrupt) == FALSE)
                      {
                        /* signal fail, */
                         MSG_E2 ("wake interrupt fail", "c4thread_create");
                      }

    }
    
        /* set current thread id (i.e. c4thread type ).*/
    if ( tid != null)
       *tid = params.tid;
     
     
     *thread = params.std;

    /* unlock self.*/
    c4mutex_unlock_ (params.imp->lock, "c4thread_create");
    return C4_OK;

cleanup:

    if ( params.link != null)
      c4free (params.link);
    
    if ( params.imp != null)
      {
        if ( _store32 ( & initMtx, FALSE) != FALSE)
          c4mutex_destroy (params.imp->lock);

        CloseHandle (params.imp->cancel_event);
        CloseHandle (params.host_echo);

        if (!! interrupt)
        CloseHandle (params.interrupt);

        c4free (params.imp);

      }
      
    /* unlock self.*/
    c4mutex_unlock_ (params.imp->lock, "c4thread_create");
    return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Thread exit (self request)
 * 
 * @param  exitcode (temporarily does not handle error codes)
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:host thread is invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:host thread has been destroyed
 *          C4_BUSY:temporarily unable to respond to demand
 */ 
c4impl 
c4error c4thread_exit (c4error exitcode) {
  
  __c4constructor {
    
    c4error error_;
    c4threadImp threadImp = c4getop ( (struct c4thread_ *) tec);
    c4obj_check_thread (threadImp);
    
    if ( threadImp->startUpThread != FALSE)
      exit (exitcode);

    /*
     * hold self.
     */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;
    if ( threadImp->status == C4THREAD_STATUS_FREE)
      {
        /* set busy/exmode. **/
        threadImp->status = C4THREAD_STATUS_BUSY;
        threadImp->exmode = C4THREAD_EXIT_MODE_CALLEXIT;

        selfcontext_set (& threadImp->destr_ctx);
        /* never reach here, only for warning.*/
        return C4_OK;
      }
    else 
      {
        c4mutex_unlock_ (threadImp->lock, "c4thread_exit");
        return C4_BUSY;
      }
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}


/**
 * test cancel status, if exist cancel signal, quit thread (self request)
 * 
 * @param  exitcode (temporarily does not handle error codes)
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:host thread has been destroyed
 *          C4_BUSY:temporarily unable to respond to demand
 */ 
c4impl 
c4error c4thread_testcancel (void) {
  
  __c4constructor {
    
    c4error error_;
    c4threadImp threadImp = c4getop (tec);
    c4obj_check_thread (threadImp);
    
    /* hold self */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;
    else if ( threadImp->status == C4THREAD_STATUS_BUSY)
      {
        c4mutex_unlock_ (threadImp->lock, "c4thread_testcancel");
        return C4_BUSY;
      }
    else if ( threadImp->cancel_state == C4THREAD_CANCEL_DISABLE)
      {
        c4mutex_unlock_ (threadImp->lock, "c4thread_testcancel");
        return C4_BUSY;
      }
    // else if ( threadImp->cancel_type == C4THREAD_CANCEL_DEFFERED)
      {
        if (WaitForSingleObject (threadImp->cancel_event, 0) == WAIT_OBJECT_0)
          {
                    threadImp->status = C4THREAD_STATUS_BUSY;
                    threadImp->exmode = C4THREAD_EXIT_MODE_CALLCANEL;

                          ResetEvent (threadImp->cancel_event);
        selfcontext_set (& threadImp->destr_ctx);
             
            /* never reach here, only for warning.*/
                     return C4_OK;
         
          }
        else 
          {
            return C4_BUSY;
          }
      }
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Set the thread cancel-type
 * 
 * @param  c4thread object.
 * @param  C4THREAD_CANCEL_TYPE
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:deal thread is invailed object.
 *          C4_CPARAM:param error.
 *          C4_SFAIL:superfluous request
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:deal thread has been destroyed
 *          C4_BUSY:temporarily unable to respond to demand
 */ 
c4impl 
c4error c4thread_setcanceltype (c4thread thread, enum C4THREAD_CANCEL_TYPE ct) {
  
  __c4constructor {
    
    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);
     
    /* param2 check.*.*/
    if (ct != C4THREAD_CANCEL_ASYNCHRONOUS && ct != C4THREAD_CANCEL_DEFFERED)
      return C4_CPARAM;
    
    /* hold self. */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;
    else if ( threadImp->status == C4THREAD_STATUS_BUSY)
      {
        c4mutex_unlock_ (threadImp->lock, "c4thread_setcanceltype");
        return C4_BUSY;
      }
      
    /* 
     * let the two boring request out 
     * old:C4THREAD_CANCEL_ASYNCHRONOUS cur:C4THREAD_CANCEL_ASYNCHRONOUS
     * old:C4THREAD_CANCEL_DEFFERED cur:C4THREAD_CANCEL_DEFFERED
     */
    if (threadImp->cancel_type == ct)
      {
        c4mutex_unlock_ (threadImp->lock, "c4thread_setcanceltype");
        return C4_SFAIL;
      }
    else if (ct == C4THREAD_CANCEL_DEFFERED) 
      {
        /* ASYNCHRONOUS -> DEFFERED, nodone */
        threadImp->cancel_type = ct;

        c4mutex_unlock_ (threadImp->lock, "c4thread_setcanceltype");
        return C4_OK;
      }
    else if (ct == C4THREAD_CANCEL_ASYNCHRONOUS)
      {
        /* DEFFERED -> ASYNCHRONOUS, check async signal */
        threadImp->cancel_type = ct;
        if ( (threadImp->cancelPending != 0 || WaitForSingleObject (threadImp->cancel_event, 0) == WAIT_OBJECT_0) 
          &&  threadImp->cancel_state == C4THREAD_CANCEL_ENABLE )
          {
            enum C4THREAD_STATUS status_t = threadImp->status;
            enum C4THREAD_EXIT_MODE exmode_t = threadImp->exmode;

            /* set busy/cancel **/
            threadImp->status = C4THREAD_STATUS_BUSY;
            threadImp->exmode = C4THREAD_EXIT_MODE_CALLCANEL;

            /* self ? set self program pointer and register.*/
            if (threadImp->tid == & _g_tid)
              selfcontext_set (& threadImp->destr_ctx);

            if ( (error_ = c4thread_suspend (thread, null)) != C4_OK)
              goto cleanup0;
            else 
              error_ = c4thread_setcontext ( thread, & threadImp->destr_ctx);

            /* no good way to deal with the resumption of suspended failure */
            if ( c4thread_startup (thread, null) != C4_OK)
              MSG_E2 ( "c4thread_startup failed in c4thread_setcontext failed.", "c4thread_setcanceltype");

    cleanup0:
            if ( error_ != C4_OK) {
              /* resume old status **/
              threadImp->status = status_t;
              threadImp->exmode = exmode_t;
            }
          }
      }
      /******* return ******/
      c4mutex_unlock_ (threadImp->lock, "c4thread_setcanceltype");
      return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Set the thread to cancel signal phase
 * 
 * @param  c4thread object.
 * @param  C4THREAD_CANCEL_STATE
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:deal thread is invailed object.
 *          C4_CPARAM:param error.
 *          C4_SFAIL:superfluous request
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:deal thread has been destroyed
 *          C4_BUSY:temporarily unable to respond to demand
 */ 
c4impl 
c4error c4thread_setcancelstate (c4thread thread, enum C4THREAD_CANCEL_STATE cs) {
  
  __c4constructor {
    
    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);
    
    /* param2 check.*.*/
    if (cs != C4THREAD_CANCEL_ENABLE && cs != C4THREAD_CANCEL_DISABLE)
      return C4_CPARAM;
    
    /* hold self. */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;
    else if ( threadImp->status == C4THREAD_STATUS_BUSY)
      {
        c4mutex_unlock_ (threadImp->lock, "c4thread_setcancelstate");
        return C4_BUSY;
      }
      
    /* 
     * let the two boring request out 
     * old:C4THREAD_CANCEL_ENABLE cur:C4THREAD_CANCEL_ENABLE
     * old:C4THREAD_CANCEL_DISABLE cur:C4THREAD_CANCEL_DISABLE
     */
    if (threadImp->cancel_state == cs)
      {
        c4mutex_unlock_ (threadImp->lock, "c4thread_setcancelstate");
        return C4_SFAIL;
      }
    else if (cs == C4THREAD_CANCEL_ENABLE)
      {
        /* off -> on, cancel original mask */
        threadImp->cancel_state = cs;
        if (threadImp->cancelPending != 0) {
          if ( SetEvent (threadImp->cancel_event) == FALSE); /* TODO:release, but basic not fail. */
            MSG_E2 ("SetEvent failed in C4THREAD_CANCEL_ENABLE", "c4thread_setcancelstate");
          /* current is C4THREAD_CANCEL_ASYNCHRONOUS ? */
          if (threadImp->cancel_type == C4THREAD_CANCEL_ASYNCHRONOUS)
            {
              enum C4THREAD_STATUS status_t = threadImp->status;
              enum C4THREAD_EXIT_MODE exmode_t = threadImp->exmode;

              /* set busy/cancel **/
              threadImp->status = C4THREAD_STATUS_BUSY;
              threadImp->exmode = C4THREAD_EXIT_MODE_CALLCANEL;

              /* self ? set self program pointer and register.*/
              if (threadImp->tid == & _g_tid)
                selfcontext_set (& threadImp->destr_ctx);

              if ( (error_ = c4thread_suspend (thread, null)) != C4_OK)
                goto cleanup0;
              else 
                error_ = c4thread_setcontext ( thread, & threadImp->destr_ctx);

              /* no good way to deal with the resumption of suspended failure */
              if ( c4thread_startup (thread, null) != C4_OK)
                MSG_E2 ( "c4thread_startup failed in c4thread_setcontext failed.", "c4thread_setcancelstate");

      cleanup0:
              if ( error_ != C4_OK) {
                /* resume old status **/
                threadImp->status = status_t;
                threadImp->exmode = exmode_t;
              }
            }
        }
      }
    else if (cs == C4THREAD_CANCEL_DISABLE)
      {
        /* on -> off, set signal mask */
        threadImp->cancel_state = cs;
        /* kill cancel signal TODO:release */
        if ( ResetEvent (threadImp->cancel_event) == FALSE);
          MSG_E2 ("ResetEvent failed in C4THREAD_CANCEL_DISABLE", "c4thread_setcancelstate");
      }
      /******* return ******/
      c4mutex_unlock_ (threadImp->lock, "c4thread_setcancelstate");
      return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Cancel the specified thread
 * 
 * @param  c4thread object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:host thread is invailed object.
 *          C4_SFAIL:cancel shielded
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:host thread has been destroyed
 *          C4_BUSY:temporarily unable to respond to demand
 */ 
c4impl 
c4error c4thread_cancel (c4thread thread) {
  
  __c4constructor {
    
    c4error error_;
    c4threadImp threadImp = c4getop (thread);
    c4obj_check_thread (threadImp);
    
    /*
     * hold self thread before visit it.
     */
    if ( (error_ = c4mutex_lock (threadImp->lock)) != C4_OK)
      return error_;
    else if ( threadImp->status == C4THREAD_STATUS_BUSY)
      {
        c4mutex_unlock_ (threadImp->lock, "c4thread_cancel");
        return C4_BUSY;
      }
    else if ( threadImp->cancel_state == C4THREAD_CANCEL_DISABLE)
      {
        threadImp->status = C4THREAD_STATUS_FREE;
        threadImp->cancelPending = 1;

        c4mutex_unlock_ (threadImp->lock, "c4thread_cancel");
        return C4_SFAIL;
      }
    else if ( threadImp->cancel_type == C4THREAD_CANCEL_DEFFERED)
      {
        /* TODO:error check.may be wrong...0_o hahahahahah 。。。~ **/
        if ( WaitForSingleObject (threadImp->cancel_event, 0) == WAIT_OBJECT_0)
          threadImp->cancelPending = 1;
        else if ( SetEvent (threadImp->cancel_event) == FALSE)
          MSG_E2 ( "SetEvent failed. in C4THREAD_CANCEL_DEFFERED ", "c4thread_cancel");
        /* set cancelPending  i.e. http://man7.org/linux/man-pages/man3/pthread_cancel.3.html */
        threadImp->cancelPending = 1;
      }
    else
      {
        enum C4THREAD_STATUS status_t = threadImp->status;
        enum C4THREAD_EXIT_MODE exmode_t = threadImp->exmode;

        /* set busy/cancel **/
        threadImp->status = C4THREAD_STATUS_BUSY;
        threadImp->exmode = C4THREAD_EXIT_MODE_CALLCANEL;

        /* self ? set self program pointer and register.*/
        if (threadImp->tid == & _g_tid)
          selfcontext_set (& threadImp->destr_ctx);

        if ( (error_ = c4thread_suspend (thread, null)) != C4_OK)
          goto cleanup0;
        else 
          error_ = c4thread_setcontext ( thread, & threadImp->destr_ctx);

        /* no good way to deal with the resumption of suspended failure */
        if ( c4thread_startup (thread, null) != C4_OK)
          MSG_E2 ( "c4thread_startup failed in c4thread_cancel.", "c4thread_cancel");

cleanup0:
        if ( error_ != C4_OK) {
          /* resume old status **/
          threadImp->status = status_t;
          threadImp->exmode = exmode_t;
        }
      }
      /* unlock*/
      c4mutex_unlock_ (threadImp->lock, "c4thread_cancel");
      return error_;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/** 
 * Request time to attempt to wait the specified thread, timeout will return 
 *
 * @param  c4thread object.
 * @param  time to try.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:host or self thread is invailed object.
 *          C4_DEADLK:a deadlock was detected (e.g., two threads tried to join with
 *           each other); or thread specifies the calling thread.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:host or self thread has been destroyed
 *          C4_BUSY:another thread is already waiting to join with this thread or 
 *             temporarily unable to respond to demand
 *          C4_TIMEOUT:request timeout
 *          C4_PERM:thread is about to be destroyed 
 *          C4_PARAM:current is detach-thread 
 *          C4_INTR:cancel signal is masked, causing the routine to return
 */ 
c4impl 
c4error c4thread_timedjoin (c4thread thread, uint32 time) {
  
  __c4constructor {
    
    c4error error_;
    static intptr least_spin = 0;
    int32 join_mask_t;
    c4threadImp threadImpCall = c4getop (thread);
    c4threadImp threadImpSelf = c4getop (tec);
    struct c4thread_link_ *Q, *S;
    c4obj_check_thread (threadImpCall);
    c4obj_check_thread (threadImpSelf);
    
    if (threadImpCall == threadImpSelf)
      return C4_DEADLK;
    else if ( (join_mask_t = _cas32 (& threadImpCall->join_mask, 1, 0)) == -1)
      return C4_PERM;
    else if ( join_mask_t == 1)
      return C4_BUSY;
    
    /* hold self */
    if ( (error_ = c4mutex_lock (threadImpCall->lock)) != C4_OK) {
      _cas32 (& threadImpCall->join_mask, 0, 1);
      return error_;
    } else if (threadImpCall->detachable != FALSE) {
      _cas32 (& threadImpCall->join_mask, 0, 1);
      c4mutex_unlock_ (threadImpCall->lock, "c4thread_timedjoin");
      return C4_PARAM;
    } else if (threadImpSelf->status == C4THREAD_STATUS_BUSY) {
      c4mutex_unlock_ (threadImpCall->lock, "c4thread_timedjoin");
      return C4_BUSY;
    }

    /* set shield separation properties */
    threadImpCall->link_disable = TRUE;
    /* append current join thread's tid */
    threadImpCall->join_tid = & _g_tid;
    c4mutex_unlock_ (threadImpCall->lock, "c4thread_timedjoin");
    
    /* try hold self thread's lock. **/
    if ( (error_ = c4mutex_lock (threadImpSelf->lock)) != C4_OK) {
      _cas32 (& threadImpCall->join_mask, 0, 1);
      threadImpCall->link_disable = FALSE;
      return error_;
    }
    
    /* lock We must ensure that only one thread executes the code at the same time */
    while (_p_store (& least_spin, !null) != null)
      cpu_relax();
    
    if ( threadImpCall->join_tid == threadImpSelf->tid 
      && threadImpSelf->join_tid == threadImpCall->tid) {
        /*
         * join with each other, We cancel a request 
         */
         threadImpCall->link_disable = FALSE;
         _cas32 (& threadImpCall->join_mask, 0, 1);
         
         c4mutex_unlock_ (threadImpSelf->lock, "c4thread_timedjoin");
         
         /* unlock ***/
         _p_store (& least_spin, null);
         return C4_DEADLK;
    }
    
    /* unlock ***/
    _p_store (& least_spin, null);
    
    
    
    
      {
        DWORD waitResult;
        HANDLE handle_g[2];
    
        /*
         * map handle grounp.
         */
        handle_g[0] = threadImpCall->handle;
        handle_g[1] = threadImpSelf->cancel_event;  
        
        /* Give scheduler
         */
        waitResult = WaitForMultipleObjects
                     ( 2, &handle_g[0], FALSE, time);    
                         
             switch ( waitResult )
                   {
                   case WAIT_OBJECT_0:
                    /*
                     * Get the signal at the end of the thread.
                     */ 
                             /* set join disable mask. **/
                         threadImpCall->join_mask = -1;
                         
      if ( threadImpCall->exmode != C4THREAD_EXIT_MODE_NORMAL) {
        /* cleanup routine for c4thread_exit/c4thread_cancel  */
        cleanup_s2 (threadImpCall, TRUE);
      }
               /*  destroy thread handle, do it*/
        CloseHandle (threadImpCall->handle);
         /* 
         * wait thread lock.last time(i.e. non-cancelable)
         */
      c4mutex_lock (threadImpCall->lock);
      
         /* get global-lock. */
      EnterCriticalSection ( & _kthread.lock);
         /* 
         * foreach global-list delete current thread chunk.
         */
      
  for (Q = (S = _kthread.list)->level; 
       Q; 
       Q = (S = Q)->level) {
         
   if (Q->thread == thread) {
      /* find match, remove it, unlock.
       */
       S->level = Q->level;
       /* _m_access_barrier(); */
       
         /* geiveuppppp global-lock*/
      LeaveCriticalSection ( & _kthread.lock);
       
                /* destroy attach handle and c4mutex sync-lock*/

       c4mutex_destroy ( threadImpCall->lock);
       CloseHandle ( threadImpCall->cancel_event);
        
        /* cleanup routine for cleanmem  */
        cleanup_s2 (threadImpCall, FALSE);
        
       /* final release c4thread's data struct, game is over. */
       c4free (threadImpCall);
       return 0;
   }
  }
  
    /* assert exist */
    MSG_E2 ("we not find goal thread's data struct ", "c4thread_timedjoin");

    return C4_OK;
                     break;
                     
    /*
     * canceling !!!!.*/case WAIT_OBJECT_0 + 1: 
     
_cas32 (& threadImpCall->join_mask, 0, 1);
    /* hold self thread before visit it.*/
    if ( (error_ = c4mutex_lock (threadImpSelf->lock)) != C4_OK)
      return error_;
    else if ( threadImpSelf->status == C4THREAD_STATUS_BUSY)
        /* TODO:release .*/
        MSG_E2 ("cancel failed in thread-join", "c4thread_timedjoin");
    else if ( threadImpSelf->cancel_state == C4THREAD_CANCEL_DISABLE)
      {
        c4mutex_unlock_ (threadImpSelf->lock, "c4thread_timedjoin");
        return C4_INTR;
      }
     threadImpSelf->status = C4THREAD_STATUS_BUSY;
                          if (ResetEvent (threadImpSelf->cancel_event) == FALSE)
        MSG_E2 ("ResetEvent failed in canceling", "c4thread_timedjoin");
               
        selfcontext_set (& threadImpSelf->destr_ctx);
             
            /* never reach here*/
                   case WAIT_TIMEOUT:
                   /* cancel the join shield for the target thread */ 
_cas32 (& threadImpCall->join_mask, 0, 1);
                     return C4_TIMEOUT;

                   case WAIT_FAILED: 
                   default:          
                     return C4_KFAIL;                   
                   }
      }
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

