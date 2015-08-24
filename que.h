
#include <stdio.h>
//#include <malloc.h>
#include <ucontext.h>

// #define init
// #define push_back
// #define push_front

struct mythread_q
{
  struct mythread *my_thread;
  struct mythread_q *prev, *next;
};

struct mythread //tcb
{         
	int state;                      
	struct mythread *parent;
	long int my_id;
	struct mythread_q *children;
	int child_count;
	int blk_type;
	void (*thread_fxn)(void*);    
	void *args;                   
	struct mythread_q *waitq; //who all are waiting for me
	struct mythread_q *wait_forq;	//who am i waiting for - defunct
	int wait_for_count;	//how many people am i waiting for
	ucontext_t context;
};

void mythread_q_init(struct mythread_q **head_ptr, struct mythread *my_thread)
{
	struct mythread_q *temp_node=NULL, *head=NULL;
	
									#ifdef init
										printf("init: here1: (*my_thread).state = %d\n", (*my_thread).state);
									#endif
	
	temp_node = (struct mythread_q *)malloc(sizeof(struct mythread_q));
	if (temp_node == NULL) 
	{
		printf("Could not allocate thread space\n");
		exit(1);
	}

	temp_node->my_thread = my_thread;
	temp_node->prev = temp_node;
	temp_node->next = temp_node;
									#ifdef init
										printf("init: here2\n");
									#endif
	*head_ptr = temp_node;
}

void mythread_push_back (struct mythread_q **head_ptr, struct mythread *my_thread)
{
	struct mythread_q *temp_node=NULL, *head=NULL;
	
	if (*head_ptr == NULL) 
	{
		mythread_q_init(head_ptr, my_thread);
		return;
	}
									#ifdef push_back
										printf("push_back: here1- (*my_thread).state = %d\n", (*my_thread).state);
									#endif	
	head = *head_ptr;
	
	temp_node = (struct mythread_q *)malloc(sizeof(struct mythread_q));
									#ifdef push_back
										printf("push_back: here2\n");
									#endif
	if (temp_node == NULL) 
	{
		printf("Error allocating memory for push\n");
		exit(1);
	}
									#ifdef push_back
										printf("push_back: here3\n");
									#endif	
	temp_node->my_thread = my_thread;
									#ifdef push_back
										printf("push_back: here3.1: (*my_thread).state = %d \n", (*my_thread).state);
									#endif	
	temp_node->prev = head->prev;
									#ifdef push_back1
										printf("push_back: here3.2: ((head->prev->my_thread).state) = %d\n", (head->prev->my_thread).state);
									#endif	
	temp_node->next = head;
									#ifdef push_back1
										printf("push_back: here3.3: ((head->next->my_thread).state) = %d\n", (head->next->my_thread).state);
									#endif	
	temp_node->prev->next = temp_node;
									#ifdef push_back
										printf("push_back: here3.4\n");
									#endif	
	head->prev = temp_node;
// 									#ifdef push_back
// 										printf("push_back: here4\n");
// 									#endif	
}

void mythread_push_front (struct mythread_q **head_ptr, struct mythread *my_thread)
{
	struct mythread_q *temp_node=NULL, *head=NULL;
									#ifdef push_front
										printf("push_front here1: (*my_thread).state = %d\n", (*my_thread).state);
									#endif
	
	if (*head_ptr == NULL) 
	{
		mythread_q_init(head_ptr, my_thread);
		return;
	}
									#ifdef push_front
										printf("push_front: here2\n");
									#endif

	head = *head_ptr;
//	mythread_push_back(&head->next, my_thread);
	mythread_push_back(head_ptr, my_thread);
									#ifdef push_front
										printf("push_front: here3\n");
									#endif

	*head_ptr = head->prev;
}

int mythread_pop (struct mythread_q **head_ptr, struct mythread *my_thread)
{
	struct mythread_q *temp_node=NULL, *head=NULL, *thread_iter=NULL;
	
	head = *head_ptr;
	thread_iter = *head_ptr;
	
	if (thread_iter == NULL)
		return (-1);
	if (thread_iter->my_thread == NULL)
		return (-1);
		
	do 
	{
		if (thread_iter->my_thread == my_thread) 
		{
			temp_node = thread_iter;
			temp_node->next->prev = thread_iter->prev;
			temp_node->prev->next = thread_iter->next;
			if (temp_node == head) 
			{
				if (temp_node->next == temp_node)	//last node left
					*head_ptr = NULL;
				else
					*head_ptr = temp_node->next;
			}
			return (1);
		}
		thread_iter = thread_iter->next;
		if (thread_iter == head)	//didn't find the required thread
		{
			return (-1);
		}
	} while (thread_iter != head);
	return (1);
}

struct mythread * mythread_popfront (struct mythread_q **head_ptr)
{
	struct mythread_q *temp_node_ptr=NULL;
	
	temp_node_ptr = *head_ptr;
	
	if (temp_node_ptr->next == temp_node_ptr)
	{
		*head_ptr = NULL;
	}
	else
	{
		(*head_ptr)->prev->next = temp_node_ptr->next;
		(*head_ptr)->next->prev = temp_node_ptr->prev;
		*head_ptr = temp_node_ptr->next;
	}
	return (temp_node_ptr->my_thread);
}

void print_q(struct mythread_q *q_ptr)
{
	struct mythread_q *start = q_ptr;
	struct mythread_q *temp = q_ptr;
	if (!q_ptr) 
	{
		printf("Empty Queue\n");
		return;
	}
	printf("Not an empty queue\n");
	do
	{
		/*if (temp == NULL || temp->my_thread == NULL) 
		{
			printf("\n");
			return;
		}*/
		//else
		//{
			printf("  %d", temp->my_thread->my_id);
		//}
		temp = temp->next;
	} while (temp != start);
	printf("\n");
}
		
	



