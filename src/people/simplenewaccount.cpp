/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleserver.h"

//-----------------------------------------------------------------------------------------
void processNewAccount(CSimplePage *page)
{
	CPerson  newUser(getSimpleServer()->getUserDB());
	CPerson *user = &newUser;

	SFString name      = toProper(getString("name"));
	SFString email     = getString("email");
	SFString password  = getString("password");
	SFString confirm   = getString("confirm");
	SFInt32  join     =  TRUE; // leave this, checkbox defaults to false
	SFString formName =  getString("formName");
	SFBool   fromForm = (formName == "NewAccount");
	SFBool   okay     =  FALSE;
	SFString focus     = "email";

	SFString emailError, nameError, passwordError, confirmError;
	if (fromForm)
	{
		join = getCheckbox("join");

		CPerson *existingUser = getSimpleServer()->getUserDB()->ReturnUserByEmail(email);

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
				"<br>Email exists. You should have received instructions containing a temporary<br>"
				"password when you registered. `link:Login~email=[EMAIL]|Click here` to login and/or recover your password.";
			nameError = passwordError = confirmError = EMPTY;
		}
		okay = (emailError+nameError+passwordError+confirmError).IsEmpty();
	}

	page->Initialize("NewAccount", okay);
	page->innerBody.ReplaceAll("[CHECKED]", join ? "checked" : EMPTY);
	page->tailMsg.ReplaceAll("[WAS]", (join ? "has been" : "was not"));

	if (okay)
	{
		user->setUserID     (getSimpleServer()->getUserDB()->generateUserID(email)); // Userid is a unique, read-only field, never shown to users and never changed. User logs in with email
		user->setFullName   (name);
		user->getOffice     ().setEmail(email);
		user->setPassword   (password);
		user->setPermission (PRM_DEFAULT, PRM_EDIT);
		user->setOptionInt32("join", join);

		getSimpleServer()->getUserDB()->updateUser(*user);

		CPerson *theUser = getSimpleServer()->getUserDB()->ReturnUserByEmail(email);
		ASSERT(theUser);
		getSimpleServer()->getUserDB()->setCurrentUser(theUser);
		getSimpleServer()->setCookieString("lastLogin", email);

		SFString eSubject = snagFieldClear(page->strings, "eSubject");
		SFString eBody    = snagFieldClear(page->strings, "eBody");

		eBody.ReplaceAll("[WAS]",      (join ? "has been" : "was not"));
		eBody.ReplaceAll("[ADD]",      (join ? "remove" : "add"));
		eBody.ReplaceAll("[EMAIL]",    user->getMainEmailNoLink());
		eBody.ReplaceAll("[NAME]",     user->getFullName());
		eBody.ReplaceAll("[PASSWORD]", SFString('*', user->getPassword().GetLength())); //(TRUE ? user->getPassword() : EMPTY));
		eBody.ReplaceAll("[FORMAT]",   user->getUserID());
		eBody.ReplaceAll("[DATE]",     Now().Format(FMT_DATE) + " " + Now().Format(FMT_TIME));

		sendAnEmail(email, eSubject, eBody);
		sendAnEmail(getSiteData("mainEmail"), "new user added", user->toAscii(EMPTY));

		page->innerBody.ReplaceAll("[NAME_ERR]",     EMPTY);
		page->innerBody.ReplaceAll("[EMAIL_ERR]",    EMPTY);
		page->innerBody.ReplaceAll("[PASSWORD_ERR]", EMPTY);
		page->innerBody.ReplaceAll("[CONFIRM_ERR]",  EMPTY);
		page->closeForm("New account created");

	} else
	{
		page->innerBody.ReplaceAll("[NAME_ERR]",     nameError );
		page->innerBody.ReplaceAll("[EMAIL_ERR]",    emailError);
		page->innerBody.ReplaceAll("[PASSWORD_ERR]", passwordError);
		page->innerBody.ReplaceAll("[CONFIRM_ERR]",  confirmError);
	}

	page->innerBody.ReplaceAll("[MSG2]",     page->tailMsg);
	page->innerBody.ReplaceAll("[D]",        (okay ? "disabled" : EMPTY));
	page->innerBody.ReplaceAll("[FOCUS]",    focus  );

	page->innerBody.ReplaceAll("[NAME]",     name     );
	page->innerBody.ReplaceAll("[EMAIL]",    email    );
	page->innerBody.ReplaceAll("[PASSWORD]", password );
	page->innerBody.ReplaceAll("[CONFIRM]",  confirm  );

	page->subTitle.ReplaceAll("[EMAIL]", email);
	getSimpleServer()->expireCookie("format"); // make sure we're logged out.

	return;
}
