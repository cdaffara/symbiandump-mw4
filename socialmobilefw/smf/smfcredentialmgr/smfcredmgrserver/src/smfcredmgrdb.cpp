/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Chandradeep Gandhi, Sasken Communication Technologies Ltd - Initial contribution
 * 
 * Contributors:
 * Pritam Roy Biswas, Nalina Hariharan, Sasken Communication Technologies Ltd
 *
 * Description:
 * This header defines the database-table and the creation of database that will be 
 * used by the Credential Manager Server to store all the credentials related 
 * to an Authentication Application
 */

#include <BAUTILS.H>

#include "smfcredmgrdb.h"

/**
 * NewL Method
 * @return The constructed CSmfCredMgrDb instance
 */
CSmfCredMgrDb* CSmfCredMgrDb::NewL()
	{
	CSmfCredMgrDb* self = CSmfCredMgrDb::NewLC();
	CleanupStack::Pop(self);
	return (self);
	}

/**
 * NewLC Method
 * @return The constructed CSmfCredMgrDb instance
 */
CSmfCredMgrDb* CSmfCredMgrDb::NewLC()
	{
	CSmfCredMgrDb* self = new (ELeave) CSmfCredMgrDb();
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}

/**
 * Constructor
 */
CSmfCredMgrDb::CSmfCredMgrDb()
	{
	RDebug::Printf("in constructor db");
	}

void CSmfCredMgrDb::ConstructL()
	{
	//create db file 
	TInt err = iFileSession.Connect();
	User::LeaveIfError(err);

	err = iFileSession.MkDirAll(KSmfCredMgrDbPrivateDir);

	TFileName dbFileName = KSmfCredMgrDbPrivateDir();
	dbFileName.Append(KSmfCredMgrDbFileName);
	iDbFilePathName = dbFileName;
	if (!BaflUtils::FileExists(iFileSession, dbFileName))
		{
		err = iFileSession.SetSessionToPrivate(EDriveC);

		// no database exists so we make one
		CompleteDBCreationL(dbFileName);
		}
	else if (BaflUtils::FileExists(iFileSession, dbFileName))
		{
		
		//Reset if necessary
		}
	}

/**
 * Destructor
 */
CSmfCredMgrDb::~CSmfCredMgrDb()
	{
	//delete db file
	iDataBase.Close();
	iFileSession.Close();
	RDebug::Printf("in destructor db");
	}

/**
 * Method to create the Db file
 * @param aFileName The file name
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description.
 */
TInt CSmfCredMgrDb::CreateDbFile(const TFileName &aFileName)
	{
	TInt err(KErrNone);
	err = iDataBase.Create(aFileName);
	if (err != KErrNone)
		{
		//the empty file is deleted in case of error
		DeleteDbFile(aFileName);
		}
	return err;
	}

/**
 * Method to create the Db tables
 * @param aFileName The Db file name
 */
void CSmfCredMgrDb::CompleteDBCreationL(const TFileName &aFileName)
	{
	User::LeaveIfError(CreateDbFile(aFileName));
	User::LeaveIfError(PluginIDTableCreate());
	User::LeaveIfError(URLTableCreate());
	User::LeaveIfError(RegTokenValidityTableCreate());
	User::LeaveIfError(AuthParamsTableCreate());
	}

/**
 * Creates the Plugin Id table
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description. 
 */
TInt CSmfCredMgrDb::PluginIDTableCreate()
	{
	TInt err(KErrNone);
	err = iDataBase.Exec(KCreatePluginIDTable);
	if (err >= 0)
		{
		return KErrNone;
		}
	//delete file in case of incomplete table creation
	DeleteDbFile(iDbFilePathName);
	return err;
	}

/**
 * Creates the URL table
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description.
 */
TInt CSmfCredMgrDb::URLTableCreate()
	{
	TInt err(KErrNone);
	err = iDataBase.Exec(KCreateURLTable);
	if (err >= 0)
		{
		return KErrNone;
		}
	//delete file in case of incomplete table creation
	DeleteDbFile(iDbFilePathName);
	return err;
	}

/**
 * Creates the RegistrationToken-Validity Table
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description.
 */
TInt CSmfCredMgrDb::RegTokenValidityTableCreate()
	{
	TInt err(KErrNone);
	err = iDataBase.Exec(KCreateRegTokenValidityTable);
	if (err >= 0)
		{
		return KErrNone;
		}
	//delete file in case of incomplete table creation
	DeleteDbFile(iDbFilePathName);
	return err;
	}

/**
 * Creates the Authentication Parameters Table
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description.
 */
TInt CSmfCredMgrDb::AuthParamsTableCreate()
	{
	TInt err(KErrNone);
	err = iDataBase.Exec(KCreateAuthParamsTable);
	if (err >= 0)
		{
		return KErrNone;
		}
	//delete file in case of incomplete table creation
	DeleteDbFile(iDbFilePathName);
	return err;
	}

/**
 * Deletes the Db 
 * @param aFileName The Db file name
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description.
 */
TInt CSmfCredMgrDb::DeleteDbFile(const TFileName &aFileName)
	{
	//close the sql handle before deleting
	iDataBase.Close();
	TInt err = iDataBase.Delete(aFileName);
	return err;
	}

