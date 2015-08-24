#include<stdio.h>
#include "mythread.h"

MySemaphore sem = NULL;

void testfunction(void *id)
{
    printf("Thread %s entering wait\n", (char *)id);	//should print this
    MySemaphoreWait(sem);
    //MySemaphoreSignal(sem);
    printf("Thread %s exiting\n", (char *)id);	//should print this
    MyThreadExit();
}

void exitnormal(void *n)
{
    MyThread t1 = NULL;
    MyThread t2 = NULL;
   	MyThread t3 = NULL; 

    //Create 2 threads
    t1 = MyThreadCreate(testfunction, "t1");
    t2 = MyThreadCreate(testfunction, "t2");
    t3 = MyThreadCreate(testfunction, "t3");
    
    //signal last one
    //MySemaphoreSignal(sem);
    //MySemaphoreDestroy(sem);
    //Exit the parent
    //MyThreadJoinAll();
    MyThreadJoin(t1);
    //MySemaphoreSignal(sem);
    printf ("Parent thread exiting\n");
    MySemaphoreDestroy(sem);
    MyThreadExit();
}

int main(void)
{
    int n = 10;
    
    //Create the semaphore
    sem = MySemaphoreInit(1);

    MyThreadInit(exitnormal, (void *)&n);
    MyThreadRun();

    printf("Out of deadlock\n"); 
    
    return (0);
}
