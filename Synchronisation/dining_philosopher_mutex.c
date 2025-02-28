#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

int eating, thinking, hungry; // Constants to represent philosopher states
int *states; // Array to track the state of each philosopher
int n; // Number of philosophers
pthread_mutex_t mutex; // Mutex for critical section
pthread_mutex_t *plock; // Array of mutex locks for philosophers

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

// Initialize variables and mutex locks
void init()
{
    eating = 0;
    thinking = 2;
    hungry = 1;
    n = 5;
    plock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * n);
    pthread_mutex_init(&mutex, NULL);
    states = (int*)malloc(sizeof(int) * n);
    
    for (int i = 0; i < n; i++)
    {
        states[i] = 0;
        pthread_mutex_init(&plock[i], NULL);
        pthread_mutex_lock(&plock[i]); // Lock individual philosopher mutexes initially
    }
}

// Destroy mutex locks and free allocated memory
void dest()
{
    pthread_mutex_destroy(&mutex);
    for (int i = 0; i < n; i++)
    {
        pthread_mutex_destroy(&plock[i]);
    }
}

// Test if philosopher can eat
void test(int i)
{
    if (states[i] == hungry && states[right(i)] != eating && states[left(i)] != eating)
    {
        states[i] = eating;
        printf("Philosopher %d is picking up the forks %d and %d\n", i, i, right(i));
        printf("Philosopher %d is eating\n", i);
        sleep(1);
        pthread_mutex_unlock(&plock[i]); // Allow philosopher to proceed
    }
}

// Put down forks after eating
void put_down_fork(int i)
{
    pthread_mutex_lock(&mutex); // Enter critical section
    states[i] = thinking;
    printf("Philosopher %d is putting down forks %d and %d\n", i, i, right(i));
    printf("Philosopher %d is thinking\n", i);
    test(left(i)); // Check if left neighbor can eat
    test(right(i)); // Check if right neighbor can eat
    pthread_mutex_unlock(&mutex); // Exit critical section
}

// Attempt to pick up forks
void take_fork(int i)
{
    pthread_mutex_lock(&mutex); // Enter critical section
    states[i] = hungry;
    printf("Philosopher %d is hungry\n", i);
    test(i); // Check if the philosopher can eat
    pthread_mutex_unlock(&mutex); // Exit critical section
    pthread_mutex_lock(&plock[i]); // Wait if forks are not available
}

// Philosopher thread function
void *phil(void *ptr)
{
    int i = (int)ptr; // Get philosopher index
    while (1)
    {
        take_fork(i); // Attempt to pick up forks
        sleep(1); // Simulate eating
        put_down_fork(i); // Put down forks after eating
    }
}

int main()
{
    init(); // Initialize semaphores and variables
    
    pthread_t ph[n]; // Array of philosopher threads
    
    // Create philosopher threads
    for (int i = 0; i < n; i++) 
    {
        pthread_create(&ph[i], NULL, phil, (void*)(i));
    }
    
    // Wait for philosopher threads to finish (never happens in this case)
    for (int i = 0; i < n; i++) 
    {
        pthread_join(ph[i], NULL);
    }
    
    dest(); // Destroy mutexes and free memory
}