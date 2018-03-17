#ifndef __BANKER_H__
#define __BANKER_H__
#include<pthread.h>

// This is a data structure to store all data related to an instance of the banker.
struct BankerData
{
    int availableResourcesCount;
    int processCount;
    int* maxResourcesArray;
    int* availableResourcesArray;
    int** resourcesDemandMatrix;
    int** resourcesAllocatedMatrix;
    int** resourcesRequiredMatrix;

    // This can be used to prevent concurrent access in cases where needed.
    pthread_mutex_t concurrencyLock;
};
// Function Declaritions.
int Banker_init(struct BankerData*, int, int, int*, int**, int**);
int Banker_requestResource(struct BankerData*, int, int, int);
int* Banker_getSafeSequence(struct BankerData*);

#endif // __BANKER_H__