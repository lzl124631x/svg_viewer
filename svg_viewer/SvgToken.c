#include "stdafx.h"
#include "SvgToken.h"


struct _SvgToken{
	SvgTokenType tokenType;
	LPTSTR data;//�����StartTag��EndTag����data�������Tag���������Text��Comment����data��������������ݡ�
	BOOL selfCloing;//selfCloingΪTRUE���ʾ��ǩ���Թرյģ���<br/>
	DList* attrs;
	ATTR* currentAttr;
};
/************************************************************************/
/*                         ˽�к�������                                 */
/************************************************************************/

/************************************************************************/
/*                         ���к���ʵ��                                 */
/************************************************************************/
PUBLIC SvgToken* svg_token_create(){
	SvgToken* thiz=(SvgToken*)calloc(1, sizeof(SvgToken));
	thiz->tokenType=TOKEN_UNINITIALIZED;
	return thiz;
}

PUBLIC void svg_token_clear(SvgToken* thiz){
	thiz->tokenType=TOKEN_UNINITIALIZED;
	free(thiz->data);
	//dlist_destroy(thiz->attrs);//token�������б��ݸ�dom���˴������������б�
	ZeroMemory(thiz, sizeof(SvgToken));
	thiz->attrs = dlist_create(attr_destroy);
}

PUBLIC void  svg_token_destroy(SvgToken* thiz){
	//����attrs����
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
	//m_attributes.clear();//������ǩҲҪ���ԣ���ȡ����һ��

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
/*                         ˽�к���ʵ��                                 */
/************************************************************************/
