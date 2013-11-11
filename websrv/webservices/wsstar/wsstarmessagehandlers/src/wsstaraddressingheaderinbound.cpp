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








#include "wsstaraddressingheaderinbound.h"
#include "senlogger.h"

using namespace WSStarAddressing;

CWSStarAddressingHeaderInbound* CWSStarAddressingHeaderInbound::NewL(const TDesC8* aVersion)
    {
    CWSStarAddressingHeaderInbound* self = NewLC(aVersion);
    CleanupStack::Pop(self);
    return self;
    }

CWSStarAddressingHeaderInbound* CWSStarAddressingHeaderInbound::NewLC(const TDesC8* aVersion)
    {
    CWSStarAddressingHeaderInbound* self = new (ELeave) CWSStarAddressingHeaderInbound();
    CleanupStack::PushL (self);
    self->ConstructL(aVersion);
    return self;
    }


void CWSStarAddressingHeaderInbound::StartElementL(const TDesC8& aNsUri,
                               const TDesC8& aLocalName,
                               const TDesC8& /*aQName*/,
                               const RAttributeArray& /*aAttributes*/)
    {
    switch (iState)
        {
        case KStateIgnore:
            {
            if (!iVersion && (aNsUri == KAddressingXmlns200408 ||
                aNsUri == KAddressingXmlns200403 ||
                aNsUri == KAddressingXmlns200412 ||
                aNsUri == KAddressingXmlns200502 ||
                aNsUri == KAddressingXmlns200503 ||
                aNsUri == KAddressingXmlns200508))
                {
                delete iVersionBuf;
                iVersionBuf = NULL;
                iVersionBuf = aNsUri.AllocL();
                iVersion = iVersionBuf;
                }
            if (iVersion && (aNsUri == *iVersion))
                {
                iState = KStateSave;
                if  (aLocalName == KReplyToName )
                    {
                    iState = KStateParsingReplyTo;
                    }
                else if  (aLocalName == KFaultToName )
                    {
                    iState = KStateParsingFaultTo;
                    }
                else if  (aLocalName == KFromName )
                    {
                    iState = KStateParsingFrom;
                    }
                break;
                }
            if (aLocalName == WSStarAddressingEPR::KDeviceAddressName)
                {
                iState = KStateSave;
                }
            }
        default:
            break;
        }
    }                       
    
void CWSStarAddressingHeaderInbound::EndElementL(const TDesC8& aNsUri,
                             const TDesC8& aLocalName,
                             const TDesC8& /*aQName*/)
    {
    TPtrC8 content = Content();
    TUriParser8 uriParser;
    TInt error = uriParser.Parse(content);
    if (error != KErrNone)
        {
        TLSLOG_FORMAT((KSenFaultsLogChannel, KSenFaultsLogLevel, 
                        WSStarAddressingFault::KInvalidHeaderFormatString8, 
                        &aLocalName,&WSStarAddressingFault::KInvalidUri()));   
        }
        
    switch (iState)
        {
        case KStateSave:
            {
            if (aNsUri == *iVersion)
                {
                if (aLocalName == KMessageIDName)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KMessageID, 
                                                            content);
                    }
                else if (aLocalName == KRelatesToName)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KRelatesTo, 
                                                            content);
                    }
                else if (aLocalName == KToName)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KTo, content);
                    }
                else if (aLocalName == KActionName)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KAction, content);
                    }
                }
            if (aLocalName == WSStarAddressingEPR::KDeviceAddressName)
                {
                ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KReplyToDeviceAddress,
                                                        content);
                iDeviceAddress = ETrue;                                                            
                }
            ResetContentL();
            iState = KStateIgnore;
            break;
            }
        case KStateParsingReplyTo:
            {
            if (aNsUri == *iVersion )
                {
                if (aLocalName == KAddressName)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KReplyTo,content);
                    }
                ResetContentL();
                break;
                }
            if (aLocalName == WSStarAddressingEPR::KDeviceAddressName)
                {
                if (!iDeviceAddress)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KReplyToDeviceAddress,
                                                            content);
                    }
                }

            ResetContentL();
            iState = KStateIgnore;
            break;
            }            
        case KStateParsingFaultTo:
            {
            if (aNsUri == *iVersion )
                {
                if (aLocalName == KAddressName)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KFaultTo,content);
                    ResetContentL();
                    iState = KStateIgnore;
                    }
                }
            break;
            }            
        case KStateParsingFrom:
            {
            if (aNsUri == *iVersion )
                {
                if (aLocalName == KAddressName)
                    {
                    ((MSenContext*)iMessageContext)->Update(WSStarContextKeys::KFrom,content);
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
CWSStarAddressingHeaderInbound::CWSStarAddressingHeaderInbound()
    {
    }

// Destructor
CWSStarAddressingHeaderInbound::~CWSStarAddressingHeaderInbound()
    {
    delete iVersionBuf;
    }

// Second phase construction.
void CWSStarAddressingHeaderInbound::ConstructL(const TDesC8* aVersion)
    {
    if (aVersion)
        {
        CSenBaseFragment::BaseConstructL(*aVersion,
                KSenSoapEnvelopeHeaderName);
        }
    else
        {
        CSenBaseFragment::BaseConstructL(TPtrC8(NULL,0),
                KSenSoapEnvelopeHeaderName);
        }
    iVersion = aVersion;
    iDeviceAddress = EFalse;
    }


TInt CWSStarAddressingHeaderInbound::SetMessageContext(MSenMessageContext& aCtx)
    {
    iMessageContext = &aCtx;
    return KErrNone;
    }

const TDesC8* CWSStarAddressingHeaderInbound::Version() const
    {
    return iVersion;
    }

// END OF FILE

