#ifndef _SIMPLEUSERDB_H_
#define _SIMPLEUSERDB_H_
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peopledatabase.h"
#include "person.h"

//-----------------------------------------------------------------------------------------
class CSimpleUserDB : public CPeopleDatabase
{
public:
	CPerson  m_noUser;
	CPerson *m_curUser;

public:
			 CSimpleUserDB     ( void );

	void     LoadHeader        ( SFString& contents    ) {};
	void     LoadRemainder     ( SFString& contents    ) {};
	void     LoadUserTable     ( SFString& contents    );

	SFString generateUserID    ( const SFString& email );
	void     updateUser        ( const CPerson& user   );
	void     WriteDatabase     ( void );

    CPerson *getCurrentUser    ( void );
    void     setCurrentUser    ( CPerson *user );
};

#endif
