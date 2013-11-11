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











// INCLUDE FILES
#include <w32std.h>

#include "seninternalserviceconsumer.h"
#include "senservicesession.h"
#include "senidentifier.h"

//#include "MSenPolicy.h" // internal ServiceDescription\inc 
#include "senpolicy.h" // internal ServiceDescription\inc 
#include "sendebug.h" // internal Utils\inc

EXPORT_C CSenInternalServiceConsumer::CSenInternalServiceConsumer(
                                                TDescriptionClassType aType,
                                                CSIF& aSIF)
:   CSenWSDescription(aType),
    iSIF(aSIF),
    //iPolicy(NULL), // deprecated, not in use
    ipConsumerId(NULL),
    ipIdentifier(NULL)
    {
    }


EXPORT_C void CSenInternalServiceConsumer::ConstructL()
    {
    // Set the local name to "ServiceDescription"
    // and initialize the inner ipElement
    CSenWSDescription::ConstructL();

    // Construct an empty policy
    // iPolicy = CSenPolicy::NewL(); // deprecated, not in use

    ipConsumerId = iSIF.Manager().RandomGuidL();
    ipIdentifier = CSenIdentifier::NewL(SenIdentifier::ESession, 
                                        *ipConsumerId);
    }

EXPORT_C CSenInternalServiceConsumer::~CSenInternalServiceConsumer()
    {
    //delete iPolicy; // deprecated, not in use
    delete ipConsumerId;
    delete ipIdentifier;
    }

EXPORT_C TBool CSenInternalServiceConsumer::IsReadyL()
    {
    if(!ServiceSession())
        {
        return EFalse;
        }
    else
        {
        return ServiceSession()->IsReadyL();
        }
    }

EXPORT_C const TDesC8& CSenInternalServiceConsumer::Id() const
    {
    if(ipConsumerId)
        {
        return *ipConsumerId;
        }
    else
        {
        return KNullDesC8();
        }
    }

EXPORT_C TInt CSenInternalServiceConsumer::SetSessionL(MSenRemoteServiceSession& /* aServiceSession */)
    {
    return KErrNone;
    }

EXPORT_C TInt CSenInternalServiceConsumer::HandleMessageL(HBufC8* apMessage,
                                                          const TInt /* aTxnId */,
                                                          MSenProperties* /* aResponseTransportProperties */)
    {
    delete apMessage;
    return KErrNone; // KErrNotImplemented
    }


EXPORT_C TInt CSenInternalServiceConsumer::HandleErrorL(HBufC8* apError,
                                                        const TInt /* aErrorCode */,
                                                        const TInt /* aTxnId */,
                                                        MSenProperties* /* aResponseTransportProperties */)
    {
    delete apError;
    return KErrNone; // KErrNotImplemented
    }

EXPORT_C void CSenInternalServiceConsumer::SetStatusL(const TInt /* aStatus */)
    {
    }

EXPORT_C CSIF& CSenInternalServiceConsumer::Framework()
    {
    return iSIF;
    }

/*
EXPORT_C RFileLogger* CSenInternalServiceConsumer::Log() const
    {
    // logs all into the service invocation framework's log:
    return iSIF.Log();
*/

EXPORT_C CSenIdentifier& CSenInternalServiceConsumer::Identifier() const
    {
    return *ipIdentifier;
    }

EXPORT_C TInt CSenInternalServiceConsumer::ChunkByTxnIdL(TInt /* aTxnId */, CSenChunk*& /* aChunk */)
    {
    return KErrNotSupported;
    }

EXPORT_C TBool CSenInternalServiceConsumer::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::EWSDescription ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenWSDescription::HasSuperClass( aType ); 
        }
    }

EXPORT_C void CSenInternalServiceConsumer::FileProgress(TInt /*aTxnId*/, TBool /*aIncoming*/, TBool /*aIsSoap*/,
                const TDesC8& /*aSoapOrCid*/, TInt /*aProgress*/)
    {
    }
EXPORT_C void CSenInternalServiceConsumer::SetDataTrafficDetails( TSenDataTrafficDetails& /* aDetails*/)  
	{
	}


// End of File
