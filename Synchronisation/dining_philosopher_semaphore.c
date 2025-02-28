#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

sem_t *plock, mutex; // Semaphores for philosophers and mutex for critical section
int eating, hungry, thinking;
int *states;
int n;

// Get index of left neighbor
int left(int i)
{
    return (i + n - 1) % n;
}

// Get index of right neighbor
int right(int i)
{
    return (i + 1) % n;
}

// Initialize semaphores and states
void init()
{
    n = 5;
    states = (int*)malloc(sizeof(int) * n);
    plock = (sem_t*)malloc(sizeof(sem_t) * n);
    sem_init(&mutex, 0, 1);
    
    for (int i = 0; i < n; i++)
    {
        sem_init(&plock[i], 0, 0);
    }
    
    eating = 0;
    hungry = 1;
    thinking = 2;
}

// Check if philosopher can eat
void test(int i)
{
    if (states[i] == hungry && states[left(i)] != eating && states[right(i)] != eating)
    {
        states[i] = eating;
        printf("Philosopher %d is picking up forks %d and %d\n", i, i, right(i));
        printf("Philosopher %d is eating\n", i);
        sleep(1);
        sem_post(&plock[i]); // Allow philosopher to eat
    }
}

// Destroy semaphores
void dest()
{
    sem_destroy(&mutex);
    for (int i = 0; i < n; i++)
    {
        sem_destroy(&plock[i]);
    }
}

// Philosopher puts down forks after eating
void put_fork(int i)
{
    sem_wait(&mutex);
    states[i] = thinking;
    printf("Philosopher %d is putting down forks %d and %d\n", i, i, right(i));
    printf("Philosopher %d is thinking\n", i);
    test(left(i));
    test(right(i));
    sem_post(&mutex);
}

// Philosopher tries to pick up forks
void take_fork(int i)
{
    sem_wait(&mutex);
    states[i] = hungry;
    printf("%d is hungry\n", i);
    test(i);
    sem_post(&mutex);
    sem_wait(&plock[i]); // Wait if forks are not available
}

// Philosopher thread function
void *phil(void *ptr)
{
    int i = (int)ptr;
    while (1)
    {
        take_fork(i);
        sleep(1);
        put_fork(i);
    }
}

int main()
{
    init();
    pthread_t p[n];
    
    // Create philosopher threads
    for (int i = 0; i < n; i++) 
    {
        pthread_create(&p[i], NULL, phil, (void*)i);
    }
    
    // Wait for philosopher threads to finish
    for (int i = 0; i < n; i++) 
    {
        pthread_join(p[i], NULL);
    }
    
    dest(); // Clean up resources
}