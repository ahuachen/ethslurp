/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleserver.h"

//-----------------------------------------------------------------------------------------
void processLoginReset(CSimplePage *page)
{
	SFString email    =  getString("email");
	SFString current  =  getString("temp_password");
	SFString password =  getString("new_password");
	SFString confirm  =  getString("confirm");
	SFString formName =  getString("formName");
	SFBool   fromForm = (formName == "Login-Reset");
	SFBool   okay     =  FALSE;
	SFString focus    =  "temp_password";

	CPerson *user = getSimpleServer()->getUserDB()->ReturnUserByEmail(email); // do not use current user--not logged in yet.
	ASSERT(userDB);
	ASSERT(user);

	SFString currentError, passwordError, confirmError;
	if (fromForm)
	{
		if ( confirm .IsEmpty()                          ) { focus = "confirm";       confirmError  = "<br>Confirmation password is required.";                                  }
		if (!password.IsEmpty() && (password != confirm) ) { focus = "confirm";       passwordError = "<br>The password you entered does not match the confirmation password.";  }
		if (!password.IsEmpty() && (password == current) ) { focus = "new_password";  passwordError = "<br>You must change the temporary password.";                             }
		if ( password.ContainsAny(";| \n\t\r")           ) { focus = "new_password";  passwordError = "<br>Your password contains an invalid character: '" + whichBadChar(password) + "'"; }
		if ( password.GetLength() < 8                    ) { focus = "new_password";  passwordError = "<br>Password must be at least eight (8) characters.";                     }
		if ( password.IsEmpty()                          ) { focus = "new_password";  passwordError = "<br>Password field is required.";                                         }
		if ( current != user->getPassword()              ) { focus = "temp_password"; currentError  = "<br>Temporary password does not match our database.";                     }
		if ( current .IsEmpty()                          ) { focus = "temp_password"; currentError  = "<br>Temporary password is required.";                                     }

		okay = (currentError+passwordError+confirmError).IsEmpty();
	}

	page->Initialize("Login-Reset", okay);
	if (okay)
	{
		user->setPassword (password);
		user->setPending  (PERMISSION_NOTSPEC);

		getSimpleServer()->getUserDB()->updateUser(*user);

		getSimpleServer()->getUserDB()->setCurrentUser(user);
		getSimpleServer()->setCookieString("lastLogin", email);
		getSimpleServer()->setSessionCookieString("format", hard_encrypt(user->getUserID(FALSE), PW_KEY));

		SFString dest = getSetCookieString("dest", theSite.m_homePage, CK_SESSION);
		expireCookie("dest");
		page->Initialize(dest, FALSE);
		(*page->func)(page);
		return;

	} else
	{
		page->innerBody.ReplaceAll("[TEMP_PASSWORD_ERR]", currentError );
		page->innerBody.ReplaceAll("[NEW_PASSWORD_ERR]",  passwordError);
		page->innerBody.ReplaceAll("[CONFIRM_ERR]",       confirmError );
	}

	page->innerBody.ReplaceAll("[D]",                     EMPTY);
	page->innerBody.ReplaceAll("[MSG2]",                  page->tailMsg);
	page->innerBody.ReplaceAll("[FOCUS]",                 focus);

	page->innerBody.ReplaceAll("[TEMP_PASSWORD]",         current );
	page->innerBody.ReplaceAll("[NEW_PASSWORD]",          password);
	page->innerBody.ReplaceAll("[CONFIRM]",               confirm );
	page->innerBody.ReplaceAll("[EMAIL]",                 email   );

	return;
}
