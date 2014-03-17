#ifndef MyAlloc_h
#define MyAlloc_h

DECLS_BEGIN

extern unsigned g_total_alloced;
extern unsigned g_total_freed;

#define malloc my_alloc
#define calloc my_calloc
#define free my_free

void* my_alloc(size_t size);
void* my_calloc(size_t count, size_t size);
void my_free(void* thiz);

DECLS_END
#endif