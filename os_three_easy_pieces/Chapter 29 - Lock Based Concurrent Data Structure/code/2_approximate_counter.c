#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

// I think we only see beinifit of approx counter, when some other thread tries to access the global value, since only local mutexes will be used most of the time, thats going to 
// improve
#define NUMCPU 16

typedef struct {
  int global_counter;  // global global_counter
  pthread_mutex_t global_lock; // global lock
  
  int local_counters[NUMCPU];  // array of 2 counter
  pthread_mutex_t local_locks[NUMCPU]; // array of 2 locsk

  int threshold; // threshold when breached, the global_lock is updated

} approx_counter_t;

approx_counter_t *init_approx_counter() {
  approx_counter_t *ct = (approx_counter_t*) malloc(sizeof(approx_counter_t));
  if (ct == NULL) {
    printf("Unable to allocate for approx_counter_t\n");
    return NULL;
  }
  ct->threshold=4;
  pthread_mutex_init(&ct->global_lock,NULL);
  for (int i = 0; i < NUMCPU; i++) {
    ct->local_counters[i]=0;
    pthread_mutex_init(&ct->local_locks[i],NULL);
  }

  return ct;
}


void destroy(approx_counter_t *ct) {
  for (int i = 0; i < NUMCPU; i++) {
    pthread_mutex_destroy(&ct->local_locks[i]);
  }
  free(ct);
}

void increment(approx_counter_t *ct, int id) {
  if (id >= NUMCPU) {
    printf("increment(): thread id out of index for %d\n",id);
    return;
  }

  // only update local value
  pthread_mutex_lock(&ct->local_locks[id]);
  ct->local_counters[id] +=1;

  if (ct->local_counters[id] >= ct->threshold) {
    pthread_mutex_lock(&ct->global_lock);
    ct->global_counter += ct->local_counters[id];
    pthread_mutex_unlock(&ct->global_lock);
    ct->local_counters[id] =0;
  }
  pthread_mutex_unlock(&ct->local_locks[id]);
}

int get(approx_counter_t *ct) {
  pthread_mutex_lock(&ct->global_lock);
  int id = ct->global_counter;
  pthread_mutex_unlock(&ct->global_lock);
  return id;
}


typedef struct {
  int id;
  approx_counter_t *ct;
} arg_t;

void *test_threads(void *arg) {
  arg_t *args = (arg_t*) arg;
  int id = args->id;
  approx_counter_t *ct = args->ct;
  
  for (int i = 0; i < 1000000; i++) {
    increment(ct,id);
  }

  return NULL;
}

int main() {
  approx_counter_t *ct = init_approx_counter();
  if (ct == NULL) {
    printf("main(): unable to allocate memory for approx_counter_t\n");
    return EXIT_FAILURE;
  }

  pthread_t threads[NUMCPU];
  arg_t args[NUMCPU];

  for (int i = 0; i < NUMCPU; i++) {
    // Only lives till loops strack frame and rewritten on next iteration
    //pthread_t p;
    //thread_infos[i] = &p;
    //arg_t arg = {
    //  .id = i,
    //  .ct = ct,
    //};
    //

    args[i].id = i;
    args[i].ct =ct;
    if (pthread_create(&threads[i],NULL,test_threads,&args[i]) != 0) {
      printf("main(): failed to create thread %d\n",i);
      goto err_clean_up;
    }
  }


  for (int i = 0; i < NUMCPU; i++) {
    pthread_join(threads[i],NULL);
  }

  printf("main(): all threads have finished their job\n");
  printf("main(): Final value is %d\n",get(ct));
  printf("main(): running success cleanup\n");
  destroy(ct);
  return EXIT_SUCCESS;

err_clean_up:
  printf("main(): running failure cleanup");
  destroy(ct);
  return EXIT_FAILURE;

}

