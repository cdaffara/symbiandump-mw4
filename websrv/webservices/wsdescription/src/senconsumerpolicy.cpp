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
#include <e32std.h>
#include <s32strm.h>

#include "senconsumerpolicy.h"
#include "SenElement.h"
#include "SenXmlUtils.h"

#include "SenIdentityProviderIdArray8.h"



// ============================ LOCAL FUNCTIONS ===============================

namespace
    {
    _LIT8(KConsumerPolicyLocalName, "ConsumerPolicy");
    }


EXPORT_C CSenConsumerPolicy* CSenConsumerPolicy::NewL()
    {
    CSenConsumerPolicy* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenConsumerPolicy* CSenConsumerPolicy::NewLC()
    {
    CSenConsumerPolicy* pNew = new (ELeave) CSenConsumerPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }


EXPORT_C CSenConsumerPolicy* CSenConsumerPolicy::NewL(TUint32 aIapId)
    {
    CSenConsumerPolicy* pNew = NewLC(aIapId);
    CleanupStack::Pop();
    return(pNew) ;
    }


EXPORT_C CSenConsumerPolicy* CSenConsumerPolicy::NewLC(TUint32 aIapId)
    {
    CSenConsumerPolicy* pNew = new (ELeave) CSenConsumerPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aIapId);
    return pNew;
    }

EXPORT_C CSenConsumerPolicy* CSenConsumerPolicy::NewL(TUint32 aId, TBool aIsSnapId)
    {
    CSenConsumerPolicy* pNew = NewLC(aId, aIsSnapId);
    CleanupStack::Pop();
    return(pNew) ;
    }


EXPORT_C CSenConsumerPolicy* CSenConsumerPolicy::NewLC(TUint32 aId, TBool aIsSnapId)
    {
    CSenConsumerPolicy* pNew = new (ELeave) CSenConsumerPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aId, aIsSnapId);
    return pNew;
    }

CSenConsumerPolicy::CSenConsumerPolicy()
    {
    }

CSenConsumerPolicy::~CSenConsumerPolicy()
    {
    }

EXPORT_C void CSenConsumerPolicy::BaseConstructL()
    {
    CSenPolicy::BaseConstructL(KConsumerPolicyLocalName());
    }

EXPORT_C void CSenConsumerPolicy::BaseConstructL(TUint32 aIapId)
    {
    CSenPolicy::BaseConstructL(KConsumerPolicyLocalName());
    SetIapIdL(aIapId);
    }

EXPORT_C void CSenConsumerPolicy::BaseConstructL(TUint32 aId, TBool aIsSnapId)
    {
    CSenPolicy::BaseConstructL(KConsumerPolicyLocalName());
    if (aIsSnapId == EFalse)
        {
    SetIapIdL(aId);
        }
    else
        {
    SetSnapIdL(aId);
        }   
    }

EXPORT_C TBool CSenConsumerPolicy::Accepts(MSenConsumerPolicy& aPolicyPattern)
    {
    TBool retVal(ETrue);
    TRAPD(err, retVal = AcceptsL(aPolicyPattern));
    if (err) return retVal;
    return retVal;
    }

TBool CSenConsumerPolicy::AcceptsL(MSenConsumerPolicy& aPolicyPattern)
    {
    TUint32 thisIapID(0);
    TUint32 patternIapID(0);

    if(aPolicyPattern.ConsumerIapId(patternIapID)==KErrNone
        && (IapId(thisIapID) != KErrNone
         || patternIapID!=thisIapID))
        {
        return EFalse; // policies do NOT match(!)
        }

    const CSenIdentityProviderIdArray8& patternIdps =
                                aPolicyPattern.ConsumerIdentityProviderIds8L();
    if(patternIdps.IsStrict())
        {
        const CSenIdentityProviderIdArray8& thisIdps = IdentityProviderIds8L();
        TInt thisCount(thisIdps.Count());
        TInt pos(KErrNotFound);
        TBool matches(EFalse);
        for(TInt i=0; i<thisCount; i++)
            {
            pos = KErrNotFound;
            if(patternIdps.Find(thisIdps[i], pos, ECmpNormal)==KErrNone)
                {
                matches = ETrue;
                break;
                }
            }
        return matches;
        }

    return ETrue;
    }

EXPORT_C TInt CSenConsumerPolicy::RebuildFrom(
                                    MSenConsumerPolicy& aTemplate)
    {
    TInt retVal(KErrNone);
    TRAPD(err, retVal = RebuildFromL(aTemplate));
    if (err != KErrNone)
        {
        retVal = err;
        }
    return retVal;
    }

TInt CSenConsumerPolicy::RebuildFromL(MSenConsumerPolicy& aTemplate)
    {
    TInt retVal(KErrNone);
        // check for possible provider policy definition

    TUint32 iapId(0);
    TInt err = aTemplate.ConsumerIapId(iapId);
    if(err==KErrNone)
        {
        // update the latest IAP "recommendation"
        SetIapIdL(iapId);
        }
    const CSenIdentityProviderIdArray8& ids =
                                    aTemplate.ConsumerIdentityProviderIds8L();
    TInt idsCount(ids.MdcaCount());
    for(TInt j=0; j<idsCount; j++)
        {
        TPtrC8 value = ids[j];
        if(value.Length()>0)
            {
            err = AddIdentityProviderIdL(value);
            if(err!=KErrNone)
                {
                retVal = err; // indicate that some IDP could not be added
                }
            }
        }
    return retVal;
    }

// End of file
