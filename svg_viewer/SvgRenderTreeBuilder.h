#ifndef SvgRenderTreeBuilder_h
#define SvgRenderTreeBuilder_h
//此接口负责将Dom树转化为Render树
DECLS_BEGIN

#include "SvgDom.h"
#include "SvgRenderObject.h"

struct _SvgRenderTreeBuilder;
typedef struct _SvgRenderTreeBuilder SvgRenderTreeBuilder;

extern SvgRenderTree* g_render_tree;

//创建渲染树生成器
PUBLIC SvgRenderTreeBuilder* svg_render_tree_builder_create(SvgDom* thiz);
//析构渲染树生成器
PUBLIC void svg_render_tree_builder_destroy(SvgRenderTreeBuilder* thiz);
//渲染树生成器通过Dom树生成渲染树
PUBLIC SvgRenderTree* svg_render_tree_builder_build(SvgRenderTreeBuilder* thiz);

DECLS_END
#endif