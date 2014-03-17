#ifndef SvgToken_h
#define SvgToken_h
DECLS_BEGIN

struct _SvgToken;
typedef struct _SvgToken SvgToken;

//SvgToken����������
typedef enum _SvgTokenType{
	TOKEN_UNINITIALIZED,//dataΪ0���������κ�����
	TOKEN_START_TAG,//dataΪStartTagToken��
	TOKEN_END_TAG,//dataֱ�ӵ���SvgTagType������Ϊָ�롣
	TOKEN_TEXT,//dataΪ0���������κ�����
	TOKEN_AFTER_EM,//dataΪ0���������κ�����
	TOKEN_AFTER_QM//dataΪ0���������κ�����
}SvgTokenType;

//����SvgToken
PUBLIC SvgToken* svg_token_create();
//����SvgToken
PUBLIC void svg_token_destroy(SvgToken* thiz);
//���SvgToken
PUBLIC void svg_token_clear(SvgToken* thiz);
//��ȡSvgToken��TokenType��Ա
PUBLIC SvgTokenType svg_token_get_token_type(SvgToken* thiz);
//��ȡSvgToken��SelfClosing��Ա
PUBLIC BOOL svg_token_get_self_closing(SvgToken* thiz);
//����SvgToken��SelfClosing��Ա
PUBLIC void svg_token_set_self_closing(SvgToken* thiz, BOOL selfClosing);
//��ȡSvgToken��Data��Ա
PUBLIC LPTSTR svg_token_get_data(SvgToken* thiz);
//����SvgToken��Data��Ա
PUBLIC void svg_token_set_data(SvgToken* thiz, LPCTSTR data);
//��ȡSvgToken��CurrentAttribute��Ա
PUBLIC ATTR* svg_token_get_current_attribute(SvgToken* thiz);
//��ȡSvgToken��Attributes��Ա
PUBLIC DList* svg_token_get_attributes(SvgToken* thiz);

//��ʼ�µĿ�ʼ��ǩ
PUBLIC void svg_token_begin_start_tag(SvgToken* thiz);
//��ʼ�µĽ�����ǩ
PUBLIC void svg_token_begin_end_tag(SvgToken* thiz);
//��ʼ�µ�����
PUBLIC INLINE void svg_token_begin_new_attribute(SvgToken* thiz);
//�����µ�����
PUBLIC INLINE void svg_token_end_new_attribute(SvgToken* thiz);
//ȷ����ǰSvgTokenΪ�ı���ǩ
PUBLIC void svg_token_ensure_is_text_token(SvgToken* thiz);

DECLS_END
#endif