#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

#define PROCESS_COUNT 5 // Number of processes

// Structure to store process data
typedef struct ProcessData {
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT], priority[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];
} ProcessData;

// Function to check if all processes are completed
int allProcessesCompleted(int completed[]) {
    for (int i = 0; i < PROCESS_COUNT; i++) {
        if (completed[i] == 0) return 0;
    }
    return 1;
}

// Function for Non-Preemptive Priority Scheduling
void *priorityScheduling(void *ptr) {
    int i, j;
    ProcessData *data = (ProcessData *)ptr;
    
    // Declare and initialize necessary arrays
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT], originalBurstTime[PROCESS_COUNT];
    int completionTime[PROCESS_COUNT], turnaroundTime[PROCESS_COUNT], waitingTime[PROCESS_COUNT], responseTime[PROCESS_COUNT];
    int priority[PROCESS_COUNT], completed[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];
    
    int prevProcess = -1, selectedProcess = -1, currentTime = 0;
    
    // Copy input data to local variables
    for (i = 0; i < PROCESS_COUNT; i++) {
        arrivalTime[i] = data->arrivalTime[i];
        burstTime[i] = data->burstTime[i];
        priority[i] = data->priority[i];
        originalBurstTime[i] = burstTime[i];
        completionTime[i] = 0;
        turnaroundTime[i] = 0;
        waitingTime[i] = 0;
        responseTime[i] = -1;
        completed[i] = 0;
        strcpy(processID[i], data->processID[i]);
    }
    
    // Scheduling loop
    while (!allProcessesCompleted(completed)) {
        prevProcess = selectedProcess;
        selectedProcess = -1;
        int highestPriority = INT_MAX;
        
        // Find the highest priority process available at the current time
        for (i = 0; i < PROCESS_COUNT; i++) {
            if (arrivalTime[i] <= currentTime && completed[i] == 0) {
                if (priority[i] < highestPriority) {
                    highestPriority = priority[i];
                    selectedProcess = i;
                }
            }
        }
        
        // If no process is ready, system remains idle
        if (selectedProcess == -1 && prevProcess != -1) printf("idle\t");
        if (selectedProcess != -1 && prevProcess == -1) printf("|%d|\t", currentTime);
        
        // Process execution
        if (selectedProcess == -1) {
            currentTime++;
        } else {
            if (responseTime[selectedProcess] == -1) responseTime[selectedProcess] = currentTime;
            currentTime += burstTime[selectedProcess];
            burstTime[selectedProcess] = 0;
            completed[selectedProcess] = 1;
            completionTime[selectedProcess] = currentTime;
            printf("%s\t|%d|\t", processID[selectedProcess], currentTime);
        }
    }
    
    // Calculate turnaround time and waiting time
    printf("\nPID\tAT\tBT\tPrio\tCT\tTAT\tWT\tRT\n");
    for (i = 0; i < PROCESS_COUNT; i++) {
        turnaroundTime[i] = completionTime[i] - arrivalTime[i];
        waitingTime[i] = turnaroundTime[i] - originalBurstTime[i];
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", processID[i], arrivalTime[i], originalBurstTime[i], priority[i], completionTime[i], turnaroundTime[i], waitingTime[i], responseTime[i]);
    }
}

int main() {
    int i;
    ProcessData *data = (ProcessData *)malloc(sizeof(ProcessData));
    
    // Take user input for process details
    for (i = 0; i < PROCESS_COUNT; i++) {
        printf("Enter Process ID, Arrival Time, Burst Time, Priority: ");
        scanf("%s%d%d%d", data->processID[i], &data->arrivalTime[i], &data->burstTime[i], &data->priority[i]);
    }
    
    // Create a thread to run the priority scheduling algorithm
    pthread_t schedulerThread;
    pthread_create(&schedulerThread, NULL, priorityScheduling, (void *)data);
    pthread_join(schedulerThread, NULL);
    
    free(data); // Free allocated memory
    return 0;
}