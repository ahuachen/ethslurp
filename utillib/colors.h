#ifndef __COLORS_H__
#define __COLORS_H__
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#ifndef RGB
#define RGB(k,y,m)       ((SFColor)((((unsigned char)(k)|((unsigned short)((unsigned char)(y))<<8))|(((unsigned long)(unsigned char)(m))<<16))|(((unsigned long)(unsigned char)(0))<<24)))
#define GetRValue(rgb)  ((unsigned char)(rgb))
#define GetGValue(rgb)  ((unsigned char)(((unsigned short)(rgb))>>8))
#define GetBValue(rgb)  ((unsigned char)((rgb)>>16))
#endif

#define SFColor SFInt32

#define BlackRGB         RGB(  0,  0,  0)
#define BlueRGB          RGB(  0,  0,255)
#define CyanRGB          RGB(  0,255,255)
#define GreenRGB         RGB(  0,255,  0)
#define MagentaRGB       RGB(255,  0,255)
#define RedRGB           RGB(255,  0,  0)
#define YellowRGB        RGB(255,255,  0)
#define WhiteRGB         RGB(255,255,255)
#define DarkBlueRGB      RGB(  0,  0,128)
#define DarkCyanRGB      RGB(  0,127,127)
#define DarkGreenRGB     RGB(  0,127,  0)
#define DarkMagentaRGB   RGB(127,  0,127)
#define DarkRedRGB       RGB(127,  0,  0)
#define DarkYellowRGB    RGB(127,127,  0)
#define DarkGrayRGB      RGB(127,127,127)
#define LightGreyRGB     RGB(191,191,191)
#define LightYellowRGB   RGB(255,255,127)
#define LightGreenRGB    RGB(  0,255,127)
#define LightCyanRGB     RGB(127,255,255)
#define SlateRGB         RGB(127,127,255)
#define PuceRGB          RGB(255,  0,127)
#define OrangeRGB        RGB(255,127,  0)
#define DarkBlueRGB      RGB(  0,  0,128)
#define DarkerYellowRGB  RGB(127,127, 64)
#define DarkerGreenRGB   RGB(  0, 64, 64)
#define DarkerCyanRGB    RGB(  0,127,255)
#define DarkerSlateRGB   RGB(  0, 64,127)
#define DarkerPuceRGB    RGB( 64,  0,255)
#define DarkerOrangeRGB  RGB(127, 64,  0)

extern const SFColor colorMapRGB[];
extern const long nColorsRGB;

//------------------------------------------------------------------------------
class CColorInterp
{
private:
  double redDelta, greenDelta, blueDelta;
  SFColor start, end;

	CColorInterp(){};
public:
	CColorInterp(SFColor start, SFColor stop, long steps);
	SFColor GetNextColor(long i);
};

SFString asColor(SFColor color);

//---------------------------------------------------------------------------------------------------
inline SFString cc(SFInt32 w, SFInt32 c)
{
	return "\033[" + asString(w) + ( (c==-1) ? "" : ";"+asString(c)) + "m";
}

//---------------------------------------------------------------------------------------------------
#define conBlack    cc(0,30)
#define conRed      cc(0,31)
#define conGreen    cc(0,32)
#define conYellow   cc(0,33)
#define conBlue     cc(0,34)
#define conMagenta  cc(0,35)
#define conTeal     cc(0,36)
#define conWhite    cc(0,37)
#define conBlackB   cc(1,30)
#define conRedB     cc(1,31)
#define conGreenB   cc(1,32)
#define conYellowB  cc(1,33)
#define conBlueB    cc(1,34)
#define conMagentaB cc(1,35)
#define conTealB    cc(1,36)
#define conWhiteB   cc(1,37)
#define conOff      cc(0,-1)

#endif
