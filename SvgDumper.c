#include "stdafx.h"
#include "SvgDumper.h"
/************************************************************************/
/*                              私有函数声明                            */
/************************************************************************/
PRIVATE void dom_node_dump_to_file(DomNode* node, FILE* fp, int indent);
/************************************************************************/
/*                              外部函数实现                            */
/************************************************************************/
PUBLIC void svg_dumper_dump_tree(SvgDom* svgDom){
	FILE* fp;
	return_if_fail(svgDom);

	_tfopen_s(&fp,_T("D:/output.txt"),_T("w"));//需要添加判定
	dom_node_dump_to_file(svg_dom_get_root(svgDom), fp, 0);
	fclose(fp);
}
/************************************************************************/
/*                              私有函数实现                            */
/************************************************************************/
PRIVATE void dom_node_dump_to_file(DomNode* node, FILE* fp, int indent){
	Iterator* domNodeIter = dlist_iterator_create(dom_node_get_child_nodes(node));
	DomNode* child;
	int i;
	return_if_fail(node);

	//打印当前节点
	if(dom_node_get_node_type(node) == DOM_NODE_TYPE_ELEMENT){
		Iterator* attr_iter = dlist_iterator_create(dom_node_get_attributes(node));
		ATTR* attr;

		for(i=0;i<indent;i++){
			_ftprintf_s(fp, _T(" "));
		}

		_ftprintf_s(fp, _T("<%s"), dom_node_get_node_name(node));
		while(attr = (ATTR*)attr_iter->get(attr_iter)){
			_ftprintf_s(fp, _T(" %s=\"%s\""), attr->name, attr->value);
			attr_iter->next(attr_iter);
		}
		_ftprintf_s(fp, _T(">"));
	}else if(dom_node_get_node_type(node) == DOM_NODE_TYPE_TEXT){
		_ftprintf_s(fp, _T("%s"), dom_node_get_node_value(node));
	}

	
	//打印子节点
	while(domNodeIter && (child = (DomNode*)domNodeIter->get(domNodeIter))){
		dom_node_dump_to_file(child, fp, indent + 4);
		domNodeIter->next(domNodeIter);
	}
	//打印结束符
	if(dom_node_get_node_type(node) == DOM_NODE_TYPE_ELEMENT){
		_ftprintf_s(fp, _T("</%s>"), dom_node_get_node_name(node));
	}
}