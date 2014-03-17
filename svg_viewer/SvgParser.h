#ifndef SvgParser_h
#define SvgParser_h

DECLS_BEGIN

#include "DList.h"
#include "SvgTokenizer.h"
//#include "SVGElement.h"
#include "SvgDomBuilder.h"

PUBLIC SvgDom* parse_svg(LPCTSTR svg_content);
SvgDom* g_svg_dom;

DECLS_END

#endif