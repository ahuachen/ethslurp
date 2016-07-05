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
#include "webapi.h"

//--------------------------------------------------------------------------------
CWebAPI::CWebAPI(void)
{
}

//--------------------------------------------------------------------------------
CWebAPI::~CWebAPI(void)
{
}

//--------------------------------------------------------------------------------
SFBool CWebAPI::checkKey(CConfig& config)
{
	key      = config.GetProfileStringGH("SETTINGS", "api_key",      EMPTY);
	provider = config.GetProfileStringGH("SETTINGS", "api_provider", "EtherScan");
	url      = config.GetProfileStringGH("SETTINGS", "api_url",      "http://etherscan.io/apis");

	if (!key.IsEmpty())
		return TRUE;

	// Most web APIs require an API key. You will have to get one of these yourself. The
	// program will ask for an api key until it gets one. You only need to provide it once.
	char buffer[256];
	cerr
		<< conRed << "\n  ***Warning***" << conOff << "\n"
		<< "  " << conYellow << "This program" << conOff << " needs an api_key from " + provider + " in order to work. You may get one at\n"
		<< "  " + url + ". See our online help file for more information.\n"
		<< "  Please provide an API key or type 'exit'\n"
		<< "  > ";
	cerr.flush();

	cin >> buffer;
	key = buffer;
	if (key % "exit" || key % "quit")
		exit(0);

	// TODO: extend this to allow for other APIs

	// save the key for later
	config.SetProfileString("SETTINGS", "api_key",      key);
	config.SetProfileString("SETTINGS", "api_provider", "EtherScan");
	config.SetProfileString("SETTINGS", "api_url",      "http://etherscan.io/apis");

	CVersion version;
	version.setVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
	config.writeFile(version.toString());

	return TRUE;
}

//--------------------------------------------------------------------------------
SFString CWebAPI::getKey(void) const
{
	return key;
}
