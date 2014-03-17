#include "stdafx.h"
#include "SvgTokenizer.h"
#include "Helper.h"

struct _SvgTokenizer{
	LPCTSTR read_ptr;//记录当前读取位置
	LPCTSTR start;//记录符号的开始位置
	SvgToken* token;//保存token临时数据
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
/*                              私有函数声明                            */
/************************************************************************/
/************************************************************************/
/*                              公有函数实现                            */
/************************************************************************/
//初始化SvgTokenizer
PUBLIC SvgTokenizer* svg_tokenizer_create(LPCTSTR svg_content){
	SvgTokenizer* thiz = (SvgTokenizer*)malloc(sizeof(SvgTokenizer));
	thiz->start = thiz->read_ptr = svg_content;
	thiz->token = svg_token_create();
	return thiz;
}

//因为SvgTokenizer的read_ptr和start都是指向svg_content的指针，无需在这里free
//SvgTokenizer的data是Token的信息（如StartTagToken），Token会在TokenToElement的时候就free掉。
PUBLIC void svg_tokenizer_destroy(SvgTokenizer* thiz){
	//如果需要，析构token
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
		AttributeValueUnquotedState,//这个状态是指属性不被""或''包围，而是以&开头，暂时没见过
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
				//两个操作：一个是将tokenType置为TEXT，一个是存储字符。
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
				//暂时不解析形如<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">的标签
				while(*thiz->read_ptr&&*thiz->read_ptr!=_T('>'))thiz->read_ptr++;
				ADVANCE_TO(DataState);
			}else if(ch == _T('/')){
				ADVANCE_TO(EndTagOpenState);
			}else if(isalpha(ch)){
				//m_token->svg_token_begin_start_tag(cc);
				svg_token_begin_start_tag(token);
				thiz->start = thiz->read_ptr;//记录标签名的起始点
 				ADVANCE_TO(TagNameState);
			}else if(ch == _T('?')){
				//state=STATE_AFTER_QM;
				//暂时不解析形如<?xml version='1.0' encoding='ISO-8859-1'?>的标签
				while(*thiz->read_ptr && *thiz->read_ptr != _T('>')) thiz->read_ptr++;
				ADVANCE_TO(DataState);
			}else{
				state = DataState;
			}
		}
		END_STATE()

		BEGIN_STATE(TagNameState){
			if(IS_WSP(ch)){
				svg_token_set_data(token, get_substring(thiz->start,0,thiz->read_ptr - thiz->start));//结束标签名的读取
				ADVANCE_TO(BeforeAttributeNameState);
			}else if(ch==_T('/')){
				svg_token_set_data(token, get_substring(thiz->start,0,thiz->read_ptr - thiz->start));//结束标签名的读取
				ADVANCE_TO(SelfClosingStartTagState);
			}else if(ch==_T('>')){
				//return emitAndResumeIn(source, HTMLTokenizerState::DataState);
				svg_token_set_data(token, get_substring(thiz->start,0,thiz->read_ptr-thiz->start));//结束标签名的读取
				thiz->read_ptr++;
				return TRUE;//////////////////////////////////////////////////////////////////////////
			}else if(isalpha(ch)){
				//将当前ch放到token的data中去
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
				//将属性添加到token的attrs中去
				//m_token->addNewAttribute();
				svg_token_begin_new_attribute(token);
				thiz->start=thiz->read_ptr;//记录属性名的起始点
				ADVANCE_TO(AttributeNameState);
			}
		}
		END_STATE()

		BEGIN_STATE(AttributeNameState){
			if(IS_WSP(ch)){
				//结束属性名
				svg_token_get_current_attribute(token)->name = get_substring(thiz->start, 0, thiz->read_ptr - thiz->start);//结束属性名的读取
				ADVANCE_TO(AfterAttributeNameState);
			}else if(ch==_T('/')){
				//结束属性名
				ADVANCE_TO(SelfClosingStartTagState);
			}else if(ch==_T('=')){
				//结束属性名
				svg_token_get_current_attribute(token)->name = get_substring(thiz->start, 0, thiz->read_ptr - thiz->start);//结束属性名的读取
				ADVANCE_TO(BeforeAttributeValueState);
			}else if(ch==_T('>')){
				//结束属性名
				return TRUE;//////////////////////////////////////////////////////////////////////////
			}else if(isalpha(ch) || ch == _T(':') || ch == _T('-')|| isdigit(ch)){
				//这里自己妄自加了一个引号，因为遇到了xmlns:xlink='http://www.w3.org/1999/xlink'形式的属性
				//又加了个连字符号，因为遇到了stroke-linejoin='bevel'形式的属性
				//又加了数字，比如<line x1="0" y1="1.5" x2="300" y2="1.5" />

				//如果ch为"、\或<，应该产生ParseError
				//将ch添加到属性名中去
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
				//如果ch为"、\或<，应该产生ParseError
				//将ch添加到属性名中去
				ADVANCE_TO(AttributeNameState);
			}
		}
		END_STATE()

		BEGIN_STATE(BeforeAttributeValueState){
			if(IS_WSP(ch)){
				ADVANCE_TO(BeforeAttributeValueState);
			}else if(ch==_T('"')){
				//开始新的attrvalue
				thiz->start=thiz->read_ptr+1;//记录属性值的起始点
				ADVANCE_TO(AttributeValueDoubleQuotedState);
			}else if(ch==_T('&')){
				//不知道啥意思的状态
				RECONSUME_IN(AttributeValueUnquotedState);
			}else if(ch==_T('\'')){
				//开始新的attrvalue
				thiz->start=thiz->read_ptr+1;//记录属性值的起始点
				ADVANCE_TO(AttributeValueSingleQuotedState);
			}else if(ch==_T('>')){
				//ParseError!!!
				return TRUE;
			}else{
				//如果ch为<、=或`，应该产生ParseError
				//将ch添加到属性值中去
				thiz->start=thiz->read_ptr+1;//记录属性值的起始点
				ADVANCE_TO(AttributeValueUnquotedState);
			}
		}
		END_STATE()

		BEGIN_STATE(AttributeValueDoubleQuotedState){
			if(ch==_T('"')){
				//结束属性值
				svg_token_get_current_attribute(token)->value = get_substring(thiz->start,0,thiz->read_ptr-thiz->start);//结束属性值的读取
				svg_token_end_new_attribute(token);
				ADVANCE_TO(AfterAttributeValueQuotedState);
			} else if (ch == _T('&')) {
				//m_additionalAllowedCharacter = _T('"');
				ADVANCE_TO(CharacterReferenceInAttributeValueState);
			}else if(ch==0){
				//ParseError
				//结束属性值
				RECONSUME_IN(DataState);
			}else{
				//将ch放到属性值中去
				ADVANCE_TO(AttributeValueDoubleQuotedState);
			}
		}
		END_STATE()

		BEGIN_STATE(AttributeValueSingleQuotedState){
			if(ch==_T('\'')){
				//结束属性值
				svg_token_get_current_attribute(token)->value = get_substring(thiz->start,0,thiz->read_ptr-thiz->start);//结束属性值的读取
				svg_token_end_new_attribute(token);
				ADVANCE_TO(AfterAttributeValueQuotedState);
			}else if(ch==_T('&')){
				//m_additionalAllowedCharacter = _T('\'');
				ADVANCE_TO(CharacterReferenceInAttributeValueState);
			}else if(ch==0){
				//ParseError
				//结束属性值
				RECONSUME_IN(DataState);
			}else {
				//将ch放到属性值中去
				ADVANCE_TO(AttributeValueSingleQuotedState);
			}
		}
		END_STATE()

		BEGIN_STATE(AttributeValueUnquotedState){
			//先不写，不知道这是个什么状态
		}
		END_STATE()

		BEGIN_STATE(CharacterReferenceInAttributeValueState){
			//先不写，暂时不考虑CharacterReference
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
				thiz->start=thiz->read_ptr;//记录结束标签标签名的起始点
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
/*                              私有函数实现                            */
/************************************************************************/
