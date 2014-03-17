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
//����SvgDomBuilder�ṹ��
//�ڴ˺�����������SvgDomBuilder֮ǰ���ȼ�⴫��ָ���Ƿ�Ϊ0�����Ϊ0�򲻽�������ֱ�ӷ��ء�
//��ˣ��ڵ�����˺���֮����ؽ�SvgDomBuilder�������㣬��ֹҰָ�롣
PUBLIC void svg_dom_builder_destroy(SvgDomBuilder* thiz);
//
PUBLIC void svg_dom_builder_process_token(SvgDomBuilder* thiz,SvgToken* token);
//��ȡSvgDomBuilder��SvgDom��Ա
PUBLIC SvgDom* svg_dom_builder_get_svg_dom(SvgDomBuilder* thiz);

DECLS_END
#endif