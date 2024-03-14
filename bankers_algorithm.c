#include <stdio.h>
#include <limits.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

int leastRecoveryCost(int recoveryCost[], int numProcesses) {
    int min = INT_MAX;
    int index = 0;
    for (int i = 1; i < numProcesses; i++) {
        if (recoveryCost[i] < min && recoveryCost[i] != 0) {
            min = recoveryCost[i];
            index = i;
        }
    }
    return index;
}

int findSafeSequence(int available[], int max[][MAX_RESOURCES], int allocation[][MAX_RESOURCES], int need[][MAX_RESOURCES], int numProcesses, int numResources, int unsafeProcesses[]) {
    int work[MAX_RESOURCES];
    int finish[MAX_PROCESSES];
    int safeSequence[MAX_PROCESSES];
    int i, j, k;
    int recoveryCost[MAX_PROCESSES];
    int timesPrempted[MAX_PROCESSES];

    // Initialize work and finish arrays
    for (i = 0; i < numResources; i++) {
        work[i] = available[i];
    }
    for (i = 0; i < numProcesses; i++) {
        finish[i] = 0;
    }
    for (i = 0; i < numProcesses; i++) {
        unsafeProcesses[i] = 0;
        timesPrempted[i] = 0;
    }

    // Find a safe sequence
    int count = 0;
    while (count < numProcesses) {
        int found = 0;
        for (i = 0; i < numProcesses; i++) {
            if (finish[i] == 0) {
                int safe = 1;
                for (j = 0; j < numResources; j++) {
                    if (need[i][j] > work[j]) {
                        safe = 0;
                        unsafeProcesses[i] = 1;
                        break;
                    }
                }
                if (safe) {
                    for (k = 0; k < numResources; k++) {
                        work[k] += allocation[i][k];
                    }
                    safeSequence[count++] = i;
                    unsafeProcesses[i] = 0;
                    finish[i] = 1;
                    found = 1;
                }
            }
        }
        if (!found) {
            printf("System is in an unsafe state.\n");
            for (i = 0; i < numProcesses; i++) {
                if (unsafeProcesses[i]) {
                    printf("Process %d is unsafe.\n", i);
                }
            }
            return 0;
        }
    }

    printf("System is in a safe state.\nSafe sequence: ");
    for (i = 0; i < numProcesses; i++) {
        printf("%d ", safeSequence[i]);
    }
    printf("\n");
    return 1;
}



int main() {
    int numProcesses = 5;
    int numResources = 3;
    int available[] = {0, 0, 0};
    int max[MAX_PROCESSES][MAX_RESOURCES] = {
        {0, 1, 0},
        {4, 0, 2},
        {3, 0, 4},
        {3, 1, 1},
        {0, 0, 4}
    };
    int allocation[MAX_PROCESSES][MAX_RESOURCES] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };
    int need[MAX_PROCESSES][MAX_RESOURCES];
    int recoveryCost[MAX_PROCESSES];
    int timesPrempted[MAX_PROCESSES];
    int isSafe = 0;
    int i, j, k;
    int unsafeProcesses[MAX_PROCESSES];


    
    for (i = 0; i < numProcesses; i++) {
        for (j = 0; j < numResources; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    
    for (i = 0; i < numProcesses; i++) {
        recoveryCost[i] = 0;
        timesPrempted[i] = 0;
    }

    isSafe = findSafeSequence(available, max, allocation, need, numProcesses, numResources, unsafeProcesses);

    while(!isSafe) {
        for (i = 0; i < numProcesses; i++) {
            if (unsafeProcesses[i]) {
                recoveryCost[i] = 0;
                for (j = 0; j < numResources; j++) {
                    recoveryCost[i] += allocation[i][j];
                }
                recoveryCost[i] /= (timesPrempted[i] + 1);
            }
        }
        int leastRecoveryIndex = leastRecoveryCost(recoveryCost, numProcesses);
        printf("Process %d has the least recovery cost.\n", leastRecoveryIndex);
        timesPrempted[leastRecoveryIndex]++;
        printf("Process %d has been preempted %d times.\n", leastRecoveryIndex, timesPrempted[leastRecoveryIndex]);

        for(i = 0; i < numResources; i++) {
            available[i] += allocation[leastRecoveryIndex][i];
            allocation[leastRecoveryIndex][i] = 0;
            need[leastRecoveryIndex][i] = max[leastRecoveryIndex][i];
        }

        isSafe = findSafeSequence(available, max, allocation, need, numProcesses, numResources, unsafeProcesses);

    }

    return 0;
}