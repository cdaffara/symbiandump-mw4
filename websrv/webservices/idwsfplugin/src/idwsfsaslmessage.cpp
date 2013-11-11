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
#include <SenXmlElement.h> // check if this include is needed(?)
#include <SenXmlUtils.h>

#include "idwsfsaslmessage.h"

#include "sendebug.h"
#include "senlogger.h"
#include "senpasswordtransform.h"

namespace
    {
    _LIT8(KContinue, "continue");
    _LIT8(KAbort, "abort");

    _LIT8(KPasswordTransformsName, "PasswordTransforms");
    _LIT8(KTransformName, "Transform");
    _LIT8(KParameterName, "Parameter");

    _LIT8(KDataName, "Data");
    _LIT8(KNameAttr, "name");
    _LIT8(KAuthzIdName, "authzID");
    _LIT8(KAdvisoryAuthnIdName, "advisoryAuthnID");
    _LIT8(KServerMechanismName, "serverMechanism");
    _LIT8(KMechanismName, "mechanism");
    }

CIdWsfSaslMessage* CIdWsfSaslMessage::NewL(
                                    MSenCoreServiceManager& aServiceManager)
    {
    CIdWsfSaslMessage* pNew = new (ELeave) CIdWsfSaslMessage(aServiceManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    CleanupStack::Pop();
    return pNew;
    }

CIdWsfSaslMessage* CIdWsfSaslMessage::NewL(
    MSenCoreServiceManager& aServiceManager,
    const TDesC8& aXmlns,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CIdWsfSaslMessage* pNew = new (ELeave) CIdWsfSaslMessage(aServiceManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aXmlns, aLocalName, aQName);
    CleanupStack::Pop();
    return pNew;
    }

CIdWsfSaslMessage::CIdWsfSaslMessage(MSenCoreServiceManager& aServiceManager)
:   iServiceManager(aServiceManager),
    ipMechanism(NULL),
    ipParameterName(NULL)
    {
    }

void CIdWsfSaslMessage::ConstructL()
    {
    BaseConstructL(KSaslXmlns, KSaslResponseName, KSaslResponseQName);
    }

void CIdWsfSaslMessage::ConstructL(
    const TDesC8& aXmlns,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    BaseConstructL(aXmlns, aLocalName, aQName);
    }

CIdWsfSaslMessage::~CIdWsfSaslMessage()
    {
    delete ipMechanism;
    delete ipParameterName;
    }

TInt CIdWsfSaslMessage::ConstructInitRequestFromL(
                                              const TDesC8& aAllMechanismNames,
                                              CSenIdentityProvider& aAccount)
    {
    // fresh start
    delete ipMechanism;
    ipMechanism=NULL;
    delete ipParameterName;
    ipParameterName=NULL;

    MSenElement& element = AsElement();
    element.AddAttrL(KMechanismName, aAllMechanismNames);

    TPtrC8 authzId = aAccount.AuthzID();
    if (authzId.Length() > 0)
        {
        element.AddAttrL(KAuthzIdName, authzId);
        }

    TPtrC8 advisoryAuthnId = aAccount.AdvisoryAuthnID();
    if (advisoryAuthnId.Length() > 0)
        {
        element.AddAttrL(KAdvisoryAuthnIdName, advisoryAuthnId);
        }
    return KErrNone;
    }

TInt CIdWsfSaslMessage::ConstructRequestFromL(
    const TDesC8& aMech)
    {
    // fresh start
    delete ipMechanism;
    ipMechanism=NULL;
    delete ipParameterName;
    ipParameterName=NULL;

    AsElement().AddAttrL(KMechanismName, aMech);
    return KErrNone;
    }

TInt CIdWsfSaslMessage::ConstructRequestFromL(
    const TDesC8& aMech,
    const TDesC8& aData
    )
    {
    // fresh start
    delete ipMechanism;
    ipMechanism=NULL;
    delete ipParameterName;
    ipParameterName=NULL;

    TInt retVal = ConstructRequestFromL(aMech);
    HBufC8* pDataBase64 = iServiceManager.EncodeToBase64LC(aData);

    AsElement().AddElementL(
        AsElement().NamespaceURI(), KDataName).SetContentL(*pDataBase64);

    CleanupStack::PopAndDestroy(); // delete pDataBase64;
    return retVal;
    }

TBool CIdWsfSaslMessage::IsContinue()
    {
    if(!ipStatus)
        {
        return EFalse;
        }
    else
        {
        return SenXmlUtils::EndsWith(*ipStatus, KContinue);
        }
    }

TBool CIdWsfSaslMessage::IsAbort()
    {
    if (ipStatus == NULL)
        {
        return EFalse;
        }
    else
        {
        return SenXmlUtils::EndsWith(*ipStatus, KAbort);
        }
    }

TPtrC8 CIdWsfSaslMessage::Mechanism()
    {
    if (!ipMechanism)
        {
        return KNullDesC8();
        }
    else
        {
        return *ipMechanism;
        }
    }

TPtrC8 CIdWsfSaslMessage::ParameterName()
    {
    if (!ipParameterName)
        {
        return KNullDesC8();
        }
    else
        {
        return *ipParameterName;
        }
    }

TPtrC8 CIdWsfSaslMessage::Data()
    {
    MSenElement* pDataElement = AsElement().Element(KDataName);
    if(pDataElement)
        {
        return pDataElement->Content();
        }
    return KNullDesC8();
    }

// @return NULL if list is empty
CSenPasswordTransform* CIdWsfSaslMessage::LastParsedPasswordTransform()
    {
    TInt count(iTransforms.Count());
    if(count>0)
        {
        return iTransforms[count-1];
        }
    else
        {
        return NULL;
        }
    }



void CIdWsfSaslMessage::StartElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes )
    {
    if (IsOk())
        {
        CIdWsfDsQueryResponse::StartElementL(aNsUri, aLocalName, aQName,
                                                aAttributes);
        }
    else
        {
        switch (iState)
            {
            case KStateIgnore:
                {
                if (aLocalName == KSaslResponseName)
                    {
                    HBufC8* pNew = SenXmlUtils::AllocAttrValueL(
                        aAttributes,
                        KServerMechanismName
                        );
                    delete ipMechanism;
                    ipMechanism = pNew;
                    }
                else if (aLocalName == KStatusName)
                    {
                    HBufC8* pNew = SenXmlUtils::AllocAttrValueL(
                        aAttributes,
                        KStatusCodeName
                        );
                    delete ipStatus;
                    ipStatus = pNew;
                    }
                else if (aLocalName == KDataName)
                    {
                    DelegateParsingL(aNsUri, aLocalName, aQName, aAttributes);
                    }
                else if (aLocalName == KPasswordTransformsName)
                    {
                    iTransforms.Reset(); // NOT owned..
                    iState = KStateParsingPwTransforms;
                    }
                break;
                }
            case KStateParsingPwTransforms:
                {
                if (aLocalName == KTransformName)
                    {

                    TPtrC8 name = SenXmlUtils::AttrValue(aAttributes, 
                                                            KNameAttr);
                    CSenPasswordTransform* pTransform =
                        CSenPasswordTransform::NewL(name, iServiceManager);

                    if(pTransform)
                        {
                        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,
                            _L8("CIdWsfSaslMessage::StartElementL - \
                            appending password transform: '%S'"), &name));
                        iTransforms.Append(pTransform);
                        }
                    }
                else if (aLocalName == KParameterName)
                    {
                    HBufC8* pNew = SenXmlUtils::AllocAttrValueL(
                        aAttributes,
                        KNameAttr
                        );
                    delete ipParameterName; // free memory
                    ipParameterName = pNew;
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,
                        _L("CIdWsfSaslMessage::StartElementL - \
                        parsing PasswordTransform parameter: '%S'"),
                            ipParameterName));
                    ResetContentL();
                    }
                break;
                }
            }
        }
    }

void CIdWsfSaslMessage::EndElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    if (IsOk())
        {
        CIdWsfDsQueryResponse::EndElementL(aNsUri, aLocalName, aQName);
        }
    else
        {
        switch (iState)
            {
            case KStateParsingPwTransforms:
                {
                if (aLocalName == KParameterName)
                    {
                    CSenPasswordTransform* pLast =
                            LastParsedPasswordTransform();
                    if(pLast)
                        {
                        pLast->SetParameterL(*ipParameterName, Content());
                        }
                    }
                else if (aLocalName == KPasswordTransformsName)
                    {
                    ResetContentL();
                    iState = KStateIgnore;
                    }
                break;
                }
            default:
                break;
            }
        }
    }

RPointerArray<CSenPasswordTransform> CIdWsfSaslMessage::Transforms()
    {
    return iTransforms;
    }


RFileLogger* CIdWsfSaslMessage::Log() const
    {
    return iServiceManager.Log();
    }

// End of File

