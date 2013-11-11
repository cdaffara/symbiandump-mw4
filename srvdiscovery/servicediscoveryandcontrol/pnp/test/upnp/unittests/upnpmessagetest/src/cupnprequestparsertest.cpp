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
#include <upnp/tupnptable.h>


#include "cupnprequestparsertest.h"
#include "rhttpheaders.h"

_LIT8(KUpnpCodecName, "HTTP/UPnP");

CUpnpRequestParserTest* CUpnpRequestParserTest::NewL()
	{
	return (new(ELeave) CUpnpRequestParserTest());	
	}


CUpnpRequestParserTest::CUpnpRequestParserTest()
	{
	SetTestStepName(KRequestParserTest);
	}


CUpnpRequestParserTest::~CUpnpRequestParserTest()
	{
	}


TVerdict CUpnpRequestParserTest::doTestStepPreambleL()
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
	
	iStringPool.OpenL(TUPnPTable::Table());
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iStringPool);
	iRequest = CRequest::NewL(*iCodec, iStringPool);	
	iRequestParser = CUpnpRequestParser::NewL(*this);

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
	RMemoryAllocator allocator ( iMBufMgr );	
	TRAPD(error, iDataChunkChain.CreateL(dataChunk8, allocator));
	if(error != KErrNone)	
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	return TestStepResult();
	}


TVerdict CUpnpRequestParserTest::doTestStepL()
	{
	TPtrC dataChunk;
		
    if (
    	!GetStringFromConfig(ConfigSection(), KDataChunk1, dataChunk)	||
    	!GetBoolFromConfig(ConfigSection(), KParseExcessData, iNeedToParseExcessData)	||
    	!GetIntFromConfig(ConfigSection(), KExpectedRequests, iExpectedRequests)	||
    	!GetIntFromConfig( ConfigSection(), KDataChunkCount, iDataChunkCount )
    	)
		{
		ERR_PRINTF5(_L("===> Problem in reading values from ini.			\
						\nExpected fields are: \n%S\n%S\n%S\n%S <==="
					  ), &KDataChunk1, &KParseExcessData, &KExpectedRequests, 
					  &KDataChunkCount );
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	SetTestStepResult(EPass);	

	iRequestParser->ParseRequest(iDataChunkChain, iRequest);
	if(iError == KErrNone)
		{
		iIsStarted = ETrue;
		CActiveScheduler::Start();
		}
	else
		{
		SetTestStepResult(EFail);	
		}	

	return TestStepResult();
	}


TVerdict CUpnpRequestParserTest::doTestStepPostambleL()
	{
	if(iRequestParser)
		delete iRequestParser;

	if(iRequest)
		delete iRequest;
	
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


void CUpnpRequestParserTest::GotBodyData()
	{
	INFO_PRINTF1(_L("\n********** GOT REQUEST BODY **********"));
	
	iDataSupplier = iRequest->Body();
		
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


void CUpnpRequestParserTest::DataParsed()
	{
	INFO_PRINTF1(_L("\n********** REQUEST DATA PARSED **********"));
	
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

		iRequestParser->ParseRequest(iDataChunkChain, iRequest);
		}

	if ( (iNeedToParseExcessData) && (iDataChunkPtr.Length() > 0))
		{
		if(iRequestParser)
			{
			iRequestParser->ResetParser();
			iRequest->Reset();
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
		
		if(iRequestParser)
			{
			iRequestParser->ParseRequest(iDataChunkChain, iRequest);
			}
		iDataChunkPtr.Set(KNullDesC8);
		}
	}


void CUpnpRequestParserTest::GotHeaders()
	{
	INFO_PRINTF1(_L("\n********** GOT REQUEST HEADERS **********"));
	
	TInt version = iRequest->Handle().IsHTTP10();
			
	const TDesC8& requestMethod8 = iRequest->Handle().Method().DesC();
	TBuf<KMaxHeaderNameLen> requestMethod;
	requestMethod.Copy(requestMethod8);
	
	const TDesC8& requestUri8 = iRequest->Handle().URI().UriDes();
	TBuf<KMaxHeaderNameLen> requestUri;
	requestUri.Copy(requestUri8);
	
	TVersion ver = iRequest->Version();
	
	INFO_PRINTF5(_L("\n===> REQUEST START LINE: %S %S HTTP/%d.%d<==="), &requestMethod, &requestUri, ver.iMajor, ver.iMinor);

	RHTTPHeaders hdr = iRequest->Handle().GetHeaderCollection();
	THTTPHdrFieldIter it = hdr.Fields();
		
	TBuf<KMaxHeaderNameLen>  fieldName16;
	TBuf<KMaxHeaderValueLen> fieldVal16;

	while (it.AtEnd() == EFalse)
		{
		RStringTokenF fieldName = it();
		RStringF fieldNameStr = iStringPool.StringF(fieldName);
		TRAP_IGNORE(iRequest->Handle().GetHeaderCollection().FieldPartsL(fieldNameStr));
		
		TPtrC8 rawData8;
		hdr.GetRawField(fieldNameStr, rawData8);

		const TDesC8& fieldNameDesC = fieldNameStr.DesC();
		fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));
		fieldVal16.Copy(rawData8);
		INFO_PRINTF3(_L("\n===> %S: %S <==="), &fieldName16, &fieldVal16);

		++it;
		}
	}


void CUpnpRequestParserTest::ParsingComplete(RMemChunk& aExcessData)
	{
	INFO_PRINTF1(_L("\n********** REQUEST PARSING COMPLETED **********"));
	
	if(--iExpectedRequests == 0)
		{
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
			iRequest->Reset();
			iDataChunkPtr.Set(iExcessData8);
			}
		}
	}

	
void CUpnpRequestParserTest::ParserError(TInt aError)
	{
	INFO_PRINTF1(_L("\n********** REQUEST PARSER ERROR **********"));
	INFO_PRINTF2(_L("\n===> Error: %d <==="), aError);
	SetTestStepResult(EFail);
	if(iIsStarted)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iError = aError;
		}	
	}

	
void CUpnpRequestParserTest::NegateEscapeEncodeL(const TDesC& aDes)
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
