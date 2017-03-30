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

#include "condition.h"

/*
 * Simple queue simulation waiting thread
 */
struct squeue {
  struct squeue *level;
  void *any;
};

c4finline 
void enqueue (struct squeue *_squeue, 
             struct squeue * newREQ) {
  
  /* we always inert tail, get head. **/
  struct squeue *Q, *C = _squeue;
  newREQ->level = null;
  
  for ( Q = _squeue->level;
        Q!= null;
        Q = (C = Q)->level)
     ;  C->level = newREQ;
}

c4finline 
struct squeue *dequeue (struct squeue *_squeue) {
  
  /* current is empty queue ?. **/
  if (_squeue->level == null)
    return null;
  else 
    {
      /* Get head, clear node. */
      struct squeue *cur = _squeue->level;
      _squeue->level = _squeue->level->level;
      
      return cur;
    }
}

c4finline 
lean32 delspec_queue (struct squeue *_squeue, 
              struct squeue *specify) {
  
  /* current is empty queue ?. **/
  if (_squeue->level == null)
    return FALSE;
  else 
    {
      struct squeue *Q;
      struct squeue *C = _squeue;
  
      for ( Q = _squeue->level;
            Q!= null;
            Q = (C = Q)->level)
          {
            if ( Q == specify)
            {
              C->level = Q->level;
              return TRUE;
            }
          }
            
      return FALSE;
    }
}

/*
 * not support for cancellation point/robust lock, destroy sensitive
 */
typedef struct c4mutex_ {
  int32 recursived;/* cannot be modified after assignment*/
  c4tid tid; /* owner to indicate lock */
  int32 sig_waiter, recursive; /* lock's status/count for c4mutex_recursive */  
  intptr obj_g_uuid; /* object unique identifier */
  HANDLE _destroy, _lock; /* manual-reset/ auto-reset event. */
} c4mutex_s, *c4mutexImp;

/*
 * source not mine.
 * see linuxthreads 's condvar.c
 *
 * algorithm reference
 *    :http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
 *    :http://groups.google.com/group/comp.programming.threads/browse_frm/thread/1692bdec8040ba40/e7a5f9d40e86503a
 *    :https://www.microsoft.com/en-us/research/publication/implementing-condition-variables-with-conditions/
 *    :http://birrell.org/andrew/papers/ImplementingCVs.pdf
 */
typedef struct c4condition_ {
  struct squeue queue; /* same as name.. */
  struct c4mutex_ *lock; /*  */
  intptr obj_g_uuid; /* object unique identifier */
} c4condition_s, *c4conditionImp;

c4impl c4error c4condition_signal (c4condition condition);
c4impl c4error c4condition_broadcast (c4condition condition);
c4impl c4error c4condition_wait (c4condition condition, c4mutex ind_lock);
c4impl c4error c4condition_timedwait (c4condition condition, c4mutex ind_lock, uint32 time);
c4impl c4error c4condition_destroy (c4condition condition);
c4impl c4error c4condition_init (c4condition *condition);