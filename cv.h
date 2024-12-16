/*Kira Cibak
  U06950566 (netID: kcibak)
  This program utilizes threading with condition variables. The program creates a circular shared buffer for character synchronization between producer and consumer threads. The main synchronization mechanisms prevent threads from accessing the shared buffer simultaneously and ensure smooth character transfer between producer and consumer.
*/

#ifndef CV_H
#define CV_H
#include <pthread.h>
#define BUFFER_SIZE 15 // Buffer size is 15 characters max
#define MAX_INPUT_LENGTH 50 // Maximum length of input string is 50 characters

typedef struct { // Shared buffer custom structure
    char buffer[BUFFER_SIZE];
    int count; //count of buffer
    int in; //point in
    int out; //point out
    pthread_mutex_t lock; //lock
    pthread_cond_t not_full; //condition variable
    pthread_cond_t not_empty; //condition variable
    int done_producing; //flag to indicate producer is completed
} shared_buffer_t;

// Function prototypes
void* producer_thread(void* thread_func);
void* consumer_thread(void* thread_func);
void init_buffer(shared_buffer_t* buff);
void cleanup_buffer(shared_buffer_t* buff);
void read_line();
void producer_critical_section(char ch);
void consumer_critical_section();

#endif