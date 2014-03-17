#include "stdafx.h"
#include "SvgRenderTreeBuilder.h"
#include "DList.h"

struct _SvgRenderTreeBuilder{
	SvgDom* svg_dom;
	SvgRenderObject* current;
	SvgRenderTree* render_tree;
};

SvgRenderTree* g_render_tree;
/************************************************************************/
/*                              ˽�к�������                            */
/************************************************************************/
PRIVATE void svg_render_tree_builder_build_on_node(SvgRenderTreeBuilder* thiz, DomNode* node);
/************************************************************************/
/*                              �ⲿ����ʵ��                            */
/************************************************************************/
PUBLIC SvgRenderTreeBuilder* svg_render_tree_builder_create(SvgDom* svgDom){
	SvgRenderTreeBuilder* thiz;
	return_val_if_fail(svgDom, NULL);
	thiz = (SvgRenderTreeBuilder*)calloc(1, sizeof(SvgRenderTreeBuilder));
	thiz->svg_dom = svgDom;
	thiz->render_tree = svg_render_tree_create();
	thiz->current = NULL;
	return thiz;
}

PUBLIC void svg_render_tree_builder_destroy(SvgRenderTreeBuilder* thiz){
	return_if_fail(thiz);
	free(thiz);
}

PUBLIC SvgRenderTree* svg_render_tree_builder_build(SvgRenderTreeBuilder* thiz){
	return_val_if_fail(thiz, NULL);
	c_total = 0;
	svg_render_tree_builder_build_on_node(thiz, svg_dom_get_root(thiz->svg_dom));//���������⣬�ֻ�ȡ��һ�θ��ڵ�
	return thiz->render_tree;
}

/************************************************************************/
/*                              ˽�к���ʵ��                            */
/************************************************************************/

PRIVATE void svg_render_tree_builder_build_on_node(SvgRenderTreeBuilder* thiz, DomNode* dom_node){
	Iterator* dom_node_iter = dlist_iterator_create(dom_node_get_child_nodes(dom_node));
	DomNode* child = NULL;
	SvgRenderObject* render_object = NULL;
	BOOL render_child = TRUE;
	BOOL is_use_node = FALSE;
	return_if_fail(thiz && dom_node);

	//������ǰDom�ڵ��RenderԪ��
	if(dom_node_get_node_type(dom_node) == DOM_NODE_TYPE_ELEMENT){
		SvgTagType type = string_to_svg_render_object_type(dom_node_get_node_name(dom_node));
		render_object = svg_render_object_factory_create_render_object(dom_node, thiz->current);
		switch(type){
		case SVG_TAG_TYPE_PATH:{ 
			LPCTSTR clip_path = svg_render_object_get_clip_path_name(render_object);
			render_child = FALSE;
			if(clip_path){
				DomElement* element = svg_dom_get_element_by_id(thiz->svg_dom, clip_path + 1);
				if(element){
					SvgClipPathRenderObject* clip_path = svg_clip_path_render_object_create((DomNode*)element);
					Iterator* clip_child_iter = dlist_iterator_create(dom_node_get_child_nodes((DomNode*)element));
					DomNode* clip_child = NULL;
					svg_render_object_set_clip_path_element(render_object, clip_path);
					//NOTE:Ŀǰpath��clip_path��target��SvgClipPathRenderObject
					while(clip_child_iter && (clip_child = (DomNode*)iterator_get(clip_child_iter))){
						svg_render_object_append_child((SvgRenderObject*)clip_path, 
							svg_render_object_factory_create_render_object(clip_child, (SvgRenderObject*)clip_path));
						iterator_next(clip_child_iter);
					}
					iterator_destroy(clip_child_iter);
				}
			}
			break;
							   }
		case SVG_TAG_TYPE_RECT: render_child = FALSE; break;
		case SVG_TAG_TYPE_CIRCLE: render_child = FALSE; break;
		case SVG_TAG_TYPE_ELLIPSE: render_child = FALSE; break;
		case SVG_TAG_TYPE_LINE: render_child = FALSE; break;
		case SVG_TAG_TYPE_POLYLINE: render_child = FALSE; break;
		case SVG_TAG_TYPE_POLYGON: render_child = FALSE; break;

		case SVG_TAG_TYPE_SVG: break;
		case SVG_TAG_TYPE_G: break;
		case SVG_TAG_TYPE_USE:{
			LPCTSTR xlink_href = svg_use_render_object_get_xlink_href_name((SvgUseRenderObject*)render_object);
			if(xlink_href){//��һ���ǿ��ǵ�����������е�useԪ�ز�û��xlink:href���ԣ�xlink_hrefΪNULL
				svg_use_render_object_set_xlink_href_element((SvgUseRenderObject*)render_object,
					svg_dom_get_element_by_id(thiz->svg_dom, xlink_href + 1));//+1��Ϊ������#
			}
			is_use_node = TRUE;
			render_child = FALSE;
			break;
							  }
		case SVG_TAG_TYPE_TITLE: render_child = FALSE;break;
		case SVG_TAG_TYPE_DESC: render_child = FALSE;break;
		case SVG_TAG_TYPE_DEFS: render_child = FALSE; break;
		case SVG_TAG_TYPE_CLIP_PATH: render_child = FALSE; break;
		}
	}
	if(render_object != NULL){
		if(thiz->current){//FIXME:���������Ϊ��Ⱦ������һ�����ڵ㣬��������ÿ�β���ڵ��ʱ���ж�
			svg_render_object_append_child(thiz->current, (SvgRenderObject*)render_object);
		}else{
			svg_render_tree_set_root(thiz->render_tree, (SvgRenderObject*)render_object);
		}
		thiz->current = (SvgRenderObject*)render_object;
		if(is_use_node){
			svg_render_tree_builder_build_on_node(thiz, 
				(DomNode*)svg_use_render_object_get_xlink_href_element((SvgUseRenderObject*)render_object));
		}
	}
	//������Dom�ڵ��RenderԪ��
	while(render_child && dom_node_iter && (child = (DomNode*)iterator_get(dom_node_iter))){
		svg_render_tree_builder_build_on_node(thiz, child);
		iterator_next(dom_node_iter);
	}
	if(render_object != NULL){
		thiz->current = svg_render_object_get_parent((SvgRenderObject*)render_object);
	}
	iterator_destroy(dom_node_iter);
}