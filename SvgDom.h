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

typedef DList NodeList, NamedNodeMap;//暂时以DList直接实现NodeList和NamedNodeMap

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

//Comment直接继承CharacterData
typedef DomCharacterData DomComment;

//DocumentFragment直接继承Node
typedef DomNode DomDocumentFragment;

struct _DomDocumentType;
typedef struct _DomDocumentType DomDocumentType;

struct _DomDocument;
typedef struct _DomDocument DomDocument;

//CDATASection直接继承Text
typedef DomText DomCDATASection;

struct _DomNotation;
typedef struct _DomNotation DomNotation;

struct _DomEntity;
typedef struct _DomEntity DomEntity;

//EntityReference直接继承Node
typedef DomNode DomEntityReference;

struct _DomProcessingInstruction;
typedef struct _DomProcessingInstruction DomProcessingInstruction;

//创建SvgDom
PUBLIC SvgDom* svg_dom_create();
//析构SvgDom
PUBLIC void svg_dom_destroy(SvgDom* thiz);
//获取SvgDom的Root成员
PUBLIC DomNode* svg_dom_get_root(SvgDom* thiz);
//设置SvgDom的Root成员
PUBLIC void svg_dom_set_root(SvgDom* thiz, DomNode* root);
//获取SvgDom的IdElementList成员
PUBLIC DList* svg_dom_get_id_element_list(SvgDom* thiz);
//根据元素Id从SvgDom中获取DomElement
PUBLIC DomElement* svg_dom_get_element_by_id(SvgDom* thiz, LPCTSTR id);


//
PUBLIC DomElement* dom_element_create(SvgToken* token);
PUBLIC DomText* dom_text_create(SvgToken* token);
PUBLIC DomDocument* dom_document_create();
PUBLIC DomProcessingInstruction* dom_processing_instruction_create(SvgToken* token);

PUBLIC DomNode* dom_comment_create(SvgToken* token);
PUBLIC void svg_dom_node_destroy(DomNode* thiz);

PUBLIC void dom_node_append_child(DomNode* thiz, DomNode* child);
//获取DomNode的NodeName成员
PUBLIC LPCTSTR dom_node_get_node_name(DomNode* thiz);
//获取DomNode的NodeType成员
PUBLIC DomNodeType dom_node_get_node_type(DomNode* thiz);
//获取DomNode的Attributes成员
PUBLIC DList* dom_node_get_attributes(DomNode* thiz);
//获取DomNode的ChildNodes成员
PUBLIC NodeList* dom_node_get_child_nodes(DomNode* thiz);
//获取DomNode的ParentNode成员
PUBLIC DomNode* dom_node_get_parent_node(DomNode* thiz);
//获取DomNode的NodeValue成员
PUBLIC LPCTSTR dom_node_get_node_value(DomNode* thiz);

//获取DomAttr的Name成员
PUBLIC LPCTSTR dom_attr_get_name(DomAttr* thiz);
//获取DomAttr的Value成员
PUBLIC LPCTSTR dom_attr_get_value(DomAttr* thiz);
//获取DomAttr的IsId成员
PUBLIC BOOL dom_attr_is_id(DomAttr* thiz);

//获取DomElement的Id成员
PUBLIC LPCTSTR dom_element_get_id(DomElement* thiz);

DECLS_END
#endif