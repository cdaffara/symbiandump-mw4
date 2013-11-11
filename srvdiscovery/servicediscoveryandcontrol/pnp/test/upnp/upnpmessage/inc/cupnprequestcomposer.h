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

#ifndef __CUPNPREQUESTCOMPOSER_H_
#define __CUPNPREQUESTCOMPOSER_H_

// System includes
#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/thttphdrfielditer.h>
#include <comms-infras/commsdebugutility.h>
#include <http/thttptable.h>

#include "cupnpmessagecomposer.h"
// Local includes
#include "CRequest.h"
#include "mhttpmessagecomposerobserver.h"
#include "rhttpmessagecomposer.h"
#include "mcomposerobserver.h"

class CUpnpRequestComposer: public CUPnPMessageComposer
	{
public:
	IMPORT_C static CUpnpRequestComposer* NewL(MComposerObserver& aObserver);
	IMPORT_C virtual ~CUpnpRequestComposer();
	IMPORT_C void RequestDataSent();
	IMPORT_C void ResetComposer();
	
public:
	IMPORT_C void ComposeRequest(CRequest* aRequest);
	inline CRequest& Request();
	
private:
	// methods from MHttpMessageComposerObserver
	void StartLineL(TPtrC8& aMethod, TPtrC8& aRequestUri, TPtrC8& aVersion);
	TInt NextHeaderL(TPtrC8& aHeaderName, TPtrC8& aHeaderValue);
	MHTTPDataSupplier* HasBodyL();
	inline TInt NextTrailerL(TPtrC8& aHeaderName, TPtrC8& aHeaderValue);
	void MessageComplete();
	void MessageDataReadyL();
	TInt HandleComposeError(TInt aError);
	
private:
	CUpnpRequestComposer(MComposerObserver& aObserver);
	void ConstructL();
	
private:
	CRequest*				iRequest;
	
	__FLOG_DECLARATION_MEMBER;
	};

CRequest& CUpnpRequestComposer::Request()
	{
	return (*iRequest);
	}

TInt CUpnpRequestComposer::NextTrailerL(TPtrC8& /*aHeaderName*/, TPtrC8& /*aHeaderValue*/)
	{
	// We do not set any trailer headers in the request.
	return KErrNotFound;
	}

#endif /*CUPNPREQUESTCOMPOSER_H_*/
