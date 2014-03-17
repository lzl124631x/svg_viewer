#ifndef SvgTokenizer_h
#define SvgTokenizer_h

DECLS_BEGIN

#include "SvgToken.h"

struct _SvgTokenizer;
typedef struct _SvgTokenizer SvgTokenizer;

//����SvgTokenizer
PUBLIC SvgTokenizer* svg_tokenizer_create(LPCTSTR svg_content);
//����SvgTokenizer
PUBLIC void svg_tokenizer_destroy(SvgTokenizer* thiz);
//���Ż�����
PUBLIC BOOL svg_tokenizer_tokenize(SvgTokenizer* thiz);
//��ȡSvgTokenizer��Token��Ա
PUBLIC SvgToken* svg_tokenizer_get_token(SvgTokenizer* thiz);

DECLS_END
#endif