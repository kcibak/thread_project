/*Kira Cibak
  U06950566 (netID: kcibak)
  This program utilizes threading with condition variables. The program creates a circular shared buffer for character synchronization between producer and consumer threads. The main synchronization mechanisms prevent threads from accessing the shared buffer simultaneously and ensure smooth character transfer between producer and consumer.
*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <pthread.h> // Threading and synchronization functions
#include "cv.h" // Custom header file for the program

shared_buffer_t shared_buffer; // Shared buffer between producer and consumer
char input_string[MAX_INPUT_LENGTH + 1]; // Input string for producer to process

void init_buffer(shared_buffer_t* buffer) {

    pthread_mutex_init(&buffer->lock, NULL); // Initialize the mutex lock
    pthread_cond_init(&buffer->not_full, NULL); // Initialize the not_full condition variable
    pthread_cond_init(&buffer->not_empty, NULL); // Initialize the not_empty condition variable

    buffer->count = 0; // Initialize buffer count to zero
    buffer->in = 0; // Initialize the input index to zero
    buffer->out = 0; // Initialize the output index to zero
    buffer->done_producing = 0; // Reset the done producing flag
}

void cleanupNreset_buffer(shared_buffer_t* buffer) {

    pthread_mutex_destroy(&buffer->lock); // Destroy the mutex lock
    pthread_cond_destroy(&buffer->not_full); // Destroy the not_full condition variable
    pthread_cond_destroy(&buffer->not_empty); // Destroy the not_empty condition variable

    buffer->count = 0; // Reset buffer count to zero
    buffer->in = 0; // Reset the input index to zero
    buffer->out = 0; // Reset the output index to zero
    buffer->done_producing = 0; // Reset the done producing flag

    memset(input_string, 0, sizeof(input_string)); // Clear the input string
}

void producer_critical_section(char ch) {

    pthread_mutex_lock(&shared_buffer.lock); // Lock the shared buffer
    while (shared_buffer.count == BUFFER_SIZE) { // Wait until buffer is not full
        pthread_cond_wait(&shared_buffer.not_full, &shared_buffer.lock); // Wait for consumer signal
    }

    pthread_mutex_unlock(&shared_buffer.lock); // Unlock the shared buffer
    return; // Exit the function
}

void consumer_critical_section() {

    pthread_mutex_lock(&shared_buffer.lock); // Lock the shared buffer
    while (shared_buffer.count == 0 && !shared_buffer.done_producing) { // Wait until buffer is not empty or producer finishes
        pthread_cond_wait(&shared_buffer.not_empty, &shared_buffer.lock); // Wait for producer signal
    }

    if (shared_buffer.count == 0 && shared_buffer.done_producing) { // Check if buffer is empty and production is done
        pthread_mutex_unlock(&shared_buffer.lock); // Unlock the shared buffer
        return; // Exit the function
    }

    pthread_mutex_unlock(&shared_buffer.lock); // Unlock the shared buffer
    return; // Exit the function
}

void* producer_thread(void* arg) {

    shared_buffer.done_producing = 0; // Reset the production complete flag
    int i = 0; // Initialize index for input string

    while (input_string[i] != '\0') { // Loop through the input string
        producer_critical_section(input_string[i]); // Call the producer critical section
        printf("Produced: %c\n", input_string[i]); // Print the produced character

        pthread_cond_signal(&shared_buffer.not_empty); // Signal the consumer that buffer is not empty
        shared_buffer.buffer[shared_buffer.in] = input_string[i]; // Store the character in the buffer
        shared_buffer.in = (shared_buffer.in + 1) % BUFFER_SIZE; // Update the circular buffer index
        shared_buffer.count++; // Increment the buffer count
        i++; // Move to the next character in the input string
    }
    shared_buffer.done_producing = 1; // Indicate that production is complete
    printf("Producer: done\n"); // Print that the producer is done
    return NULL; // Exit the thread
}

void* consumer_thread(void* arg) {

    int consumed_count = 0; // Initialize count of consumed characters
    int total_length = strlen(input_string); // Calculate the total number of characters to consume

    while (consumed_count < total_length) { // Loop until all characters are consumed
        consumer_critical_section(); // Call the consumer critical section
        char temp = shared_buffer.buffer[shared_buffer.out]; // Retrieve the character from the buffer

        printf("Consumed: %c\n", temp); // Print the consumed character
        shared_buffer.out = (shared_buffer.out + 1) % BUFFER_SIZE; // Update the circular buffer index
        shared_buffer.count--; // Decrement the buffer count
        consumed_count++; // Increment the consumed count

        pthread_cond_signal(&shared_buffer.not_full); // Signal the producer that buffer is not full
    }
    printf("Consumer: done\n"); // Print that the consumer is done
    return NULL; // Exit the thread
}

void read_line() {

    char temp[MAX_INPUT_LENGTH + 1]; // Temporary buffer for input
    int count = 0, ch; // Initialize character count and character variable

    printf("Enter input (type 'exit' to quit): "); // Prompt user for input
    while ((ch = getchar()) != '\n') { // Loop until newline character
        if (count < MAX_INPUT_LENGTH) { // Check if within maximum length
            temp[count++] = ch; // Store character in temporary buffer
        }
    }
    temp[count] = '\0'; // Null-terminate the input string
    int len = strlen(temp); // Calculate length of the input string
    strncpy(input_string, temp, len); // Copy the input string to global variable
}

int main(void) {

    pthread_t producer, consumer; // Thread variables for producer and consumer

    while (1) { // Infinite loop
        read_line(); // Read user input
        if (strcmp(input_string, "exit") == 0) { // Check if user entered "exit"
            printf("Parent: done\n"); // Print exit message
            cleanupNreset_buffer(&shared_buffer); // Clean up and reset the buffer
            return 0; // Exit the program
        }

        printf("Input: %s\n", input_string); // Print the input string
        printf("Count: %ld characters\n", strlen(input_string)); // Print the character count

        init_buffer(&shared_buffer); // Initialize the shared buffer
        pthread_create(&producer, NULL, producer_thread, NULL); // Create producer thread
        pthread_create(&consumer, NULL, consumer_thread, NULL); // Create consumer thread
        pthread_join(consumer, NULL); // Wait for consumer thread to finish

        cleanupNreset_buffer(&shared_buffer); // Clean up and reset the buffer
    }
    return 0;
}