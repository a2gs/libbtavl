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
                            __btavl_compParam_) btavlComp_t (*__btavl_comp_)(void *a, void *b);                                           \
                                                {                                                                                         \
                                                	if(__btavl_ctx_->defaultCompare == NULL) __btavl_comp_ = __btavl_compParam_;           \
                                                	else                                     __btavl_comp_ = __btavl_ctx_->defaultCompare; \
                                                }

#ifdef BTAVL_TRANSVERSAL
#define BTAVL_FILLNODE(__btavl_node_,   \
                       __btavl_a_,      \
                       __btavl_b_,      \
                       __btavl_father_, \
                       __btavl_h_,      \
                       __btavl_data_,   \
                       __btavl_prev_,   \
                       __btavl_next_) { \
                                      	__btavl_node_->a      = __btavl_a_;      \
                                      	__btavl_node_->b      = __btavl_b_;      \
                                      	__btavl_node_->father = __btavl_father_; \
                                      	__btavl_node_->h      = __btavl_h_;      \
                                      	__btavl_node_->data   = __btavl_data_;   \
                                      	__btavl_node_->prev   = __btavl_prev_;   \
                                      	__btavl_node_->next   = __btavl_next_;   \
                                      }
#else
#define BTAVL_FILLNODE(__btavl_node_,   \
                       __btavl_a_,      \
                       __btavl_b_,      \
                       __btavl_father_, \
                       __btavl_h_,      \
                       __btavl_data_) { \
                                      	__btavl_node_->a      = __btavl_a_;      \
                                      	__btavl_node_->b      = __btavl_b_;      \
                                      	__btavl_node_->father = __btavl_father_; \
                                      	__btavl_node_->h      = __btavl_h_;      \
                                      	__btavl_node_->data   = __btavl_data_;   \
                                      }
#endif

#ifdef BTAVL_DEBUG
int btavlStupidDebug(btavl_t *ctx, char * (*printData)(void *data))
{
	btavlNode_t *walker = NULL;
	unsigned int i = 0;

	for(i = ctx->head->h; i > 0; i--){
		printf("%02d: ", i);

		for(walker = ctx->start; walker != NULL; walker = walker->next){
			if(walker->h == i)
				printf("[%s|F: %s|a: %s|b: %s] ",
				       printData(walker->data),
				       printData(walker->father == NULL ? NULL : walker->father->data),
				       printData(walker->a == NULL ? NULL : walker->a->data),
				       printData(walker->b == NULL ? NULL : walker->b->data));
		}
		printf("\n\n");
	}

	return(BTAVL_OK);
}
#endif

float btavlGetSize(btavl_t *ctx)
{
	return(((log(2.236068 * (ctx->n + 2))) / log(1.618034)) - 2);
}

inline static int btavlIsHead(btavlNode_t *n)
{
	return(n->father == NULL ? BTAVL_TRUE : BTAVL_FALSE);
}

inline static btavlNode_t ** btavlGetFather(btavl_t *ctx, btavlNode_t *n)
{
	return(btavlIsHead(n) == BTAVL_TRUE ? &(ctx->head) : &(n->father->father));
}

inline static unsigned int btavlRebalanceToHighest(btavlNode_t *node)
{
	node->h = (node->a->h > node->b->h ? node->a->h : node->b->h) + 1;
	return(node->h);
}

inline static int btavlCalcBalanceFactorByHeight(btavlNode_t *node)
{
	return((node->a == NULL ? 0 : node->a->h) - (node->b == NULL ? 0 : node->b->h));
}

inline static int btavlSLR(btavlNode_t **top, btavlNode_t *a, btavlNode_t *b, btavlNode_t *c) /* Simple Left Rotation */
{
	*top = b;
	a->b = b->a;
	b->a = a;
	b->b = c;

	b->father = a->father;
	a->father = b;
	c->father = b;

	btavlRebalanceToHighest(a);
	btavlRebalanceToHighest(b);
	btavlRebalanceToHighest(c);

	return(BTAVL_TRUE);
}

inline static int btavlSRR(btavlNode_t **top, btavlNode_t *a, btavlNode_t *b, btavlNode_t *c) /* Simple Right Rotation */
{
	*top = b;
	a->b = b->b;
	b->a = c;
	b->b = a;

	b->father = a->father;
	a->father = b;
	c->father = b;

	btavlRebalanceToHighest(a);
	btavlRebalanceToHighest(b);
	btavlRebalanceToHighest(c);

	return(BTAVL_TRUE);
}

inline static int btavlDLR(btavlNode_t **top, btavlNode_t *a, btavlNode_t *b, btavlNode_t *c) /* Double Left Rotation */
{
	*top = c;
	a->b = c->a;
	b->a = c->b;
	c->a = a;
	c->b = b;

	c->father = a->father;
	a->father = c;
	b->father = c;

	btavlRebalanceToHighest(a);
	btavlRebalanceToHighest(b);
	btavlRebalanceToHighest(c);

	return(BTAVL_TRUE);
}

inline static int btavlDRR(btavlNode_t **top, btavlNode_t *a, btavlNode_t *b, btavlNode_t *c) /* Double Right Rotation */
{
	*top = c;
	a->a = c->b;
	b->b = c->a;
	c->a = b;
	c->b = a;

	c->father = a->father;
	a->father = c;
	b->father = c;

	btavlRebalanceToHighest(a);
	btavlRebalanceToHighest(b);
	btavlRebalanceToHighest(c);

	return(BTAVL_TRUE);
}

void * btavlSearch(btavl_t *ctx, void *data, btavlComp_t (*compare)(void *a, void *b))
{
	btavlNode_t *walker = NULL;
	int compareResult   = 0;

	BTAVL_SETCOMPARATOR(ctx, comp, compare);

	if(ctx->head == NULL) return(NULL);

	for(walker = ctx->head, compareResult = 0; ; ){
		compareResult = comp(data, walker);

		if     (compareResult == btavlComp_Left)  walker = walker->a;
		else if(compareResult == btavlComp_Right) walker = walker->b;
		else if(compareResult == btavlComp_Equal) break;
		else                                      return(NULL);
	}

	return(walker->data);
}

#ifdef BTAVL_TRANSVERSAL
int btavlInitTransversal(btavl_t *ctx, btavlFetch_t *f)
{
	f->walker = ctx->start;
	return(BTAVL_OK);
}

void * btavlFetchTransversal(btavlFetch_t *f)
{
	void *data = NULL;

	if(f->walker == NULL)
		return(NULL);

	data = f->walker->data;
	f->walker = f->walker->next;

	return(data);
}
#endif

inline static int btavlBalanceFactor(btavlNode_t *node)
{
	unsigned int ah = (node->a == NULL ? 0 : node->a->h);
	unsigned int bh = (node->b == NULL ? 0 : node->b->h);

	return(ah - bh);
}

inline static int btavlRecalcHeight(btavl_t *ctx, btavlNode_t *backtrack)
{
	for(; ; backtrack = backtrack->father){

		backtrack->h++;

		if(btavlIsHead(backtrack) == BTAVL_TRUE || backtrack->h < backtrack->father->h)
			break;
	}

	return(BTAVL_OK);
}

#define BTAVL_MODE_INSERT (1)
#define BTAVL_MODE_DELETE (2)

#define BTAVL_GETDIRECTION(__btavl_direction_)      (__btavl_direction_ & 0x03)
#define BTAVL_SETDIRECTIONLEFT(__btavl_direction_)  (__btavl_direction_ <<= 0x01)
#define BTAVL_SETDIRECTIONRIGHT(__btavl_direction_) (__btavl_direction_ = ((__btavl_direction_ << 0x01) | 0x01))
#define BTAVL_LEFTLEFT   (0x00)
#define BTAVL_LEFTRIGHT  (0x01)
#define BTAVL_RIGHTLEFT  (0x02)
#define BTAVL_RIGHTRIGHT (0x03)

int btavlBalance(btavl_t *ctx, btavlNode_t *node, int direction, int mode)
{
	int h = 0;
	btavlNode_t **god         = NULL;
	btavlNode_t  *grandfather = NULL;
	btavlNode_t  *father      = NULL;
	btavlNode_t  *son         = NULL;

	grandfather = node->father->father;
	father      = node->father;
	son         = node;
	god         = btavlGetFather(ctx, node->father);

	h = btavlCalcBalanceFactorByHeight(grandfather);
	if((h >= -1) && (h <= 1))
		return(BTAVL_OK);

	if(mode == BTAVL_MODE_INSERT){
		if     (direction == BTAVL_LEFTLEFT)   btavlSRR(god, grandfather, father, son); /* Simple Right Rotation */
		else if(direction == BTAVL_LEFTRIGHT)  btavlDLR(god, grandfather, father, son); /* Double Left Rotation  */
		else if(direction == BTAVL_RIGHTLEFT)  btavlDRR(god, grandfather, father, son); /* Double Right Rotation */
		else if(direction == BTAVL_RIGHTRIGHT) btavlSLR(god, grandfather, father, son); /* Simple Left Rotation  */
	}else if(mode == BTAVL_MODE_DELETE){
	}else return(BTAVL_ERROR);

	return(BTAVL_OK);
}

int btavlDelete(btavl_t *ctx, void *data, btavlComp_t (*compare)(void *a, void *b), int callDataDealloc)
{
	/*BTAVL_SETCOMPARATOR(ctx, comp, compare);*/

	ctx->n--;

	return(BTAVL_ERROR);
}

int btavlInsert(btavl_t *ctx, void *data, btavlComp_t (*compare)(void *a, void *b))
{
	btavlNode_t *ins    = NULL;
	btavlNode_t *walker = NULL;
	int compareResult   = 0;
	unsigned int direction = 0;
	unsigned int i      = 0;

	BTAVL_SETCOMPARATOR(ctx, comp, compare);

	ins = ctx->defaultAllocator(sizeof(btavlNode_t));
	if(ins == NULL){
		return(BTAVL_ERROR);
	}

	if(ctx->head == NULL){

#ifdef BTAVL_TRANSVERSAL
		BTAVL_FILLNODE(ins, NULL, NULL, NULL, 1, data, NULL, NULL);
		ctx->start = ins;
		ctx->end   = ins;
#else
		BTAVL_FILLNODE(ins, NULL, NULL, NULL, 1, data);
#endif

		ctx->head = ins;
		ctx->n    = 1;

		return(BTAVL_OK);
	}

	for(walker = ctx->head, direction = 0x00, i = 0; ; i++){

		if(i > 2){
		}

		compareResult = comp(data, walker->data);

		if(compareResult == btavlComp_Left){

			BTAVL_SETDIRECTIONLEFT(direction);

			if(walker->a == NULL){
				/* insert here */

#ifdef BTAVL_TRANSVERSAL
				BTAVL_FILLNODE(ins, NULL, NULL, walker, 0, data, ctx->end, NULL);
#else
				BTAVL_FILLNODE(ins, NULL, NULL, walker, 0, data);
#endif

				walker->a = ins;

				break;

			}else{
				walker = walker->a;
				continue;
			}

		}else if(compareResult == btavlComp_Right){

			BTAVL_SETDIRECTIONRIGHT(direction);

			if(walker->b == NULL){
				/* insert here */

#ifdef BTAVL_TRANSVERSAL
				BTAVL_FILLNODE(ins, NULL, NULL, walker, 0, data, ctx->end, NULL);
#else
				BTAVL_FILLNODE(ins, NULL, NULL, walker, 0, data);
#endif

				walker->b = ins;

				break;

			}else{
				walker = walker->b;
				continue;
			}

		}else if(compareResult == btavlComp_Equal){
			/* already exists */
			ctx->defaultDeallocator(ins);
			return(BTAVL_ERROR);
		}else{
			ctx->defaultDeallocator(ins);
			return(BTAVL_ERROR);
		}
	}

	ctx->n++;
	ctx->end->next = ins;
	ctx->end = ins;

	btavlRecalcHeight(ctx, ins);

	btavlBalance(ctx, ins, BTAVL_GETDIRECTION(direction), BTAVL_MODE_INSERT);

	return(BTAVL_OK);
}

int btavlInit(btavl_t    *ctx,
              btavlComp_t (*compare) (void *a, void *b),
              void       *(*alloc)   (size_t size),
              void        (*dealloc) (void *data))
{
	ctx->n                  = 0;
	ctx->head               = NULL;
	ctx->defaultCompare     = compare;
	ctx->defaultAllocator   = alloc;
	ctx->defaultDeallocator = dealloc;

#ifdef BTAVL_TRANSVERSAL
	ctx->start = NULL;
	ctx->end   = NULL;
#endif

	return(BTAVL_OK);
}
