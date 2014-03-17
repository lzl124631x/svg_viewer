/*
 * File:    DList.h
 * Author:  Liu Zhenglai <lzl124631x@163.com>
 * Date:	2012-12-05
 * Brief:   Doubly Linked List Header File.
 * Copyright (c) Liu Zhenglai
 */

#include "stdafx.h"
#include "Helper.h"
#include <math.h>

clock_t c_start, c_end;
float c_total;

const ColorNotation Colors[147]={
	{_T("ALICEBLUE"),	RGB(240,248,255)},//AliceBlue
	{_T("ANTIQUEWHITE"),	RGB(250,235,215)},//AntiqueWhite
	{_T("AQUA"),	RGB(0,255,255)},//Aqua
	{_T("AQUAMARINE"),	RGB(127,255,212)},//Aquamarine
	{_T("AZURE"),	RGB(240,255,255)},//Azure
	{_T("BEIGE"),	RGB(245,245,220)},//Beige
	{_T("BISQUE"),	RGB(255,228,196)},//Bisque
	{_T("BLACK"),	RGB(0,0,0)},//Black
	{_T("BLANCHEDALMOND"),	RGB(255,235,205)},//BlanchedAlmond
	{_T("BLUE"),	RGB(0,0,255)},//Blue
	{_T("BLUEVIOLET"),	RGB(138,43,226)},//BlueViolet
	{_T("BROWN"),	RGB(165,42,42)},//Brown
	{_T("BURLYWOOD"),	RGB(222,184,135)},//BurlyWood
	{_T("CADETBLUE"),	RGB(95,158,160)},//CadetBlue
	{_T("CHARTREUSE"),	RGB(127,255,0)},//Chartreuse
	{_T("CHOCOLATE"),	RGB(210,105,30)},//Chocolate
	{_T("CORAL"),	RGB(255,127,80)},//Coral
	{_T("CORNFLOWERBLUE"),	RGB(100,149,237)},//CornflowerBlue
	{_T("CORNSILK"),	RGB(255,248,220)},//Cornsilk
	{_T("CRIMSON"),	RGB(237,164,61)},//Crimson
	{_T("CYAN"),	RGB(0,255,255)},//Cyan
	{_T("DARKBLUE"),	RGB(0,0,139)},//DarkBlue
	{_T("DARKCYAN"),	RGB(0,139,139)},//DarkCyan
	{_T("DARKGOLDENROD"),	RGB(184,134,11)},//DarkGoldenRod
	{_T("DARKGRAY"),	RGB(169,169,169)},//DarkGray
	{_T("DARKGREY"),	RGB(169,169,169)},//DarkGrey
	{_T("DARKGREEN"),	RGB(0,100,0)},//DarkGreen
	{_T("DARKKHAKI"),	RGB(189,183,107)},//DarkKhaki
	{_T("DARKMAGENTA"),	RGB(139,0,139)},//DarkMagenta
	{_T("DARKOLIVEGREEN"),	RGB(85,107,47)},//DarkOliveGreen
	{_T("DARKORANGE"),	RGB(255,140,0)},//Darkorange
	{_T("DARKORCHID"),	RGB(153,50,204)},//DarkOrchid
	{_T("DARKRED"),	RGB(139,0,0)},//DarkRed
	{_T("DARKSALMON"),	RGB(233,150,122)},//DarkSalmon
	{_T("DARKSEAGREEN"),	RGB(143,188,143)},//DarkSeaGreen
	{_T("DARKSLATEBLUE"),	RGB(72,61,139)},//DarkSlateBlue
	{_T("DARKSLATEGRAY"),	RGB(47,79,79)},//DarkSlateGray
	{_T("DARKSLATEGREY"),	RGB(47,79,79)},//DarkSlateGrey
	{_T("DARKTURQUOISE"),	RGB(0,206,209)},//DarkTurquoise
	{_T("DARKVIOLET"),	RGB(148,0,211)},//DarkViolet
	{_T("DEEPPINK"),	RGB(255,20,147)},//DeepPink
	{_T("DEEPSKYBLUE"),	RGB(0,191,255)},//DeepSkyBlue
	{_T("DIMGRAY"),	RGB(105,105,105)},//DimGray
	{_T("DIMGREY"),	RGB(105,105,105)},//DimGrey
	{_T("DODGERBLUE"),	RGB(30,144,255)},//DodgerBlue
	{_T("FIREBRICK"),	RGB(178,34,34)},//FireBrick
	{_T("FLORALWHITE"),	RGB(255,250,240)},//FloralWhite
	{_T("FORESTGREEN"),	RGB(34,139,34)},//ForestGreen
	{_T("FUCHSIA"),	RGB(255,0,255)},//Fuchsia
	{_T("GAINSBORO"),	RGB(220,220,220)},//Gainsboro
	{_T("GHOSTWHITE"),	RGB(248,248,255)},//GhostWhite
	{_T("GOLD"),	RGB(255,215,0)},//Gold
	{_T("GOLDENROD"),	RGB(218,165,32)},//GoldenRod
	{_T("GRAY"),	RGB(128,128,128)},//Gray
	{_T("GREY"),	RGB(128,128,128)},//Grey
	{_T("GREEN"),	RGB(0,128,0)},//Green
	{_T("GREENYELLOW"),	RGB(173,255,47)},//GreenYellow
	{_T("HONEYDEW"),	RGB(240,255,240)},//HoneyDew
	{_T("HOTPINK"),	RGB(255,105,180)},//HotPink
	{_T("INDIANRED"),	RGB(205,92,92)},//IndianRed
	{_T("INDIGO"),	RGB(75,0,130)},//Indigo
	{_T("IVORY"),	RGB(255,255,240)},//Ivory
	{_T("KHAKI"),	RGB(240,230,140)},//Khaki
	{_T("LAVENDER"),	RGB(230,230,250)},//Lavender
	{_T("LAVENDERBLUSH"),	RGB(255,240,245)},//LavenderBlush
	{_T("LAWNGREEN"),	RGB(124,252,0)},//LawnGreen
	{_T("LEMONCHIFFON"),	RGB(255,250,205)},//LemonChiffon
	{_T("LIGHTBLUE"),	RGB(173,216,230)},//LightBlue
	{_T("LIGHTCORAL"),	RGB(240,128,128)},//LightCoral
	{_T("LIGHTCYAN"),	RGB(224,255,255)},//LightCyan
	{_T("LIGHTGOLDENRODYELLOW"),	RGB(250,250,210)},//LightGoldenRodYellow
	{_T("LIGHTGRAY"),	RGB(211,211,211)},//LightGray
	{_T("LIGHTGREY"),	RGB(211,211,211)},//LightGrey
	{_T("LIGHTGREEN"),	RGB(144,238,144)},//LightGreen
	{_T("LIGHTPINK"),	RGB(255,182,193)},//LightPink
	{_T("LIGHTSALMON"),	RGB(255,160,122)},//LightSalmon
	{_T("LIGHTSEAGREEN"),	RGB(32,178,170)},//LightSeaGreen
	{_T("LIGHTSKYBLUE"),	RGB(135,206,250)},//LightSkyBlue
	{_T("LIGHTSLATEGRAY"),	RGB(119,136,153)},//LightSlateGray
	{_T("LIGHTSLATEGREY"),	RGB(119,136,153)},//LightSlateGrey
	{_T("LIGHTSTEELBLUE"),	RGB(176,196,222)},//LightSteelBlue
	{_T("LIGHTYELLOW"),	RGB(255,255,224)},//LightYellow
	{_T("LIME"),	RGB(0,255,0)},//Lime
	{_T("LIMEGREEN"),	RGB(50,205,50)},//LimeGreen
	{_T("LINEN"),	RGB(250,240,230)},//Linen
	{_T("MAGENTA"),	RGB(255,0,255)},//Magenta
	{_T("MAROON"),	RGB(128,0,0)},//Maroon
	{_T("MEDIUMAQUAMARINE"),	RGB(102,205,170)},//MediumAquaMarine
	{_T("MEDIUMBLUE"),	RGB(0,0,205)},//MediumBlue
	{_T("MEDIUMORCHID"),	RGB(186,85,211)},//MediumOrchid
	{_T("MEDIUMPURPLE"),	RGB(147,112,219)},//MediumPurple
	{_T("MEDIUMSEAGREEN"),	RGB(60,179,113)},//MediumSeaGreen
	{_T("MEDIUMSLATEBLUE"),	RGB(123,104,238)},//MediumSlateBlue
	{_T("MEDIUMSPRINGGREEN"),	RGB(0,250,154)},//MediumSpringGreen
	{_T("MEDIUMTURQUOISE"),	RGB(72,209,204)},//MediumTurquoise
	{_T("MEDIUMVIOLETRED"),	RGB(199,21,133)},//MediumVioletRed
	{_T("MIDNIGHTBLUE"),	RGB(25,25,112)},//MidnightBlue
	{_T("MINTCREAM"),	RGB(245,255,250)},//MintCream
	{_T("MISTYROSE"),	RGB(255,228,225)},//MistyRose
	{_T("MOCCASIN"),	RGB(255,228,181)},//Moccasin
	{_T("NAVAJOWHITE"),	RGB(255,222,173)},//NavajoWhite
	{_T("NAVY"),	RGB(0,0,128)},//Navy
	{_T("OLDLACE"),	RGB(253,245,230)},//OldLace
	{_T("OLIVE"),	RGB(128,128,0)},//Olive
	{_T("OLIVEDRAB"),	RGB(107,142,35)},//OliveDrab
	{_T("ORANGE"),	RGB(255,165,0)},//Orange
	{_T("ORANGERED"),	RGB(255,69,0)},//OrangeRed
	{_T("ORCHID"),	RGB(218,112,214)},//Orchid
	{_T("PALEGOLDENROD"),	RGB(238,232,170)},//PaleGoldenRod
	{_T("PALEGREEN"),	RGB(152,251,152)},//PaleGreen
	{_T("PALETURQUOISE"),	RGB(175,238,238)},//PaleTurquoise
	{_T("PALEVIOLETRED"),	RGB(219,112,147)},//PaleVioletRed
	{_T("PAPAYAWHIP"),	RGB(255,239,213)},//PapayaWhip
	{_T("PEACHPUFF"),	RGB(255,218,185)},//PeachPuff
	{_T("PERU"),	RGB(205,133,63)},//Peru
	{_T("PINK"),	RGB(255,192,203)},//Pink
	{_T("PLUM"),	RGB(221,160,221)},//Plum
	{_T("POWDERBLUE"),	RGB(176,224,230)},//PowderBlue
	{_T("PURPLE"),	RGB(128,0,128)},//Purple
	{_T("RED"),	RGB(255,0,0)},//Red
	{_T("ROSYBROWN"),	RGB(188,143,143)},//RosyBrown
	{_T("ROYALBLUE"),	RGB(65,105,225)},//RoyalBlue
	{_T("SADDLEBROWN"),	RGB(139,69,19)},//SaddleBrown
	{_T("SALMON"),	RGB(250,128,114)},//Salmon
	{_T("SANDYBROWN"),	RGB(244,164,96)},//SandyBrown
	{_T("SEAGREEN"),	RGB(46,139,87)},//SeaGreen
	{_T("SEASHELL"),	RGB(255,245,238)},//SeaShell
	{_T("SIENNA"),	RGB(160,82,45)},//Sienna
	{_T("SILVER"),	RGB(192,192,192)},//Silver
	{_T("SKYBLUE"),	RGB(135,206,235)},//SkyBlue
	{_T("SLATEBLUE"),	RGB(106,90,205)},//SlateBlue
	{_T("SLATEGRAY"),	RGB(112,128,144)},//SlateGray
	{_T("SLATEGREY"),	RGB(112,128,144)},//SlateGrey
	{_T("SNOW"),	RGB(255,250,250)},//Snow
	{_T("SPRINGGREEN"),	RGB(0,255,127)},//SpringGreen
	{_T("STEELBLUE"),	RGB(70,130,180)},//SteelBlue
	{_T("TAN"),	RGB(210,180,140)},//Tan
	{_T("TEAL"),	RGB(0,128,128)},//Teal
	{_T("THISTLE"),	RGB(216,191,216)},//Thistle
	{_T("TOMATO"),	RGB(255,99,71)},//Tomato
	{_T("TURQUOISE"),	RGB(64,224,208)},//Turquoise
	{_T("VIOLET"),	RGB(238,130,238)},//Violet
	{_T("WHEAT"),	RGB(245,222,179)},//Wheat
	{_T("WHITE"),	RGB(255,255,255)},//White
	{_T("WHITESMOKE"),	RGB(245,245,245)},//WhiteSmoke
	{_T("YELLOW"),	RGB(255,255,0)},//Yellow
	{_T("YELLOWGREEN"),	RGB(154,205,50)},//YellowGreen
};

const NUMBER unit_matrix[] ={
	1.0f, 0, 0,
	0, 1.0f, 0,
	0, 0, 1.0f
};

int dpi_x, dpi_y;

PUBLIC LPTSTR get_substring(LPCTSTR src, int start, int len){
	int i;
	LPTSTR dest = (LPTSTR)malloc(sizeof(TCHAR)*(len + 1));
	src += start;
	for(i = 0; i < len && src[i]; i++){
		dest[i] = src[i];
	}
	dest[i] = 0;
	return dest;
}

PUBLIC LPTSTR copy_string(LPCTSTR src){
	int srclen = (int)_tcslen(src),i;
	LPTSTR dest = (LPTSTR)malloc(sizeof(TCHAR)*(srclen + 1));
	for(i = 0; i < srclen; i++){
		dest[i] = src[i];
	}
	dest[i] = 0;
	return dest;
}

PUBLIC LENGTH string_to_length(LPTSTR str, LPTSTR* str_ptr){
	LENGTH out_len = {0};
	NUMBER unit_ratio = 0.0;
	LPCTSTR unit_id;
	out_len.number = string_to_number(str, &str);
	out_len.unit_id = UNIT_ID_PX; // set px as default
	unit_id = str;
	OMIT_NONE_WSP(str);
	if(str_ptr) *str_ptr = str;

	switch(str - unit_id){
	case 0: break; //Unit identifier is not provided, set it as px
	case 1: 
		if(*unit_id == _T('%')){//percentage
			out_len.number /= 100;
			out_len.unit_id = UNIT_ID_PERCENTAGE;
		}
		break;
	case 2:
		//FIXME：em和ex还未处理
		if(!_tcscmp(unit_id, _T("px"))){//pixel
			;
		}else if(!_tcscmp(unit_id, _T("pt"))){//point
			out_len.number *= dpi_x / 72.0f;
		}else if(!_tcscmp(unit_id, _T("pc"))){//pica
			out_len.number *= dpi_x / 6.0f;
		}else if(!_tcscmp(unit_id, _T("cm"))){//centimeter
			out_len.number *= dpi_x / 2.54f;
		}else if(!_tcscmp(unit_id, _T("mm"))){//millimeter
			out_len.number *= dpi_x / 25.4f;
		}else if(!_tcscmp(unit_id, _T("in"))){//inch
			out_len.number *= dpi_x;
		}else{//invalid unit identifier
			out_len.number = 0.0f;
		}
		break;
	default://invalid unit identifier
		out_len.number = 0.0f;
		break;
	}
	return out_len;
}

//string_to_color的辅助函数，将字符转化为数字
PRIVATE int HexDigit(TCHAR ch){
	if (ch >= _T('0') && ch <= _T('9'))
		return ch - _T('0');
	else if (ch >= _T('A') && ch <= _T('F'))
		return ch - _T('A') + 10;
	else if (ch >= _T('a') && ch <= _T('f'))
		return ch - _T('a') + 10;
	return 0;
}

//将颜色属性翻译为颜色
/*************************************************************
css颜色有四种表现形式：
单位				描述
(颜色名)			颜色名称 (比如 red)
rgb(x,x,x)			RGB 值 (比如 rgb(255,0,0))
rgb(x%, x%, x%)		RGB 百分比值 (比如 rgb(100%,0%,0%))
#rrggbb				十六进制数 (比如 #ff0000)
*************************************************************/
PUBLIC COLORREF string_to_color(LPTSTR str){
	int red, green, blue, i, len;
	red = green = blue = 0;
	if (*str == '#'){//十六进制数
		str++;
		if(str[6] == '\"'||_tcslen(str) == 6){//六位模式"#000000"
			red   = (HexDigit(str[0]) << 4) | HexDigit(str[1]);
			green = (HexDigit(str[2]) << 4) | HexDigit(str[3]);
			blue  = (HexDigit(str[4]) << 4) | HexDigit(str[5]);
		}else if(str[3] == _T('"')|| _tcslen(str) == 3){//三位模式"#000"
			red   = (HexDigit(str[0]) << 4) | HexDigit(str[0]);
			green = (HexDigit(str[1]) << 4) | HexDigit(str[1]);
			blue  = (HexDigit(str[2]) << 4) | HexDigit(str[2]);
		}
	}else if(_tcsncmp(str,_T("RGB"),3)==0){//RGB值"rgb(0,0,0)"
		str += 4;
		red = (int)string_to_number(str, NULL);
		while(*str !=_T(','))str++;
		if(*(str-1) == _T('%'))//百分比形式
			red *= 255;
		str++;
		green = (int)string_to_number(str, NULL);
		while(*str != _T(','))str++;
		if(*(str-1) == _T('%'))//百分比形式
			green *= 255;
		str++;
		blue = (int)string_to_number(str, NULL);
		while(*str != _T(')'))str++;
		if(*(str-1) == _T('%'))//百分比形式
			blue *= 255;
	}else{//颜色名
		len = 0;
		while(str[len] != 0 && str[len] != _T('"')) len++;
		for (i =0; i < sizeof Colors / sizeof Colors[0]; i++)
			if (_tcsnicmp(str, Colors[i].mnemonic,len) == 0)
				break;
		// FIXME: 这里不完善, 若名称不正确, 那么应该返回异常
		return Colors[i].rgb;
	}
	return RGB(red, green, blue);
}

PUBLIC void multiply_matrix(const NUMBER* M1,const NUMBER* M2,NUMBER *Mout,int r,int s,int t){
	int i,j,k;
	ZeroMemory(Mout,sizeof(NUMBER)*r*t);
	for(i=0;i<r;i++){
		for(j=0;j<t;j++){
			for(k=0;k<s;k++){
				Mout[t*i+j]+=M1[s*i+k]*M2[t*k+j];
			}
		}
	}
}

PUBLIC void transform(REAL_POINT* pt, const NUMBER* mtx){
	NUMBER trans_in[3] = {pt->x, pt->y, 1};
	NUMBER trans_out[3] = {0};
	multiply_matrix(mtx, trans_in, trans_out, 3, 3, 1);
	memcpy(pt, trans_out, sizeof(REAL_POINT));
}

PUBLIC ARRAY dlist_to_array_shallow(DList* list, size_t size){
	ARRAY array = {0};
	void* data = NULL;
	unsigned int cursor = 0;
	Iterator* iter = NULL;
	return_val_if_fail(list, array_empty);

	array.length = dlist_length(list);
	array.data = malloc(size * array.length);

	cursor = (unsigned int)array.data;
	iter = dlist_iterator_create(list);
	while(data = iterator_get(iter)){
		memcpy((void*)cursor, data, size);
		cursor += size;
		iterator_next(iter);
	}
	iterator_destroy(iter);
	return array;
}

PUBLIC ARRAY dlist_to_array_deep(DList* list, size_t size, DEEP_COPY deep_copy){
	ARRAY array = {0};
	void* data = NULL;
	unsigned int cursor = 0;
	Iterator* iter = NULL;
	return_val_if_fail(list && deep_copy, array_empty);

	array.length = dlist_length(list);
	array.data = malloc(size * array.length);

	cursor = (unsigned int)array.data;
	iter = dlist_iterator_create(list);
	while(data = iterator_get(iter)){
		deep_copy((void*)cursor, data);
		cursor += size;
		iterator_next(iter);
	}
	iterator_destroy(iter);
	return array;
}