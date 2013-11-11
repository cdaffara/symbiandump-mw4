/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    
*
*/




#ifndef CO_BRANDING_CLIENT_H
#define CO_BRANDING_CLIENT_H

// INCLUDES
#include <e32base.h>
#include <flogger.h>
#include <e32math.h>

#include "wsstarsessionvalidator.h"
#include "seninternalserviceconsumer.h"
#include "SenXmlElement.h"
#include "sentransportbase.h"
#include "wsstarhandlercontext.h"



/**
 * Class: CWSStarCoBrandingClient
 *
 * Description: Consumer which communicate with real STS.
 * Client will try obtain credentail based on WS-Trust specification.
 */
class CWSStarCoBrandingClient : public CSenInternalServiceConsumer
    {
public: // Constructors and destructor

    /**
    * Standard 2 phase constructor
    * @param aSIF SIF implementation used in contructor of internal consumer
    * @param aLogger logger used for logs in debug mode
    * @return new instance of trust client
    */
    static CWSStarCoBrandingClient* NewL(    CSIF& aSIF,
                                                    RFileLogger& aLogger);
    /**
    * Standard 2 phase constructor
    * @since Series60 4.0
    * @param aSIF SIF implementation used in contructor of internal consumer
    * @param aLogger logger used for logs in debug mode
    * @return new instance of trust client which is pushed into CleanupStack
    */
    static CWSStarCoBrandingClient* NewLC(CSIF& aSIF,
                                                    RFileLogger& aLogger);

    /**
    * Normal destructor. Destroy STS session.
    */
    ~CWSStarCoBrandingClient();
    
    
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
    void SetCoBrandingSessionL(CWSStarServiceSession* aSession);                    
    
    /**
    *   Sets version (mode) for trust client. Old or new trust
    * @param aVersion - old or new trust version 
    *   _LIT8( KTrustNamespace200502, "http://schemas.xmlsoap.org/ws/2005/02/trust");
    *   _LIT8( KTrustNamespace200404, "http://schemas.xmlsoap.org/ws/2004/04/trust");
    *
    */
    void SetHandlerCtx(MSenHandlerContext* aHndCtx);
    
    //TBool HasSuperClass( TDescriptionClassType aType );

    TInt FetchSCTL(CWSStarServiceSession& aSession,
                   HBufC8*& aErrorMessage);
        
protected://methods
    
    /**
    * C++ default constructor.
    */
    CWSStarCoBrandingClient(    CSIF& aSIF,
                                    TDescriptionClassType aType,
                                    RFileLogger& aLogger);
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void BaseConstructL();
    virtual TBool Matches(MSenServiceDescription& aPattern);
    

private: //methods
    //Co-Branding CR
    CSenElement* CreateFSTL();
    //Co-Branding CR
    TInt AdaptCoBrandEndpointL(TBool aToOrginal = EFalse);
    void AddAppInfoHeaderL();
private: // Data
    CWSStarServiceSession* iWSStarServiceSession;   // Not owned, session which we validate
    CWSStarServiceSession* iCoBrandingSession;             //STS session, not owned
    HBufC8* iCoBrandingOrginalEndpoint;             //STS session, owned
    
    CSenIdentityProvider* iIdentityProvider; // Not owned, comes from validation phase
    RFileLogger& iLog;                      //not owned
    
    CSenTransportBase* ipTransport;//owned
    MSenHandlerContext* iHndCtx; //not owned
    TInt iTpCounter;    
};

#endif // CO_BRANDING_CLIENT_H

// End of File
