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
 * This is the private implementation Class on Symbian OS for Credential Manager Client.
 *  Derives from CAtive to support asynchronous requests.
 *
 */

//  Include Files
#include <smfcredmgrcommon.h>
#include <smfcredmgrclientdatastruct.h>
#include <smfutils.h>
#include <securitydefs.h>
#include "smfcredmgrclientutil.h"
#include "smfcredmgrclient_p.h"

CSmfCredMgrClientSymbian::CSmfCredMgrClientSymbian(
		SmfCredMgrClient* aPublicImpl) :
		CActive(EPriorityStandard),
		iPublicImpl(aPublicImpl) 
	{

	}

CSmfCredMgrClientSymbian* CSmfCredMgrClientSymbian::NewL(
		SmfCredMgrClient* aPublicImpl)
	{
	CSmfCredMgrClientSymbian* self = NewLC(aPublicImpl);
	CleanupStack::Pop(self);
	return (self);
	}

CSmfCredMgrClientSymbian* CSmfCredMgrClientSymbian::NewLC(
		SmfCredMgrClient* aPublicImpl)
	{
	CSmfCredMgrClientSymbian* self = new (ELeave) CSmfCredMgrClientSymbian(
			aPublicImpl);
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}

void CSmfCredMgrClientSymbian::ConstructL()
	{
	User::LeaveIfError(iSession.connectToServer());
	}

CSmfCredMgrClientSymbian::~CSmfCredMgrClientSymbian()
	{
	//cancel pending request before close session
	Cancel();
	iSession.Close();
	}

void CSmfCredMgrClientSymbian::RunL()
	{

	}

void CSmfCredMgrClientSymbian::DoCancel()
	{

	}

TBool CSmfCredMgrClientSymbian::AuthDataSetL(QString RegToken,
		QDateTime Validity, SmfAuthParams& AuthTokenSet)
	{
	RDebug::Printf("+In AuthDataSetL()");
	CSmfFetchAuthTokenSet* fetchAuthTokenSetParams =
			new (ELeave) CSmfFetchAuthTokenSet;

	CleanupStack::PushL(fetchAuthTokenSetParams);

	//fill the input params
	fetchAuthTokenSetParams->iRegistrationToken = qt_QString2HBufC(RegToken);
	fetchAuthTokenSetParams->iValidity = Validity.toTime_t();

	//create buffer to serialize data
	CBufFlat* buf = CBufFlat::NewL(KMaxBufSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	fetchAuthTokenSetParams->ExternalizeL(stream);
	stream.CommitL();

	TPtr8 bufPtr = buf->Ptr(0);

	TIpcArgs args;
	args.Set(0, &bufPtr);

	// to get the data from server, we create a space.
	HBufC8* retBuf = HBufC8::NewL(KMaxBufSize);
	CleanupStack::PushL(retBuf);

	TPtr8 outputptr = retBuf->Des();
	args.Set(1, &outputptr);

	iSession.RequestService(ESendAuthDataSet, args);

	//create buffer to read data sent by server
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);
	dataBuf.Create(outputptr);

	fetchAuthTokenSetParams->InternalizeL(dataBuf);


	CleanupStack::PopAndDestroy(&dataBuf);
	CleanupStack::PopAndDestroy(retBuf);
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);

	//return EFalse if count is 0
	if (fetchAuthTokenSetParams->iAuthTokenArray.Count() == 0)
		{
		CleanupStack::PopAndDestroy(fetchAuthTokenSetParams);
		RDebug::Printf("-In AuthDataSetL()");
		return EFalse;
		}

	smfcredmgrclientutil::RArrayToSmfAuthParams(
			fetchAuthTokenSetParams->iAuthTokenArray, AuthTokenSet);


	CleanupStack::PopAndDestroy(fetchAuthTokenSetParams);
	RDebug::Printf("-In AuthDataSetL()");
	return ETrue;
	}

QString CSmfCredMgrClientSymbian::storeAuthDataL(SmfAuthParams Set,
		QDateTime Validity, QList<QUrl> URLList, QStringList PluginList,
		QString AuthAppAID, bool Flag)
	{
	RDebug::Printf("+In storeAuthDataL()");
	CSmfStoreAuthParams* authenticationProcessData =
			new (ELeave) CSmfStoreAuthParams;
	CleanupStack::PushL(authenticationProcessData);

	//create space for iRegistrationToken
	authenticationProcessData->iRegistrationToken = HBufC::NewL(
			KMaxRegistrationTokenLength);


	//fill the input params
	smfcredmgrclientutil::SmfAuthParamstoRArray(Set,
			authenticationProcessData->iAuthTokenArray);
	authenticationProcessData->pluginIDEnabled = Flag;
	//set iValidity
	authenticationProcessData->iValidity = Validity.toTime_t();


	//set authappid
	authenticationProcessData->iAuthAppID = qt_QString2HBufC(AuthAppAID);
	//set the lists
	authenticationProcessData->iURLList
			= smfcredmgrclientutil::convertToSymbianURLList(URLList);
	authenticationProcessData->iPluginIDList
			= smfcredmgrclientutil::convertToSymbianPluginList(
					PluginList);

	//create buffer to serialize data
	CBufFlat* buf = CBufFlat::NewL(KMaxBufSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	authenticationProcessData->ExternalizeL(stream);
	stream.CommitL();

	TPtr8 bufPtr = buf->Ptr(0);

	TIpcArgs args;
	args.Set(0, &bufPtr);

	
	// to get the data from server, we create a space.
	HBufC8* retBuf = HBufC8::NewL(KMaxBufSize);
	CleanupStack::PushL(retBuf);

	TPtr8 outputptr = retBuf->Des();
	args.Set(1, &outputptr);

	iSession.RequestService(EStoreAuthData, args);

	//create buffer to read data received
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);
	dataBuf.Create(outputptr);

	authenticationProcessData->InternalizeL(dataBuf);

	TPtr regTokenPtr(
			authenticationProcessData->iRegistrationToken->Des());

	//convert and return
	QString regString = qt_TDes2QString( regTokenPtr );

	CleanupStack::PopAndDestroy(&dataBuf);
	CleanupStack::PopAndDestroy(retBuf);
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::Pop();

	return regString;
	}

TBool CSmfCredMgrClientSymbian::isPluginAuthenticatedL(QString PluginID)
	{
	CBufFlat* buf = CBufFlat::NewL(KMinBufSize);
	CleanupStack::PushL(buf);

	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	TPtr idPtr(qt_QString2HBufC(PluginID)->Des());
	SmfUtils::ExternalizeDesL(idPtr, stream);

	stream.CommitL();

	// to get the data from server, we create a space.
	HBufC8* retBuf = HBufC8::NewL(32);
	CleanupStack::PushL(retBuf);

	TPtr8 bufPtr = buf->Ptr(0);
	TPtr8 flag(retBuf->Des());

	TIpcArgs args;

	args.Set(0, &bufPtr);
	args.Set(1, &flag);

	iSession.RequestService(ECheckPluginAuthentication, args);

	TLex8 iLex = TLex8(flag);
	TInt value = 0;
	iLex.Val(value);

	CleanupStack::PopAndDestroy(retBuf);
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);

	if (value)
		{
		RDebug::Printf("flag returned is ETrue");
		return ETrue;
		}
	else
		{
		RDebug::Printf("flag returned is EFalse");
		return EFalse;
		}
	}

void CSmfCredMgrClientSymbian::authenticatedPluginListL(
		QString RegistrationToken, QStringList& List)
	{
	CSmfPluginIDListParams* fetchPluginListParams =
			new (ELeave) CSmfPluginIDListParams;
	CleanupStack::PushL(fetchPluginListParams);

	//fill input params
	fetchPluginListParams->iRegistrationToken = qt_QString2HBufC(
			RegistrationToken);

	//create buffer to serialize data
	CBufFlat* buf = CBufFlat::NewL(KMinBufSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	fetchPluginListParams->ExternalizeL(stream);
	stream.CommitL();

	TPtr8 bufPtr1 = buf->Ptr(0);
	TIpcArgs args;
	args.Set(0, &bufPtr1);

	// to get the data from server, we create a space.
	HBufC8* retBuf = HBufC8::NewL(KMaxBufSize);
	CleanupStack::PushL(retBuf);

	TPtr8 outputptr = retBuf->Des();
	args.Set(1, &outputptr);

	iSession.RequestService(ESendPluginIDList, args);

	//create buffer to read data received
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);
	dataBuf.Create(outputptr);

	fetchPluginListParams->InternalizeL(dataBuf);

	smfcredmgrclientutil::convertToQStringList(
			fetchPluginListParams->iPluginList, List);

	CleanupStack::PopAndDestroy(&dataBuf);
	CleanupStack::PopAndDestroy(retBuf);
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(fetchPluginListParams);
	}

void CSmfCredMgrClientSymbian::URLListL(QString PluginID, QList<QUrl>& List)

	{
	CSmfURLListParams* fetchURLListParams = new (ELeave) CSmfURLListParams;
	CleanupStack::PushL(fetchURLListParams);

	fetchURLListParams->iPluginID = qt_QString2HBufC(PluginID);

	//create buffer to serialize data
	CBufFlat* buf = CBufFlat::NewL(KMinBufSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	fetchURLListParams->ExternalizeL(stream);
	stream.CommitL();

	TPtr8 bufPtr = buf->Ptr(0);

	TIpcArgs args;
	args.Set(0, &bufPtr);

	// to get the data from server, we create a space.
	HBufC8* retBuf = HBufC8::NewL(KMaxBufSize);
	CleanupStack::PushL(retBuf);

	TPtr8 outputptr = retBuf->Des();
	args.Set(1, &outputptr);

	iSession.RequestService(ESendURLList, args);

	//create buffer to read data received
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);
	dataBuf.Create(outputptr);

	fetchURLListParams->InternalizeL(dataBuf);

	smfcredmgrclientutil::convertToQUrlList(
			fetchURLListParams->iURLList, List);

	CleanupStack::PopAndDestroy(&dataBuf);
	CleanupStack::PopAndDestroy(retBuf);
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::Pop(fetchURLListParams);

	}

QString CSmfCredMgrClientSymbian::generateNONCE(const qint64 Length)
	{
	srand(time(0));
	QDateTime UniqueNumber = QDateTime::currentDateTime();

	//read upto milliseconds
	QString RetString(UniqueNumber.toString("hh:mm:ss.zzz"));

	QString Letters(
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

	//append a randomly generated string to RetString
	for (int i = RetString.count(); i < Length; i++)
		{
		RetString.insert((i), Letters.at(rand() % Letters.size()));
		}

	return RetString;
	}

void CSmfCredMgrClientSymbian::changePluginIDListL(const QString NewPluginID,
		const bool Flag, const QString OldPluginID)
	{
	CSmfPluginIDUpdate* changePluginListParams =
			new (ELeave) CSmfPluginIDUpdate;
	CleanupStack::PushL(changePluginListParams);

	//set the input params
	changePluginListParams->iNewPluginID = qt_QString2HBufC(NewPluginID);
	changePluginListParams->iOldPluginID = qt_QString2HBufC(OldPluginID);
	changePluginListParams->pluginIDEnabled = Flag;

	//create buffer to serialize data
	CBufFlat* buf = CBufFlat::NewL(KMaxBufSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	changePluginListParams->ExternalizeL(stream);
	stream.CommitL();

	TPtr8 bufPtr = buf->Ptr(0);

	TIpcArgs args;
	args.Set(0, &bufPtr);

	iSession.RequestService(EUpdatePluginIDList, args);

	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(changePluginListParams);

	}

SMFCredMgrErrorCode CSmfCredMgrClientSymbian::signMessageL(QString Message,
		QString Key, QString& Signature, SmfSignatureMethod AlgorithmUsed)
	{
	SMFCredMgrErrorCode signError = SmfErrNone;
	TPtr msgPtr((qt_QString2HBufC(Message))->Des());
	TPtr keyPtr((qt_QString2HBufC(Key))->Des());

	CSmfSignParameters* signMsgParams = CSmfSignParameters::NewL(
			msgPtr.Collapse(), keyPtr.Collapse());
	CleanupStack::PushL(signMsgParams);

	CBufFlat* buf = CBufFlat::NewL(KMinBufSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	signMsgParams->ExternalizeL(stream);
	stream.CommitL();

	TPtr8 bufPtr = buf->Ptr(0);

	TIpcArgs args;
	args.Set(0, &bufPtr);

	HBufC8* msgBuf = HBufC8::NewL(KMaxSignedMsgLength);
	TPtr8 msgBufPtr(msgBuf->Des());
	CleanupStack::PushL(msgBuf);
	args.Set(1, &msgBufPtr);

	switch (AlgorithmUsed)
		{
		case ESMFRSAProtocol:
			{
			iSession.RequestService(ESmfRSASignMessage, args);
			}
			break;
		case ESMFHMACProtocol:
			{
			iSession.RequestService(ESmfHMACSHA1SignMessage, args);
			}
			break;
		default:
			{
			RDebug::Printf("Unsupported Algo:");
			return SmfErrBadParameter;
			}
		}

	TBuf<KMaxSignedMsgLength> signedMsg;
	signedMsg.Copy(msgBufPtr);

	Signature = qt_TDesC2QString(signedMsg);

	CleanupStack::Pop(msgBuf);
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(signMsgParams);

	return signError;
	}

QString CSmfCredMgrClientSymbian::storeRSAKeysL(const QString KeyLabel,
		const QString keydata, const QDateTime Validity)
	{
	RDebug::Printf("Sending store RSA key message to server");

	TPtrC labelPtr(qt_QString2TPtrC(KeyLabel));
	TPtr dataPtr((qt_QString2HBufC(keydata)->Des()));

	QDateTime CurrentTime = QDateTime::currentDateTime();
	TTimeIntervalSeconds duration(CurrentTime.secsTo(Validity));

	TTime startDate;
	startDate.UniversalTime();

	TTime endDate(startDate);
	endDate += duration;

	CSmfRsaKeyParameters* storeRSAKeysparams = CSmfRsaKeyParameters::NewL(
			labelPtr, startDate, endDate, (dataPtr.Collapse()));
	CleanupStack::PushL(storeRSAKeysparams);

	CBufFlat* buf = CBufFlat::NewL(KMinBufSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	storeRSAKeysparams->ExternalizeL(stream);
	stream.CommitL();

	TPtr8 bufPtr = buf->Ptr(0);

	TIpcArgs args;
	args.Set(0, &bufPtr);

	CleanupStack::PopAndDestroy(&stream);

	HBufC8* retBuf = HBufC8::NewLC(KSHA1HashLengthBytes);
	TPtr8 retBufPtr(retBuf->Des());
	args.Set(1, &retBufPtr);

	iSession.RequestService(ESmfStoreRSAKey, args);

	RDebug::Printf("SMF: Message completed");

	TBuf<KSHA1HashLengthBytes> key;
	key.Copy(retBufPtr);

	QString RetString(qt_TDesC2QString(key));

	CleanupStack::Pop(retBuf);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(storeRSAKeysparams);

	RDebug::Printf("-In storeAuthDataL()");
	
	return (RetString);

	}

void CSmfCredMgrClientSymbian::deleteRSAKey(QString KeyLabel)
	{
	TPtr bufPtr((qt_QString2HBufC(KeyLabel))->Des());

	TIpcArgs args;
	args.Set(0, &bufPtr);

	iSession.RequestService(ESmfDeleteKeys, args);
	}
