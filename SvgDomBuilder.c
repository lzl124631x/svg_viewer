#include "stdafx.h"
#include "SvgDomBuilder.h"
#include "SvgDom.h"

struct _SvgDomBuilder{
	InsertionMode insertionMode;
	SvgDom* svgDom;
	DomNode* current;
};
/************************************************************************/
/*                             ˽�к�������                             */
/************************************************************************/
PRIVATE void svg_dom_builder_process_start_token(SvgDomBuilder* thiz,SvgToken* token);
PRIVATE void svg_dom_builder_process_end_token(SvgDomBuilder* thiz,SvgToken* token);
PRIVATE void svg_dom_builder_process_text(SvgDomBuilder* thiz,SvgToken* token);
PRIVATE void svg_dom_builder_process_pi(SvgDomBuilder* thiz,SvgToken* token);
/************************************************************************/
/*                             ���к���ʵ��                             */
/************************************************************************/
PUBLIC SvgDomBuilder* svg_dom_builder_create(){
	SvgDomBuilder* thiz = (SvgDomBuilder*)calloc(1, sizeof(SvgDomBuilder));
	thiz->insertionMode = InSVGMode;
	thiz->svgDom = svg_dom_create();
	thiz->current = (DomNode*)dom_document_create();
	svg_dom_set_root(thiz->svgDom, thiz->current);
	return thiz;
}

PUBLIC void svg_dom_builder_destroy(SvgDomBuilder* thiz){
	return_if_fail(thiz);
	free(thiz);
}

PUBLIC void svg_dom_builder_process_token(SvgDomBuilder* thiz, SvgToken* token){
	switch(svg_token_get_token_type(token)){
	case TOKEN_UNINITIALIZED:break;//��Ӧ�õ�������
	case TOKEN_START_TAG:svg_dom_builder_process_start_token(thiz, token); break;
	case TOKEN_END_TAG:svg_dom_builder_process_end_token(thiz, token); break;
	case TOKEN_TEXT:svg_dom_builder_process_text(thiz, token); break;
	case TOKEN_AFTER_EM: break;
	case TOKEN_AFTER_QM: svg_dom_builder_process_pi(thiz, token); break;
	}
}

PUBLIC SvgDom* svg_dom_builder_get_svg_dom(SvgDomBuilder* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->svgDom;
}

/************************************************************************/
/*                             ˽�к���ʵ��                             */
/************************************************************************/
PRIVATE void svg_dom_builder_process_start_token(SvgDomBuilder* thiz, SvgToken* token){
	//��current�ڵ�����ӽڵ㡣��selfClosing��current���䣻����currentָ���²���Ľڵ㡣
	DomNode* dom_node;
	return_if_fail(thiz && token && svg_token_get_token_type(token) == TOKEN_START_TAG);
	dom_node = (DomNode*)dom_element_create(token);

	if(dom_element_get_id((DomElement*)dom_node)){
		dlist_append(svg_dom_get_id_element_list(thiz->svgDom), dom_node);
	}

	dom_node_append_child(thiz->current, dom_node);
	if(!svg_token_get_self_closing(token)){//�������Թرյı�ǩ��currentָ���ӽڵ㡣
		thiz->current = dom_node;
	}
}

PRIVATE void svg_dom_builder_process_end_token(SvgDomBuilder* thiz,SvgToken* token){
	return_if_fail(thiz && token && thiz->current && dom_node_get_parent_node(thiz->current));
	//����Ӧ�����һ���жϣ���ǰ�ڵ�������Ƿ����token������
	thiz->current = dom_node_get_parent_node(thiz->current);
}

PRIVATE void svg_dom_builder_process_text(SvgDomBuilder* thiz,SvgToken* token){
	DomNode* dom_node;
	return_if_fail(thiz && token && thiz->current && svg_token_get_token_type(token) == TOKEN_TEXT);
	dom_node = (DomNode*)dom_text_create(token);

	dom_node_append_child(thiz->current, dom_node);
}

PRIVATE void svg_dom_builder_process_pi(SvgDomBuilder* thiz,SvgToken* token){
	DomNode* dom_node;
	return_if_fail(thiz && token && thiz->current && svg_token_get_token_type(token) == TOKEN_AFTER_QM);
	dom_node = (DomNode*)dom_processing_instruction_create(token);

	dom_node_append_child(thiz->current, dom_node);
}