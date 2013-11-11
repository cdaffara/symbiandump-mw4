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

#ifndef __CUPNPMESSGETEST_H__
#define __CUPNPMESSGETEST_H__


//  Include Files
#include <e32base.h>
#include <test/testexecuteserverbase.h>


// 	Literal Constants. INI field names for parser tests
_LIT(KDataChunk1,  "DataChunk1");
_LIT(KParseExcessData, "ParseExcessData");
_LIT(KExpectedResponses, "ExpectedResponses");
_LIT(KExpectedRequests,	"ExpectedRequests");
_LIT(KDataChunkCount, "DataChunkCount");
_LIT(KDateFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3");
_LIT(KCRLF, "\r\n");
_LIT(KDataChunk, "DataChunk%d");

//	Literal Constants. INI field names for composer tests
_LIT(KRequestMethod,		"RequestMethod");
_LIT(KUri,					"URI");
_LIT(KRequestBodyPresence, 	"RequestBodyPresence");
_LIT(KBody,					"Body");
_LIT(KNoOfHdrValPairs,		"NoOfHdrValPairs");
_LIT(KNoOfHdrParamValPairs, "NoOfHdrParamValPairs");
_LIT(KResponseStatusCode,	"ResponseStatusCode");
_LIT(KResponseStatusText,	"ResponseStatusText");
_LIT(KResponseBodyPresence, "ResponseBodyPresence");
_LIT(KHeaderValuePair, "HeaderValuePair%d");
_LIT(KHeaderParamValPair, "HeaderParamValPair%d");
_LIT(KDirName, "C:\\logs\\UpnpMessageTestLogs\\");

//	Literal constants. INI field names for codec decode tests
_LIT(KRequestHeader, "RequestHeader");

// For RMBufChain
static const TInt KMBuf_MBufSize = 128;
static const TInt KMBuf_MinGrowth = 64;
static const TInt KMBuf_GrowthThreshold = 40;
static const TInt KMBuf_InitialAllocation = 128;


const TInt KMaxHeaderNameLen = 128;
const TInt KMaxHeaderValueLen = 128;
const TChar backSlash = '\\';

const TInt KMaxBodyLen = 1024;
const TInt KMaxMsgLen = 6144;

const TChar colon = ':';
const TChar equal = '=';
const TChar partIdx = '@';

const TInt KCMaxMsgLen = 6144;
const TInt KMaxLen = 6144;


class CUpnpMessageTest:public CTestServer
	{
public:
	static CUpnpMessageTest* NewL();
	CUpnpMessageTest();
	~CUpnpMessageTest();

	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	const TPtrC  ServerName();

private:
	CTestStep *iTestStep;
	};
//  Function Prototypes
GLDEF_C TInt E32Main();


#endif  // __CUPNPMESSGETEST_H__


