#include <stdio.h>
#include "mythread.h"

MyThread someThread = NULL;

void donothing(void * n)
{
    printf("Basically doing nothing\n");
    MyThreadExit();
}

void testjoininvalid(void *n)
{
    int x = MyThreadJoin(someThread);
    if(x == -1)
    {
        printf("Invalid join\n");
    }
    //Exit the second thread
    MyThreadExit();
}

void testjoinvalid(void *n)
{
    int x = 10;

    someThread = MyThreadCreate(donothing, (void *)&x);
    //Exit first thread
    MyThreadExit(); 
}

void testjoinmain(void * n)
{
    MyThread t1 = NULL;
    MyThread t2 = NULL;

    //Create the 2 threads
    t1 = MyThreadCreate(testjoinvalid, "t1");
    t2 = MyThreadCreate(testjoininvalid, "t2");
    
    //Join on the first thread
    MyThreadJoinAll();

    //Exit the parent thread
    MyThreadExit();
}

int main(void)
{
    int x = 10;

    MyThreadInit(testjoinmain, (void *)&x);
    MyThreadRun();

    //Done
    printf("Done\n");
}
