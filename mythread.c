
#include "mythread.h"
#include "que.h"
#include <stdio.h>
#include <malloc.h>
#include <ucontext.h>

#define STACKSIZE 8192

// #define create_test
// #define yield_test
// #define join_test
// #define joinall_test
// #define exit_test
// #define run_test
// #define sem_init
// #define sem_wait
// #define sem_sig

static int r_thrd_cnt = 0, b_thrd_cnt = 0;
static long int thread_count = 0, alive_thread_count = 0;
static int rt_first = 1, bt_first = 1;
struct mythread_q *rdy_q = NULL, *blk_q = NULL;
struct mythread *threads = NULL, *prev_thrd_ptr = NULL;
static ucontext_t ctx, ctxt/*, prev_ctx = NULL*/;

typedef struct sem_str
{
	int initial;
	int count;
	int valid;
	struct mythread_q *sem_q;
} sem_str;

MySemaphore MySemaphoreInit(int init_val)
{
	sem_str *tempsem_ptr;
	tempsem_ptr = (sem_str *)malloc(sizeof(struct sem_str));
	if(tempsem_ptr == NULL)
	{
		printf("malloc error- space not allocated to semaphore struct\n");
	  exit(-1);
	}
	tempsem_ptr->initial = init_val;
	tempsem_ptr->count = init_val;
	tempsem_ptr->sem_q = NULL;
	tempsem_ptr->valid = 1;
	return tempsem_ptr;
}	

void MySemaphoreWait(MySemaphore sem)
{
	sem_str *tempsem_ptr;
	struct mythread *cur_thread, *tempthrd_ptr;
	
	tempsem_ptr = (sem_str *)sem;
	cur_thread = rdy_q->my_thread; 
	
	if(!sem) return;
	
	if((tempsem_ptr == NULL) || (tempsem_ptr->valid != 1))
	{
    printf("Something wrong with semaphore structure passed\n");
    return;
	}
	
	if(tempsem_ptr->count <= 0)
  {
	  tempthrd_ptr = mythread_popfront(&(rdy_q));
	  mythread_push_back(&(tempsem_ptr->sem_q), tempthrd_ptr);
	  
		if(rdy_q == NULL)
  	{
    	printf("last thread. exit \n");
    	setcontext(&ctxt);
  	}
		else
	   swapcontext(&(cur_thread->context), &(rdy_q->my_thread->context));   // change context to head of run queue
	}
	
	if (tempsem_ptr->count > 0)
	{
		(tempsem_ptr->count)--;
	}
}

void MySemaphoreSignal(MySemaphore sem)
{
	sem_str *tempsem_ptr;
	struct mythread *cur_thread, *temp_thread;
	
	tempsem_ptr = (sem_str *)sem;
	
	if((tempsem_ptr == NULL) || (tempsem_ptr->valid != 1))
	{
	  printf(" Something wrong with this semaphore \n ");
	  return ;
	}
	
	if ((tempsem_ptr->count) < (tempsem_ptr->initial))
	{
		(tempsem_ptr->count)++;
	}
	
	//printf(" in signal semaphore value after adding - %d \n ", sem->init_value); 
	
  if(tempsem_ptr->count >= 0 && tempsem_ptr->sem_q != NULL)	//on releasing a semaphore, if the count is 0+, make a waiting thread grab the semaphore
  {
    if (tempsem_ptr->count > 0)	//count should not go below 0; but still on a signal even if count is 0, one thread should be released
    {
	   	(tempsem_ptr->count)--;	//waiting thread grabs semaphore- so count is decremented
    }
	  temp_thread = (struct mythread *)mythread_popfront(&(tempsem_ptr->sem_q));     // get the thread from the front of the semaphore que
	  mythread_push_back(&(rdy_q), temp_thread);	//put the thread obtained from the sem_q above into the back of the rdy_q
  }

}

void MySemaphoreDestroy(MySemaphore sem)
{
	sem_str *tempsem_ptr;
	struct mythread *temp_thread_ptr;
	struct mythread_q *temp_q_ptr;
	
	tempsem_ptr = (sem_str *)sem;
	temp_q_ptr = tempsem_ptr->sem_q;
	
	while (temp_q_ptr != NULL)
	{
		temp_thread_ptr = mythread_popfront(&(temp_q_ptr));
		printf("Forcefully putting thread %d at the back of ready queue since semaphore is being destroyed\n- this is how I have interpreted the destroy function, please let me know if this needs to be changed\n", temp_thread_ptr->my_id);
		mythread_push_back(&(rdy_q), temp_thread_ptr);
	}
	
	free(tempsem_ptr);
	
}

void MyThreadInit(void(*start_funct)(void *), void *args)
{
	MyThreadCreate(start_funct, args);
}

MyThread MyThreadCreate(void (*thread_function)(void *), void *arg)
{
		
	struct mythread *new_thread;
	
	new_thread = (struct mythread *)calloc(1, sizeof(struct mythread));
	if (new_thread == NULL)
	{
		printf("Error while callocing thread (MyThreadCreate)\n");
		exit (-1);
	}
	
	mythread_push_back (&rdy_q, new_thread);	//put child at the end of the ready que
	if (thread_count > 0)
	{
		mythread_push_back (&(rdy_q->my_thread->children), new_thread);	//put new_thread in the children que of present thread
		(rdy_q->my_thread->child_count)++;
	}
	
	new_thread->state = 0;                
  if (thread_count == 0 || rdy_q->my_thread == NULL)
  {
	  new_thread->parent = NULL;
  }
  else
  {
	  new_thread->parent = rdy_q->my_thread;    //how to do this???           
  }
  
  getcontext(&(new_thread->context));
	new_thread->context.uc_stack.ss_sp = (void *) malloc(STACKSIZE);
  new_thread->context.uc_stack.ss_size = STACKSIZE;
  new_thread->context.uc_link = 0;
  
  new_thread->my_id = thread_count;   
  new_thread->children = NULL;             
  new_thread->child_count = 0;             
  new_thread->blk_type = 0;             
  new_thread->thread_fxn = thread_function;
  new_thread->args = arg;  
  new_thread->wait_forq = NULL;             
  new_thread->waitq = NULL;  
  new_thread->wait_for_count = 0;
  
	if (new_thread->context.uc_stack.ss_sp == NULL) 
	{
		printf("malloc error\n");
		exit(-1);
	}
	makecontext(&(new_thread->context), new_thread->thread_fxn, 1, new_thread->args);
	
  thread_count++;
  alive_thread_count++;
  r_thrd_cnt++;
  
  			#ifdef create_test
  				printf("create_test: new_thread->my_id: %d\n", new_thread->my_id);
  				printf("rdy_q:");
  				print_q(rdy_q);
  				printf("present_q #%d's ->children:", rdy_q->my_thread->my_id);
  				print_q(rdy_q->my_thread->children);
  			#endif
  
  return (new_thread); 
}

void MyThreadYield(void)
{
	struct mythread *temp_thread;
	temp_thread = mythread_popfront(&rdy_q);
	mythread_push_back(&rdy_q, temp_thread);
				#ifdef yield_test
  				printf("yield_test: temp_thread->my_id: %d\n", temp_thread->my_id);
  				printf("rdy_q:");
  				print_q(rdy_q);
  				printf("blk_q:");
  				print_q(blk_q);
  			#endif
	swapcontext(&(temp_thread->context), &(rdy_q->my_thread->context));
		
}

int MyThreadJoin(MyThread thread)
{
	if (thread == NULL) return (0);
	struct mythread *wait_for_ptr = (struct mythread *)thread;
	struct mythread *temp_thread_ptr;
	struct mythread_q *temp_q_ptr;
	int found = 0;
	
	//temp_thread_ptr = rdy_q->my_thread;	//prob not reqd
//	if (wait_for_ptr == NULL) return (-1);
	temp_q_ptr = rdy_q;
				#ifdef join_test
					if (wait_for_ptr->parent != NULL)
					{
						printf("joint_test: wait_for_ptr->parent->my_id = %d; rdy_q->my_thread->my_id = %d\n", wait_for_ptr->parent->my_id, rdy_q->my_thread->my_id);
					}
				#endif
	if (wait_for_ptr->parent != rdy_q->my_thread)
	{
				#ifdef join_test
					printf("not my child- doing nothing\n");
					printf("rdy_q:");
					print_q(rdy_q);
					printf("blk_q:");
					print_q(blk_q);
					printf("present_q #%d's ->children:", rdy_q->my_thread->my_id);
  				print_q(rdy_q->my_thread->children);
				#endif
		return (-1);
	}
	
	found = 0;
	while (found == 0 && temp_q_ptr != NULL)
	{
		if (temp_q_ptr->my_thread == wait_for_ptr)
		{
			found = 1;
			break;
		}
		else temp_q_ptr = temp_q_ptr->next;
		if (temp_q_ptr == rdy_q)
			break;
	}
	if (found == 0)
	{
		//temp_thread_ptr = blk_q->my_thread;	//prob not reqd
		temp_q_ptr = blk_q;
		while (found == 0 && temp_q_ptr != NULL)
		{
			if (temp_q_ptr->my_thread == wait_for_ptr)
			{
				found = 1;
				break;
			}
			else temp_q_ptr = temp_q_ptr->next;
			if (temp_q_ptr == blk_q)
				break;
		}
	}
	if (found == 0)
	{
				#ifdef join_test
  				printf("join_test: found = 0: wait_for_ptr->my_id: %d\n", wait_for_ptr->my_id);
  			#endif
		return (0);
	}
	else 
	{
		mythread_push_back(&(wait_for_ptr->waitq), rdy_q->my_thread);	//put present thread at the back of the waiting que in the blocking thread
		temp_thread_ptr = mythread_popfront (&(rdy_q));	//remove present thread from front of running que
		mythread_push_back(&(blk_q), temp_thread_ptr);	//put present thread to back of blocked que
		//mythread_push_back(&(temp_thread_ptr->wait_forq), wait_for_ptr);	//put the blocking thread at the end of the wait_forq of the present thread
		(temp_thread_ptr->wait_for_count)++;	//increment the number of threads the present thread is waiting for
				#ifdef join_test
  				printf("join_test: found = 1: wait_for_ptr->my_id: %d\n", wait_for_ptr->my_id);
  			#endif
		if (rdy_q != NULL)
		 swapcontext(&(temp_thread_ptr->context), &(rdy_q->my_thread->context));	//take context from outgoing thread and give to thread in front of ready que
	}
	
	if (rdy_q->my_thread == NULL)
	{
				#ifdef join_test
  				printf("join_test: deadlock: wait_for_ptr->my_id: %d\n", wait_for_ptr->my_id);
  			#endif
		//DEADLOCK HANDLING
	}
	
				#ifdef join_test
  				printf("join_test: wait_for_ptr->my_id: %d\n", wait_for_ptr->my_id);
  			#endif
	return (0);
}

void MyThreadJoinAll(void)
{
	struct mythread *wait_for_ptr, *myselfthrd_ptr;
	struct mythread_q *temp_q_ptr;
	struct mythread_q *child_q_ptr;
	int found = 0, found_any = 0;
		
	myselfthrd_ptr = rdy_q->my_thread;
	child_q_ptr = myselfthrd_ptr->children;	//points to the node in the que of children
	
				#ifdef joinall_test
					printf("rdy_q:");
  				print_q(rdy_q);
  				printf("blk_q:");
  				print_q(blk_q);
  				printf("present_q #%d's ->children:", rdy_q->my_thread->my_id);
  				print_q(rdy_q->my_thread->children);
  			#endif
	
	while (child_q_ptr != NULL)
	{
		found = 0;
		temp_q_ptr = rdy_q;	//points to the node in the rdy_q
		while (found == 0 && temp_q_ptr != NULL)
		{
			if (temp_q_ptr->my_thread == child_q_ptr->my_thread)
			{
				found = 1;
				found_any = 1;
				break;
			}
			else temp_q_ptr = temp_q_ptr->next;
			if (temp_q_ptr == rdy_q)
				break;
		}
		if (found == 0)
		{
			//temp_thread_ptr = blk_q->my_thread;	//prob not reqd
			temp_q_ptr = blk_q;
			while (found == 0 && temp_q_ptr != NULL)
			{
				if (temp_q_ptr->my_thread == child_q_ptr->my_thread)
				{
					found = 1;
					found_any = 1;
					break;
				}
				else temp_q_ptr = temp_q_ptr->next;
				if (temp_q_ptr == blk_q)
					break;
			}
		}
		
		if (found == 0)
		{
			printf("Something wrong- child dead, but parent doesn't know\n");
			exit(-1);
		}
		
		if(found == 1)	//active child found- its node in the rdy/blkd qis pointed to by temp_q_ptr
		{		
			wait_for_ptr = child_q_ptr->my_thread;
			//mythread_push_back(&(myselfthrd_ptr->wait_forq), wait_for_ptr);	//make myself wait for child
			(myselfthrd_ptr->wait_for_count)++;	//increment the number of threads the present thread is waiting for
			mythread_push_back(&(wait_for_ptr->waitq), myselfthrd_ptr); //let child know i'm waiting for him
				
				#ifdef joinall_test
  				printf("joinall_test: myselfthrd_ptr->my_id: %d has to wait for child child_q_ptr->my_thread->my_id: %d\n", myselfthrd_ptr->my_id, child_q_ptr->my_thread->my_id);
  			#endif
		}
		child_q_ptr = child_q_ptr->next;	//point to the next child of the present thread
		if (child_q_ptr == myselfthrd_ptr->children)	//end of children thread
		{
			break;
		}
	}	//while ends- done with notifying all my children that i'm waiting for them --and-- setting the number of children i have to wait for
	
	if (found_any == 1)	//atleast one active child thread has been found
	{
		myselfthrd_ptr = mythread_popfront(&(rdy_q));	//remove present thread from front of running que
		mythread_push_back(&(blk_q), myselfthrd_ptr);	//put present thread to back of blocked que
		//mythread_push_back(&(temp_thread_ptr->wait_forq), wait_for_ptr);	//put the blocking thread at the end of the wait_forq of the present thread
				#ifdef joinall_test
					printf("joinall_test: mysetlfthrd_ptr->my_id: %d goes to the back of the blk_q\n", myselfthrd_ptr->my_id);
				#endif
		if (rdy_q != NULL)
			swapcontext(&(myselfthrd_ptr->context), &(rdy_q->my_thread->context));	//take context from outgoing thread and give to thread in front of ready que
	}
	
	
	if (rdy_q->my_thread == NULL)
	{
		printf("deadlock\n");
		//DEADLOCK HANDLING
	}
	
//	return (1);
	
}
		
void MyThreadExit(void)
{
	struct mythread *myselfthrd_ptr, *myparent_ptr, *waitingforhim_ptr;
	struct mythread_q *temp_q_ptr, *myparentschildren_q_ptr, *waitingforme_q_ptr, *mychildren_q_ptr;
	
	if (prev_thrd_ptr != NULL)
	{
				#ifdef exit_test
					printf ("freeing memory\n");
				#endif
		free(prev_thrd_ptr->context.uc_stack.ss_sp);
		free(prev_thrd_ptr);
	}
	
	myselfthrd_ptr = rdy_q->my_thread;
	myparent_ptr = myselfthrd_ptr->parent;
	if (myparent_ptr != NULL)
	{
		myparentschildren_q_ptr = myparent_ptr->children;
		if (myparentschildren_q_ptr == NULL)
		{
			printf("you shouldn't be here- you are alive, but your parent doesn't know\n");
			exit (-1);
		}
		
				#ifdef exit_test
					printf("exit_test: I'm q #%d\n", myselfthrd_ptr->my_id);
					printf("rdy_q:");
  				print_q(rdy_q);
  				printf("blk_q:");
  				print_q(blk_q);
  				printf("q #%d's ->children:", myparent_ptr->my_id);
  				print_q(myparent_ptr->children);
  			#endif

		//go to parent remove myself from its children que and decrement child count
		(myparent_ptr->child_count)--;
		if (mythread_pop(&(myparent_ptr->children), myselfthrd_ptr) == -1)
		{
			printf("you shouldn't be here- you are alive, but your parent doesn't know\n");
		}	
				#ifdef exit_test
					printf("after removing myself q #%d's ->children:", myparent_ptr->my_id);
  				print_q(myparent_ptr->children);
  			#endif
	}
	//go to all blocked on me and decrement blocked count- if this reaches 0 take them out of blocked and put them in ready
	waitingforme_q_ptr = myselfthrd_ptr->waitq;
	while (waitingforme_q_ptr != NULL)
	{
		waitingforhim_ptr = waitingforme_q_ptr->my_thread;
		(waitingforhim_ptr->wait_for_count)--;
		if (waitingforhim_ptr->wait_for_count <= 0)
		{
			//temp = waitingforhim_ptr;
// 			if (mythread_pop(&(waitingforme_q_ptr), waitingforhim_ptr) == -1)	//get the guy who is waiting out of my waiting que- i don't really care though- i'm exiting anyway
// 				printf ("error while popping from waitingforme_q\n");
			if (mythread_pop(&(blk_q), waitingforhim_ptr) == -1)	//get the waiting guy out of the blk_q
			{
				printf ("error while popping from blok_q\n");
				exit (-1);
			}
			mythread_push_back(&(rdy_q), waitingforhim_ptr);	//and put him in the end of the rdy_q
		}
		waitingforme_q_ptr = waitingforme_q_ptr->next;
		if (waitingforme_q_ptr == myselfthrd_ptr->waitq)	//reached end of que
			break;
	}
	
	//set all my childrens parent pointers to NULL
	temp_q_ptr = myselfthrd_ptr->children;	//same as below
	mychildren_q_ptr = rdy_q->my_thread->children;	
	do
	{
		if (temp_q_ptr == NULL)
			break;	
		mychildren_q_ptr->my_thread->parent = NULL;
		mychildren_q_ptr = mychildren_q_ptr->next;
	}while (mychildren_q_ptr != temp_q_ptr);
	
	//swap context to the next thread in que
	myselfthrd_ptr = mythread_popfront (&(rdy_q));	//remove present thread from front of running que
	
	prev_thrd_ptr = myselfthrd_ptr;
//	prev_ctx = myselfthrd_ptr->context.uc_stack.ss_sp;
	
	if (rdy_q != NULL)
		swapcontext(&(myselfthrd_ptr->context), &(rdy_q->my_thread->context));	//take context from outgoing thread and give to thread in front of ready que
	else //rdy_q is now empty- do something
	{
				#ifdef exit_test
					printf("ready que is now empty. total threads created: %d\n", thread_count);
				#endif
		setcontext(&ctxt);
	}
}
			
void MyThreadRun(void)
{
	char *charptr = (char *)malloc(81920);
	struct mythread *rdy_q_head_thrd = rdy_q->my_thread;	
	
	if (rdy_q == NULL)
	{
		printf("rdy_q is empty\n");
		exit(-1);
	}
	rdy_q_head_thrd = rdy_q->my_thread;	
	ctxt.uc_stack.ss_sp = charptr;
	if (ctxt.uc_stack.ss_sp == NULL) 
	{
    printf("malloc error\n"); 
    exit(-1);
  }
  ctxt.uc_stack.ss_size = STACKSIZE;

	//first_th = &rdy_q_head_thrd->context;
	swapcontext(&ctxt, &(rdy_q_head_thrd->context));
}


	
	
