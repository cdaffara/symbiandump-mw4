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
#include "senselecttransform.h"
#include "msencoreservicemanager.h"

namespace
    {
    _LIT8(KSelectParamDefVal,
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!*-+?");
    _LIT8(KSelectParamName, "allowed");
    }

// Create instance of concrete ECOM interface implementation
CSenSelectTransform* CSenSelectTransform::NewL(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenSelectTransform* pNew = CSenSelectTransform::NewLC(aServiceManager);
    CleanupStack::Pop();
    return pNew;
    }


CSenSelectTransform* CSenSelectTransform::NewLC(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenSelectTransform* pNew =
                            new (ELeave) CSenSelectTransform(aServiceManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

void CSenSelectTransform::ConstructL()
    {
    ipSelect = HBufC8::NewL(124); // 68 chars in default select-string..
    TPtr8 select = ipSelect->Des();
    select.Append(KSelectParamDefVal);
    }

CSenSelectTransform::CSenSelectTransform(
                                    MSenCoreServiceManager& aServiceManager)
:   CSenPasswordTransform(aServiceManager),
    ipSelect(NULL)
    {
    }

CSenSelectTransform::~CSenSelectTransform()
    {
    delete ipSelect;
    }

// NOTE: returns KErrNotSupported, if parameter name is unknown!
TInt CSenSelectTransform::SetParameterL(TPtrC8 aName,
                                        TPtrC8 aValue)
    {
    if(aName == KSelectParamName)
        {
        delete ipSelect;
        ipSelect = aValue.AllocL();
        return KErrNone;
        }
    else
        {
        return KErrNotSupported;
        }
    }

HBufC8* CSenSelectTransform::TransformL(TPtrC8 aPassphrase) const
    {
    // no need to use stack, this is the only leaving method here:
    HBufC8* pResult = HBufC8::NewL(aPassphrase.Length());
    TPtr8 result = pResult->Des();

    for(TInt i=0; i < aPassphrase.Length(); i++)
        {
        TChar ch(aPassphrase[i]);
        if(ipSelect->Locate(ch) != KErrNotFound)
            {
            result.Append(ch);
            }
        }
    return pResult;
    }

// End of File
