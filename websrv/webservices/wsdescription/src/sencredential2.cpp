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
#include <SenCredential2.h>
#include <SenFacet.h>
#include <SenDateUtils.h>
#include <SenXmlElement.h> // check if this include is needed(?)
#include <SenXmlUtils.h>
#include <SenXmlServiceDescription.h>


#ifdef SYMBIAN_SECURE_ECOM
    // for S60 FP3 (2.8) platform or newer (3.0 and above)
    #include <xml/attribute.h> // needed for RAttributeArray
//#else  // for S60 FP2 platform (2.6) or older
    //#include "Attribute.h"
#endif

using namespace Xml;

namespace
    {
//    _LIT8(KSenCredentialIdentifierLocalname, "CredentialIdentifier");

    // Identifiers which are accepted (and searched after)
    _LIT8(KId,                      "ID");
    _LIT8(Kwsu_ns_plus_Id,          "wsu:Id");
    _LIT8(KAssertionId,             "AssertionID");

    _LIT8(KConditions,              "Conditions");
    _LIT8(KVal_NotOnOrAfter,        "NotOnOrAfter");
    
//    _LIT8(KSenCredentialId,         "SenCredentialId");
//    _LIT8(KProviderIdLocalName,     "ProviderID");
    
//    const TInt KCredIdBufSize       = 128;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewL()
    {
    CSenCredential2* pNew = NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewLC()
    {
    CSenCredential2* pNew = new (ELeave) CSenCredential2;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(KNullDesC8);
    return pNew;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenCredential2* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenCredential2* pNew = new (ELeave) CSenCredential2;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent)
    {
    CSenCredential2* pNew = NewLC(aNsUri, aLocalName,
                                aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent)
    {
    CSenCredential2* pNew = new (ELeave) CSenCredential2;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }
    
EXPORT_C CSenCredential2* CSenCredential2::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument)
    {
    CSenCredential2* pNew = NewLC(aNsUri, aLocalName,
                                       aQName, aAttributes,
                                       aParent, aOwnerDocument);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument)
    {
    CSenCredential2* pNew = new (ELeave) CSenCredential2;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent,
                         aOwnerDocument);
    return pNew;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewL(
                                             const CSenCredential2& aCredential)
    {
    CSenCredential2* pNew = NewLC(aCredential);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredential2* CSenCredential2::NewLC(
                                             const CSenCredential2& aCredential)
    {
    CSenCredential2* pNew = new (ELeave) CSenCredential2;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL((CSenCredential2&)aCredential);
    return pNew;
    }

EXPORT_C void CSenCredential2::ConstructL(const TDesC8& aLocalName)
    {
    CSenFragmentBase::BaseConstructL(aLocalName);
    }

EXPORT_C void CSenCredential2::BaseConstructL(CSenCredential2& aCredential)
    {
    CSenFragmentBase::BaseConstructL(aCredential.AsElementL());
    if (aCredential.ValidUntil() != Time::NullTTime())
        {
        SetValidUntil(aCredential.ValidUntil());
        }
    SetIdL(aCredential.Id());
    }

EXPORT_C void CSenCredential2::BaseConstructL(
                                        const TDesC8&  aNsUri,
                                        const TDesC8&  aLocalName,
                                        const TDesC8&  aQName,
                                        const RAttributeArray& aAttributes,
                                        TXmlEngElement& aParent)
    {
    CSenFragmentBase::BaseConstructL(aNsUri, aLocalName, aQName, aAttributes,
                                     aParent);

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
    }

EXPORT_C void CSenCredential2::BaseConstructL(const TDesC8&  aNsUri,
                                        const TDesC8&  aLocalName,
                                        const TDesC8&  aQName,
                                        const RAttributeArray& aAttributes,
                                        TXmlEngElement& aParent,
                                        RSenDocument& aOwnerDocument)
    {
    CSenFragmentBase::BaseConstructL(aNsUri, aLocalName, aQName, aAttributes,
                                     aParent, aOwnerDocument);

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
    }

EXPORT_C void CSenCredential2::BaseConstructL(const TDesC8&  aNsUri,
                                        const TDesC8&  aLocalName,
                                        const TDesC8&  aQName,
                                        const RAttributeArray& aAttributes)
    {
    CSenFragmentBase::BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);

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
    }

EXPORT_C CSenCredential2::CSenCredential2()
:
    iId(NULL),
    iNotOnOrAfter(Time::NullTTime())
    {
    }

EXPORT_C CSenCredential2::~CSenCredential2()
    {
    if(iId)
    	{
    	delete iId;    	
    	}
    }

EXPORT_C void CSenCredential2::SetIdL(const TDesC8& aNewId)
    {
    HBufC8* pNew = aNewId.AllocL();
    delete iId;
    iId = pNew;
    }

EXPORT_C const TDesC8& CSenCredential2::Id()
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


EXPORT_C void CSenCredential2::OnStartElementL(const RTagInfo& aElement,
        							           const RAttributeArray& aAttributes,
        						 	           TInt aErrorCode)
    {
    if ( aElement.LocalName().DesC().Compare(KConditions) == 0)
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
    CSenFragmentBase::OnStartElementL(aElement, aAttributes, aErrorCode);
    }

EXPORT_C TTime& CSenCredential2::ValidUntil()
    {
    return iNotOnOrAfter;
    }


EXPORT_C void CSenCredential2::SetValidUntil(TTime& aTime)
    {
    iNotOnOrAfter = aTime;
    }

// END OF FILE
