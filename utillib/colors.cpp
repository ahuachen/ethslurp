/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "colors.h"

const SFColor colorMapRGB[] =
{
	RGB(  0,  0,  0),
	RGB(  0,  0,255),
	RGB(  0,255,255),
	RGB(  0,255,  0),
	RGB(255,  0,255),
	RGB(255,  0,  0),
	RGB(255,255,  0),
	RGB(255,255,255),
	RGB(  0,  0,128),
	RGB(  0,127,127),
	RGB(  0,127,  0),
	RGB(127,  0,127),
	RGB(127,  0,  0),
	RGB(127,127,  0),
	RGB(127,127,127),
	RGB(191,191,191),
	RGB(255,255,127),
	RGB(  0,255,127),
	RGB(127,255,255),
	RGB(127,127,255),
	RGB(255,  0,127),
	RGB(255,127,  0),
	RGB(  0,  0,128),
	RGB(127,127, 64),
	RGB(  0, 64, 64),
	RGB(  0,127,255),
	RGB(  0, 64,127),
	RGB( 64,  0,255),
	RGB(127, 64,  0),
/*
  BlackRGB,
  BlueRGB,
  CyanRGB,
  GreenRGB,
  MagentaRGB,
  RedRGB,
  YellowRGB,
  WhiteRGB,
  DarkBlueRGB,
  DarkCyanRGB,
  DarkGreenRGB,
  DarkMagentaRGB,
  DarkRedRGB,
  DarkYellowRGB,
  DarkGrayRGB,
  LightGreyRGB,
  LightYellowRGB,
  LightGreenRGB,
  LightCyanRGB,
  SlateRGB,
  PuceRGB,
  OrangeRGB,
  DarkerYellowRGB,
  DarkerGreenRGB,
  DarkerCyanRGB,
  DarkerSlateRGB,
  DarkerPuceRGB,
  DarkerOrangeRGB
*/
};

const long nColorsRGB = sizeof(colorMapRGB) / sizeof(SFColor);

CColorInterp::CColorInterp(SFColor strt, SFColor stp, long steps)
{
	start      = strt;
	end        = stp;

	redDelta   = (GetRValue(end) - GetRValue(start)) / (double)steps;
	greenDelta = (GetGValue(end) - GetGValue(start)) / (double)steps;
	blueDelta  = (GetBValue(end) - GetBValue(start)) / (double)steps;
}

SFColor CColorInterp::GetNextColor(long i)
{
  double red   = GetRValue(start);
  double green = GetGValue(start);
  double blue  = GetBValue(start);

  return RGB(red + i * redDelta, green + i * greenDelta, blue + i * blueDelta);
}

SFString asColor(SFColor color)
{
	if (color == -1)
		return EMPTY;
	
	char str[100];
	sprintf(str, "%06x", (unsigned int)color);
	Swap(str[0], str[4]);
	Swap(str[1], str[5]);
	return str;
}
