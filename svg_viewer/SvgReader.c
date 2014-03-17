#include "stdafx.h"
#include "SvgReader.h"
#include <stdio.h>

//将UTF-8编码的字符串转化为Unicode编码的字符串
PRIVATE LPTSTR utf8_to_unicode(LPSTR src){
	LPTSTR out_str;
	int i, j, size = (int)strlen(src);
	unsigned short int hex;
	out_str = (LPWSTR)malloc(sizeof(char) * 2 * (size + 1));
	for(i = 0, j = 0; i < size; i++){
		hex = (unsigned char)*(src + i);
		if(hex < 0x80){
			*(out_str + j) = hex;
			j++;
		}else if(hex < 0xE0){
			hex -= 0xC0;
			hex *= 0x40;
			hex += (unsigned char)*(src + (++i)) - 0x80;
			*(out_str + j) = hex;
			j++;
		}else if(hex < 0xF0){
			hex -= 0xE0;
			hex *= 0x40;
			hex += (unsigned char)*(src + (++i)) - 0x80;
			hex *= 0x40;
			hex += (unsigned char)*(src + (++i)) - 0x80;
			*(out_str + j) = hex;
			j++;
		}
	}
	*(out_str + j) = 0;
	return out_str;
}

PUBLIC LPTSTR read_svg(LPCTSTR file_path){
	FILE *fp = NULL;
	int file_length = 0;
	LPSTR buf = NULL;
	LPTSTR svg_content = NULL;
	//现在假设SVG文件是以ANSI格式编码
	_tfopen_s(&fp, file_path, _T("rb"));
	if(!fp){
		MessageBox(NULL, _T("Error occurred while reading file!"), _T("Message"), MB_OK|MB_ICONEXCLAMATION);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);
	rewind(fp);
	buf = (LPSTR)malloc(sizeof(LPSTR)*(file_length + 1));
	fread(buf,sizeof(char),file_length,fp);
	buf[file_length] = 0;
	fclose(fp);
	svg_content = utf8_to_unicode(buf);
	free(buf);
	return svg_content;
}