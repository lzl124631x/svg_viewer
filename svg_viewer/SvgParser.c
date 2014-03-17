#include "stdafx.h"
#include "SvgParser.h"
#ifdef DUMP_TREE
#include "SvgDumper.h"
#endif

/************************************************************************/
/*                              ˽�к�������                            */
/************************************************************************/
/************************************************************************/
/*                              �ⲿ����ʵ��                            */
/************************************************************************/
//����SVG�����ϻ�ȡ��һ��Token������ת��ΪElementȻ����ӵ�SVG DOM��ȥ
PUBLIC SvgDom* parse_svg(LPCTSTR svg_content){
	SvgToken* token;
	SvgTokenizer* tokenizer = svg_tokenizer_create(svg_content);
	SvgDomBuilder* dom_builder = svg_dom_builder_create();
	if(!svg_content){
		MessageBox(NULL,_T("Empty svg content!"),_T("Message"),MB_OK|MB_ICONEXCLAMATION);
		return NULL;
	}
	while(svg_tokenizer_tokenize(tokenizer)){
		token = svg_tokenizer_get_token(tokenizer);
		svg_dom_builder_process_token(dom_builder, token);
		svg_token_clear(token);
	}
	svg_tokenizer_destroy(tokenizer);
	g_svg_dom = svg_dom_builder_get_svg_dom(dom_builder);
#ifdef DUMP_TREE
	svg_dumper_dump_tree(g_svg_dom);
#endif
	svg_dom_builder_destroy(dom_builder);
	return g_svg_dom;
}
/************************************************************************/
/*                              ˽�к���ʵ��                            */
/************************************************************************/