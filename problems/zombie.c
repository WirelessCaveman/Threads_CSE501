#include <stdio.h>
#include "mythread.h"

void print(void *item)
{
    printf("\n%s executed", (char *)item);
    MyThreadExit();
}

void createzombie(void *n)
{
    MyThread c1 = NULL, c2 = NULL, c3 = NULL;
    char *c1_str = "Child 1";
    char *c2_str = "Child 2";
    char *c3_str = "Child 3";

    printf("\nParent thread executing");
    
    /* Create the children */
    c1  = MyThreadCreate(print, (void *)c1_str);
    c2  = MyThreadCreate(print, (void *)c2_str);
    c3  = MyThreadCreate(print, (void *)c3_str);

    /* Exit the main parent thread */
    MyThreadExit();
}

void main ()
{
    int n = 10;
    
    MyThreadInit(createzombie, (void *)n);
    MyThreadRun();

    printf("\n Program complete !!!! \n");
}
