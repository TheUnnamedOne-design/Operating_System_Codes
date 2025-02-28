#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<limits.h>

#define PROCESS_COUNT 5  // Number of processes

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

void *preemptivePriorityScheduling(void *ptr) {
    int i;
    
    ProcessData *data = (ProcessData*) ptr;
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT], initialBurstTime[PROCESS_COUNT];
    int completionTime[PROCESS_COUNT], turnaroundTime[PROCESS_COUNT], waitingTime[PROCESS_COUNT], responseTime[PROCESS_COUNT], completed[PROCESS_COUNT];
    int priority[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];
    
    int previousProcess = -1, selectedProcess = -1, currentTime = 0;
    
    // Initialize process data
    for (i = 0; i < PROCESS_COUNT; i++) {
        arrivalTime[i] = data->arrivalTime[i];
        burstTime[i] = data->burstTime[i];
        priority[i] = data->priority[i];
        initialBurstTime[i] = burstTime[i];
        completionTime[i] = 0;
        turnaroundTime[i] = 0;
        waitingTime[i] = 0;
        responseTime[i] = -1;
        completed[i] = 0;
        strcpy(processID[i], data->processID[i]);
    }
    
    // Scheduling loop
    while (!allProcessesCompleted(completed)) {
        previousProcess = selectedProcess;
        selectedProcess = -1;
        int highestPriority = INT_MAX;
        
        // Select the process with the highest priority (lowest priority value)
        for (i = 0; i < PROCESS_COUNT; i++) {
            if (arrivalTime[i] <= currentTime && completed[i] == 0) {
                if (priority[i] < highestPriority) {
                    highestPriority = priority[i];
                    selectedProcess = i;
                }
            }
        }
        
        // Print idle time if no process is selected
        if (selectedProcess == -1 && previousProcess != -1) printf("idle\t");
        if (selectedProcess != -1 && previousProcess == -1) printf("|%d|\t", currentTime);
        
        if (selectedProcess == -1) {
            currentTime++;
        } else {
            if (responseTime[selectedProcess] == -1) responseTime[selectedProcess] = currentTime;
            int preempted = 0;
            
            // Execute the selected process until preempted
            while (burstTime[selectedProcess] > 0) {
                for (i = 0; i < PROCESS_COUNT; i++) {
                    if (arrivalTime[i] <= currentTime && completed[i] == 0 && priority[i] < priority[selectedProcess]) 
					{ // Condition for when a process of more priority arrives while a process is executing
                        preempted = 1;
                        printf("%s\t|%d|\t", processID[selectedProcess], currentTime);
                        break;
                    }
                }
                if (preempted) break;
                burstTime[selectedProcess]--;
                currentTime++;
            }
            
            if (preempted) continue;
            completed[selectedProcess] = 1;
            completionTime[selectedProcess] = currentTime;
            printf("%s\t|%d|\t", processID[selectedProcess], currentTime);
        }
    }
    
    // Calculate turnaround time and waiting time
    printf("\nPID\tAT\tBT\tPriority\tCT\tTAT\tWT\tRT\n");
    for (i = 0; i < PROCESS_COUNT; i++) {
        turnaroundTime[i] = completionTime[i] - arrivalTime[i];
        waitingTime[i] = turnaroundTime[i] - initialBurstTime[i];
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", processID[i], arrivalTime[i], initialBurstTime[i], priority[i], completionTime[i], turnaroundTime[i], waitingTime[i], responseTime[i]);
    }
}

int main() {
    int i;
    ProcessData *processData = (ProcessData*) malloc(sizeof(ProcessData));
    
    // Taking input from user
    for (i = 0; i < PROCESS_COUNT; i++) {
        printf("Enter Process ID, Arrival Time, Burst Time, Priority: ");
        scanf("%s%d%d%d", processData->processID[i], &processData->arrivalTime[i], &processData->burstTime[i], &processData->priority[i]);
    }
    
    // Create thread to execute the scheduling algorithm
    pthread_t schedulerThread;
    pthread_create(&schedulerThread, NULL, preemptivePriorityScheduling, (void*) processData);
    pthread_join(schedulerThread, NULL);
    
    free(processData);
    return 0;
}
