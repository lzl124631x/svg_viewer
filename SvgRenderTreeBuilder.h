#ifndef SvgRenderTreeBuilder_h
#define SvgRenderTreeBuilder_h
//�˽ӿڸ���Dom��ת��ΪRender��
DECLS_BEGIN

#include "SvgDom.h"
#include "SvgRenderObject.h"

struct _SvgRenderTreeBuilder;
typedef struct _SvgRenderTreeBuilder SvgRenderTreeBuilder;

extern SvgRenderTree* g_render_tree;

//������Ⱦ��������
PUBLIC SvgRenderTreeBuilder* svg_render_tree_builder_create(SvgDom* thiz);
//������Ⱦ��������
PUBLIC void svg_render_tree_builder_destroy(SvgRenderTreeBuilder* thiz);
//��Ⱦ��������ͨ��Dom��������Ⱦ��
PUBLIC SvgRenderTree* svg_render_tree_builder_build(SvgRenderTreeBuilder* thiz);

DECLS_END
#endif