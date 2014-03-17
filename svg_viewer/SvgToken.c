#include "stdafx.h"
#include "SvgToken.h"


struct _SvgToken{
	SvgTokenType tokenType;
	LPTSTR data;//如果是StartTag或EndTag，则data保存的是Tag名；如果是Text或Comment，则data保存的是文字内容。
	BOOL selfCloing;//selfCloing为TRUE则表示标签是自关闭的，如<br/>
	DList* attrs;
	ATTR* currentAttr;
};
/************************************************************************/
/*                         私有函数声明                                 */
/************************************************************************/

/************************************************************************/
/*                         公有函数实现                                 */
/************************************************************************/
PUBLIC SvgToken* svg_token_create(){
	SvgToken* thiz=(SvgToken*)calloc(1, sizeof(SvgToken));
	thiz->tokenType=TOKEN_UNINITIALIZED;
	return thiz;
}

PUBLIC void svg_token_clear(SvgToken* thiz){
	thiz->tokenType=TOKEN_UNINITIALIZED;
	free(thiz->data);
	//dlist_destroy(thiz->attrs);//token将属性列表传递给dom，此处不析构属性列表
	ZeroMemory(thiz, sizeof(SvgToken));
	thiz->attrs = dlist_create(attr_destroy);
}

PUBLIC void  svg_token_destroy(SvgToken* thiz){
	//析构attrs链表
	free(thiz);
}

PUBLIC SvgTokenType svg_token_get_token_type(SvgToken* thiz){
	return thiz->tokenType;
}

PUBLIC BOOL svg_token_get_self_closing(SvgToken* thiz){
	return thiz->selfCloing;
}

PUBLIC void svg_token_set_self_closing(SvgToken* thiz, BOOL selfClosing){
	thiz->selfCloing = selfClosing;
}

PUBLIC LPTSTR svg_token_get_data(SvgToken* thiz){
	return thiz->data;
}

PUBLIC void svg_token_set_data(SvgToken* thiz, LPCTSTR data){
	thiz->data = (LPTSTR)data;
}

PUBLIC ATTR* svg_token_get_current_attribute(SvgToken* thiz){
	return thiz->currentAttr;
}

PUBLIC DList* svg_token_get_attributes(SvgToken* thiz){
	return thiz->attrs;
}

PUBLIC void svg_token_begin_start_tag(SvgToken* thiz)
{
	//ASSERT(character);
	//ASSERT(m_type == TypeSet::Uninitialized);
	thiz->tokenType = TOKEN_START_TAG;
	thiz->selfCloing = FALSE;
	thiz->currentAttr = 0;
	thiz->attrs = dlist_create(attr_destroy);//m_attributes.clear();

	//m_data.append(character);
}

PUBLIC void svg_token_begin_end_tag(SvgToken* thiz)
{
	//ASSERT(m_type == TypeSet::Uninitialized);
	thiz->tokenType=TOKEN_END_TAG;
	thiz->selfCloing = FALSE;
	thiz->currentAttr = 0;
	//m_attributes.clear();//结束标签也要属性？先取消这一行

	//m_data.append(characters);
}

PUBLIC INLINE void svg_token_begin_new_attribute(SvgToken* thiz){
	thiz->currentAttr = attr_create();
}

PUBLIC INLINE void svg_token_end_new_attribute(SvgToken* thiz){
	dlist_append(thiz->attrs, thiz->currentAttr);
	thiz->currentAttr = 0;
}

PUBLIC void svg_token_ensure_is_text_token(SvgToken* thiz){
	return_if_fail(thiz->tokenType == TOKEN_UNINITIALIZED || thiz->tokenType == TOKEN_TEXT)
	thiz->tokenType = TOKEN_TEXT;
}
/************************************************************************/
/*                         私有函数实现                                 */
/************************************************************************/
