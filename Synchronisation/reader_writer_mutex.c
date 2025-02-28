#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<pthread.h>
#include<unistd.h>

pthread_mutex_t rl, wl; // Mutex locks for read and write access
int reading; // Counter to track the number of readers

// Initialize mutex locks and the reader counter
void init()
{
    pthread_mutex_init(&rl, NULL);
    pthread_mutex_init(&wl, NULL);
    reading = 0;
}

// Reader function
void *reader(void *ptr)
{
    int i = (int)ptr;
    
    // Lock the reader mutex to update the reading count
    pthread_mutex_lock(&rl);
    reading++;
    if (reading == 1) // If first reader, lock the writer mutex
        pthread_mutex_lock(&wl);
    pthread_mutex_unlock(&rl);
    
    printf("%d is reading\n", i + 1);
    sleep(5); // Simulating reading process
    printf("%d has finished reading\n", i + 1);
    
    // Lock the reader mutex to update the reading count
    pthread_mutex_lock(&rl);
    reading--;
    if (reading == 0) // If last reader, unlock the writer mutex
        pthread_mutex_unlock(&wl);
    pthread_mutex_unlock(&rl);
}

// Writer function
void *writer(void *ptr)
{
    int i = (int)ptr;
    pthread_mutex_lock(&wl); // Lock writer mutex to ensure exclusive access
    
    printf("%d is writing\n", i + 1);
    sleep(5); // Simulating writing process
    printf("%d has finished writing\n", i + 1);
    
    pthread_mutex_unlock(&wl); // Unlock writer mutex after writing
}

int main()
{
    init(); // Initialize mutex locks and variables
    int n = 5;
    pthread_t readers[n], writers[n];
    
    // Creating reader threads
    for (int i = 0; i < n; i++)
        pthread_create(&readers[i], NULL, reader, (void *)(i));
    
    // Creating writer threads
    for (int i = 0; i < n; i++)
        pthread_create(&writers[i], NULL, writer, (void *)(i));
    
    // Joining reader threads
    for (int i = 0; i < n; i++)
        pthread_join(readers[i], NULL);
    
    // Joining writer threads
    for (int i = 0; i < n; i++)
        pthread_join(writers[i], NULL);
    
    // Destroying mutex locks
    pthread_mutex_destroy(&rl);
    pthread_mutex_destroy(&wl);
}