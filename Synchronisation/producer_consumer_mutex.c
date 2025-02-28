#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<pthread.h>
#include<unistd.h>

// Mutex and condition variables for synchronization
pthread_mutex_t mutex;
pthread_cond_t full, empty;
int *buff;
int f, r, nb, maxit, pc, cc;

// Producer function
void *prod(void *ptr)
{
	int item = 1;
	while (pc < maxit)
	{
		pthread_mutex_lock(&mutex);
		while ((r + 1) % nb == f) pthread_cond_wait(&empty, &mutex); // Wait if buffer is full
		buff[r] = item;
		printf("Item produced : %d\n", item);
		r = (r + 1) % nb;
		pc++;
		item++;
		pthread_cond_signal(&full); // Signal consumer
		pthread_mutex_unlock(&mutex);
	}
}

// Consumer function
void *cons(void *ptr)
{
	while (cc < maxit)
	{
		pthread_mutex_lock(&mutex);
		while (r == f) pthread_cond_wait(&full, &mutex); // Wait if buffer is empty
		printf("Item consumed : %d\n", buff[f]);
		f = (f + 1) % nb;
		cc++;
		pthread_cond_signal(&empty); // Signal producer
		pthread_mutex_unlock(&mutex);
	}
}

// Initialization function
void init()
{
	f = 1;
	r = 1;
	nb = 5; // Buffer size
	maxit = 100; // Maximum items to produce/consume
	pc = 0;
	cc = 0;
	buff = (int*)malloc(sizeof(int) * nb);
	pthread_cond_init(&full, NULL);
	pthread_cond_init(&empty, NULL);
	pthread_mutex_init(&mutex, NULL);
}

// Cleanup function
void ender()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&full);
	pthread_cond_destroy(&empty);
}

int main()
{
	init();
	pthread_t t1, t2;
	pthread_create(&t1, NULL, prod, (void*)0); // Create producer thread
	pthread_create(&t2, NULL, cons, (void*)1); // Create consumer thread
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	ender(); // Cleanup resources
}