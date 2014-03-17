#ifndef TypeDef_h
#define TypeDef_h

#ifdef __cplusplus
#define DECLS_BEGIN extern "C" {
#define DECLS_END   }
#else
#define DECLS_BEGIN
#define DECLS_END
#endif/*__cplusplus*/

DECLS_BEGIN

#define PUBLIC
#define PRIVATE static
#define INLINE
#define VIRTUAL
#define READONLY
#define DOMString LPCTSTR
typedef float NUMBER;
typedef NUMBER OPACITY_VALUE;

#ifndef __FUNCTION__
#define __FUNCTION__ "__FUNCTION__"
#endif
#define return_if_fail(p) if(!(p)) \
	{printf("%s:%d Warning: "#p" failed.\n", \
	__FUNCTION__, __LINE__); return;}
#define return_val_if_fail(p, ret) if(!(p)) \
	{printf("%s:%d Warning: "#p" failed.\n",\
	__FUNCTION__, __LINE__); return (ret);}

typedef struct _REAL_POINT{
	NUMBER x;
	NUMBER y;
}REAL_POINT;

PUBLIC POINT real_point_to_point(REAL_POINT thiz);

//attribute
typedef struct _ATTR{
	LPTSTR name;
	LPTSTR value;
}ATTR;

PUBLIC INLINE ATTR* attr_create();
PUBLIC INLINE void attr_destroy(void* attr);

//array
typedef struct _ARRAY{
	void* data;
	int length;
}ARRAY;

extern const ARRAY array_empty;

//unit identifier
typedef enum _UNIT_ID{
	UNIT_ID_EM,				//em
	UNIT_ID_EX,				//ex
	UNIT_ID_PX,				//px, pixel
	UNIT_ID_PT,				//pt, point
	UNIT_ID_PC,				//pc, pica
	UNIT_ID_CM,				//cm, centimeter
	UNIT_ID_MM,				//mm, millimeter
	UNIT_ID_IN,				//in, inch
	UNIT_ID_PERCENTAGE		//percentage, %
}UNIT_ID;

//length, coordinate
typedef struct _LENGTH{
	NUMBER number;
	UNIT_ID unit_id;
}LENGTH, COORDINATE;

extern const LENGTH length_zero;
extern const LENGTH length_100_percent;
#define coordinate_zero length_zero

DECLS_END

#endif