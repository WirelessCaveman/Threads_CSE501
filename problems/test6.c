 /**********************************************************
* File Name........: test.c
* Author...........: Terrence J. OConnor
* Class............: CSC501, NC State University
* References.......: None.
*
* Description......: test program for threads library
*
* To execute: ./test
*
************************************************************/

#include "mythread.h"
#include <stdio.h>
#include <stdlib.h>

// Test Cases for Recusion & Producer Consumer
// Please run on your home machine if you are attempting to blow stack
int rcount = 100; // recusion count -- feel free to change
int pcount = 100; // producer & consumer count - feel free to change

// A simple test function without an argument
void printv(void)
{
int i;
printf("2. Test with Void Arg (NULL = NULL)\n");
MyThreadExit();
}

//A simple test function with an argument
void printa(int n)
{
printf("3. Test Passing Integer (2 = %i)\n",n);
MyThreadExit();
}

//A simple function that is created and then joined
void printj(void)
{
printf("... Joinall Test\n");
MyThreadYield();
MyThreadExit();
}

//A simple semaphore test that makes the child wait
//on his/her parent's signal
void semtest(MySemaphore sem)
{
printf("... Child waits on semaphore with value 0.\n");
MySemaphoreWait(sem);
printf("... Child can now go forward. \n");
MyThreadExit();
}

//A simple semaphore test that makes the parent wait
//on his/her child's signal
void semtest2(MySemaphore sem)
{
printf("...Child signals sempahore.\n");
MySemaphoreSignal(sem);
MyThreadExit();
}

//A recusion test that spawns up to "rcount" threads
void recursion(int i) {
int j;
j=i+1;
if (i < rcount) {
MyThreadCreate(recursion,j);
}
MyThreadYield();
MyThreadExit();
}

//A producer program
void producer(MySemaphore sem){
MySemaphore test123;
MySemaphoreSignal(sem);
MyThreadExit();
}

//A consumer program
void consumer(MySemaphore sem){
MySemaphore test123/* = NULL*/;
MySemaphoreWait(sem);
MyThreadExit();
}

//A producer/consumer program
void producer_consumer(void){
MySemaphore sem[pcount];
MyThread producers[pcount];
MyThread consumers[pcount];
int i;

for (i=1;i<pcount;i++){
sem[i]=MySemaphoreInit(0);
consumers[i]=MyThreadCreate(consumer,sem[i]);
producers[i]=MyThreadCreate(producer,sem[i]);

}
MyThreadExit();
}

//The main function.
//Should be pretty simple to understand
void start_func(void *j)
{
MyThread tmp[100];
MySemaphore sem[10];

printf("0. Root thread started.\n");
printf("...<MyThreadCreate>>\n");
tmp[1]=MyThreadCreate(printv,NULL );
printf("1. Created a Test Thread. \n");
printf("...<MyThreadYield>>\n");
MyThreadYield();
tmp[2]=MyThreadCreate(printa,2);
printf("4. Return value from a valid join (0=%i)\n",MyThreadJoin(tmp[2]));
printf("5. Return value from an expired join (-1=%i)\n",MyThreadJoin(tmp[2]));
printf("...<MyThreadJoinAll>>\n");
printf("6. Create three threads & then joinall on them.\n");
tmp[3]=MyThreadCreate(printj,NULL);
tmp[4]=MyThreadCreate(printj,NULL);
tmp[5]=MyThreadCreate(printj,NULL);
MyThreadJoinAll();
printf("7. Returned from Join All. \n");
printf("...<MySemaphoreInit>>\n");
sem[1]=MySemaphoreInit(1);
sem[2]=MySemaphoreInit(2);
sem[3]=MySemaphoreInit(3);
printf("8. Created 3 Semaphores with initialvalues 1,2,3.\n");
printf("...<MySemaphoreWait>>\n");
MySemaphoreWait(sem[1]);
MySemaphoreWait(sem[2]);
MySemaphoreWait(sem[3]);
printf("9. Waited on all three Semaphores should have values 0,1,2.\n");
tmp[6]=MyThreadCreate(semtest,sem[1]);
MyThreadYield();
MySemaphoreSignal(sem[1]);
printf("... Parent Signaled \n");
MyThreadYield();
MySemaphoreWait(sem[2]);
tmp[7]=MyThreadCreate(semtest2,sem[2]);
printf("10. Lets try it in reverse...\n");
printf("...parent waits on a semaphore with value 0.\n");
MySemaphoreWait(sem[2]);
printf("...parent passes through now.\n");
printf("11. Recursively build %i threads ",rcount);
MyThreadCreate(recursion,1);
MyThreadJoinAll();
printf("... success.\n");
rcount=1000;
printf("12. Create a producer/consumer scenario with %i threads",pcount*2);
tmp[8]=MyThreadCreate(producer_consumer,NULL);
MyThreadJoin(tmp[8]);
printf("... success \n");
MyThreadExit();
}

int main()
{
printf("...<MyThreadInit>>\n");
MyThreadInit(start_func,NULL);
printf("0. Root thread initialized.\n");
printf("...<MyThreadRun>>\n");
MyThreadRun();


}
