#include<stdio.h>
#include"banker.h"
#include"helper.c"
int main()
{
    printf("Welcome\nThis is the banker's algorithm implementation.\n");
    int resourcesMAX[] = {1,2,3,4,5};
    init1(arraylength(resourcesMAX),resourcesMAX);
    struct BankerData bd;
    //TODO: fix this
    //to2DArray((1,2),(3,4));
    int initcode = Banker_init(&bd,3,2,
        //AvailableResourceArray
        (int[]){10, 20, 30},
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
            (int[]){1,1,1},
            (int[]){3,2,4}
            }
        );
    printf("init code = %d\n",initcode);
    Banker_displayBanker(&bd);
}