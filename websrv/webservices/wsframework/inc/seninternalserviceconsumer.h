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
* Description:    Header declaration
*
*/








#ifndef SEN_INTERNAL_SERVICE_CONSUMER_H
#define SEN_INTERNAL_SERVICE_CONSUMER_H

// INCLUDES
#include <e32base.h>
#include <s32strm.h>

#include "senserviceinvocationframework.h"
#include "msenremoteserviceconsumer.h"
#include "senwsdescription.h"
#include <SenServiceConnection.h>

// FORWARD DECLARATION
//class MSenPolicy;
//class CSenPolicy;
class CSenServiceSession;
class CSenIdentifier;
class MSenProperties;

// CLASS DECLARATION

/**
* Base class for internal consumers of various SIF implementations,
* offers for example a unique id implementation..
*/
class CSenInternalServiceConsumer : public CSenWSDescription,
                                    public MSenRemoteServiceConsumer
    {
    public: // Constructors and destructor
        IMPORT_C virtual ~CSenInternalServiceConsumer();

        // New functions
    
        virtual CSenServiceSession* ServiceSession() = 0; 
        IMPORT_C virtual TBool IsReadyL();
        IMPORT_C RFileLogger* Log() const;  
        
        // Functions from base classes
    
        // From MSenRemoteServiceConsumer
        IMPORT_C virtual const TDesC8& Id() const;
        IMPORT_C virtual TInt SetSessionL(MSenRemoteServiceSession& aServiceSession);

        IMPORT_C virtual TInt HandleMessageL(HBufC8* aMessage,
                                             const TInt aTxnId,
                                             MSenProperties* aResponseTransportProperties);

        IMPORT_C virtual TInt HandleErrorL(HBufC8* aError,
                                           const TInt aErrorCode,
                                           const TInt aTxnId,
                                           MSenProperties* aResponseTransportProperties);

        IMPORT_C virtual void SetStatusL(const TInt aStatus);

        IMPORT_C CSenIdentifier& CSenInternalServiceConsumer::Identifier() const;

        IMPORT_C TInt ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk);

        IMPORT_C virtual void FileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
                const TDesC8& aSoapOrCid, TInt aProgress);    
        /**
        * @see CSenWSDescription
        */
        IMPORT_C virtual TBool HasSuperClass( TDescriptionClassType aType );
		IMPORT_C virtual void SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) ; 		

    protected:
        
        /**
        * C++ default constructor.
        */
        IMPORT_C CSenInternalServiceConsumer(TDescriptionClassType aType,
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */                                   CSIF& aSIF);
        IMPORT_C void ConstructL();

        /**
        * getter for children
        */
        IMPORT_C CSIF& Framework();

    private: // Data
        CSIF& iSIF; // Not owned
        // Owned. Typically an empty policy.
        //CSenPolicy* iPolicy; // is deprecated, not in use
        HBufC8* ipConsumerId;
        CSenIdentifier* ipIdentifier;
        TSenDataTrafficDetails iDetails;
    };

#endif // SEN_INTERNAL_SERVICE_CONSUMER_H

// End of File
