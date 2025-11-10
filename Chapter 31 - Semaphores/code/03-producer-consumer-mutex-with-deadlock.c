#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 1
#define loop 100

int buffer[MAX];  // holds the producer produced data lol
int fill = 0;
int use = 0;

pthread_mutex_t lock;

void put(int value) {
  // no mutex safety here, if 2 producers here, and MAX is greater than 2, 2 producer can fill the counter at the same time creating race condition
  buffer[fill] = value;
  fill = (fill + 1) % MAX;
}

int get() {
  int tmp = buffer[use];
  use = (use + 1) % MAX;
  return tmp;
}

sem_t empty;  // semaphore for producer to listen to so that it know when the queue is empty
sem_t full;   // sempahore for consumer to wait on

void *producer(void *arg) {
  (void)arg;  // telling the compiler I am not using it
  for (int i = 0; i < loop; i++) {
    pthread_mutex_lock(&lock);  // if the child runs first, then the lock will be held by it, but child will  never give it back since child is waiting for parent to post some value in full 
    sem_wait(&empty);  // consumer will send signal here that it took out the value, and consumer will also increase the value
    put(i);
    sem_post(&full); // tell the child that the semaphroe is full
    pthread_mutex_unlock(&lock);
  }
  printf("producer(): finished\n");
  return NULL;
}

void *consumer(void *arg) {
  (void)arg;  // telling the compiler I am not using it
  int tmp = 0;
  for (int i = 0; i< loop; i++) {
    pthread_mutex_lock(&lock);
    sem_wait(&full);  // wait till producer puts some value here
    tmp = get();
    sem_post(&empty); // incrase the semaphore for empty, signalling that its ok for producer to put value 
    pthread_mutex_unlock(&lock);
    printf("%d\n",tmp);
  }
  printf("consumer(): finished\n");
  return NULL;
}


int main() {

  if (pthread_mutex_init(&lock,NULL) != 0) {
    printf("main(): unable to initalize lock\n");
    exit(-1);
  }
  pthread_t p1;
  pthread_t p2;

  sem_init(&empty,0,MAX);  // set empty to 1, so that producer can first send value
  sem_init(&full,0,0); // set full to 0, consumer call to sem_wait will decrease this and wait on it, 
  // this will be increase by the producer
  pthread_create(&p1,NULL,producer,NULL);
  pthread_create(&p2,NULL,consumer,NULL);

  pthread_join(p1,NULL);
  pthread_join(p2,NULL);

  printf("main(): ends\n");
  
}
