#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<limits.h>

#define NUM_PROCESSES 5  // Number of processes

// Structure to store process details
typedef struct ProcessData {
    int arrivalTime[NUM_PROCESSES], burstTime[NUM_PROCESSES];
    char processID[NUM_PROCESSES][100];
} ProcessData;

// Function to check if all processes are completed
int allProcessesCompleted(int completed[]) {
    for(int i = 0; i < NUM_PROCESSES; i++) 
        if(completed[i] == 0) return 0;  // If any process is not completed, return false
    return 1;
}

// Shortest Job First Scheduling (Non-Preemptive)
void *SJF(void *ptr) {
    int i, j;
    ProcessData *data = (ProcessData*)ptr;
    
    int arrival[NUM_PROCESSES], burst[NUM_PROCESSES], originalBurst[NUM_PROCESSES];
    int completionTime[NUM_PROCESSES], turnaroundTime[NUM_PROCESSES], waitingTime[NUM_PROCESSES], responseTime[NUM_PROCESSES], completed[NUM_PROCESSES];
    int prevProcess = -1, selectedProcess = -1, currentTime = 0;
    char processID[NUM_PROCESSES][100];
    
    // Initialize process data
    for(i = 0; i < NUM_PROCESSES; i++) {
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
    
    // Process execution loop
    while (!allProcessesCompleted(completed)) {
        prevProcess = selectedProcess;
        selectedProcess = -1;
        int minBurst = INT_MAX;

        // Find the process with the shortest burst time that has arrived
        for (i = 0; i < NUM_PROCESSES; i++) {
            if (arrival[i] <= currentTime && completed[i] == 0) {
                if (burst[i] < minBurst) {
                    minBurst = burst[i];
                    selectedProcess = i;
                }
            }
        }

        // Handling idle time if no process is ready
        if (selectedProcess == -1 && prevProcess != -1) printf("idle\t");
        if (selectedProcess != -1 && prevProcess == -1) printf("|%d|\t", currentTime);

        // If no process is available, increase time
        if (selectedProcess == -1) {
            currentTime++;
        } else {
            // First time the process is being scheduled
            if (responseTime[selectedProcess] == -1) responseTime[selectedProcess] = currentTime;
            
            // Process execution
            currentTime += burst[selectedProcess];
            burst[selectedProcess] = 0;
            completed[selectedProcess] = 1;
            completionTime[selectedProcess] = currentTime;
            printf("%s\t|%d|\t", processID[selectedProcess], currentTime);
        }
    }
    
    // Calculate Turnaround Time and Waiting Time
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");
    for (i = 0; i < NUM_PROCESSES; i++) {
        turnaroundTime[i] = completionTime[i] - arrival[i];
        waitingTime[i] = turnaroundTime[i] - originalBurst[i];
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\n", processID[i], arrival[i], originalBurst[i], completionTime[i], turnaroundTime[i], waitingTime[i], responseTime[i]);
    }
}

int main() {
    int i;
    ProcessData *processData = (ProcessData*)malloc(sizeof(ProcessData));
    
    // Input process details
    for(i = 0; i < NUM_PROCESSES; i++) {
        printf("Enter Process ID, Arrival Time, and Burst Time: ");
        scanf("%s%d%d", processData->processID[i], &processData->arrivalTime[i], &processData->burstTime[i]);
    }
    
    // Create thread for SJF scheduling
    pthread_t schedulerThread;
    pthread_create(&schedulerThread, NULL, SJF, (void*)processData);
    pthread_join(schedulerThread, NULL);
    
    free(processData);
}