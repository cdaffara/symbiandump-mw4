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
#include "senidwsfanonymoussaslmechanism.h"
#include "msensaslmessage.h"

namespace
    {
    _LIT8(KAnonymous, "ANONYMOUS");
    _LIT8(KAnonUsername, "Anonymous");
    }

// Create instance of concrete ECOM interface implementation
CSenIdWsfAnonymousSaslMechanism* CSenIdWsfAnonymousSaslMechanism::NewL(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenIdWsfAnonymousSaslMechanism* pNew =
                    CSenIdWsfAnonymousSaslMechanism::NewLC(aServiceManager);
    CleanupStack::Pop();
    return pNew;
    }

CSenIdWsfAnonymousSaslMechanism* CSenIdWsfAnonymousSaslMechanism::NewLC(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenIdWsfAnonymousSaslMechanism* pNew =
                new (ELeave) CSenIdWsfAnonymousSaslMechanism(aServiceManager);
    CleanupStack::PushL(pNew);
    return pNew;
    }
CSenIdWsfAnonymousSaslMechanism::CSenIdWsfAnonymousSaslMechanism(
                                    MSenCoreServiceManager& aServiceManager)
: CSenSecurityMechanism(aServiceManager)
    {
    }


const TDesC8& CSenIdWsfAnonymousSaslMechanism::Name()
    {
    return KAnonymous();
    }

TInt CSenIdWsfAnonymousSaslMechanism::HandleResponseL(
    MSenSaslMessage& /*aResponse*/,
    MSenSaslMessage& aNewRequest
    )
    {
    // Return a SASLRequest that continues
    // ANONYMOUS with response, should not happen though!
    return aNewRequest.ConstructInitRequestFromL(KAnonUsername, *ipAccount);
    }

TBool CSenIdWsfAnonymousSaslMechanism::IsPasswordFromUser()
    {
        return ETrue;
    }

// End of File
