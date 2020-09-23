/* Andre Augusto Giannotti Scota (https://sites.google.com/view/a2gs/) */
#include <stdio.h>
/*
#include <stdlib.h>
#include <unistd.h>
*/
#include <string.h>
#include <math.h>

#include "btavl.h"

#define BTAVL_SETCOMPARATOR(__btavl_ctx_,                                                                                                 \
                            __btavl_comp_,                                                                                                \
                            __btavl_compParam_) int (*__btavl_comp_)(void *a, void *b);                                                   \
                                                {                                                                                         \
                                                	if(__btavl_ctx_->defaultCompare == NULL) __btavl_comp_ = __btavl_compParam_;           \
                                                	else                                     __btavl_comp_ = __btavl_ctx_->defaultCompare; \
                                                }

#ifdef BTAVL_TRANSVERSAL
#define BTAVL_FILLNODE(__btavl_node_,   \
                       __btavl_a_,      \
                       __btavl_b_,      \
                       __btavl_father_, \
                       __btavl_h,       \
                       __btavl_data_,   \
                       __btavl_prev_,   \
                       __btavl_next_) { \
                                      	__btavl_node_->a = __btavl_a_;           \
                                      	__btavl_node_->b = __btavl_b_;           \
                                      	__btavl_node_->father = __btavl_father_; \
                                      	__btavl_node_->h      = __btavl_h;       \
                                      	__btavl_node_->data   = __btavl_data_;   \
                                      	__btavl_node_->prev   = __btavl_prev_;   \
                                      	__btavl_node_->next   = __btavl_next_;   \
                                      }
#else
#define BTAVL_FILLNODE(__btavl_node_,   \
                       __btavl_a_,      \
                       __btavl_b_,      \
                       __btavl_father_, \
                       __btavl_h,       \
                       __btavl_data_) { \
                                      	__btavl_node_->a = __btavl_a_;           \
                                      	__btavl_node_->b = __btavl_b_;           \
                                      	__btavl_node_->father = __btavl_father_; \
                                      	__btavl_node_->h      = __btavl_h;       \
                                      	__btavl_node_->data   = __btavl_data_;   \
                                      }
#endif

float btavlGetSize(btavl_t *ctx)
{
	return(((log(2.236068 * (ctx->n + 2))) / log(1.618034)) - 2);
}

int btavlIsHead(btavlNode_t *n)
{
	/* or: return(n->h == 0 ? BTAVL_TRUE : BTAVL_FALSE); */
	return(n->father == NULL ? BTAVL_TRUE : BTAVL_FALSE);
}

btavlNode_t ** btavlGetFather(btavl_t *ctx, btavlNode_t *n)
{
	return(btavlIsHead(n) == BTAVL_TRUE ? &(ctx->head) : &(n->father->father));
}

int btavlSLR(btavlNode_t **top, btavlNode_t *a, btavlNode_t *b, btavlNode_t *c) /* Simple Left Rotation */
{
	*top = b;
	b->a = a;
	b->b = c;
	a->a = a->b = c->a = c->b = NULL;

	return(BTAVL_TRUE);
}

int btavlSRR(btavlNode_t **top, btavlNode_t *a, btavlNode_t *b, btavlNode_t *c) /* Simple Right Rotation */
{
	*top = b;
	b->a = c;
	b->b = a;
	a->a = a->b = c->a = c->b = NULL;

	return(BTAVL_TRUE);
}

int btavlDLR(btavlNode_t **top, btavlNode_t *a, btavlNode_t *b, btavlNode_t *c) /* Double Left Rotation */
{
	*top = c;
	c->a = a;
	c->b = b;
	a->a = a->b = b->a = b->b = NULL;

	return(BTAVL_TRUE);
}

int btavlDRR(btavlNode_t **top, btavlNode_t *a, btavlNode_t *b, btavlNode_t *c) /* Double Right Rotation */
{
	*top = c;
	c->a = b;
	c->b = a;
	a->a = a->b = b->a = b->b = NULL;

	return(BTAVL_TRUE);
}

void * btavlSearch(btavl_t *ctx, void *data, int (*compare)(void *a, void *b))
{
	btavlNode_t *walker = NULL;
	int compareResult   = 0;

	BTAVL_SETCOMPARATOR(ctx, comp, compare);

	if(ctx->head == NULL) return(NULL);

	for(walker = ctx->head, compareResult = 0; ; ){
		compareResult = comp(data, walker);

		if     (compareResult == -1) walker = walker->a;
		else if(compareResult ==  1) walker = walker->b;
		else if(compareResult ==  0) break;
		else                         return(NULL);
	}

	return(walker->data);
}

#ifdef BTAVL_TRANSVERSAL
void * btavl_InitTranversal(btavl_t *ctx, btavlFetch_t *f)
{
	f->walker = ctx->head;
	return(f->walker->data);
}

void * btavl_FetchTranversal(btavlFetch_t *f)
{
	if(f->walker == NULL)
		return(NULL);

	f->walker = f->walker->next;
	return(f->walker->data);
}
#endif

int btavlDelete(btavl_t *ctx, void *data, int (*compare)(void *a, void *b), int callDataDealloc)
{
	/*BTAVL_SETCOMPARATOR(ctx, comp, compare);*/

	ctx->n--;



	return(BTAVL_ERROR);
}

int btavlInsert(btavl_t *ctx, void *data, int (*compare)(void *a, void *b))
{
	btavlNode_t *ins    = NULL;
	btavlNode_t *walker = NULL;
	int compareResult   = 0;
	unsigned int h      = 0;

	BTAVL_SETCOMPARATOR(ctx, comp, compare);

	if(ctx->head == NULL){
		ins = ctx->defaultAllocator(sizeof(btavlNode_t));
		if(ins == NULL){
			return(BTAVL_ERROR);
		}

#ifdef BTAVL_TRANSVERSAL
		BTAVL_FILLNODE(ins, NULL, NULL, NULL, 0, data, ctx->end, NULL);
		ctx->end = ins;
#else
		BTAVL_FILLNODE(ins, NULL, NULL, NULL, 0, data);
#endif

		ctx->head = ins;
		ctx->n    = 1;

		return(BTAVL_OK);
	}

	for(walker = ctx->head, h = 1; ; h++){

		compareResult = comp(data, walker->data);

		if(compareResult == -1){

			if(walker->a == NULL){
				/* insert here */

				ins = ctx->defaultAllocator(sizeof(btavlNode_t));
				if(ins == NULL){
					return(BTAVL_ERROR);
				}

#ifdef BTAVL_TRANSVERSAL
				BTAVL_FILLNODE(ins, NULL, NULL, walker, 0, data, ctx->end, NULL);
#else
				BTAVL_FILLNODE(ins, NULL, NULL, walker, 0, data);
#endif

				walker->a = ins;



				ctx->n++;

			}else{
				walker = walker->a;
				continue;
			}

		}else if(compareResult ==  1){

			if(walker->b == NULL){
				/* insert here */

				ins = ctx->defaultAllocator(sizeof(btavlNode_t));
				if(ins == NULL){
					return(BTAVL_ERROR);
				}

#ifdef BTAVL_TRANSVERSAL
				BTAVL_FILLNODE(ins, NULL, NULL, walker, 0, data, ctx->end, NULL);
#else
				BTAVL_FILLNODE(ins, NULL, NULL, walker, 0, data);
#endif

				walker->b = ins;



				ctx->n++;

			}else{
				walker = walker->b;
				continue;
			}

		}else if(compareResult ==  0)
			return(BTAVL_ERROR); /* already exists */
		else
			return(BTAVL_ERROR);

	}





	return(BTAVL_ERROR);
}

int btavlInit(btavl_t *ctx,
              int   (*compare) (void *a, void *b),
              void *(*alloc)   (size_t size),
              void  (*dealloc) (void *data))
{
	ctx->n                  = 0;
	ctx->head               = NULL;
	ctx->defaultCompare     = compare;
	ctx->defaultAllocator   = alloc;
	ctx->defaultDeallocator = dealloc;

#ifdef BTAVL_TRANSVERSAL
	ctx->end = NULL;
#endif

	return(BTAVL_OK);
}
