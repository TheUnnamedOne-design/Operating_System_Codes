#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

// Semaphores for synchronization
sem_t mutex, full, empty;

int f, r, nb, pc, cc, mi;
int *buff;

// Initialize shared resources and semaphores
void init()
{
	f = 0;
	r = 0;
	nb = 5; // Buffer size
	pc = 0;
	cc = 0;
	mi = 100; // Maximum number of items to produce/consume
	buff = (int*)malloc(sizeof(int) * nb);
	sem_init(&mutex, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, nb);
}

// Destroy semaphores
void dest()
{
	sem_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);
}

// Producer function
void *prod(void *ptr)
{
	int item = 0;
	while (pc < mi)
	{
		sem_wait(&empty); // Wait if buffer is full
		sem_wait(&mutex); // Lock mutex
		
		buff[r] = item;
		printf("Item produced : %d\n", item);
		r = (r + 1) % nb;
		pc++;
		item++;
		
		sem_post(&mutex); // Unlock mutex
		sem_post(&full); // Signal consumer
	}
}

// Consumer function
void *cons(void *ptr)
{
	while (cc < mi)
	{
		sem_wait(&full); // Wait if buffer is empty
		sem_wait(&mutex); // Lock mutex
		
		printf("Item consumed : %d\n", buff[f]);
		f = (f + 1) % nb;
		cc++;
		
		sem_post(&mutex); // Unlock mutex
		sem_post(&empty); // Signal producer
	}
}

int main()
{
	init();
	pthread_t t1, t2;
	pthread_create(&t1, NULL, prod, (void*)0); // Create producer thread
	pthread_create(&t2, NULL, cons, (void*)1); // Create consumer thread
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	dest(); // Cleanup resources
}