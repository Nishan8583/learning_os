#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>

// this is our semaphore
sem_t s;

void *child() {
  printf("Child\n");
  sem_post(&s);  // sem_post increases the value
  return NULL;
}

int main() {
  sem_init(&s,0,0); // Lets set it to 0, we have no resource to share
  printf("Parent: begin\n");
  pthread_t c;
  pthread_create(&c,NULL,child,NULL);
  sem_wait(&s);  // sem_wait will decrease the value of semaphore, and if it is less than 0, it will wait
  // After child increase the semaphore and wake the parent;
  printf("Parent: end\n");
}
