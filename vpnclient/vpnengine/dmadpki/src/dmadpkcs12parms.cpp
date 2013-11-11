/*
* Copyright (c) 2000-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CDmAdPKCS12Parms
*
*/





#include <utf.h>

#include "dmadpkcs12parms.h"
#include "vpnlogger.h"

CDmAdPKCS12Parms* CDmAdPKCS12Parms::NewL()
    {
    TRACE("CDmAdCertParms::NewL");
    
    CDmAdPKCS12Parms* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CDmAdPKCS12Parms* CDmAdPKCS12Parms::NewLC()
    {
    CDmAdPKCS12Parms* self = new (ELeave) CDmAdPKCS12Parms();
    CleanupStack::PushL(self);
    return self;
    }

CDmAdPKCS12Parms::CDmAdPKCS12Parms()
    {
    }
    
CDmAdPKCS12Parms::~CDmAdPKCS12Parms()
    {
    TRACE("CDmAdCertParms::~CDmAdCertParms");
    
    iApplicability.Reset();
    iApplicability.Close();
    delete iContent;
    delete iPassword;
    }
    
TBool CDmAdPKCS12Parms::Deletable() const
    {
    return iDeletable;
    }
    
void CDmAdPKCS12Parms::SetDeletable(TBool aDeletable)
    {
    TRACE("CDmAdCertParms::SetDeletable");
    
    iDeletable = aDeletable;
    }
 
const RArray<TUid>& CDmAdPKCS12Parms::Applicability() const
    {
    return iApplicability;
    }
 
void CDmAdPKCS12Parms::SetApplicabilityL(const RArray<TUid>& aApplicability)
    {
    TRACE("CDmAdPKCS12Parms::SetApplicabilityL");
    
    iApplicability.Reset();
    
    for (TInt i = 0; i < aApplicability.Count(); ++i)
        {
        User::LeaveIfError(iApplicability.Append(aApplicability[i]));
        }
    }

TPtrC8 CDmAdPKCS12Parms::Content() const
    {
    TPtrC8 ret(KNullDesC8);
    if (iContent != 0)
        {
        ret.Set(*iContent);
        }
    return ret;
    }
 
void CDmAdPKCS12Parms::SetContentL(const TDesC8& aContent)
    {
    TRACE("CDmAdPKCS12Parms::SetContentL");
    
    delete iContent;
    iContent = 0;
    if (aContent.Length() > 0)
        {
        iContent = aContent.AllocL();
        }
    }

TPtrC8 CDmAdPKCS12Parms::Password() const
    {
    TPtrC8 ret(KNullDesC8);
    if (iPassword)
        {
        ret.Set(*iPassword);
        }
    return ret;
    }
 
void CDmAdPKCS12Parms::SetPasswordL(const TDesC8& aPassword)
    {
    TRACE("CDmAdPKCS12Parms::SetPasswordL");

    delete iPassword;
    iPassword = NULL;
    if (aPassword.Length() > 0)
        {
        iPassword = aPassword.AllocL();
        }
    else 
        {
        iPassword = HBufC8::NewL(0);
        }
    }
    
    

