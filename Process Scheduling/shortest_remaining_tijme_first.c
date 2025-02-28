#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<limits.h>

#define PROCESS_COUNT 5  // Number of processes

typedef struct {
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];
} ProcessData;

// Function to check if all processes are completed
int allProcessesCompleted(int completed[]) {
    for (int i = 0; i < PROCESS_COUNT; i++) 
        if (completed[i] == 0) 
            return 0;
    return 1;
}

// Shortest Remaining Time First (SRTF) Scheduling Algorithm
void *SRTF(void *ptr) {
    ProcessData *data = (ProcessData *)ptr;
    
    int arrivalTime[PROCESS_COUNT], burstTime[PROCESS_COUNT], originalBurstTime[PROCESS_COUNT];
    int completionTime[PROCESS_COUNT], turnaroundTime[PROCESS_COUNT], waitingTime[PROCESS_COUNT], responseTime[PROCESS_COUNT];
    int completed[PROCESS_COUNT];
    char processID[PROCESS_COUNT][100];
    
    int prevProcess = -1, selectedProcess = -1, currentTime = 0;

    // Initialize process data
    for (int i = 0; i < PROCESS_COUNT; i++) {
        arrivalTime[i] = data->arrivalTime[i];
        burstTime[i] = data->burstTime[i];
        originalBurstTime[i] = burstTime[i];
        completionTime[i] = 0;
        turnaroundTime[i] = 0;
        waitingTime[i] = 0;
        responseTime[i] = -1;
        completed[i] = 0;
        strcpy(processID[i], data->processID[i]);
    }
    
    // Scheduling process execution
    while (!allProcessesCompleted(completed)) {
        prevProcess = selectedProcess;
        selectedProcess = -1;
        int shortestRemaining = INT_MAX;

        // Select process with the shortest remaining time
        for (int i = 0; i < PROCESS_COUNT; i++) {
            if (arrivalTime[i] <= currentTime && completed[i] == 0) {
                if (burstTime[i] < shortestRemaining) {
                    shortestRemaining = burstTime[i];
                    selectedProcess = i;
                }
            }
        }

        // CPU is idle if no process is selected
        if (selectedProcess == -1 && prevProcess != -1) 
            printf("idle\t");
        if (selectedProcess != -1 && prevProcess == -1) 
            printf("|%d|\t", currentTime);
        
        if (selectedProcess == -1) {
            currentTime++;  // Move time forward if CPU is idle
        } else {
            if (responseTime[selectedProcess] == -1) 
                responseTime[selectedProcess] = currentTime;  // Set response time
            
            int preemptFlag = 0;
            
            // Execute process one time unit at a time
            while (burstTime[selectedProcess] > 0) 
			{
                for (int i = 0; i < PROCESS_COUNT; i++) {
                    if (arrivalTime[i] <= currentTime && completed[i] == 0 && burstTime[i] < burstTime[selectedProcess]) 
					{ //If a process arrives which has lesser burst than the process currently executing, that is allowed to preempt
                        printf("%s\t|%d|\t", processID[selectedProcess], currentTime);
                        preemptFlag = 1;
                        break;
                    }
                }
                if (preemptFlag) 
                    break;
                
                burstTime[selectedProcess]--;
                currentTime++;
            }
            
            if (preemptFlag) 
                continue;
            
            completed[selectedProcess] = 1;  // Mark process as completed
            completionTime[selectedProcess] = currentTime;
            printf("%s\t|%d|\t", processID[selectedProcess], currentTime);
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
    ProcessData *processData = (ProcessData *)malloc(sizeof(ProcessData));
    
    // Take user input for process details
    for (int i = 0; i < PROCESS_COUNT; i++) {
        printf("Enter Process ID, Arrival Time, Burst Time: ");
        scanf("%s %d %d", processData->processID[i], &processData->arrivalTime[i], &processData->burstTime[i]);
    }
    
    pthread_t schedulingThread;
    pthread_create(&schedulingThread, NULL, SRTF, (void *)processData);
    pthread_join(schedulingThread, NULL);
    
    free(processData);
    return 0;
}