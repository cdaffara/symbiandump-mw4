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
#include "SenIdentityProviderIdArray8.h"


EXPORT_C CSenIdentityProviderIdArray8* CSenIdentityProviderIdArray8::NewL()
    {
    CSenIdentityProviderIdArray8* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenIdentityProviderIdArray8* CSenIdentityProviderIdArray8::NewLC()
    {
    // without an argument, a non-strict array is created:
    CSenIdentityProviderIdArray8* pNew =
                            new (ELeave) CSenIdentityProviderIdArray8(EFalse);

    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

EXPORT_C CSenIdentityProviderIdArray8* CSenIdentityProviderIdArray8::NewL(
                                                        const TBool aStrict)
    {
    CSenIdentityProviderIdArray8* pNew = NewLC(aStrict);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenIdentityProviderIdArray8* CSenIdentityProviderIdArray8::NewLC(
                                                        const TBool aStrict)
    {
    CSenIdentityProviderIdArray8* pNew =
                            new (ELeave) CSenIdentityProviderIdArray8(aStrict);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }


void CSenIdentityProviderIdArray8::ConstructL()
    {
    }

CSenIdentityProviderIdArray8::CSenIdentityProviderIdArray8(const TBool aStrict)
:   CDesC8ArraySeg(5),
    iStrict(aStrict)
    {
    }

// destructor
EXPORT_C CSenIdentityProviderIdArray8::~CSenIdentityProviderIdArray8()
    {

    }

EXPORT_C TBool CSenIdentityProviderIdArray8::IsStrict() const
    {
    return iStrict;
    }

EXPORT_C void CSenIdentityProviderIdArray8::SetStrict(TBool aStrict)
    {
    iStrict =  aStrict;
    }
// End of File
