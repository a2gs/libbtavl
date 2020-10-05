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
 * a < b  -> LEFT
 * 1 = b  -> EQUAL
 * a > b  -> RIGHT
 */
btavlComp_t dataSampleCompare(void *a, void *b)
{
	if     (((sample_t *)a)->x < ((sample_t *)b)->x) return(btavlComp_Left);
	else if(((sample_t *)a)->x > ((sample_t *)b)->x) return(btavlComp_Right);
	return(btavlComp_Equal);
}

#ifdef BTAVL_DEBUG
char * printData(void *data)
{
	sample_t *d = (sample_t *)data;
	return(d->y);
}
#endif

int main(int argc, char *argv[])
{
	btavl_t ctx;
#ifdef BTAVL_TRANSVERSAL
	btavlFetch_t fetch;
	sample_t *dataFetch = NULL;
#endif

	sample_t data1  = {15, "aaa"}; sample_t data2  = {27, "bbb"}; sample_t data3 = {49, "ccc"};
	sample_t data4  = {10, "ddd"}; sample_t data5  = { 8, "eee"}; sample_t data6 = {67, "fff"};
	sample_t data7  = {59, "ggg"}; sample_t data8  = { 9, "hhh"}; sample_t data9 = {13, "iii"};
	sample_t data10 = {20, "jjj"}; sample_t data11 = {14, "lll"};
	sample_t *dataSearch;

	btavlInit(&ctx, dataSampleCompare, malloc, free);

	btavlInsert(&ctx, &data1 , NULL); btavlInsert(&ctx, &data2 , NULL); btavlInsert(&ctx, &data3, NULL);
	btavlInsert(&ctx, &data4 , NULL); btavlInsert(&ctx, &data5 , NULL); btavlInsert(&ctx, &data6, NULL);
	btavlInsert(&ctx, &data7 , NULL); btavlInsert(&ctx, &data8 , NULL); btavlInsert(&ctx, &data9, NULL);
	btavlInsert(&ctx, &data10, NULL); btavlInsert(&ctx, &data11, NULL);

#ifdef BTAVL_DEBUG
	btavlStupidDebug(&ctx, printData);
#endif

/*
	btavlDelete(&ctx, &data4, NULL, BTAVL_FALSE);
	btavlDelete(&ctx, &data9, NULL, BTAVL_FALSE);
	btavlDelete(&ctx, &data1, NULL, BTAVL_FALSE);

	dataSearch = btavlSearch(&ctx, &data6, NULL);

	if(dataSearch != NULL)
		printf("Search: [%s]\n", dataSearch->y);
*/

#ifdef BTAVL_TRANSVERSAL
	if(btavlInitTransversal(&ctx, &fetch) != BTAVL_OK){
	}

	for(dataFetch = btavlFetchTransversal(&fetch); dataFetch != NULL; dataFetch = btavlFetchTransversal(&fetch)){
		printf("Key: [%d] | Value: [%s]\n", dataFetch->x, dataFetch->y);
	}
#endif

	return(0);
}
