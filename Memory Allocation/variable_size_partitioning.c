#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

int numMemoryBlocks; // Number of memory blocks
int numProcesses; // Number of processes requesting memory
int *memoryBlocks; // Array to store the sizes of memory blocks
int *processRequests; // Array to store the memory requests of processes
char **processNames; // Array to store the names of processes

// Worst Fit Memory Allocation Strategy
void worstFit()
{
    int i, j;
    int internalFragmentation = 0, externalFragmentation = 0;
    int availableMemory[numMemoryBlocks], initialMemory[numMemoryBlocks];
    int requests[numProcesses];
    char processIDs[numProcesses][100];
    
    // Copying memory blocks and process requests to local arrays
    for(i = 0; i < numMemoryBlocks; i++) availableMemory[i] = memoryBlocks[i], initialMemory[i] = memoryBlocks[i];
    for(i = 0; i < numProcesses; i++) requests[i] = processRequests[i];
    for(i = 0; i < numProcesses; i++) strcpy(processIDs[i], processNames[i]);
    
    // Allocating processes using the Worst Fit strategy
    for(i = 0; i < numProcesses; i++)
    {
        int currentRequest = requests[i];
        int max = INT_MIN;
        int selectedBlock = -1;
        
        // Finding the block with the maximum available memory that can fit the request
        for(j = 0; j < numMemoryBlocks; j++)
        {
            if(availableMemory[j] > max && availableMemory[j] > currentRequest)
            {
                max = availableMemory[j];
                selectedBlock = j;
            }
        }
        
        // If no suitable block is found, the process remains unallocated
        if(selectedBlock == -1) 
        {
            printf("%s is unallocated\n\n", processIDs[i]);
        }
        else
        {
            printf("%s allocated to memory %d\n", processIDs[i], availableMemory[selectedBlock]);
            availableMemory[selectedBlock] -= currentRequest;
            printf("Updated memory : %d\n", availableMemory[selectedBlock]);
        }
    }
    
    // Calculating internal and external fragmentation
    for(i = 0; i < numMemoryBlocks; i++)
    {
        if(initialMemory[i] == availableMemory[i]) 
            externalFragmentation += availableMemory[i];
        else 
            internalFragmentation += availableMemory[i];
    }
    printf("INF : %d EXF %d", internalFragmentation, externalFragmentation);
}   

// Best Fit Memory Allocation Strategy
void bestFit()
{
    int i, j;
    
    int availableMemory[numMemoryBlocks];
    int requests[numProcesses];
    char processIDs[numProcesses][100];
    
    // Copying memory blocks and process requests to local arrays
    for(i = 0; i < numMemoryBlocks; i++) availableMemory[i] = memoryBlocks[i];
    for(i = 0; i < numProcesses; i++) requests[i] = processRequests[i];
    for(i = 0; i < numProcesses; i++) strcpy(processIDs[i], processNames[i]);
    
    int difference[numMemoryBlocks];
    for(i = 0; i < numProcesses; i++)
    {
        int currentRequest = requests[i];
        int min = INT_MAX;
        int selectedBlock = -1;
        
        // Calculating the difference between block sizes and the request
        for(j = 0; j < numMemoryBlocks; j++)
        {
            difference[j] = availableMemory[j] - currentRequest;
        }
        
        // Finding the smallest available block that can fit the request
        for(j = 0; j < numMemoryBlocks; j++)
        {
            if(difference[j] < min && difference[j] >= 0)
            {
                min = availableMemory[j];
                selectedBlock = j;
            }
        }
        
        // If no suitable block is found, the process remains unallocated
        if(selectedBlock == -1) 
        {
            printf("%s is unallocated\n\n", processIDs[i]);
        }
        else
        {
            printf("%s allocated to memory %d\n", processIDs[i], availableMemory[selectedBlock]);
            availableMemory[selectedBlock] -= currentRequest;
            printf("Updated memory : %d\n", availableMemory[selectedBlock]);
        }
    }
}   

// First Fit Memory Allocation Strategy
void firstFit()
{
    int i, j;
    
    int availableMemory[numMemoryBlocks];
    int requests[numProcesses];
    char processIDs[numProcesses][100];
    
    // Copying memory blocks and process requests to local arrays
    for(i = 0; i < numMemoryBlocks; i++) availableMemory[i] = memoryBlocks[i];
    for(i = 0; i < numProcesses; i++) requests[i] = processRequests[i];
    for(i = 0; i < numProcesses; i++) strcpy(processIDs[i], processNames[i]);
    
    for(i = 0; i < numProcesses; i++)
    {
        int currentRequest = requests[i];
        int selectedBlock = -1;
        
        // Finding the first available block that can fit the request
        for(j = 0; j < numMemoryBlocks; j++)
        {
            if(availableMemory[j] >= currentRequest)
            {
                printf("%s allocated to memory %d\n", processIDs[i], availableMemory[j]);
                availableMemory[j] -= currentRequest;
                printf("Updated memory : %d\n\n", availableMemory[j]);
                selectedBlock = j;
                break;
            }
        }
        
        // If no suitable block is found, the process remains unallocated
        if(selectedBlock == -1) 
        {
            printf("%s is unallocated\n\n", processIDs[i]);
        }
    }
}   

int main()
{
    numMemoryBlocks = 3; // Number of memory blocks
    numProcesses = 5; // Number of processes requesting memory
    
    memoryBlocks = (int*)malloc(sizeof(int) * numMemoryBlocks);
    processRequests = (int*)malloc(sizeof(int) * numProcesses);
    processNames = (char**)malloc(sizeof(char*) * numProcesses);
    
    int i;
    printf("Processes : ");
    for(i = 0; i < numProcesses; i++)
    {
        processNames[i] = (char*)malloc(sizeof(char) * 100);
        scanf("%s", processNames[i]);
    }
    printf("Requests : ");
    for(i = 0; i < numProcesses; i++)
    {
        scanf("%d", &processRequests[i]);
    }
    printf("Memory : ");
    for(i = 0; i < numMemoryBlocks; i++)
    {
        scanf("%d", &memoryBlocks[i]);
    }
    
    worstFit(); // Uncomment to test Worst Fit
    //bestFit(); // Uncomment to test Best Fit
    //firstFit(); // Uncomment to test First Fit
}