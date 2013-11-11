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

#ifndef CCODECENCODETEST_H_
#define CCODECENCODETEST_H_

#include <e32base.h>
#include <stringpool.h>
#include <inetprottextutils.h>
#include <utf.h>
#include <httpstringconstants.h>
#include <http/thttptable.h>
#include <http/framework/cheadercodec.h>

#include "cupnpmessagetest.h"
#include "mparserobserver.h"
#include "cupnpresponsecomposer.h"
#include "cupnprequestcomposer.h"
#include "rhttpheaders.h"
#include "CResponse.h"
#include "CRequest.h"


_LIT(KCodecEncodeTest, "CodecEncodeTest");

class CCodecEncodeTest: public CTestStep,
						public MComposerObserver
	{
public:
	static CCodecEncodeTest* NewL();
	CCodecEncodeTest();
	~CCodecEncodeTest();

public:
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void ConstructHeadersL(const TInt aNoOfHdrValPairs);
	void SetHeaderL(const TDesC8& aHeader, const TDesC8& aValue);
	void GetFieldL();

private:	// methods from MParserObserver
	virtual void MessageDataReadyL(RBuf8& aData);
	virtual void ComposingConcluded();
	virtual void ComposerError(TInt aError);

private:
	CUpnpResponseComposer*			iRespComposer;
	CUpnpRequestComposer*			iReqComposer;
	CActiveScheduler*				iSched;
	RStringPool						iPool;
	RHTTPHeaders					iRHeaders;
	CRequest*						iRequest;
	CResponse*						iResponse;
	CHeaderCodec*					iCodec;
	};

#endif /*CCODECENCODETEST_H_*/
