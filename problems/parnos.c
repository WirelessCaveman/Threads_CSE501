#include <stdio.h>
//#include <conio.h>

int func(int start, int end)	//non zero returns are errors
{
	int icount;
	
	if (start > end)
	{
		printf ("error: start greater than end\n");
		return (1);	
	}
	if (start < 0)
	{
		printf ("error: only positive numbers\n");
		return (2);	
	}
	if (end > 99)
	{
		printf ("error: only two digits accepted\n");
		return (3);
	}
		
	
// 	for (icount = start; icount <= end; icount++)
// 	{
// 		if (icount < 10)
// 		{
// 			printf("0%d\t", icount);
// 		}
// 		else printf ("%d\t", icount);
// 	}

	for (icount = start; icount <= end; icount++)
	{
		printf("%d", icount/10);
	}
	printf ("\n");
	for (icount = start; icount <= end; icount++)
	{
		printf ("%d", icount%10);
	}
	printf ("\n");
	
	return (0);
}

int main(int argc, char *argv[])
{
	//clrscr();
	if (argc != 3)
	{
		printf("Error: incorrect inputs\n");
		return (1);
	}
	
	func (atoi(argv[1]), atoi(argv[2]));
	
}	



