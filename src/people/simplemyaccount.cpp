/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleserver.h"

//-----------------------------------------------------------------------------------------
void processMyAccount(CSimplePage *page)
{
	// we better have a current user otherwise how did we get here?
	CPerson *user = getSimpleServer()->getUserDB()->getCurrentUser();
	ASSERT(user);

	SFString name     =  getString("name");
	SFString email    =  getString("email");
	SFString password =  getString("password");
	SFString confirm  =  getString("confirm");
	SFInt32  join     =  TRUE; // leave this, checkbox defaults to false
	SFString formName =  getString("formName");
	SFBool   fromForm = (formName == "MyAccount");
	SFBool   okay     =  FALSE;
	SFString focus    =  "email";

	SFString emailError, nameError, passwordError, confirmError;
	if (fromForm)
	{
		join = getCheckbox("join");

		CPerson *existingUser = getSimpleServer()->getUserDB()->ReturnUserByEmail(email);
		if (existingUser && existingUser->getUserID() == getSimpleServer()->getUserDB()->getCurrentUser()->getUserID())
			existingUser = NULL; // don't match ourselves, won't happen for new account but check anyway

		if ( confirm .IsEmpty()                          ) { focus = "confirm";  confirmError  = "<br>Confirmation password is required.";                                    }
		if (!password.IsEmpty() && (password != confirm) ) { focus = "confirm";  passwordError = "<br>The password you entered does not match the confirmation password.";    }
		if ( password.ContainsAny(";| \n\t\r")           ) { focus = "password"; passwordError = "<br>Your password contains an invalid character: '" + whichBadChar(password) + "'"; }
		if ( password.GetLength() < 8                    ) { focus = "password"; passwordError = "<br>Password must be at least eight (8) characters.";                       }
		if ( password.IsEmpty()                          ) { focus = "password"; passwordError = "<br>Password field is required.";                                           }
		if ( name    .IsEmpty()                          ) { focus = "name" ;    nameError     = "<br>Name field is required.";                                               }
		if (!IsValidEmail(email)                         ) { focus = "email";    emailError    = "<br>This appears to be an invalid email. Correct it or use another email."; }
		if ( email   .IsEmpty()                          ) { focus = "email";    emailError    = "<br>Email field is required.";                                              }
		if ( existingUser )
		{
			focus = "email";
 			emailError =
			"<br>The email you provided is already associated with an existing user. Provide a different email or `link:Login~email=[EMAIL]|click here` to login to that user.";
			nameError = confirmError = EMPTY;
		}

		okay = (emailError+nameError+passwordError+confirmError).IsEmpty();

	} else
	{
		// first entry to form use existing user data
		name     = user->getFullName ();
		email    = user->getOffice   ().getEmail();
		password = user->getPassword ();
		confirm  = password;
		join     = user->isOptionOn("join");
	}

	page->Initialize("MyAccount", okay);
	page->innerBody.ReplaceAll("[CHECKED]", join ? "checked" : EMPTY);

	if (okay)
	{
		user->setFullName   (name);
		user->getOffice     ().setEmail(email);
		user->setPassword   (password);
		user->setOptionInt32("join", join);

		getSimpleServer()->getUserDB()->updateUser(*user);

		getSimpleServer()->setCookieString("lastLogin", email);

		SFString eSubject = snagFieldClear(page->strings, "eSubject");
		SFString eBody    = snagFieldClear(page->strings, "eBody");
		eBody.ReplaceAll("[EMAIL]", email);
		eBody.ReplaceAll("[DATE]",     Now().Format(FMT_DATE) + " " + Now().Format(FMT_TIME));

		sendAnEmail(email, eSubject, eBody);
		sendAnEmail(getSiteData("mainEmail"), "User modified",  user->toAscii(EMPTY));

		page->innerBody.ReplaceAll("[NAME_ERR]",     EMPTY);
		page->innerBody.ReplaceAll("[EMAIL_ERR]",    EMPTY);
		page->innerBody.ReplaceAll("[PASSWORD_ERR]", EMPTY);
		page->innerBody.ReplaceAll("[CONFIRM_ERR]",  EMPTY);
		page->innerBody.ReplaceAll("[USERID_ERR]",   EMPTY);
		page->closeForm("Account settings saved");

	} else
	{
		page->innerBody.ReplaceAll("[NAME_ERR]",     nameError );
		page->innerBody.ReplaceAll("[EMAIL_ERR]",    emailError);
		page->innerBody.ReplaceAll("[PASSWORD_ERR]", passwordError);
		page->innerBody.ReplaceAll("[CONFIRM_ERR]",  confirmError);
		page->innerBody.ReplaceAll("[USERID_ERR]",   "</span><br><small>You may not edit your userid.</small>");
	}

	page->innerBody.ReplaceAll("[MSG2]",     page->tailMsg);
	page->innerBody.ReplaceAll("[D]",        (okay ? "disabled" : EMPTY));
	page->innerBody.ReplaceAll("[FOCUS]",    focus  );

	page->innerBody.ReplaceAll("[USERID]",   user->getUserID());
	page->innerBody.ReplaceAll("[NAME]",     name     );
	page->innerBody.ReplaceAll("[EMAIL]",    email    );
	page->innerBody.ReplaceAll("[PASSWORD]", password );
	page->innerBody.ReplaceAll("[CONFIRM]",  confirm  );
	
	return;
}
