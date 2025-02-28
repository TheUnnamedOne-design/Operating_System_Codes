#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>




int numBlocks;  // Number of memory blocks
int numProcesses; // Number of processes requesting memory
int *blockSizes; // Array storing sizes of memory blocks
int *processRequests; // Array storing memory requests of each process
char **processNames; // Array storing process names

// Worst Fit Memory Allocation Strategy
void worstFit() 
{
    int i, j;
    int blocks[numBlocks]; // Copy of blockSizes array
    int requests[numProcesses]; // Copy of processRequests array
    char processIDs[numProcesses][100]; // Copy of process names
    
    // Copying original values to local arrays
    for(i = 0; i < numBlocks; i++) blocks[i] = blockSizes[i];
    for(i = 0; i < numProcesses; i++) requests[i] = processRequests[i];
    for(i = 0; i < numProcesses; i++) strcpy(processIDs[i], processNames[i]);
    
    int allocated[numBlocks]; // Allocation status of blocks
    for(i = 0; i < numBlocks; i++) allocated[i] = 0;
    
    for(i = 0; i < numProcesses; i++)
    {
        int request = requests[i];
        int max = INT_MIN;
        int index = -1;
        
        // Finding the largest available block that fits the request
        for(j = 0; j < numBlocks; j++)
        {
            if(blocks[j] > max && allocated[j] == 0 && blocks[j] > request)
            {
                max = blocks[j];
                index = j;
            }
        }
        
        if(index == -1) 
        {
            printf("%s is unallocated\n\n", processIDs[i]);
        }
        else
        {
            printf("%s allocated to block %d\n", processIDs[i], blocks[index]);
            blocks[index] -= request;
            allocated[index] = 1; // Mark block as allocated
            printf("Updated block size: %d\n", blocks[index]);
        }
    }
}

// Best Fit Memory Allocation Strategy
void bestFit()
{
    int i, j;
    int blocks[numBlocks]; // Copy of blockSizes array
    int requests[numProcesses]; // Copy of processRequests array
    char processIDs[numProcesses][100]; // Copy of process names
    
    // Copying original values to local arrays
    for(i = 0; i < numBlocks; i++) blocks[i] = blockSizes[i];
    for(i = 0; i < numProcesses; i++) requests[i] = processRequests[i];
    for(i = 0; i < numProcesses; i++) strcpy(processIDs[i], processNames[i]);
    
    int allocated[numBlocks]; // Allocation status of blocks
    for(i = 0; i < numBlocks; i++) allocated[i] = 0;
    
    int diff[numBlocks]; // Stores the difference between block size and request size
    for(i = 0; i < numProcesses; i++)
    {
        int request = requests[i];
        int min = INT_MAX;
        int index = -1;
        
        // Calculate block-request differences
        for(j = 0; j < numBlocks; j++)
        {
            diff[j] = blocks[j] - request;
        }
        
        // Find the smallest suitable block
        for(j = 0; j < numBlocks; j++)
        {
            if(diff[j] < min && allocated[j] == 0 && diff[j] >= 0)
            {
                min = blocks[j];
                index = j;
            }
        }
        
        if(index == -1) 
        {
            printf("%s is unallocated\n\n", processIDs[i]);
        }
        else
        {
            printf("%s allocated to block %d\n", processIDs[i], blocks[index]);
            blocks[index] -= request;
            allocated[index] = 1; // Mark block as allocated
            printf("Updated block size: %d\n", blocks[index]);
        }
    }
}

// First Fit Memory Allocation Strategy
void firstFit()
{
    int i, j;
    int blocks[numBlocks]; // Copy of blockSizes array
    int requests[numProcesses]; // Copy of processRequests array
    char processIDs[numProcesses][100]; // Copy of process names
    
    // Copying original values to local arrays
    for(i = 0; i < numBlocks; i++) blocks[i] = blockSizes[i];
    for(i = 0; i < numProcesses; i++) requests[i] = processRequests[i];
    for(i = 0; i < numProcesses; i++) strcpy(processIDs[i], processNames[i]);
    
    int allocated[numBlocks]; // Allocation status of blocks
    for(i = 0; i < numBlocks; i++) allocated[i] = 0;
    
    for(i = 0; i < numProcesses; i++)
    {
        int request = requests[i];
        int index = -1;
        
        // Allocating the first suitable block found
        for(j = 0; j < numBlocks; j++)
        {
            if(allocated[j] == 0 && blocks[j] >= request)
            {
                printf("%s allocated to block %d\n", processIDs[i], blocks[j]);
                blocks[j] -= request;
                allocated[j] = 1; // Mark block as allocated
                printf("Updated block size: %d\n", blocks[j]);
                index = j;
                break;
            }
        }
        
        if(index == -1) 
        {
            printf("%s is unallocated\n\n", processIDs[i]);
        }
    }
}

int main()
{
    numBlocks = 3;
    numProcesses = 5;
    blockSizes = (int*)malloc(sizeof(int) * numBlocks);
    processRequests = (int*)malloc(sizeof(int) * numProcesses);
    processNames = (char**)malloc(sizeof(char*) * numProcesses);
    
    int i;
    printf("Processes: ");
    for(i = 0; i < numProcesses; i++)
    {
        processNames[i] = (char*)malloc(sizeof(char) * 100);
        scanf("%s", processNames[i]);
    }
    
    printf("Requests: ");
    for(i = 0; i < numProcesses; i++)
    {
        scanf("%d", &processRequests[i]);
    }
    
    printf("Memory Blocks: ");
    for(i = 0; i < numBlocks; i++)
    {
        scanf("%d", &blockSizes[i]);
    }
    
    // worstFit(); // Uncomment to use Worst Fit
    // bestFit(); // Uncomment to use Best Fit
    firstFit(); // Currently using First Fit
}
