/* pthread-sem sync test1 for GCC. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> /* pthread included */
#include <semaphore.h> /* sem inclued */

#define  GARRAY_MAX 32
#define  THREAD_MAX 16
#define  SEM_ICOUNT 0x7F000
#define  null ((void*)0)

static int32_t    sg_array[GARRAY_MAX];
static bool       sg_quit_instance = false;
static pthread_t  sg_ths[THREAD_MAX];
static sem_t      sg_semt = null;

int sync_chk_rountine(void *param) {

  while (sg_quit_instance == false) {
    
    int32_t idx;
    sem_wait(&sg_semt);

    for (idx = 0; idx != GARRAY_MAX; idx++) {
      sg_array[idx] += 5;
    }
    
    {
      int32_t in, out;
      
      for (out = 0; out != GARRAY_MAX; out++)
      for (in = 0; in != GARRAY_MAX; in++)
      {
        if (sg_array[out] != sg_array[in])
         printf("not equal\n");
      }
    }

    sem_post(&sg_semt);
  }
  return 0;
}

int main(void) {
  
  int32_t idx;
  
  /* init global things. */
  memset(&sg_array[0], 0, sizeof(sg_array));
  memset(&sg_ths[0], 0, sizeof(sg_ths));
  
  if (sem_init(&sg_semt, null, SEM_ICOUNT) != 0) {
    /* create sem failed. */
    printf("create sem failed.");
    return 0;
  }

  /* create pthread. */
  for (idx = 0; idx != THREAD_MAX; idx++) {
    if (0 != pthread_create(&sg_ths[idx], null, sync_chk_rountine, null)) {
      /* thread create failed. */
      printf("create thread failed. max:%d, cur:%d", THREAD_MAX, idx);
      goto cleanup;
    }
  }

  /* assert quit. use key. */
  getch();

  cleanup:
    /* Quit Testthread. */
  {
    /* set siganlquit. */
    sg_quit_instance = true;

    for (idx = 0; idx != THREAD_MAX; idx++) {
      
      if (sg_ths[idx]!= null) {
        /* wait thread. */
        pthread_join(sg_ths[idx], null);
      }
    }
  }

  /* do free sem. */
  if (sg_semt != null) {
    sem_destroy(&sg_semt);
  }

  return 0;
}
