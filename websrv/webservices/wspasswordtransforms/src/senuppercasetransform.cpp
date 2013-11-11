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
#include "senuppercasetransform.h"
#include "msencoreservicemanager.h"


// Create instance of concrete ECOM interface implementation
CSenUppercaseTransform* CSenUppercaseTransform::NewL(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenUppercaseTransform* pNew =
                                CSenUppercaseTransform::NewLC(aServiceManager);
    CleanupStack::Pop();
    return pNew;
    }


CSenUppercaseTransform* CSenUppercaseTransform::NewLC(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenUppercaseTransform* pNew =
                        new (ELeave) CSenUppercaseTransform(aServiceManager);
    CleanupStack::PushL(pNew);
    return pNew;
    }

CSenUppercaseTransform::CSenUppercaseTransform(
                                    MSenCoreServiceManager& aServiceManager)
: CSenPasswordTransform(aServiceManager)
    {
    }

CSenUppercaseTransform::~CSenUppercaseTransform()
    {
    }

// method does nothing
TInt CSenUppercaseTransform::SetParameterL(TPtrC8 /* aName */,
                                           TPtrC8 /* aValue */)
    {
    // default does nothing
    return KErrNone;
    }

HBufC8* CSenUppercaseTransform::TransformL(TPtrC8 aPassphrase) const
    {
    // NOTE: Conversion is implemented as appropriate to the current locale.
    HBufC8* pUpper = aPassphrase.AllocL();
    TPtr8 upper = pUpper->Des();
    upper.UpperCase();
    return pUpper;
    }

// End of File
