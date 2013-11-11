/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/








#ifndef WSOVI_SERVICE_SESSION_H
#define WSOVI_SERVICE_SESSION_H

// INCLUDES
#include "msenremoteserviceconsumer.h"
#include "msenserviceinvocationframework.h"

#include "wsoviplugin.h"
#include "senwebservicesession.h"
#include "wsovisessionconsumer.h"
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengelement.h>

class CWSOviCredentialObserver;
class CSenHttpTransportProperties;

// CONSTANTS
namespace WSOviSession
    {
    _LIT8(KProviderIDTag,                  "<ProviderID>");
    _LIT8(KProviderIDEndTag,               "</ProviderID>");
    _LIT8(KTrustAnchorTag,                 "<TrustAnchor>");
    _LIT8(KTrustAnchorEndTag,              "</TrustAnchor>");
    _LIT8(KTrustAnchorElementLocalName,  "TrustAnchor");
    _LIT8(KTokenCreationTimeTag,         "<TokenCreationTime>");
    _LIT8(KTokenCreationTimeEndTag,      "</TokenCreationTime>");
    _LIT8(KTokenCreationTimeLocalName,   "TokenCreationTime");
    _LIT8(KTokenValidUntilTimeTag,         "<ValidUntil>");
    _LIT8(KTokenValidUntilTimeEndTag,      "</ValidUntil>");
    _LIT8(KTokenValidUntilTimeLocalName,   "ValidUntil");
    _LIT8(KTTLTag,                 		 "<TTL>");
    _LIT8(KTTLEndTag,                    "</TTL>");
    _LIT8(KTTLLocalName,                 "TTL");
    _LIT8(KTokenSecretTag,               "<TokenSecret>");
    _LIT8(KTokenSecretEndTag,            "</TokenSecret>");
    _LIT8(KTokenSecretLocalName,         "TokenSecret");
    
    }



// CLASS DECLARATION
/**
 * WSOvi Service Session.
 * Represent session between client and service in ws* framework.
 */
class CWSOviServiceSession : public CSenWebServiceSession
    {

friend class CWSOviPlugin;

public: // Constructors and destructor

    static CWSOviServiceSession* NewL(MSIF& aFramework);
    static CWSOviServiceSession* NewLC(MSIF& aFramework);

    virtual ~CWSOviServiceSession();

    // From SenServiceSession
    virtual MSenRemoteServiceConsumer* RemoteConsumerL(
                                                const TDesC8& aSenderID);
    
    virtual TBool Matches(MSenServiceDescription& aPattern);
    virtual TInt ScoreMatchL(MSenServiceDescription& aPattern);
    
    virtual void WriteExtensionsAsXMLToL(RWriteStream& aWriteStream);


    //From SenWebServiceSession
    virtual TInt SendL( const TDesC8& aMessage, 
                        const TDesC8& aTransportProperties,
                        MSenRemoteServiceConsumer& aConsumer,
                        TInt& aTxnId,
                        HBufC8*& aRevalidationError );
                        
    virtual TInt SubmitL(const TDesC8& aMessage, 
                         const TDesC8& aTransportProperties,
                         MSenRemoteServiceConsumer& aConsumer,
                         HBufC8*& aResponse);

    virtual TInt SubmitSoapL(const TDesC8& aSoapMessage, 
                             const TDesC8& aTransportProperties,
                             MSenRemoteServiceConsumer& aConsumer,
                             HBufC8*& aResponse);

    virtual TInt SendSoapL( const TDesC8& aSoapMessage, 
                            const TDesC8& aTransportProperties,
                            MSenRemoteServiceConsumer& aConsumer,
                            TInt& aTxnId,
                            HBufC8*& aRevalidationError );
    
    
    virtual TInt SendErrorToConsumerL( const TInt aErrorCode,
                                       HBufC8* apError,
                                       const TInt aTxnId,
                                       MSenRemoteServiceConsumer& aConsumer,
                                       MSenProperties* aResponseTransportProperties );
    virtual TInt SendToConsumerL(HBufC8* aMessage,
                              const TInt aTxnId,
                              MSenRemoteServiceConsumer& aConsumer,
                              MSenProperties* aResponseTransportProperties = NULL);
                              
    virtual TInt InitializeFromL(MSenServiceDescription& aDescription);


    
    /**
    add consumer. Eeach session has multiple consumers.From 3rd parties point of view
    its mean 2 connection per the same service description.
    */
    virtual TInt AddConsumerL(MSenRemoteServiceConsumer& aConsumer);
    
    /**
    remove consumer
    */
    virtual TInt RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer);
    
    /**
    Framework specific method. Process message before sending to network
            In other words add specific headers, and store correlation info in proper consumer
    */
    virtual TInt MessageForSendingL( CSenHttpTransportProperties*& aTP,
                                     const TDesC8& aSenderID);

    virtual TInt SetTransportPropertiesL( const TDesC8& aProperties,
                                          MSenRemoteServiceConsumer& aConsumer );
    virtual TBool IsExpiredL();
    /**
     * Clear credential for current session , make it invalid.
     * @since S60 5.0
     */
    void ClearCredentialL();
    
    //GData token has undefined life time, threfore default FOREVER life time is set
    void SetMaxValidUntil();
    
    
    
    
    /**
    * Overload addcredential from WebServiceSession. Calucate also state, 
    * @since S60 4.0
    */
    void AddCredentialL(const TDesC8& aSecurity, TTime aValidUntil);
    void AddCredentialL(RSenCredentialPtr aCredentialPtr, TTime aValidUntil);
    void SetProviderIDL(const TDesC8& aProviderId);
    TPtrC8 ProviderID();
    void SetTrustAnchorL(const TDesC8& aURI);
    TPtrC8 TrustAnchor();
    TInt ComputeStatusL();
    void SetSessionContext(CWSOviSessionContext* aContext);
    CWSOviSessionContext* SessionContext();
    
    void AddSecurityTokenToContextL();

    void AddPropertiesFromSessionContextToCredentialL();
    
    virtual void CredentialChanged(TSenCredentialChange aChange, TAny* aPointer);
    TInt RevalidateMobileTicketIfExpiredL(HBufC8*& aErrorMessage);
    TBool ExistConsumerL();

    // Add ProviderID to primary keys (in addition to Endpoint, Contract and FrameworkID):
    TBool HasEqualPrimaryKeysL( MSenServiceDescription& aCandidate ); 
    
    void ActiveTicketObserverL();
    TInt ShareTokenWithL( CWSOviServiceSession* aWSStarSessionDst);

private: 
       
    /**
    * C++ default constructor.
    */
    CWSOviServiceSession(TDescriptionClassType aType, MSIF& aFramework);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    // New functions
    
    
    /**
    * To obtain consumer during processing INBOUND. We have to dispatch using wsa:RelatesTo
    */
    TInt SessionConsumerL(CWSOviMessageContext*& aMessage,
                          CWSOviSessionConsumer*& aSessionConsumer);
    
    /**
    * To obtain consumer during processing OUTBOUND.
    */
    TInt SessionConsumerL( const TDesC8& aSenderID, CWSOviSessionConsumer*& aSessionConsumer);
    void CreateAndParseSoapMessageL(const TDesC8& aSoapMessage, CSenSoapMessage*& aMessage, HBufC8*& aBody);
    CSenHttpTransportProperties* ApplyTransportPropertiesLC(const TDesC8& aTransportProperties);
    void PrepareOutCtxL(const TDesC8& aMessageBody);
    void DestroyOutCtx();
    TBool HasSuperClass( TDescriptionClassType aType );
    TBool AmIHostletSession();
    virtual void FillCredentialIdentifierL(CSenCredentialIdentifier& aIdentifier);
    
    CSenIdentityProvider* LookupIdentityProviderL();
    void VerifyPermissionL();
    void ClearCredentialPropertiesFromContext();
    TInt CanHandleErrorL();
    void AdaptEndpointL(TBool aToOrginal);
    CSenIdentityProvider* IdentityProviderFromCoreL(const TDesC8& aProviderID);
    
//Data
    
    CWSOviMessageContext* iOutContext;//owned, live only within one sending
    CWSOviMessageContext* iInContext;//owned, live only within one sending
    CWSOviSessionContext* iSessionContext;//owned

    CWSOviCredentialObserver* iTokenObs;
    
    //trust anchor is a pointer to providerID.
    HBufC8* iTrustAnchor;
    TInt iSubmitState;
    TInt iRenewCounter;
    TInt iRetryCounter;
    HBufC8* iProviderID;
    HBufC8* iTransProp;
    TTime iClientTime; 
    TTime iServerTime;
    HBufC8* iOrginalEndpoint;
    };
    
#endif // WSOvi_SERVICE_SESSION_H

// End of File

