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
 
#if !defined (c4guid_included_MOECMKS)
#define c4guid_included_MOECMKS

#include "attribute.h"

/*
 * object identifier, mismatch will return X_INVAL
 */
#if defined (__cplusplus)
extern "C" { 
intptr
#else 
extern intptr
#endif 

obj_g_invailed, /* always 0 */
obj_g_thread, 
obj_g_key,
obj_g_mutex,
obj_g_mcs_mutex,
obj_g_rwmutex,
obj_g_spin, 
obj_g_mcs_spin, 
obj_g_rwspin, 
obj_g_semaphore, 
obj_g_condition, 
obj_g_event, 
obj_g_barrier;

#if defined (__cplusplus)
}
#endif 

/*
 * facilitate import...
 */
#define c4thread void *
#define c4tid void *
#define c4key void *
#define c4mutex void *
#define c4mcs_mutex void *
#define c4rwmutex void *
#define c4spin void *
#define c4mcs_spin void *
#define c4rwspin void *
#define c4semaphore void *
#define c4condition void *
#define c4event void *
#define c4barrier void *

/*
 * base check MARCO, used to expand the object guid test
 */
#define uidCksXxX(objDummyCombineXxX, objGuidXxX)     \
  do {                                                \
    if (objDummyCombineXxX->obj_g_uuid != objGuidXxX) \
    {                                                 \
      return C4_INVAL;                               \
                                                      \
    }                                                 \
  } while (0)

/*
 * expand the object guid test
 */
#define c4obj_check_thread(oBjXxX) uidCksXxX(oBjXxX, obj_g_thread)
#define c4obj_check_key(oBjXxX) uidCksXxX(oBjXxX, obj_g_key)
#define c4obj_check_mutex(oBjXxX) uidCksXxX(oBjXxX, obj_g_mutex)
#define c4obj_check_mcs_mutex(oBjXxX) uidCksXxX(oBjXxX, obj_g_mcs_mutex)
#define c4obj_check_rwmutex(oBjXxX) uidCksXxX(oBjXxX, obj_g_mcs_mutex)
#define c4obj_check_spin(oBjXxX) uidCksXxX(oBjXxX, obj_g_spin)
#define c4obj_check_mcs_spin(oBjXxX) uidCksXxX(oBjXxX, obj_g_mcs_spin)
#define c4obj_check_rwspin(oBjXxX) uidCksXxX(oBjXxX, obj_g_mcs_mutex)
#define c4obj_check_semaphore(oBjXxX) uidCksXxX(oBjXxX, obj_g_semaphore)
#define c4obj_check_condition(oBjXxX) uidCksXxX(oBjXxX, obj_g_condition)
#define c4obj_check_event(oBjXxX) uidCksXxX(oBjXxX, obj_g_event)
#define c4obj_check_barrier(oBjXxX) uidCksXxX(oBjXxX, obj_g_barrier)

#endif