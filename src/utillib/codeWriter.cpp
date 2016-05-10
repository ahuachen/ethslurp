/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "codeWriter.h"
#include "database.h"

//------------------------------------------------------------------------------------------------------------
void writeTheCode(const SFString& fileName, const SFString& coudOutIn)
{
	SFString codeOut = coudOutIn;
	SFString existingCode;
	existingCode = asciiFileToString(fileName);

	SFString tabs;
	
	SFInt32 nTabs=2;

	tabs = SFString('\t', nTabs--);
	//--------------------------------------------------------------------------------------
	while (existingCode.Contains(tabs + "// EXISTING_CODE"))
	{
		existingCode.Replace(tabs + "// EXISTING_CODE", "<code>");
		existingCode.Replace(tabs + "// EXISTING_CODE", "</code>");
	}
	while (existingCode.Contains("</code>"))
	{
		SFString snipit = Strip(snagFieldClear(existingCode, "code"), '\n');
		codeOut.Replace(tabs + "// EXISTING_CODE\n" + tabs + "// EXISTING_CODE", tabs + "// EXISTING_CODE\n" + snipit + "\n" + tabs + "// EXISTING_CODE");
	}
	codeOut.ReplaceAll("// EXISTING_CODE\n\n" + tabs + "// EXISTING_CODE", "// EXISTING_CODE\n" + tabs + "// EXISTING_CODE");
	//--------------------------------------------------------------------------------------

	tabs = SFString('\t', nTabs--);
	//--------------------------------------------------------------------------------------
	while (existingCode.Contains(tabs + "// EXISTING_CODE"))
	{
		existingCode.Replace(tabs + "// EXISTING_CODE", "<code>");
		existingCode.Replace(tabs + "// EXISTING_CODE", "</code>");
	}
	while (existingCode.Contains("</code>"))
	{
		SFString snipit = Strip(snagFieldClear(existingCode, "code"), '\n');
		codeOut.Replace(tabs + "// EXISTING_CODE\n" + tabs + "// EXISTING_CODE", tabs + "// EXISTING_CODE\n" + snipit + "\n" + tabs + "// EXISTING_CODE");
		codeOut.Replace("return \"This is the code for", "//XXX");
	}
	codeOut.ReplaceAll("// EXISTING_CODE\n\n" + tabs + "// EXISTING_CODE", "// EXISTING_CODE\n" + tabs + "// EXISTING_CODE");
	//--------------------------------------------------------------------------------------

	tabs = SFString('\t', nTabs--);
	//--------------------------------------------------------------------------------------
	while (existingCode.Contains(tabs + "// EXISTING_CODE"))
	{
		existingCode.Replace(tabs + "// EXISTING_CODE", "<code>");
		existingCode.Replace(tabs + "// EXISTING_CODE", "</code>");
	}
	while (existingCode.Contains("</code>"))
	{
		SFString snipit = Strip(snagFieldClear(existingCode, "code"), '\n');
		codeOut.Replace(tabs + "// EXISTING_CODE\n" + tabs + "// EXISTING_CODE", tabs + "// EXISTING_CODE\n" + snipit + "\n" + tabs + "// EXISTING_CODE");
	}
	codeOut.ReplaceAll("// EXISTING_CODE\n\n" + tabs + "// EXISTING_CODE", "// EXISTING_CODE\n" + tabs + "// EXISTING_CODE");
	//--------------------------------------------------------------------------------------

	codeOut.ReplaceAll("//XXX", "//return \"This is the code for");
	
//	codeOut.ReplaceAll("\n\t// EXISTING_CODE\n\t// EXISTING_CODE\n", "\n");
//	codeOut.ReplaceAll("\n// EXISTING_CODE\n// EXISTING_CODE\n", "\n");

	// One final cleanup
	codeOut.ReplaceAll("\n\n}","\n}");
	codeOut.ReplaceAll("\n\n\n","\n\n");

	stringToAsciiFile(fileName, codeOut);
}

