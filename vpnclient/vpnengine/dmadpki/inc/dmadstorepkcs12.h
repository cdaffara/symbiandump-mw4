/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class definition of CDmAdPKCS12
*
*/



#ifndef __DMADSTOREPKCS12_H__
#define __DMADSTOREPKCS12_H__

#include <e32std.h>
#include <x509cert.h>
#include <x500dn.h>

#include "eventlogger.h"

#include "pkiserviceapi.h"
#include "dmadpkcs12parms.h"

class CDmAdPKCS12 : public CBase
    {
public:
    static CDmAdPKCS12* NewL(RPKIServiceAPI& aPkiServiceApi);
    static CDmAdPKCS12* NewLC(RPKIServiceAPI& aPkiServiceApi);
    ~CDmAdPKCS12();

    HBufC8* AddL(const CDmAdPKCS12Parms& aData); // returns Luid

private:
    void ConstructL();
    CDmAdPKCS12(RPKIServiceAPI& aPkiServiceApi);

private:
    RPKIServiceAPI*                             iPkiServiceApi; // ref
    DEFINE_EVENT_LOGGER
    };

#endif
