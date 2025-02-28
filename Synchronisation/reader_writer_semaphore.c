#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

int n; // Number of readers and writers
sem_t wl, rl; // Semaphores for writer lock and reader lock
int reading; // Counter for the number of active readers

// Initialize semaphores and variables
void init()
{
	reading = 0;
	sem_init(&wl, 0, 1); // Writer lock, allows one writer at a time
	sem_init(&rl, 0, n); // Reader lock, allows multiple readers
}

// Reader function
void *reader(void *ptr)
{
	int i = (int)ptr;
	
	sem_wait(&rl); // Wait to access reader lock
	reading++;
	if (reading == 1) sem_wait(&wl); // First reader locks the writer
	sem_post(&rl); // Release reader lock
	
	printf("%d is reading\n", i + 1);
	sleep(2);
	printf("%d has finished reading.\n", i + 1);
	
	sem_wait(&rl);
	reading--;
	if (reading == 0) sem_post(&wl); // Last reader unlocks the writer
	sem_post(&rl);
}

// Writer function
void *writer(void *ptr)
{
	int i = (int)ptr;
	sem_wait(&wl); // Only one writer can access at a time
	
	printf("%d is writing\n", i + 1);
	sleep(2);
	printf("%d has finished writing\n", i + 1);
	
	sem_post(&wl); // Release writer lock
}

int main()
{
	n = 5; // Number of readers and writers
	init();
	pthread_t readers[n], writers[n];

	int i;
	// Create reader threads
	for (i = 0; i < n; i++) pthread_create(&readers[i], NULL, reader, (void*)(i));
	// Create writer threads
	for (i = 0; i < n; i++) pthread_create(&writers[i], NULL, writer, (void*)(i));

	// Join reader threads
	for (i = 0; i < n; i++) pthread_join(readers[i], NULL);
	// Join writer threads
	for (i = 0; i < n; i++) pthread_join(writers[i], NULL);

	// Destroy semaphores
	sem_destroy(&rl);
	sem_destroy(&wl);
}
