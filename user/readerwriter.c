#include "lib.h"
#define READER_NUM 5
#define WRITER_NUM 2

sem_t fmutex, rdcntmutex, wtcntmutex, queue;
int readcount = 0, writecount = 0;

void * reader(void * args) {
    int threadid = syscall_get_threadid();
    int i = 0;
    for (i = 0; i < 20; i++) {
        sem_wait(&queue);
        sem_wait(&rdcntmutex);
        if (readcount == 0) {
            sem_wait(&fmutex);
        }
        readcount++;
        sem_post(&rdcntmutex);
        sem_post(&queue);
        int sleep;
        writef("%b: do some reading\n", threadid);
        for (sleep = 0; sleep < 50; sleep ++) {
            writef("s");  
        }
        writef("\n");
        sem_wait(&rdcntmutex);
        readcount--;
        if (readcount == 0) {
            sem_post(&fmutex);
        }
        sem_post(&rdcntmutex);
    }
}

void * writer(void * args) {
    int threadid = syscall_get_threadid();
    int i = 0;
    for (i = 0; i < 5; i++) {
        sem_wait(&wtcntmutex);
        if (writecount == 0) {
            sem_wait(&queue);
        }
        writecount++;
        sem_post(&wtcntmutex);
        sem_wait(&fmutex);
        int sleep;
        writef("writer %b: do some writing\n", threadid);
        for (sleep = 0; sleep < 100; sleep++) {
            writef("w");
        }
        writef("\n");
        sem_post(&fmutex);
        sem_wait(&wtcntmutex);
        writecount--;
        if (writecount == 0) {
            sem_post(&queue);
        }
        sem_post(&wtcntmutex);
    }
}

void umain() {
    u_int args[10];
    args[0] = syscall_get_threadid();
    pthread_t readers[READER_NUM];
    pthread_t writers[WRITER_NUM];

    sem_init(&fmutex, 0, 1);
    args[1] = &fmutex;
    sem_init(&rdcntmutex, 0, 1);
    args[2] = &rdcntmutex;
    sem_init(&wtcntmutex, 0, 1);
    args[3] = &wtcntmutex;
    sem_init(&queue, 0, 1);
    args[4] = &queue;

    int i;
    for (i = 0; i < READER_NUM; i++) {
        pthread_create(&readers[i], NULL, reader, (void*)args);
    }
    for (i = 0; i < WRITER_NUM; i++) {
        pthread_create(&writers[i], NULL, writer, (void *)args);
    }
    pthread_exit(NULL);
}
