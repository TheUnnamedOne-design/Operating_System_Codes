#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

#define PROCESS_COUNT 5 
#define TIME_QUANTUM_QUEUE1 3
#define TIME_QUANTUM_QUEUE2 4

/**
 * Queue 1 - Round Robin (Time Quantum = 3)
 * Queue 2 - Round Robin (Time Quantum = 4)
 * Queue 3 - Shortest Remaining Time First (SRTF)
 */

typedef struct ProcessData {
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT], priority[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];
} ProcessData;

typedef struct Queue {
    int processIndices[PROCESS_COUNT * 10];
    int front, rear;
} Queue;

// Function to add a process to the queue
void enqueue(Queue *queue, int processIndex) {
    if (queue->front == -1) queue->front++;
    queue->rear++;
    queue->processIndices[queue->rear] = processIndex;
}

// Function to remove a process from the queue
void dequeue(Queue *queue) {
    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front++;
    }
}

// Function to check if a queue is empty
int isQueueEmpty(Queue *queue) {
    return queue->front == -1;
}

// Function to check if all processes are completed
int areAllProcessesCompleted(int completionFlags[]) {
    for (int i = 0; i < PROCESS_COUNT; i++) {
        if (completionFlags[i] == 0) return 0;
    }
    return 1;
}

void *multiLevelQueueScheduling(void *ptr) {
    ProcessData *data = (ProcessData *)ptr;
    Queue *queue1 = (Queue*)malloc(sizeof(Queue));
    queue1->front = -1;
    queue1->rear = -1;
    Queue *queue2 = (Queue*)malloc(sizeof(Queue));
    queue2->front = -1;
    queue2->rear = -1;
    Queue *queue3 = (Queue*)malloc(sizeof(Queue));
    queue3->front = -1;
    queue3->rear = -1;

    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT], originalBurstTime[PROCESS_COUNT];
    int completionTime[PROCESS_COUNT], turnaroundTime[PROCESS_COUNT], waitingTime[PROCESS_COUNT], responseTime[PROCESS_COUNT];
    int priority[PROCESS_COUNT], completionFlags[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];

    // Initialize process details
    for (int i = 0; i < PROCESS_COUNT; i++) {
        arrivalTime[i] = data->arrivalTime[i];
        burstTime[i] = data->burstTime[i];
		priority[i]=data->priority[i];
        originalBurstTime[i] = burstTime[i];
        completionTime[i] = 0;
        turnaroundTime[i] = 0;
        waitingTime[i] = 0;
        responseTime[i] = -1;
        completionFlags[i] = 0;
        strcpy(processID[i], data->processID[i]);
    }

    int currentTime = 0;
    int previousProcess = -1;
    int selectedProcess = -1;
    int queueSelection = -1;
    int waitingProcess1 = -1;
    int waitingProcess2 = -1;
    int waitingProcess3 = -1;

    // Process scheduling loop
    while (!areAllProcessesCompleted(completionFlags)) {
        previousProcess = selectedProcess;
        selectedProcess = -1;

        // Check if new processes have arrived and enqueue them
        for (int i = 0; i < PROCESS_COUNT; i++) {
            if (arrivalTime[i] <= currentTime && completionFlags[i] == 0 && responseTime[i] == -1) {
                responseTime[i] = currentTime;
                switch (priority[i]) {
                    case 1:
                        enqueue(queue1, i);
                        break;
                    case 2:
                        enqueue(queue2, i);
                        break;
                    case 3:
                        enqueue(queue3, i);
                        break;
                    default:
                        printf("Invalid queue priority data\n");
                        exit(0);
                }
            }
        }

        if (waitingProcess1 != -1) enqueue(queue1, waitingProcess1);
        if (waitingProcess2 != -1) enqueue(queue2, waitingProcess2);
        if (waitingProcess3 != -1) enqueue(queue3, waitingProcess3);

        // Determine which queue to process next
        if (!isQueueEmpty(queue1)) {
            queueSelection = 1;
        } else if (!isQueueEmpty(queue2)) {
            queueSelection = 2;
        } else if (!isQueueEmpty(queue3)) {
            queueSelection = 3;
        } else {
            queueSelection = -1;
        }

		if(previousProcess!=-1&&queueSelection==-1) printf("idle\t");
		if(previousProcess==-1&&queueSelection!=-1) printf("|%d|\t",currentTime);

        if (queueSelection == -1) {
            currentTime++;
            continue;
        }

        waitingProcess1 = -1;
        waitingProcess2 = -1;
        waitingProcess3 = -1;

        switch (queueSelection) {
            case 1: // Round Robin Queue 1
			{

                selectedProcess = queue1->processIndices[queue1->front];

                if(responseTime[selectedProcess]==-1) responseTime[selectedProcess]=currentTime;

                if (burstTime[selectedProcess] <= TIME_QUANTUM_QUEUE1) {
                    completionFlags[selectedProcess] = 1;
                    currentTime += burstTime[selectedProcess];
                    burstTime[selectedProcess] = 0;
                    completionTime[selectedProcess] = currentTime;
					printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                } else {
                    waitingProcess1 = selectedProcess;
                    currentTime += TIME_QUANTUM_QUEUE1;
                    burstTime[selectedProcess] -= TIME_QUANTUM_QUEUE1;
					printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                }
                dequeue(queue1);
                break;
			}
            case 2: // Round Robin Queue 2
			{

                selectedProcess = queue2->processIndices[queue2->front];
                if(responseTime[selectedProcess]==-1) responseTime[selectedProcess]=currentTime;
                if (burstTime[selectedProcess] <= TIME_QUANTUM_QUEUE2) {
                    completionFlags[selectedProcess] = 1;
                    currentTime += burstTime[selectedProcess];
                    burstTime[selectedProcess] = 0;
                    completionTime[selectedProcess] = currentTime;
					printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                } else {
                    waitingProcess2 = selectedProcess;
                    currentTime += TIME_QUANTUM_QUEUE2;
                    burstTime[selectedProcess] -= TIME_QUANTUM_QUEUE2;
					printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                }
                dequeue(queue2);
                break;
			}
            case 3: // Shortest Remaining Time First (SRTF)
			{

                int minBurst = INT_MAX;
                for (int i = 0; i <= queue3->rear; i++) {
                    if (burstTime[queue3->processIndices[i]] < minBurst && completionFlags[queue3->processIndices[i]] == 0) {
                        minBurst = burstTime[queue3->processIndices[i]];
                        selectedProcess = queue3->processIndices[i];
                    }
                }
                if(responseTime[selectedProcess]==-1) responseTime[selectedProcess]=currentTime;
                burstTime[selectedProcess]--;
                currentTime++;
                if (burstTime[selectedProcess] == 0) {
                    completionFlags[selectedProcess] = 1;
                    completionTime[selectedProcess] = currentTime;
                    printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                    dequeue(queue3);
                }
                break;
			}
        }
    }
	// Compute turnaround time and waiting time
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        turnaroundTime[i] = completionTime[i] - arrivalTime[i];
        waitingTime[i] = turnaroundTime[i] - originalBurstTime[i];
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\n", processID[i], arrivalTime[i], originalBurstTime[i], completionTime[i], turnaroundTime[i], waitingTime[i], responseTime[i]);
    }
}

int main() {
    ProcessData *data = (ProcessData *)malloc(sizeof(ProcessData));
    for (int i = 0; i < PROCESS_COUNT; i++) {
        printf("Enter Process ID, Arrival Time, Burst Time, Priority: ");
        scanf("%s%d%d%d", data->processID[i], &data->arrivalTime[i], &data->burstTime[i], &data->priority[i]);
    }
    pthread_t schedulerThread;
    pthread_create(&schedulerThread, NULL, multiLevelQueueScheduling, (void *)data);
    pthread_join(schedulerThread, NULL);
    free(data);
    return 0;
}