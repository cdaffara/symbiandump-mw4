/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/











#ifndef WSSTAR_SERVICE_SESSION_H
#define WSSTAR_SERVICE_SESSION_H

// INCLUDES
#include "msenremoteserviceconsumer.h"
#include "msenserviceinvocationframework.h"

#include "wsstarplugin.h"
#include "senwebservicesession.h"
#include "wsstarsessionconsumer.h"
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengelement.h>
#include "sensoapmessagedom2.h"

class CWSStarCredentialObserver;
// CONSTANTS
namespace WSStarSession
    {
    _LIT8(KProviderIDTag,                  "<ProviderID>");
    _LIT8(KProviderIDEndTag,               "</ProviderID>");
    _LIT8(KTrustAnchorTag,                 "<TrustAnchor>");
    _LIT8(KTrustAnchorEndTag,              "</TrustAnchor>");
    _LIT8(KPOPBase64Tag,                   "<POP>");
    _LIT8(KPOPBase64EndTag,                "</POP>");
    _LIT8(KTokenTypeTag,                   "<TokenType>");
    _LIT8(KTokenTypeEndTag,                "</TokenType>");
    _LIT8(KSTRTag,                          "<STR>");
    _LIT8(KSTREndTag,                       "</STR>");
    _LIT8(KBinaryTypeTag,                   "<BinaryType>");
    _LIT8(KBinaryTypeEndTag,                "</BinaryType>");
    
    _LIT8(KClusterTypeTag,                   "<Cluster>");
    _LIT8(KClusterEndTypeTag,                "</Cluster>");
    
    _LIT8(KServiceInterval,             "<ServiceInterval>");
    _LIT8(KServiceIntervalEnd,          "</ServiceInterval>");

    
    //related to ticket
    _LIT8(KPhoneTimeWhenMTResolvedTag,        "<PhoneTimeWhenMTResolved>");
    _LIT8(KPhoneTimeWhenMTResolvedEndTag,        "</PhoneTimeWhenMTResolved>");
    _LIT8(KCreatedTag,        "<Created>");
    _LIT8(KCreatedEndTag,        "</Created>");  
    //_LIT8(KProviderIdElementLocalName,  "ProviderID");
    _LIT8(KPOPBase64LocalName,  "POP");
    _LIT8(KSTRLocalName,  "STR");
    _LIT8(KBinaryTypeLocalName,  "BinaryType");
    _LIT8(KTrustAnchorElementLocalName,  "TrustAnchor");
    _LIT8(KClusterLocalName,        "Cluster");
    _LIT8(KPhoneTimeWhenMTResolvedLocalName,        "PhoneTimeWhenMTResolved");
    _LIT8(KCreatedLocalName,        "Created");
    _LIT8(KTokenType,        "TokenType");
    }



// CLASS DECLARATION
/**
 * WSStar Service Session.
 * Represent session between client and service in ws* framework.
 */
class CWSStarServiceSession : public CSenWebServiceSession
    {

friend class CWSStarPlugin;

public: // Constructors and destructor

    static CWSStarServiceSession* NewL(MSIF& aFramework);
    static CWSStarServiceSession* NewLC(MSIF& aFramework);

    virtual ~CWSStarServiceSession();

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
    
    virtual void StartTransaction();                             
    virtual void TransactionCompleted();
    virtual TInt InitializeFromL(MSenServiceDescription& aDescription, CWSStarPolicyHandler* aPolicyHandler = NULL);


    
    /**
    add consumer. Eeach session has multiple consumers.From 3rd parties point of view
    its mean 2 connection per the same service description.Consumers are distinguished by addressing id (MessagesID and RelatesTo)
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
    virtual TInt MessageForSendingL( const TDesC8& aBody,
                                     const TDesC8& aSenderID,
                                     CSenSoapMessage*& aMessage ); //out param

    /**
    During parsing Inbound new SoapMessage is created, based on active soap version.
    Method is called by framework
    */
    virtual TInt NewMessageL( CSenSoapMessage*& aMessage );
    
    
    //virtual TInt ParseResponseL(const TDesC8& aResponse,CSenSoapMessage*& aMessage);
    
    virtual TInt ParseMessageL( CSenSoapMessage& aSOAPMessage );


    //aConsumer is not used, becouse session holds many consumers, and first has to establish correct one
    virtual TInt SendSoapMessageToConsumerL( CSenSoapMessage* apMessage, 
                                             const TInt aTxnId,
                                             MSenRemoteServiceConsumer& aConsumer,
                                             MSenProperties* aResponseTransportProperties );
    

    virtual TInt SetTransportPropertiesL( const TDesC8& aProperties,
                                          MSenRemoteServiceConsumer& aConsumer );

    //in WS* and SCHarF methiod disabled.
    //virtual void SetFrameworkHeadersL(CSenSoapMessage& aMsg);
    
    
    virtual TInt HandleSoapFaultL( CSenSoapMessage* apSOAPMessage,
                                   HBufC8*& aResponse);

    virtual TInt HandleSoapFaultL( CSenSoapMessage* apSOAPMessage,
                                   const TInt aErrorCode,
                                   const TInt aTxnId,
                                   MSenRemoteServiceConsumer& aConsumer,
                                   MSenProperties* aTransportProperties );

    virtual TBool IsExpiredL();
    /**
     * Clear credential for current session , make it invalid.
     * @since S60 5.0
     */
    void ClearCredentialL();
    
    
    /**
     * Share token. Copy SCT and MT into new star session
     * @since S60 5.0
     */
    TInt ShareTokenWithL(CWSStarServiceSession* aWSStarSessionDst, TBool& aMTwasReplaceBySCT, TBool aSeekSCT);
    
    
    /**
     * Getter of Provider ID (STS which represents this session)
     *
     * @return TDesC8 id of provider
     * @since S60 5.0
     */
    
    /**
    * Overload addcredential from WebServiceSession. Calucate also state, 
    * @since S60 5.0
    */
    void AddCredentialL(const TDesC8& aSecurity, TTime aValidUntil);
    void AddCredentialL(RSenCredentialPtr aCredentialPtr, TTime aValidUntil);
    TPtrC8 ProviderID();
    void SetTrustAnchorL(const TDesC8& aURI);
    TPtrC8 TrustAnchor();
    TInt ComputeStatusL();
    void SetSessionContext(CWSStarSessionContext* aContext);
    CWSStarSessionContext* SessionContext();
    TInt UpdatePolicyL(CWSStarPolicyHandler* aPolicyHandler, MSenServiceDescription* aSD);
    
    void AddSecurityTokenToContextL();

    void AddPropertiesFromSessionContextToCredentialL();
    
    virtual void CredentialChanged(TSenCredentialChange aChange, TAny* aPointer);
    void ActiveTicketObserverL();
    //virtual TInt RefreshMTL(HBufC8*& aErrorMessage);
    TInt RevalidateMobileTicketIfExpiredL(HBufC8*& aErrorMessage);
    TBool ExistConsumerL();

    // Add ProviderID to primary keys (in addition to Endpoint, Contract and FrameworkID):
    TBool HasEqualPrimaryKeysL( MSenServiceDescription& aCandidate ); 

	/**
	 * DetachCredentialL
	 *
	 * This function  invalidates the session by deleting the credential
	 * This also call setStatusL to recompute the status
	 */
	void DetachCredentialsL() ;	
	TBool AmIHostletSession();

private: 
       
    /**
    * C++ default constructor.
    */
    CWSStarServiceSession(TDescriptionClassType aType, MSIF& aFramework);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    // New functions
    
    
    /**
    * To obtain consumer during processing INBOUND. We have to dispatch using wsa:RelatesTo
    */
    TInt SessionConsumerL(CWSStarMessageContext*& aMessage,
                          CWSStarSessionConsumer*& aSessionConsumer);
    
    /**
    * To obtain consumer during processing OUTBOUND.
    */
    TInt SessionConsumerL( const TDesC8& aSenderID, CWSStarSessionConsumer*& aSessionConsumer);
    TInt CanHandleErrorL();
    void CreateAndParseSoapMessageL(const TDesC8& aSoapMessage, CSenSoapMessage*& aMessage, HBufC8*& aBody);
    HBufC8* ApplyTransportPropertiesL(const TDesC8& aTransportProperties);
    void PrepareOutCtxL(const TDesC8& aTransportProperties);
    TBool HasSuperClass( TDescriptionClassType aType );
   // TBool AmIHostletSession();
    void FindAndShareSCTL();
    
    
    
    
    void ParseToSoapMessage2L( const TDesC8& aSoapMessage, MSenRemoteServiceConsumer& aConsumer, TInt& aTxnId );
    
    TInt ListBinaryElementsL( RArray<TXmlEngElement>& aElementArray, TXmlEngElement& aElement );
    
    virtual void FillCredentialIdentifierL(CSenCredentialIdentifier& aIdentifier);
    
    CSenIdentityProvider* LookupIdentityProviderL();
    void VerifyPermissionL();
    void ClearCredentialPropertiesFromContext();
    // Add ProviderID to primary keys (in addition to Endpoint, Contract and FrameworkID):
    //TBool HasEqualPrimaryKeysL( MSenServiceDescription& aCandidate ); 

		void ReattachCredentialsL() ;
    
private: // Data
    
    CWSStarMessageContext* iOutContext;//owned, live only within one sending
    CWSStarMessageContext* iInContext;//owned, live only within one sending
    
    CWSStarSessionContext* iSessionContext;//owned
    
    
    
    //trust anchor is a pointer to providerID.
    //ProviderId identify SD which represent STS, and in order to distinguis who validate whom;
    //  STS has providerID which is pointed by trustAnchor in validatet WebService
    //  Unfortuantely webService inherits same classes as STS (it is still XmlSD) so webservice has both TrustAnchor 
    //  and ProviderId which are equal
    HBufC8* iTrustAnchor;
    
    //to keep transaction chain
    HBufC8* ipReceivedMessageIdInTrans;//owned
    
    TBool  iMessageThread;
    TInt iSubmitState;
    TInt iRetryCounter;
    TInt iRenewCounter;
    TInt iRedirectCounter;
    HBufC8* iProviderID;
    HBufC8* iTransProp;
    TTime iClientTime; 
    TTime iServerTime;
    CWSStarCredentialObserver* iTicketObs;
    };
    
#endif // WSSTAR_SERVICE_SESSION_H

// End of File

