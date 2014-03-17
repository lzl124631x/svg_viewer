#ifndef SvgTokenizer_h
#define SvgTokenizer_h

DECLS_BEGIN

#include "SvgToken.h"

struct _SvgTokenizer;
typedef struct _SvgTokenizer SvgTokenizer;

//创建SvgTokenizer
PUBLIC SvgTokenizer* svg_tokenizer_create(LPCTSTR svg_content);
//析构SvgTokenizer
PUBLIC void svg_tokenizer_destroy(SvgTokenizer* thiz);
//符号化操作
PUBLIC BOOL svg_tokenizer_tokenize(SvgTokenizer* thiz);
//获取SvgTokenizer的Token成员
PUBLIC SvgToken* svg_tokenizer_get_token(SvgTokenizer* thiz);

DECLS_END
#endif