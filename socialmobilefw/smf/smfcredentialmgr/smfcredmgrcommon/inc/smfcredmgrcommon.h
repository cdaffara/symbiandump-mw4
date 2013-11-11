/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Pritam Roy Biswas, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Description:
 * This header contains common data-types between the server and client
 */
/**
 *  @section CredentialMgr_info Implementation Information
 *  Currently there are 9 Apis exposed by the CredMgr Client.
 *
 *  The client is a Dll which will be used by Apps to get the CredMgr services.
 *  The server is a seperate EXE(process).
 *
 *  There is also a static LIB SmfCredMgrCommon which contains data structures that are
 *  common between the server-client.
 *
 *  Currently data is given to server to store in a Database using @ref storeAuthData()
 *  and the other APIs are used to retrieve the data from the server-database
 *  using SqLite queries.
 *
 * 	So, generally to store authentication data the flow is like
 * @msc {
 * SmfCredMgrClient, SmfCredMgrClient_p, RSmfCredMgrClientSession, SmfCredMgrServer, SmfCredMgrServerSession, CSmfCredMgrDbUser;
 * SmfCredMgrClient->SmfCredMgrClient_p  [label = "storeAuthData()"];
 * SmfCredMgrClient_p=>SmfCredMgrClient_p  [label = "ConversionsOfDatatypeAndSerializing"];
 * SmfCredMgrClient_p->RSmfCredMgrClientSession  [label = "RequestService()"];
 * RSmfCredMgrClientSession->SmfCredMgrServerSession  [label = "SendReceive()"];
 * SmfCredMgrServerSession=>SmfCredMgrServerSession  [label = "GenerateRegToken()"];
 * SmfCredMgrServerSession->CSmfCredMgrDbUser  [label = "InsertTables"];
 * SmfCredMgrClient_p<-SmfCredMgrServerSession  [label = "return = Registration Token"];
 * SmfCredMgrClient<-SmfCredMgrClient_p  [label = "return = Registration Token"];
 * };
 *
 *  The flag associated to each Plugin ID is now taken as a generic flag to entire
 * PluginList during storing in  storeAuthData() API ,i.e if Flag = 1 the
 * entire List of Plugins are enabled.
 *
 *  The NONCE is generated using rand() library function.
 *
 *  @Todo-  1)The validity check of the authentication data at the server side is an open item yet and is to be implemented.
 *  This will require the signal-slot implementation at the client side to notify authentication expiry, if decided that
 *  credentialmgr will launch authapp.
 *  2)The error handling and  state-machine (if required) are to be implemented.
 *  3) The process id of smfserver needs to be checked inside credmgr to ensure only smfserver reads the keys.
 *
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <e32base.h>
#include <e32debug.h>

/**
 *Name of the server.
 */
_LIT(KCredMgrServerName,"smfcredmgrserver");
_LIT(KCredMgrServerSemaphoreName, "smfcredmgrserversemaphore" );
_LIT(KCredMgrServerFilename, "smfcredmgrserver" );

const TUint KDefaultMessageSlots = 10;
/**
 * Version of the server
 */
const TUint KSecureServMajorVersionNumber = 1;
const TUint KSecureServMinorVersionNumber = 0;
const TUint KSecureServBuildVersionNumber = 0;

/**
 * @ingroup smf_credmgr_group
 *Enumeration to represent Cryptographic Algorithm useds
 */
enum SmfSignatureMethod
	{
	/**
	 * Enum for no algorithm
	 */
	ESmfNoSigningMethod = 0,

	/**
	 *Enum for algorithm RSA-SHA1
	 */
	ESMFRSAProtocol,

	/**
	 *Enum for algorithm HMAC-SHA1
	 */
	ESMFHMACProtocol,

	/**
	 *Enum for Plain Tezt
	 */
	ESMFPlainText,

	/**
	 *Enum for SHA256
	 */
	ESMFSHA256Protocol
	};

/**
 * Op codes supported by the server
 */
enum TCredentialServerRequestID
	{
	/**
	 * Op code to check plugin is authorised
	 */
	ECheckPluginAuthentication = 1,

	/**
	 * Op code to store entire set of data while authentication
	 */
	EStoreAuthData,

	/**
	 * Op code to send OAuth key set i.e SmfAuthParams
	 */
	ESendAuthDataSet,

	/**
	 * Op code to send authenticated plugin list
	 */
	ESendPluginIDList,

	/**
	 * Op code to send URL list.
	 */
	ESendURLList,

	/**
	 * Op code to update plugin list.
	 */
	EUpdatePluginIDList,

	/**
	 * Op code to store RSA keys.
	 */
	ESmfStoreRSAKey,

	/**
	 * Op code to sign message using RSA algo.
	 */
	ESmfRSASignMessage,

	/**
	 * Op code to sign using HAMC-SHA1 algo.
	 */
	ESmfHMACSHA1SignMessage,

	/**
	 * Op code to delete RSA keys.
	 */
	ESmfDeleteKeys
	};

#endif /* COMMON_H_ */
