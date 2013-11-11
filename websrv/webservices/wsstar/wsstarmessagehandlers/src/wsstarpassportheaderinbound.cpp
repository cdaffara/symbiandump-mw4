/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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




//Co-Branding CR
#include "wsstarpassportheaderinbound.h"
#include "senlogger.h"
#include "SenXmlUtils.h"
#include "SenBaseElement.h"


CWSStarPassportHeaderInbound* CWSStarPassportHeaderInbound::NewL()
    {
    CWSStarPassportHeaderInbound* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CWSStarPassportHeaderInbound* CWSStarPassportHeaderInbound::NewLC()
    {
    CWSStarPassportHeaderInbound* self = new (ELeave) CWSStarPassportHeaderInbound();
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
    }


void CWSStarPassportHeaderInbound::StartElementL(const TDesC8& aNsUri,
                               const TDesC8& aLocalName,
                               const TDesC8& aQName,
                               const RAttributeArray& aAttributes)
    {
    switch (iState)
        {
        case KStateIgnore:
            {
            if (aNsUri == WSPassport32::KPassportFaultNamespaceXmlns)
                {
                iState = KStateSave;
                if  (aLocalName == WSPassport32::KCredProperty )
                    {
                    TPtrC8 value =
                        SenXmlUtils::AttrValue(aAttributes, WSPassport32::KName);
                    if(value == WSPassport32::KMainBrandID)
                        {
                            iState = KStateParsingMainBrandID;
                        }
                    
                    }
                 else if  (aLocalName == WSPassport32::KCredProperties )
                    {
                    iState = KStateParsingBrandIDList;
                    if(ipBrandIdListElem)
                        {
                            delete ipBrandIdListElem;
                            ipBrandIdListElem = NULL;
                        }  
                    ipBrandIdListElem = CSenBaseElement::NewL(aNsUri,aLocalName,aQName,aAttributes);
                  	}
                break;
                }
            }
        default:
            break;
        }
    }                       
    
void CWSStarPassportHeaderInbound::EndElementL(const TDesC8& aNsUri,
                             const TDesC8& aLocalName,
                             const TDesC8& /*aQName*/)
    {
    TPtrC8 content = Content();
    switch (iState)
        {
        case KStateSave:
            {
            ResetContentL();
            iState = KStateIgnore;
            break;
            }
        case KStateParsingMainBrandID:
            {
            if (aNsUri == WSPassport32::KPassportFaultNamespaceXmlns)
                {
                if (aLocalName == WSPassport32::KCredProperty)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KMainBrandID,content);
                    ResetContentL();
                    iState = KStateParsingBrandIDList;
                    }
                }
            break;
            }            
        case KStateParsingBrandIDList:
            {
            if (aNsUri == WSPassport32::KPassportFaultNamespaceXmlns)
                {
                if (aLocalName == WSPassport32::KCredProperties)
                    {
                    ipBrandIdListElem->SetContentL(content);
                    HBufC8* xmlSnippet = ipBrandIdListElem->AsXmlL();
                    CleanupStack::PushL(xmlSnippet);
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KBrandIDList,xmlSnippet);
                    CleanupStack::PopAndDestroy(xmlSnippet);
                    
                    ResetContentL();
                    iState = KStateIgnore;
                    }
                }
            break;
            }            
        default:
            break;
        }
    }


// Constructor
CWSStarPassportHeaderInbound::CWSStarPassportHeaderInbound()
    {
    }

// Destructor
CWSStarPassportHeaderInbound::~CWSStarPassportHeaderInbound()
    {
    if(ipBrandIdListElem)
        {
            delete ipBrandIdListElem;
            ipBrandIdListElem = NULL;
        }      
    }

// Second phase construction.
void CWSStarPassportHeaderInbound::ConstructL()
    {
    CSenBaseFragment::BaseConstructL(TPtrC8(NULL,0),
            KSenSoapEnvelopeHeaderName);
    }


TInt CWSStarPassportHeaderInbound::SetMessageContext(MSenMessageContext& aCtx)
    {
    iMessageContext = &aCtx;
    return KErrNone;
    }

// END OF FILE

