#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define NUMCPU 16

// thread safe counter type
typedef struct {
  int value;
  pthread_mutex_t lock;
} counter_t;

typedef struct {
  int id;
  counter_t *c;
} arg_t;

counter_t* init_counter() {
  counter_t *counter = (counter_t*) malloc(sizeof(counter_t));
  if (counter == NULL) {
    printf("Allocation for counter_t type failed\n");
    return NULL;
  }

  counter->value = 0;

  if (pthread_mutex_init(&counter->lock,NULL) != 0) {
    free(counter);
    printf("Mutex initializtion failed\n");
    return NULL;
  }

  return counter;
}

void increment(counter_t *c) {
  pthread_mutex_lock(&c->lock);
  c->value++;
  pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c) {
  pthread_mutex_lock(&c->lock);
  c->value++;
  pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c) {
  pthread_mutex_lock(&c->lock);
  int v = c->value;
  pthread_mutex_unlock(&c->lock);
  return v;
}

void destroy(counter_t *c) {
  pthread_mutex_destroy(&c->lock);
  free(c);
}



void *test_threads(void *arg) {
  
  arg_t *carg = (arg_t*) arg;

  counter_t *c = carg->c;
  int id = carg->id;

  if (c == NULL) {
    printf("Thread ID: %d received NULL counter\n",id);
    return NULL;
  }

  for (unsigned long i = 0; i < 1000000; i++) {
    increment(c);
  }
  pthread_exit(NULL);  // this is supposed to be more clearer
  //return NULL;
}

int main() {
  counter_t *c = init_counter();
  if (c == NULL) {
    printf("Unable to initialize counter lock\n");
    goto err_cleanup;
  }

  pthread_t threads[NUMCPU];
  arg_t args[NUMCPU];

  struct timeval start, end;

  gettimeofday(&start,NULL);

  for (int i = 0; i< NUMCPU; i++) {
      args[i].id=i;
      args[i].c = c;

    int error=pthread_create(&threads[i],NULL,test_threads,&args[i]); 
    if (error != 0)  {
      printf("unable to create thread 1 \n");
      goto err_cleanup;
    };

  }

  for (int i = 0; i < NUMCPU; i++) {
    pthread_join(threads[i],NULL);
  }
  gettimeofday(&end,NULL);

  long seconds = end.tv_sec - start.tv_sec;
  long microseconds = end.tv_usec - start.tv_usec;
  double elapsed = seconds + (microseconds * 1e-6);  // converting microseconds to seconds

  printf("All threads completed their journey\n");
  printf("Time measuired: %.6f seconds %ld. \n",elapsed,microseconds);
  printf("Final counter value is %d\n",get(c));
  destroy(c);
  exit(EXIT_SUCCESS);

err_cleanup:
  destroy(c);
  exit(EXIT_FAILURE);
}

/*
 *pthread_t *threads[NUMCPU];

is because the first one allocates actual pthread_t objects, while the second allocates an array of pointers, not the objects themselves.
 * */
