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
* Description: Header declaration
*
*/








#ifndef SEN_HOSTLET_REQUEST_H
#define SEN_HOSTLET_REQUEST_H

#include <e32base.h>

#include "MSenHostletRequest.h"
#include "MSenProperties.h"

// FORWARD DECLARATIONS
class CSenIdentifier;
class CSenChunk;

class CSenHostletRequest : public CBase, public MSenHostletRequest
    {
    
    public:
        /**
        * Static 2-phased constructor
        *
        * Descriptors are not copied, it will only be referenced
        * by the provider processing this request.
        * @param aRequestUtf8 must contains reference to the actual
        * request message (body, or complete message with one or more
        * headers, depending on whether or not the hostlet wishes to
        * receive complete client messages). Note that the caller of
        * this static constructor needs to keep this reference alive
        * as long as hostlet request object exists, since *no* copy
        * of original descriptor is made, but direct reference is 
        * stored instead.
        * @param aThreadId pointing to requester thread (typically full
        * name of that thread). Note that descriptor is not copied, but
        * it needs to be kept alive by the caller instead (mandatory).
        * @param aConsumerId pointing to requester service consumer 
        * (typically some URN). Note that descriptor is not copied, but
        * it needs to be kept alive by the caller instead (mandatory).
        * @return pointer to new hostler request instance
        */
        IMPORT_C static CSenHostletRequest* NewL(const TInt aRequestId,
                                                 const TDesC8& aRequestUtf8,
                                                 RThread aThread,
                                                 const CSenIdentifier& aConsumerId,
                                                 CSenChunk* apChunk);
        /**
        * Static 2-phased constructor
        *
        * Descriptor are not copied, it will only be referenced
        * by the provider processing this request.
        * @param aRequestUtf8 must contains reference to the actual
        * request message (body, or complete message with one or more
        * headers, depending on whether or not the hostlet wishes to
        * receive complete client messages). Note that the caller of
        * this static constructor needs to keep this reference alive
        * as long as hostlet request object exists, since *no* copy
        * of original descriptor is made, but direct reference is 
        * stored instead.
        * @param aThreadId pointing to requester thread (typically full
        * name of that thread). Note that descriptor is not copied, but
        * it needs to be kept alive by the caller instead (mandatory).
        * @param aConsumerId pointing to requester service consumer 
        * (typically some URN). Note that descriptor is not copied, but
        * it needs to be kept alive by the caller instead (mandatory).
        * @param apChunk
        * @return pointer to new hostler request instance, which
        * has been left on cleanup stack.
        */
        IMPORT_C static CSenHostletRequest* NewLC(const TInt aRequestId,
                                                  const TDesC8& aRequestUtf8,
                                                  RThread aThread,
                                                  const CSenIdentifier& aConsumerId,
                                                  CSenChunk* apChunk);
        
    public:
        // From MSenHostletRequest
        IMPORT_C virtual TPtrC8 RequestUtf8() const;

        // Destructor
        IMPORT_C virtual ~CSenHostletRequest();

        /**
        * Getter for requester thread id
        */
        IMPORT_C virtual TPtrC ThreadId() const;

        /** 
        * Getter for requester consumer id
        */
        IMPORT_C virtual TPtrC8 ConsumerId() const;
        
        /** 
        * Getter for request id
        */
        IMPORT_C virtual TInt RequestId() const;
        
        /**
        * Getter for request properties and type
        */
        IMPORT_C virtual TPtrC8 Properties(MSenProperties::TSenPropertiesClassType& aType) const;
                

    private:
        // C++ constructor
        CSenHostletRequest(const TInt aRequestId,
                           const TDesC8& aRequestUtf8, 
                           RThread aThread,
                           const CSenIdentifier& aConsumerId,
                           CSenChunk* apChunk);
                           
        void ConstructL();
        
    private: // Data
        TInt iRequestId;
        TPtrC8 iRequestUtf8;                        // not owned reference to the request
        RThread iThread;            
        const CSenIdentifier& iConsumerIdentifier;  // not owned reference to service consumer
        CSenChunk* ipChunk;                          // not owned
        
        TAny* ipReserved;                           // not used currently
    };

#endif
