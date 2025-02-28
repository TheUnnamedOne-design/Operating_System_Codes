#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<string.h>

#define n 6  // Number of processes
#define rn 4 // Number of resource types

char pid[n][100]; // Process identifiers
int avail[rn];    // Available resources
int max[n][rn];   // Maximum demand of each process
int alloc[n][rn]; // Allocated resources for each process
int need[n][rn];  // Remaining needs of each process

// Function to add two resource vectors
void add(int a[], int b[], int c[]) {
    int i;
    for(i = 0; i < rn; i++) a[i] = b[i] + c[i];
}

// Function to subtract two resource vectors
void sub(int a[], int b[], int c[]) {
    int i;
    for(i = 0; i < rn; i++) a[i] = b[i] - c[i];
}

// Function to check if one resource vector is greater than or equal to another
int agb(int a[], int b[]) {
    int i;
    for(i = 0; i < rn; i++) if(a[i] < b[i]) return 0;
    return 1;
}

// Function to check if all processes are completed
int iscomp(int comp[]) {
    int i;
    for(i = 0; i < n; i++) if(comp[i] == 0) return 0;
    return 1;
}

// Function to print a resource vector
void printer(int a[]) {
    int i;
    for(i = 0; i < rn; i++) printf("%d ", a[i]);
}

// Banker's Algorithm for Deadlock Detection
int bankers() {
    int i, j;
    int comp[n]; // Array to track completed processes
    for(i = 0; i < n; i++) comp[i] = 0;
    int ind = -1;

    while(!iscomp(comp)) {
        ind = -1;
        for(i = 0; i < n; i++) {
            if(agb(avail, need[i]) && comp[i] == 0) { // Check if resources can be allocated
                printf("%s granted resources\n", pid[i]);
                printf("Previously available : ");
                printer(avail);
                printf("\n");

                add(avail, avail, alloc[i]); // Release allocated resources back

                printf("Modified available : ");
                printer(avail);
                printf("\n\n");
                ind = 1;
                comp[i] = 1;
            }
        }
        if(ind == -1) { // If no process could proceed, deadlock detected
            printf("Deadlock\n\n");
            return 0;
        }
    }
    return 1; // No deadlock
}

// Function to copy one resource vector to another
void equals(int a[], int b[]) {
    int i;
    for(i = 0; i < rn; i++) a[i] = b[i];
}

int main() {
    int i, j, k;
    
    // Input process names
    printf("Enter process names :\n");
    for(j = 0; j < n; j++) {
        scanf("%s", pid[j]);
    }

    // Input allocation matrix
    printf("Enter allocation matrix :\n");
    for(i = 0; i < n; i++) {
        for(j = 0; j < rn; j++) {
            scanf("%d", &alloc[i][j]);
        }
    }
    
    // Input max matrix
    printf("Enter max matrix :\n");
    for(i = 0; i < n; i++) {
        for(j = 0; j < rn; j++) {
            scanf("%d", &max[i][j]);
        }
    }
    
    // Input available resources
    printf("Enter available matrix :\n");
    for(j = 0; j < rn; j++) {
        scanf("%d", &avail[j]);
    }
    
    // Compute need matrix
    for(i = 0; i < n; i++) sub(need[i], max[i], alloc[i]);
    
    // Input resource requests
    int reqn = 1;
    int requests[reqn][rn];
    int index[reqn];
    for(i = 0; i < reqn; i++) {
        printf("Enter process request index : ");
        scanf("%d", &index[i]);
        printf("Enter resources : ");
        for(j = 0; j < rn; j++) {
            scanf("%d", &requests[i][j]);
        }
    }
    
    // Print need matrix
    printf("Need matrix : \n");
    for(i = 0; i < n; i++) {
        printer(need[i]);
        printf("\n");
    }
    
    // Process resource requests
    for(i = 0; i < reqn; i++) {
        int ind = index[i];
        if(agb(requests[i], need[ind])) {
            printf("Invalid request as request exceeds maximum required : %s\n", pid[ind]);
        } else {
            if(agb(requests[i], avail)) {
                printf("Request for %s cannot be granted. Exceeds available.\n", pid[ind]);
            } else {
                int hold[rn];
                equals(hold, avail);
                sub(avail, avail, requests[i]);
                add(alloc[ind], alloc[ind], requests[i]);
                sub(need[ind], need[ind], requests[i]);
                int ch = bankers();
                
                if(ch) printf("Request granted for %s\n", pid[ind]);
                else {
                    printf("Request not granted for %s\n", pid[ind]);
                    equals(avail, hold);
                    sub(alloc[ind], alloc[ind], requests[i]);
                    add(need[ind], need[ind], requests[i]);
                }
            }
        }
    }
}