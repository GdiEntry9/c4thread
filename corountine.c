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
\*----------------------------------------------------------------------------
 *  thanks smith, taught me a lot about the knowledge of asm/thread and winNT
 *----------------------------------------------------------------------------*/
 
#include "coroutine.h"

/*  forward declaration */
struct c4coroutine_link_;
struct c4coroutine_;

/*  always point to current coroutine.*/
static 
c4tls void *curcoro = null;

/*  coroutine of the same thread shares a lock.*/
static 
c4tls c4spin co_spinlock;

/*  always point to the top coroutine list.*/
static 
c4tls c4coroutine_link_ *lv2_corolink = null;

/*  first initialized in thread. */
static 
c4tls lean32 IRST_init = FALSE;

/*  same as name.*/
static 
c4tls lean32 success_ON_envinit = FALSE;

/*  coroutine nums in thread .*/
static 
c4tls uint32 coroutine_nums = 0;

/*
 *  c4coroutine struct.
 */
typedef struct c4coroutine_ {
  struct c4context cur_ctx, except_ctx2, final_ctx;
  struct c4except_ except_;
  struct c4coroutine_link_ *child_chain; 
  struct c4coroutine_link_ *level_chain; 
  struct c4coroutine_ *parent_; 
  void *heap_buf, cls_buf; /* current coroutine's environment/local storage stack */
  intptr obj_g_uuid; /* object unique identifier */
  int32 space_size; /* stack space. **/
  int32 status; /*  */
  int32 params_len;
  intptr *params_set;
  lean32 collctx; /* collect context in inital time **/
  lean32 care_fpu /* collect fpu-context ?*/
  c4tid _tid; /* host thread's id */
  c4spin _spin; /* simplicity, we use spin locks for synchronization [in the different threads of the coroutine]*/
} c4coroutine_s;

/*
 *  c4coroutine link.
 */
struct c4coroutine_link_ {
  struct c4coroutine_link_ *level;
  struct c4coroutine_ *coroutine;
};

static 
lean32 initialize_coro_onThread (void) {
  
}


static c4error
c4callc __comgr_c (struct c4coroutine_ *coroutine) {
  
}

/**
 * Create a coroutine. 
 * 
 * @param  c4coroutine accecpt 
 * @param  space size,  the default argument is passed to 0 (default 2MB space)
 * @param  collect fpu context?
 * @param  routine
 * @param  variable parameter
 * 
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_NOMEM:biggest possibility is not enough heap space.
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:host thread has been destroyed
 *          C4_CPARAM:call paramter errors.
 *          C4_IRREV:environment initialization failed
 */ 
c4impl c4error 
c4callc c4coroutine_create_unused (c4coroutine *coroutine, int32 space, lean32 care_fpu, void *routine, ...) {
	
  struct c4coroutine_ *coro = null;// = c4memalign (16, sizeof (c4coroutine_s));
  struct c4coroutine_link_ *child_ = null;// = c4malloc ( sizeof (struct c4coroutine_link_)); 
  struct c4coroutine_link_ *self_ = null;// = c4malloc ( sizeof (struct c4coroutine_link_)); 
//struct c4coroutine_link_ *level_ = null;// = c4malloc ( sizeof (struct c4coroutine_link_));
//struct c4coroutine_ *parent_ = curcoro;
  void *cls_buf = null, heap_buf = null;
//c4spin spin_t;
  c4error error_ = C4_NOMEM;
  lean32 do_spin = FALSE; /* only for GCC..*/
  int32 space_ = space ? space : (1024 * 1024 * 2); /* 2MB = 2048KB = 1024KB * 2 = 1024 * 1024 * 2 **/
  int32 space2_= 1024 * 1024; /* default 1MB */
//lean32 top_link = (curcoro != null) ? TRUE : FALSE;
  intptr *sbt_off = & ( ( (intptr *)& routine) [1]);
  int32 pa_len = 0;
  intptr *pa_set = null; 
  
  /* base check...**?*/
  if (IRST_init == TRUE && success_ON_envinit == FALSE)
    return C4_IRREV;
  /* routine is null ?**/
  else if (routine == null)
    return C4_CPARAM;
  
  /* fisrt wo calc callback's param buffer.***/
  while ( sbt_off[pa_len+0] != C4COroutine_EOF_MAGIC_HEAD1
     ||   sbt_off[pa_len+1] == C4COroutine_EOF_MAGIC_HEAD2)
     pa_len++;
     
  if (pa_len != 0)
     {
       if ( (pa_set = c4malloc ( sizeof (intptr_t) * pa_len)) == null)
         return C4_NOMEM;
       else /* copy param to buf, The indefinite parameter is magical, 
               and many compilers enforce the floating point data to \
               follow the calling rules of the integer data
             */
         memcpy ( & pa_set, sbt_off, sizeof (intptr_t) * pa_len);
     }
  
  /* init base data struct/alloc context heap space ***/
  coro = c4memalign (16, sizeof (c4coroutine_s));
  child_ = c4malloc ( sizeof (struct c4coroutine_link_)); 
  self_ = c4malloc ( sizeof (struct c4coroutine_link_)); 
  cls_buf= c4memalign (16, space2_);
  heap_buf= c4memalign (16, space_);
//level_ = c4malloc ( sizeof (struct c4coroutine_link_));
  
  if (child_ == null || coro == null || self_ == null
    || cls_buf == null || heap_buf == null
   /*|| c4spin_init (& spin_t, TRUE ) */
   /*|| level_ == null*/)
    goto cleanup0;
  else 
    {
      /* empty list head... **?*/
      child_->level = null;
      self_->level = null;
      /* make self link */
      self_->coroutine = coro;
    //level_->level = null;
      do_spin = TRUE;
    }

  /* set init**/
  coro->obj_g_uuid = obj_g_coroutine;
//coro->level_chain = level_;
  coro->space_size = space_;
  coro->params_len = pa_len;
  coro->params_set = pa_set;
  coro->collctx = FALSE;
  coro->care_fpu = !! care_fpu;
  coro->_tid = & _g_tid;
  coro->_spin = co_spinlock;
  coro->parent_ = curcoro;
  coro->heap_buf= heap_buf;
  coro->cls_buf = cls_buf;
  coro->child_chain = child_;
  coro->status = -1;
  
  /* set parent list, if exist **/
  if (curcoro != null)
    {
      /* exist parent, try get parent's spin-lock */
      error_ = c4spin_lock (co_spinlock);
      if (error_ != C4_OK)
        goto cleanup0;
      /* insert list-head.**/
      self_->level = coro->parent_->child_chain->level;
      coro->parent_->child_chain->level = self_->level;
      coro->level_chain = self_;
      /* unlock.. return **/
      c4spin_unlock (co_spinlock);
      coroutine_nums++;
      return C4_OK;
    }
  else 
    {
      /* top coroutine..**/
      if (initialize_coro_onThread() != TRUE)
        {
          error_ = C4_IRREV;
          goto cleanup0;
        }
      else 
        {
      /* not exist parent, try get parent's spin-lock */
          error_ = c4spin_lock (co_spinlock);
          if (error_ != C4_OK)
            goto cleanup0;
      /* insert list-head.**/
          self_->level = lv2_corolink->level;
          lv2_corolink->level = self_->level;
          coro->level_chain = self_;
          /* unlock.. return **/
          c4spin_unlock (co_spinlock);
          coroutine_nums++;
          return C4_OK;
        }
    }

cleanup0:
     /* XXX:Constant memory fragmentation optimization */
    if (heap_buf != null) c4free (heap_buf);
    if (cls_buf != null) c4free (cls_buf);
    if (pa_set != null) c4free (pa_set);
    if (self_ != null) c4free (self_);
    if (child_ != null) c4free (child_);
    if (do_spin != FALSE) c4spin_destroy (spin_t);
    if (coro != null) c4free (coro);
    
    return error_;
}

c4impl c4error c4callc c4coroutine_exec (c4coroutine coroutine);
c4impl c4error c4callc c4coroutine_status (c4coroutine coroutine, lean32 *is_running);
c4impl c4error c4callc c4coroutine_kill (c4coroutine coroutine);
c4impl c4error c4callc c4coroutine_host (c4coroutine coroutine, c4thread *thread);
c4impl c4error c4callc c4coroutine_hostid (c4coroutine coroutine, c4tid *tid);
c4impl c4error c4callc c4coroutine_current (c4coroutine *coroutine);
c4impl c4error c4callc c4coroutine_nums (uint32 *nums);
c4impl c4error c4callc c4coroutine_link (void **link); /* link element idx0:level, idx1:coroutine (all void *)*/
c4impl c4error c4callc c4coroutine_spinlock (c4spin *spin);
c4impl c4error c4callc c4coroutine_nums_ex (c4tid tid, uint32 *nums);
c4impl c4error c4callc c4coroutine_link_ex (c4tid tid, void **link); /* link element idx0:level, idx1:coroutine (all void *)*/
c4impl c4error c4callc c4coroutine_spinlock_ex (c4tid tid, c4spin *spin);



