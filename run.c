#include<stdio.h>
#include"banker.c"
#include"helper.c"
int main()
{
    printf("Welcome\nThis is the banker's algorithm implementation.\n");
    int resourcesMAX[] = {1,2,3,4,5};
    init(arraylength(resourcesMAX),resourcesMAX);
}