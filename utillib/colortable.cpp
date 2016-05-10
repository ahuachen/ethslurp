/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "colortable.h"

//-----------------------------------------------------------------------
// Do not change the ordering of this list.  Event background and text
// colors store an index into this list - changing the order of these
// items will break existing events.
//-----------------------------------------------------------------------
const SFAttribute CColorTable::m_colorTable[maxColors] =
{
	SFAttribute("default",               "-1"),
	SFAttribute("black",                 "000000"),
	SFAttribute("navy",                  "000080"),
	SFAttribute("darkblue",              "00008B"),
	SFAttribute("mediumblue",            "0000CD"),
	SFAttribute("blue",                  "0000FF"),
	SFAttribute("darkgreen",             "006400"),
	SFAttribute("green",                 "008000"),
	SFAttribute("teal",                  "008080"),
	SFAttribute("darkcyan",              "008B8B"),
	SFAttribute("deepskyblue",           "00BFFF"),
	SFAttribute("darkturquoise",         "00CED1"),
	SFAttribute("mediumspringgreen",     "00FA9A"),
	SFAttribute("lime",                  "00FF00"),
	SFAttribute("springgreen",           "00FF7F"),
	SFAttribute("aqua",                  "00FFFF"),
	SFAttribute("cyan",                  "00FFFF"),
	SFAttribute("midnightblue",          "191970"),
	SFAttribute("dodgerblue",            "1E90FF"),
	SFAttribute("lightseagreen",         "20B2AA"),
	SFAttribute("forestgreen",           "228B22"),
	SFAttribute("seagreen",              "2E8B57"),
	SFAttribute("darkslategray",         "2F4F4F"),
	SFAttribute("limegreen",             "32CD32"),
	SFAttribute("mediumseagreen",        "3CB371"),
	SFAttribute("turquoise",             "40E0D0"),
	SFAttribute("royalblue",             "4169E1"),
	SFAttribute("steelblue",             "4682B4"),
	SFAttribute("darkslateblue",         "483D8B"),
	SFAttribute("mediumturquoise",       "48D1CC"),
	SFAttribute("indigo",                "4B0082"),
	SFAttribute("darkolivegreen",        "556B2F"),
	SFAttribute("cadetblue",             "5F9EA0"),
	SFAttribute("cornflowerblue",        "6495ED"),
	SFAttribute("mediumaquamarine",      "66CDAA"),
	SFAttribute("dimgray",               "696969"),
	SFAttribute("slateblue",             "6A5ACD"),
	SFAttribute("olivedrab",             "6B8E23"),
	SFAttribute("slategray",             "708090"),
	SFAttribute("lightslategray",        "778899"),
	SFAttribute("mediumslateblue",       "7B68EE"),
	SFAttribute("lawngreen",             "7CFC00"),
	SFAttribute("chartreuse",            "7FFF00"),
	SFAttribute("aquamarine",            "7FFFD4"),
	SFAttribute("maroon",                "800000"),
	SFAttribute("purple",                "800080"),
	SFAttribute("olive",                 "808000"),
	SFAttribute("gray",                  "808080"),
	SFAttribute("skyblue",               "87CEEB"),
	SFAttribute("lightskyblue",          "87CEFA"),
	SFAttribute("blueviolet",            "8A2BE2"),
	SFAttribute("darkred",               "8B0000"),
	SFAttribute("darkmagenta",           "8B008B"),
	SFAttribute("saddlebrown",           "8B4513"),
	SFAttribute("darkseagreen",          "8FBC8F"),
	SFAttribute("lightgreen",            "90EE90"),
	SFAttribute("mediumpurple",          "9370DB"),
	SFAttribute("darkviolet",            "9400D3"),
	SFAttribute("palegreen",             "98FB98"),
	SFAttribute("darkorchid",            "9932CC"),
	SFAttribute("yellowgreen",           "9ACD32"),
	SFAttribute("sienna",                "A0522D"),
	SFAttribute("brown",                 "A52A2A"),
	SFAttribute("darkgray",              "A9A9A9"),
	SFAttribute("lightblue",             "ADD8E6"),
	SFAttribute("greenyellow",           "ADFF2F"),
	SFAttribute("paleturquoise",         "AFEEEE"),
	SFAttribute("lightsteelblue",        "B0C4DE"),
	SFAttribute("powderblue",            "B0E0E6"),
	SFAttribute("firebrick",             "B22222"),
	SFAttribute("darkgoldenrod",         "B8860B"),
	SFAttribute("mediumorchid",          "BA55D3"),
	SFAttribute("rosybrown",             "BC8F8F"),
	SFAttribute("darkkhaki",             "BDB76B"),
	SFAttribute("silver",                "C0C0C0"),
	SFAttribute("mediumvioletred",       "C71585"),
	SFAttribute("indianred",             "CD5C5C"),
	SFAttribute("peru",                  "CD853F"),
	SFAttribute("chocolate",             "D2691E"),
	SFAttribute("tan",                   "D2B48C"),
	SFAttribute("lightgrey",             "D3D3D3"),
	SFAttribute("thistle",               "D8BFD8"),
	SFAttribute("orchid",                "DA70D6"),
	SFAttribute("goldenrod",             "DAA520"),
	SFAttribute("palevioletred",         "DB7093"),
	SFAttribute("crimson",               "DC143C"),
	SFAttribute("gainsboro",             "DCDCDC"),
	SFAttribute("plum",                  "DDA0DD"),
	SFAttribute("burlywood",             "DEB887"),
	SFAttribute("lightcyan",             "E0FFFF"),
	SFAttribute("lavender",              "E6E6FA"),
	SFAttribute("darksalmon",            "E9967A"),
	SFAttribute("violet",                "EE82EE"),
	SFAttribute("palegoldenrod",         "EEE8AA"),
	SFAttribute("lightcoral",            "F08080"),
	SFAttribute("khaki",                 "F0E68C"),
	SFAttribute("aliceblue",             "F0F8FF"),
	SFAttribute("honeydew",              "F0FFF0"),
	SFAttribute("azure",                 "F0FFFF"),
	SFAttribute("sandybrown",            "F4A460"),
	SFAttribute("wheat",                 "F5DEB3"),
	SFAttribute("beige",                 "F5F5DC"),
	SFAttribute("whitesmoke",            "F5F5F5"),
	SFAttribute("mintcream",             "F5FFFA"),
	SFAttribute("ghostwhite",            "F8F8FF"),
	SFAttribute("salmon",                "FA8072"),
	SFAttribute("antiquewhite",          "FAEBD7"),
	SFAttribute("linen",                 "FAF0E6"),
	SFAttribute("lightgoldenrodyellow",  "FAFAD2"),
	SFAttribute("oldlace",               "FDF5E6"),
	SFAttribute("red",                   "FF0000"),
	SFAttribute("fuchsia",               "FF00FF"),
	SFAttribute("magenta",               "FF00FF"),
	SFAttribute("deeppink",              "FF1493"),
	SFAttribute("orangered",             "FF4500"),
	SFAttribute("tomato",                "FF6347"),
	SFAttribute("hotpink",               "FF69B4"),
	SFAttribute("coral",                 "FF7F50"),
	SFAttribute("darkorange",            "FF8C00"),
	SFAttribute("lightsalmon",           "FFA07A"),
	SFAttribute("orange",                "FFA500"),
	SFAttribute("lightpink",             "FFB6C1"),
	SFAttribute("pink",                  "FFC0CB"),
	SFAttribute("gold",                  "FFD700"),
	SFAttribute("peachpuff",             "FFDAB9"),
	SFAttribute("navajowhite",           "FFDEAD"),
	SFAttribute("moccasin",              "FFE4B5"),
	SFAttribute("bisque",                "FFE4C4"),
	SFAttribute("mistyrose",             "FFE4E1"),
	SFAttribute("blanchedalmond",        "FFEBCD"),
	SFAttribute("papayawhip",            "FFEFD5"),
	SFAttribute("lavenderblush",         "FFF0F5"),
	SFAttribute("seashell",              "FFF5EE"),
	SFAttribute("cornsilk",              "FFF8DC"),
	SFAttribute("lemonchiffon",          "FFFACD"),
	SFAttribute("floralwhite",           "FFFAF0"),
	SFAttribute("snow",                  "FFFAFA"),
	SFAttribute("yellow",                "FFFF00"),
	SFAttribute("lightyellow",           "FFFFE0"),
	SFAttribute("white",                 "FFFFFF"),
	SFAttribute("white",                 "FFFFFF"),
};

SFString CColorTable::getNameAt(SFInt32 col)
{
	SFInt32 n = MAX(0, MIN(maxColors-1, col));
	return CColorTable::m_colorTable[n].getName();
}

SFString CColorTable::getValueAt(SFInt32 col)
{
	SFInt32 n = MAX(0, MIN(maxColors-1, col));
	return CColorTable::m_colorTable[n].getValue();
}

SFString CColorTable::findByName(const SFString& colorName)
{
	for (int i=0;i<maxColors;i++)
	{
		if (colorName % CColorTable::m_colorTable[i].getName())
			return CColorTable::m_colorTable[i].getValue();
	}
	return colorName;
}

SFInt32 CColorTable::findIndexByName(const SFString& colorName)
{
	for (int i=0;i<maxColors;i++)
	{
		if (colorName % CColorTable::m_colorTable[i].getName())
			return i;
	}
	return 0;
}

extern SFInt32 getRed  (const SFString& hexColor);
extern SFInt32 getGreen(const SFString& hexColor);
extern SFInt32 getBlue (const SFString& hexColor);

SFInt32 getRed(const SFString& hexColor)
{
	ASSERT(hexColor.GetLength() == 6); 
	return strtoul((const char *)hexColor.Mid(0,2), NULL, 16);
}

SFInt32 getGreen(const SFString& hexColor)
{
	ASSERT(hexColor.GetLength() == 6); 
	return strtoul((const char *)hexColor.Mid(2,2), NULL, 16);
}

SFInt32 getBlue(const SFString& hexColor)
{
	ASSERT(hexColor.GetLength() == 6); 
	return strtoul((const char *)hexColor.Mid(4,2), NULL, 16);
}

SFString CColorTable::lighten(const SFString& colorName, SFInt32 adjustAmt)
{ 
	if (colorName.Contains("cw_css_"))
		return colorName;

	SFString color = CColorTable::findByName(colorName); 

	ASSERT(color.GetLength() == 6); 

	SFInt32 red   = (MIN(255, getRed(color)   + adjustAmt)); 
	SFInt32 green = (MIN(255, getGreen(color) + adjustAmt)); 
	SFInt32 blue  = (MIN(255, getBlue(color)  + adjustAmt)); 
 
	return asHex(red) + asHex(green) + asHex(blue);
} 

SFString CColorTable::darken(const SFString& colorName, SFInt32 adjustAmt) 
{ 
	if (colorName.Contains("cw_css_"))
		return colorName;

	SFString color = CColorTable::findByName(colorName); 

	ASSERT(color.GetLength() == 6); 

	SFInt32 red   = (MAX(0, getRed(color)   - adjustAmt)); 
	SFInt32 green = (MAX(0, getGreen(color) - adjustAmt)); 
	SFInt32 blue  = (MAX(0, getBlue(color)  - adjustAmt)); 
 
	return asHex(red) + asHex(green) + asHex(blue);
} 

SFInt32 CColorTable::getDistance(const SFString& val1, SFInt32 c1)
{
	return getDistance(val1, m_colorTable[c1].getValue());
}

SFInt32 CColorTable::getDistance(const SFString& val1, const SFString& val2)
{
	SFInt32 r1 = MAX(0, getRed(val1)  ); 
	SFInt32 g1 = MAX(0, getGreen(val1)); 
	SFInt32 b1 = MAX(0, getBlue(val1) ); 

	SFInt32 r2 = MAX(0, getRed(val2)  ); 
	SFInt32 g2 = MAX(0, getGreen(val2)); 
	SFInt32 b2 = MAX(0, getBlue(val2) );
	
	SFInt32 v1 = (r2 - r1);
	SFInt32 v2 = (g2 - g1);
	SFInt32 v3 = (b2 - b1);
	
	SFInt32 val = (v1*v1 + v2*v2 + v3*v3);
	return (SFInt32)(sqrt((double)val)); 
}

SFInt32 CColorTable::findClosestIndex(const SFString& value)
{
	// Note: findByName returns 'value' if it matches no names so this
	// handles both named colors (like black) and HEX colors (since they
	// won't be found and returned as is. For values that are neither
	// hex or a named color it return '0'
	SFString hexColor = findByName(value);
	if (hexColor.GetLength() != 6)
		return 0;

	SFInt32 minDist  = 1000000;
	SFInt32 minColor = 0;
	for (int i=1;i<maxColors;i++) // we start at index '1' since the first is not it since its default
	{
	   SFInt32 dist = getDistance(hexColor, m_colorTable[i].getValue());
		if (dist < minDist)
		{
			minDist  = dist;
			minColor = i;
		}
	}

	return minColor;
}

SFInt32 CColorTable::findClosestRGB(SFInt32 r, SFInt32 g, SFInt32 b)
{
	return CColorTable::findClosestIndex(asHex(r) + asHex(g) + asHex(b));
}

