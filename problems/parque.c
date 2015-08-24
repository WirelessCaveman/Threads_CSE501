#include <stdio.h>

//#define push_back

struct que
{
  struct node *data;
  struct que *prev, *next;
};

struct node
{
	int data;
};

//static int testing; //static variables can be declared without any problems


void que_init(struct que **head_ptr, struct node *node)
{
	struct que *temp_node=NULL, *head=NULL;
	//static int testing;
	
	//printf("testing init: %d\n", testing);
	
	temp_node = (struct que *)malloc(sizeof(struct que));
	if (temp_node == NULL) 
	{
		printf("Could not allocate space\n");
		exit(1);
	}

	temp_node->data = node;
	temp_node->prev = temp_node;
	temp_node->next = temp_node;
	*head_ptr = temp_node;
}


void que_push_back (struct que **head_ptr, int node_data)
{
	struct que *temp_node=NULL, *head=NULL;
	struct node *node;
	//static int testing;
	
	//printf("testing push: %d\n", testing);
	
	node = (struct node*)malloc(sizeof(struct node));
	node->data = node_data;
	
	if (*head_ptr == NULL) 
	{
		que_init(head_ptr, node);
		return;
	}
									
	head = *head_ptr;
	
	temp_node = (struct que *)malloc(sizeof(struct que));
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
	temp_node->data = node;
									
	temp_node->prev = head->prev;
									
	temp_node->next = head;
	temp_node->prev->next = temp_node;
	head->prev = temp_node;

}


int main(void)
{
	int x = 1, y = 2, z = 3;	//give three values to be pushed back here
	struct que *my_que_ptr = NULL;
	struct que *head;
	
	//printf("here\n");
	
	que_push_back (&(my_que_ptr), x);
	head = my_que_ptr;	//should have a head pointer...
	printf("first push: %d\n", head->data->data);
	que_push_back (&(head), y);
	que_push_back (&(head), z);
	my_que_ptr = my_que_ptr->next;
	printf("second push: %d\n", my_que_ptr->data->data);
	my_que_ptr = my_que_ptr->next;
	printf("third push: %d\n", my_que_ptr->data->data);
}




