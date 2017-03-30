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
 
#include "attribute.h"
#include <Windows.h>

/*
 * object identifier, mismatch will return X_INVAL
 */
intptr obj_g_invailed = 0; /* always 0 */
intptr obj_g_thread; 
intptr obj_g_key;
intptr obj_g_mutex; 
intptr obj_g_mcs_mutex; 
intptr obj_g_rwmutex;
intptr obj_g_spin; 
intptr obj_g_mcs_spin; 
intptr obj_g_rwspin; 
intptr obj_g_semaphore; 
intptr obj_g_condition; 
intptr obj_g_event; 
intptr obj_g_barrier; 
intptr obj_g_coroutine;

/*
 * base init function, used to expand the object guid init
 */
c4finline
void s_initGuid (intptr *__uid) {
  
  static union {
    GUID s_dummy;
    intptr s_real;
  } stem;
  
  do 
  { 
    CoCreateGuid (& stem.s_dummy);
  } while (stem.s_real == 0);  
  
  *__uid = stem.s_real;
}

/* @return 0:success, -1:fail */
int32 c4guid_einit (void) {
  
  s_initGuid (& obj_g_thread);
  s_initGuid (& obj_g_key);
  s_initGuid (& obj_g_mutex);
  s_initGuid (& obj_g_mcs_mutex);
  s_initGuid (& obj_g_rwmutex);
  s_initGuid (& obj_g_spin);
  s_initGuid (& obj_g_mcs_spin);
  s_initGuid (& obj_g_rwspin);
  s_initGuid (& obj_g_semaphore);
  s_initGuid (& obj_g_condition);
  s_initGuid (& obj_g_event);
  s_initGuid (& obj_g_barrier);
  s_initGuid (& obj_g_coroutine);

  return 0;
}
