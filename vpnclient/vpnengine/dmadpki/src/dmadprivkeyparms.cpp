/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CDmAdPrivKeyParms.
*
*/


#include <utf.h>

#include "dmadprivkeyparms.h"

CDmAdPrivKeyParms* CDmAdPrivKeyParms::NewL()
    {
    CDmAdPrivKeyParms* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CDmAdPrivKeyParms* CDmAdPrivKeyParms::NewLC()
    {
    CDmAdPrivKeyParms* self = new (ELeave) CDmAdPrivKeyParms();
    CleanupStack::PushL(self);
    return self;
    }
    
CDmAdPrivKeyParms::CDmAdPrivKeyParms()
    {
    iKeyType = EPKIRSA;
    }    
    
CDmAdPrivKeyParms::~CDmAdPrivKeyParms()
    {
    delete iKeyId;
    }
    
TPKIKeyAlgorithm CDmAdPrivKeyParms::KeyType() const
    {
    return iKeyType;
    }
    
void CDmAdPrivKeyParms::SetKeyTypeL(TPKIKeyAlgorithm aKeyType)
    {
    
    if (aKeyType != EPKIRSA &&
        aKeyType != EPKIDSA)
        {
        User::Leave(KErrCorrupt);
        }
    
    iKeyType = aKeyType;
    }
 
TPtrC8 CDmAdPrivKeyParms::KeyId() const
    {
    TPtrC8 ret(KNullDesC8);
    if (iKeyId != 0)
        {
        ret.Set(*iKeyId);
        }
    return ret;
    }
 
void CDmAdPrivKeyParms::SetKeyIdL(const TDesC8& aKeyId)
    {
    delete iKeyId;
    iKeyId = 0;
    if (aKeyId.Length() > 0)
        {
        iKeyId = aKeyId.AllocL();
        }
    }

TInt CDmAdPrivKeyParms::KeyLength() const
    {
    return iKeyLength;
    }
    
void CDmAdPrivKeyParms::SetKeyLength(TInt aKeyLength)
    {
    iKeyLength = aKeyLength;
    }
 
