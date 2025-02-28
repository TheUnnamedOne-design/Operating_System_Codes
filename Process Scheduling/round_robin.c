#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<limits.h>

#define PROCESS_COUNT 5
#define TIME_QUANTUM 2

// Structure to hold process details
typedef struct ProcessData {
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];
} ProcessData;

// Queue structure for Round Robin scheduling
typedef struct Queue {
    int *queue;
    int front, rear;
} Queue;

// Initialize the queue
Queue *initializeQueue() {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    q->front = -1;
    q->rear = -1;
    q->queue = (int*)malloc(sizeof(int) * PROCESS_COUNT * PROCESS_COUNT);
    return q;
}

// Enqueue a process index into the queue
void enqueue(Queue *q, int index) {
    if (q->front == -1) q->front++;
    q->rear++;
    q->queue[q->rear] = index;
}

// Dequeue a process index from the queue
void dequeue(Queue *q) {
    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
    } else {
        q->front++;
    }
}

// Check if all processes are completed
int areAllProcessesCompleted(int completionStatus[]) {
    for (int i = 0; i < PROCESS_COUNT; i++) {
        if (completionStatus[i] == 0) return 0;
    }
    return 1;
}

// Check if the queue is empty
int isQueueEmpty(Queue *q) {
    return (q->front == -1);
}

// Round Robin scheduling function
void *roundRobinScheduler(void *ptr) {
    ProcessData *data = (ProcessData*)ptr;
    
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT], initialBurstTime[PROCESS_COUNT];
    int completionTime[PROCESS_COUNT], turnaroundTime[PROCESS_COUNT], waitingTime[PROCESS_COUNT], responseTime[PROCESS_COUNT];
    int completionStatus[PROCESS_COUNT], hasEnteredQueue[PROCESS_COUNT];
    int previousProcess = -1, currentProcess = -1, currentTime = 0, processWaiting = -1;
    char processID[PROCESS_COUNT][100];

    for (int i = 0; i < PROCESS_COUNT; i++) {
        arrivalTime[i] = data->arrivalTime[i];
        burstTime[i] = data->burstTime[i];
        initialBurstTime[i] = burstTime[i];
        completionTime[i] = 0;
        turnaroundTime[i] = 0;
        waitingTime[i] = 0;
        responseTime[i] = -1;
        completionStatus[i] = 0;
        hasEnteredQueue[i] = 0;
        strcpy(processID[i], data->processID[i]);
    }
    
    Queue *queue = initializeQueue();

    while (!areAllProcessesCompleted(completionStatus)) {
        previousProcess = currentProcess;
        currentProcess = -1;

        // Add new arriving processes to the queue
        for (int i = 0; i < PROCESS_COUNT; i++) {
            if (arrivalTime[i] <= currentTime && completionStatus[i] == 0 && hasEnteredQueue[i] == 0) {
                hasEnteredQueue[i] = 1;
                enqueue(queue, i);
            }
        }

        // Re-enqueue the previously executing process if it still has remaining burst time
        if (processWaiting != -1) enqueue(queue, processWaiting);

        if (isQueueEmpty(queue)) {
            currentProcess = -1;
        } else {
            currentProcess = queue->queue[queue->front];
        }

        if (currentProcess == -1 && previousProcess != -1) printf("idle\t");
        if (currentProcess != -1 && previousProcess == -1) printf("|%d|\t", currentTime);

        if (currentProcess == -1) {
            currentTime++;
        } else {
            processWaiting = -1;
            currentProcess = queue->queue[queue->front];

            if (responseTime[currentProcess] == -1) responseTime[currentProcess] = currentTime;

            if (burstTime[currentProcess] <= TIME_QUANTUM) {
                currentTime += burstTime[currentProcess];
                burstTime[currentProcess] = 0;
                completionTime[currentProcess] = currentTime;
                completionStatus[currentProcess] = 1;
                printf("%s\t|%d|\t", processID[currentProcess], currentTime);
            } else {
                currentTime += TIME_QUANTUM;
                burstTime[currentProcess] -= TIME_QUANTUM;
                processWaiting = currentProcess;
                printf("%s\t|%d|\t", processID[currentProcess], currentTime);
            }
            dequeue(queue);
        }
    }

    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        turnaroundTime[i] = completionTime[i] - arrivalTime[i];
        waitingTime[i] = turnaroundTime[i] - initialBurstTime[i];
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\n", processID[i], arrivalTime[i], initialBurstTime[i], completionTime[i], turnaroundTime[i], waitingTime[i], responseTime[i]);
    }
}

int main() {
    ProcessData *processData = (ProcessData*)malloc(sizeof(ProcessData));
    
    for (int i = 0; i < PROCESS_COUNT; i++) {
        printf("PID AT BT : ");
        scanf("%s%d%d", processData->processID[i], &processData->arrivalTime[i], &processData->burstTime[i]);
    }
    
    pthread_t schedulerThread;
    pthread_create(&schedulerThread, NULL, roundRobinScheduler, (void*)processData);
    pthread_join(schedulerThread, NULL);
    
    return 0;
}
