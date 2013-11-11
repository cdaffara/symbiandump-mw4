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










#ifndef WSSTAR_HTTP_CLIENT_H
#define WSSTAR_HTTP_CLIENT_H

// INCLUDES
#include <e32base.h>
#include <flogger.h>
#include <e32math.h>

#include "wsstarservicesession.h"
#include "seninternalserviceconsumer.h"
#include "SenXmlElement.h"
#include "sentransportbase.h"
/*
// FORWARD DECLARATIONS

/**
 * Class: CWSStarHttpClient
 *
 * Description: Consumer which communicate with real STS.
 * Client will try obtain credentail based on WS-Trust specification.
 */
class CWSStarHttpClient : public CSenInternalServiceConsumer
    {
public: // Constructors and destructor

    /**
    * Standard 2 phase constructor
    * @param aSIF SIF implementation used in contructor of internal consumer
    * @param aLogger logger used for logs in debug mode
    * @return new instance of trust client
    */
    static CWSStarHttpClient* NewL( CSIF& aSIF,
                                          CSenWSDescription* aInitializer,
                                          RFileLogger& aLogger );
    /**
    * Standard 2 phase constructor
    * @since Series60 4.0
    * @param aSIF SIF implementation used in contructor of internal consumer
    * @param aLogger logger used for logs in debug mode
    * @return new instance of trust client which is pushed into CleanupStack
    */
    static CWSStarHttpClient* NewLC( CSIF& aSIF,
                                           CSenWSDescription* aInitializer,
                                           RFileLogger& aLogger );

    /**
    * Normal destructor. Destroy STS session.
    */
    ~CWSStarHttpClient();
    
    
    /**
    * The most siginificant method of this class. Gets the policy from URi via HTTPS
    * @param aSession session which is validated
    */
    TInt GetHttpL( const TDesC8& aRequest, HBufC8*& apPolicyResponse, CSenWSDescription* apSD );

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
    void WriteAsXMLToL(RWriteStream& aWriteStream);
    
    /**
    * Getter for validate session
    */
    virtual CSenServiceSession* ServiceSession();
    
    /**
    * Getter for transport (taken from remote consumer of validated session)
    */
    virtual MSenTransport& TransportL();
    
    /**
    *   Sets session, 
    *   
    * @param aSession - clear session assign
    */
    void SetClientSessionL(CWSStarServiceSession* aSession);                    
    
    void SetVersion(const TDesC8* aVersion);
    
    TBool HasSuperClass( TDescriptionClassType aType );
    
	virtual void SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) ; 			
    void DataTrafficDetails( TSenDataTrafficDetails& aDetails); 		
protected://methods
    
    /**
    * C++ default constructor.
    */
    CWSStarHttpClient(    CSIF& aSIF,
                                    TDescriptionClassType aType,
                                    CSenWSDescription* aInitializer,
                                    RFileLogger& aLogger);
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void BaseConstructL();
    
    

    
private: // Data
    CWSStarServiceSession* iPolicySession;             //Policy session, owned
    const TDesC8* iVersion;
    CSenTransportBase* ipTransport;//owned
    CSenWSDescription* iInitializer;
   RFileLogger& iLog;                      //not owned
    TSenDataTrafficDetails iHttpDetails;    

};

#endif // WSSTAR_POLICY_HTTP_CLIENT_H

// End of File
