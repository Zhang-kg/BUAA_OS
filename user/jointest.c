#include "lib.h"

#define NTHREADS 5

sem_t mutex;
int  counter = 0;

void *thread_function(void *args)
{
   writef("Thread number %08x\n", syscall_get_threadid());
   sem_wait( &mutex );
   counter++;
   sem_post( &mutex );
}

void umain() {
	pthread_t thread_id[NTHREADS];
	int i, j;

	sem_init(&mutex, 0, 1);

	for(i=0; i < NTHREADS; i++) {
		writef("i = %d\n", i);
		pthread_create( &thread_id[i], NULL, thread_function, NULL );
	}
	for (i = 0; i < NTHREADS; i++) writef("thread %d = %b, ", i, thread_id[i]);
	writef("Final counter value = %d\n", counter);
	for(j=0; j < NTHREADS; j++) {
		pthread_join( thread_id[j], NULL); 
	}
  
   /* Now that all threads are complete I can print the final result.     */
   /* Without the join I could be printing a value before all the threads */
   /* have been completed.                                                */

	writef("Final counter value: %d\n", counter);
}

