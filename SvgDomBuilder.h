#ifndef SvgDomBuilder_h
#define SvgDomBuilder_h
DECLS_BEGIN

#include "SvgDom.h"
#include "SvgTokenizer.h"

struct _SvgDomBuilder;
typedef struct _SvgDomBuilder SvgDomBuilder;

typedef enum _InsertionMode{
	InSVGMode,
	InDefsMode,
}InsertionMode;

//
PUBLIC SvgDomBuilder* svg_dom_builder_create();
//析构SvgDomBuilder结构。
//在此函数尝试析构SvgDomBuilder之前会先检测传入指针是否为0，如果为0则不进行析构直接返回。
//因此，在调用完此函数之后，务必将SvgDomBuilder变量置零，防止野指针。
PUBLIC void svg_dom_builder_destroy(SvgDomBuilder* thiz);
//
PUBLIC void svg_dom_builder_process_token(SvgDomBuilder* thiz,SvgToken* token);
//获取SvgDomBuilder的SvgDom成员
PUBLIC SvgDom* svg_dom_builder_get_svg_dom(SvgDomBuilder* thiz);

DECLS_END
#endif