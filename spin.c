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

#include "spin.h"

/*
 * !!! spin maybe not suitable for use on a single core machine.
 */
typedef struct c4spin_ {
  int32 recursived;/* cannot be modified after assignment*/
  int32 signal, recursive; /* lock's status/count for c4spin_recursive */
  c4tid tid; /* owner to indicate lock */
  intptr obj_g_uuid; /* object unique identifier */
} c4spin_s, *c4spinImp;

/**
 * Unlock a spin lock
 *
 * @param  c4spin object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_PERM:calling thread does not hold the lock., or unlocked without locking
 *          C4_BADMEM:illegal memory operation
 */
c4impl 
c4error c4spin_unlock (c4spin spin) {
  
  __c4constructor {

    c4spinImp spinImp = c4getop (spin);
    c4obj_check_spin (spinImp);

    /* check owner*/
    if (spinImp->tid != & _g_tid)
      return  C4_PERM;
    
    /*
     * FIXME:optimizer will retain the semantics of the operator precedence??
     */
    if ( (spinImp->recursived == FALSE) || (--spinImp->recursive == 0) ) {
      spinImp->tid = null; 
       /* write barrier, serial. */
      _m_write_barrier();
      spinImp->signal = 0;
    }
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Unlock a spin lock plus.
 * 
 * @param  c4spin object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_PERM:calling thread does not hold the lock., or unlocked without locking
 *          C4_BADMEM:illegal memory operation
 *
 * @remark  For non recursive locks, the behavior is the same as for c4spin_unlock. 
 *          For recursive locks, it will cancel all recursive counts and hand over the ownership of the lock
 */
c4impl 
c4error c4spin_unlock_root (c4spin spin) {
  
  __c4constructor {

    c4spinImp spinImp = c4getop (spin);
    c4obj_check_spin (spinImp);

    /* check owner*/
    if (spinImp->tid != & _g_tid)
      return  C4_PERM;
    
     /* empty tid. */
    spinImp->tid = null; 
     /* write barrier, serial. */
    _m_write_barrier();
    
    spinImp->signal = 0;
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Destroy a spin lock
 *
 * @param  c4spin object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 */
c4impl 
c4error c4spin_destroy (c4spin spin) {
  
  __c4constructor {

    c4spinImp spinImp = c4getop (spin);
    c4obj_check_spin (spinImp);

    spinImp->obj_g_uuid = obj_g_invailed;
    c4free (spinImp);
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * lock a spin lock
 * 
 * @param  c4spin object.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_DEADLK:resource deadlock would occur[Just for non recursive locks]
 *          C4_BADMEM:illegal memory operation
 */
c4impl 
c4error c4spin_lock (c4spin spin) {
  
  __c4constructor {

    c4spinImp spinImp = c4getop (spin);
    c4obj_check_spin (spinImp);

    /* 
     * current owner ? 
     */
    if (spinImp->tid == & _g_tid) {
      if (spinImp->recursived != TRUE)
        return C4_DEADLK;
        ++spinImp->recursive;
        return C4_OK;
    }

    while ( _store32 (& spinImp->signal, 1) != 0)
      cpu_relax();
    
    spinImp->recursive = 1;
    spinImp->tid = & _g_tid;
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * trylock a spin lock
 * 
 * @param  c4spin object.
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
c4error c4spin_trylock (c4spin spin) {
  
  __c4constructor {

    c4spinImp spinImp = c4getop (spin);
    c4obj_check_spin (spinImp);

    /* 
     * current owner ? 
     */
    if (spinImp->tid == & _g_tid) {
      if (spinImp->recursived != TRUE)
        return C4_DEADLK;
        ++spinImp->recursive;
        return C4_OK;
    }

    if ( _store32 (& spinImp->signal, 1) != 0)
      return C4_BUSY;
    
    spinImp->recursive = 1;
    spinImp->tid = & _g_tid;
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Gets the ID of the thread that owns the lock
 *
 * @param  c4spin object.
 * @param  c4tid accept pointer, If there is no owner then will return to null
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, will return 
 *    
 *          C4_INVAL:invailed object.
 *          C4_BADMEM:illegal memory operation
 */
c4impl 
c4error c4spin_getowner (c4spin spin, c4tid *tid) {
  
  __c4constructor {

    c4spinImp spinImp = c4getop (spin);
    c4obj_check_spin (spinImp);

    *tid = _p_cas ( &spinImp->tid, 0, 0);
    return C4_OK;
  }
  __c4exception (ACCESS_VIOLATION) {
    return C4_BADMEM;
  }
}

/**
 * Init a spin.
 *
 * @param  accept pointer, if successful then spin points to a valid c4spin object.
 * @param  indicate whether it can be recursive.
 *
 * @return  If the function succeeds, the return value is C4_OK.
 *          If the function fails, the return value is C4_NOMEM.
 */
c4impl 
c4error c4spin_init (c4spin *spin, lean32 recursive) {
  
  c4spinImp spinImp = c4memalign (16, sizeof (c4spin_s));
  
  if (spinImp != null) 
   {
     spinImp->signal         = 0;
     spinImp->tid            = null;
     spinImp->obj_g_uuid     = obj_g_spin;
     spinImp->recursived     = !!recursive;
    
    *spin = c4setop (spinImp);
     return C4_OK;
   }
  else 
   {
     return C4_NOMEM;
   }
}