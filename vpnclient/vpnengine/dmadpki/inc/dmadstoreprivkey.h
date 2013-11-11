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
* Description: Class definition of CDmAdPrivKey.
*
*/



#ifndef __DMADSTOREPRIVKEY_H__
#define __DMADSTOREPRIVKEY_H__

#include <e32std.h>
#include <x509cert.h>
#include <x500dn.h>

#include "pkiserviceapi.h"
#include "dmadprivkeyparms.h"

class CDmAdPrivKey : public CBase
    {
public:
    static CDmAdPrivKey* NewL(RPKIServiceAPI& aPkiServiceApi);
    static CDmAdPrivKey* NewLC(RPKIServiceAPI& aPkiServiceApi);
    ~CDmAdPrivKey();
    
    TBool FindL(const TDesC8& aLuid);    
    void FetchL(const TDesC8& aLuid, CDmAdPrivKeyParms& aData);
    void DeleteL(const TDesC8& aLuid);
    void ListL(RPointerArray<HBufC8>& aLuidList);

private:
    CDmAdPrivKey(RPKIServiceAPI& aPkiServiceApi);


    RPKIServiceAPI* iPkiServiceApi;    
    };

#endif
