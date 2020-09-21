/* Andre Augusto Giannotti Scota (https://sites.google.com/view/a2gs/) */

#ifndef __BTAVL_H__
#define __BTAVL_H__ 1

#include <string.h>

#define BTAVL_OK    (1)
#define BTAVL_ERROR (0)
#define BTAVL_TRUE  (1)
#define BTAVL_FALSE (0)

typedef struct _btavlNode_t{
	void *data;
	unsigned int h;
	struct _btavlNode_t *a, *b, *father;
}btavlNode_t;

typedef struct _btavl_t{
	int   (*defaultCompare)    (void *a, void *b);
	void  (*defaultDeallocator)(void *data);
	void *(*defaultAllocator)  (size_t size);
	btavlNode_t *head;
	unsigned long n;
}btavl_t;

float btavlGetSize(btavl_t *ctx);
void *btavlSearch(btavl_t *ctx, void *data, int (*compare)(void *a, void *b));
int btavlDelete(btavl_t *ctx, void *data, int (*compare)(void *a, void *b), int callDataDealloc);
int btavlInsert(btavl_t *ctx, void *data, int (*compare)(void *a, void *b));
int btavlInit(btavl_t *ctx, int (*compare) (void *a, void *b), void *(*alloc) (size_t size), void (*dealloc) (void *data));

#endif
