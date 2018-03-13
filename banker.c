/*
 * This is the banker.
*/
#include<stdio.h>
#include"helper.c"
// This is a data structure to store all data related to an instance of the banker.
struct BankerData
{
    int availableResourcesCount;
    int processCount;
    int* availableResourcesArray;
    int** resourcesDemandMatrix;
    int** resourcesAllocatedMatrix;
    int** resourcesRequiredMatrix;
};
/*
    Takes a BankerData structure and corresponding data to initialize it.
    Return:  1 on succesfull initialisation.
            -1 if availableResourcesCount is less than 1.
    TODO: Check if given values are correct.
*/
int init(struct BankerData *data, int availableResourcesCount,int processCount, 
        int* availableResourcesArray, int** resourcesDemandMatrix, int** resourcesAllocatedMatrix, 
        int** resourcesRequiredMatrix)
{
    if(availableResourcesCount<1)return -1;
    data->processCount = processCount;
    data->availableResourcesCount = availableResourcesCount;
    data->availableResourcesArray = availableResourcesArray;
    data->resourcesDemandMatrix = resourcesDemandMatrix;
    data->resourcesAllocatedMatrix = resourcesAllocatedMatrix;
    data->resourcesRequiredMatrix = resourcesRequiredMatrix;
    return 1;
}
/*
    Used for debugging.
    Prints the data of the given banker structure.
*/
void displayBanker(struct BankerData *data)
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
