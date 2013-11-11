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
* Description:   implementation of CDmAdPKCS12
*
*/



#include "dmadutil.h"
#include "dmadstorepkcs12.h"
#include "vpnlogger.h"
#include "dmadcertxmldefs.h"
#include "XppImpl.h"
#include "XwImpl.h"
#include "pkcs12vpn.h"

#include <vpnlogmessages.rsg>

CDmAdPKCS12* CDmAdPKCS12::NewL(RPKIServiceAPI& aPkiServiceApi)
    {
    TRACE("CDmAdPKCS12::NewL");
    
    CDmAdPKCS12* self = NewLC(aPkiServiceApi);
    CleanupStack::Pop(self);
    return self;
    }

CDmAdPKCS12* CDmAdPKCS12::NewLC(RPKIServiceAPI& aPkiServiceApi)
    {
    CDmAdPKCS12* self = new (ELeave) CDmAdPKCS12(aPkiServiceApi);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CDmAdPKCS12::ConstructL()
    {
    TRACE("CDmAdPKCS12::ConstructL");
    }
    
CDmAdPKCS12::CDmAdPKCS12(RPKIServiceAPI& aPkiServiceApi) : iPkiServiceApi(&aPkiServiceApi)
    {
    }
    
CDmAdPKCS12::~CDmAdPKCS12()
    {
    TRACE("CDmAdPKCS12::~CDmAdPKCS12");
    }
    
HBufC8* CDmAdPKCS12::AddL(const CDmAdPKCS12Parms& aParms)
    {    
    TRACE("CDmAdPKCS12::AddL");

    DEBUG_LOG(_L("Instantiating pkcs12handler"));
    CPKCS12Handler* pkcs12handler = CPKCS12Handler::NewLC(*iPkiServiceApi);

    DEBUG_LOG(_L("Setting deletable"));
    pkcs12handler->SetDeletable(aParms.Deletable());

    DEBUG_LOG(_L("Setting applicability"));
    pkcs12handler->SetApplicability(aParms.Applicability());

    HBufC* pwd(NULL);
    if (aParms.Password().Length() == 0) 
        {
        DEBUG_LOG(_L("Password length is zero, no password given"));
        pwd = HBufC::NewLC(0);
        }
    else 
        {
        DEBUG_LOG(_L("Converting password to 16bit base"));
        pwd = HBufC::NewLC(aParms.Password().Length());
        pwd->Des().Copy(aParms.Password());
        }

    DEBUG_LOG(_L("Storing pkcs12 object"));
    pkcs12handler->StorePKCS12ObjectL(aParms.Content(), *pwd);

    DEBUG_LOG(_L("Freeing handler resources"));

    CleanupStack::PopAndDestroy(2, pkcs12handler);
    
    return NULL;
    }

