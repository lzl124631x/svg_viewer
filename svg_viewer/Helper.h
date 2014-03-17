#ifndef Helper_h
#define Helper_h

#define PI 3.1415926f
// Conversion from radian to degree can be accomplished by multiplying the radian with DEGREE
// Conversion from degree to radian can be accomplished by dividing the degree with DEGREE
#define DEGREE (180 / PI)
DECLS_BEGIN

//��������ʱ�����
#include <time.h>

extern clock_t c_start, c_end;
extern float c_total;
#define SET_START c_start = clock()
#define SET_END do{ c_end = clock(); c_total += (float)(c_end - c_start) /CLOCKS_PER_SEC;}while(0)


//��ɫ���б�
typedef struct _ColorNotation{
		LPCTSTR mnemonic;
		COLORREF rgb;
}ColorNotation;
//���extern��ʾ��ֻ��һ��������������ӻᵼ���ظ����塣
extern const ColorNotation Colors[];

extern const NUMBER unit_matrix[];

extern int dpi_x, dpi_y;

//�ж�ch�Ƿ�Ϊwsp�����ո�\t��\r��\n
#define IS_WSP(ch) (ch=='\x20'||ch=='\x9'||ch=='\xD'||ch=='\xA')
//�˹�wsp��(#x20 | #x9 | #xD | #xA)
#define OMIT_WSP(str) while(IS_WSP(*str))str++
#define OMIT_NONE_WSP(str) while(*str && !IS_WSP(*str)) str++
//�˹�comma-wsp��(wsp+ comma? wsp*) | (comma wsp*)
#define OMIT_COMMA_WSP(str)\
	do{\
	OMIT_WSP(str);\
	if(*str == ',') str++;\
	OMIT_WSP(str);\
	}while(0)

#define OMIT_DIGIT(str) while(*str && *str >= _T('0') && *str <= _T('9')) str++
#define OMIT_NUMBER(str)\
	do{\
		if(*str == _T('-')) str++;\
		OMIT_DIGIT(str);\
		if(*str == _T('.')){\
			str++;\
			OMIT_DIGIT(str);\
		}\
	}while(0)

//��startλ�ý�ȡ�ַ���src�ĳ���Ϊlen�����ַ���
PUBLIC LPTSTR get_substring(LPCTSTR src, int start, int len);
//�����ַ���
PUBLIC LPTSTR copy_string(LPCTSTR src);
//���ַ���ת��ΪREAL��ֵ��modify_ptrָʾ�Ƿ�Ҫ�޸Ĵ����ָ��
#define string_to_number (NUMBER)_tcstod
//���ַ���ת��ΪREAL��ֵ��out_len���
PUBLIC LENGTH string_to_length(LPTSTR str, LPTSTR* str_ptr);
#define string_to_coordinate string_to_length
//��strת��ΪCOLORRREF�����
PUBLIC COLORREF string_to_color(LPTSTR str);
//r��s��M1��s��t��M2��������r��t��Mout��ȥ
PUBLIC void multiply_matrix(const NUMBER* M1, const NUMBER* M2, NUMBER *Mout, int r, int s, int t);
//������<x,y,1>����mtx(3*3)��������ת��
PUBLIC void transform(REAL_POINT* pPoint, const NUMBER *mtx);
//������ת��Ϊ���飬ʹ��ǳ����
PUBLIC ARRAY dlist_to_array_shallow(DList* list, size_t size);
typedef void(*DEEP_COPY)(void* dst, void* src);
//������ת��Ϊ���飬ʹ�����
PUBLIC ARRAY dlist_to_array_deep(DList* list, size_t size, DEEP_COPY deep_copy);


DECLS_END
#endif//Helper_h