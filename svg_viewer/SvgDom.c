#include "stdafx.h"
#include "SvgDom.h"

struct _SvgDom{
	DomNode* root;
	DList* id_element_list;
};
/************************************************************************/
/*                                NODE                                  */
/************************************************************************/
#define DOM_NODE_STRUCTURE\
	READONLY DOMString nodeName;\
	DOMString nodeValue;\
	READONLY DomNodeType nodeType;\
	READONLY struct _DomNode* parentNode;\
	READONLY NodeList* childNodes;\
	READONLY struct _DomNode* firstChild;\
	READONLY struct _DomNode* lastChild;\
	READONLY struct _DomNode* previousSibling;\
	READONLY struct _DomNode* nextSibling;\
	READONLY NamedNodeMap* attributes;/*只有Element的attributes不为空！*/\
	/*Modified in DOM Level 2:*/\
	READONLY  DomDocument* ownerDocument;\
	/*Introduced in DOM Level 2:*/\
	READONLY DOMString namespaceURI;\
	/*Introduced in DOM Level 2:*/\
	DOMString prefix;\
	/*Introduced in DOM Level 2:*/\
	READONLY DOMString localName;\
	/*Introduced in DOM Level 3:*/\
	DOMString textContent
struct _DomNode{
	DOM_NODE_STRUCTURE;
};

/************************************************************************/
/*                        ELEMENT : NODE                                */
/************************************************************************/
#define DOM_ELEMENT_STRUCTURE\
	DOM_NODE_STRUCTURE;\
	READONLY DOMString tagName;\
	/*Introduced in DOM Level 3:*/\
	/*READONLY TypeInfo schemaTypeInfo;*/\
	LPCTSTR id/*FIXME：这是自己添加的*/
struct _DomElement{
	DOM_ELEMENT_STRUCTURE;
};

/************************************************************************/
/*                       ATTRIBUTE : NODE                               */
/************************************************************************/
#define  DOM_ATTRIBUTE_STRUCTURE\
	DOM_NODE_STRUCTURE;\
	READONLY DOMString name;\
	READONLY BOOL specified;\
	DOMString value;\
	/*Introduced in DOM Level 2:*/\
	READONLY DomElement* ownerElement;\
	/*Introduced in DOM Level 3:*/\
	/*READONLY TypeInfo schemaTypeInfo;*/\
	/*Introduced in DOM Level 3:*/\
	READONLY BOOL isId
struct _DomAttr{
	DOM_ATTRIBUTE_STRUCTURE;
};

/************************************************************************/
/*                   CHARACTER DATA : NODE                              */
/************************************************************************/
#define DOM_CHARACTER_DATA_STRUCTURE\
	DOM_NODE_STRUCTURE;\
	DOMString data;\
	READONLY unsigned long length
struct _DomCharacterData{
	DOM_CHARACTER_DATA_STRUCTURE;
};

/************************************************************************/
/*                  TEXT : CHARACTER DATA                               */
/************************************************************************/
#define DOM_TEXT_STRUCTURE\
	DOM_NODE_STRUCTURE;\
	/*Introduced in DOM Level 3:*/\
	READONLY BOOL isElementContentWhitespace;\
	/*Introduced in DOM Level 3:*/\
	READONLY DOMString wholeText
struct _DomText{
	DOM_TEXT_STRUCTURE;
};

/************************************************************************/
/*                COMMENT : CHARACTER DATA                              */
/************************************************************************/
/************************************************************************/
/*              DOCUMENT FRAGMENT: NODE                                 */
/************************************************************************/
/************************************************************************/
/*                DOCUMENT TYPE : NODE                                  */
/************************************************************************/
#define DOM_DOCUMENT_TYPE_STRUCTURE\
	DOM_NODE_STRUCTURE;\
	READONLY DOMString name;\
	READONLY NamedNodeMap* entities;\
	READONLY NamedNodeMap* notations;\
	/*Introduced in DOM Level 2:*/\
	READONLY DOMString publicId;\
	/*Introduced in DOM Level 2:*/\
	READONLY DOMString systemId;\
	/*Introduced in DOM Level 2:*/\
	READONLY DOMString internalSubset
struct _DomDocumentType{
	DOM_DOCUMENT_TYPE_STRUCTURE;
};

/************************************************************************/
/*                    DOCUMENT : NODE                                   */
/************************************************************************/
#define DOM_DOCUMENT_STUCTURE\
	DOM_NODE_STRUCTURE;\
	/*Modified in DOM Level 3:*/\
	READONLY DomDocumentType* doctype;\
	/*READONLY DOMImplementation implementation;*/\
	READONLY DomElement* documentElement;\
	/*Introduced in DOM Level 3:*/\
	READONLY DOMString inputEncoding;\
	/*Introduced in DOM Level 3:*/\
	READONLY DOMString xmlEncoding;\
	/*Introduced in DOM Level 3:*/\
	BOOL xmlStandalone;\
	/*Introduced in DOM Level 3:*/\
	DOMString xmlVersion;\
	/*Introduced in DOM Level 3:*/\
	BOOL strictErrorChecking;\
	/*Introduced in DOM Level 3:*/\
	DOMString documentURI\
	/*Introduced in DOM Level 3:*/\
	/*READONLY DOMConfiguration domConfig;*/
struct _DomDocument{
	DOM_DOCUMENT_STUCTURE;
};

/****************  Extended Interfaces: XML Module  *********************/
/************************************************************************/
/*                    CDATASECTION : TEXT                               */
/************************************************************************/
/************************************************************************/
/*                     NOTATION : NODE                                  */
/************************************************************************/
#define DOM_NOTATION_STRUCTURE\
	DOM_NODE_STRUCTURE;\
	READONLY DOMString publicId;\
	READONLY DOMString systemId
struct _DomNotation{
	DOM_NOTATION_STRUCTURE;
};

/************************************************************************/
/*                     ENTITY : NODE                                    */
/************************************************************************/
#define DOM_ENTITY_STRUCTURE\
	DOM_NODE_STRUCTURE;\
	READONLY DOMString publicId;\
	READONLY DOMString systemId;\
	READONLY DOMString notationName;\
	/*Introduced in DOM Level 3:*/\
	READONLY DOMString inputEncoding;\
	/*Introduced in DOM Level 3:*/\
	READONLY DOMString xmlEncoding;\
	/*Introduced in DOM Level 3:*/\
	READONLY DOMString xmlVersion
struct _DomEntity{
	DOM_ENTITY_STRUCTURE;
};

/************************************************************************/
/*                 ENTITY REFERENCE : NODE                              */
/************************************************************************/
/************************************************************************/
/*              PROCESSING INSTRUCTION : NODE                           */
/************************************************************************/
#define DOM_PROCESSING_INSTRUCTION_STRUCTURE\
	DOM_NODE_STRUCTURE;\
	READONLY DOMString target;\
	DOMString data
struct _DomProcessingInstruction{
	DOM_PROCESSING_INSTRUCTION_STRUCTURE;
};
/************************************************************************/
/*                           私有函数声明                               */
/************************************************************************/
PRIVATE DList* attr_dlist_to_dom_attr_list(DList* attr_list);
/************************************************************************/
/*                           公有函数实现                               */
/************************************************************************/
PUBLIC SvgDom* svg_dom_create(){
	SvgDom* thiz = (SvgDom*)calloc(1, sizeof(SvgDom));
	thiz->root = NULL;
	thiz->id_element_list = dlist_create(NULL);//FIXME
	return thiz;
}

PUBLIC void svg_dom_destroy(SvgDom* thiz){
	//FIXME:析构应该析构所有节点，待改进
	return_if_fail(thiz);
	dlist_destroy(thiz->id_element_list);
	free(thiz);
}

PUBLIC DomNode* svg_dom_get_root(SvgDom* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->root;
}

PUBLIC void svg_dom_set_root(SvgDom* thiz, DomNode* root){
	return_if_fail(thiz && root);
	thiz->root = root;
}

PUBLIC DList* svg_dom_get_id_element_list(SvgDom* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->id_element_list;
}

PUBLIC DomElement* svg_dom_get_element_by_id(SvgDom* thiz, LPCTSTR id){
	DomElement* dom_element = NULL;
	Iterator* dom_element_iter = NULL;
	BOOL id_element_found = FALSE;
	return_val_if_fail(thiz && id, NULL);//FIXME
	dom_element_iter = dlist_iterator_create(thiz->id_element_list);
	while(dom_element = (DomElement*)dom_element_iter->get(dom_element_iter)){
		if(!_tcscmp(dom_element->id, id)){
			id_element_found = TRUE;
			break;
		}
		dom_element_iter->next(dom_element_iter);
	}
	dom_element_iter->destroy(dom_element_iter);
	if(id_element_found){
		return dom_element;
	}else{
		return NULL;
	}
}

/************************************************************************/
/*                        ELEMENT : NODE                                */
/************************************************************************/
PUBLIC DomElement* dom_element_create(SvgToken* token){
	DomElement* thiz = NULL;
	ATTR* attr = NULL;
	Iterator* attr_iter = NULL;
	return_val_if_fail(token, NULL);
	thiz = (DomElement*)calloc(1, sizeof(DomElement));
	thiz->nodeType = DOM_NODE_TYPE_ELEMENT;
	thiz->nodeName = copy_string(svg_token_get_data(token));
	thiz->nodeValue = NULL;
	//读取属性信息
	//thiz->attributes = attr_dlist_to_dom_attr_list(svg_token_get_attributes(token));
	thiz->attributes = svg_token_get_attributes(token);
	//查找是否有id属性
	attr_iter = dlist_iterator_create(thiz->attributes);
	while(attr = (ATTR*)attr_iter->get(attr_iter)){
		if(!_tcscmp(attr->name, _T("id"))){
			thiz->id = attr->value;//FIXME:这里没有拷贝
			break;
		}
		attr_iter->next(attr_iter);
	}

	//FIXME:创建子节点链表，这里有待完善
	thiz->childNodes = dlist_create(NULL);

	thiz->tagName = copy_string(svg_token_get_data(token));//FIXME:一些相同的属性是否保留两份副本？如Element的tagName和nodeName
	return thiz;
}

PUBLIC void dom_element_destroy(DomElement* thiz){
	dlist_destroy(thiz->attributes);
	dlist_destroy(thiz->childNodes);
	free((void*)thiz->localName);
}

/************************************************************************/
/*                       ATTRIBUTE : NODE                               */
/************************************************************************/
PUBLIC DomAttr* dom_attr_create(ATTR* attr){
	DomAttr* thiz = NULL;
	return_val_if_fail(attr, NULL);
	thiz = (DomAttr*)calloc(1, sizeof(DomAttr));
	thiz->nodeType = DOM_NODE_TYPE_ATTRIBUTE;
	thiz->nodeName = copy_string(attr->name);
	thiz->nodeValue = copy_string(attr->value);//same as CharacterData.data, the content of the text node.
	//Attr的attributes成员为NULL
	thiz->attributes = NULL;
	//FIXME:If there are no children, this is a NodeList containing no nodes
	//也就是说即使childNodes是空的也应该创建一个空格链表
	//Attr的子节点应该是一个Text节点，其中存储了Attr属性值的字符串。
	thiz->childNodes = NULL;

	thiz->name = copy_string(attr->name);
	//thiz->ownerElement
	//FIXME：程序设计逻辑有问题，ownerElement没传到这里
	//thiz->schemaTypeInfo
	thiz->specified = TRUE;
	//On setting, this creates a Text [p.95] node with the unparsed contents of the string, i.e.
	//any characters that an XML processor would recognize as markup are instead treated as
	//literal text.
	thiz->value = copy_string(attr->value);
	thiz->isId = !_tcscmp(attr->name, _T("id"));//FIXME：ID区不区分大写？
	return thiz;
}
/************************************************************************/
/*                   CHARACTER DATA : NODE                              */
/************************************************************************/

/************************************************************************/
/*                  TEXT : CHARACTER DATA                               */
/************************************************************************/
PUBLIC DomText* dom_text_create(SvgToken* token){
	DomText* thiz = NULL;
	return_val_if_fail(token, NULL);
	thiz = (DomText*)calloc(1, sizeof(DomText));
	thiz->nodeType = DOM_NODE_TYPE_TEXT;
	thiz->nodeName = _T("#text");
	thiz->nodeValue = copy_string(svg_token_get_data(token));//same as CharacterData.data, the content of the text node.
	//Text的attributes成员为NULL
	thiz->attributes = NULL;
	//Text没有子节点
	thiz->childNodes = NULL;
	return thiz;
}

/************************************************************************/
/*                DOCUMENT TYPE : NODE                                  */
/************************************************************************/

/************************************************************************/
/*                     NOTATION : NODE                                  */
/************************************************************************/

/************************************************************************/
/*                     ENTITY : NODE                                    */
/************************************************************************/

/************************************************************************/
/*                 ENTITY REFERENCE : NODE                              */
/************************************************************************/

/************************************************************************/
/*              PROCESSING INSTRUCTION : NODE                           */
/************************************************************************/
PUBLIC DomProcessingInstruction* dom_processing_instruction_create(SvgToken* token){
	DomProcessingInstruction* thiz;
	return_val_if_fail(token, NULL);
	thiz = (DomProcessingInstruction*)calloc(1, sizeof(DomProcessingInstruction));
	thiz->nodeType = DOM_NODE_TYPE_PROCESSING_INSTRUCTION;
	thiz->nodeName = NULL;//same as ProcessingInstruction.target
	thiz->nodeValue = NULL;//same as ProcessingInstruction.data
	//ProcessingInstruction的attributes成员为NULL
	return thiz;
}

/************************************************************************/
/*              DOCUMENT FRAGMENT: NODE                                 */
/************************************************************************/

/************************************************************************/
/*                    DOCUMENT : NODE                                   */
/************************************************************************/
PUBLIC DomDocument* dom_document_create(){
	DomDocument* thiz;
	thiz = (DomDocument*)calloc(1, sizeof(DomDocument));
	thiz->nodeType = DOM_NODE_TYPE_DOCUMENT;
	thiz->nodeName = _T("#document");
	thiz->nodeValue = NULL;
	//Document的attributes成员为NULL
	thiz->attributes = NULL;
	//创建子节点，有待完善
	thiz->childNodes = dlist_create(NULL);
	return thiz;
}

//PUBLIC SVGDOMNode* svg_dom_node_create_comment(SvgToken* token){
//	SVGDOMNode* thiz;
//	return_val_if_fail(token != NULL, NULL);
//	thiz = (SVGDOMNode*)calloc(1, sizeof(SVGDOMNode));
//	thiz->type = DOM_NODE_TYPE_COMMENT;
//	thiz->data = token->data;
//	thiz->attrs = NULL;
//	return thiz;
//}
//
//PUBLIC SVGDOMNode* svg_dom_node_create_pi(SvgToken* token){
//	SVGDOMNode* thiz;
//	return_val_if_fail(token != NULL, NULL);
//	thiz = (SVGDOMNode*)calloc(1, sizeof(SVGDOMNode));
//	thiz->type = DOM_NODE_TYPE_PROCESSING_INSTRUCTION;
//	thiz->data = token->data;
//	thiz->attrs = token->attrs;
//	return thiz;
//}
//
//PUBLIC void svg_dom_node_destroy(SVGDOMNode* thiz){
//	free(thiz->data);
//	dlist_destroy(thiz->attrs);
//	free(thiz);
//}
//
//PUBLIC void svg_dom_node_dump_to_file(SVGDOMNode* thiz, FILE* fp){
//	Iterator* attr_iter;
//	Attr* attr;
//	switch(thiz->type){
//	case DOM_NODE_TYPE_TEXT:
//		fprintf(fp,"%s\n",thiz->data);
//		break;
//	case DOM_NODE_TYPE_COMMENT:
//		fprintf(fp,"<!--%s-->\n",thiz->data);
//		break;
//	case DOM_NODE_TYPE_ELEMENT:
//		fprintf(fp, "<%s ",thiz->data);
//		attr_iter = dlist_iterator_create(thiz->attrs);
//		while(attr = (Attr*)attr_iter->get(attr_iter)){
//			fprintf(fp, "%s=\"%s\" ",attr->name, attr->value);
//			attr_iter->next(attr_iter);
//		}
//		if(thiz->selfClosing){
//			fprintf(fp, "/>");
//		}else{
//			fprintf(fp, ">");
//		}
//		break;
//	case DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
//		fprintf(fp, "<?%s ",thiz->data);
//		attr_iter = dlist_iterator_create(thiz->attrs);
//		while(attr = (Attr*)attr_iter->get(attr_iter)){
//			fprintf(fp, "%s=\"%s\" ",attr->name, attr->value);
//			attr_iter->next(attr_iter);
//		}
//		fprintf(fp, "?>");
//		break;
//	}
//}


PUBLIC void dom_node_append_child(DomNode* thiz, DomNode* child){
	return_if_fail(thiz && child);

	if(thiz->lastChild){
		thiz->lastChild->nextSibling = child;
		child->previousSibling = thiz->lastChild;
		thiz->lastChild = child;
	}else{
		thiz->firstChild = thiz->lastChild = child;
	}
	dlist_append(thiz->childNodes, child);
	child->parentNode = thiz;
}

PUBLIC LPCTSTR dom_node_get_node_name(DomNode* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->nodeName;
}

PUBLIC DomNodeType dom_node_get_node_type(DomNode* thiz){
	return_val_if_fail(thiz, DOM_NODE_TYPE_UNINITIALIZED);
	return thiz->nodeType;
}

PUBLIC DList* dom_node_get_attributes(DomNode* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->attributes;
}

PUBLIC NodeList* dom_node_get_child_nodes(DomNode* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->childNodes;
}

PUBLIC DomNode* dom_node_get_parent_node(DomNode* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->parentNode;
}

PUBLIC LPCTSTR dom_node_get_node_value(DomNode* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->nodeValue;
}

PUBLIC LPCTSTR dom_attr_get_name(DomAttr* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->name;
}

PUBLIC LPCTSTR dom_attr_get_value(DomAttr* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->value;
}

PUBLIC BOOL dom_attr_is_id(DomAttr* thiz){
	return_val_if_fail(thiz, FALSE);
	return thiz->isId;
}

PUBLIC LPCTSTR dom_element_get_id(DomElement* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->id;
}
/************************************************************************/
/*                           私有函数实现                               */
/************************************************************************/
PRIVATE DList* attr_dlist_to_dom_attr_list(DList* attr_list){
	DomAttr* dom_attr = NULL;
	ATTR* attr = NULL;
	Iterator* attr_iter = NULL;
	DList* dom_attr_list = NULL;
	return_val_if_fail(attr_list, NULL);
	dom_attr_list = dlist_create(NULL);//FIXME
	attr_iter = dlist_iterator_create(attr_list);
	while(attr = (ATTR*)attr_iter->get(attr_iter)){
		dom_attr = dom_attr_create(attr);
		dlist_append(dom_attr_list, dom_attr);
	}
	return dom_attr_list;
}