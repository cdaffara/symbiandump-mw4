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

#ifndef __CUPNPRESPONSECOMPOSER_H_
#define __CUPNPRESPONSECOMPOSER_H_

// System includes
#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/thttphdrfielditer.h>
#include <comms-infras/commsdebugutility.h>
#include <http/thttptable.h>
#include "cupnpmessagecomposer.h"

// Local includes
#include "mhttpmessagecomposerobserver.h"
#include "CResponse.h"
#include "rhttpmessagecomposer.h"
#include "mcomposerobserver.h"

class CUpnpResponseComposer: public CUPnPMessageComposer
	{
public:
	IMPORT_C static CUpnpResponseComposer* NewL(MComposerObserver& aObserver);
	IMPORT_C virtual ~CUpnpResponseComposer();
	IMPORT_C void ResponseDataSent();
	IMPORT_C void ResetComposer();

public:
	IMPORT_C void ComposeResponse(CResponse* aResponse);
	
private:
	// methods from MHttpMessageComposerObserver
	void StartLineL(TPtrC8& aVersion, TPtrC8& aStatusCode, TPtrC8& aStatusText);
	TInt NextHeaderL(TPtrC8& aHeaderName, TPtrC8& aHeaderValue);
	MHTTPDataSupplier* HasBodyL();
	inline TInt NextTrailerL(TPtrC8& aHeaderName, TPtrC8& aHeaderValue);
	void MessageComplete();
	void MessageDataReadyL();
	TInt HandleComposeError(TInt aError);
	
private:
	CUpnpResponseComposer(MComposerObserver& aObserver);
	void ConstructL();
	
private:
	CResponse* 				iResponse;
	HBufC8*					iStatusCodeBuf;

	__FLOG_DECLARATION_MEMBER;
	};
	
TInt CUpnpResponseComposer::NextTrailerL(TPtrC8& /*aHeaderName*/, TPtrC8& /*aHeaderValue*/)
	{
	// Response cannot contain trailer headers
	return KErrNotFound;
	}

#endif /*CUPNPRESPONSECOMPOSER_H_*/
