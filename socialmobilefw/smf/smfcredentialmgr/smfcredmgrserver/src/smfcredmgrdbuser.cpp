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
 * This file contains routines to handle the data-base operations like insertion, query etc. 
 * on Credential Manager Server Database
 */

#include <BAUTILS.H>

#include "smfcredmgrdbuser.h"
#include "smfcredmgrdb.h"

/**
 * NewL function
 * @param aSession the session object using this class
 * @return The constructed CSmfCredMgrDbUser instance
 */
CSmfCredMgrDbUser* CSmfCredMgrDbUser::NewL(CSmfCredMgrServerSession* aSession)
	{
	CSmfCredMgrDbUser* self = CSmfCredMgrDbUser::NewLC(aSession);
	CleanupStack::Pop(self);
	return (self);
	}

/**
 * NewLC function. This method adds the return value to the 
 * Cleanup Stack if constructed
 * @param aSession the session object using this class
 * @return The constructed CSmfCredMgrDbUser instance
 */
CSmfCredMgrDbUser* CSmfCredMgrDbUser::NewLC(CSmfCredMgrServerSession* aSession)
	{
	CSmfCredMgrDbUser* self = new (ELeave) CSmfCredMgrDbUser(aSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}

/**
 * Two phase constructor
 */
void CSmfCredMgrDbUser::ConstructL()
	{
	TInt err = iFileSession.Connect();
	User::LeaveIfError(err);

	iFileSession.SetSessionToPrivate(EDriveC);

	TFileName dbFileName;
	iFileSession.PrivatePath(dbFileName);
	TFindFile PrivFolder(iFileSession);

	// find drive-private path
	if (KErrNone == PrivFolder.FindByDir(dbFileName, KNullDesC))
		{
		dbFileName.Copy(PrivFolder.File());
		dbFileName.Append(KSmfCredMgrDbFileName);
		iDbFilePathName = dbFileName;
		if (BaflUtils::FileExists(iFileSession, dbFileName))
			{
			//err = iDataBase.Open(dbFileName);
			User::LeaveIfError(iDataBase.Open(dbFileName));
			RSqlDatabase::TIsolationLevel isolationLevel =
					RSqlDatabase::EReadUncommitted;
			iDataBase.SetIsolationLevel(isolationLevel);
			}
		else
			{
			//file not present
			User::Leave(KErrNotFound);
			}
		}
	else
		{
		//path not present
		User::Leave(KErrPathNotFound);
		}
	}

/**
 * Constructor
 * @param aSession The CSmfCredMgrServerSession instance
 */
CSmfCredMgrDbUser::CSmfCredMgrDbUser(CSmfCredMgrServerSession* aSession) :
	iSession(aSession)
	{
	}

/**
 * Destructor
 */
CSmfCredMgrDbUser::~CSmfCredMgrDbUser()
	{
	iFileSession.Close();
	iDataBase.Close();
	}

/**
 * Method to insert PluginIDTable
 * @param aPluginID The ID of the plugin 
 * @param aAuthAppId The ID of the Authentication app associated with the plugin
 * @param aEnableFlag A flag to indicate if the plugin is enabled, 
 * i.e, aEnableFlag = 0 for a disabled plugin
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description.
 */
TInt CSmfCredMgrDbUser::PluginIDTableInsert(const TDesC& aPluginId,
		const TDesC& aAuthAppId, TBool aEnableFlag)
	{
	TInt err(KErrNone);
	RSqlStatement sqlStatement;
	RSqlDatabase db;

	err = db.Open(iDbFilePathName);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	err = sqlStatement.Prepare(db, KSmfPluginIDTableInsert);
	TInt paramIndex(KErrNone);

	TBuf<KMaxBufSize> pluginbuf(aPluginId);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val1"));
	err = sqlStatement.BindText(paramIndex, pluginbuf);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val2"));
	err = sqlStatement.BindText(paramIndex, aAuthAppId);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val3"));
	err = sqlStatement.BindInt(paramIndex, aEnableFlag);

	err = db.Exec(KBegin);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	err = sqlStatement.Exec();
	if (KSqlErrConstraint == err)
		{
		//Table already present.
		}
	else if (err < KErrNone)
		{
		__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
		}

	err = db.Exec(KCommit);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	//if commit fails we have to roll back
	if (err < KErrNone)
		{
		err = db.Exec(KRollback);
		}
	sqlStatement.Close();
	db.Close();
	if (err >= 0)
		{
		return KErrNone;
		}
	return err;
	}

/**
 * Method to insert URLTable
 * @param aAuthAppId The ID of the Authentication app associated with the URLs 
 * @param aURL The URL to be stored
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description.
 */
TInt CSmfCredMgrDbUser::URLTableInsert(const TDesC& aAuthAppId,
		const TDesC& aURL)
	{
	TInt err(KErrNone);
	RSqlStatement sqlStatement;
	RSqlDatabase db;

	err = db.Open(iDbFilePathName);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	err = sqlStatement.Prepare(db, KSmfURLTableInsert);
	TInt paramIndex(KErrNone);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val1"));
	err = sqlStatement.BindText(paramIndex, aAuthAppId);

	TBuf<KMaxBufSize> urlbuf(aURL);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val2"));
	err = sqlStatement.BindText(paramIndex, urlbuf);

	err = db.Exec(KBegin);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	err = sqlStatement.Exec();
	if (KSqlErrConstraint == err)
		{
		//Table already present.
		}
	else if (err < KErrNone)
		{
		__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
		}

	err = db.Exec(KCommit);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	//if commit fails we have to roll back
	if (err < KErrNone)
		{
		err = db.Exec(KRollback);
		}
	sqlStatement.Close();
	db.Close();
	if (err >= 0)
		{
		return KErrNone;
		}
	return err;
	}

/**
 * Method to insert RegTokenValidityTable
 * @param aRegToken The Registration token for the authentication app 
 * @param aAuthAppId The ID of the Authentication app
 * @param aValidity The time by which the set will expire
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description. 
 */
TInt CSmfCredMgrDbUser::RegTokenValidityTableInsert(const TDesC& aRegToken,
		const TDesC& aAuthAppId, const TUint aValidity)
	{
	TInt err(KErrNone);
	RSqlStatement sqlStatement;
	RSqlDatabase db;

	TBuf<KMaxBufSize> regTokenBuf(aRegToken);

	err = db.Open(iDbFilePathName);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	err = sqlStatement.Prepare(db, KSmfRegTokenValidityTableInsert);
	TInt paramIndex(KErrNone);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val1"));
	err = sqlStatement.BindText(paramIndex, aRegToken);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val2"));
	err = sqlStatement.BindText(paramIndex, aAuthAppId);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val3"));
	err = sqlStatement.BindInt(paramIndex, aValidity);

	err = db.Exec(KBegin);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	err = sqlStatement.Exec();
	if (KSqlErrConstraint == err)
		{
		//Table already present.
		}
	else if (err < KErrNone)
		{
		__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
		}

	err = db.Exec(KCommit);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	//if commit fails we have to roll back
	if (err < KErrNone)
		{
		err = db.Exec(KRollback);
		}
	sqlStatement.Close();
	db.Close();
	if (err >= 0)
		{
		return KErrNone;
		}
	return err;

	}

/**
 * Method to insert AuthParamsTable
 * @param aAuthAppId The ID of the Authentication app
 * @param aKey The Key
 * @param aSecret The Secret
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description. 
 */
TInt CSmfCredMgrDbUser::AuthParamsTableInsert(const TDesC& aAuthAppId,
		const TDesC& aKey, const TDesC& aSecret)
	{
	TInt err(KErrNone);
	RSqlStatement sqlStatement;
	RSqlDatabase db;

	TBuf<KMaxBufSize> keyBuf(aKey);
	TBuf<KMaxBufSize> secretBuf(aSecret);

	err = db.Open(iDbFilePathName);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	err = sqlStatement.Prepare(db, KSmfAuthParamsTableInsert);
	TInt paramIndex(KErrNone);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val1"));
	err = sqlStatement.BindText(paramIndex, aAuthAppId);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val2"));
	err = sqlStatement.BindText(paramIndex, keyBuf);

	paramIndex = sqlStatement.ParameterIndex(_L(":Val3"));
	err = sqlStatement.BindText(paramIndex, secretBuf);

	err = db.Exec(KBegin);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	err = sqlStatement.Exec();
	if (KSqlErrConstraint == err)
		{
		//Table already present.
		}
	else if (err < KErrNone)
		{
		__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
		}

	err = db.Exec(KCommit);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	//if commit fails we have to roll back
	if (err < KErrNone)
		{
		err = db.Exec(KRollback);
		}
	sqlStatement.Close();
	db.Close();
	if (err >= 0)
		{
		return KErrNone;
		}
	return err;
	}

/**
 * Method to fetch all the plugins associated with the registration token
 * @param aRegToken The Registration token of the authentication app
 * @param aArray [out] The array to be updated with plugin ids
 */
void CSmfCredMgrDbUser::fetchPluginListL(const TDesC& aRegToken, RPointerArray<
		HBufC>& aArray)
	{
	TBuf<KMaxBufSize> authAppID;
	readAuthAppIdInRegTokenTable(aRegToken, authAppID);
	readPluginIdL(authAppID, aArray);
	}

/**
 * Method to Key-Secret pairs of the Authentication app
 * @param aAuthAppId The ID of the Authentication app
 * @param aArray [out] The array containing the key-secret pair
 */
void CSmfCredMgrDbUser::readAuthTokensL(const TDesC& aAuthAppId, RArray<
		TSmfAuthToken>& aArray)
	{
	TInt err(KErrNone);

	RSqlStatement sqlReadStatement;
	TInt paramIndex(KErrNone);

	err = sqlReadStatement.Prepare(iDataBase, KSmfDbReadAuthTokens);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	paramIndex = sqlReadStatement.ParameterIndex(_L(":iID"));
	err = sqlReadStatement.BindText(paramIndex, aAuthAppId);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	while ((err = sqlReadStatement.Next()) == KSqlAtRow)
		{
		//sometimes sqlStmt.Next returns KSqlAtRow even if no row is present
		if (!sqlReadStatement.IsNull(0))
			{
			TSmfAuthToken Set;

			Set.iKey = HBufC::NewL(KMaxAuthTokenLength);
			Set.iSecret = HBufC::NewL(KMaxAuthTokenLength);

			TBuf<KMaxBufSize> keyBuf;
			TBuf<KMaxBufSize> secretBuf;

			sqlReadStatement.ColumnText(0, keyBuf);
			sqlReadStatement.ColumnText(1, secretBuf);

			Set.iKey->Des().Copy(keyBuf);
			Set.iSecret->Des().Copy(secretBuf);
			//add it to the array
			aArray.Append(Set);

			}
		else
			{
			__ASSERT_DEBUG( 0, User::Invariant());
			}
		}
	sqlReadStatement.Close();
	}

/**
 * Method to fetch all the URLs associated with the Authentication app
 * @param aAuthAppId The ID of the Authentication app
 * @param aArray [out] The array to be updated with URLs 
 */
void CSmfCredMgrDbUser::readUrlL(const TDesC& aAuthAppId,
		RPointerArray<HBufC>& aArray)
	{
	TInt err(KErrNone);

	RSqlStatement sqlReadStatement;
	TInt paramIndex(KErrNone);

	err = sqlReadStatement.Prepare(iDataBase, KSmfDbReadURL);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	paramIndex = sqlReadStatement.ParameterIndex(_L(":iID"));
	err = sqlReadStatement.BindText(paramIndex, aAuthAppId);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	while ((err = sqlReadStatement.Next()) == KSqlAtRow)
		{
		//sometimes sqlStmt.Next returns KSqlAtRow even if no row is present
		if (!sqlReadStatement.IsNull(0))
			{
			TBuf<KMaxBufSize> urlBuf;
			HBufC* buf = HBufC::NewL(KMaxBufSize);
			sqlReadStatement.ColumnText(0, urlBuf);
			buf->Des().Copy(urlBuf);
			aArray.Append(buf);
			}
		else
			{
			__ASSERT_DEBUG( 0, User::Invariant());
			}
		}
	sqlReadStatement.Close();
	}

/**
 * Method to fetch Validity of the Authentication app
 * @param aAuthAppId The ID of the Authentication app
 * @param aValidity [out] The time by which the Auth set will expire
 */
void CSmfCredMgrDbUser::readValidity(const TDesC& aAuthAppId, TInt64& aValidity)
	{
	TInt err(KErrNone);

	RSqlStatement sqlReadStatement;
	TInt paramIndex(KErrNone);

	err = sqlReadStatement.Prepare(iDataBase, KSmfDbReadValidity);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());	
	paramIndex = sqlReadStatement.ParameterIndex(_L(":iID"));
	err = sqlReadStatement.BindText(paramIndex, aAuthAppId);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());	

	while ((err = sqlReadStatement.Next()) == KSqlAtRow)
		{
		//sometimes sqlStmt.Next returns KSqlAtRow even if no row is present
		if (!sqlReadStatement.IsNull(0))
			{
			aValidity = sqlReadStatement.ColumnInt64(1);
			}
		else
			{
			__ASSERT_DEBUG( 0, User::Invariant());
			}
		}
	sqlReadStatement.Close();

	}

/**
 * Method to fetch Registration token of the Authentication app from RegTokenValidityTable
 * @param aAuthAppId The ID of the Authentication app
 * @param aRegToken [out] The Registration token of the authentication app
 */
void CSmfCredMgrDbUser::readRegistrationTokenL(const TDesC& aAuthAppId,
		TDesC& aRegToken)
	{
	TInt err(KErrNone);

	RSqlStatement sqlReadStatement;
	TInt paramIndex(KErrNone);

	err = sqlReadStatement.Prepare(iDataBase, KSmfDbReadRegistrationToken);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	paramIndex = sqlReadStatement.ParameterIndex(_L(":iID"));
	err = sqlReadStatement.BindText(paramIndex, aAuthAppId);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	while ((err = sqlReadStatement.Next()) == KSqlAtRow)
		{
		//sometimes sqlStmt.Next returns KSqlAtRow even if no row is present
		if (!sqlReadStatement.IsNull(0))
			{
			aRegToken = sqlReadStatement.ColumnTextL(0);//aKey.AllocL();
			}
		else
			{
			__ASSERT_DEBUG( 0, User::Invariant());
			}
		}
	sqlReadStatement.Close();
	}

/**
 * Method to fetch Authentication app ID from RegTokenValidityTable 
 * giving the reg token
 * @param aRegToken The Registration token of the authentication app
 * @param aAuthAppId [out] The ID of the Authentication app
 */
void CSmfCredMgrDbUser::readAuthAppIdInRegTokenTable(const TDesC& aRegToken,
		TDes& aAuthAppId)
	{
	TInt err(KErrNone);

	RSqlStatement sqlReadStatement;
	TInt paramIndex(KErrNone);
	TInt64 duration;
	TBuf<KMaxBufSize> tokenBuf(aRegToken);
	err = sqlReadStatement.Prepare(iDataBase,
			KSmfDbReadAuthAppIdInRegTokenTable);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	paramIndex = sqlReadStatement.ParameterIndex(_L(":iID"));
	err = sqlReadStatement.BindText(paramIndex, tokenBuf);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	while ((err = sqlReadStatement.Next()) == KSqlAtRow)
		{
		//sometimes sqlStmt.Next returns KSqlAtRow even if no row is present
		if (!sqlReadStatement.IsNull(0))
			{
			duration = sqlReadStatement.ColumnInt(1);
			if (duration) //to be checked with epoch date-time
				{
				sqlReadStatement.ColumnText(0, aAuthAppId);
				}
			}
		else
			{
			__ASSERT_DEBUG( 0, User::Invariant());
			}
		}
	sqlReadStatement.Close();
	}


/**
 * Method to fetch Authentication app ID from PluginIDTable giving the plugin ID
 * @param aPluginID the ID of the plugin
 * @param aAuthAppId [out] ID of the Authentication app
 */
void CSmfCredMgrDbUser::readAuthAppIdInPluginIdTable(const TDesC& aPluginID,
		TDes& aAuthAppId)
	{
	TInt err(KErrNone);

	RSqlStatement sqlReadStatement;
	TInt paramIndex(KErrNone);

	err = sqlReadStatement.Prepare(iDataBase, KSmfDbReadAuthAppIdInPluginTable);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	paramIndex = sqlReadStatement.ParameterIndex(_L(":iID"));
	err = sqlReadStatement.BindText(paramIndex, aPluginID);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	while ((err = sqlReadStatement.Next()) == KSqlAtRow)
		{
		//sometimes sqlStmt.Next returns KSqlAtRow even if no row is present
		if (!sqlReadStatement.IsNull(0))
			{
			sqlReadStatement.ColumnText(0, aAuthAppId);
			}
		}
	sqlReadStatement.Close();
	}

/**
 * Method to read the IsEnabled flag from plugin Id table
 * @param aPluginID The ID of the plugin
 * @param aFlag [out] The flag that indicates the plugin id is enables or disabled
 */
void CSmfCredMgrDbUser::readFlagInPluginIdTable(const TDesC& aPluginID,
		TInt& aFlag)
	{
	TInt err(KErrNone);

	RSqlStatement sqlReadStatement;
	TInt paramIndex(KErrNone);

	err = sqlReadStatement.Prepare(iDataBase, KSmfDbReadFlagInPluginTable);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	paramIndex = sqlReadStatement.ParameterIndex(_L(":iID"));
	err = sqlReadStatement.BindText(paramIndex, aPluginID);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	while ((err = sqlReadStatement.Next()) == KSqlAtRow)
		{
		//sometimes sqlStmt.Next returns KSqlAtRow even if no row is present
		if (!sqlReadStatement.IsNull(0))
			{
			aFlag = sqlReadStatement.ColumnInt(0);
			}
		}
	sqlReadStatement.Close();
	}



/**
 * Method to change a plugin id in plugin Id table
 * @param aNewPluginID The ID of the new plugin
 * @param aFlag The flag that indicates the plugin id is enables or disabled
 * @param aOldPluginID The ID of the plugin to be replaced
 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
 * error codes for detailed error description.
 */
TInt CSmfCredMgrDbUser::updatePlugin(const TDesC& aPluginID,
		const TBool& aFlag, const TDesC& aOldPluginID)
	{
	TInt err(KErrNone);
	RSqlStatement sqlStatement;
	TInt paramIndex(KErrNone);
	TBuf<KMaxBufSize> newPluginBuf(aPluginID);
	TBuf<KMaxBufSize> OldPluginBuf(aOldPluginID);
	RSqlDatabase db;

	err = db.Open(iDbFilePathName);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	err = sqlStatement.Prepare(db, KUpdatePluginID);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	paramIndex = sqlStatement.ParameterIndex(_L(":iText"));
	err = sqlStatement.BindText(paramIndex, newPluginBuf);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	paramIndex = sqlStatement.ParameterIndex(_L(":iFlag"));
	err = sqlStatement.BindInt(paramIndex, aFlag);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	paramIndex = sqlStatement.ParameterIndex(_L(":iID"));
	err = sqlStatement.BindText(paramIndex, OldPluginBuf);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	err = db.Exec(KBegin);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	err = sqlStatement.Exec();
	if (KSqlErrConstraint == err)
		{
		//
		}
	else if (err < KErrNone)
		{
		__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
		}

	err = db.Exec(KCommit);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());

	//if commit fails we have to roll back
	if (err < KErrNone)
		{
		err = db.Exec(KRollback);
		}
	sqlStatement.Close();
	db.Close();
	if (err >= 0)
		{
		return KErrNone;
		}
	return err;
	}

/**
 * Method to fetch the list of plugin ids associated with Authentication App id, 
 * it is called internally by fetchPluginList().
 * @param aAuthAppId ID of the Authentication app
 * @param aArray [out] The array to be updated with plugin ids 
 */
void CSmfCredMgrDbUser::readPluginIdL(const TDesC& aAuthAppId, RPointerArray<
		HBufC>& aArray)
	{
	TInt err(KErrNone);
	RSqlStatement sqlReadStatement;
	TInt paramIndex(KErrNone);
	TInt flag;

	err = sqlReadStatement.Prepare(iDataBase, KSmfDbReadPluginID);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());	
	paramIndex = sqlReadStatement.ParameterIndex(_L(":iID"));
	err = sqlReadStatement.BindText(paramIndex, aAuthAppId);
	__ASSERT_DEBUG( (err >= KErrNone), User::Invariant());
	
	while ((err = sqlReadStatement.Next()) == KSqlAtRow)
		{
		//sometimes sqlStmt.Next returns KSqlAtRow even if no row is present
		if (!sqlReadStatement.IsNull(0))
			{
			flag = sqlReadStatement.ColumnInt(1);
			if (flag > 0)
				{
				TBuf<KMaxBufSize> pluginbuf;
				HBufC* buf = HBufC::NewL(KMaxBufSize);
				sqlReadStatement.ColumnText(0, pluginbuf);
				buf->Des().Copy(pluginbuf);
				aArray.Append(buf);
				}
			}
		else
			{
			__ASSERT_DEBUG( 0, User::Invariant());
			}
		}
	sqlReadStatement.Close();
	}
