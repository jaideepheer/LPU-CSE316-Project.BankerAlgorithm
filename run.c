#include<stdio.h>
#include<semaphore.h>
#include<string.h>
#include"banker.h"
#include"helper.h"
#include"UI.h"

struct systemContext
{
    struct UIframe* currentFrame;
    struct threadTickBehaviour *tickBehaviourArray;
    struct BankerData* bankerData;
    int threadCount;
    // This semaphore is posted on by each thread when the it finishes a tick.
    sem_t* onTickSem;

    struct UITextField* textFieldsArray;
};
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

void prepareRunScreen(struct systemContext* context)
{
    UIframe_flush(context->currentFrame);
    UIframe_addLine(context->currentFrame, "|Running System|", 0, 0.5);
    UIframe_addLine(context->currentFrame, "|~~~~~~~~~~~~~~|", 0, 0.5);
    UIframe_addLine(context->currentFrame,"",0,0);
    UIframe_addLine(context->currentFrame,"Below displayed is the current state of the threads that request resources from the banker.",0,0.15);
    UIframe_addLine(context->currentFrame,"",0,0);
    UIframe_addLine(context->currentFrame,"               CurrentTick | Requested |        RequestResult        | CurrentResources",0,0);

    int i;
    for(i=0;i<context->threadCount;++i)
    {
        // Add lines with text fields...
        struct UITextField* tfarray = &context->textFieldsArray[i*7];
        UIframe_addLineAndBindTextFeilds(context->currentFrame,
            "    Thread %f2:    %f2/%f2       R%f2 x%f2     %f23       %f15",
            0,0,tfarray);
    }
    ++context->currentFrame->cursor[0];
    UIframe_addLine(context->currentFrame,"Press any key to continue tick.",0,0.5);
}
void printRunScreen(struct systemContext* context, int x, int y)
{
    clrscr();
    int i,lasttick,k;
    char *lastResult;
    char curRes[30],tmp[10];
    struct threadTickBehaviour* currenttb;
    struct UITextField* tfarray;
    for(i=0;i<context->threadCount;++i)
    {
        currenttb = (context->tickBehaviourArray)+i;
        lasttick = currenttb->lastReqResponse==1?currenttb->currentTick-1:currenttb->currentTick;
        snprintf(curRes,29,"");
        switch(currenttb->lastReqResponse)
        {
            case  1:lastResult = "Acquired Resources"; break;
            case -1:lastResult = "Invalid ResourceIndex"; break;
            case -2:lastResult = "Invalid ProcessIndex"; break;
            case -3:lastResult = "Request more than max."; break;
            case -4:lastResult = "Unsafe State"; break;
            case -5:lastResult = "Resource Unavailable"; break;
            default: lastResult = "Unknown Result";
        }
        for(k=0;k<currenttb->banker->availableResourcesCount;++k)
        {
            snprintf(tmp,9," %d",currenttb->banker->resourcesAllocatedMatrix[currenttb->processIndex][k]);
            strcat(curRes,tmp);
        }

        tfarray = &context->textFieldsArray[i*7];
        // Set the text field values.
        UItextField_setText(tfarray+0,"%d",currenttb->processIndex);
        UItextField_setText(tfarray+1,"%d",lasttick+1);
        UItextField_setText(tfarray+2,"%d",currenttb->lifeTicks);
        UItextField_setText(tfarray+3,"%d",currenttb->perTickResourceDemandMatrix[lasttick][0]+1);
        UItextField_setText(tfarray+4,"%d",currenttb->perTickResourceDemandMatrix[lasttick][1]);
        UItextField_setText(tfarray+5,"%s",lastResult);
        UItextField_setText(tfarray+6,"%s",curRes);
    }
    UIframe_print(context->currentFrame,x,y);
}
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
    // Release all resources.
    Banker_freeAllResources(behaviour->banker,behaviour->processIndex);
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
void run(struct systemContext* context)
{
    int ticksDone,newCount,runningThreadCount=context->threadCount,noUpdateIteration;
    // Prepare the current UIframe by laying out the TextFields.
    prepareRunScreen(context);
    // Loop while threads have work to do...
    while(runningThreadCount>0)
    {
        // Resume all threads by allowing them to execute next tick.
        for(ticksDone=0;ticksDone<context->threadCount;++ticksDone)sem_post(context->tickBehaviourArray[ticksDone].tickWaiter);
        // Reset ticksDone
        ticksDone = 0;

        // Wait for the threads to finish their tick
        while(ticksDone<runningThreadCount)
        {
            // Wait for onTickSem to be posted by a thread...
            sem_wait(context->onTickSem);
            ++ticksDone;
        }

        // Update threadCount regarding finished threads.
        runningThreadCount = 0;
        for(ticksDone=0;ticksDone<context->threadCount;++ticksDone)
            // Increment for unfinished threads.
            if(context->tickBehaviourArray[ticksDone].currentTick!=context->tickBehaviourArray[ticksDone].lifeTicks)
                ++runningThreadCount;

        // Print the current system state.
        // Banker_displayBanker(context->bankerData);
        printRunScreen(context,10,1);
        getch();

        // Check for deadlock, i.e no threads have valid lastReqResponse.
        // Reuse newCount and ticksDone variables...
        newCount = 1;
        for(ticksDone=0;ticksDone<context->threadCount;++ticksDone)
        {
            // If any running thread ticked without errors,i.e has status code 1, there is no deadlock...
            if(context->tickBehaviourArray[ticksDone].currentTick!= context->tickBehaviourArray[ticksDone].lifeTicks && (context->tickBehaviourArray[ticksDone].lastReqResponse == 1||context->tickBehaviourArray[ticksDone].lastReqResponse == -5))
            {
                newCount = 0;
                break;
            }
        }
        // Print errors on deadlock and break
        if(newCount)
        {
            ++noUpdateIteration;
            if(noUpdateIteration>1){
                printf("Oh no...! No thread proceded for two ticks, deadlock...!\n");
                break;
            }
        }
        else noUpdateIteration = 0;
    }
}
void initContext(struct systemContext* context)
{
    struct threadTickBehaviour tb[5];
    sem_t twaiter[5],onTickSem;
    int i;
    sem_init(&onTickSem, 0, 0);
    for(i=0;i<5;++i){
        sem_init(&twaiter[i], 0, 0);
        initTickBehaviour(&(tb[i]), 5, (int*[]){(int[]){2,10},(int[]){0,1},(int[]){2,10},(int[]){2,10},(int[]){2,10}}, &twaiter[i], &onTickSem, i, context->bankerData);
    }

    pthread_t userThreads[5];
    for(i=0;i<4;++i)pthread_create(&userThreads[i], NULL, threadFunction, &(tb[i]));

    struct UITextField tfarray[5*7];
    context->textFieldsArray = tfarray;

    context->tickBehaviourArray = tb;
    context->threadCount = i;
    context->onTickSem = &onTickSem;
    run(context);
}

int main()
{
    printf("Welcome\nThis is the banker's algorithm implementation.\n");
    struct BankerData bd;
    int processCount = 4, resourceCount = 3;
    
    int initcode = Banker_init(&bd,resourceCount,processCount,
        //MaxResourceArray
        (int[]){30, 3, 50},
        //resourcesDemandMatrix
        (int*[]){
            (int[]){1,1,40},
            (int[]){3,3,40},
            (int[]){1,1,40},
            (int[]){3,3,40}
            },
        //resourcesAllocatedMatrix
        (int*[]){
            (int[]){0,0,0},
            (int[]){0,0,0},
            (int[]){0,0,0},
            (int[]){0,0,0}
            }
        );
    printf("init code = %d\n",initcode);
    Banker_displayBanker(&bd);
    printf("Safe status: %d\n", Banker_requestResource(&bd, 0, 1, 1));

    struct UIframe f;
    int i,maxResourceVal;
    for(i=0;i<resourceCount;++i)
        maxResourceVal = (maxResourceVal>bd.maxResourcesArray[i])? bd.maxResourcesArray[i]:maxResourceVal;
    int requiredWidth = 90+2
                        +digitCount(processCount)
                        +(digitCount(maxResourceVal)*resourceCount)
                        +digitCount(resourceCount)+maxResourceVal
                        +(digitCount(10)*2)
                        +23;
    UIframe_init(&f,requiredWidth,20,'|','|','-','-', 1, 1);

    struct systemContext context;
    context.bankerData = &bd;
    context.currentFrame = &f;
    initContext(&context);

    struct UITextField tary[2];
    UIframe_addLineAndBindTextFeilds(&f,"hio |%f5|%f12| yoyo",0,0,tary);
    UIframe_print(&f,0,0);
    UItextField_setText(&tary[0],"%s","yelloww");
    UItextField_setText(&tary[1],"%s","yelloww");
    UIframe_print(&f,0,0);
    printf("flen=%d",tary[1].length);
}