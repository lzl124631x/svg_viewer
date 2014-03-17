#ifndef DListIterator_h
#define DListIterator_h
DECLS_BEGIN

#include "DList.h"
#include "Iterator.h"

Iterator* dlist_iterator_create(DList* dlist);

DECLS_END
#endif/*DLIST_ITERATOR_H*/