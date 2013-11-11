// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <http/framework/cheadercodecplugin.h>
#include <httpstringconstants.h>
#include <inetprottextutils.h>

#include "cupnpresponsecomposer.h"
#include "tupnpmessagepanic.h"

__FLOG_STMT(_LIT8(KSubsys,"UPnPResComposer");)
__FLOG_STMT(_LIT8(KComponent,"UPnPMessage");)


EXPORT_C CUpnpResponseComposer* CUpnpResponseComposer::NewL(MComposerObserver& aObserver)
	{
	CUpnpResponseComposer* self = new (ELeave) CUpnpResponseComposer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CUpnpResponseComposer::~CUpnpResponseComposer()
	{
	iMessageComposer.Close();
	delete iStatusCodeBuf;
	__FLOG(_L8("-> Response composer destroyed"));
	__FLOG_CLOSE;
	}

CUpnpResponseComposer::CUpnpResponseComposer(MComposerObserver& aObserver)
	:CUPnPMessageComposer(aObserver)
	{
	}

void CUpnpResponseComposer::ConstructL()
	{
	CUPnPMessageComposer::ConstructL();
	__FLOG_OPEN(KSubsys, KComponent);
	__FLOG(_L8("-> Response Composer created"));
	}

void CUpnpResponseComposer::StartLineL(TPtrC8& aVersion, TPtrC8& aStatusCode, TPtrC8& aStatusText)
	{
	__FLOG(_L8("-> Composing response"));
	__FLOG(_L8("-> Supplying start line"));

	TInt statusCode = iResponse->Status();
	InetProtTextUtils::ConvertIntToDescriptorL(statusCode, iStatusCodeBuf);
	
	aStatusCode.Set(iStatusCodeBuf->Des());
	aStatusText.Set(iResponse->StatusString().DesC());
	aVersion.Set(iResponse->StringPool().StringF(HTTP::EHttp11, THTTPTable::Table()).DesC());
	__FLOG_3(_L8("%S %S %S"), &aVersion, &aStatusCode, &aStatusText);
	}
	
// This function should be moved to CUPnPMessageComposer once the CRequest, CResponse & CMessage
// split happens.
TInt CUpnpResponseComposer::NextHeaderL(TPtrC8& aHeaderName, TPtrC8& aHeaderValue)
	{
	// Are there any more headers?
	TInt err = KErrNotFound;

	THTTPHdrFieldIter fields = iResponse->Handle().GetHeaderCollection().Fields();
	TInt fieldPos = 0;
	
	while(fieldPos != iFieldIterPos)
		{
		++fields;
		fieldPos++;
		}
	
	if( !fields.AtEnd() )
		{
		// Get field current field.
		RStringF name = iResponse->StringPool().StringF(fields());

		err = KErrNone;
			
		// Get the OTA version of the field value
		TPtrC8 value;
		iResponse->Handle().GetHeaderCollection().GetRawField(name, value);
		
		// Pass back these values
		aHeaderName.Set(name.DesC());
		aHeaderValue.Set(value);
		__FLOG(_L8("-> Supplying header value pair"));
		__FLOG_2(_L8("%S: %S"), &aHeaderName, &aHeaderValue);		
		
		// Move onto next header field...
		++fields;
		++iFieldIterPos;
		}
	return err;
	}

MHTTPDataSupplier* CUpnpResponseComposer::HasBodyL()
	{
	return iResponse->Handle().Body();
	}

void CUpnpResponseComposer::MessageComplete()
	{
	CUPnPMessageComposer::MessageComplete();
	}

void CUpnpResponseComposer::MessageDataReadyL()
	{
	CUPnPMessageComposer::MessageDataReadyL();	
	}

TInt CUpnpResponseComposer::HandleComposeError(TInt aError)
	{
	return CUPnPMessageComposer::HandleComposeError(aError);
	}

EXPORT_C void CUpnpResponseComposer::ComposeResponse(CResponse* aResponse)
	{
	__ASSERT_DEBUG(aResponse, TUPnPMessagePanic::Panic(TUPnPMessagePanic::EMissingResponse));
	iResponse = aResponse;

	// Notify the Message Composer that the response composing can start.
	NotifyNewBodyData ( );
	//iMessageComposer.MessageInfoAvailable();
	}

EXPORT_C void CUpnpResponseComposer::ResponseDataSent()
	{
	// Notify the Message Composed that the current data has been read and it can
	// release it.
	__FLOG(_L8("-> Releasing message data"));
	iDataChain.Close();	//Need to check
	iMessageComposer.ReleaseMessageData();	
	}

EXPORT_C void CUpnpResponseComposer::ResetComposer()
	{
	__FLOG(_L8("-> Response composer reset"));
	iMessageComposer.Reset();
	iFieldIterPos = 0;
	iDataChain.Close();	
	}
