#include "stdafx.h"
#include <string.h>
#include "DListIterator.h"
//注意，此.c文件被Dlist.c引用，不参与编译。

typedef struct _PrivInfo{
	DList* dlist;
	DListNode* cursor;
	int offset;
}PrivInfo;

PRIVATE Ret  dlist_iterator_set(Iterator* thiz, void* data);
PRIVATE void* dlist_iterator_get(Iterator* thiz);
PRIVATE Ret  dlist_iterator_next(Iterator* thiz);
PRIVATE Ret  dlist_iterator_prev(Iterator* thiz);
PRIVATE Ret  dlist_iterator_advance(Iterator* thiz, int offset);
PRIVATE int  dlist_iterator_offset(Iterator* thiz);
PRIVATE Ret  dlist_iterator_clone(Iterator* thiz, Iterator** cloned);
PRIVATE void dlist_iterator_destroy(Iterator* thiz);

PUBLIC Iterator* dlist_iterator_create(DList* dlist){
	Iterator* thiz = NULL;
	return_val_if_fail(dlist != NULL, NULL);

	if((thiz = (Iterator*)calloc(1, sizeof(Iterator) + sizeof(PrivInfo))) != NULL){
		PrivInfo* priv = (PrivInfo*)thiz->priv;

		thiz->set      =  dlist_iterator_set;
		thiz->get      =  dlist_iterator_get;
		thiz->next     =  dlist_iterator_next;
		thiz->prev     =  dlist_iterator_prev;
		thiz->advance  =  dlist_iterator_advance;
		thiz->clone    =  dlist_iterator_clone;
		thiz->offset   =  dlist_iterator_offset;
		thiz->destroy  =  dlist_iterator_destroy;

		priv->dlist  = dlist;
		priv->cursor = dlist->first;
		priv->offset = 0;
	}

	return thiz;
}

PRIVATE Ret  dlist_iterator_set(Iterator* thiz, void* data){
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	return_val_if_fail(priv->cursor != NULL && priv->dlist != NULL, RET_INVALID_PARAMS);

	priv->cursor->data = data;

	return RET_OK;
}

PRIVATE void* dlist_iterator_get(Iterator* thiz){
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	return_val_if_fail(priv->cursor != NULL && priv->dlist != NULL, NULL);

	return priv->cursor->data;
}

PRIVATE Ret  dlist_iterator_next(Iterator* thiz){
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	return_val_if_fail(priv->cursor != NULL && priv->dlist != NULL, RET_INVALID_PARAMS);

	priv->cursor = priv->cursor->next;
	priv->offset++;
	if(priv->cursor)
		return RET_OK;
	return RET_FAIL;
}

PRIVATE Ret  dlist_iterator_prev(Iterator* thiz){
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	return_val_if_fail(priv->cursor != NULL && priv->dlist != NULL, RET_INVALID_PARAMS);

	priv->cursor = priv->cursor->prev;
	priv->offset--;

	return RET_OK;
}

PRIVATE Ret  dlist_iterator_advance(Iterator* thiz, int offset){
	int n   = offset;
	Ret ret = RET_FAIL;
	DListNode* iter = NULL;
	PrivInfo*  priv = (PrivInfo*)thiz->priv;
	return_val_if_fail(priv->cursor != NULL && priv->dlist != NULL, RET_INVALID_PARAMS);

	iter = priv->cursor;
	if(offset > 0){
		for(n = offset; n > 0 && iter != NULL; n--){
			iter = iter->next;
		}
	}else{
		for(n = -offset; n > 0 && iter != NULL; n--){
			iter = iter->prev;
		}
	}

	if(iter != NULL){
		priv->cursor = iter;
		priv->offset += offset;

		ret = RET_OK;
	}

	return ret;
}

PRIVATE int  dlist_iterator_offset(Iterator* thiz){
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	return_val_if_fail(priv->cursor != NULL && priv->dlist != NULL, RET_INVALID_PARAMS);

	return priv->offset;
}

PRIVATE Ret  dlist_iterator_clone(Iterator* thiz, Iterator** cloned){
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	return_val_if_fail(priv->cursor != NULL && priv->dlist != NULL, RET_INVALID_PARAMS);

	*cloned = (Iterator*)calloc(1, sizeof(Iterator) + sizeof(PrivInfo));
	if(*cloned != NULL){
		memcpy(*cloned, thiz, sizeof(Iterator) + sizeof(PrivInfo));
	}

	return *cloned != NULL ? RET_OK : RET_FAIL;
}

PRIVATE void dlist_iterator_destroy(Iterator* thiz){
	if(thiz != NULL){
		free(thiz);
	}
}