/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleserver.h"

//-----------------------------------------------------------------------------------------
void processRecover(CSimplePage *page)
{
	SFString email    =  getString("email");
	SFString formName =  getString("formName");
	SFBool   fromForm = (formName == "Recover");
	SFBool   okay     =  FALSE;
	SFString focus    =  "email";

	CPerson *user     = getSimpleServer()->getUserDB()->ReturnUserByEmail(email);

	SFString emailError;
	if (fromForm)
	{
		if (!user           ) { focus = "email"; emailError = "<br>Email '" + email + "' not found in our database."; }
		if ( email.IsEmpty()) { focus = "email"; emailError = "<br>Email field is required.";  }

		okay = (user!=NULL);

	} else
	{
		email = getString("email", getCookieString("lastLogin")); // only use cookie if we're coming in fresh
	}

	page->Initialize("Recover", okay);
	if (okay)
	{
		user->setPassword(randomPassword());
		user->setPending(TRUE);

		getSimpleServer()->getUserDB()->updateUser(*user);

		getSimpleServer()->setCookieString("lastLogin", email);

		SFString eSubject = snagFieldClear(page->strings, "eSubject");
		SFString eBody    = snagFieldClear(page->strings, "eBody");

		eBody.ReplaceAll("[EMAIL]",    email);
		eBody.ReplaceAll("[PASSWORD]", user->getPassword());
		eBody.ReplaceAll("[DATE]",     Now().Format(FMT_DATE) + " " + Now().Format(FMT_TIME));

		sendAnEmail(email, eSubject, eBody);

		page->innerBody.ReplaceAll("[EMAIL_ERR]", EMPTY);
		page->closeForm("Your password was reset. Check your email.");

		page->subTitle.ReplaceAll("[EMAIL]", email);

	} else
	{
		page->innerBody.ReplaceAll("[EMAIL_ERR]", emailError);
	}

	page->innerBody.ReplaceAll("[D]",        (okay ? "disabled" : EMPTY));
	page->innerBody.ReplaceAll("[MSG2]",     page->tailMsg);
	page->innerBody.ReplaceAll("[FOCUS]",    focus);
	page->innerBody.ReplaceAll("[EMAIL]",    email);

	return;
}
