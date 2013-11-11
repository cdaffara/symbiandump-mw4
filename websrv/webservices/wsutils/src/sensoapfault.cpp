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
#include "SenSoapFault.h"
#include "SenSoapEnvelope.h"

EXPORT_C CSenSoapFault* CSenSoapFault::NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenSoapFault* pNew = new (ELeave) CSenSoapFault;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenSoapFault* CSenSoapFault::NewL(CSenElement& aCopiedSource)
    {
    CSenSoapFault* pNew = new (ELeave) CSenSoapFault;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aCopiedSource);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }


EXPORT_C CSenSoapFault::~CSenSoapFault()
    {
    }

EXPORT_C CSenSoapFault::CSenSoapFault()
    {
    }

EXPORT_C void CSenSoapFault::ConstructL(CSenElement& aCopiedSource)
    {
    BaseConstructL(aCopiedSource.LocalName());
    CopyFromL(aCopiedSource);
    }


EXPORT_C void CSenSoapFault::ConstructL(const TDesC8& aNsUri,
                             const TDesC8& aLocalName,
                             const TDesC8& aQName,
                             const RAttributeArray& aAttributes)
    {
    BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    }
EXPORT_C TPtrC8 CSenSoapFault::FaultCode()
    {
        if (NamespaceURI().Compare(KSenSoap12EnvelopeXmlns)!=0)
        {
                
            CSenElement* pChild = Element(KNullDesC8, KFaultCodeLocalname);
            if(pChild)
                {
                return pChild->Content();
                }
        }
        else
        {
            CSenElement* pChild = Element(KFault12CodeLocalname);
            if (pChild)
            {
                CSenElement* pValue = pChild->Element(KFault12ValueLocalname);
                if (pValue)
                {
                    return pValue->Content();
                }
            }
            
        }
        return KNullDesC8();
    }

EXPORT_C TPtrC8 CSenSoapFault::FaultSubcode()
    {
        if (NamespaceURI().Compare(KSenSoap12EnvelopeXmlns)==0)
        {
            CSenElement* pChild = Element(KFault12CodeLocalname);
            if (pChild)
            {
                CSenElement* pSubcode = pChild->Element(KFault12SubcodeLocalname);
                if (pSubcode)
                {
                    CSenElement* pValue = pSubcode->Element(KFault12ValueLocalname);
                    if (pValue)
                    {
                        return pValue->Content();
                    }
                }
            }
            
        }
        return KNullDesC8();
    }


EXPORT_C TPtrC8 CSenSoapFault::FaultString()
    {
        if (NamespaceURI().Compare(KSenSoap12EnvelopeXmlns)!=0)
        {
            CSenElement* pChild = Element(KNullDesC8, KFaultStringLocalname);
            if(pChild)
                {
                return pChild->Content();
                }
        }
        else
        {
            CSenElement* pChild = Element(KNullDesC8, KFault12ReasonLocalname);
            if (pChild)
            {
                CSenElement* pText = pChild->Element(KFault12TextLocalname);
                if (pText)
                {
                    return pText->Content();
                }
            }
        }
    return KNullDesC8();    
    }

EXPORT_C TPtrC8 CSenSoapFault::FaultActor()
    {
    CSenElement* pChild = Element(KNullDesC8, KFaultActorLocalname);
    if(pChild)
        {
        return pChild->Content();
        }
    return KNullDesC8();    
    }

EXPORT_C TPtrC8 CSenSoapFault::Detail()
    {
        if (NamespaceURI().Compare(KSenSoap12EnvelopeXmlns)!=0)
        {
            CSenElement* pChild = Element(KNullDesC8, KDetailLocalname);
            if(pChild)
                {
                return pChild->Content();
                }
        }
        else
        {
            CSenElement* pChild = Element(KFault12DetailLocalname);
            if (pChild)
            {
                    return pChild->Content();
            }
        }
    return KNullDesC8();    
    }

// End of File
