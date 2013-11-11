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

#include <httpstringconstants.h>

#include "cupnprequestcomposer.h"
#include "tupnpmessagepanic.h"

__FLOG_STMT(_LIT8(KSubsys,"UPnPReqComposer");)
__FLOG_STMT(_LIT8(KComponent,"UPnPMessage");)


EXPORT_C CUpnpRequestComposer* CUpnpRequestComposer::NewL(MComposerObserver& aObserver)
	{
	CUpnpRequestComposer* self = new (ELeave) CUpnpRequestComposer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CUpnpRequestComposer::~CUpnpRequestComposer()
	{
	iMessageComposer.Close();
	__FLOG(_L8("-> Request composer destroyed"));
	__FLOG_CLOSE;
	}

CUpnpRequestComposer::CUpnpRequestComposer(MComposerObserver& aObserver)
	:CUPnPMessageComposer(aObserver)
	{
	}

void CUpnpRequestComposer::ConstructL()
	{
	CUPnPMessageComposer::ConstructL();
	__FLOG_OPEN(KSubsys, KComponent);
	__FLOG(_L8("-> Request Composer created"));
	}

void CUpnpRequestComposer::StartLineL(TPtrC8& aMethod, TPtrC8& aRequestUri, TPtrC8& aVersion)
	{
	__FLOG(_L8("-> Composing request"));
	__FLOG(_L8("-> Supplying start line"));

	aMethod.Set(iRequest->StringPool().StringF(iRequest->Method()).DesC());
	aRequestUri.Set(iRequest->URI()->Uri().UriDes());
	aVersion.Set(iRequest->StringPool().StringF(HTTP::EHttp11, THTTPTable::Table()).DesC());
	__FLOG_3(_L8("%S %S %S"), &aMethod, &aRequestUri, &aVersion);
	}

// This function should be moved to CUPnPMessageComposer once the CRequest, CResponse & CMessage
// split happens.
TInt CUpnpRequestComposer::NextHeaderL(TPtrC8& aHeaderName, TPtrC8& aHeaderValue)
	{
	// Are there any more headers?
	TInt err = KErrNotFound;
	
	THTTPHdrFieldIter fields = iRequest->Handle().GetHeaderCollection().Fields();
	TInt fieldPos = 0;
	
	while(fieldPos != iFieldIterPos)
		{
		++fields;
		fieldPos++;
		}
	
	if( !fields.AtEnd() )
		{
		// Get field current field.
		RStringF name = iRequest->StringPool().StringF(fields());

		err = KErrNone;
				
		// Get the OTA version of the field value
		TPtrC8 value;
		iRequest->Handle().GetHeaderCollection().GetRawField(name, value);
			
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

MHTTPDataSupplier* CUpnpRequestComposer::HasBodyL()
	{
	return iRequest->Handle().Body();
	}

void CUpnpRequestComposer::MessageComplete()
	{
	CUPnPMessageComposer::MessageComplete();
	}

void CUpnpRequestComposer::MessageDataReadyL()
	{
	CUPnPMessageComposer::MessageDataReadyL();	
	}

TInt CUpnpRequestComposer::HandleComposeError(TInt aError)
	{
	return CUPnPMessageComposer::HandleComposeError(aError);
	}


EXPORT_C void CUpnpRequestComposer::ComposeRequest(CRequest* aRequest)
	{
	__ASSERT_DEBUG(aRequest, TUPnPMessagePanic::Panic(TUPnPMessagePanic::EMissingRequest));
	iRequest = aRequest;

	// Notify the Message Composer that the request composing can start.
	//iMessageComposer.MessageInfoAvailable();
	NotifyNewBodyData ( );
	}

EXPORT_C void CUpnpRequestComposer::RequestDataSent()
	{
	// Notify the Message Composed that the current data has been read and it can
	// release it.
	__FLOG(_L8("-> Releasing message data"));
	iDataChain.Close(); //Need to check
	iMessageComposer.ReleaseMessageData();		
	}

EXPORT_C void CUpnpRequestComposer::ResetComposer()
	{
	__FLOG(_L8("-> Request composer reset"));
	iMessageComposer.Reset();
	iRequest = NULL;
	iFieldIterPos = 0;
	iDataChain.Close();
	}
