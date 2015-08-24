#include<stdio.h>
#include "mythread.h"

MySemaphore sem = NULL;

void testfunction(void *id)
{
    printf("Thread %s\n", (char *)id);
    MySemaphoreWait(sem);
    printf("This should not be printed\n");
}

void testdeadlock(void *n)
{
    MyThread t1 = NULL;
    MyThread t2 = NULL;

    //Create 2 threads
    t1 = MyThreadCreate(testfunction, "t1");
    t2 = MyThreadCreate(testfunction, "t2");
    
    //Block on them
    MyThreadJoinAll();
    //Exit the parent
    MyThreadExit();
}

int main(void)
{
    int n = 10;
    
    //Create the semaphore
    sem = MySemaphoreInit(0);

    MyThreadInit(testdeadlock, (void *)&n);
    MyThreadRun();

    printf("Out of deadlock\n"); 
}
