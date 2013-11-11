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

#ifndef CUPNPRESPONSECOMPOSERTEST_H_
#define CUPNPRESPONSECOMPOSERTEST_H_

#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/framework/cheadercodec.h>
#include <es_mbuf.h>
#include <comms-infras/rworkerlock.h>
#include <comms-infras/mbufmanager.h>
#include <cflog.h>

#include "cupnpmessagetest.h"
#include "mcomposerobserver.h"
#include "cupnpresponsecomposer.h"
#include "CResponse.h"

_LIT(KResponseComposerTest, "ResponseComposerTest");

class CUpnpResponseComposerTest: public CTestStep,
							public MComposerObserver,
							public MHTTPDataSupplier
	{
public:	// factory methods
	static CUpnpResponseComposerTest* NewL();
	CUpnpResponseComposerTest();
	~CUpnpResponseComposerTest();

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
	void ConstructResponseStatusLineL(const TInt aStatusCode, const TDesC& aStatusText);
	void ConstructResponseHeadersL(const TInt aNoOfHdrValPairs, const TInt aNoOfHdrParamValPairs);
	void ConstructResponseBodyL(const TBool aResponseBodyPresence);
	void SetHeaderL(const TDesC8& aHeader, const TDesC8& aValue);
	void SetParamL(const TDesC8& aHeader, const TDesC8& aParam, const TDesC8& aValue, const TInt aPartIndex);

private:
	CUpnpResponseComposer*			iResponseComposer;
	HBufC8*							iResponseBody;
	CActiveScheduler* 				iSched;
	RBuf8							iResponseBuffer;
	CResponse*						iResponse;
	CHeaderCodec*					iCodec;
	RStringPool						iStringPool;
	CChunkManager* 					iMBufMgr;
	//MMBufSizeAllocator* 			iMBufSzAllctr;
	
#ifdef __CFLOG_ACTIVE
	__CFLOG_DECLARATION_MEMBER;
#endif
	
	};

#endif /*CUPNPRESPONSECOMPOSERTEST_H_*/
