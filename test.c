/* Andre Augusto Giannotti Scota (https://sites.google.com/view/a2gs/) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btavl.h"

typedef struct _sample_t{
	int x;
	char y[10];
}sample_t;

/*
 * a < b  -> -1
 * 1 = b  -> 0
 * a > b  -> 1
 */
int dataSampleCompare(void *a, void *b)
{
	if     (((sample_t *)a)->x > ((sample_t *)b)->x) return(-1);
	else if(((sample_t *)a)->x < ((sample_t *)b)->x) return(1);
	return(0);
}

int main(int argc, char *argv[])
{
	btavl_t btCtx;

	sample_t data1  = {15, "666"}; sample_t data2  = {27, "333"}; sample_t data3 = {49, "222"};
	sample_t data4  = {10, "888"}; sample_t data5  = { 8, "111"}; sample_t data6 = {67, "777"};
	sample_t data7  = {59, "444"}; sample_t data8  = { 9, "555"}; sample_t data9 = {13, "999"};
	sample_t data10 = {20, "444"}; sample_t data11 = {14, "555"};
	sample_t *dataSearch;

	btavlInit(&btCtx, dataSampleCompare, malloc, free);

	btavlInsert(&btCtx, &data1 , NULL); btavlInsert(&btCtx, &data2 , NULL); btavlInsert(&btCtx, &data3, NULL);
	btavlInsert(&btCtx, &data4 , NULL); btavlInsert(&btCtx, &data5 , NULL); btavlInsert(&btCtx, &data6, NULL);
	btavlInsert(&btCtx, &data7 , NULL); btavlInsert(&btCtx, &data8 , NULL); btavlInsert(&btCtx, &data9, NULL);
	btavlInsert(&btCtx, &data10, NULL); btavlInsert(&btCtx, &data11, NULL);

	btavlDelete(&btCtx, &data4, NULL, BTAVL_FALSE);
	btavlDelete(&btCtx, &data9, NULL, BTAVL_FALSE);
	btavlDelete(&btCtx, &data1, NULL, BTAVL_FALSE);

	dataSearch = btavlSearch(&btCtx, &data6, NULL);

	if(dataSearch != NULL)
		printf("Search: [%s]\n", dataSearch->y);

#ifdef BTAVL_TRANSVERSAL
#endif

	return(0);
}
