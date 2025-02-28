#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define PROCESS_COUNT 2
#define TIME_QUANTUM1 3
#define TIME_QUANTUM2 6
/**
 * RR - 3 (Queue1)
 * RR - 6 (Queue2)
 * FCFS (Queue3)
 */

// Structure to hold process details
typedef struct {
    char processID[PROCESS_COUNT][100];
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT];
} ProcessData;

// Queue structure for MLFQ
typedef struct {
    int *queue;
    int front, rear;
} Queue;

Queue *initializeQueue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->queue = (int *)malloc(sizeof(int) * PROCESS_COUNT * 100);
    q->front = -1;
    q->rear = -1;
    return q;
}

void enqueue(Queue *q, int index) {
    if (q->front == -1) q->front = 0;
    q->rear++;
    q->queue[q->rear] = index;
}

void dequeue(Queue *q) {
    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
    } else {
        q->front++;
    }
}

int isQueueEmpty(Queue *q) {
    return q->front == -1;
}

int areAllProcessesCompleted(int completionStatus[]) {
    for (int i = 0; i < PROCESS_COUNT; i++) {
        if (completionStatus[i] == 0) return 0;
    }
    return 1;
}

void *multiLevelFeedbackQueueScheduling(void *arg) {
    ProcessData *data = (ProcessData *)arg;
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT], originalBurstTime[PROCESS_COUNT];
    int completionTime[PROCESS_COUNT], turnaroundTime[PROCESS_COUNT], waitingTime[PROCESS_COUNT], responseTime[PROCESS_COUNT];
    int completionStatus[PROCESS_COUNT], hasArrived[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];
    
    Queue *queue1 = initializeQueue();
    Queue *queue2 = initializeQueue();
    Queue *queue3 = initializeQueue();
    
    for (int i = 0; i < PROCESS_COUNT; i++) {
        strcpy(processID[i], data->processID[i]);
        arrivalTime[i] = data->arrivalTime[i];
        burstTime[i] = data->burstTime[i];
        originalBurstTime[i] = burstTime[i];
        completionTime[i] = 0;
        turnaroundTime[i] = 0;
        waitingTime[i] = 0;
        responseTime[i] = -1;
        completionStatus[i] = 0;
        hasArrived[i] = 0;
    }
    
    int currentTime = 0, previousProcess = -1, selectedProcess = -1;
    int queueStatus = -1;
    int waitingInQueue1 = -1, waitingInQueue2 = -1, waitingInQueue3 = -1;
    
    while (!areAllProcessesCompleted(completionStatus)) {
        previousProcess = selectedProcess;
        selectedProcess = -1;
        
        for (int i = 0; i < PROCESS_COUNT; i++) {
            if (arrivalTime[i] <= currentTime && completionStatus[i] == 0 && hasArrived[i] == 0) {
                hasArrived[i] = 1;
                enqueue(queue1, i);
            }
        }
        
        if (waitingInQueue1 != -1) enqueue(queue1, waitingInQueue1);
        if (waitingInQueue2 != -1) enqueue(queue2, waitingInQueue2);
        if (waitingInQueue3 != -1) enqueue(queue3, waitingInQueue3);
        
        if (!isQueueEmpty(queue1)) queueStatus = 1;
        else if (!isQueueEmpty(queue2)) queueStatus = 2;
        else if (!isQueueEmpty(queue3)) queueStatus = 3;
        else queueStatus = -1;

		if(previousProcess!=-1&&queueStatus==-1) printf("idle\t");
		if(previousProcess==-1&&queueStatus!=-1) printf("|%d|\t",currentTime);
        
        if (queueStatus == -1) {
            currentTime++;
            continue;
        }
        
        waitingInQueue1 = -1;
        waitingInQueue2 = -1;
        waitingInQueue3 = -1;
        
        switch (queueStatus) {
            case 1: {
                selectedProcess = queue1->queue[queue1->front];

                if (responseTime[selectedProcess] == -1) responseTime[selectedProcess] = currentTime;
                
                if (burstTime[selectedProcess] <= TIME_QUANTUM1) {
                    currentTime += burstTime[selectedProcess];
                    burstTime[selectedProcess] = 0;
                    completionStatus[selectedProcess] = 1;
                    completionTime[selectedProcess] = currentTime;
					printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                } else {
                    currentTime += TIME_QUANTUM1;
                    burstTime[selectedProcess] -= TIME_QUANTUM1;
                    waitingInQueue2 = selectedProcess;
					printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                }
                dequeue(queue1);
                break;
            }
            case 2: {
                selectedProcess = queue2->queue[queue2->front];

                int timeSlice = 0;
                int stopProcessing = 0;
                
                while (timeSlice < TIME_QUANTUM2) {
                    for (int i = 0; i < PROCESS_COUNT; i++) {
                        if (arrivalTime[i] <= currentTime && completionStatus[i] == 0 && hasArrived[i] == 0) {
                            stopProcessing = 1;
							printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                            break;
                        }
                    }
                    if (stopProcessing) break;
                    burstTime[selectedProcess]--;
                    currentTime++;
                    timeSlice++;
                }
                if (burstTime[selectedProcess] == 0) {
					completionStatus[selectedProcess] = 1;
                    completionTime[selectedProcess] = currentTime;
					printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                } else {
					printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                    waitingInQueue3 = selectedProcess;
                }
                dequeue(queue2);
                break;
            }
            case 3: {
                selectedProcess = queue3->queue[queue3->front];
				int stopflag=0;
                while (burstTime[selectedProcess] > 0) {

					for (int i = 0; i < PROCESS_COUNT; i++) {
                        if (arrivalTime[i] <= currentTime && completionStatus[i] == 0 && hasArrived[i] == 0) {
                            stopflag = 1;
							printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                            break;
                        }
                    }
					if(stopflag) break;
                    burstTime[selectedProcess]--;
                    currentTime++;
				
                }
				if(stopflag) break;
                completionStatus[selectedProcess] = 1;
                completionTime[selectedProcess] = currentTime;
				printf("%s\t|%d|\t",processID[selectedProcess],currentTime);
                dequeue(queue3);
                break;
            }
        }
    }
    
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        turnaroundTime[i] = completionTime[i] - arrivalTime[i];
        waitingTime[i] = turnaroundTime[i] - originalBurstTime[i];
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\n", processID[i], arrivalTime[i], originalBurstTime[i], completionTime[i], turnaroundTime[i], waitingTime[i], responseTime[i]);
    }
    return NULL;
}

int main() {
    ProcessData data;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        printf("Enter Process ID, Arrival Time, Burst Time: ");
        scanf("%s %d %d", data.processID[i], &data.arrivalTime[i], &data.burstTime[i]);
    }
    pthread_t schedulerThread;
    pthread_create(&schedulerThread, NULL, multiLevelFeedbackQueueScheduling, (void *)&data);
    pthread_join(schedulerThread, NULL);
    return 0;
}
