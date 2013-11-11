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

#include "cupnpmessagecomposer.h"
#include <http/thttphdrfielditer.h>
#include "mcomposerobserver.h"

__FLOG_STMT(_LIT8(KSubsys,"UPnPComposer");)
__FLOG_STMT(_LIT8(KComponent,"UPnPMessage");)

CUPnPMessageComposer ::~CUPnPMessageComposer ()
	{
	Cancel ();
	iMessageComposer.Close ();
	iDataChain.Close();
	__FLOG(_L8("-> Request composer destroyed"));
	__FLOG_CLOSE;		
	}
CUPnPMessageComposer::CUPnPMessageComposer (MComposerObserver& aObserver)
: CActive (EPriorityNormal),
iObserver ( aObserver )
	{
	CActiveScheduler::Add(this);
	}

void CUPnPMessageComposer::ConstructL ()
	{
	iMessageComposer.OpenL(*this);
	__FLOG_OPEN(KSubsys, KComponent);
	__FLOG(_L8("-> Response composer created"));		
	}

EXPORT_C void CUPnPMessageComposer::NotifyNewBodyData()
	{
	if ( !IsActive () )
		CompleteSelf();
	else
		return;
	}

// From MHttpMessageComposerObserver
void CUPnPMessageComposer::MessageComplete()
	{
	__FLOG(_L8("-> Message complete\n"));
	iFieldIterPos = 0;
	iObserver.ComposingConcluded();
	}

void CUPnPMessageComposer::MessageDataReadyL()
	{
	__FLOG(_L8("-> Got composed request"));
	TPtrC8 dataPtr;
	iMessageComposer.GetMessageData(dataPtr);

	__FLOG_1(_L8("%S"), &dataPtr);
	
	// Notify the observer that there is message data ready to send.
	iDataChain.Close();
	iDataChain.CreateL(dataPtr);
	iObserver.MessageDataReadyL(iDataChain);
	}

TInt CUPnPMessageComposer::HandleComposeError(TInt aError)
	{
	// The Message Composer has found a problem. Report to error to the observer.
	__FLOG_1(_L8("-> HTTP message composer received error: %d"), aError);
	iFieldIterPos = 0;
	iObserver.ComposerError(aError);
	return KErrNone;
	}

//void CUPnPMessageComposer::StartLineL(TPtrC8& /*aVersion*/, TPtrC8& /*aStatusCode*/, TPtrC8& /*aStatusText*/)
//	{
//	ASSERT(0);		
//	}

//TInt CUPnPMessageComposer::NextHeaderL(TPtrC8& /*aHeaderName*/, TPtrC8& /*aHeaderValue*/)
//	{
//	ASSERT(0);	
//	return KErrNone;	
//	}

//MHTTPDataSupplier* CUPnPMessageComposer::HasBodyL()	
//	{
//	ASSERT(0);	
//	return NULL;	
//	}

//TInt CUPnPMessageComposer::NextTrailerL(TPtrC8& /*aHeaderName*/, TPtrC8& /*aHeaderValue*/)
//	{
//	ASSERT(0);	
//	return 0;	
//	}

// From CActive
void CUPnPMessageComposer::RunL ()
	{
	__FLOG_1(_L8("-> HTTP message composer RunL called: %d"), iStatus.Int());
	User::LeaveIfError (iStatus.Int());
	iMessageComposer.MessageInfoAvailable ();		
	}

TInt CUPnPMessageComposer::RunError ()
	{
	__FLOG_0(_L8("-> Composer called RunError"));
	iFieldIterPos = 0;
	iObserver.ComposerError(KErrUnknown);
	return KErrNone;		
	}

void CUPnPMessageComposer::CompleteSelf ()
	{
	TRequestStatus* pStat = &iStatus;
	User::RequestComplete(pStat, KErrNone);
	SetActive();		
	}
