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
 * This header provides a handle to use the data-base operations like insertion, query etc. which
 * is used by the Credential Manager Server Sessions to operate on
 * Credential Manager Server Database
 */

#ifndef SMFCREDMGRDBUSER_H_
#define SMFCREDMGRDBUSER_H_

#include <sqldb.h>
#include <e32base.h>
#include <f32file.h>

#include "smfcredmgrserversession.h"

//DB COMMANDS
_LIT(KBegin , "BEGIN");
_LIT(KCommit, "COMMIT");
_LIT(KRollback, "ROLLBACK");

//INSERTION
_LIT(KSmfPluginIDTableInsert, "INSERT OR REPLACE INTO PluginIDTable(PluginId, AuthAppId, IsEnabled) VALUES(:Val1, :Val2, :Val3)");
_LIT(KSmfURLTableInsert, "INSERT OR REPLACE INTO URLTable(AuthAppId, URL) VALUES(:Val1, :Val2)");
_LIT(KSmfRegTokenValidityTableInsert, "INSERT OR REPLACE INTO RegTokenValidityTable(RegistrationToken, AuthAppId, Validity) VALUES(:Val1, :Val2, :Val3)");
_LIT(KSmfAuthParamsTableInsert, "INSERT OR REPLACE INTO AuthParamsTable(AuthAppId, AuthKey, AuthSecret) VALUES(:Val1, :Val2, :Val3)");

//READ
_LIT(KSmfDbReadValidity, "SELECT Validity FROM RegTokenValidityTable WHERE AuthAppId = :iID");
_LIT(KSmfDbReadRegistrationToken, "SELECT RegistrationToken FROM RegTokenValidityTable WHERE AuthAppId = :iID");
_LIT(KSmfDbReadAuthTokens, "SELECT AuthKey, AuthSecret FROM AuthParamsTable WHERE AuthAppId = :iID");
_LIT(KSmfDbReadPluginID, "SELECT PluginId, IsEnabled FROM PluginIDTable WHERE AuthAppId = :iID");
_LIT(KSmfDbReadURL, "SELECT URL FROM URLTable WHERE AuthAppId = :iID");
_LIT(KSmfDbReadAuthAppIdInPluginTable, "SELECT AuthAppId FROM PluginIDTable WHERE PluginId = :iID");
_LIT(KSmfDbReadFlagInPluginTable, "SELECT IsEnabled FROM PluginIDTable WHERE PluginId = :iID");
_LIT(KSmfDbReadAuthAppIdInRegTokenTable, "SELECT AuthAppId, Validity FROM RegTokenValidityTable WHERE RegistrationToken = :iID");

//UPDATE 
_LIT( KUpdatePluginID, "UPDATE PluginIDTable SET PluginId =:iText, IsEnabled =:iFlag WHERE PluginId = :iID");

/**
 * The class to use the server database i.e insert table in Db, query from Db.
 */
class CSmfCredMgrDbUser : public CBase
	{
public:
	/**
	 * NewL function
	 * @param aSession the session object using this class
	 * @return The constructed CSmfCredMgrDbUser instance
	 */
	static CSmfCredMgrDbUser* NewL(CSmfCredMgrServerSession* aSession);
	
	/**
	 * NewLC function. This method adds the return value to the 
	 * Cleanup Stack if constructed
	 * @param aSession the session object using this class
	 * @return The constructed CSmfCredMgrDbUser instance
	 */
	static CSmfCredMgrDbUser* NewLC(CSmfCredMgrServerSession* aSession);

	/**
	 * Destructor
	 */
	~CSmfCredMgrDbUser();

public:
	/**
	 * Method to insert PluginIDTable
	 * @param aPluginID The ID of the plugin 
	 * @param aAuthAppId The ID of the Authentication app associated with the plugin
	 * @param aEnableFlag A flag to indicate if the plugin is enabled, 
	 * i.e, aEnableFlag = 0 for a disabled plugin
	 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
	 * error codes for detailed error description.
	 */
	TInt PluginIDTableInsert(const TDesC& aPluginID, 
			const TDesC& aAuthAppId,
			TBool aEnableFlag);

	/**
	 * Method to insert URLTable
	 * @param aAuthAppId The ID of the Authentication app associated with the URLs 
	 * @param aURL The URL to be stored
	 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
	 * error codes for detailed error description.
	 */
	TInt URLTableInsert(const TDesC& aAuthAppId, const TDesC& aURL);

	/**
	 * Method to insert RegTokenValidityTable
	 * @param aRegToken The Registration token for the authentication app 
	 * @param aAuthAppId The ID of the Authentication app
	 * @param aValidity The time by which the set will expire
	 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
	 * error codes for detailed error description. 
	 */
	TInt RegTokenValidityTableInsert(const TDesC& aRegToken,
			const TDesC& aAuthAppId, const TUint aValidity);
	
	/**
	 * Method to insert AuthParamsTable
	 * @param aAuthAppId The ID of the Authentication app
	 * @param aKey The Key
	 * @param aSecret The Secret
	 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
	 * error codes for detailed error description. 
	 */
	TInt AuthParamsTableInsert(const TDesC& aAuthAppId, const TDesC& aKey,
			const TDesC& aSecret);

	/**
	 * Method to fetch all the plugins associated with the registration token
	 * @param aRegToken The Registration token of the authentication app
	 * @param aArray [out] The array to be updated with plugin ids
	 */
	void fetchPluginListL(const TDesC& aRegToken, RPointerArray<HBufC>& aArray);

	/**
	 * Method to Key-Secret pairs of the Authentication app
	 * @param aAuthAppId The ID of the Authentication app
	 * @param aArray [out] The array containing the key-secret pair
	 */
	void readAuthTokensL(const TDesC& aAuthAppId, RArray<TSmfAuthToken>& aArray);

	/**
	 * Method to fetch all the URLs associated with the Authentication app
	 * @param aAuthAppId The ID of the Authentication app
	 * @param aArray [out] The array to be updated with URLs 
	 */
	void readUrlL(const TDesC& aAuthAppId, RPointerArray<HBufC>& aArray);

	/**
	 * Method to fetch Validity of the Authentication app
	 * @param aAuthAppId The ID of the Authentication app
	 * @param aValidity [out] The time by which the Auth set will expire
	 */
	void readValidity(const TDesC& aAuthAppId, TInt64& aValidity);

	/**
	 * Method to fetch Registration token of the Authentication app from RegTokenValidityTable
	 * @param aAuthAppId The ID of the Authentication app
	 * @param aRegToken [out] The Registration token of the authentication app
	 */
	void readRegistrationTokenL(const TDesC& aAuthAppId, TDesC& aRegToken);

	/**
	 * Method to fetch Authentication app ID from RegTokenValidityTable 
	 * giving the reg token
	 * @param aRegToken The Registration token of the authentication app
	 * @param aAuthAppId [out] The ID of the Authentication app
	 */
	void readAuthAppIdInRegTokenTable(const TDesC& aRegToken, TDes& aAuthAppId);

	/**
	 * Method to fetch Authentication app ID from PluginIDTable giving the plugin ID
	 * @param aPluginID the ID of the plugin
	 * @param aAuthAppId [out] ID of the Authentication app
	 */
	void readAuthAppIdInPluginIdTable(const TDesC& aPluginID, TDes& aAuthAppId);

	/**
	 * Method to read the IsEnabled flag from plugin Id table
	 * @param aPluginID The ID of the plugin
	 * @param aFlag [out] The flag that indicates the plugin id is enables or disabled
	 */
	void readFlagInPluginIdTable(const TDesC& aPluginID, TInt& aFlag);

	/**
	 * Method to change a plugin id in plugin Id table
	 * @param aNewPluginID The ID of the new plugin
	 * @param aFlag The flag that indicates the plugin id is enables or disabled
	 * @param aOldPluginID The ID of the plugin to be replaced
	 * @return Returns KErrNone if success. Refer ESqlDbError and system-wide 
	 * error codes for detailed error description.
	 */
	TInt updatePlugin(const TDesC& aPluginID, const TBool& aFlag,
			const TDesC& aOldPluginID);

	/**
	 * Method to fetch the list of plugin ids associated with Authentication App id, 
	 * it is called internally by fetchPluginList().
	 * @param aAuthAppId ID of the Authentication app
	 * @param aArray [out] The array to be updated with plugin ids 
	 */
	void readPluginIdL(const TDesC& aAuthAppId, RPointerArray<HBufC>& aArray);

private:
	/**
	 * Constructor
	 * @param aSession The CSmfCredMgrServerSession instance
	 */
	CSmfCredMgrDbUser(CSmfCredMgrServerSession* aSession);

	/**
	 * Two phase constructor
	 */
	void ConstructL();

private:
	//data
	RFs iFileSession;
	TFileName iDbFilePathName;
	RSqlDatabase iDataBase;
	CSmfCredMgrServerSession* iSession;
	};

#endif /* SMFCREDMGRDBUSER_H_ */
