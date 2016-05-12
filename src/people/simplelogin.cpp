/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleserver.h"

//-----------------------------------------------------------------------------------------
void processLogin(CSimplePage *page)
{
	// If user has click directly on the Login link we expire "dest" otherwise unexpected destinations may appear
	if (isCommand("Login") && getCookieString("dest") == "MyAccount")
		expireCookie("dest");

	SFString email    =  getString("email", getCookieString("lastLogin"));
	SFString password =  getString("password");
	SFString formName =  getString("formName");
	SFBool   fromForm = (formName == "Login");
	SFBool   okay     =  FALSE;
	SFString focus    =  email.IsEmpty() ? "email" : "password";

	CPerson *user     = getSimpleServer()->getUserDB()->ReturnUserByEmail(email);

	SFString emailError, passwordError;
	if (fromForm)
	{
		if (!user                                  ) { focus = "email";    emailError = "<br>Email '" + email + "' not found in our database.";                          }
		if (email.IsEmpty()                        ) { focus = "email";    emailError = "<br>Email field is required.";                                                  }
		if (user && password != user->getPassword()) { focus = "password"; passwordError = "<br>The password you entered does not match our records. Please try again."; }

		okay = (emailError+passwordError).IsEmpty();
	}

	page->Initialize("Login", okay);
	if (okay)
	{
		if (user->isPending())
		{
			page->Initialize("Login-Reset", FALSE);
			(*page->func)(page);
			return;
		}

		getSimpleServer()->getUserDB()->setCurrentUser(user);
		getSimpleServer()->setCookieString("lastLogin", email);

		SFBool persist = getCheckbox("persist");
		if (persist || user->isSuper())
			getSimpleServer()->setCookieString("format", hard_encrypt(user->getUserID(FALSE), PW_KEY));
		else
			getSimpleServer()->setSessionCookieString("format", hard_encrypt(user->getUserID(FALSE), PW_KEY));

		SFString dest = getSetCookieString("dest", theSite.m_homePage, CK_SESSION);
		expireCookie("dest");
		page->Initialize(dest, FALSE);
		(*page->func)(page);
		return;
	}

	SFBool isLogin = (isCommand("Login") || isCommand("Login-Logout"));
	if (!isLogin)
		page->subTitle = "<h3>You must login to access this feature.</h3><p>\n";

	page->innerBody.ReplaceAll("[EMAIL_ERR]",    emailError);
	page->innerBody.ReplaceAll("[PASSWORD_ERR]", passwordError );
	page->innerBody.ReplaceAll("[D]",            EMPTY);
	page->innerBody.ReplaceAll("[FOCUS]",        focus);
	page->innerBody.ReplaceAll("[MSG2]",         page->tailMsg);

	page->innerBody.ReplaceAll("[CMD]",          getString("cmd"));
	page->innerBody.ReplaceAll("[EMAIL]",        email);
	page->innerBody.ReplaceAll("[PASSWORD]",     password);

	return;
}
