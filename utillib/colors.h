#ifndef _COLORS_H_
#define _COLORS_H_
/*--------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2016 Great Hill Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------*/
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
