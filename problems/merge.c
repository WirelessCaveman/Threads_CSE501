/******************************************************************************
 *
 *  File Name........: merge.c
 *
 *  Description......:
 *
 *  Created by vin on 11/22/06
 *
 *  Revision History.:
 *
 *
 *****************************************************************************/

#include "mythread.c"
int *List;

// this does a merge sort, which is a recursive sort.
// the list is split in half.  each half is recursively split again.
// after spliting the sorted halves are "merged".  we know that the
// smallest value is at the head of each half.  so the smaller of the
// two heads becomes the head of the new list and so on.
// NOTE: the details of the merge sort are not provided.
//

// this unpacks two params from the void pointer that the interface
// requires.
// 

void merge(int start, int end)
{
  if (end - start < 2)
    return;
  else {
    mid = (start+end)/2;	/* split list in half */
    MyThreadCreate(merge_unpack, merge_pack(start, mid)); // sort 1st half
    MyThreadCreate(merge_unpack, merge_pack(mid, end));   // sort 2nd half
    MyThreadJoinAll();
    k = start;
    j = mid;
    // merge the lists
    while (k < j) {
      if (j == end)
	break;
      if (List[k] <= List[j])
	k++;
      else {
	tmp = List[k];
	List[k] = List[j];
	// insert tmp in the proper place
	// assume this can be done...
      }
    }
  }
}

void merge_unpack(void *params)
{
  int start, end;
  sscanf(params, "%d %d", &start, &end);
  merge(start, end);
  free(params);			/* paired w/ malloc in merge_pack */
}

// this packs two params into the void pointer that the interface
// requires
//
void *merge_pack(int start, int end)
{
  char *params;
  params = malloc(8192); /* free in merge_unpack */
  if (params == NULL) exit(-1);	/* not the best feedback */

  sprint(params, "%d %d", start, end);
  return (void *)params;
}

// 


void mergesort(void *n)
{
  merge(0, (int)n);
}

main(int argc, char *argv[])
{
  int n;

  if (argc < 1) {
    printf("usage: %s <n>\n", argv[0]);
    exit(-1);
  }
  n = atoi(argv[1]);
  if (n < 0) {
    printf("invalid value for n (%d)\n", n);
    exit(-1);
  }

  List = malloc(sizeof(int) * n);
  if (List == NULL) return -1;	/* not the best feedback */
  initializeList(n, List);	/* put some random values in the array */

  MyThreadInit(mergesort, (void*)n);
  MyThreadRun();

}

/*........................ end of merge.c ...................................*/
