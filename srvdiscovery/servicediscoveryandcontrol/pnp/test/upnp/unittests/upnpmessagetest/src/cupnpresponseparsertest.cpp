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

#include <http/thttphdrfielditer.h>
#include <httpstringconstants.h>
#include <http/thttptable.h>
#include <stringpool.h>
#include <utf.h>
#include <http/framework/cheadercodecplugin.h>
#include <e32debug.h>
#include <upnp/tupnptable.h>

#include "cupnpresponseparsertest.h"
#include "rhttpheaders.h"

_LIT8(KUpnpCodecName, "HTTP/UPnP");
_LIT(KTest14, "RESPONSE_PARSER_TEST_18");
_LIT(KTest15, "RESPONSE_PARSER_TEST_19");
_LIT(KTest16, "RESPONSE_PARSER_TEST_20");


CUpnpResponseParserTest* CUpnpResponseParserTest::NewL()
	{
	return (new(ELeave) CUpnpResponseParserTest());
	}


CUpnpResponseParserTest::CUpnpResponseParserTest()
	{
	SetTestStepName(KResponseParserTest);
	}


CUpnpResponseParserTest::~CUpnpResponseParserTest()
	{
	}


TVerdict CUpnpResponseParserTest::doTestStepPreambleL()
	{
#ifdef __CFLOG_ACTIVE
	__CFLOG_CREATEL;
	__CFLOG_OPEN;
#endif
	
	iIsStarted = EFalse;
	iError = KErrNone;
	
	iSched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iSched);

	//iMBufMgr = CMBufManager::NewL(256 * 1024, iMBufSzAllctr);
	//iMBufSzAllctr->AddL(KMBuf_MBufSize, (256 * 1024/(KMBuf_MBufSize+sizeof(RMBuf)))-1, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
	iMBufMgr = CChunkManager::NewL ( 256 * 1024 );
	iMBufMgr->AddPoolL(KMBuf_MBufSize, (256 * 1024/(KMBuf_MBufSize+sizeof(RMBuf)))-1, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
	RMemoryAllocator allocator( iMBufMgr );
	
	iStringPool.OpenL(TUPnPTable::Table());
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iStringPool);
	iResponse = CResponse::NewL(*iCodec, iStringPool);	
	iResponseParser = CUpnpResponseParser::NewL(*this);

	iDataChunk = NULL;
	if(!iDataChunkChain.IsEmpty())
		iDataChunkChain.Free();
	
	GetIntFromConfig( ConfigSection(), KDataChunkCount, iDataChunkCount );
	
	TInt dataChunkCountIter = 1;
	
	while( dataChunkCountIter <= iDataChunkCount )
		{
		TPtrC dataChunk;
		TBuf<10> buf(KNullDesC);
		
		// The below line is to create formats such as
		// DataChunk1, DataChunk2, DataChunk3...
		buf.Format(KDataChunk, dataChunkCountIter);
		
		GetStringFromConfig(ConfigSection(), buf, dataChunk);
		RBuf tt;
		tt.CreateL(dataChunk);
		CleanupClosePushL(tt);
		TRAPD(error, NegateEscapeEncodeL(tt));
		if(error != KErrNone)
			{
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		dataChunkCountIter++;
		CleanupStack::PopAndDestroy(&tt);
		}
	
	TPtrC8 dataChunk8(iDataChunkArray[0]->Des());
	TRAPD(error, iDataChunkChain.CreateL(dataChunk8, allocator));
	if(error != KErrNone)	
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	return TestStepResult();
	}


TVerdict CUpnpResponseParserTest::doTestStepL()
	{
	TPtrC dataChunk;
		
    if (
    	!GetStringFromConfig(ConfigSection(), KDataChunk1, dataChunk)	||
    	!GetBoolFromConfig(ConfigSection(), KParseExcessData, iNeedToParseExcessData)	||
    	!GetIntFromConfig(ConfigSection(), KExpectedResponses, iExpectedResponses)	||
    	!GetIntFromConfig( ConfigSection(), KDataChunkCount, iDataChunkCount )
    	)
		{
		ERR_PRINTF5(_L("===> Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n%S\n%S\n%S <==="
					  ), &KDataChunk1, &KParseExcessData, &KExpectedResponses,
					   &KDataChunkCount );
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	SetTestStepResult(EPass);	
	
	if(ConfigSection().Compare(_L("RESPONSE_PARSER_TEST_15")) == 0)
		{
		delete iResponse;
		iResponse = NULL;	
		}

	iResponseParser->ParseResponse(iDataChunkChain, iResponse);
	if(iError == KErrNone)
		{
		CResponse& response = iResponseParser->Response();
		iIsStarted = ETrue;
		CActiveScheduler::Start();
		}
	else
		{
		SetTestStepResult(EFail);	
		}	
	
	return TestStepResult();
	}


TVerdict CUpnpResponseParserTest::doTestStepPostambleL()
	{
	if(iResponseParser)
		delete iResponseParser;

	if(iResponse)
		delete iResponse;
	
	iStringPool.Close();
	
	if(!iDataChunkChain.IsEmpty())
		{
		iDataChunkChain.Free();
		}
	
	if(iMBufMgr)
		delete iMBufMgr;
		
	delete iCodec;
	delete iSched;
	
	iDataChunkArray.ResetAndDestroy();
	iDataChunkArray.Close();

#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif

	REComSession::FinalClose();
	return TestStepResult();
	}


void CUpnpResponseParserTest::GotBodyData()
	{

	INFO_PRINTF1(_L("\n********** GOT RESPONSE BODY **********"));
	
	iDataSupplier = iResponse->Body();
	
	INFO_PRINTF2(_L("\n===> BODY CHUNK LENGTH: %d <==="), iDataSupplier->OverallDataSize());
	
	TPtrC8 ptr;
	iDataSupplier->GetNextDataPart(ptr);
	
	TBuf8<1024> buf8(ptr);
	TBuf<1024> buf;
	buf.Copy(buf8);
	
	INFO_PRINTF2(_L("\n===> BODY: %S <==="), &buf);
	
	iDataSupplier->ReleaseData();
	iDataSupplier->Reset();
	iDataSupplier->ReleaseData();
	}


void CUpnpResponseParserTest::DataParsed()
	{
	INFO_PRINTF1(_L("\n********** RESPONSE DATA PARSED **********"));
	RMemoryAllocator allocator( iMBufMgr );
	
	if( (iDataChunkArray.Count() > 0) && (iDataChunkCount > 1) && (iDataChunkPtr.Length() == 0) )
		{
		HBufC8* firstChunk = iDataChunkArray[0];
		iDataChunkArray.Remove(0);
		delete firstChunk;
		}
	
	if( (iDataChunkArray.Count() > 0) && (iDataChunkCount > 1) && (iDataChunkPtr.Length() == 0) )
		{
		TPtrC8 dataChunk8(iDataChunkArray[0]->Des());
		if(!iDataChunkChain.IsEmpty())
			{
			iDataChunkChain.Free();
			}
		
		TRAPD(error, iDataChunkChain.CreateL(dataChunk8, allocator));
		if(error != KErrNone)
			{
			ParserError(error);
			return;
			}

		iResponseParser->ParseResponse(iDataChunkChain, iResponse);
		}

	if ((iNeedToParseExcessData) && (iDataChunkPtr.Length() > 0))
		{
		if(iResponseParser)
			{
			iResponseParser->ResetParser();
			iResponse->Reset();
			}
		
		if(!iDataChunkChain.IsEmpty())
			{
			iDataChunkChain.Free();
			}
		TRAPD(error, iDataChunkChain.CreateL(iDataChunkPtr, allocator));
		if(error != KErrNone)
			{
			ParserError(error);
			return;
			}
		
		if(iResponseParser)
			{
			iResponseParser->ParseResponse(iDataChunkChain, iResponse);
			}
		iDataChunkPtr.Set(KNullDesC8);
		}
	}


void CUpnpResponseParserTest::GotHeaders()
	{
	INFO_PRINTF1(_L("\n********** GOT RESPONSE HEADERS **********"));
	
	TInt ver = iResponse->Handle().IsHTTP10();
	
	const TDesC8& statusText8 = iResponse->Handle().StatusText().DesC();
	TBuf<KMaxHeaderNameLen> statusText;
	statusText.Copy(statusText8);
	
	INFO_PRINTF3(_L("\n===> RESPONSE STATUS: %d %S <==="), iResponse->Handle().StatusCode(), &statusText);
	
	RHTTPHeaders hdr = iResponse->Handle().GetHeaderCollection();
	THTTPHdrFieldIter it = hdr.Fields();
	
	TBuf<KMaxHeaderNameLen>  fieldName16;
	TBuf<KMaxHeaderValueLen> fieldVal16;

	while (it.AtEnd() == EFalse)
		{
		RStringTokenF fieldName = it();
		RStringF fieldNameStr = iStringPool.StringF(fieldName);
		
		TPtrC8 rawData8;
		hdr.GetRawField(fieldNameStr, rawData8);

		const TDesC8& fieldNameDesC = fieldNameStr.DesC();
		fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));

		fieldVal16.Copy(rawData8);
		INFO_PRINTF3(_L("\n===> %S: %S <==="), &fieldName16, &fieldVal16);
		
		++it;
		}
		
	TInt hasBody = iResponse->Handle().HasBody();	
	}


void CUpnpResponseParserTest::ParsingComplete(RMemChunk& aExcessData)
	{
	INFO_PRINTF1(_L("\n********** RESPONSE PARSING COMPLETED **********"));

	if(--iExpectedResponses == 0)
		{
		iResponse->Handle().RemoveBody();
		CActiveScheduler::Stop();
		iIsStarted = EFalse;	
		}

	TInt len = aExcessData.Length();
	if(len > 0)
		{
		iExcessData8.SetLength(len);
		iExcessData8.FillZ(len);
		aExcessData.CopyOut(iExcessData8);

#ifdef __WINSCW__
		TBuf<KMaxMsgLen> excessData16;
		excessData16.Copy(iExcessData8);
		INFO_PRINTF2(_L("\n===> EXCESS DATA: %S <==="), &excessData16);
#endif	

		if(iNeedToParseExcessData)
			{
			iResponse->Reset();
			iDataChunkPtr.Set(iExcessData8);
			}
		}
	}

	
void CUpnpResponseParserTest::ParserError(TInt aError)
	{
	INFO_PRINTF1(_L("\n********** RESPONSE PARSER ERROR **********"));
	INFO_PRINTF2(_L("\n===> Error: %d <==="), aError);

	if((ConfigSection().Compare(KTest14()) != 0)&&
	   (ConfigSection().Compare(KTest15()) != 0)&&
	   (ConfigSection().Compare(KTest16()) != 0))
		{
		SetTestStepResult(EFail);
		}			
	if(iIsStarted)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iError = aError;
		}	
	}

	
void CUpnpResponseParserTest::NegateEscapeEncodeL(const TDesC& aDes)
	{
	HBufC* buf = HBufC::NewL(KMaxMsgLen);
	CleanupStack::PushL(buf);
	TPtr tmpBuf(buf->Des());
	TPtrC ptr(aDes);
	
	while(ptr.Length() > 0)
		{
		TInt pos = ptr.Locate(backSlash);
		if(pos == KErrNotFound)
			{
			tmpBuf.Append(ptr);
			break;
			}
		else
			{
			tmpBuf.Append(ptr.Left(pos));
			tmpBuf.Append(KCRLF);
			ptr.Set(ptr.Mid(pos+4));
			}
		}
	iDataChunk = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*buf);
	TRAPD(err, iDataChunkArray.AppendL(iDataChunk));
	if(err != KErrNone)
		{
		delete iDataChunk;
		iDataChunk = NULL;
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(buf);
	}
