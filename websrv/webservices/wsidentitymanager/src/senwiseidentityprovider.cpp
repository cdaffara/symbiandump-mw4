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
#include "senwiseidentityprovider.h"

namespace
    {
    _LIT8(KIMEI, "IMEI");
    }


EXPORT_C CSenWiseIdentityProvider* CSenWiseIdentityProvider::NewL(
                                                    TPtrC8 aIMEI,
                                                    const TDesC8& aEndPoint,
                                                    const TDesC8& aContract)
    {
    CSenWiseIdentityProvider* pNew = NewLC( aIMEI, aEndPoint, aContract );
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenWiseIdentityProvider* CSenWiseIdentityProvider::NewLC(
                                                       TPtrC8 aIMEI,
                                                       const TDesC8& aEndPoint,
                                                       const TDesC8& aContract)
    {
    CSenWiseIdentityProvider* pNew =
            new (ELeave) CSenWiseIdentityProvider(EIdentityProvider, aIMEI);
    CleanupStack::PushL(pNew);
    pNew->ConstructL( aEndPoint, aContract );
    return pNew;
    }

void CSenWiseIdentityProvider::ConstructL(const TDesC8& aEndPoint,
                                          const TDesC8& aContract)
    {
    CSenIdentityProvider::ConstructL(aEndPoint, aContract);
    }

CSenWiseIdentityProvider::CSenWiseIdentityProvider(TDescriptionClassType aType,
                                                   TPtrC8 aIMEI)
:   CSenIdentityProvider(aType),
    iIMEI(aIMEI)
    {
    }

EXPORT_C CSenWiseIdentityProvider::~CSenWiseIdentityProvider()
    {
    }

TPtrC8 CSenWiseIdentityProvider::AuthzID()
    {
    TPtrC8 authzid = CSenIdentityProvider::AuthzID();
    if (authzid == KIMEI) return iIMEI;
    return authzid;
    }

TPtrC8 CSenWiseIdentityProvider::AdvisoryAuthnID()
    {
    TPtrC8 advisoryauthnid = CSenIdentityProvider::AdvisoryAuthnID();
    if (advisoryauthnid == KIMEI) return iIMEI;
    return advisoryauthnid;
    }

TPtrC8 CSenWiseIdentityProvider::Password()
    {
    TPtrC8 password = CSenIdentityProvider::Password();
    if (password == KIMEI) return iIMEI;
    return password;
    }

// End of File
