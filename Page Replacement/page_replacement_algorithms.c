#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

int numPages, numFrames;
int *pageRequests;
int *pageFrames;

// Function to print the current state of page frames
void printFrames(int frames[])
{
    int i;
    printf("Frame content: ");
    for(i = 0; i < numFrames; i++) printf("%d ", frames[i]);
    printf("\n");
}

// Function to check if a page is already in frames
int isPageInFrames(int frames[], int page, int requests[])
{
    int i;
    for(i = 0; i < numFrames; i++)
    {
        if(page == frames[i]) return 1; // Page hit
    }
    return 0; // Page miss
}

// Function to replace pages using FIFO algorithm
void replaceFIFO(int frames[], int page, int *index)
{
    int i;
    for(i = 0; i < numFrames; i++)
    {
        if(frames[i] == -1) // If an empty slot is available
        {
            frames[i] = page;
            return;
        }
    }
    // Replace the oldest page (FIFO policy)
    frames[*index] = page;
    *index = (*index + 1) % numFrames; // Move to the next frame
}

// FIFO Page Replacement Algorithm
void FIFO()
{
    printf("FIFO\n");
    int requests[numPages], frames[numFrames];
    int i;
    for(i = 0; i < numPages; i++)
    {
        requests[i] = pageRequests[i];
    }
    for(i = 0; i < numFrames; i++)
    {
        frames[i] = pageFrames[i];
    }
    int hits = 0, misses = 0;
    int index = 0;
    for(i = 0; i < numPages; i++)
    {
        int page = requests[i];
        int found = isPageInFrames(frames, page, requests);
        if(found) 
        {
            printf("(HIT) ");
            hits++;
        }
        else
        {
            printf("(FAULT) ");
            replaceFIFO(frames, page, &index);
        }
        printFrames(frames);
    }
    misses = numPages - hits;
    printf("Hit Ratio: %d/%d\tMiss Ratio: %d/%d\n\n", hits, numPages, misses, numPages);
}

// Function to replace pages using LRU algorithm
void replaceLRU(int frames[], int page, int currentIndex, int requests[])
{
    int i, j;
    int lastUsed[numFrames];
    for(i = 0; i < numFrames; i++)
    {
        if(frames[i] == -1) // If an empty slot is available
        {
            frames[i] = page;
            return;
        }
        lastUsed[i] = -1;
    }
    
    // Find the least recently used page
    for(i = 0; i < numFrames; i++)
    {
        int check = frames[i];
        for(j = 0; j < currentIndex; j++)
        {
            if(check == requests[j])
            {
                lastUsed[i] = j;
            }
        }
    }
    int min = INT_MAX;
    int replaceIndex = -1;
    for(i = 0; i < numFrames; i++)
    {
        if(lastUsed[i] == -1)
        {
            frames[i] = page;
            return;
        }
        if(lastUsed[i] < min) 
        {
            min = lastUsed[i];
            replaceIndex = i;
        }
    }
    frames[replaceIndex] = page;
}

// Function to replace pages using Optimal algorithm
void replaceOptimal(int frames[], int page, int currentIndex, int requests[])
{
    int i, j;
    int nextUse[numFrames];
    for(i = 0; i < numFrames; i++)
    {
        if(frames[i] == -1) // If an empty slot is available
        {
            frames[i] = page;
            return;
        }
        nextUse[i] = -1;
    }
    
    // Find the page that will be used farthest in future
    for(i = 0; i < numFrames; i++)
    {
        int check = frames[i];
        for(j = numPages - 1; j > currentIndex; j--)
        {
            if(check == requests[j])
            {
                nextUse[i] = j;
            }
        }
    }
    int max = INT_MIN;
    int replaceIndex = -1;
    for(i = 0; i < numFrames; i++)
    {
        if(nextUse[i] == -1)
        {
            frames[i] = page;
            return;
        }
        if(nextUse[i] > max) 
        {
            max = nextUse[i];
            replaceIndex = i;
        }
    }
    frames[replaceIndex] = page;
}

// Optimal Page Replacement Algorithm
void Optimal()
{
    printf("Optimal\n");
    int requests[numPages], frames[numFrames];
    int i;
    for(i = 0; i < numPages; i++)
    {
        requests[i] = pageRequests[i];
    }
    for(i = 0; i < numFrames; i++)
    {
        frames[i] = pageFrames[i];
    }
    int hits = 0, misses = 0;
    for(i = 0; i < numPages; i++)
    {
        int page = requests[i];
        int found = isPageInFrames(frames, page, requests);
        if(found) 
        {
            printf("(HIT) ");
            hits++;
        }
        else
        {
            printf("(FAULT) ");
            replaceOptimal(frames, page, i, requests);
        }
        printFrames(frames);
    }
    misses = numPages - hits;
    printf("Hit Ratio: %d/%d\tMiss Ratio: %d/%d\n\n", hits, numPages, misses, numPages);
}

int main()
{
    numPages = 13;
    numFrames = 3;
    pageRequests = (int*)malloc(sizeof(int) * numPages);
    pageFrames = (int*)malloc(sizeof(int) * numFrames);

    int i;
    printf("Enter page requests: ");
    for(i = 0; i < numPages; i++)
    {
        scanf("%d", &pageRequests[i]);
    }
    for(i = 0; i < numFrames; i++)
    {
        pageFrames[i] = -1; // Initialize all frames to empty (-1)
    }
    //FIFO();
    //LRU();
    Optimal();
}
