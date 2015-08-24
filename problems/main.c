#include <malloc.h>
#include <stdio.h>
//#include <ucontext.h>
//#include "mythread.h"
#include "que.h"

int main()
{
	struct mythread_q queue, *q_ptr=NULL, *present=NULL;
	struct mythread arr[4], *temp_thread;
	int i;
	
	printf ("here1\n");
	
	for(i = 0; i < 4; i++)
	{
		printf ("here2\n");
		arr[i].state = i + 3;
		printf ("here2.5: arr[%d].state = %d\n", i, arr[i].state);
		mythread_push_front (&(q_ptr), &(arr[i]));
	} 
	printf ("here3\n");
	present = q_ptr;
	for (i = 0; i < 4; i++)
	{
		printf("%d: %d\n", i, ((struct mythread *)(present->my_thread))->state);
		present = present->next;
	}
// 	printf ("here4\n");
 	mythread_pop(&(q_ptr), &(arr[2]));
	
	present = q_ptr;
	for (i = 0; i < 4; i++)
	{
		if (present->my_thread == NULL)
			exit(0);
		printf("%d: %d\n", i, ((struct mythread *)(present->my_thread))->state);
		if (present->next == NULL)
			exit(0);
		present = present->next;
	}
	
	temp_thread = mythread_popfront(&(q_ptr));
	printf ("popped from front: temp_thread->state: %d\n", temp_thread->state);
	
	present = q_ptr;
	for (i = 0; i < 4; i++)
	{
		if (present->my_thread == NULL)
			exit(0);
		printf("%d: %d\n", i, ((struct mythread *)(present->my_thread))->state);
		if (present->next == NULL)
			exit(0);
		present = present->next;
	}
	
	
	return (1);
}
	
