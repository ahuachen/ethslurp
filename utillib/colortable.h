#ifndef COLOR_TABLE_H
#define COLOR_TABLE_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "attribute.h"

//This is an index into an array
#define BODYBGCOLOR                    "class=cw_css_bodytable"
#define HEADERBGCOLOR                  "class=cw_css_header"
#define MENUCOLOR_SUB                  "class=cw_css_menu_sub"
#define MENUCOLOR_MAIN                 "class=cw_css_menu_main"
#define FOOTERBGCOLOR                  "class=cw_css_footer"
#define MENUSELECTCOLOR                "class=cw_css_menu_selected"
#define HINTCOLOR                      "class=cw_css_hinttable"
#define HINTHEADCOLOR                  "class=cw_css_hinttable_head"
#define REVIEWEVENTSCOLOR              "class=cw_css_edit"
#define DISABLEDTEXTCOLOR     SFString("class=cw_css_disabled")
#define WARNINGCOLOR          SFString("class=cw_css_warning" )
#define TODAYCOLOR            SFString("class=cw_css_today"   )
#define DYNOCOLOR                      "class=cw_css_dynamic"
#define DIALOGCOLOR                    "class=cw_css_dialog"
#define DIALOGHEADCOLOR                "class=cw_css_dialog_head"

#define MAINTABLECOLOR_INNER           "align=middle|class=cw_css_maintable_inner|cellpadding=0|cellspacing=1|width=100%"
#define MAINTABLECOLOR_BG              "align=middle|cellpadding=0|cellspacing=0|class=cw_css_maintable|width="+asString(width)+"%"
#define MAINTABLECOLOR_HEADER          "class=cw_css_maintable_header"
#define MAINTABLECOLOR_1               "class=cw_css_maintable_color1"
#define MAINTABLECOLOR_2               "class=cw_css_maintable_color2"
#define MAINTABLECOLOR_3               "class=cw_css_maintable_color3"

const SFInt32 maxColors = 141;
const SFInt32 defAmt    = 0x33;

//-------------------------------------------------------------------------
// This class holds name/value pairs for the colors used by various class
//-------------------------------------------------------------------------
class CColorTable
{
private:
	static const SFAttribute m_colorTable[maxColors];

public:	
	static SFString getNameAt       (SFInt32 col);
	static SFString getValueAt      (SFInt32 col);
	static SFString findByName      (const SFString& colorName);
	static SFInt32  findIndexByName (const SFString& colorName);
	static SFString lighten         (const SFString& colorName, SFInt32 adjustAmt=defAmt);
	static SFString darken          (const SFString& colorName, SFInt32 adjustAmt=defAmt);
	static SFInt32  getDistance     (const SFString& val1, const SFString& val2);
	static SFInt32  getDistance     (const SFString& val1, SFInt32 c1);
	static SFInt32  findClosestIndex(const SFString& value);
	static SFInt32  findClosestRGB  (SFInt32 r, SFInt32 g, SFInt32 b);
};

#endif
