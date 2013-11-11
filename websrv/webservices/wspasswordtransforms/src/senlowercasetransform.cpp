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
#include "senlowercasetransform.h"
#include "msencoreservicemanager.h"


// Create instance of concrete ECOM interface implementation
CSenLowercaseTransform* CSenLowercaseTransform::NewL(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenLowercaseTransform* pNew =
                                CSenLowercaseTransform::NewLC(aServiceManager);
    CleanupStack::Pop();
    return pNew;
    }


CSenLowercaseTransform* CSenLowercaseTransform::NewLC(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenLowercaseTransform* pNew =
                        new (ELeave) CSenLowercaseTransform(aServiceManager);
    CleanupStack::PushL(pNew);
    return pNew;
    }

CSenLowercaseTransform::CSenLowercaseTransform(
                                    MSenCoreServiceManager& aServiceManager)
: CSenPasswordTransform(aServiceManager)
    {
    }

CSenLowercaseTransform::~CSenLowercaseTransform()
    {
    }

// method does nothing
TInt CSenLowercaseTransform::SetParameterL(TPtrC8 /* aName */,
                                           TPtrC8 /* aValue */)
    {
    // default does nothing
    return KErrNone;
    }

HBufC8* CSenLowercaseTransform::TransformL(TPtrC8 aPassphrase) const
    {
    // NOTE: Conversion is implemented as appropriate to the current locale.
    HBufC8* pLower = aPassphrase.AllocL();
    TPtr8 lower = pLower->Des();
    lower.LowerCase();
    return pLower;
    }

// End of File
