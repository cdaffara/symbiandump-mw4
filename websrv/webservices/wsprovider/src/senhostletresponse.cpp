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











#include "senhostletresponse.h"

#include <SenHttpTransportProperties.h>
#include "senvtcptransportproperties.h"
#include <SenParser.h>

#include <MSenProperties.h>
#include <MSenMessage.h>
#include "senmessagecontext.h"
#include "SenSoapMessage2.h"

//using namespace SenContext;

EXPORT_C CSenHostletResponse* CSenHostletResponse::NewL(const TInt aRequestId)
    {
    CSenHostletResponse* pNew = CSenHostletResponse::NewLC(aRequestId);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenHostletResponse* CSenHostletResponse::NewLC(const TInt aRequestId)
    {
    CSenHostletResponse* pNew = new(ELeave) CSenHostletResponse(aRequestId);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

void CSenHostletResponse::BaseConstructL()
    {
    }

CSenHostletResponse::CSenHostletResponse(const TInt aRequestId)
: iRequestId(aRequestId),
  iResponseCode(KErrNone),
  ipOwnedResponseUtf8(NULL),
  ipMessageContext(NULL)
    {
    }
    
EXPORT_C CSenHostletResponse::~CSenHostletResponse()
    {
    delete ipOwnedResponseUtf8;
    ipOwnedResponseUtf8 = NULL;
    delete ipMessageContext;
    ipMessageContext = NULL;
    }

EXPORT_C void CSenHostletResponse::SetResponseUtf8L(const TDesC8& aResponseBodyUtf8,
                                                    const TInt aResponseCode,
                                                    MSenProperties* apResponseProperties,
                                                    MSenMessage* apMessage)
    {
    HBufC8* pTemp = aResponseBodyUtf8.AllocL();
    iResponseCode = aResponseCode;
    delete ipOwnedResponseUtf8;
    ipOwnedResponseUtf8 = NULL;
    ipOwnedResponseUtf8 = pTemp;
    

    TInt error(KErrNone);
    MSenProperties* p = NULL;
    if( apResponseProperties && apResponseProperties->PropertiesClassType() != MSenProperties::ENotInUse )
        {
        p = apResponseProperties->Clone(error);
        }

    if( p )
        {
        // Create new message context
        delete ipMessageContext;  
        ipMessageContext = NULL;
        MessageContextL( p );
        }
    else
        {
        MessageContextL();
        }
        
    MSenMessageContext& ctx = MessageContextL();
    MSenMessage* pMessage = NULL;
    if( !apMessage ) 
        {
        pMessage = CSenSoapMessage2::NewL( ctx ); // ctx holds the transport properties, if such exist
        ctx.SetMessage(pMessage, ETrue);
        }
    else
        {
        pMessage = apMessage->CloneL();
        ctx.SetMessage(pMessage, ETrue);
        }        
    
    // Set the message into context, with possible properties..
    error = 0; // not used in release builds
    
/*
    MSenProperties* pResponseProperties = NULL;
    
    if( apResponseProperties && apResponseProperties->PropertiesClassType() != MSenProperties::ENotInUse )
        {
        switch(apResponseProperties->PropertiesClassType())
            {
            case MSenProperties::ENotInUse:
                {
                break;
                }
                
            
            case MSenProperties::ESenXmlProperties:
            case MSenProperties::ESenLayeredXmlProperties: // flatten the props
                {
                pResponseProperties = CSenXmlProperties::NewL(); // iSerializedMessage, *pReader);
                break;
                }
            case MSenProperties::ESenTransportProperties:
            case MSenProperties::ESenLayeredTransportProperties: // flatten the props
                {
                pResponseProperties = CSenTransportProperties::NewL();
                break;
                }
            case MSenProperties::ESenHttpTransportProperties:
            case MSenProperties::ESenLayeredHttpTransportProperties: // flatten the props
                {
                pResponseProperties = CSenHttpTransportProperties::NewL();
                break;
                }
            case MSenProperties::ESenVtcpTransportProperties:
            case MSenProperties::ESenLayeredVtcpTransportProperties: // flatten the props
                {
                pResponseProperties = CSenVtcpTransportProperties::NewL();
                break;
                }
            default: 
                {
                break;
                }
            }
            
        if( ipResponseProperties )  
            {
            HBufC8* pSerializedProperties = apResponseProperties->AsUtf8LC();
            if (pSerializedProperties && pSerializedProperties->Length()>0)
                {
                TRAPD( err, ipResponseProperties->ReadFromL(*pSerializedProperties); ) // suppress any parsing leave
                TInt unused(err);
                err = unused;
                }
            CleanupStack::PopAndDestroy(pSerializedProperties);
            }
        }
*/
    }


EXPORT_C MSenProperties* CSenHostletResponse::ResponseProperties()
    {
    TInt leaveCode(KErrNone);
    MSenMessageContext* pCtx = NULL;
    MSenMessage* pMessage = NULL;
    TRAP( leaveCode, pCtx = &(MessageContextL()); )
    if( !leaveCode && pCtx )
        {
        TRAP( leaveCode, pMessage = pCtx->MessageL(); )
        //pMessage = pCtx->Message();
        if( !leaveCode && pMessage )
            {
            return pMessage->Properties();
            }
        }
    return NULL; // not found
    }
    
EXPORT_C MSenMessage* CSenHostletResponse::ResponseMessageL()
    {
    TInt leaveCode(KErrNone);
    MSenMessage* pMsg = NULL;
    TRAP( leaveCode,  pMsg = MessageContextL().MessageL(); )
    if( leaveCode == KErrNone )
        {
        return pMsg;
        }
    leaveCode = 0; // ignored
    return NULL;        
    }
    

EXPORT_C HBufC8* CSenHostletResponse::ExtractResponseUtf8()
    {
    HBufC8* pResult = ipOwnedResponseUtf8;
    ipOwnedResponseUtf8 = NULL;
    return pResult;
    }


EXPORT_C TPtrC8 CSenHostletResponse::ResponseUtf8()
    {
    if(ipOwnedResponseUtf8)
        {
        return *ipOwnedResponseUtf8;
        }
    else
        {
        return KNullDesC8();
        }
    }

EXPORT_C TInt CSenHostletResponse::RequestId() const
    {
    return iRequestId;
    }

EXPORT_C TInt CSenHostletResponse::ResponseCode() const
    {
    return iResponseCode;
    }


MSenMessageContext& CSenHostletResponse::MessageContextL(MSenProperties* apProperties)
    {
    if( !ipMessageContext )
        {
        if( apProperties )
            ipMessageContext = CSenMessageContext::NewL(SenContext::EOutgoing, apProperties);
        else
            {
            CSenXmlProperties* p = CSenXmlProperties::NewL();
            CleanupStack::PushL(p);
            ipMessageContext = CSenMessageContext::NewL(SenContext::EOutgoing, p);
            CleanupStack::Pop(p);
            }
        }
    return *ipMessageContext;
    }
    
    
// END OF FILE



