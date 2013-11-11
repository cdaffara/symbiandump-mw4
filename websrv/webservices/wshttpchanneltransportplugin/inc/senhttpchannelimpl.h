/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Header description       
*
*/











#ifndef SEN_HTTP_CHANNEL_IMPL_H
#define SEN_HTTP_CHANNEL_IMPL_H

// INCLUDES
#include <badesca.h>
#include <http/mhttpauthenticationcallback.h>
#include <flogger.h>
#include <es_sock.h>
#include <commdbconnpref.h>

#include "senhttpchannel.h"
#include "MSenElement.h"
#include "SenIdentityProvider.h"
#include "senpointermap.h"
#include "senmtomtxnstate.h"

_LIT(KColon,                            ":");

// Multipart message
_LIT8(KMimeAccept, "*/*");

// Panics
_LIT(KUnrecognisedValueTypeOfContentTypePanicText, "ContentType has unrecognised value");

// FORWARD DECLARATIONS
class CSenHttpEventHandler;
class CSenTxnState;
class CSenAtomEntry;
//class CSenMtomTxnState;
class MSenIdentityManager;
//class CSenElement;
class CSenLayeredHttpTransportProperties;

// CLASS DECLARATION
class CSenHttpChannelImpl : public CSenHttpChannel,
                            public MHTTPAuthenticationCallback
    {
public:
    static CSenHttpChannelImpl* NewL(MSenIdentityManager& aManager);
    static CSenHttpChannelImpl* NewLC(MSenIdentityManager& aManager);
    static CSenHttpChannelImpl* NewL(MSenIdentityManager& aManager, TUint32 aIapId);
    static CSenHttpChannelImpl* NewLC(MSenIdentityManager& aManager, TUint32 aIapId);

    virtual ~CSenHttpChannelImpl();

    void HandleResponseHeadersL(RHTTPTransaction aTransaction);
    void HandleResponseBodyDataL(RHTTPTransaction aTransaction);
    void HandleResponseL(RHTTPTransaction aTransaction);
    void HandleRunErrorL(RHTTPTransaction aTransaction, TInt aError);
    void HandleRedirectRequiresConfirmationL(RHTTPTransaction aTransaction);

    // Functions from base classes
    virtual TInt SendL(MSenResponseObserver& aObserver,
                       const TDesC8& aUri,
                       const TDesC8& aContent,
                       CSenLayeredHttpTransportProperties& aProps);
                       
    virtual TInt SendL(MSenResponseObserver& aObserver,
                 	   const TDesC8& aUri,
                       CSenSoapEnvelope2& aContent, 
                       CSenLayeredHttpTransportProperties& aProps);
    virtual TInt SendL(MSenResponseObserver& aObserver,
                           	    const TDesC8& aUri,
                               	CSenAtomEntry& aContent, 
                               	CSenLayeredHttpTransportProperties& aProps);
                               	
	virtual TInt SendL(MSenResponseObserver& aObserver,
						const TDesC8& aUri,
						const RFile& aFile,
						CSenLayeredHttpTransportProperties& aProps);
					                                                      	
     
    virtual void SetProxyL(const TDesC8& aProxyHostBaseAddr,
                           TInt aProxyPort);
    virtual void SetProxyL(const TDesC8& aProxyAddrStr);
    virtual void SetHttpVersionL(TInt aVersion);
    virtual void SetProxyUsageL(TBool aProxyUsage);
    virtual void SetSecureDialogL(TBool aSecureDialog);

    TInt CancelTransaction(const TInt aTxnId);
    void CancelAllTransactions();

    virtual void SetExplicitIapDefined(TBool aExplicitIapDefined);
    /*
    * Getter for currently effective IAP ID
    * @param aIapId will be assigned to hold
    * the value of that Internet Access Point
    * identifier (an unsigned integer utilized
    * by CommsDB API).
    * @return EFalse, if IAP ID has not been
    * specified. Otherwise, returns ETrue.
    */
    virtual TBool EffectiveIapId(TUint32 &aIapId);

    // Helper, for file logging. 
    //RFileLogger* Log() const;
    void NotifyMoreBodyL();

    // Functions from MHTTPAuthenticationCallback
    virtual TBool GetCredentialsL(const TUriC8& aURI, RString aRealm,
                                  RStringF aAuthenticationType,
                                  RString& aUsername,
                                  RString& aPassword);
    void EnableTimeOutL(TInt aTxnId, TInt aTimeOutSec);
    void DisableTimeOutL(TInt aTxnId);
    virtual TInt32 UsedIap();
    virtual void ResetIapId();
    virtual void ResetUsedIapId();
    
protected:
    CSenHttpChannelImpl(MSenIdentityManager& aManager);

    void AddRequestHeadersL(RHTTPHeaders& aHeaders, CSenLayeredHttpTransportProperties& aProps);
    void AppendNewTxnStateL(CSenTxnState* aTxnState);
    void DeleteTxnState(TInt aTxnId);
    CSenTxnState* FindTxnState(TInt aTxnId, TInt* aIndex = NULL);
    /**
    * Applies new IAP preferencies to current HTTP Session (RHTTPSession)
    * @param aIapId new IAP ID
    * @param aDialogPref shows is IAPDialog allowed or not 
    * @param aConnection is the connection to be attached to HTTP session
    * @param aSocketServer is the socket server to be attached to HTTP session
    * Leaves with system-wide error code if preferences cannot be set.
    */
    TInt SetIapPrefsL( TUint32 aIapId, TBool aDialogPref, RConnection& aConnection, RSocketServ& aSocketServer );
    TInt SetSnapPrefsL( TUint32 aSnapId, TBool aDialogPref, RConnection& aConnection, RSocketServ& aSocketServer );
    
    //void SetupConnectionWithIapPrefsL( TUint32 aIapId, RConnection& aConnection, RSocketServ& aSocketServer );
    //void AttachSocketServerAndConnectionWithHttpSession( TUint32 aIapId, RConnection& aConnection, RSocketServ& aSocketServer );

    void ConstructL();
    void ConstructL( TUint32 aIapId );


private:
    
    // New functions

	TInt SetID(TUint32 aIapId, TBool aDialogPref, RConnection& aConnection, RSocketServ& aSocketServer, TBool aSNAP = EFalse);
	TInt GetS60PlatformVersion(TUint& aMajor, TUint& aMinor);
	TBool IsOCCImplementedSDK();	
    TPtrC8 SoapActionL(MSenElement& aSoapEnvelope);
    TInt SubmitRequestL(MSenResponseObserver& aObserver,
                        const TDesC8& aUri,
                        const TDesC8* aContentType = NULL,
                        const TDesC8* aContent = NULL);

    TInt SubmitRequestL(MSenResponseObserver& aObserver,
                        const TDesC8& aUri,
                        MSenElement& aMessage,
                        const TDesC8* aContentType = NULL);

    TInt InvokeHttpMethodL(CSenTxnState* aTxnState,
                           CSenLayeredHttpTransportProperties& aProps);
	
    void ListFilters();
    void DumpRespHeadersL(RHTTPTransaction& aTrans);
    void DumpBodyData(const TDesC8& aData);
    TInt ContentTypeParamValueL(const RHTTPHeaders& aResponseHeaders,
                                const RStringF& aContent, 
                                const TPtrC8& aParamName,
                                SenMultiPartUtils::TMultiPartContentType& aMultiPartContentType);

protected:
    RSocketServ iSockServ;
    RConnection iConnection;
    TUint32 iIapId;
    TUint32 iUsedIapId;
    TUint32 iSnapId;
    RStringPool iStringPool;
    RHTTPSession iSess;
    CSenHttpEventHandler* iTransObs;
    //RFileLogger iLog;
    
private:

    CArrayPtrFlat<CSenTxnState>* iTxnStates;
    MSenIdentityManager& iManager;
    TInt iBasicConnectionTries;
    CSenIdentityProvider*   iSessionAuthentication; // NOT owned
    TBool   iPasswordFromUser;  // Is ETrue if data from identitymanager is
    TPtrC8 iContentType; // content type of usual response
    TBool iExplicitIapDefined;
    HBufC8* iProxyHostPort;
    TBool iXopResponse;
    TBool iDeflateFilter;//Is true if deflate filter loaded for a session
    TBool iHasHttpContentType;
    SenMultiPartUtils::TMultiPartContentType iMultiPartContentType; // content type of MultiPart response
    RHTTPTransaction iHttpTransaction ;
#ifndef __ENABLE_ALR__    
    TBool iOCCenabled ;
#endif    
    };

#endif //SEN_HTTP_CHANNEL_IMPL_H

// End of file
