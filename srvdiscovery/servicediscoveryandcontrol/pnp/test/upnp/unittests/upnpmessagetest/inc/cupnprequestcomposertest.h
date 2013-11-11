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

#ifndef CUPNPREQUESTCOMPOSERTEST_H_
#define CUPNPREQUESTCOMPOSERTEST_H_

#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/framework/cheadercodec.h>
#include <es_mbuf.h>
#include <comms-infras/rworkerlock.h>
#include <comms-infras/mbufmanager.h>
#include <cflog.h>

#include "cupnpmessagetest.h"
#include "mcomposerobserver.h"
#include "cupnprequestcomposer.h"
#include "CRequest.h"

_LIT(KRequestComposerTest, "RequestComposerTest");

class CUpnpRequestComposerTest: public CTestStep,
							public MComposerObserver,
							public MHTTPDataSupplier
	{
public:	// factory methods
	static CUpnpRequestComposerTest* NewL();
	CUpnpRequestComposerTest();
	~CUpnpRequestComposerTest();

private:	// methods from MComposerObserver
	void ComposingConcluded();
	void MessageDataReadyL(RBuf8& aData);
	void ComposerError(TInt aError);

public:	// methods from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

public:	// methods from MHTTPDataSupplier
	TBool GetNextDataPart(TPtrC8& aDataPart);
	void ReleaseData();
	TInt OverallDataSize();
	TInt Reset();

private:
	void ConstructRequestMethodL(const TDesC& aRequestMethod);
	void ConstructRequestUriL(const TDesC& aRequestUri);
	void ConstructRequestHeadersL(const TInt aNoOfHdrValPairs, const TInt aNoOfHdrParamValPairs);
	void ConstructRequestBodyL(const TBool aRequestBodyPresence);
	void SetHeaderL(const TDesC8& aHeader, const TDesC8& aValue);
	void SetParamL(const TDesC8& aHeader, const TDesC8& aParam, const TDesC8& aValue, const TInt aPartIndex);

private:
	CUpnpRequestComposer*			iRequestComposer;
	HBufC8*							iRequestBody;
	CActiveScheduler* 				iSched;
	RBuf8							iRequestBuffer;
	CRequest*						iRequest;
	CHeaderCodec*					iCodec;
	RStringPool						iStringPool;
	CChunkManager* 					iMBufMgr;
	//MMBufSizeAllocator* 			iMBufSzAllctr;

#ifdef __CFLOG_ACTIVE
	__CFLOG_DECLARATION_MEMBER;
#endif
	};

#endif /*CUPNPREQUESTCOMPOSERTEST_H_*/
