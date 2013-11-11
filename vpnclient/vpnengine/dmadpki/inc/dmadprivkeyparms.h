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
* Description: Class definition of CDmAdPrivKeyParms.
*
*/



#ifndef __DMADPRIVKEYPARMS_H__
#define __DMADPRIVKEYPARMS_H__

#include <e32base.h>

#include "pkidefs.h"

class CDmAdPrivKeyParms : public CBase
    {
public:
    static CDmAdPrivKeyParms* NewL();
    static CDmAdPrivKeyParms* NewLC();
    ~CDmAdPrivKeyParms();
    
    TPKIKeyAlgorithm KeyType() const;
    void SetKeyTypeL(TPKIKeyAlgorithm aType);
    TPtrC8 KeyId() const;
    void SetKeyIdL(const TDesC8& aKeyId);
    TInt KeyLength() const;
    void SetKeyLength(TInt aType);
    
private:
    CDmAdPrivKeyParms();
    
private:
    TPKIKeyAlgorithm    iKeyType;
    TInt                iKeyLength;
    HBufC8*             iKeyId;  
    };

#endif
