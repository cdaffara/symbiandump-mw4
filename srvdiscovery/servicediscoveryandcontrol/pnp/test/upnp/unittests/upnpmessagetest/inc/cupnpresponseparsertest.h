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

#ifndef CUPNPRESPONSEPARSERTEST_H_
#define CUPNPRESPONSEPARSERTEST_H_

#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/framework/cheadercodec.h>
//#include <es_mbuf.h>
#include <comms-infras/rworkerlock.h>
//#include <comms-infras/mbufmanager.h>
#include <cflog.h>
#include <rmemchunk.h>

#include "cupnpmessagetest.h"
#include "mparserobserver.h"
#include "cupnpresponseparser.h"
#include "CResponse.h"

_LIT(KResponseParserTest, "ResponseParserTest");

class CUpnpResponseParserTest: public CTestStep,
						public MParserObserver
	{
public:	// factory methods
	static CUpnpResponseParserTest* NewL();
	CUpnpResponseParserTest();
	~CUpnpResponseParserTest();

private:	// methods from MParserObserver
	void GotBodyData();
	void DataParsed();
	void GotHeaders();
	void ParsingComplete(RMemChunk& aExcessData);
	void ParserError(TInt aError);

public:	// methods from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void NegateEscapeEncodeL(const TDesC& aDes);

private:
	CUpnpResponseParser*				iResponseParser;
	MHTTPDataSupplier*					iDataSupplier;
	HBufC8*								iDataChunk;
	TBool								iNeedToParseExcessData;
	TPtrC8								iDataChunkPtr;
	CActiveScheduler* 					iSched;
	TInt								iExpectedResponses;
	TInt								iDataChunkCount;
	RPointerArray<HBufC8>				iDataChunkArray;
	CResponse*							iResponse;
	CHeaderCodec*						iCodec;
	RStringPool							iStringPool;
	CChunkManager* 						iMBufMgr;
	//MMBufSizeAllocator* 				iMBufSzAllctr;
	RMemChunk							iDataChunkChain;
	TBuf8<KMaxMsgLen>					iExcessData8;
	TBool								iIsStarted;
	TInt								iError;
	
#ifdef __CFLOG_ACTIVE
	__CFLOG_DECLARATION_MEMBER;
#endif

	};

#endif /*CUPNPRESPONSEPARSERTEST_H_*/
