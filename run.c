#include<stdio.h>
#include"banker.c"
#include"helper.c"
int main()
{
    printf("Welcome\nThis is the banker's algorithm implementation.\n");
    int resourcesMAX[] = {1,2,3,4,5};
    init1(arraylength(resourcesMAX),resourcesMAX);
    struct BankerData bd;
    //TODO: fix this
    //to2DArray((1,2),(3,4));
    init(&bd,3,2,
        //AvailableResourceArray
        (int[]){1, 2, 3},
        //resourcesDemandMatrix
        (int*[]){
            (int[]){1,1,1},
            (int[]){3,2,4}
            },
        //resourcesAllocatedMatrix
        (int*[]){
            (int[]){0,0,0},
            (int[]){0,0,0}
            },
        //resourcesRequiredMatrix
        (int*[]){
            (int[]){0,0,0},
            (int[]){0,8,0}
            }
        );
    displayBanker(&bd);
}