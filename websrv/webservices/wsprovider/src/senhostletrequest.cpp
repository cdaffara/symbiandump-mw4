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
* Description:        
*
*/








#include "senhostletrequest.h"

#include "senidentifier.h"  // internal, Utils\inc
#include "senchunk.h"       // internal, Utils\inc

EXPORT_C CSenHostletRequest* CSenHostletRequest::NewL(const TInt aRequestId,
                                                      const TDesC8& aRequestUtf8,
                                                      RThread aThread,
                                                      const CSenIdentifier& aConsumerId,
                                                      CSenChunk* apChunk)
    {
    CSenHostletRequest* pNew = CSenHostletRequest::NewLC(aRequestId,
                                                         aRequestUtf8, 
                                                         aThread, 
                                                         aConsumerId,
                                                         apChunk);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenHostletRequest* CSenHostletRequest::NewLC(const TInt aRequestId,
                                                       const TDesC8& aRequestUtf8,
                                                       RThread aThread,
                                                       const CSenIdentifier& aConsumerId,
                                                       CSenChunk* apChunk)
    {
    CSenHostletRequest* pNew = 
        new(ELeave) CSenHostletRequest(aRequestId, aRequestUtf8, aThread, aConsumerId, apChunk);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenHostletRequest::CSenHostletRequest(const TInt aRequestId,
                                       const TDesC8& aRequestUtf8,
                                       RThread aThread,
                                       const CSenIdentifier& aConsumerId,
                                       CSenChunk* apChunk)
: iRequestId(aRequestId),
  iRequestUtf8(aRequestUtf8),
  iThread(aThread),
  iConsumerIdentifier(aConsumerId),
  ipChunk(apChunk)
        {
        }
  
EXPORT_C CSenHostletRequest::~CSenHostletRequest()
    {
    }

void CSenHostletRequest::ConstructL()
    {
    
    }

EXPORT_C TPtrC8 CSenHostletRequest::RequestUtf8() const
    {
    return iRequestUtf8;
    }


EXPORT_C TPtrC CSenHostletRequest::ThreadId() const
    {
    return iThread.FullName();
    }

EXPORT_C TPtrC8 CSenHostletRequest::ConsumerId() const
    {
    return iConsumerIdentifier.Value();
    }

EXPORT_C TInt CSenHostletRequest::RequestId() const
    {
    return iRequestId;
    }


EXPORT_C TPtrC8 CSenHostletRequest::Properties(MSenProperties::TSenPropertiesClassType& aType) const
    {
    
    if(!ipChunk)
        {
        aType = MSenProperties::ENotInUse;
        return KNullDesC8();
        }
        
    aType = ipChunk->ChunkHeader().PropertiesType();
    TPtrC8 properties;
    ipChunk->DescFromChunk(properties, 1);
    return properties;
    }


// END OF FILE



    
    
