
/********************************************************************
 * Operating Systems
 * Program 3
 *
 * By: David Tyler
 * 
 * Memory management in a multithreaded environment
 *
 * I basically keep track of the allocated memory in m_map[][]
 * *****************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define MAX_SIZE 4096 //Amount of RAM used for this simulation
#define MAX_BLOCK 1024 //Max amount allowed to be requested by thread
#define MAX_POOL 200 //max number of user threads

//Array to hold the thread pointers
pthread_t thread_pool[MAX_POOL];

//Array to keep track of allocated memory
int m_map[MAX_POOL][3] = {}; //pointer,size,thread

//Function Prototypes
int memory_malloc(int size, int thread_id);
int memory_free(int thread_id);
void *User(void *arg);
void display_mem(void);

//pointer to start of malloc'ed space
char *head_p;

//Mutexes
pthread_mutex_t mutx;

int main(int argc, char **argv) {
    int threads;
    int i;

    pthread_mutex_init(&mutx,NULL);

    //Malloc MAX_SIZE bytes and treat it as a character array from now on (byte array)
    head_p = (char *)malloc(MAX_SIZE);
    
    if(argc != 2)
    {
        printf("Usage: lab3 [num threads]\n");
        exit(1);
    }
    threads = atoi(argv[1]);
    if(threads>MAX_POOL)
    {
        printf("Number of threads must be %d or less!\n",MAX_POOL);
        exit(1);
    }

    for(i=0;i<threads;i++)
    {
        pthread_create(&thread_pool[i],NULL,User,(int *)i); 
    }
    //Wait for all threads to exit
    for(i=0;i<threads;i++)
    {
        pthread_join(thread_pool[i], NULL);
    }

    return 0;
}

void *User(void *arg) {
    int size,delay;
    int location;
    //build constant delay time
    delay = (arc4random() % 9)+1; //Sleep between 1 and 10 sec
    printf("Creating thread: %d\n", arg);
    size = (arc4random() % MAX_BLOCK); //decide how much memory to request
    for(;;)
    {
        pthread_mutex_lock(&mutx);
        printf("I am thread %d and I am waking up\n",arg);
        if(location) {
            memory_free((int)arg);
        }
        size = (arc4random() % MAX_BLOCK); //decide how much memory to request
        printf("Size requested: %d\n",size);
        location = memory_malloc(size,(int)arg);
        printf("Location: %d\n",location);
        printf("I am thread %d and I am going to sleep\n",arg);
        pthread_mutex_unlock(&mutx);
        sleep(delay);
    }
}

//Iterate through m_map and look for first open element. Since the array is always compacted, 
//this will also always be the last. Take the sum of the sizes of the previous elements and use that
//to compute the pointer in the malloc'ed memory
int memory_malloc(int size, int thread_id)
{
    int mem_inuse = 0;
    int i;
    printf("====BEGIN MALLOC====\n");
    for(i=0;i<MAX_POOL;i++)
    {
        if(!m_map[i][0])
        {
            printf("malloc size %d\n",size);
            if(mem_inuse+size>MAX_SIZE)
            {
                //Memory is full, take memory from the current last element in the array
                printf("!!===> Memory full! size= %d <===!!\n Seizing memory from thread %d\n",mem_inuse,m_map[i-1][2]);
                memory_free(m_map[i-1][2]);
                return memory_malloc(size,thread_id); //Tail call recursion
            }
            //We found the last element of the array, insert our new memory pointer,size, and thread id here
            m_map[i][0] = (int)&head_p[mem_inuse-1]; //compute mem pointer by indexing on total memory already in use 
                                                     //into malloc space as char pointer
            m_map[i][1] = size;
            m_map[i][2] = thread_id;
            break;
        }
        mem_inuse += m_map[i][1]; //increment total memory used
    }
    printf("====END MALLOC====\n");
    display_mem(); //print out memory usage
    return m_map[i][0];
}

//Iterate though m_map and find the thread_id. Assign the next element in m_map to the place that was occupied by thread_id.
//Iterate though the remainder of the array moving the next element to the current element and recalulating the memory 
//pointers in order to compact the memory
int memory_free(int thread_id)
{
    int i;
    int size=0;

    printf("****BEGIN FREE****\n");
    printf("Freeing thread %d's memory\n",thread_id);
    //Find thread_id
    for(i=0;i<MAX_POOL;i++)
    {
        if(m_map[i][2]==thread_id)
            break;
        size+=(int)m_map[i][1];
    }
    //Shift elements to the left in the array after removing thread_id's element
    for(;i<MAX_POOL;i++)
    {
        if(!m_map[i+1][0])
        {
            m_map[i][0] = 0;
            m_map[i][1] = 0;
            break;
        }
        m_map[i][0]=(int)&head_p[size-1]; //recompute pointer to memory for each element after removed one
        m_map[i][1]=m_map[i+1][1];
        m_map[i][2]=m_map[i+1][2];
        size+=m_map[i][1];
    }
    printf("****END FREE****\n");
    display_mem();
    return 0;
}
void display_mem(void)
{
    int i;
    int size=0;

    printf("Current Memory: |");
    for(i=0;i<MAX_POOL;i++)
    {
        if(m_map[i][0]==0)
        {
            printf("\nTOTAL SIZE: %d\n",size);
            return;
        }
        printf("Thread %d @ %d (%d bytes) |",m_map[i][2],m_map[i][0],m_map[i][1]);
        size+=m_map[i][1];
    }
}
