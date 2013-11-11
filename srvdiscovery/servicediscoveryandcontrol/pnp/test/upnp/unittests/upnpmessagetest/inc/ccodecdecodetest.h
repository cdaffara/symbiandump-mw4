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

#ifndef CCODECDECODETEST_H_
#define CCODECDECODETEST_H_

#include <e32base.h>
#include <stringpool.h>
#include <inetprottextutils.h>
#include <utf.h>
#include <httpstringconstants.h>
#include <http/thttptable.h>
#include <http/framework/cheadercodec.h>

#include "cupnpmessagetest.h"
#include "mparserobserver.h"
#include "cupnpresponseparser.h"
#include "cupnprequestparser.h"
#include "rhttpheaders.h"
#include "CResponse.h"
#include "CRequest.h"


_LIT(KCodecDecodeTest, "CodecDecodeTest");

class CCodecDecodeTest: public CTestStep,
						public MParserObserver
	{
public:
	static CCodecDecodeTest* NewL();
	CCodecDecodeTest();
	~CCodecDecodeTest();

public:
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void ConstructHeadersL(const TInt aNoOfHdrValPairs);
	void SetHeaderL(const TDesC8& aHeader, const TDesC8& aValue);
	void GetFieldL();

private:	// methods from MParserObserver
	virtual void GotBodyData();
	virtual void DataParsed();
	virtual void GotHeaders();
	virtual void ParsingComplete(RMemChunk& aExcessData);
	virtual void ParserError(TInt aError);

private:
	CUpnpResponseParser*			iRespParser;
	CUpnpRequestParser*				iReqParser;
	CActiveScheduler*				iSched;
	RStringPool						iPool;
	RHTTPHeaders					iRHeaders;
	CRequest*						iRequest;
	CResponse*						iResponse;
	CHeaderCodec*					iCodec;
	};

#endif /*CCODECDECODETEST_H_*/
