#include<stdio.h>
#include"banker.h"
#include"helper.h"
int main()
{
    printf("Welcome\nThis is the banker's algorithm implementation.\n");
    int resourcesMAX[] = {1,2,3,4,5};
    init1(arraylength(resourcesMAX),resourcesMAX);
    struct BankerData bd;
    //TODO: fix this
    //to2DArray((1,2),(3,4));
    int initcode = Banker_init(&bd,3,2,
        //MaxResourceArray
        (int[]){3, 3, 30},
        //resourcesDemandMatrix
        (int*[]){
            (int[]){1,1,1},
            (int[]){3,3,4}
            },
        //resourcesAllocatedMatrix
        (int*[]){
            (int[]){0,1,0},
            (int[]){3,1,4}
            }
        );
    printf("init code = %d\n",initcode);
    Banker_displayBanker(&bd);
    printf("Safe status: %d", Banker_getSafeSequence(&bd));
}