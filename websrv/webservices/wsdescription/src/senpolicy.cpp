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

//#include "SenDebug.h"

#include <SenElement.h>
#include <SenXmlUtils.h>

#include "senpolicy.h"
#include "SenIdentityProviderIdArray8.h"

// ============================ LOCAL FUNCTIONS ===============================

namespace
    {
    _LIT8(KPolicyLocalName, "Policy");
    _LIT8(KTransportLocalName, "Transport");
    _LIT8(KIapIdLocalName, "IapId");
    _LIT8(KSnapIdLocalName, "SnapId");
    _LIT8(KIdentityProviderIDsLocalName, "IdentityProviderIDs");
    _LIT8(KIdentityProviderIDsStrictAttrName, "strict");
    _LIT8(KIdentityProviderIDsStrictAttrTrue, "true");
    _LIT8(KIdentityProviderIDLocalName, "IdentityProviderID");
    }


EXPORT_C CSenPolicy* CSenPolicy::NewL()
    {
    CSenPolicy* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenPolicy* CSenPolicy::NewLC()
    {
    CSenPolicy* pNew = new (ELeave) CSenPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(KPolicyLocalName());
    return pNew;
    }


EXPORT_C CSenPolicy* CSenPolicy::NewL(TUint32 aIapId)
    {
    CSenPolicy* pNew = NewLC(aIapId);
    CleanupStack::Pop();
    return(pNew) ;
    }


EXPORT_C CSenPolicy* CSenPolicy::NewLC(TUint32 aIapId)
    {
    CSenPolicy* pNew = new (ELeave) CSenPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(KPolicyLocalName(), aIapId);
    return pNew;
    }

EXPORT_C CSenPolicy* CSenPolicy::NewL(TUint32 aId, TBool aIsSnapId)
    {
    CSenPolicy* pNew = NewLC(aId, aIsSnapId);
    CleanupStack::Pop();
    return(pNew) ;
    }


EXPORT_C CSenPolicy* CSenPolicy::NewLC(TUint32 aId, TBool aIsSnapId)
    {
    CSenPolicy* pNew = new (ELeave) CSenPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(KPolicyLocalName(), aId, aIsSnapId);
    return pNew;
    }


CSenPolicy::CSenPolicy()
:   iIdpIdList(NULL)
    {
    }

CSenPolicy::~CSenPolicy()
    {
    delete iIdpIdList;
    }

EXPORT_C void CSenPolicy::BaseConstructL(const TDesC8& aPolicyLocalName)
    {
    CSenBaseFragment::BaseConstructL(aPolicyLocalName);
    }

EXPORT_C void CSenPolicy::BaseConstructL(const TDesC8& aPolicyLocalName,
                                         TUint32 aIapId)
    {
    CSenBaseFragment::BaseConstructL(aPolicyLocalName);
    SetIapIdL(aIapId);
    }

EXPORT_C void CSenPolicy::BaseConstructL(const TDesC8& aPolicyLocalName,
                                         TUint32 aId, TBool aIsSnapId)
    {
    CSenBaseFragment::BaseConstructL(aPolicyLocalName);
    if (aIsSnapId == EFalse)
        {
    SetIapIdL(aId);
        }
    else
        {
    SetSnapIdL(aId);
        }
    }

EXPORT_C TBool CSenPolicy::Accepts(MSenPolicy& aPolicyPattern)
    {
    TBool retVal(ETrue);
    TRAPD(err, retVal = AcceptsL(aPolicyPattern));
    if (err) return EFalse;
    return retVal;
    }

TBool CSenPolicy::AcceptsL(MSenPolicy& aPolicyPattern)
    {
    TUint32 thisIapID(0);
    TUint32 patternIapID(0);

    if(aPolicyPattern.IapId(patternIapID) == KErrNone
        && (IapId(thisIapID) != KErrNone
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
        TInt pos(-1);
        TBool matches(EFalse);
        for(TInt i=0; i<thisCount; i++)
            {
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


// Setter for IAP ID
EXPORT_C void CSenPolicy::SetIapIdL(TUint32 aIapId)
    {
    CSenElement* pTransportElement = AsElement().Element(KTransportLocalName);
    if(aIapId > 0)
        {
        if(!pTransportElement)
            {
            pTransportElement = &(AsElement().AddElementL(KTransportLocalName));
            }
        CSenElement* pSnapIdElement =
        pTransportElement->Element(KSnapIdLocalName);

        if(pSnapIdElement)
            {
            pSnapIdElement = pTransportElement->RemoveElement(KSnapIdLocalName);
            }        
        CSenElement* pIapIdElement =
                                pTransportElement->Element(KIapIdLocalName);
        if(!pIapIdElement)
            {
            pIapIdElement = &(pTransportElement->AddElementL(KIapIdLocalName));
            }
        TBuf8<128> buf;
        buf.AppendFormat(_L8("%u"), aIapId);
        pIapIdElement->SetContentL(buf);        
        }
    else 
        {
        if(pTransportElement)
            {
            CSenElement* pIapIdElement =
            pTransportElement->Element(KIapIdLocalName);

            if(pIapIdElement)
                {
                pIapIdElement = pTransportElement->RemoveElement(KIapIdLocalName);
                }
            }
        }


    }

// Getter for IAP ID
EXPORT_C TInt CSenPolicy::IapId(TUint32& aCurrentIapId)
    {
    CSenElement* pTransportElement = AsElement().Element(KTransportLocalName);
    if(pTransportElement)
        {
        CSenElement* pIapIdElement =
                                pTransportElement->Element(KIapIdLocalName);
        if(pIapIdElement && pIapIdElement->HasContent())
            {
            //SENDEBUG((_L("CSenPolicy::IapIdL: content found")));
            TPtrC8 content = pIapIdElement->Content();
            TLex8 lex(content);

            TInt err = lex.Val(aCurrentIapId, EDecimal);
            return err;
            }
        //SENDEBUG((_L("CSenPolicy::IapIdL: <IapId> element not found")));
        }
    //SENDEBUG((_L("CSenPolicy::IapIdL: <Transport> element not found")));
    return KErrNotFound;
    }

// Setter for SNAP ID
EXPORT_C void CSenPolicy::SetSnapIdL(TUint32 aSnapId)
    {
    if(aSnapId > 0)
        {
        CSenElement* pTransportElement = AsElement().Element(KTransportLocalName);
        if(!pTransportElement)
            {
            pTransportElement = &(AsElement().AddElementL(KTransportLocalName));
            }
        
        CSenElement* pIapIdElement =
        pTransportElement->Element(KIapIdLocalName);

        if(pIapIdElement)
            {
            pIapIdElement = pTransportElement->RemoveElement(KIapIdLocalName);
            }        
        
        CSenElement* pSnapIdElement =
                                pTransportElement->Element(KSnapIdLocalName);
        if(!pSnapIdElement)
            {
            pSnapIdElement = &(pTransportElement->AddElementL(KSnapIdLocalName));
            }
        TBuf8<128> buf;
        buf.AppendFormat(_L8("%u"), aSnapId);
        pSnapIdElement->SetContentL(buf);        
        }
    else
        {
        CSenElement* pTransportElement = AsElement().Element(KTransportLocalName);
        if(pTransportElement)
            {
            CSenElement* pSnapIdElement =
            pTransportElement->Element(KSnapIdLocalName);

            if(pSnapIdElement)
                {
                pSnapIdElement = pTransportElement->RemoveElement(KSnapIdLocalName);
                }
            }
        }

    }

// Getter for SNAP ID
EXPORT_C TInt CSenPolicy::SnapId(TUint32& aCurrentSnapId)
    {
    CSenElement* pTransportElement = AsElement().Element(KTransportLocalName);
    if(pTransportElement)
        {
        CSenElement* pSnapIdElement =
                                pTransportElement->Element(KSnapIdLocalName);
        if(pSnapIdElement && pSnapIdElement->HasContent())
            {
            //SENDEBUG((_L("CSenPolicy::SnapIdL: content found")));
            TPtrC8 content = pSnapIdElement->Content();
            TLex8 lex(content);

            TInt err = lex.Val(aCurrentSnapId, EDecimal);
            return err;
            }
        //SENDEBUG((_L("CSenPolicy::SnapIdL: <SnapId> element not found")));
        }
    //SENDEBUG((_L("CSenPolicy::SnapIdL: <Transport> element not found")));
    return KErrNotFound;
    }

// Setter: overrides current values with the given values from the array
EXPORT_C void CSenPolicy::SetIdentityProviderIdsL(
                                        CSenIdentityProviderIdArray8& aList)
    {
    CSenElement* pListElement =
                            AsElement().Element(KIdentityProviderIDsLocalName);

    if(pListElement)
        {
        CSenElement* pRemoved = AsElement().RemoveElement(*pListElement);
        delete pRemoved;
        }

    TInt count(aList.Count());
    if(count>0)
        {
        for(TInt i=0; i<count; i++)
            {
            TPtrC8 providerId = aList[i];
            if(providerId.Length()>0)
                {
                AddIdentityProviderIdL(providerId);
                }
            }
        }
    }

// Adder: adds a new IDP ID value at the end of the current list value(s)
// Checks for duplicate (does not insert new ID elements with equal content)
// @return KErrAlreadyExists, if a duplicate is tried to add
//         KErrArgument if a zero-length descriptor is tried
//         to add (aProviderId.Length() == 0)
//
//          NOTE: allocates a copy of given aProviderId!
//
EXPORT_C TInt CSenPolicy::AddIdentityProviderIdL(const TDesC8& aProviderId)
    {
    if(aProviderId.Length()>0)
        {
        // there is something relevant to add..

        // check next that <IdentityProviderIDs> element exists
        CSenElement* pListElement =
                            AsElement().Element(KIdentityProviderIDsLocalName);

        // check if this policy is "empty" (no content yet)
        if(!pListElement)
            {
            // add the <IdentityProviderIDs> element..
            pListElement =
                    &AsElement().AddElementL(KIdentityProviderIDsLocalName);
            }
        else
            {
            // check if ID being inserted already exists
            // (prevent inserting a duplicate)

            RPointerArray<CSenElement>& children = pListElement->ElementsL();
            TInt childCount(children.Count());

            // loop through child elements and check their content
            for(TInt i=0; i<childCount; i++)
                {
                TPtrC8 content = children[i]->Content();
                if(content == aProviderId)
                    {
                    // duplicate found(!)
                    return KErrAlreadyExists;   
                    }
                }

            }

        CSenElement& added =
                    pListElement->AddElementL(KIdentityProviderIDLocalName);

        added.SetContentL(aProviderId);
        // content set: ok
        return KErrNone;
        }
    else // tried to add zero length descriptor (aProviderId.Length() == 0)
        {
        // return argument error
        return KErrArgument;
        }
    }

// sets (rebuilds) the IAP ID and IDP ID list values from template
// @return  - a error, if at least one addition of new contents has
// failed OR KerrNone if every property was successfully reset
EXPORT_C TInt CSenPolicy::RebuildFrom(MSenPolicy& aTemplate)
    {
    TInt retVal(KErrNone);
    TRAPD(err, retVal = RebuildFromL(aTemplate));
    if (err != KErrNone)
        {
        retVal = err;
        }
    return retVal;
    }

TInt CSenPolicy::RebuildFromL(MSenPolicy& aTemplate)
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

// Getter: return an empty array if no IDP:s have been spesified
// or a list of IDP arrays if such value(s) have been set.
//
// Ownership of the array remains within this class
EXPORT_C const CSenIdentityProviderIdArray8&
                                        CSenPolicy::IdentityProviderIds8L()
    {
    // construct temporary array and fill it with the xml content
    CSenIdentityProviderIdArray8* pArray =
                                        CSenIdentityProviderIdArray8::NewLC();

    // check next that <IdentityProviderIDs> element exists
    CSenElement* pListElement =
                            AsElement().Element(KIdentityProviderIDsLocalName);
    if(pListElement)
        {
        // Check if strict attribute has "true" value.
        const TDesC8* pStrictValue =
                pListElement->AttrValue(KIdentityProviderIDsStrictAttrName);
        if(pStrictValue && *pStrictValue == KIdentityProviderIDsStrictAttrTrue)
            {
            pArray->SetStrict(ETrue);
            }

        // next check that child elements exist
        RPointerArray<CSenElement>& elements  = pListElement->ElementsL();
        TInt count(elements.Count());
        for(TInt i=0; i<count; i++)
            {
            CSenElement* element = elements[i];
            if(element && element->HasContent())
                {
                pArray->AppendL(element->Content());
                }
            }
        }

    delete iIdpIdList;
    iIdpIdList = pArray;
    CleanupStack::Pop(); // pArray
    return *iIdpIdList;
    }

// End of file
