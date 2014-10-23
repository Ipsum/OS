
/********************************************************************
 * Operating Systems
 * Program 2
 *
 * By: David Tyler
 * 
 * Multithreading and Synchronization
 *
 * I implemented a circular buffer with 1 producer and N consumers
 * *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 4
#define MAX_POOL 200

//Circular buffer
typedef struct {
    int items[BUFFER_SIZE];
    int head;
    int cnt;
} queue;

//global variable, all threads can acess
queue item_queue;
typedef int buffer_item;
pthread_t thread_pool[MAX_POOL];

//Function Prototypes
void *Producer(void *arg);
void *Consumer(void *arg);

//Semaphores
sem_t bin_sem;
sem_t q_sem;
sem_t sch_sem;

//Mutexs
pthread_mutex_t mutx;

char thread1[]="PRODUCER";

int main(int argc,char **argv) {
    pthread_t t1;
    char comment[10];
    void *thread_result;
    int state1,threads;
    char *dst;
    
    //Read in number of threads as argument
    if(argc != 2)
    {
        printf("Usage: lab2 [num threads]\n");
        exit(1);
    }
    threads = atoi(argv[1]);
    if(threads>MAX_POOL)
    {
        printf("Number of threads must be %d or less!\n",MAX_POOL);
        exit(1);
    }

    //Init mutex and semaphores
    state1 = pthread_mutex_init(&mutx, NULL);
    state1 |= sem_init(&bin_sem, 0 ,0);
    state1 |= sem_init(&sch_sem,0,0);
    state1 |= sem_init(&q_sem,0,0);
    if(state1!=0)
        puts("Error mutex & semaphore initialization!!!");
    
    // Create Producer thread
    pthread_create(&t1, NULL, Producer, &thread1);
    // Create Consumer threads
    for(state1=0;state1<threads;state1++)
    {
        snprintf(comment, sizeof(comment), "%d", state1);
        dst = malloc(10);
        memcpy(dst,comment,sizeof comment); //Creates comment
        pthread_create(&thread_pool[state1], NULL, Consumer, dst);
    }
    // Waiting thread to terminate
    for(state1=0;state1<threads;state1++)
    {
        pthread_join(thread_pool[state1], &thread_result);
    }
    
    // destroy semaphores
    sem_destroy(&bin_sem);
    sem_destroy(&q_sem);
    sem_destroy(&sch_sem);
    // destroy mutex
    pthread_mutex_destroy(&mutx);
    printf("All threads have eaten!\n");
    //Producer is supposed to produce forever so this program will never end!
    pthread_join(t1, &thread_result);
    return 0;
}

/********************************************************************
 * void *Producer(void *arg)
 *
 * The producer thread. Runs forever trying to push numbers
 * into the queue.
 * *****************************************************************/
void *Producer(void *arg)
{
    int i,r;
#ifdef _DEBUG
    printf("Creating Thread: %s\n", (char*)arg);
#endif    
    for(;;)
    {
        r = rand()%10000; //Generate a number between 0 and 10,000
        i = Queue_insert(r);
        if(i==-1)
            sleep(1);
        else
#ifndef _DEBUG
            printf("%s: INSERT %d into BUFFER\n", (char*)arg,r);
#else
            printf("%s: INSERT %d into BUFFER %d\n", (char*)arg,r,i);
#endif
    }
}

/********************************************************************
 * void *Consumer(void *arg)
 *
 * The consumer thread. Pops numbers off the queue, prints them,
 * and exits. Synchronized against the other consumers.
 * *****************************************************************/

void *Consumer(void *arg)
{
    int i;
#ifdef _DEBUG
    printf("Creating Thread: %s\n", (char*)arg);
#endif
    pthread_mutex_lock(&mutx);
    sleep(1);
    i = Queue_pop();
    printf("THREAD %s: REMOVE %d from BUFFER\n", (char*)arg, i);
    pthread_mutex_unlock(&mutx);
    return 0;
}

/********************************************************************
 * int Queue_insert(x)
 *
 * Inserts item onto end of circular queue. Returns queue length.
 * 
 * Stops inserting when queue is full and instead returns -1.
 * *****************************************************************/
int Queue_insert(int x)
{
        int size;
        sem_getvalue(&q_sem,&size);
#ifdef _DEBUG
        printf("queue size: %d\n",size);
#endif
        if(size >= BUFFER_SIZE-1)
            return -1;
        if(item_queue.cnt==BUFFER_SIZE)
            item_queue.cnt=0; //Circular queue
        
        item_queue.items[item_queue.cnt++] = x;
        sem_post(&q_sem);
        
        return item_queue.cnt-1;
}

/********************************************************************
 * int Queue_pop()
 *
 * Removes item from beginning of circular queue and returns it
 * 
 * If queue is empty, it waits on the queue semaphore, q_sem
 * *****************************************************************/
int Queue_pop()
{
        int rtrn;
        sem_wait(&q_sem);
        if(item_queue.head==BUFFER_SIZE)
            item_queue.head=0;
        
        rtrn = item_queue.items[item_queue.head];
        if(rtrn==-1) //Queue empty!
            return -1;

        item_queue.items[item_queue.head++] = -1;
        return rtrn;
}

