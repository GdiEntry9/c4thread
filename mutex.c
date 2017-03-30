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

#include "mutex.h"

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

/**
 * Unlock a mutex lock
 *
 * @param  c4mutex object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_PERM:calling thread does not hold the lock., or unlocked without locking
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 */
c4impl 
c4error c4mutex_unlock (c4mutex mutex) {
  
  __c4constructor {

    c4mutexImp mutexImp = c4getop (mutex);
    c4obj_check_mutex (mutexImp);

    /* check owner*/
    if (mutexImp->tid != & _g_tid)
      return  C4_PERM;
    
    /*
     * FIXME:optimizer will retain the semantics of the operator precedence??
     */
    if ( (mutexImp->recursived == FALSE) || (--mutexImp->recursive == 0) ) {
      mutexImp->tid = null; 
      if (_cas32 (& mutexImp->sig_waiter, 0, 0x80000000) != 0x80000000) {
              if ( SetEvent (mutexImp->_lock) == FALSE)
                return C4_KFAIL;
            }
    }
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Unlock a mutex lock plus.
 * 
 * @param  c4mutex object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_PERM:calling thread does not hold the lock., or unlocked without locking
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *
 * @remark  For non recursive locks, the behavior is the same as for c4mutex_unlock. 
 *          For recursive locks, it will cancel all recursive counts and hand over the ownership of the lock
 */
c4impl 
c4error c4mutex_unlock_root (c4mutex mutex) {
  
  __c4constructor {

    c4mutexImp mutexImp = c4getop (mutex);
    c4obj_check_mutex (mutexImp);

    /* check owner*/
    if (mutexImp->tid != & _g_tid)
      return  C4_PERM;
    
    /*
     * FIXME:optimizer will retain the semantics of the operator precedence??
     */
    // if ( (mutexImp->recursived == FALSE) || (--mutexImp->recursive == 0) ) {
      mutexImp->tid = null; 
      if (_cas32 (& mutexImp->sig_waiter, 0, 0x80000000) != 0x80000000) {
              if ( SetEvent (mutexImp->_lock) == FALSE)
                return C4_KFAIL;
            }
    //}
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Destroy a mutex lock
 *
 * @param  c4mutex object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 *
 * @remark  Destruction is mandatory, no matter whether the lock is locked
 */
c4impl 
c4error c4mutex_destroy (c4mutex mutex) {
  
  __c4constructor {

    HANDLE _lock, _destroy;
    c4mutexImp mutexImp = c4getop (mutex);
    c4obj_check_mutex (mutexImp);

    _destroy = mutexImp->_destroy;
    _lock = mutexImp->_lock;
    
    mutexImp->obj_g_uuid = obj_g_invailed;
    
    /*
     * Close _lock handle.
     * Broadcast destroy event .
     */
    CloseHandle (_lock);
    SetEvent (_destroy);
    CloseHandle (_destroy);
    
    /*
     * Destroy mutex object's heap.
     */   
    c4free (mutexImp);
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Atomic execution, if it is 0 will be directly written 0x80000000 otherwise direct +1
 * @return:old value
 */
c4finline
int32 atomic_inc_Z80000000h ( volatile int32 *addend) {
  
  {
    int32 previous;
    int32 current; 
    
    do 
    { 
      if ( (previous = addend[0]) == 0) current = 0x80000000;     
      else                        current = previous + 1;            
      
    } while ( 
    _cas32 
    (  addend, current, previous) != previous); return previous;
  }
}

/**
 * Request time to attempt to lock mutex, timeout will return
 * 
 * @param  c4mutex object.
 * @param  time to try.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_DEADLK:resource deadlock would occur[Just for non recursive locks]
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:object has been destroyed
 *          C4_TIMEOUT:request timeout
 */
c4impl 
c4error c4mutex_timedlock (c4mutex mutex, uint32 time) {
  
  __c4constructor {

    c4mutexImp mutexImp = c4getop (mutex);
    c4obj_check_mutex (mutexImp);

    /* 
     * current owner ? 
     */
    if (mutexImp->tid == & _g_tid) 
         {
            /* #1:normal mode will return C4_DEADLK */
            if (mutexImp->recursived != TRUE)
              return C4_DEADLK;
            /* #2:add recursive count. */
              ++mutexImp->recursive;
              return C4_OK;
         }

    if ( atomic_inc_Z80000000h (& mutexImp->sig_waiter) != 0)
         {
             DWORD waitResult;
             HANDLE handle_g[2];
             
            /*
             * map handle grounp.
             */
             handle_g[0] = mutexImp->_lock;
             handle_g[1] = mutexImp->_destroy;        

            /* Give scheduler
             */
             waitResult = WaitForMultipleObjects
                         ( 2, &handle_g[0], FALSE, time);         
                         
             switch ( waitResult )
                   {
                   case WAIT_OBJECT_0:
                    /*
                     * cancel one wait.
                     */ 
                     _gradual32 (& mutexImp->sig_waiter, -0x00000001);
                     break;
                     
                   case WAIT_OBJECT_0 + 1: 
                     return C4_DESTROY;
                     
                   case WAIT_TIMEOUT:
                    /*
                     * cancel one wait. return immediately
                     */ 
                     _gradual32 (& mutexImp->sig_waiter, -0x00000001);
                     return C4_TIMEOUT;
                   case WAIT_FAILED: 
                   default:          
                     return C4_KFAIL;                   
                   }
             
         }
         
        /*
         * this branch is only for the first time or 
         * waiting for the signal to arrive
         */
         mutexImp->recursive = 1;
         mutexImp->tid = & _g_tid;
         return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}


/**
 * lock a mutex lock
 * 
 * @param  c4mutex object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_DEADLK:resource deadlock would occur[Just for non recursive locks]
 *          C4_BADMEM:illegal memory operation
 *          C4_KFAIL:kernel object failed.
 *          C4_DESTROY:object has been destroyed
 */
c4impl 
c4error c4mutex_lock (c4mutex mutex) {
  return c4mutex_timedlock (mutex, INFINITE);
}

/**
 * trylock a mutex lock
 * 
 * @param  c4mutex object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_DEADLK:resource deadlock would occur[Just for non recursive locks]
 *          C4_BUSY:a thread currently holds the lock
 *          C4_BADMEM:illegal memory operation
 */
c4impl 
c4error c4mutex_trylock (c4mutex mutex) {
  
  __c4constructor {

    c4mutexImp mutexImp = c4getop (mutex);
    c4obj_check_mutex (mutexImp);

    /* 
     * current owner ? 
     */
    if (mutexImp->tid == & _g_tid) {
      if (mutexImp->recursived != TRUE)
        return C4_DEADLK;
        ++mutexImp->recursive;
        return C4_OK;
    }

    if ( _cas32 (& mutexImp->sig_waiter, 0x80000000, 0) != 0)
      return C4_BUSY;
    
    mutexImp->recursive = 1;
    mutexImp->tid = & _g_tid;
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Gets the ID of the thread that owns the lock
 *
 * @param  c4mutex object.
 * @param  c4tid accept pointer, If there is no owner then will return to null
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 */
c4impl 
c4error c4mutex_getowner (c4mutex mutex, c4tid *tid) {
  
  __c4constructor {

    c4mutexImp mutexImp = c4getop (mutex);
    c4obj_check_mutex (mutexImp);

    *tid = _p_cas ( &mutexImp->tid, 0, 0);
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}


/**
 * Init a mutex.
 *
 * @param  accept pointer, if successful then mutex points to a valid c4mutex object.
 * @param  indicate whether it can be recursive.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_KFAIL:kernel object failed.
 *          C4_NOMEM:memory allocation failed
 */
c4impl 
c4error c4mutex_init (c4mutex *mutex, lean32 recursive) {
  
  c4mutexImp mutexImp = c4memalign (16, sizeof (c4mutex_s));
  
  if (mutexImp != null) 
   {
     mutexImp->sig_waiter     = 0;
     mutexImp->tid            = null;
     mutexImp->obj_g_uuid     = obj_g_mutex;
     mutexImp->recursived     = !!recursive;
     mutexImp->_lock     = CreateEvent (null, FALSE, FALSE, null);
     mutexImp->_destroy  = CreateEvent (null, TRUE, FALSE, null);
    *mutex = c4setop (mutexImp);
     
    if (mutexImp->_destroy && mutexImp->_lock )
     {
       return C4_OK;
     }
    else 
     {
     /* time to clean up */
       c4mutex_destroy (*mutex);
       return C4_KFAIL;
     }
   }
  else 
   {
     return C4_NOMEM;
   }
}

/**
 * Check a mutex type.
 *
 * @param  c4mutex object.
 * @param  write accept pointer, If it is not a recursive lock, it will return FALSE
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 */
c4impl 
c4error c4mutex_is_recursive (c4mutex mutex, lean32 *recursive) {
  
  __c4constructor {

    c4mutexImp mutexImp = c4getop (mutex);
    c4obj_check_mutex (mutexImp);

    *recursive = !!mutexImp->recursived;
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}