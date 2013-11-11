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
#include <SenDateUtils.h>
#include <SenXmlUtils.h>
#include <SenXmlElement.h> // check if this include is needed(?)
#include "senwsdescription.h" // state constants for credentials

#include "idwsfsiuheader.h"

CIdWsfSiuHeader* CIdWsfSiuHeader::NewL()
    {
    CIdWsfSiuHeader* pNew = new (ELeave) CIdWsfSiuHeader;
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfSiuHeader::CIdWsfSiuHeader()
    {
    }

void CIdWsfSiuHeader::ConstructL()
    {
    BaseConstructL(KSiuXmlns, KSiuName);
    iNotOnOrAfter = Time::NullTTime();
    }

CIdWsfSiuHeader::~CIdWsfSiuHeader()
    {
    delete ipCredential;
    }

TPtrC8 CIdWsfSiuHeader::Endpoint()
    {
    MSenElement* pElement = this->AsElement().Element(KEndpointName);
    if(!pElement)
        {
        return KNullDesC8();
        }
    else
        {
        return pElement->Content();
        }
    }

TPtrC8 CIdWsfSiuHeader::SecurityMechId()
    {
    MSenElement* pElement = this->AsElement().Element(KSecurityMechIdName);
    if(!pElement)
        {
        return KNullDesC8();
        }
    else
        {
        return pElement->Content();
        }
    }

const TTime& CIdWsfSiuHeader::NotOnOrAfter()
    {
    return iNotOnOrAfter;
    }

TBool CIdWsfSiuHeader::IsNotOnOrAfterSet()
    {
    return (iNotOnOrAfter.Int64() != 0);
    }

CSenCredential* CIdWsfSiuHeader::Credential()
    {
    return ipCredential;
    }

void CIdWsfSiuHeader::StartElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    switch (iState)
        {
        case KStateParsingCredentials:
            {
             // change to even state which ignores and does not save content
            iState = KStateParsingSingleCredential;

            CSenCredential* pCredential = CSenCredential::NewL(aNsUri,
                aLocalName, aQName, aAttributes, AsElement());
            delete ipCredential;
            ipCredential = pCredential;
            if (iNotOnOrAfter != Time::NullTTime())
                {
                ipCredential->SetValidUntil(iNotOnOrAfter);
                }
            DelegateParsingL(*ipCredential);
            break;
            }
        default:
            {
            if ((aLocalName == KEndpointName) ||
                (aLocalName == KSecurityMechIdName))
                {
                DelegateParsingL(aNsUri, aLocalName, aQName, aAttributes);
                }
            else if (aLocalName == KCredentialElementLocalNameName ||
                        aLocalName == KCredentialsName)
                {
                TPtrC8 notOnOrAfter =
                        SenXmlUtils::AttrValue(aAttributes, KNotOnOrAfterName);
                if (notOnOrAfter.Length() > 0)
                    {
                    iNotOnOrAfter =
                                SenDateUtils::FromXmlDateTimeL(notOnOrAfter);
                    }
                else
                    {
                    iNotOnOrAfter = Time::NullTTime();
                    }

                // even state stops saving content
                iState = KStateParsingCredentials;
                CSenBaseFragment::StartElementL(aNsUri, aLocalName,
                                                aQName, aAttributes);
                }
            else
                {
                CSenBaseFragment::StartElementL(aNsUri, aLocalName,
                                                aQName, aAttributes);
                }
            break;
            }
        }
    }

void CIdWsfSiuHeader::EndElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    switch (iState)
        {
        // this is due the SAX parsing nature of CXMLReader:
        // in start element delegateparsing(),
        // AO is blocked, and thats why the state really is
        // PARSING single CREDENTIAL,
        // when we arrive to the end element </Credential>
        case KStateParsingSingleCredential:
            {

            if (aLocalName == KCredentialElementLocalNameName ||
                aLocalName == KCredentialsName)

                {
                iState = KStateIgnore;
                }
            break;
            }
        default:
            break;
        }
        CSenBaseFragment::EndElementL(aNsUri, aLocalName, aQName);
    }

// End of File

