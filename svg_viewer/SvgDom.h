#ifndef SvgDom_h
#define SvgDom_h
DECLS_BEGIN

#include "DList.h"
#include "SvgToken.h"

typedef enum _DomNodeType{
	DOM_NODE_TYPE_UNINITIALIZED = 0,
	DOM_NODE_TYPE_ELEMENT = 1,//const unsigned short
	DOM_NODE_TYPE_ATTRIBUTE = 2,
	DOM_NODE_TYPE_TEXT = 3,
	DOM_NODE_TYPE_CDATA_SECTION = 4,
	DOM_NODE_TYPE_ENTITY_REFERENCE = 5,
	DOM_NODE_TYPE_ENTITY = 6,
	DOM_NODE_TYPE_PROCESSING_INSTRUCTION = 7,
	DOM_NODE_TYPE_COMMENT = 8,
	DOM_NODE_TYPE_DOCUMENT = 9,
	DOM_NODE_TYPE_DOCUMENT_TYPE = 10,
	DOM_NODE_TYPE_DOCUMENT_FRAGMENT = 11,
	DOM_NODE_TYPE_NOTATION = 12
}DomNodeType;

typedef DList NodeList, NamedNodeMap;//��ʱ��DListֱ��ʵ��NodeList��NamedNodeMap

struct _SvgDom;
typedef struct _SvgDom SvgDom;

struct _DomNode;
typedef struct _DomNode DomNode;

struct _DomDocument;
typedef struct _DomDocument DomDocument;

struct _DomElement;
typedef struct _DomElement DomElement;

struct _DomAttr;
typedef struct _DomAttr DomAttr;

struct _DomCharacterData;
typedef struct _DomCharacterData DomCharacterData;

struct _DomText;
typedef struct _DomText DomText;

//Commentֱ�Ӽ̳�CharacterData
typedef DomCharacterData DomComment;

//DocumentFragmentֱ�Ӽ̳�Node
typedef DomNode DomDocumentFragment;

struct _DomDocumentType;
typedef struct _DomDocumentType DomDocumentType;

struct _DomDocument;
typedef struct _DomDocument DomDocument;

//CDATASectionֱ�Ӽ̳�Text
typedef DomText DomCDATASection;

struct _DomNotation;
typedef struct _DomNotation DomNotation;

struct _DomEntity;
typedef struct _DomEntity DomEntity;

//EntityReferenceֱ�Ӽ̳�Node
typedef DomNode DomEntityReference;

struct _DomProcessingInstruction;
typedef struct _DomProcessingInstruction DomProcessingInstruction;

//����SvgDom
PUBLIC SvgDom* svg_dom_create();
//����SvgDom
PUBLIC void svg_dom_destroy(SvgDom* thiz);
//��ȡSvgDom��Root��Ա
PUBLIC DomNode* svg_dom_get_root(SvgDom* thiz);
//����SvgDom��Root��Ա
PUBLIC void svg_dom_set_root(SvgDom* thiz, DomNode* root);
//��ȡSvgDom��IdElementList��Ա
PUBLIC DList* svg_dom_get_id_element_list(SvgDom* thiz);
//����Ԫ��Id��SvgDom�л�ȡDomElement
PUBLIC DomElement* svg_dom_get_element_by_id(SvgDom* thiz, LPCTSTR id);


//
PUBLIC DomElement* dom_element_create(SvgToken* token);
PUBLIC DomText* dom_text_create(SvgToken* token);
PUBLIC DomDocument* dom_document_create();
PUBLIC DomProcessingInstruction* dom_processing_instruction_create(SvgToken* token);

PUBLIC DomNode* dom_comment_create(SvgToken* token);
PUBLIC void svg_dom_node_destroy(DomNode* thiz);

PUBLIC void dom_node_append_child(DomNode* thiz, DomNode* child);
//��ȡDomNode��NodeName��Ա
PUBLIC LPCTSTR dom_node_get_node_name(DomNode* thiz);
//��ȡDomNode��NodeType��Ա
PUBLIC DomNodeType dom_node_get_node_type(DomNode* thiz);
//��ȡDomNode��Attributes��Ա
PUBLIC DList* dom_node_get_attributes(DomNode* thiz);
//��ȡDomNode��ChildNodes��Ա
PUBLIC NodeList* dom_node_get_child_nodes(DomNode* thiz);
//��ȡDomNode��ParentNode��Ա
PUBLIC DomNode* dom_node_get_parent_node(DomNode* thiz);
//��ȡDomNode��NodeValue��Ա
PUBLIC LPCTSTR dom_node_get_node_value(DomNode* thiz);

//��ȡDomAttr��Name��Ա
PUBLIC LPCTSTR dom_attr_get_name(DomAttr* thiz);
//��ȡDomAttr��Value��Ա
PUBLIC LPCTSTR dom_attr_get_value(DomAttr* thiz);
//��ȡDomAttr��IsId��Ա
PUBLIC BOOL dom_attr_is_id(DomAttr* thiz);

//��ȡDomElement��Id��Ա
PUBLIC LPCTSTR dom_element_get_id(DomElement* thiz);

DECLS_END
#endif