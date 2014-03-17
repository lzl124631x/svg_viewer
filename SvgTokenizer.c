#include "stdafx.h"
#include "SvgTokenizer.h"
#include "Helper.h"

struct _SvgTokenizer{
	LPCTSTR read_ptr;//��¼��ǰ��ȡλ��
	LPCTSTR start;//��¼���ŵĿ�ʼλ��
	SvgToken* token;//����token��ʱ����
};

#define BEGIN_STATE(stateName) case stateName: stateName:
#define END_STATE() /*ASSERT_NOT_REACHED(); */break;
#define RECONSUME_IN(stateName)                                    \
	do {                                                                   \
	state = stateName;                                       \
	goto stateName;                                                    \
	} while (0)

#define ADVANCE_TO(stateName)                                      \
	do {                                                                   \
	state = stateName;                                       \
	thiz->read_ptr++;									\
	ch=*thiz->read_ptr;									 \
	goto stateName;                                                    \
	} while (0)
/************************************************************************/
/*                              ˽�к�������                            */
/************************************************************************/
/************************************************************************/
/*                              ���к���ʵ��                            */
/************************************************************************/
//��ʼ��SvgTokenizer
PUBLIC SvgTokenizer* svg_tokenizer_create(LPCTSTR svg_content){
	SvgTokenizer* thiz = (SvgTokenizer*)malloc(sizeof(SvgTokenizer));
	thiz->start = thiz->read_ptr = svg_content;
	thiz->token = svg_token_create();
	return thiz;
}

//��ΪSvgTokenizer��read_ptr��start����ָ��svg_content��ָ�룬����������free
//SvgTokenizer��data��Token����Ϣ����StartTagToken����Token����TokenToElement��ʱ���free����
PUBLIC void svg_tokenizer_destroy(SvgTokenizer* thiz){
	//�����Ҫ������token
	free(thiz);
}

//////////////////////////////////////////////////////////////////////////
PUBLIC BOOL svg_tokenizer_tokenize(SvgTokenizer* thiz){
	enum _State{
		DataState,
		CharacterReferenceInDataState,
		TagOpenState,
		EndTagOpenState,
		TagNameState,
		SelfClosingStartTagState,
		BeforeAttributeNameState,
		AttributeNameState,
		AfterAttributeNameState,
		BeforeAttributeValueState,
		AttributeValueDoubleQuotedState,
		AttributeValueUnquotedState,//���״̬��ָ���Բ���""��''��Χ��������&��ͷ����ʱû����
		AttributeValueSingleQuotedState,
		AfterAttributeValueQuotedState,
		CharacterReferenceInAttributeValueState,
		STATE_END_TAG,
		STATE_AFTER_EM,//Exclamation Mark
		STATE_AFTER_QM,//Question Mark
		STATE_END,
	}state = DataState;

	SvgToken* token = thiz->token;
	TCHAR ch = 0;
	thiz->start = thiz->read_ptr;

	for(; *thiz->read_ptr; thiz->read_ptr++){
		ch = *thiz->read_ptr;
		switch(state){
		BEGIN_STATE(DataState){
			if(ch == _T('&')){
				ADVANCE_TO(CharacterReferenceInDataState);
			}else if(ch == _T('<')){
				if(svg_token_get_token_type(token) == TOKEN_TEXT){
					svg_token_set_data(token, get_substring(thiz->start, 0, thiz->read_ptr - thiz->start));
					return TRUE;
				} 
				ADVANCE_TO(TagOpenState);
			}else if(ch == 0){
				//return emitEndOfFile(source);
				return FALSE;
			}else{
				//bufferCharacter(cc);
				//����������һ���ǽ�tokenType��ΪTEXT��һ���Ǵ洢�ַ���
				svg_token_ensure_is_text_token(token);
				ADVANCE_TO(DataState);
			}
		}
		END_STATE()

		BEGIN_STATE(CharacterReferenceInDataState) {
				//if (!processEntity(source))
				//	return haveBufferedCharacterToken();
				ADVANCE_TO(DataState);//SWITCH_TO
		}
		END_STATE()

		BEGIN_STATE(TagOpenState){
			if(ch == _T('!')){
				//state=STATE_AFTER_EM;
				//��ʱ����������<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">�ı�ǩ
				while(*thiz->read_ptr&&*thiz->read_ptr!=_T('>'))thiz->read_ptr++;
				ADVANCE_TO(DataState);
			}else if(ch == _T('/')){
				ADVANCE_TO(EndTagOpenState);
			}else if(isalpha(ch)){
				//m_token->svg_token_begin_start_tag(cc);
				svg_token_begin_start_tag(token);
				thiz->start = thiz->read_ptr;//��¼��ǩ������ʼ��
 				ADVANCE_TO(TagNameState);
			}else if(ch == _T('?')){
				//state=STATE_AFTER_QM;
				//��ʱ����������<?xml version='1.0' encoding='ISO-8859-1'?>�ı�ǩ
				while(*thiz->read_ptr && *thiz->read_ptr != _T('>')) thiz->read_ptr++;
				ADVANCE_TO(DataState);
			}else{
				state = DataState;
			}
		}
		END_STATE()

		BEGIN_STATE(TagNameState){
			if(IS_WSP(ch)){
				svg_token_set_data(token, get_substring(thiz->start,0,thiz->read_ptr - thiz->start));//������ǩ���Ķ�ȡ
				ADVANCE_TO(BeforeAttributeNameState);
			}else if(ch==_T('/')){
				svg_token_set_data(token, get_substring(thiz->start,0,thiz->read_ptr - thiz->start));//������ǩ���Ķ�ȡ
				ADVANCE_TO(SelfClosingStartTagState);
			}else if(ch==_T('>')){
				//return emitAndResumeIn(source, HTMLTokenizerState::DataState);
				svg_token_set_data(token, get_substring(thiz->start,0,thiz->read_ptr-thiz->start));//������ǩ���Ķ�ȡ
				thiz->read_ptr++;
				return TRUE;//////////////////////////////////////////////////////////////////////////
			}else if(isalpha(ch)){
				//����ǰch�ŵ�token��data��ȥ
				//m_token->appendToName(cc);
				ADVANCE_TO(TagNameState);
			}
		}
		END_STATE()

		BEGIN_STATE(BeforeAttributeNameState){
			if(IS_WSP(ch)){
				ADVANCE_TO(BeforeAttributeNameState);
			}else if(ch == _T('/')){
				ADVANCE_TO(SelfClosingStartTagState);
			}else if(ch == _T('>')){
				//return emitAndResumeIn(source, HTMLTokenizerState::DataState);
				return TRUE;//////////////////////////////////////////////////////////////////////////
			}else if(isalpha(ch)){
				//��������ӵ�token��attrs��ȥ
				//m_token->addNewAttribute();
				svg_token_begin_new_attribute(token);
				thiz->start=thiz->read_ptr;//��¼����������ʼ��
				ADVANCE_TO(AttributeNameState);
			}
		}
		END_STATE()

		BEGIN_STATE(AttributeNameState){
			if(IS_WSP(ch)){
				//����������
				svg_token_get_current_attribute(token)->name = get_substring(thiz->start, 0, thiz->read_ptr - thiz->start);//�����������Ķ�ȡ
				ADVANCE_TO(AfterAttributeNameState);
			}else if(ch==_T('/')){
				//����������
				ADVANCE_TO(SelfClosingStartTagState);
			}else if(ch==_T('=')){
				//����������
				svg_token_get_current_attribute(token)->name = get_substring(thiz->start, 0, thiz->read_ptr - thiz->start);//�����������Ķ�ȡ
				ADVANCE_TO(BeforeAttributeValueState);
			}else if(ch==_T('>')){
				//����������
				return TRUE;//////////////////////////////////////////////////////////////////////////
			}else if(isalpha(ch) || ch == _T(':') || ch == _T('-')|| isdigit(ch)){
				//�����Լ����Լ���һ�����ţ���Ϊ������xmlns:xlink='http://www.w3.org/1999/xlink'��ʽ������
				//�ּ��˸����ַ��ţ���Ϊ������stroke-linejoin='bevel'��ʽ������
				//�ּ������֣�����<line x1="0" y1="1.5" x2="300" y2="1.5" />

				//���chΪ"��\��<��Ӧ�ò���ParseError
				//��ch��ӵ���������ȥ
				ADVANCE_TO(AttributeNameState);
			}
		}
		END_STATE()

		BEGIN_STATE(AfterAttributeNameState){
			if(IS_WSP(ch)){
				ADVANCE_TO(AfterAttributeNameState);
			}else if(ch==_T('/')){
				ADVANCE_TO(SelfClosingStartTagState);
			}else if(ch==_T('=')){
				ADVANCE_TO(BeforeAttributeValueState);
			}else if(ch==_T('>')){
				return TRUE;//////////////////////////////////////////////////////////////////////////
			}else if(isalpha(ch)){
				//���chΪ"��\��<��Ӧ�ò���ParseError
				//��ch��ӵ���������ȥ
				ADVANCE_TO(AttributeNameState);
			}
		}
		END_STATE()

		BEGIN_STATE(BeforeAttributeValueState){
			if(IS_WSP(ch)){
				ADVANCE_TO(BeforeAttributeValueState);
			}else if(ch==_T('"')){
				//��ʼ�µ�attrvalue
				thiz->start=thiz->read_ptr+1;//��¼����ֵ����ʼ��
				ADVANCE_TO(AttributeValueDoubleQuotedState);
			}else if(ch==_T('&')){
				//��֪��ɶ��˼��״̬
				RECONSUME_IN(AttributeValueUnquotedState);
			}else if(ch==_T('\'')){
				//��ʼ�µ�attrvalue
				thiz->start=thiz->read_ptr+1;//��¼����ֵ����ʼ��
				ADVANCE_TO(AttributeValueSingleQuotedState);
			}else if(ch==_T('>')){
				//ParseError!!!
				return TRUE;
			}else{
				//���chΪ<��=��`��Ӧ�ò���ParseError
				//��ch��ӵ�����ֵ��ȥ
				thiz->start=thiz->read_ptr+1;//��¼����ֵ����ʼ��
				ADVANCE_TO(AttributeValueUnquotedState);
			}
		}
		END_STATE()

		BEGIN_STATE(AttributeValueDoubleQuotedState){
			if(ch==_T('"')){
				//��������ֵ
				svg_token_get_current_attribute(token)->value = get_substring(thiz->start,0,thiz->read_ptr-thiz->start);//��������ֵ�Ķ�ȡ
				svg_token_end_new_attribute(token);
				ADVANCE_TO(AfterAttributeValueQuotedState);
			} else if (ch == _T('&')) {
				//m_additionalAllowedCharacter = _T('"');
				ADVANCE_TO(CharacterReferenceInAttributeValueState);
			}else if(ch==0){
				//ParseError
				//��������ֵ
				RECONSUME_IN(DataState);
			}else{
				//��ch�ŵ�����ֵ��ȥ
				ADVANCE_TO(AttributeValueDoubleQuotedState);
			}
		}
		END_STATE()

		BEGIN_STATE(AttributeValueSingleQuotedState){
			if(ch==_T('\'')){
				//��������ֵ
				svg_token_get_current_attribute(token)->value = get_substring(thiz->start,0,thiz->read_ptr-thiz->start);//��������ֵ�Ķ�ȡ
				svg_token_end_new_attribute(token);
				ADVANCE_TO(AfterAttributeValueQuotedState);
			}else if(ch==_T('&')){
				//m_additionalAllowedCharacter = _T('\'');
				ADVANCE_TO(CharacterReferenceInAttributeValueState);
			}else if(ch==0){
				//ParseError
				//��������ֵ
				RECONSUME_IN(DataState);
			}else {
				//��ch�ŵ�����ֵ��ȥ
				ADVANCE_TO(AttributeValueSingleQuotedState);
			}
		}
		END_STATE()

		BEGIN_STATE(AttributeValueUnquotedState){
			//�Ȳ�д����֪�����Ǹ�ʲô״̬
		}
		END_STATE()

		BEGIN_STATE(CharacterReferenceInAttributeValueState){
			//�Ȳ�д����ʱ������CharacterReference
		}
		END_STATE()

		BEGIN_STATE(AfterAttributeValueQuotedState){
			if(IS_WSP(ch)){
				ADVANCE_TO(BeforeAttributeNameState);
			}else if(ch==_T('/')){
				ADVANCE_TO(SelfClosingStartTagState);
			}else if(ch==_T('>')){
				//return emitAndResumeIn(source, HTMLTokenizerState::DataState);
				thiz->read_ptr++;
				return TRUE;//////////////////////////////////////////////////////////////////////////
			}else if(ch==0){
				//ParseError
				RECONSUME_IN(DataState);
			}else {
				//ParseError
				ADVANCE_TO(BeforeAttributeNameState);
			}
		}
		END_STATE()

		BEGIN_STATE(SelfClosingStartTagState){
			if (ch == _T('>')) {
				//m_token->setSelfClosing();
				//return emitAndResumeIn(source, HTMLTokenizerState::DataState);
				svg_token_set_self_closing(token, TRUE);
				thiz->read_ptr++;
				return TRUE;
			} else if (ch == 0) {
				//parseError();
				state=DataState;
			} else {
				//parseError();
				state=BeforeAttributeNameState;
			}
		}
		END_STATE()

		BEGIN_STATE(EndTagOpenState){
			if (isalpha(ch)) {
				//m_token->svg_token_begin_end_tag(toLowerCase(cc));
				svg_token_begin_end_tag(token);
				thiz->start=thiz->read_ptr;//��¼������ǩ��ǩ������ʼ��
				ADVANCE_TO(TagNameState);
			} else if (ch == _T('>')) {
				//parseError();
				ADVANCE_TO(DataState);
			} else if (ch == 0) {
				//parseError();
				//bufferCharacter('<');
				//bufferCharacter(_T('/'));
				RECONSUME_IN(DataState);
			} else {
				//parseError();
				//HTML_RECONSUME_IN(BogusCommentState);
			}
		}
		END_STATE()

		default:break;
		}
	}
	return ch;
}

PUBLIC SvgToken* svg_tokenizer_get_token(SvgTokenizer* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->token;
}
/************************************************************************/
/*                              ˽�к���ʵ��                            */
/************************************************************************/
