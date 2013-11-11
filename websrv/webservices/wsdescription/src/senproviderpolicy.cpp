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
* Description:        This class represents the policy information, which has been
*                resolved in some way from the actual WSP. Currently applications
*                typically "just know", which policy WSP prefers, but in the
*                future this class may be used as a container for data
*                originating directly from a WSP (as the for example, when the
*                Liberty ID-WSF evolves).
*                To point out a single feature, lets go through the Identity
*                Provider ID list getter, setter and adder -methods; these are
*                meant to be used as a storing place for a the policy information
*                Typically the IDP ID list is just a recommendation, and rarely
*                a strict list.
*
*/








// INCLUDE FILES
#include <e32std.h>
#include <s32strm.h>

#include "senproviderpolicy.h"
#include "SenElement.h"
#include "SenXmlUtils.h"

#include "SenIdentityProviderIdArray8.h"

namespace
    {
    _LIT8(KTransportLocalName, "Transport");
    }
// ============================= LOCAL FUNCTIONS ===============================

namespace
    {
    _LIT8(KProviderPolicyLocalName, "ProviderPolicy");
    }


EXPORT_C CSenProviderPolicy* CSenProviderPolicy::NewL()
    {
    CSenProviderPolicy* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenProviderPolicy* CSenProviderPolicy::NewLC()
    {
    CSenProviderPolicy* pNew = new (ELeave) CSenProviderPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }


EXPORT_C CSenProviderPolicy* CSenProviderPolicy::NewL(TUint32 aIapId)
    {
    CSenProviderPolicy* pNew = NewLC(aIapId);
    CleanupStack::Pop();
    return(pNew);
    }


EXPORT_C CSenProviderPolicy* CSenProviderPolicy::NewLC(TUint32 aIapId)
    {
    CSenProviderPolicy* pNew = new (ELeave) CSenProviderPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aIapId);
    return pNew;
    }

EXPORT_C CSenProviderPolicy* CSenProviderPolicy::NewL(TUint32 aId, TBool aIsSnapId)
    {
    CSenProviderPolicy* pNew = NewLC(aId, aIsSnapId);
    CleanupStack::Pop();
    return(pNew);
    }


EXPORT_C CSenProviderPolicy* CSenProviderPolicy::NewLC(TUint32 aId, TBool aIsSnapId)
    {
    CSenProviderPolicy* pNew = new (ELeave) CSenProviderPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aId, aIsSnapId);
    return pNew;
    }

CSenProviderPolicy::CSenProviderPolicy()
    {
    }

CSenProviderPolicy::~CSenProviderPolicy()
    {
    }

EXPORT_C void CSenProviderPolicy::BaseConstructL()
    {
    CSenPolicy::BaseConstructL(KProviderPolicyLocalName());
    }

EXPORT_C void CSenProviderPolicy::BaseConstructL(TUint32 aIapId)
    {
    CSenPolicy::BaseConstructL(KProviderPolicyLocalName(), aIapId);
    }

EXPORT_C void CSenProviderPolicy::BaseConstructL(TUint32 aId, TBool aIsSnapId)
    {
    CSenPolicy::BaseConstructL(KProviderPolicyLocalName(), aId, aIsSnapId);
    }

EXPORT_C TBool CSenProviderPolicy::Accepts(MSenProviderPolicy& aPolicyPattern)
    {
    TBool retVal(ETrue);
    TRAPD(err, retVal = AcceptsL(aPolicyPattern));
    if (err) return EFalse;
    return retVal;
    }

TBool CSenProviderPolicy::AcceptsL(MSenProviderPolicy& aPolicyPattern)
    {
    TUint32 thisIapID(0);
    TUint32 patternIapID(0);

    if(aPolicyPattern.IapId(patternIapID)==KErrNone
        && (IapId(thisIapID) != KErrNone
         || patternIapID!=thisIapID))
        {
        return EFalse; // policies do NOT match(!)
        }
    
    if(aPolicyPattern.SnapId(patternIapID)==KErrNone
        && (SnapId(thisIapID) != KErrNone
         || patternIapID!=thisIapID))
        {
        return EFalse; // policies do NOT match(!)
        }    

    const CSenIdentityProviderIdArray8& patternIdps =
                                    aPolicyPattern.IdentityProviderIds8L();
    if(patternIdps.IsStrict())
        {
        const CSenIdentityProviderIdArray8& thisIdps = IdentityProviderIds8L();
        TInt thisCount(thisIdps.Count());
        TInt pos(KErrNotFound);
        TBool matches(EFalse);
        for(TInt i=0; i<thisCount; i++)
            {
            pos = KErrNotFound;
            if(patternIdps.Find(thisIdps[i], pos, ECmpNormal) == KErrNone)
                {
                matches = ETrue;
                break;
                }
            }
        return matches;
        }

    return ETrue;
    }

// sets (rebuilds) the IAP ID and IDP ID list values from template
// @return  - a error, if at least one addition of new contents has
// failed OR KerrNone if every property was successfully reset
EXPORT_C TInt CSenProviderPolicy::RebuildFrom(MSenProviderPolicy& aTemplate)
    {
    TInt retVal(KErrNone);
    TRAPD(err, retVal = RebuildFromL(aTemplate));
    if (err != KErrNone)
        {
        retVal = err;
        }
    return retVal;
    }

TInt CSenProviderPolicy::RebuildFromL(MSenProviderPolicy& aTemplate)
    {
    TInt retVal(KErrNone);
        // check for possible provider policy definition

    TUint32 iapId(0);
    TInt err = aTemplate.IapId(iapId);
    if(err==KErrNone)
        {
        // update the latest IAP "recommendation"
        SetIapIdL(iapId);
        }
    
    err = aTemplate.SnapId(iapId);
    if(err==KErrNone)
        {
        // update the latest SNAP "recommendation"
        SetSnapIdL(iapId);
        }    
    const CSenIdentityProviderIdArray8& ids =
                                        aTemplate.IdentityProviderIds8L();
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

EXPORT_C TInt CSenProviderPolicy::SetTransportPropertiesL(const TDesC8& aProperties)
    {
    CSenElement* pTransportElement = AsElement().Element(KTransportLocalName);
    if(!pTransportElement)
        {
        pTransportElement = &(AsElement().AddElementL(KTransportLocalName));
        }
    pTransportElement->SetContentL(aProperties);
    return KErrNone;
    }


EXPORT_C TInt CSenProviderPolicy::TransportPropertiesL(HBufC8*& aProperties)
    {
    CSenElement* pTransportElement = AsElement().Element(KTransportLocalName);
    if(pTransportElement)
        {
        aProperties = pTransportElement->AsXmlL();
        }
    return KErrNone;
    }

// End of file
