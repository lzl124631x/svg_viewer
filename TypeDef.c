#include "stdafx.h"
#include "TypeDef.h"

const ARRAY array_empty = {0};

const LENGTH length_zero = {0};
const LENGTH length_100_percent = {1, UNIT_ID_PERCENTAGE};

//��ʼ������
PUBLIC INLINE ATTR* attr_create(){
	ATTR* attr = (ATTR*)calloc(1, sizeof(ATTR));
	return attr;
}

//�������ԣ�����attr������ӦΪAttr
PUBLIC INLINE void attr_destroy(void* attr){
	free(((ATTR*)attr)->name);
	free(((ATTR*)attr)->value);
	free(((ATTR*)attr));
}

PUBLIC POINT real_point_to_point(REAL_POINT thiz){
	POINT pt = {(LONG)thiz.x, (LONG)thiz.y};
	return pt;
}
