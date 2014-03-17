/*
 * File:    DList.h
 * Author:  Liu Zhenglai <lzl124631x@163.com>
 * Date:	2012-12-05
 * Brief:   Doubly Linked List Header File.
 * Copyright (c) Liu Zhenglai
 */

#ifndef DList_h
#define DList_h

DECLS_BEGIN

	#include <stdio.h>

	typedef enum _DListRet{
		DLIST_RET_OK,
		DLIST_RET_OOM,
		DLIST_RET_STOP,
		DLIST_RET_INVALID_PARAMS,
		DLIST_RET_FAIL
	}DListRet;

	struct _DList;
	typedef struct _DList DList;

	typedef void     (*DListDataDestroyFunc)(void* data);
	typedef int      (*DListDataCompareFunc)(void* ctx, void* data);
	typedef DListRet (*DListDataVisitFunc)(void* ctx, void* data);

	DList*		dlist_create(DListDataDestroyFunc data_destroy);//NOTE: removed the context of destructor.
	void		dlist_destroy(DList* thiz);
	
	DListRet	dlist_insert_at(DList* thiz, size_t index, void* data);
	DListRet	dlist_prepend(DList* thiz, void* data);
	DListRet	dlist_append(DList* thiz, void* data);
	DListRet	dlist_remove_at(DList* thiz, size_t index);
	DListRet	dlist_get_at(DList* thiz, size_t index, void** data);
	DListRet	dlist_set_at(DList* thiz, size_t index, void* data);
	size_t		dlist_length(DList* thiz);
	int			dlist_find(DList* thiz, DListDataCompareFunc cmp, void* ctx);//FIXME : missing a target data?
	DListRet	dlist_foreach(DList* thiz, DListDataVisitFunc visit, void* ctx);

DECLS_END
#endif