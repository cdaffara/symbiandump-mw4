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


#ifndef __CUPNPMESSAGECOMPOSER_H
#define __CUPNPMESSAGECOMPOSER_H

#include <e32base.h>
#include "mhttpmessagecomposerobserver.h"
#include "rhttpmessagecomposer.h"
#include <comms-infras/commsdebugutility.h>
#include <rmemchunk.h>

class MComposerObserver;
class CUPnPMessageComposer : public CActive, 
							public MHttpMessageComposerObserver
	{	
	public:
	virtual ~CUPnPMessageComposer ();	
	IMPORT_C void NotifyNewBodyData();
	
	inline void DataSentFailed ();
	
	protected:
	CUPnPMessageComposer (MComposerObserver& aObserver);
	void ConstructL ();
	
	protected:
	// From CActive
	void RunL ();
	TInt RunError ();
	inline void DoCancel ();
	
	void CompleteSelf ();

	// methods from MHttpMessageComposerObserver
	//virtual void StartLineL(TPtrC8& aVersion, TPtrC8& aStatusCode, TPtrC8& aStatusText);
	//virtual TInt NextHeaderL(TPtrC8& aHeaderName, TPtrC8& aHeaderValue);
	//virtual MHTTPDataSupplier* HasBodyL();
	//virtual TInt NextTrailerL(TPtrC8& aHeaderName, TPtrC8& aHeaderValue);
	virtual void MessageComplete();
	virtual void MessageDataReadyL() ;
	virtual TInt HandleComposeError(TInt aError);
	
	protected:
	RHttpMessageComposer	iMessageComposer;
	TInt					iFieldIterPos;
	MComposerObserver& 		iObserver;			
	RBuf8				iDataChain;

	__FLOG_DECLARATION_MEMBER;
	};

void CUPnPMessageComposer::DoCancel ()
	{
	// Do nothing.	
	}
	
void CUPnPMessageComposer::DataSentFailed ()
	{
	iDataChain.Close();
	}

#endif // __CUPNPMESSAGECOMPOSER_H
