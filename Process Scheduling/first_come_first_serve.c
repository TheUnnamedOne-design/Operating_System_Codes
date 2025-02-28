#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<limits.h>

#define NUM_PROCESSES 4

// Structure to store process data
typedef struct ProcessData {
    int arrivalTime[NUM_PROCESSES], burstTime[NUM_PROCESSES];
    char processID[NUM_PROCESSES][100];
} ProcessData;

// Function to check if all processes are completed
int allProcessesCompleted(int completed[]) {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (completed[i] == 0) return 0;
    }
    return 1;
}

// First Come First Serve (FCFS) Scheduling Algorithm
void *FCFS(void *ptr) {
    int i, j;
    ProcessData *data = (ProcessData *)ptr;
    
    int arrival[NUM_PROCESSES], burst[NUM_PROCESSES], originalBurst[NUM_PROCESSES];
    int completionTime[NUM_PROCESSES], turnaroundTime[NUM_PROCESSES], waitingTime[NUM_PROCESSES], responseTime[NUM_PROCESSES], completed[NUM_PROCESSES];
    int prevSelected = -1, currentSelected = -1, currentTime = 0;
    char processID[NUM_PROCESSES][100];
    
    // Copy process data from the structure
    for (i = 0; i < NUM_PROCESSES; i++) {
        arrival[i] = data->arrivalTime[i];
        burst[i] = data->burstTime[i];
        originalBurst[i] = burst[i];
        completionTime[i] = 0;
        turnaroundTime[i] = 0;
        waitingTime[i] = 0;
        responseTime[i] = -1;
        completed[i] = 0;
        strcpy(processID[i], data->processID[i]);
    }
    
    // Simulate the scheduling process
    while (!allProcessesCompleted(completed)) {
        prevSelected = currentSelected;
        currentSelected = -1;
        int minArrival = INT_MAX;
        
        // Find the next process with the smallest arrival time that is not completed
        for (i = 0; i < NUM_PROCESSES; i++) {
            if (arrival[i] <= currentTime && completed[i] == 0) {
                if (arrival[i] < minArrival) {
                    minArrival = arrival[i];
                    currentSelected = i;
                }
            }
        }
        
        // If no process is ready, print "idle" and move to the next time unit
        if (currentSelected == -1 && prevSelected != -1) printf("idle\t");
        if (currentSelected != -1 && prevSelected == -1) printf("|%d|\t", currentTime);
        
        if (currentSelected == -1) {
            currentTime++;
        } else {
            // If response time is not yet recorded, set it
            if (responseTime[currentSelected] == -1) responseTime[currentSelected] = currentTime;
            
            // Process execution
            currentTime += burst[currentSelected];
            burst[currentSelected] = 0;
            completed[currentSelected] = 1;
            completionTime[currentSelected] = currentTime;
            printf("%s\t|%d|\t", processID[currentSelected], currentTime);
        }
    }
    
    // Calculate turnaround time and waiting time for each process
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");
    for (i = 0; i < NUM_PROCESSES; i++) {
        turnaroundTime[i] = completionTime[i] - arrival[i];
        waitingTime[i] = turnaroundTime[i] - originalBurst[i];
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\n", processID[i], arrival[i], originalBurst[i], completionTime[i], turnaroundTime[i], waitingTime[i], responseTime[i]);
    }
}

int main() {
    int i;
    ProcessData *processData = (ProcessData *)malloc(sizeof(ProcessData));
    
    // Taking user input for process details
    for (i = 0; i < NUM_PROCESSES; i++) {
        printf("Enter Process ID, Arrival Time, and Burst Time: ");
        scanf("%s %d %d", processData->processID[i], &processData->arrivalTime[i], &processData->burstTime[i]);
    }
    
    pthread_t schedulerThread;
    pthread_create(&schedulerThread, NULL, FCFS, (void *)processData);
    pthread_join(schedulerThread, NULL);
    
    free(processData);
}