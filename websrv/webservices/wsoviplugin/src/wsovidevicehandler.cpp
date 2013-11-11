/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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










#include "wsovidevicehandler.h"
#include "senlogger.h"
#include <SenTransportProperties.h>
using namespace OAuth;
// Create instance of concrete ECOM interface implementation
CWSOviDeviceHandler* CWSOviDeviceHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSOviDeviceHandler* self   = new (ELeave) CWSOviDeviceHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSOviDeviceHandler::CWSOviDeviceHandler(MSenHandlerContext& aCtx):CSenMessageHandler(aCtx)
    {
    }

// Destructor
CWSOviDeviceHandler::~CWSOviDeviceHandler()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviDeviceHandler::~CWSOviDeviceHandler()")));
    delete iNAuthVersion;
    }

// Second phase construction.
void CWSOviDeviceHandler::ConstructL()
    {
    }
    
TInt CWSOviDeviceHandler::InvokeL(MSenMessageContext& aCtx)
    {
        
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviDeviceHandler::InvokeL(MSenMessageContext& aCtx)")));
        CSenTransportProperties* tp = ((CWSOviMessageContext&)aCtx).TP();
        if (!tp) return KErrNotFound;
        TPtrC8 consumerKey;
        tp->PropertyL(KNCIMConsumerKeyId, consumerKey);		
        
        const TDesC8* nAuthVersion = iHandlerContext.GetDesC8L(HandlerContextKey::KVersion);
        
        if (!iNAuthVersion)
            {
            if ( !nAuthVersion || *nAuthVersion == KNullDesC8() )
                {
                iNAuthVersion = KParValVersion().AllocL();
                }
            else
                {
                iNAuthVersion = nAuthVersion->AllocL();
                }
            }
        
        HBufC8* value = HBufC8::NewLC(KParNokiaAuth().Length() +
        							  KSpace().Length() +
        							  
        							  KParRealm().Length() +
 				                      KEqual().Length() + KQtMark().Length() +
 				                      KParValRealm().Length() +
				                      KQtMark().Length() + KComma().Length() +
                                      
                                      KParVersion().Length() + 
                                      KEqual().Length() + 2*KQtMark().Length() + 
                                      iNAuthVersion->Length() + KComma().Length() +
                                        
                                      KParConsumerKey().Length() + 
                                      KEqual().Length() + 2*KQtMark().Length() + 
                                      consumerKey.Length()
                                        
                                        
                                      );
        
        
        //Authorization: OAuth,oauth_version="1.0"
        value->Des().Append(KParNokiaAuth());
        value->Des().Append(KSpace());
       
        value->Des().Append(KParRealm());
        value->Des().Append(KEqual());
        value->Des().Append(KQtMark());
        value->Des().Append(KParValRealm);
        value->Des().Append(KQtMark());
        value->Des().Append(KComma());

        value->Des().Append(KParConsumerKey());
        value->Des().Append(KEqual());
        value->Des().Append(KQtMark());
        value->Des().Append(consumerKey);
        value->Des().Append(KQtMark());
        value->Des().Append(KComma());
        
        
        value->Des().Append(KParVersion());
        value->Des().Append(KEqual());
        value->Des().Append(KQtMark());
        value->Des().Append(*iNAuthVersion);
        value->Des().Append(KQtMark());
     
        tp->SetPropertyL(KAuthHttpHeader, *value, KHttpHeaderType);
        CleanupStack::PopAndDestroy(value);

        RMapDescriptors& headerProperties = ((CWSOviMessageContext&)aCtx).OAuthProperies();
        HBufC8* newValue = NULL;
		newValue = iNAuthVersion->AllocL();
        headerProperties.Insert(&KParVersion, newValue);

        //msg context keeps same tp, so we dont have to update msg context
        return KErrNone;
    }
    
SenHandler::THandlerDirection CWSOviDeviceHandler::Direction() const
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviDeviceHandler::Direction()")));
    return SenHandler::EOutgoing;
    };
SenHandler::THandlerPhase CWSOviDeviceHandler::Phase()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviDeviceHandler::Phase()")));
    return SenHandler::EMessage;
    };    
    
TInt CWSOviDeviceHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
// END OF FILE

