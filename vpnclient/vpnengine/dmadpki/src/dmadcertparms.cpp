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
* Description: Implementation of CDmAdCertParms.
*
*/


#include <utf.h>

#include "dmadcertparms.h"
#include "vpnlogger.h"

CDmAdCertParms* CDmAdCertParms::NewL()
    {
    TRACE("CDmAdCertParms::NewL");
    
    CDmAdCertParms* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CDmAdCertParms* CDmAdCertParms::NewLC()
    {
    CDmAdCertParms* self = new (ELeave) CDmAdCertParms();
    CleanupStack::PushL(self);
    return self;
    }

CDmAdCertParms::CDmAdCertParms()
    {
    }
    
CDmAdCertParms::~CDmAdCertParms()
    {
    TRACE("CDmAdCertParms::~CDmAdCertParms");
    
    iApplicability.Reset();
    iApplicability.Close();
    delete iContent;    
    }
    
TPKICertificateOwnerType CDmAdCertParms::Type() const
    {
    TRACE("CDmAdCertParms::Type");    
    return iType;
    }
    
void CDmAdCertParms::SetType(TPKICertificateOwnerType aType)
    {
    TRACE("CDmAdCertParms::SetType");

    iType = aType;
    } 


TBool CDmAdCertParms::Deletable() const
    {
    return iDeletable;
    }
    
void CDmAdCertParms::SetDeletable(TBool aDeletable)
    {
    TRACE("CDmAdCertParms::SetDeletable");
    
    iDeletable = aDeletable;
    }
 
TBool CDmAdCertParms::Trusted() const
    {        
    return iTrusted;
    }
    
void CDmAdCertParms::SetTrusted(TBool aTrusted)
    {
    TRACE("CDmAdCertParms::SetTrusted");
    iTrusted = aTrusted;
    }
 
const RArray<TUid>& CDmAdCertParms::Applicability() const
    {
    return iApplicability;
    }
 
void CDmAdCertParms::SetApplicabilityL(const RArray<TUid>& aApplicability)
    {
    TRACE("CDmAdCertParms::SetApplicabilityL");
    
    iApplicability.Reset();
    
    for (TInt i = 0; i < aApplicability.Count(); ++i)
        {
        User::LeaveIfError(iApplicability.Append(aApplicability[i]));
        }
    }

TPtrC8 CDmAdCertParms::Content() const
    {
    TPtrC8 ret(KNullDesC8);
    if (iContent != 0)
        {
        ret.Set(*iContent);
        }
    return ret;
    }
 
void CDmAdCertParms::SetContentL(const TDesC8& aContent)
    {
    TRACE("CDmAdCertParms::SetContentL");
    
    delete iContent;
    iContent = 0;
    if (aContent.Length() > 0)
        {
        iContent = aContent.AllocL();
        }
    }

TPtrC8 CDmAdCertParms::KeyId() const
    {
    return iKeyId;
    }
 
void CDmAdCertParms::SetKeyId(const TDesC8& aKeyLuid)
    {
    TRACE("CDmAdCertParms::SetKeyId");
    
    __ASSERT_ALWAYS(aKeyLuid.Length() <= iKeyId.MaxLength(), User::Invariant());    
    iKeyId = aKeyLuid;
    }
