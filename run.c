#include<stdio.h>
#include<semaphore.h>
#include"banker.h"
#include"helper.h"
#include"UI.h"

struct threadTickBehaviour
{
    // No. of ticks the process lives for and the current tick the process is at.
    int lifeTicks, currentTick;

    // Tick semaphore
    sem_t* tickWaiter;
    // This semaphore is posted on when the thread finishes a tick.
    sem_t* onTickSem;

    // The process index for this thread to be used by the banker.
    int processIndex;

    // The banker to release or demand resources from.
    struct BankerData* banker;

    // The response code of the last request to the banker
    int lastReqResponse;

    // Pointer to array of two integers.
    // its size should be [lifeTicks][2]
    // Stores what resource and its count to be demanded or released per tick.
    // -ve values of count mean release resource, +ve count means demand resource.
    int **perTickResourceDemandMatrix;
};
void *threadFunction(void *tickBehaviour)
{
    struct threadTickBehaviour* behaviour = (struct threadTickBehaviour*)tickBehaviour;
    int resourceCount,currentTick=0;
    behaviour->currentTick=0;

    while(currentTick<behaviour->lifeTicks)
    {
        // Wait on the semaphore for the next tick...
        sem_wait(behaviour->tickWaiter);

        resourceCount = behaviour->perTickResourceDemandMatrix[currentTick][1];
        // Allocate/deallocate resources.
        if(resourceCount>0)behaviour->lastReqResponse = Banker_requestResource(behaviour->banker,
                                                behaviour->processIndex,
                                                behaviour->perTickResourceDemandMatrix[currentTick][0],
                                                resourceCount);
        else behaviour->lastReqResponse = Banker_freeResource(behaviour->banker,
                                    behaviour->processIndex,
                                    behaviour->perTickResourceDemandMatrix[currentTick][0], -resourceCount);
        
        // Check if tick was successfull.
        if(behaviour->lastReqResponse == 1)
        {
            // Tick successfull, resource allocation/release done.
            behaviour->currentTick += 1;
            ++currentTick;
        }
        // Tick done, post on semaphore to signal tick done
        sem_post(behaviour->onTickSem);
    }
}
void initTickBehaviour(struct threadTickBehaviour *tb, int lifeTicks, int **perTickResourceDemandMatrix, sem_t *tickWaiter, sem_t *onTickSem, int processIndex, struct BankerData *banker)
{
    tb->lifeTicks=lifeTicks;
    tb->banker = banker;
    tb->processIndex = processIndex;
    tb->tickWaiter = tickWaiter;
    tb->perTickResourceDemandMatrix = perTickResourceDemandMatrix;
    tb->onTickSem =onTickSem;
}

void run(int threadCount, struct threadTickBehaviour tbarray[5], sem_t* twaiter, sem_t* onTickSem)
{
    int ticksDone,newCount;
    
    // Loop while threads have work to do...
    while(threadCount>0)
    {
        // Resume all threads by allowing them to execute next tick.
        for(ticksDone=0;ticksDone<threadCount;++ticksDone)sem_post(&twaiter[ticksDone]);
        // Reset ticksDone
        ticksDone = 0;

        // Wait for the threads to finish their tick
        while(ticksDone<threadCount)
        {
            // Wait for onTickSem to be posted by a thread...
            sem_wait(onTickSem);
            ++ticksDone;
        }

        // Update threadCount regarding finished threads.
        newCount = 0;
        for(ticksDone=0;ticksDone<threadCount;++ticksDone)
            // Increment for unfinished threads.
            if(tbarray[ticksDone].currentTick!=tbarray[ticksDone].lifeTicks)
                ++newCount;
        threadCount = newCount;

        // Print the current system state.
        Banker_displayBanker(tbarray[0].banker);

        // Check for deadlock, i.e no threads have positive lastReqResponse.
        // Reuse newCount and ticksDone variables...
        newCount = 1;
        for(ticksDone=0;ticksDone<threadCount;++ticksDone)
        {
            // If any thread ticked without errors,i.e has status code 1, there is no deadlock...
            if(tbarray[ticksDone].lastReqResponse == 1)
            {
                newCount = 0;
                break;
            }
        }
        // Print errors on deadlock and break
        if(newCount)
        {
            printf("Oh no...! There were errors,\n");
            break;
        }
    }
}
void init(struct BankerData* bd)
{
    struct threadTickBehaviour tb[5];
    sem_t twaiter[5],onTickSem;
    int i;
    sem_init(&onTickSem, 0, 0);
    for(i=0;i<5;++i){
        sem_init(&twaiter[i], 0, 0);
        initTickBehaviour(&tb[i], 2, (int*[]){(int[]){2,10},(int[]){0,1}}, &twaiter[i], &onTickSem, i, bd);
    }

    pthread_t userThreads[5];
    for(i=0;i<1;++i)pthread_create(&userThreads[i], NULL, threadFunction, &tb[i]);
    run(1,tb,twaiter,&onTickSem);
}

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
        (int[]){3, 3, 50},
        //resourcesDemandMatrix
        (int*[]){
            (int[]){1,1,40},
            (int[]){3,3,40}
            },
        //resourcesAllocatedMatrix
        (int*[]){
            (int[]){0,0,0},
            (int[]){3,1,0}
            }
        );
    printf("init code = %d\n",initcode);
    Banker_displayBanker(&bd);
    printf("Safe status: %d\n", Banker_requestResource(&bd, 0, 1, 1));

     //   system("clear");
    struct UIframe f;
    UIframe_init(&f,12,5,'|','|','-','-', 1, 1);
    char *c = "12345678 hi ab";
    UIframe_addLine(&f, c, 0, 14, 0.5);
    UIframe_print(&f, 0, 0);
    //system("clear");
    UIframe_addLine(&f, c, 0, 4, 0.5);
    UIframe_print(&f, 10, 4);

    init(&bd);
}