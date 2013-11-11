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
* Description: Header declaration
*
*/








#ifndef WSSTAR_RST_RESPONSE_MESSAGE_H
#define WSSTAR_RST_RESPONSE_MESSAGE_H

// INCLUDES
#include "SenBaseFragment.h"
#include "wsstarplugin.h"
#include "wsstarvalidatehandler.h"
#include "SenCredential.h"
#include "senwsdescription.h" //credential const


// FORWARD DECLARATIONS

    
// CLASS DECLARATION
/**
 * Class: CWSStarRSTResponse
 *
 * Description: Structure which is self parsing. Holds information about
 *              RST response. See WS-Trust specification
 */
class CWSStarRSTResponse :  public CSenBaseFragment
    {
public: // Constructors and destructor
    
    /**
    * Standard 2 phase constructor
    * @since Series60 4.0
    * @param aParseIssuedTokens determine if token will searched only in header under issuedToken tag
    * @param aAppliedTo precise which token in response collection is used after sts negotation.
    *               If null passed 1st token from collection is taken
    * @return new instance of rst response
    */
    static CWSStarRSTResponse* NewL(TBool aParseIssuedTokens, const TDesC8& aAppliedTo, const TDesC8* aVersion);
    
    /**
    * Standard 2 phase constructor
    * @since Series60 4.0
    * @param aParseIssuedTokens determine if token will searched only in header under issuedToken tag
    * @param aAppliedTo precise which token in response collection is used after sts negotation
    *               If null passed 1st token from collection is taken
    * @return new instance of rst response (also pushed on cleanupStack)
    */
    static CWSStarRSTResponse* NewLC(TBool aParseIssuedTokens, const TDesC8& aAppliedTo, const TDesC8* aVersion);
    
    /**
    * Normal destructor
    */
    virtual ~CWSStarRSTResponse();

    // New functions
    
    /**
    * Getter for credential
    * @return credential
    */
    CSenCredential* Credential();
    
    /**
    * Getter for POP encoded as Base64
    * @return credential
    */
    TPtrC8 POPBase64();
    
    /**
    * Getter for TokenType
    * @return credential
    */
    TPtrC8 TokenType();
    
    /**
    * Getter for type of binary
    * @return type of binary
    */
    TPtrC8 BinaryType();
    
    /**
    * Getter for entropy of STS
    * @return entropy of STS
    */
    TPtrC8 EntropySTS();
    
    /**
    * Getter for computed key
    * @return computed key
    */
    TPtrC8 ComputedKey();
    
    /**
    * Getter for time of credential expired time
    * @return expired time in XSD time-date format
    */
    TPtrC8 CredentialExpires();
    
    /**
    * Getter for time of credential creation time
    * @return creation time in XSD time-date format
    */
    TPtrC8 CredentialCreated();
    
    /**
    * Getter for security token referenve
    * @return security token reference
    */
    TPtrC8 STR();
    
    /**
    * Determine type of token (binary token or SCT)
    * @return Etrue if token is SCT (security Context Token).
    *        See WS-SecureConversation specification.
    */
    TBool IsContextToken();
    
    /**
    * Deteremine if this really is a response (RST)
    * @return ETrue if reponse is RST
    */
    TBool IsResponse();
    
    /**
    * Getter for KeySize (used during calculating computed Key in PSHA1 algorithm).
    * @return Size in bites.
    */
    TInt KeySize();
    
    /**
    * Set addressing holders for endpoint.In message phase 
    *  addressing handler shuld set values for addressing holders.
    */
    void UpdateAddressingHoldersL(MSenSessionContext& aCtx);
    void SetPolicyVersion(const TDesC8* aVersion);


private:

    /**
    * C++ default constructor.
    */
    CWSStarRSTResponse(TBool aParseIssuedTokens, const TDesC8& aAppliedTo);
    
    /**
    * By default Symbian 2nd phase constructors is private.
    */
    void ConstructL(const TDesC8* aVersion);
    
    // From CBaseFragment
    virtual void StartElementL(const TDesC8& aNsUri,
                               const TDesC8& aLocalName,
                               const TDesC8& aQName,
                               const RAttributeArray& aAttributes);
    
    virtual void EndElementL(const TDesC8& aNsUri,
                             const TDesC8& aLocalName,
                             const TDesC8& aQName);
    
    //reset state machine
    void Reset();
    

private: // Data
    
    TBool iIsResponse;
    /**
     * Should be set to ETrue when parsing header for issued tokens
     */
    TBool iParseIssuedTokens;
    
    //now this flag is not used anywhere. How to use it, any idea?
    TBool iCollection;
    TBool iContextToken;
    
    //This bool determine if token appliedTo has been found.
    //Next RSTRs are not processing if iTokenEstablished == TRUE
    //Set to true only in positive case (when NO reset of machine state)
    TBool iTokenEstablished;
    TBool iParsingApliedTo;
    TBool iDefTokenEstablished;
    
    CSenCredential* iCredential;
    CSenCredential* iCredentialDef;
    CSenBaseFragment* iSTRFr;
    HBufC8* iSTR;
    HBufC8* iSTRDef;                   
    HBufC8* iPOPencodedAsBase64;
    HBufC8* iPOPencodedAsBase64Def;
    HBufC8* iTokenType;
    HBufC8* iTokenTypeDef;
    HBufC8* iTypeOfBinary;
    HBufC8* iTypeOfBinaryDef;
    HBufC8* iEntropySTS;
    HBufC8* iEntropySTSDef;
    const TDesC8* iActiveXmlns;
    TTime iNotOnOrAfter;
    HBufC8* iComputedKey;
    TInt* iKeySize;
    TInt* iKeySizeDef;
    HBufC8* iComputedKeyDef;
    HBufC8* iCredentialExpires;
    HBufC8* iCredentialExpiresDef;
    HBufC8* iCredentialCreated;
    HBufC8* iCredentialCreatedDef;
    TPtrC8 iAppliedTo;
    const TDesC8* iAddressingNs;            //not owned    
    const TDesC8* iAddressingEndpointTag;  //not owned    
    const TDesC8* iPolicyVersion;//not owned
};

#endif // WSSTAR_RST_RESPONSE_MESSAGE_H

// End of File

