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







#ifndef Authentication_CLIENT_H
#define Authentication_CLIENT_H

// INCLUDES
#include <e32base.h>
#include <flogger.h>
#include <e32math.h>

#include "seninternalserviceconsumer.h"
#include "SenXmlElement.h"
#include "sentransportbase.h"
#include "wsovihandlercontext.h"

// FORWARD DECLARATIONS
class CWSOviServiceSession;

/**
 * Class: CWSOviOAuthClient
 *
 * Description: Consumer which communicate with real NAUTH.
 * Client will try obtain credentail based on WS-Authentication specification.
 */
class CWSOviOAuthClient : public CSenInternalServiceConsumer
    {
public: // Constructors and destructor

    /**
    * Standard 2 phase constructor
    * @param aSIF SIF implementation used in contructor of internal consumer
    * @param aLogger logger used for logs in debug mode
    * @return new instance of Authentication client
    */
    static CWSOviOAuthClient* NewL(    CSIF& aSIF);
    /**
    * Standard 2 phase constructor
    * @since Series60 4.0
    * @param aSIF SIF implementation used in contructor of internal consumer
    * @param aLogger logger used for logs in debug mode
    * @return new instance of Authentication client which is pushed into CleanupStack
    */
    static CWSOviOAuthClient* NewLC(CSIF& aSIF);

    /**
    * Normal destructor. Destroy NAUTH session.
    */
    ~CWSOviOAuthClient();
    
    
    /**
    * The most siginificant method of this class. Make validation of session, 
    *   credentails obtained if needed.
    * @param aSession session which is validated
    * @param aRemoteConsumer consumer of validated session, holds transport entryPoint
    */
    TInt ValidateL(CWSOviServiceSession& aSession, 
                   HBufC8*& aErrorMessage,
                   TBool aOnlySharing );
    

    void SetStatus(TInt aStatus);
    /**
    * Sets identyProvider for Authentication client
    * @param aIdentityProvider identity provider representation of NAUTH
    */
    void SetAccount(CSenIdentityProvider* aIdentityProvicer);
    
//-----derived from CSenWSDescription, 
    //in fact we expose info from NAUTHsession which is holded by that client.
    // Method have to overwrited becouse of core, which use those methods during registering
    TPtrC8 Endpoint();
    TPtrC8 Contract();
    TPtrC8 ProviderID();
    TPtrC8 FrameworkId();
    MSenServicePolicy* ServicePolicy();
    TInt IapId(TUint32& aCurrentIapId);
    void WriteAsXMLToL(RWriteStream& aWriteStream);
    
    /**
    * Getter for validate session (not NAUTH session)
    */
    virtual CSenServiceSession* ServiceSession();
    
    /**
    * Getter for transport (used coreManger as Factory for transport)
    */
    virtual MSenTransport& TransportL();
    
    /**
    *   Sets NAUTH session, used during registering NAUTH which is not initiated. 
    *   Clear NAUTHsession will be created and assign during registering.
    * @param aSession - clear NAUTH session assign to this Authentication clint
    */
    void SetAuthSessionL(CWSOviServiceSession* aSession);                    
    
    void SetHandlerCtx(MSenHandlerContext* aHndCtx);
    
    TBool HasSuperClass( TDescriptionClassType aType );
    void CreateBodyL(const TDesC8& aXmlNs, const TDesC8& aUsername, const TDesC8& aPassword, const TDesC8& aCreated);
        
protected://methods
    
    /**
    * C++ default constructor.
    */
    CWSOviOAuthClient(    CSIF& aSIF,
                                    TDescriptionClassType aType);
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void BaseConstructL();
    // from MSenServiceDescription
    virtual TBool Matches(MSenServiceDescription& aPattern);

    
    

private: //methods
    
    // Add ProviderID to primary keys (in addition to Endpoint, Contract and FrameworkID):
    TBool HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate); 
    
private: // Data
    CWSOviServiceSession* iWSOviServiceSession;   // Not owned, session which we validate
    CWSOviServiceSession* iAuthSession;             //auth session, owned
    
    CSenIdentityProvider* iIdentityProvider; // Not owned, comes from validation phase
    
    CSenTransportBase* ipTransport;//now owned, ownership moved to validated session
    MSenHandlerContext* iHndCtx; //not owned
    HBufC8* iBody;
    TTime iDiff;

};

#endif // Authentication_CLIENT_H

// End of File

