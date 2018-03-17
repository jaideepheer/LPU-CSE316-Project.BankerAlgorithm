/*
 * This is the banker.
*/
// Include Files.
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"helper.c"
#include"banker.h"

/*
    Takes a BankerData structure and corresponding data to initialize it.
    Return:  1 on succesfull initialisation.
            -1 if availableResourcesCount is less than 1.
            -2 if resourcesRequiredMatrix values are not a difference of resourcesDemandMatrix and resourcesAllocatedMatrix values.
            -3 if sum of allocated and required resources for a process exceeds the max available resources.
    TODO: Check if given values are correct.
*/
int Banker_init(struct BankerData *data, int availableResourcesCount,int processCount, 
        int* availableResourcesArray, int** resourcesDemandMatrix, int** resourcesAllocatedMatrix, 
        int** resourcesRequiredMatrix)
{
    if(availableResourcesCount<1)return -1;
    // Check if resourcesRequiredMatrix has valid values.
    int i,j,v;
    for(i=0;i<processCount;++i)
    {
        for(j=0;j<availableResourcesCount;++j)
        {
            v = (resourcesDemandMatrix[i][j]-resourcesAllocatedMatrix[i][j]);
            if(v<0 || resourcesRequiredMatrix[i][j]!=v)return -2;
            if((resourcesAllocatedMatrix[i][j]+resourcesRequiredMatrix[i][j])>availableResourcesArray[j])return -3;
        }
    }
    // All is good.
    // Initialise the concurrencyLock of our BankerData structure.
    pthread_mutex_init(&(data->concurrencyLock), NULL);
    // Store the data in our BankerData structure.
    data->processCount = processCount;
    data->availableResourcesCount = availableResourcesCount;
    data->availableResourcesArray = availableResourcesArray;
    data->resourcesDemandMatrix = resourcesDemandMatrix;
    data->resourcesAllocatedMatrix = resourcesAllocatedMatrix;
    data->resourcesRequiredMatrix = resourcesRequiredMatrix;
    return 1;
}

/*
    Gives resource to the caller if it is available and does not lead to an unsafe state.
    Parameters: banker,         the BankerData structure storing the current state of the banker
                resourceIndex,  the index number of the resource to allocate to the caller
                resourceCount,  the numbers of resource instances to allocate to the caller

    Return:  1 on successfull resource allocation.
            -1 if the resourceIndex is invalid.
            -2 if process index is invalid.
            -3 if process is requests more resources than its max requirement or resourceCount is less than 0.
            -4 if resource allocation cannot be done due to unsafe state.
*/
int Banker_requestResource(struct BankerData *banker,int processIndex, int resourceIndex, int resourceCount)
{
    // Check for validity of passed parameters.
    if(resourceIndex<0 || resourceIndex>(banker->availableResourcesCount)-1)return -1;
    if(processIndex<0 || processIndex>(banker->processCount)-1)return -2;
    if(resourceCount>(banker->resourcesDemandMatrix[processIndex][resourceIndex]) || resourceCount<0)return -3;

    // This will be returned ny the function.
    int returnCode = 1;
    
    // Use a mutex lock to revent concurrent resource allocation.
    pthread_mutex_lock(&(banker->concurrencyLock));

    // Allocate the resources to simulate if the banker will be in a safe state.
    banker->resourcesAllocatedMatrix[processIndex][resourceIndex]+=resourceCount;
    banker->availableResourcesArray[resourceIndex]-=resourceCount;
    banker->resourcesRequiredMatrix[processIndex][resourceIndex]-=resourceCount;

    int* safeSequence = Banker_getSafeSequence(banker);
    // If the new state is unsafe, revert changes and set returnCode to signal faliure in allocation.
    if(safeSequence == NULL)
    {
        // Revert allocated resources.
        banker->resourcesAllocatedMatrix[processIndex][resourceIndex]-=resourceCount;
        banker->availableResourcesArray[resourceIndex]+=resourceCount;
        banker->resourcesRequiredMatrix[processIndex][resourceIndex]+=resourceCount;
        returnCode = -4;
    }

    // Unlock the mutex lock to allow other threads to allocate resources.
    pthread_mutex_unlock(&(banker->concurrencyLock));
    
    // Free the safeSequence array after use.
    free(safeSequence);

    return returnCode;
}

/*
    Checks if the passed BankerData is in a safe state and returns the safe sequence.
    Return:  NULL, if the BankerData is not in a safe state i.e. there is no safe sequence.
             int*, array of size processCount(of the BankerData passed) which stores the index of processes, in the safe sequence.
*/
int* Banker_getSafeSequence(struct BankerData *banker)
{
    // Allocate memory to store the safe sequence.
    int* safeSequence = (int*)malloc((banker->processCount)*sizeof(int));

    // No safe sequence found.
    free(safeSequence);
    return NULL;
}

/*
    Used for debugging.
    Prints the data of the given banker structure.
*/
void Banker_displayBanker(struct BankerData *data)
{
    int i,j,k;
    printf("\tBanker Data\n");
    int** matrices[] = {data->resourcesDemandMatrix,data->resourcesAllocatedMatrix,data->resourcesRequiredMatrix};
    for(k=0;k<arraylength(matrices);++k)
    {
        printf("\t  %s Matrix:-\n",
        (k==0)?"Recources Demand":(k==1)?"Resources Allocated":"Resources Required");
        for(i=0;i<data->processCount;++i)
        {
            printf("\t\t");
            for(j=0;j<data->availableResourcesCount;++j)
            {
                printf("%d ",matrices[k][i][j]);
            }
            printf("\n");
        }
    }
}
int init1(int resourceCount, int *resourcesMAX)
{
    printf("resourceCount = %d\n",resourceCount);
    return 0;
}
