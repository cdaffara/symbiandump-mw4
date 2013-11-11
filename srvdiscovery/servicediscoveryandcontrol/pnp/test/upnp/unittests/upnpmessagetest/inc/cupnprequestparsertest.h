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

#ifndef CUPNPREQUESTPARSERTEST_H_
#define CUPNPREQUESTPARSERTEST_H_

#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/framework/cheadercodec.h>
//#include <es_mbuf.h>
#include <comms-infras/rworkerlock.h>
//#include <comms-infras/mbufmanager.h>
#include <cflog.h>
#include <rmemchunk.h>
#include <cchunkmanager.h>

#include "cupnpmessagetest.h"
#include "mparserobserver.h"
#include "cupnprequestparser.h"
#include "CResponse.h"

_LIT(KRequestParserTest, "RequestParserTest");

class CUpnpRequestParserTest: public CTestStep,
						public MParserObserver
	{
public:	// factory methods
	static CUpnpRequestParserTest* NewL();
	CUpnpRequestParserTest();
	~CUpnpRequestParserTest();

private:	// methods from MParserObserver
	virtual void GotBodyData();
	virtual void DataParsed();
	virtual void GotHeaders();
	virtual void ParsingComplete(RMemChunk& aExcessData);
	virtual void ParserError(TInt aError);

public:	// methods from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();


private:
	void NegateEscapeEncodeL(const TDesC& aDes);

private:
	CUpnpRequestParser*					iRequestParser;
	MHTTPDataSupplier*					iDataSupplier;
	HBufC8*								iDataChunk;
	TBool								iNeedToParseExcessData;
	TPtrC8								iDataChunkPtr;
	CActiveScheduler* 					iSched;
	TInt								iExpectedRequests;
	TInt								iDataChunkCount;
	RPointerArray<HBufC8>				iDataChunkArray;
	CRequest*							iRequest;
	CHeaderCodec*						iCodec;
	RStringPool							iStringPool;
	CChunkManager* 						iMBufMgr;
	//MMBufSizeAllocator* 				iMBufSzAllctr;
	//RMBufChain							iDataChunkChain;
	RMemChunk							iDataChunkChain;
	TBuf8<KMaxMsgLen>					iExcessData8;
	TBool								iIsStarted;
	TInt								iError;
	
#ifdef __CFLOG_ACTIVE
	__CFLOG_DECLARATION_MEMBER;
#endif
	};

#endif /*CUPNPREQUESTPARSERTEST_H_*/
