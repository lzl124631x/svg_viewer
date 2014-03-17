/*
 * File:    DList.c
 * Author:  Liu Zhenglai <lzl124631x@163.com>
 * Date:	2012-12-05
 * Brief:   Doubly Linked List Implementation.
 * Copyright (c) Liu Zhenglai
 */

#include "stdafx.h"
#include "DList.h"

typedef struct _DListNode{
	struct _DListNode *prev, *next;
	void* data;
}DListNode;

struct _DList{
	DListNode *first, *last;
	size_t length;
	DListDataDestroyFunc data_destroy;
};
/************************************************************************/
/*                            DECLARATION                               */
/************************************************************************/
PRIVATE DListNode*		dlist_node_create(void* data);
PRIVATE void			dlist_node_destroy(DListNode* thiz, DListDataDestroyFunc data_destroy);
PRIVATE DListNode*		dlist_get_node(DList* thiz, size_t index);
PRIVATE void			dlist_remove_node(DList* thiz, DListNode* node);
/************************************************************************/
/*                              PUBLIC                                  */
/************************************************************************/
DList* dlist_create(DListDataDestroyFunc data_destroy){
	DList* thiz = (DList*)malloc(sizeof(DList));
	if(thiz != NULL){
		thiz->first = thiz->last = NULL;
		thiz->length = 0;
		thiz->data_destroy = data_destroy;
	}
	return thiz;
}

void dlist_destroy(DList* thiz){
	DListNode* iter = NULL, *next = NULL;
	return_if_fail(thiz != NULL);
	
	iter = thiz->first;
	while(iter != NULL){
		next = iter->next;
		dlist_remove_node(thiz, iter);
		iter = next;
	}
	free(thiz);
}

/*
 * The index parameter represents the position where the new data locates after insertion.
 * If the index parameter is out of bound, the new data is always inserted and becomes the new tail of the list.
 */
DListRet dlist_insert_at(DList* thiz, size_t index, void* data){
	DListNode* node = NULL;
	DListNode* cursor = NULL;
	return_val_if_fail(thiz != NULL, DLIST_RET_INVALID_PARAMS); 
	if((node = dlist_node_create(data)) == NULL){
		return DLIST_RET_OOM; 
	}

	cursor = dlist_get_node(thiz, index);
	if(cursor != NULL){		//in-bound
		if(cursor == thiz->first){
			thiz->first = node;
		}else{
			cursor->prev->next = node;
			node->prev = cursor->prev;
		}
		node->next = cursor;
		cursor->prev = node;
	}else{					//out-of-bound
		if(thiz->length == 0){
			thiz->first = node;
		}else{
			thiz->last->next = node;
			node->prev = thiz->last;
		}
		thiz->last = node;
	}
	thiz->length++;
	return DLIST_RET_OK;
}

DListRet dlist_prepend(DList* thiz, void* data){
	return dlist_insert_at(thiz, 0, data);
}

DListRet dlist_append(DList* thiz, void* data){
	return dlist_insert_at(thiz, -1, data);
}

DListRet dlist_remove_at(DList* thiz, size_t index){
	DListNode* cursor = NULL;
	return_val_if_fail(thiz != NULL, DLIST_RET_INVALID_PARAMS); 
	cursor = dlist_get_node(thiz, index);
	if(cursor != NULL){
		if(cursor == thiz->first){
			thiz->first = thiz->first->next;
			thiz->first->prev = NULL;
		}else if(cursor == thiz->last){
			thiz->last = thiz->last->prev;
			thiz->last->next = NULL;
		}else{
			cursor->prev->next = cursor->next;
			cursor->next->prev = cursor->prev;
		}
		dlist_remove_node(thiz, cursor);
		thiz->length--;
	}
	return DLIST_RET_OK;
}

DListRet dlist_get_at(DList* thiz, size_t index, void** data){
	DListNode* cursor = NULL;
	return_val_if_fail(thiz != NULL, DLIST_RET_INVALID_PARAMS); 
	cursor = dlist_get_node(thiz, index);
	if(cursor != NULL){
		*data = cursor->data;
		return DLIST_RET_OK;
	}else{
		return DLIST_RET_FAIL;
	}
}

DListRet dlist_set_at(DList* thiz, size_t index, void* data){
	DListNode* cursor = NULL;
	return_val_if_fail(thiz != NULL, DLIST_RET_INVALID_PARAMS); 
	cursor = dlist_get_node(thiz, index);
	if(cursor != NULL){
		cursor->data = data;
		return DLIST_RET_OK;
	}else{
		return DLIST_RET_FAIL;
	}
}

size_t dlist_length(DList* thiz){
	return_val_if_fail(thiz != NULL, 0);
	return thiz->length;
}

DListRet dlist_foreach(DList* thiz, DListDataVisitFunc visit, void* ctx){
	DListRet ret = DLIST_RET_OK;
	DListNode* iter = NULL;
	return_val_if_fail(thiz != NULL && visit != NULL, DLIST_RET_INVALID_PARAMS);
	iter = thiz->first;
	while(iter != NULL && ret != DLIST_RET_STOP){
		ret = visit(ctx, iter->data);
		iter = iter->next;
	}
	return ret;
}

int dlist_find(DList* thiz, DListDataCompareFunc cmp, void* ctx){
	int i = 0;
	DListNode* iter = NULL;
	return_val_if_fail(thiz != NULL && cmp != NULL, -1);
	iter = thiz->first;
	while(iter != NULL){
		if(cmp(ctx, iter->data) == 0){
			break;
		}
		i++;
		iter = iter->next;
	}
	return i;
}
/************************************************************************/
/*                              PRIVATE                                 */
/************************************************************************/
PRIVATE DListNode* dlist_node_create(void* data){
	DListNode* thiz = (DListNode*)malloc(sizeof(DListNode));
	if(thiz != NULL){
		thiz->prev = NULL;
		thiz->next = NULL;
		thiz->data = data;
	}
	return thiz;
}

PRIVATE void dlist_remove_node(DList* thiz, DListNode* node){
	dlist_node_destroy(node, thiz->data_destroy);
}

PRIVATE void dlist_node_destroy(DListNode* thiz, DListDataDestroyFunc data_destroy){
	if(thiz != NULL){
		thiz->prev = thiz->next = NULL;
		if(data_destroy != NULL){
			data_destroy(thiz->data);
		}
		free(thiz);
	}
}

PRIVATE DListNode* dlist_get_node(DList* thiz, size_t index){
	DListNode* iter = NULL;
	return_val_if_fail(thiz != NULL, NULL); 

	if(index < 0 || index >= thiz->length){
		return NULL;
	}

	if(index == 0){// FIXME:前两个Ifelse感觉浪费
		return thiz->first;
	}else if(index == thiz->length - 1){
		return thiz->last;
	}else{
		if(index < thiz->length / 2){
			iter = thiz->first;
			while(iter && index > 0){
				iter = iter->next;
				index--;
			}
		}else{
			iter = thiz->last;
			while(iter && index < thiz->length - 1){
				iter = iter->prev;
				index++;
			}
		}
		return iter;
	}
}

#include "DListIterator.c"