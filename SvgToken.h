#ifndef SvgToken_h
#define SvgToken_h
DECLS_BEGIN

struct _SvgToken;
typedef struct _SvgToken SvgToken;

//SvgToken的所有类型
typedef enum _SvgTokenType{
	TOKEN_UNINITIALIZED,//data为0，不保存任何数据
	TOKEN_START_TAG,//data为StartTagToken。
	TOKEN_END_TAG,//data直接等于SvgTagType，不作为指针。
	TOKEN_TEXT,//data为0，不保存任何数据
	TOKEN_AFTER_EM,//data为0，不保存任何数据
	TOKEN_AFTER_QM//data为0，不保存任何数据
}SvgTokenType;

//创建SvgToken
PUBLIC SvgToken* svg_token_create();
//析构SvgToken
PUBLIC void svg_token_destroy(SvgToken* thiz);
//清空SvgToken
PUBLIC void svg_token_clear(SvgToken* thiz);
//获取SvgToken的TokenType成员
PUBLIC SvgTokenType svg_token_get_token_type(SvgToken* thiz);
//获取SvgToken的SelfClosing成员
PUBLIC BOOL svg_token_get_self_closing(SvgToken* thiz);
//设置SvgToken的SelfClosing成员
PUBLIC void svg_token_set_self_closing(SvgToken* thiz, BOOL selfClosing);
//获取SvgToken的Data成员
PUBLIC LPTSTR svg_token_get_data(SvgToken* thiz);
//设置SvgToken的Data成员
PUBLIC void svg_token_set_data(SvgToken* thiz, LPCTSTR data);
//获取SvgToken的CurrentAttribute成员
PUBLIC ATTR* svg_token_get_current_attribute(SvgToken* thiz);
//获取SvgToken的Attributes成员
PUBLIC DList* svg_token_get_attributes(SvgToken* thiz);

//开始新的开始标签
PUBLIC void svg_token_begin_start_tag(SvgToken* thiz);
//开始新的结束标签
PUBLIC void svg_token_begin_end_tag(SvgToken* thiz);
//开始新的属性
PUBLIC INLINE void svg_token_begin_new_attribute(SvgToken* thiz);
//结束新的属性
PUBLIC INLINE void svg_token_end_new_attribute(SvgToken* thiz);
//确保当前SvgToken为文本标签
PUBLIC void svg_token_ensure_is_text_token(SvgToken* thiz);

DECLS_END
#endif