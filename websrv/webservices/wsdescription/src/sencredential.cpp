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
#include "SenCredential.h"

//#include "SenDebug.h"

#include <SenDateUtils.h>

#include <SenXmlElement.h>
#include <SenXmlUtils.h>


#ifdef SYMBIAN_SECURE_ECOM 
    // for S60 FP3 (2.8) platform or newer (3.0 and above)
    #include <xml/attribute.h> // needed for RAttributeArray
//#else  // for S60 FP2 platform (2.6) or older
    //#include "Attribute.h"
#endif

using namespace Xml;

namespace
{
    /* milliseconds before actual notOnOrAfter time
     * when credentials are treated
     * as expired.
     */

    // Identifiers which are accepted (and searched after)
    _LIT8(KId,                      "ID");
    _LIT8(Kwsu_ns_plus_Id,          "wsu:Id");
    _LIT8(KAssertionId,             "AssertionID");

    _LIT8(KConditions,              "Conditions");
    _LIT8(KVal_NotOnOrAfter,        "NotOnOrAfter");
}

EXPORT_C CSenCredential* CSenCredential::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    CSenCredential* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredential* CSenCredential::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    CSenCredential* pNew = new (ELeave) CSenCredential;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

EXPORT_C CSenCredential* CSenCredential::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes,
    CSenElement& aParent
    )
    {
    CSenCredential* pNew = NewLC(aNsUri, aLocalName, 
                                aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredential* CSenCredential::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes,
    CSenElement& aParent
    )
    {
    CSenCredential* pNew = new (ELeave) CSenCredential;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }


EXPORT_C CSenCredential* CSenCredential::NewL(
                                             const CSenCredential& aCredential)
    {
    CSenCredential* pNew = NewLC(aCredential);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredential* CSenCredential::NewLC(
                                             const CSenCredential& aCredential)
    {
    CSenCredential* pNew = new (ELeave) CSenCredential;
    CleanupStack::PushL(pNew);
    pNew->ConstructL((CSenCredential&)aCredential);
    return pNew;
    }


EXPORT_C void CSenCredential::ConstructL(CSenCredential& aCredential)
    {
    ipElement = CSenXmlElement::NewL(aCredential.ipElement->LocalName());
    ipElement->CopyFromL(*(CSenXmlElement*)(aCredential.ipElement));
    if (aCredential.ValidUntil() != Time::NullTTime())
        {
        SetValidUntil(aCredential.ValidUntil());
        }
    SetIdL(aCredential.Id());
    }

EXPORT_C void CSenCredential::ConstructL(
    const TDesC8&  aNsUri,
    const TDesC8&  aLocalName,
    const TDesC8&  aQName,
    const RAttributeArray& aAttributes,
    CSenElement& aParent
    )
    {
    BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    // this ensures we take note of possible namespaces and
    // prefixed attributes
    CSenBaseFragment::SetAttributesL(aAttributes);
    // we want to save its ID and validity

    TPtrC8 identifier = SenXmlUtils::AttrValue(aAttributes, KId);
    if(identifier.Length()>0)
        {
        SetIdL(identifier);
        }

    TPtrC8 identifier2 = SenXmlUtils::AttrValue(aAttributes, Kwsu_ns_plus_Id);
    if(identifier2.Length()>0)
        {
        SetIdL(identifier2);
        }
    
    TPtrC8 identifier3 = SenXmlUtils::AttrValue(aAttributes, KAssertionId);
    if(identifier3.Length()>0)
        {
        SetIdL(identifier3);
        }

    /*

    for(int i=0; i<aAttributes.Count(); i++)
        {
        TPtrC8 attrName = aAttributes[i].Attribute().LocalName().DesC();
        if( attrName == KId)
            {

            SetIdL(aAttributes[i].Value().DesC());
            }
        else if(attrName == Kwsu_ns_plus_Id)
            {
            SetIdL(aAttributes[i].Value().DesC());
            }

        else if(attrName == KAssertionId)
            {
            SetIdL(aAttributes[i].Value().DesC());
            }
        }
    */

    }

EXPORT_C void CSenCredential::ConstructL(
    const TDesC8&  aNsUri,
    const TDesC8&  aLocalName,
    const TDesC8&  aQName,
    const RAttributeArray& aAttributes )
    {
    BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    // this ensures we take note of possible namespaces and
    // prefixed attributes
    CSenBaseFragment::SetAttributesL(aAttributes);

    TPtrC8 identifier = SenXmlUtils::AttrValue(aAttributes, KId);
    if(identifier.Length()>0)
        {
        SetIdL(identifier);
        }

    TPtrC8 identifier2 = SenXmlUtils::AttrValue(aAttributes, Kwsu_ns_plus_Id);
    if(identifier2.Length()>0)
        {
        SetIdL(identifier2);
        }
    
    TPtrC8 identifier3 = SenXmlUtils::AttrValue(aAttributes, KAssertionId);
    if(identifier3.Length()>0)
        {
        SetIdL(identifier3);
        }
    /*
    // we want to save its ID and validity
    for(int i=0; i<aAttributes.Count(); i++)
        {
        TPtrC8 attrName = aAttributes[i].Attribute().LocalName().DesC();
        if(attrName == KId)  
            {
            SetIdL(aAttributes[i].Value().DesC());
            }

        if(attrName == Kwsu_ns_plus_Id)
            {
            SetIdL(aAttributes[i].Value().DesC());
            }

        if(attrName == KAssertionId)
            {
            SetIdL(aAttributes[i].Value().DesC());
            }
        }
    */
    }

EXPORT_C CSenCredential::CSenCredential()
:
    iId(NULL),
    iNotOnOrAfter(Time::NullTTime())
    {
    }

EXPORT_C CSenCredential::~CSenCredential()
    {
    delete iId;
    }

void CSenCredential::SetIdL(const TDesC8& aNewId)
    {
    HBufC8* pNew = aNewId.AllocL();
    delete iId;
    iId = pNew;
    }

EXPORT_C const TDesC8& CSenCredential::Id()
    {
    if(iId)
        {
        return *iId;
        }
    else
        {
        return KNullDesC8();
        }
    }


EXPORT_C void CSenCredential::StartElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    if(aLocalName.Compare(KConditions) == 0)
        {
        // we should save the notOnOrAfter attribute value
        TPtrC8 ptr = SenXmlUtils::AttrValue(aAttributes, KVal_NotOnOrAfter );

        // Convert dateTime (from XML schema) to Symbian TTime.
        // The returned time will be in UTC.
        if (ptr.Length() > 0)
            {
            iNotOnOrAfter = SenDateUtils::FromXmlDateTimeL(ptr);
            }
        }
    CSenBaseFragment::StartElementL(aNsUri, 
            aLocalName, aQName, aAttributes);
    }

EXPORT_C TTime& CSenCredential::ValidUntil()
    {
    return iNotOnOrAfter;
    }


EXPORT_C void CSenCredential::SetValidUntil(TTime& aTime)
    {
    iNotOnOrAfter = aTime;
    }


// END OF FILE

