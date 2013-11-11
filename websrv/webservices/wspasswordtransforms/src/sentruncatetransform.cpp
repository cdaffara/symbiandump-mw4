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
#include "sentruncatetransform.h"
#include "msensaslmessage.h"


namespace
    {
    const TInt KTruncateParamDefVal = 256;
    _LIT8(KTruncateParamName, "length");
    }

// Create instance of concrete ECOM interface implementation
CSenTruncateTransform* CSenTruncateTransform::NewL(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenTruncateTransform* pNew =
                            CSenTruncateTransform::NewLC(aServiceManager);
    CleanupStack::Pop();
    return pNew;
    }

CSenTruncateTransform* CSenTruncateTransform::NewLC(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CSenTruncateTransform* pNew =
                        new (ELeave) CSenTruncateTransform(aServiceManager);
    CleanupStack::PushL(pNew);
    return pNew;
    }

CSenTruncateTransform::CSenTruncateTransform(
                                    MSenCoreServiceManager& aServiceManager)
:   CSenPasswordTransform(aServiceManager),
    iLength(KTruncateParamDefVal)
    {
    }

CSenTruncateTransform::~CSenTruncateTransform()
    {
    }

TInt CSenTruncateTransform::SetParameterL(TPtrC8 aName,
                                          TPtrC8 aValue)
    {
    if(aName == KTruncateParamName)
        {
        TLex8 value(aValue);
        return value.Val(iLength);
        }
    else
        {
        return KErrNotSupported;
        }
    }

HBufC8* CSenTruncateTransform::TransformL(TPtrC8 aPassphrase) const
    {
    TInt max =
        (iLength > aPassphrase.Length()) ? aPassphrase.Length() : iLength;
    return aPassphrase.Left(max).AllocL();
    }

// End of File
