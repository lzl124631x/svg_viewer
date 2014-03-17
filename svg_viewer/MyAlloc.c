#include "stdafx.h"
#include "MyAlloc.h"

#undef malloc
#undef calloc
#undef free
#include <malloc.h>

unsigned g_total_alloced=0;
unsigned g_total_freed=0;

void* my_alloc(size_t size){
	g_total_alloced += size;
	return malloc(size);
}

void* my_calloc(size_t count, size_t size){
	g_total_alloced += size;
	return calloc(count, size);
}

void my_free(void* thiz){
	if(!thiz) return;
	g_total_freed += _msize(thiz);
	free(thiz);
}