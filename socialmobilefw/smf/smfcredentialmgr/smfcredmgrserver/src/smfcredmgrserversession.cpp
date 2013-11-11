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
 * Pritam Roy Biswas, Sasken Communication Technologies Ltd
 *
 * Description:
 * Header file for Credential Manager Server Session.
 */

#include <S32MEM.H>
#include <e32math.h>
#include <time.h>
#include <stdlib.h>
#include <random.h>
#include <smfcredmgrcommon.h>
#include <smfcredmgrclientdatastruct.h>
#include <smfutils.h>

#include "smfcredmgrserversession.h"
#include "smfcredmgrserver.h"
#include "smfcredmgrdbuser.h"

/**
 * NewL Method
 * @param aServer the server object
 * @return The constructed CSmfCredMgrServerSession instance
 */
CSmfCredMgrServerSession* CSmfCredMgrServerSession::NewL(
		CSmfCredMgrServer& aServer)
	{
	CSmfCredMgrServerSession* self = CSmfCredMgrServerSession::NewLC(aServer);
	CleanupStack::Pop(self);
	return self;
	}

/**
 * NewLC Method
 * @param aServer the server object
 * @return The constructed CSmfCredMgrServerSession instance
 */
CSmfCredMgrServerSession* CSmfCredMgrServerSession::NewLC(
		CSmfCredMgrServer& aServer)
	{
	CSmfCredMgrServerSession* self = new (ELeave) CSmfCredMgrServerSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * Constructor
 * @param aServer The server object
 */
CSmfCredMgrServerSession::CSmfCredMgrServerSession(CSmfCredMgrServer& aServer) :
	iServer(aServer)
	{
	RDebug::Printf("in constructor CSmfCredMgrServerSession");
	}

/**
 * Two-Phase constructor
 */
void CSmfCredMgrServerSession::ConstructL()
	{
	iDbUser = CSmfCredMgrDbUser::NewL(this);
	iKeyStore = CSmfKeyStoreManager::NewL();
	}

/**
 * Destructor
 */
CSmfCredMgrServerSession::~CSmfCredMgrServerSession()
	{
	RDebug::Printf("in destructor CSmfCredMgrServerSession");
	delete iDbUser;
	delete iKeyStore;
	}

/**
 * Handles the servicing of a client request that has been 
 * passed to the server
 * @param aMessage The message containing the details of the client request
 */
void CSmfCredMgrServerSession::ServiceL(const RMessage2& aMessage)
	{
	TInt err = KErrNone;
	switch (aMessage.Function())
		{
		case ESendAuthDataSet:
			{
			//create buffer to read the received data
			RBuf8 dataBuf;
			CleanupClosePushL(dataBuf);
			dataBuf.CreateL(aMessage.GetDesLength(0));
			aMessage.ReadL(0, dataBuf);
			CSmfFetchAuthTokenSet* fetchAuthTokenSetParams =
					new (ELeave) CSmfFetchAuthTokenSet;

			CleanupStack::PushL(fetchAuthTokenSetParams);

			fetchAuthTokenSetParams->InternalizeL(dataBuf);

			getTokenArrayL(fetchAuthTokenSetParams);

			//create buffer to serialize the data to be sent
			CBufFlat* buf = CBufFlat::NewL(512);
			CleanupStack::PushL(buf);
			RBufWriteStream stream(*buf);
			CleanupClosePushL(stream);

			fetchAuthTokenSetParams->ExternalizeL(stream);

			stream.CommitL();

			TPtr8 bufPtr = buf->Ptr(0);

			TInt err = aMessage.Write(1, bufPtr);

			CleanupStack::PopAndDestroy(&stream);
			CleanupStack::PopAndDestroy(buf);
			CleanupStack::PopAndDestroy(fetchAuthTokenSetParams);
			CleanupStack::PopAndDestroy(&dataBuf);
			aMessage.Complete(err);
			}
			break;

		case EUpdatePluginIDList:
			{
			//create buffer to read the received data
			RBuf8 dataBuf;
			CleanupClosePushL(dataBuf);
			dataBuf.CreateL(aMessage.GetDesLength(0));
			aMessage.ReadL(0, dataBuf);

			CSmfPluginIDUpdate* changePluginListParams =
					new (ELeave) CSmfPluginIDUpdate;
			CleanupStack::PushL(changePluginListParams);

			changePluginListParams->InternalizeL(dataBuf);

			iDbUser->updatePlugin(changePluginListParams->iNewPluginID->Des(),
					changePluginListParams->pluginIDEnabled,
					changePluginListParams->iOldPluginID->Des());

			CleanupStack::PopAndDestroy(changePluginListParams);
			CleanupStack::PopAndDestroy(&dataBuf);
			aMessage.Complete(err);
			}
			break;

		case ESendURLList:
			{
			//create buffer to read the received data
			RBuf8 dataBuf;
			CleanupClosePushL(dataBuf);
			dataBuf.CreateL(aMessage.GetDesLength(0));
			aMessage.ReadL(0, dataBuf);

			CSmfURLListParams* fetchURLListParams =
					new (ELeave) CSmfURLListParams;
			CleanupStack::PushL(fetchURLListParams);

			fetchURLListParams->InternalizeL(dataBuf);

			fetchUrlL(fetchURLListParams);

			//to serialize data we need a buffer
			CBufFlat* buf = CBufFlat::NewL(512);
			CleanupStack::PushL(buf);
			RBufWriteStream stream(*buf);
			CleanupClosePushL(stream);

			fetchURLListParams->ExternalizeL(stream);

			stream.CommitL();

			TPtr8 bufPtr = buf->Ptr(0);

			TInt err = aMessage.Write(1, bufPtr);

			CleanupStack::PopAndDestroy(&stream);
			CleanupStack::PopAndDestroy(buf);
			CleanupStack::PopAndDestroy(fetchURLListParams);
			CleanupStack::PopAndDestroy(&dataBuf);
			aMessage.Complete(err);
			}
			break;

		case ESendPluginIDList:
			{
			//create buffer to read
			RBuf8 dataBuf;
			CleanupClosePushL(dataBuf);
			dataBuf.CreateL(aMessage.GetDesLength(0));
			aMessage.ReadL(0, dataBuf);

			CSmfPluginIDListParams* fetchPluginListParams =
					new (ELeave) CSmfPluginIDListParams;
			CleanupStack::PushL(fetchPluginListParams);
			fetchPluginListParams->InternalizeL(dataBuf);

			fetchPluginIDsL(fetchPluginListParams);

			//to serialize data we need a buffer
			CBufFlat* buf = CBufFlat::NewL(512);
			CleanupStack::PushL(buf);
			RBufWriteStream stream(*buf);
			CleanupClosePushL(stream);
			fetchPluginListParams->ExternalizeL(stream);

			stream.CommitL();

			TPtr8 bufPtr = buf->Ptr(0);

			TInt err = aMessage.Write(1, bufPtr);

			CleanupStack::PopAndDestroy(&stream);
			CleanupStack::PopAndDestroy(buf);
			CleanupStack::PopAndDestroy(fetchPluginListParams);
			CleanupStack::PopAndDestroy(&dataBuf);
			aMessage.Complete(err);
			}
			break;

		case ECheckPluginAuthentication:
			{
			//create buffer to read
			RBuf8 dataBuf;
			CleanupClosePushL(dataBuf);

			dataBuf.CreateL(aMessage.GetDesLength(0));
			aMessage.ReadL(0, dataBuf);

			RDesReadStream stream(dataBuf);
			CleanupClosePushL(stream);

			TBool isAuthenticated = EFalse;

			HBufC* buf = SmfUtils::InternalizeDesL(stream);
			CleanupStack::PushL(buf);
			TBuf<KMaxBufSize> pluginIDBuf(buf->Des());

			TInt value = 0;
			iDbUser->readFlagInPluginIdTable(pluginIDBuf, value);
			if (value > 0)
				isAuthenticated = ETrue;
			else
				isAuthenticated = EFalse;

			HBufC8* isAuthenticatedBuf = HBufC8::NewLC(32);

			TPtr8 isAuthenticatedBufPtr(isAuthenticatedBuf->Des());
			isAuthenticatedBufPtr.AppendNum(isAuthenticated);

			TInt err = aMessage.Write(1, isAuthenticatedBufPtr);

			CleanupStack::PopAndDestroy(isAuthenticatedBuf);
			CleanupStack::PopAndDestroy(buf);
			CleanupStack::PopAndDestroy(&stream);
			CleanupStack::PopAndDestroy(&dataBuf);
			aMessage.Complete(err);
			}
			break;

		case EStoreAuthData:
			{
			//create buffer to read
			RBuf8 dataBuf;
			CleanupClosePushL(dataBuf);
			dataBuf.CreateL(aMessage.GetDesLength(0));
			aMessage.ReadL(0, dataBuf);

			CSmfStoreAuthParams* authenticationProcessData =
					new (ELeave) CSmfStoreAuthParams;
			CleanupStack::PushL(authenticationProcessData);

			authenticationProcessData->InternalizeL(dataBuf);

			//generate cryptographically secure random number.
     	    TRandom generator;
			TBuf8<KMaxRegistrationTokenLength> randNum;
			randNum.SetLength(randNum.MaxLength());
			generator.RandomL(randNum);
			
			//assign it to iRegistrationToken
			authenticationProcessData->iRegistrationToken = HBufC::NewL(/*regToken.Length()*/KMaxRegistrationTokenLength);;
	
			TPtr regTokenPtr(authenticationProcessData->iRegistrationToken->Des());
			regTokenPtr.Copy(randNum);

			storeInDb(authenticationProcessData);

			//to serialize data we need a buffer
			CBufFlat* buf = CBufFlat::NewL(KMaxBufSize);
			CleanupStack::PushL(buf);
			RBufWriteStream stream(*buf);
			CleanupClosePushL(stream);

			authenticationProcessData->ExternalizeL(stream);

			stream.CommitL();

			TPtr8 bufPtr = buf->Ptr(0);

			TInt err = aMessage.Write(1, bufPtr);

			CleanupStack::PopAndDestroy(&stream);
			CleanupStack::PopAndDestroy(buf);
			
			CleanupStack::PopAndDestroy(authenticationProcessData);
			CleanupStack::PopAndDestroy(&dataBuf);
			aMessage.Complete(err);
			}
			break;
			
		case ESmfHMACSHA1SignMessage:
		case ESmfRSASignMessage:
		case ESmfStoreRSAKey:
		case ESmfDeleteKeys:
			{
			iKeyStore->HandleMessageL(aMessage);
			}
			break;
		default:
			//todo -panic client
			break;
		}
	}

/**
 * Stores data to Db during Authentication process
 * @param aParams class containg the data to be stored at Db
 */
void CSmfCredMgrServerSession::storeInDb(
		CSmfStoreAuthParams* aAuthenticationProcessData)
	{
	TBuf<KMaxBufSize> authAppIDbuf(
			aAuthenticationProcessData->iAuthAppID->Des());
	TBuf<KMaxBufSize> regTokenBuf(aAuthenticationProcessData->iRegistrationToken->Des());
	iDbUser->RegTokenValidityTableInsert(
			regTokenBuf,
			authAppIDbuf, aAuthenticationProcessData->iValidity);

	for (int i = 0; i < aAuthenticationProcessData->iAuthTokenArray.Count(); i++)
		{
		iDbUser->AuthParamsTableInsert(
				authAppIDbuf,
				aAuthenticationProcessData->iAuthTokenArray[i].iKey->Des(),
				aAuthenticationProcessData->iAuthTokenArray[i].iSecret->Des());
		}

	//iterate pluginlist and insert in DB
	for (int i = 0; i < aAuthenticationProcessData->iPluginIDList.Count(); i++)
		{
		HBufC* buf = aAuthenticationProcessData->iPluginIDList[i];
		iDbUser->PluginIDTableInsert(buf->Des(), authAppIDbuf,
				aAuthenticationProcessData->pluginIDEnabled);
		}

	//iterate URLlist and insert in DB
	for (int i = 0; i < aAuthenticationProcessData->iURLList.Count(); i++)
		{
		HBufC* buf = aAuthenticationProcessData->iURLList[i];
		iDbUser->URLTableInsert(authAppIDbuf, buf->Des());
		}
	}

/**
 * Retrieves the plugin ids from Db
 * @param aParams class object to be updated
 */
void CSmfCredMgrServerSession::fetchPluginIDsL(CSmfPluginIDListParams* aParams)
	{
	TBuf<KMaxBufSize> tokenBuf(aParams->iRegistrationToken->Des());
	iDbUser->fetchPluginListL(tokenBuf, aParams->iPluginList);
	}

/**
 * Retrieves the URLs from Db
 * @param aArg class object to be updated
 */
void CSmfCredMgrServerSession::fetchUrlL(CSmfURLListParams* aArg)
	{
	TBuf<KMaxBufSize> authAppIDBuf;
	iDbUser->readAuthAppIdInPluginIdTable(aArg->iPluginID->Des(), authAppIDBuf);
	iDbUser->readUrlL(authAppIDBuf, aArg->iURLList);
	}

/**
 * Retrieves each token set from Db and updates the array of CSmfFetchAuthTokenSet
 * @param aParams class containg the array to be filled
 */
void CSmfCredMgrServerSession::getTokenArrayL(CSmfFetchAuthTokenSet* aParams)
	{
	TBuf<KMaxBufSize> authAppIDBuf;
	iDbUser->readAuthAppIdInRegTokenTable(aParams->iRegistrationToken->Des(),
			authAppIDBuf);
	iDbUser->readAuthTokensL(authAppIDBuf, aParams->iAuthTokenArray);
	}
