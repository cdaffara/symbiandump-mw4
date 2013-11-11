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










#ifndef TRUST_CLIENT_H
#define TRUST_CLIENT_H

// INCLUDES
#include <e32base.h>
#include <flogger.h>
#include <e32math.h>

#include "wsstarsessionvalidator.h"
#include "seninternalserviceconsumer.h"
#include "SenXmlElement.h"
#include "sentransportbase.h"
#include "wsstarhandlercontext.h"

// FORWARD DECLARATIONS
class CWSStarPolicyHandler;
namespace WSPolicy
    {
   _LIT8( KPolicyNamespace200409, "http://schemas.xmlsoap.org/ws/2004/09/policy");
   _LIT8( KPolicyNamespace200212, "http://schemas.xmlsoap.org/ws/2002/12/policy");
   
   _LIT8( KPolicyPrefix, "wsp");
   _LIT8( KAppliesToTag, "AppliesTo");
   _LIT8( KAppliesToQTag, "wsp:AppliesTo"); 
   _LIT8( KPolicyReference, "PolicyReference"); 
    }

namespace WSTrust
    {    
    //---------namespaces
    _LIT8( KTrustNamespace200502, "http://schemas.xmlsoap.org/ws/2005/02/trust");
    _LIT8( KTrustNamespace200404, "http://schemas.xmlsoap.org/ws/2004/04/trust");
    
    _LIT8( KTrustPrefix,    "wst");
    _LIT8( KWsuNamespace, "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd");
    
    
    //=================================Request specific
    //-------tags
    _LIT8( KRSTTag, "RequestSecurityToken");
    _LIT8( KRSTQTag, "wst:RequestSecurityToken");
    _LIT8( KRequestTypeTag, "RequestType");
    _LIT8( KRenewingTag, "Renewing");
    _LIT8( KRenewTargetTag, "RenewTarget");
    _LIT8( KEntropyTag, "Entropy");
    _LIT8( KTokenTypeTag, "TokenType");
    
    _LIT8( KFSTTag, "FetchSCTRequest");
    //-------value
    //---addressing layer - actions
    _LIT8( KActionIssueRST200502, "http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue");
    _LIT8( KActionRenewRST200502, "http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Renew");    
    _LIT8( KActionIssueRST200404, "http://schemas.xmlsoap.org/ws/2004/04/trust/RST/Issue");
    _LIT8( KActionRenewRST200404, "http://schemas.xmlsoap.org/ws/2004/04/trust/RST/Renew");
    
    _LIT8( KActionIssueRSTR200502, "http://schemas.xmlsoap.org/ws/2005/02/trust/RSTR/Issue");
    _LIT8( KActionRenewRSTR200502, "http://schemas.xmlsoap.org/ws/2005/02/trust/RSTR/Renew");    
    _LIT8( KActionIssueRSTR200404, "http://schemas.xmlsoap.org/ws/2004/04/trust/RSTR/Issue");
    _LIT8( KActionRenewRSTR200404, "http://schemas.xmlsoap.org/ws/2004/04/trust/RSTR/Renew");
    
    _LIT8( KActionIssueFST200610, "http://schemas.live.com/mws/2006/10/proxy/FetchSCT");
    //---trust layer - requestType
    _LIT8( KRequestTypeIssueRST200502, "http://schemas.xmlsoap.org/ws/2005/02/trust/Issue");
    _LIT8( KRequestTypeRenewRST200502, "http://schemas.xmlsoap.org/ws/2005/02/trust/Renew");
    _LIT8( KRequestTypeIssueRST200404, "http://schemas.xmlsoap.org/ws/2004/04/security/trust/Issue");
    _LIT8( KRequestTypeRenewRST200404, "http://schemas.xmlsoap.org/ws/2004/04/security/trust/Renew");
    

    //-------computed Keys values
    _LIT8( KComputedKeyPSHA1Value,  "http://schemas.xmlsoap.org/ws/2005/02/trust/CK/PSHA1");
    const TInt KSizeOfComputedKey        = 256;
    
    //=================================Response specific
    _LIT8( KRSTResponseTag,             "RequestSecurityTokenResponse");
    _LIT8( KRSTResponseCollectionTag,   "RequestSecurityTokenResponseCollection");
    _LIT8( KRequestedSecurityTokenTag,  "RequestedSecurityToken");
    _LIT8( KLifetimeTag,                "Lifetime");
    _LIT8( KLifeTimeTag,                "LifeTime");//Wrong one, passport use this
    _LIT8( KTokenType,                  "TokenType");
    _LIT8( KKeySize,                    "KeySize");
    _LIT8( KBinarySecretTag,            "BinarySecret");
    _LIT8( KProofTokenTag,              "RequestedProofToken");
    _LIT8( KRequestedAttachedReferenceTag, "RequestedAttachedReference");
    _LIT8( KRequestedTokenReferenceTag, "RequestedTokenReference");
    _LIT8( KBinaryTypeAttr,             "Type");
    _LIT8( KComputedKeyTag,                "ComputedKey");
    //--lifetime
    _LIT8( KCreatedTag,             "Created");
    _LIT8( KExpiresTag,             "Expires");
    //--other
    _LIT8( KIssuedTokens, "IssuedTokens");
    
    
    _LIT8( KBinaryTypeSimmetricValue,   "http://schemas.xmlsoap.org/ws/2005/02/trust/SymmetricKey");
    _LIT8( KBinaryTypeAsimmetricValue,  "http://schemas.xmlsoap.org/ws/2005/02/trust/AsymmetricKey");
    _LIT8( KBinaryTypeNonceValue,       "http://schemas.xmlsoap.org/ws/2005/02/trust/Nonce");
    //---------Faults data
    
    _LIT8( KInvalidRequest,"InvalidRequest");
    _LIT8( KFailedAuthentication,"FailedAuthentication");
    _LIT8( KRequestFailed,"RequestFailed");
    _LIT8( KInvalidSecurityToken,"InvalidSecurityToken");
    _LIT8( KAuthenticationBadElements,"AuthenticationBadElements");
    
    _LIT8( KBadRequest,"BadRequest");
    _LIT8( KExpiredData,"ExpiredData");
    _LIT8( KInvalidTimeRange,"InvalidTimeRange");
    _LIT8( KInvalidScope,"InvalidScope");
    _LIT8( KRenewNeeded, "RenewNeeded");
    
    _LIT8( KUnableToRenew,"UnableToRenew");
    }
namespace WSStarTrustFault
    {
    _LIT8(KBadSCT, "Fault.WS-Trust : Bad STR in RSTR");
    _LIT8(KBadKeySize, "Fault.WS-Security : KeySize not specified during calculating entropy in PSHA1");
    }
namespace WSSecureConversation
    {
    _LIT8(KSecureConversationNamespaceMS,
            "http://schemas.xmlsoap.org/ws/2005/02/sc/ws-secureconversation.xsd");
    _LIT8(KSecureConversationNamespace,
            "http://schemas.xmlsoap.org/ws/2005/02/sc");
    _LIT8(KSecurityContextToken, "SecurityContextToken");
    }
namespace WSPassport32
    {
    _LIT8(KPassportAppliesTo,    "http://Passport.NET/tb");
    //_LIT8(KDefaultTokenType,     "urn:oasis:names:tc:SAML:1.0");
    _LIT8(KDefaultTokenType,     "urn:passport:compact");
    }


/**
 * Class: CWSStarTrustClient
 *
 * Description: Consumer which communicate with real STS.
 * Client will try obtain credentail based on WS-Trust specification.
 */
class CWSStarTrustClient : public CSenInternalServiceConsumer,
                           public MWSStarSessionValidator
    {
public: // Constructors and destructor

    /**
    * Standard 2 phase constructor
    * @param aSIF SIF implementation used in contructor of internal consumer
    * @param aLogger logger used for logs in debug mode
    * @return new instance of trust client
    */
    static CWSStarTrustClient* NewL(    CSIF& aSIF,
                                                    RFileLogger& aLogger);
    /**
    * Standard 2 phase constructor
    * @since Series60 4.0
    * @param aSIF SIF implementation used in contructor of internal consumer
    * @param aLogger logger used for logs in debug mode
    * @return new instance of trust client which is pushed into CleanupStack
    */
    static CWSStarTrustClient* NewLC(CSIF& aSIF,
                                                    RFileLogger& aLogger);

    /**
    * Normal destructor. Destroy STS session.
    */
    ~CWSStarTrustClient();
    
    
    /**
    * The most siginificant method of this class. Make validation of session, 
    *   credentails obtained if needed.
    * @param aSession session which is validated
    * @param aRemoteConsumer consumer of validated session, holds transport entryPoint
    * @param aRenewing define mode of requesting for credential. See into WS-Trust specification,
    *       renewal and issuance binding
    */
    TInt ValidateL(CWSStarServiceSession& aSession, 
                   TBool aRenewing,
                   HBufC8*& aErrorMessage,
                   TBool aOnlySharing);
    

    void SetStatus(TInt aStatus);
    /**
    * Sets identyProvider for trust client
    * @param aIdentityProvider identity provider representation of STS
    */
    void SetAccount(CSenIdentityProvider* aIdentityProvicer);
    
//-----derived from CSenWSDescription, 
    //in fact we expose info from stssession which is holded by that client.
    // Method have to overwrited becouse of core, which use those methods during registering
    TPtrC8 Endpoint();
    TPtrC8 Contract();
    TPtrC8 ProviderID();
    TPtrC8 FrameworkId();
    MSenServicePolicy* ServicePolicy();
    TInt IapId(TUint32& aCurrentIapId);
    TInt UpdatePolicyL(CWSStarPolicyHandler* aPolicyHandler, MSenServiceDescription* aSD);
    void WriteAsXMLToL(RWriteStream& aWriteStream);
    
    /**
    * Getter for validate session (not STS session)
    */
    virtual CSenServiceSession* ServiceSession();
    
    /**
    * Getter for transport (used coreManger as Factory for transport)
    */
    virtual MSenTransport& TransportL();
    
    /**
    *   Sets STS session, used during registering STS which is not initiated. 
    *   Clear STSsession will be created and assign during registering.
    * @param aSession - clear sts session assign to this trust clint
    */
    void SetSTSSessionL(CWSStarServiceSession* aSession);                    
    
    CWSStarServiceSession* GetSTSSessionL();                    
    /**
    *   Sets version (mode) for trust client. Old or new trust
    * @param aVersion - old or new trust version 
    *   _LIT8( KTrustNamespace200502, "http://schemas.xmlsoap.org/ws/2005/02/trust");
    *   _LIT8( KTrustNamespace200404, "http://schemas.xmlsoap.org/ws/2004/04/trust");
    *
    */
    void SetVersion(const TDesC8* aVersion);
    void SetHandlerCtx(MSenHandlerContext* aHndCtx);
    
    TBool HasSuperClass( TDescriptionClassType aType );
        
protected://methods
    
    /**
    * C++ default constructor.
    */
    CWSStarTrustClient(    CSIF& aSIF,
                                    TDescriptionClassType aType,
                                    RFileLogger& aLogger);
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void BaseConstructL();
    virtual TBool Matches(MSenServiceDescription& aPattern);
    
	virtual void SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) ; 			    

private: //methods
    void RandomEntropyClientL();
    
    /**
    *   Create RST body skeleton.
    */
    CSenElement* CreateRSTL(TBool aRenewing, TPtrC8 aAppliesTo);
    CSenElement* CreateBodyL();
    
    const TDesC8& GetAction();
    const TDesC8& GetActionResponse();
    const TDesC8& GetRequestType();
    const TDesC8& GetPolicyNs();
    
    // Add ProviderID to primary keys (in addition to Endpoint, Contract and FrameworkID):
    TBool HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate); 
    void AdaptStsEndpointL(TBool aToOrginal = EFalse);
private: // Data
    CWSStarServiceSession* iWSStarServiceSession;   // Not owned, session which we validate
    CWSStarServiceSession* iSTSSession;             //STS session, owned
    HBufC8* iSTSOrginalEndpoint;             //STS session, owned
    
    CSenIdentityProvider* iIdentityProvider; // Not owned, comes from validation phase
    RFileLogger& iLog;                      //not owned
    
    HBufC8* iEntropyClient;
    TBool iPassportMode;
    TBool iRenewingMode;
    const TDesC8* iVersion;
    HBufC8* iAppliesTo;//owned
    CSenTransportBase* ipTransport;//owned
    MSenHandlerContext* iHndCtx; //not owned
    HBufC8* iTokenType;
    TInt iTpCounter;
	TSenDataTrafficDetails iTrustDetails;
};

#endif // TRUST_CLIENT_H

// End of File
