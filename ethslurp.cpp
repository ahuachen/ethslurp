/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "manage.h"
#include "ethslurp.h"

/* See options.cpp for a list of command line options */

//---------------------------------------------------------------------------------------------------
SFInt32 cmdEthSlurp(SFInt32 nArgs, const SFString *args)
{
	// During testing, send all output to the screen so it can be re-directed
	if (isTesting)
		outErr = outScreen;
	
	CSlurperApp slurper;
	slurper.version.setVersion(FILE_MAJOR_VERSION, FILE_MINOR_VERSION, FILE_BUILD_VERSION);
	slurper.config.setFilename(PATH_TO_ETH_SLURP+"config.dat");

	// Parse the command line
	if (slurper.opt.parseArguments(nArgs, args) != RETURN_OK)
		return RETURN_FAIL;

	// If this is the first time we've ever run, build the config file
	if (!establish_folders(slurper.config, slurper.version.toString()))
		return usage(args[0], "Unable to create data folders at " + PATH_TO_SLURPS);

	// The web APIs require an API key. You will have to get one of these yourself. The
	// program will ask for an api key until it gets one. You only need to provide it once.
	slurper.api_key      = slurper.config.GetProfileStringGH("SETTINGS", "api_key",      EMPTY);
	slurper.api_provider = slurper.config.GetProfileStringGH("SETTINGS", "api_provider", "EtherScan");
	slurper.api_url      = slurper.config.GetProfileStringGH("SETTINGS", "api_url",      "http://etherscan.io/apis");
	while (slurper.api_key.IsEmpty())
	{
		cerr
			<< conRed << "\n  ***Warning***" << conOff << "\n"
			<< "  " << conYellow << "ethslurp" << conOff << " needs an api_key from " + slurper.api_provider + " in order to work. You may get one at\n"
			<< "  " + slurper.api_url + ". See our online help file for more information.\n"
			<< "  Please provide an API key or type 'exit'\n"
			<< "  > ";
		cerr.flush();
		char buffer[256];
		cin >> buffer;
		slurper.api_key = buffer;
		if (slurper.api_key % "exit" || slurper.api_key % "quit")
			exit(0);
		
		// save the API key for later
		slurper.config.SetProfileString("SETTINGS", "api_key",      slurper.api_key);
		slurper.config.SetProfileString("SETTINGS", "api_provider", "EtherScan");
		slurper.config.SetProfileString("SETTINGS", "api_url",      "http://etherscan.io/apis");
		slurper.config.writeFile(slurper.version.toString());
	}
	
	// If we are told to get the address from the rerun address, do so...
	if (slurper.opt.addr.IsEmpty() && slurper.opt.rerun)
		slurper.opt.addr = slurper.config.GetProfileStringGH("SETTINGS", "rerun", EMPTY);
	
	// Ethereum addresses are case insensative. Force all address to lower case to avoid mismatches.
	slurper.opt.addr.MakeLower();
	
	// We can't run without an address...
	if (slurper.opt.addr.IsEmpty())
	{
		SFString msg = "You must supply an Ethereum account or contract address. ";
		if (!slurper.opt.rerun)
			msg += "Use -r flag to rerun the most recently slurped address.";
		return usage(args[0], msg);
	}
	
	// Save the address for the next run if needed
	slurper.config.SetProfileString("SETTINGS", "rerun", slurper.opt.addr);
	slurper.config.writeFile(slurper.version.toString());

	// Ready to slurp
    slurper.theSlurp.addr = slurper.opt.addr;
	outErr << "Slurping " << slurper.theSlurp.addr << "\n";

	// Slurp...
	SFString message;
	if (!slurper.Slurp(message))
		return usage(args[0], message);

	// Display...
	if (!slurper.Display(message))
		return usage(args[0], message);

	// Finished
	return RETURN_OK;
}

//--------------------------------------------------------------------------------
// Make sure our data folder exist, if not establish it
SFBool establish_folders(CConfig& config, const SFString& vers)
{
	if (SFos::folderExists(PATH_TO_SLURPS) && SFos::fileExists(PATH_TO_ETH_SLURP+"config.dat"))
	{
		config.loadFile(PATH_TO_ETH_SLURP+"config.dat");
		return TRUE;
	}
	
	// create the main folder
	SFos::mkdir(PATH_TO_ETH_SLURP);
	if (!SFos::folderExists(PATH_TO_ETH_SLURP))
		return FALSE;
	
	// create the folder for the slurps
	SFos::mkdir(PATH_TO_SLURPS);
	if (!SFos::folderExists(PATH_TO_SLURPS))
		return FALSE;
	
	config.SetProfileString("SETTINGS",     "datapath",          PATH_TO_SLURPS);
	config.SetProfileString("SETTINGS",     "api_key",           EMPTY);
	config.SetProfileString("SETTINGS",     "focus",             "189");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_fieldList",     EMPTY);
	
	config.SetProfileString("DISPLAY_STR",  "fmt_txt_file",      "[{HEADER}]\\n[{RECORDS}]");
	config.SetProfileString("DISPLAY_STR",  "fmt_txt_record",    "[{FIELDS}]\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_txt_field",     "\\t[{FIELD}]");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_csv_file",      "[{HEADER}]\\n[{RECORDS}]");
	config.SetProfileString("DISPLAY_STR",  "fmt_csv_record",    "[{FIELDS}]\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_csv_field",     "[\"{FIELD}\"],");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_html_file",     "<table>\\n[{HEADER}]\\n[{RECORDS}]</table>\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_html_record",   "\\t<tr>\\n[{FIELDS}]</tr>\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_html_field",    "\\t\\t<td>[{FIELD}]</td>\\n");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_json_file",     "[{RECORDS}]\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_json_record",   "\\n        {\\n[{FIELDS}]        },");
	config.SetProfileString("DISPLAY_STR",  "fmt_json_field",    "\"[{p:FIELD}]\":\"[{FIELD}]\",");

	config.SetProfileString("DISPLAY_STR",  "fmt_custom_file",	 "file:custom_format_file.html");
	config.SetProfileString("DISPLAY_STR",  "fmt_custom_record", "fmt_html_record");
	config.SetProfileString("DISPLAY_STR",  "fmt_custom_field",	 "fmt_html_field");

	config.writeFile(vers);
	return SFos::fileExists(config.getFilename());
}
