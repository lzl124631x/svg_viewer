#ifndef Helper_h
#define Helper_h

#define PI 3.1415926f
// Conversion from radian to degree can be accomplished by multiplying the radian with DEGREE
// Conversion from degree to radian can be accomplished by dividing the degree with DEGREE
#define DEGREE (180 / PI)
DECLS_BEGIN

//帮助计算时间损耗
#include <time.h>

extern clock_t c_start, c_end;
extern float c_total;
#define SET_START c_start = clock()
#define SET_END do{ c_end = clock(); c_total += (float)(c_end - c_start) /CLOCKS_PER_SEC;}while(0)


//颜色名列表
typedef struct _ColorNotation{
		LPCTSTR mnemonic;
		COLORREF rgb;
}ColorNotation;
//添加extern表示这只是一个声明，若不添加会导致重复定义。
extern const ColorNotation Colors[];

extern const NUMBER unit_matrix[];

extern int dpi_x, dpi_y;

//判断ch是否为wsp，即空格、\t、\r和\n
#define IS_WSP(ch) (ch=='\x20'||ch=='\x9'||ch=='\xD'||ch=='\xA')
//滤过wsp：(#x20 | #x9 | #xD | #xA)
#define OMIT_WSP(str) while(IS_WSP(*str))str++
#define OMIT_NONE_WSP(str) while(*str && !IS_WSP(*str)) str++
//滤过comma-wsp：(wsp+ comma? wsp*) | (comma wsp*)
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

//从start位置截取字符串src的长度为len的子字符串
PUBLIC LPTSTR get_substring(LPCTSTR src, int start, int len);
//拷贝字符串
PUBLIC LPTSTR copy_string(LPCTSTR src);
//将字符串转化为REAL数值，modify_ptr指示是否要修改传入的指针
#define string_to_number (NUMBER)_tcstod
//将字符串转化为REAL数值以out_len输出
PUBLIC LENGTH string_to_length(LPTSTR str, LPTSTR* str_ptr);
#define string_to_coordinate string_to_length
//将str转化为COLORRREF并输出
PUBLIC COLORREF string_to_color(LPTSTR str);
//r×s的M1与s×t的M2相乘输出到r×t的Mout中去
PUBLIC void multiply_matrix(const NUMBER* M1, const NUMBER* M2, NUMBER *Mout, int r, int s, int t);
//将坐标<x,y,1>按照mtx(3*3)进行坐标转换
PUBLIC void transform(REAL_POINT* pPoint, const NUMBER *mtx);
//将链表转化为数组，使用浅复制
PUBLIC ARRAY dlist_to_array_shallow(DList* list, size_t size);
typedef void(*DEEP_COPY)(void* dst, void* src);
//将链表转化为数组，使用深复制
PUBLIC ARRAY dlist_to_array_deep(DList* list, size_t size, DEEP_COPY deep_copy);


DECLS_END
#endif//Helper_h