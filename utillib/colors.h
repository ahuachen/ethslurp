#ifndef _COLORS_H_
#define _COLORS_H_

//---------------------------------------------------------------------------------------------------
inline SFString cc(SFInt32 w, SFInt32 c) { return "\033[" + asString(w) + ( (c==-1) ? "" : ";"+asString(c)) + "m"; }

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
