#include "stdafx.h"
#include "TypeDef.h"

const ARRAY array_empty = {0};

const LENGTH length_zero = {0};
const LENGTH length_100_percent = {1, UNIT_ID_PERCENTAGE};

//初始化属性
PUBLIC INLINE ATTR* attr_create(){
	ATTR* attr = (ATTR*)calloc(1, sizeof(ATTR));
	return attr;
}

//析构属性，参数attr的类型应为Attr
PUBLIC INLINE void attr_destroy(void* attr){
	free(((ATTR*)attr)->name);
	free(((ATTR*)attr)->value);
	free(((ATTR*)attr));
}

PUBLIC POINT real_point_to_point(REAL_POINT thiz){
	POINT pt = {(LONG)thiz.x, (LONG)thiz.y};
	return pt;
}
