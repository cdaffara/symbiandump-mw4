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

#include <httpstringconstants.h>
#include <http/thttptable.h>
#include <stringpool.h>
#include <utf.h>
#include <inetprottextutils.h>
#include <http/thttphdrval.h>
#include <uri8.h>
#include <http/framework/cheadercodecplugin.h>
#include <upnp/tupnptable.h>

#include "cupnpresponsecomposertest.h"
#include "rhttpheaders.h"

_LIT(KFileName, "C:\\logs\\UpnpMessageTestLogs\\composedresponses.dump");
_LIT(KRCTest, "RESPONSE_COMPOSER_TEST_1");

_LIT8(KUpnpCodecName, "HTTP/UPnP");


const TInt KMaxStatusTextLen = 50;


CUpnpResponseComposerTest* CUpnpResponseComposerTest::NewL()
	{
	return (new(ELeave) CUpnpResponseComposerTest());
	}


CUpnpResponseComposerTest::CUpnpResponseComposerTest()
	{
	SetTestStepName(KResponseComposerTest);
	}


CUpnpResponseComposerTest::~CUpnpResponseComposerTest()
	{
	}


TVerdict CUpnpResponseComposerTest::doTestStepPreambleL()
	{
#ifdef __CFLOG_ACTIVE
	__CFLOG_CREATEL;
	__CFLOG_OPEN;
#endif
	
	iSched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iSched);

	//iMBufMgr = CMBufManager::NewL(256 * 1024, iMBufSzAllctr);
	//iMBufSzAllctr->AddL(KMBuf_MBufSize, (256 * 1024/(KMBuf_MBufSize+sizeof(RMBuf)))-1, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
	iMBufMgr = CChunkManager::NewL ( 256 * 1024 );
	iMBufMgr->AddPoolL(KMBuf_MBufSize, (256 * 1024/(KMBuf_MBufSize+sizeof(RMBuf)))-1, KMBuf_MinGrowth, KMBuf_GrowthThreshold);

	iResponseBuffer.Create(KCMaxMsgLen);
	iStringPool.OpenL(TUPnPTable::Table());
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iStringPool);
	iResponse = CResponse::NewL(*iCodec, iStringPool);	
	iResponseComposer = CUpnpResponseComposer::NewL(*this);
	
	iResponseBody = NULL;
	
	return TestStepResult();
	}

TVerdict CUpnpResponseComposerTest::doTestStepL()
	{
	TBool responseBodyPresence;
	TInt responseStatusCode;
	TPtrC responseStatusText;
	TInt noOfHdrValPairs;
	TInt noOfHdrParamValPairs;
	RMemoryAllocator allocator ( iMBufMgr );
	
	if( !GetIntFromConfig(ConfigSection(), KResponseStatusCode, responseStatusCode) ||
		!GetStringFromConfig(ConfigSection(), KResponseStatusText, responseStatusText) ||
		!GetBoolFromConfig(ConfigSection(), KResponseBodyPresence, responseBodyPresence) ||
		!GetIntFromConfig(ConfigSection(), KNoOfHdrValPairs, noOfHdrValPairs) ||
		!GetIntFromConfig(ConfigSection(), KNoOfHdrParamValPairs, noOfHdrParamValPairs)
			)
		{
		ERR_PRINTF6(_L("===> Problem in reading values from ini.			\
				\nExpected fields are: \n%S\n%S\n%S\n%S\n%S <==="
			  ), &KResponseStatusCode, &KResponseStatusText, &KResponseBodyPresence,
			  &KNoOfHdrValPairs, &KNoOfHdrParamValPairs);
		
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	SetTestStepResult(EPass);
	
	// Construct the status line
	TRAPD(err, ConstructResponseStatusLineL(responseStatusCode, responseStatusText));
	if( err != KErrNone )
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// Construct and set the request headers
	TRAP(err, ConstructResponseHeadersL(noOfHdrValPairs, noOfHdrParamValPairs));
	if( err != KErrNone )
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// Construct the request body reqd from the ini file
	TRAP(err, ConstructResponseBodyL(responseBodyPresence));
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		
	if(responseBodyPresence)
		{
		RMemChunk tmpResponseBody;
		tmpResponseBody.CreateL(*iResponseBody, allocator);
		iResponse->AddBody(this);
		}

	iResponseComposer->ComposeResponse(iResponse);
	CActiveScheduler::Start();
	
	return TestStepResult();
	}


TVerdict CUpnpResponseComposerTest::doTestStepPostambleL()
	{	
	if(iResponse)
		delete iResponse;
	
	iStringPool.Close();
		
	if(iResponseComposer)
		delete iResponseComposer;
	
	if(iMBufMgr)
		delete iMBufMgr;
	
	delete iCodec;
	delete iSched;
	
	if(iResponseBody)
		delete iResponseBody;
	
	iResponseBuffer.Close();
	
#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
	
	REComSession::FinalClose();
	return TestStepResult();
	}


void CUpnpResponseComposerTest::ConstructResponseStatusLineL(const TInt aStatusCode, const TDesC& aStatusText)
	{
	RResponse response = iResponse->Handle();
	
	response.SetStatusCode(aStatusCode);
	
	TBuf8<KMaxStatusTextLen> statusBuf8;
	statusBuf8.Copy(aStatusText);
	RStringF statusTextSF = iStringPool.OpenFStringL(statusBuf8);
	
	response.SetStatusText(statusTextSF);
	
	statusTextSF.Close();
	}

	
void CUpnpResponseComposerTest::ConstructResponseHeadersL(const TInt aNoOfHdrValPairs, const TInt aNoOfHdrParamValPairs)
	{
	// Create the "Header: Value" pair array.
	TInt headersCounter = 1;
	while(headersCounter <= aNoOfHdrValPairs)
		{
		TPtrC headerValuePair;
		TBuf<20> buf(KNullDesC);
		
		// The below line is to create formats such as
		// HeaderValuePair1, HeaderValuePair2...
		buf.Format(KHeaderValuePair, headersCounter);
		
		GetStringFromConfig(ConfigSection(), buf, headerValuePair);
		HBufC8* headerValPairBuf;
		headerValPairBuf = CnvUtfConverter::ConvertFromUnicodeToUtf8L(headerValuePair);
		CleanupStack::PushL(headerValPairBuf);
		
		TPtrC8 headerValPairPtr(headerValPairBuf->Des());
		TInt consumed = headerValPairPtr.Locate(colon);
		User::LeaveIfError(consumed);
		
		// Get the header. i.e, the string before the ':'
		TPtrC8 header = headerValPairPtr.Left(consumed);
		
		headerValPairPtr.Set(headerValPairPtr.Mid(consumed+1));
		User::LeaveIfError(InetProtTextUtils::RemoveWhiteSpace(headerValPairPtr, InetProtTextUtils::ERemoveBoth));
		
		// The left over data is the value for the header.
		SetHeaderL(header, headerValPairPtr);
 		
 		CleanupStack::PopAndDestroy(headerValPairBuf);
		headersCounter++;
		}
	
	// Create the "Header: param=value" array.
	TInt headerParamsCounter = 1;
	while(headerParamsCounter <= aNoOfHdrParamValPairs)
		{
		TPtrC headerParamValPair;
		TBuf<30> buf(KNullDesC);
		
		// The below line is to create formats such as
		// HeaderParamValPair1, HeaderParamValPair2...
		buf.Format(KHeaderParamValPair, headerParamsCounter);
		
		GetStringFromConfig(ConfigSection(), buf, headerParamValPair);
		HBufC8* hdrParamValPairBuf;
		hdrParamValPairBuf = CnvUtfConverter::ConvertFromUnicodeToUtf8L(headerParamValPair);
		CleanupStack::PushL(hdrParamValPairBuf);
		
		TPtrC8 hdrParamValPairPtr(hdrParamValPairBuf->Des());
		TInt consumed = hdrParamValPairPtr.Locate(colon);
		User::LeaveIfError(consumed);
		
		// Get the header. i.e, the string before the ':'
		TPtrC8 header = hdrParamValPairPtr.Left(consumed);
		
		hdrParamValPairPtr.Set(hdrParamValPairPtr.Mid(consumed+1));
		User::LeaveIfError(InetProtTextUtils::RemoveWhiteSpace(hdrParamValPairPtr, InetProtTextUtils::ERemoveBoth));
		
		// The left over data has the "param=value" pair
		consumed = hdrParamValPairPtr.Locate(equal);
		User::LeaveIfError(consumed);
		
		// Get the param name. i.e, the string before the '='
		TPtrC8 param = hdrParamValPairPtr.Left(consumed);
		
		hdrParamValPairPtr.Set(hdrParamValPairPtr.Mid(consumed+1));
		User::LeaveIfError(InetProtTextUtils::RemoveWhiteSpace(hdrParamValPairPtr, InetProtTextUtils::ERemoveBoth));
		
		consumed = hdrParamValPairPtr.Locate(partIdx);
		
		TPtrC8 paramVal = hdrParamValPairPtr.Left(consumed-1);
		
		hdrParamValPairPtr.Set(hdrParamValPairPtr.Mid(consumed+1));
		
		// hdrParamValPairPtr now has the part index. Convert it to Integer
		TInt partIndex;
		InetProtTextUtils::ConvertDescriptorToInt(hdrParamValPairPtr, partIndex);
		
		if( partIndex > (aNoOfHdrValPairs-1) )
			{
			ERR_PRINTF1(_L("===> Illegal index value for the parameter value <===="));
			User::Leave(KErrGeneral);
			}
			
		SetParamL(header, param, paramVal, partIndex);
		
		CleanupStack::PopAndDestroy(hdrParamValPairBuf);		
		headerParamsCounter++;
		}
	}


void CUpnpResponseComposerTest::SetHeaderL(const TDesC8& aHeader, const TDesC8& aValue)
	{
	RResponse response = iResponse->Handle();
	RHTTPHeaders headers = response.GetHeaderCollection();
	THTTPHdrVal val;
	
	RStringF headerSF = iStringPool.OpenFStringL(aHeader);
	CleanupClosePushL(headerSF);
	
	if( (headerSF == iStringPool.StringF(HTTP::EContentLength, THTTPTable::Table())) || (headerSF == iStringPool.StringF(HTTP::EMaxAge, THTTPTable::Table())) )
		{
		TInt value;
		InetProtTextUtils::ConvertDescriptorToInt(aValue, value);
		val.SetInt(value);
		headers.SetFieldL(headerSF, val);
		}
	
	else
		{
		RStringF ValSF = iStringPool.OpenFStringL(aValue);
		CleanupClosePushL(ValSF);
		val.SetStrF(ValSF);
		headers.SetFieldL(headerSF, val);
		CleanupStack::PopAndDestroy(&ValSF);
		}

	CleanupStack::PopAndDestroy(&headerSF);
	}


void CUpnpResponseComposerTest::SetParamL(const TDesC8& aHeader, const TDesC8& aParam, const TDesC8& aValue, const TInt aPartIndex)
	{
	RResponse response = iResponse->Handle();
	RHTTPHeaders headers = response.GetHeaderCollection();

	RStringF headerSF = iStringPool.OpenFStringL(aHeader);
	CleanupClosePushL(headerSF);
	RStringF paramSF = iStringPool.OpenFStringL(aParam);
	CleanupClosePushL(paramSF);
	
	THTTPHdrVal val;
	
	if((aParam.Compare(iStringPool.StringF(HTTP::EQ, THTTPTable::Table()).DesC()) == 0))
		{
		if((aValue.Compare(_L8("0.1"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.5));
			}
		else if((aValue.Compare(_L8("0.2"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.2));
			}
		else if((aValue.Compare(_L8("0.3"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.3));
			}
		else if((aValue.Compare(_L8("0.4"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.4));
			}
		else if((aValue.Compare(_L8("0.5"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.5));
			}
		else if((aValue.Compare(_L8("0.6"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.6));
			}
		else if((aValue.Compare(_L8("0.7"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.7));
			}
		else if((aValue.Compare(_L8("0.8"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.8));
			}
		else if((aValue.Compare(_L8("0.9"))) == 0)
			{
			val.SetInt(THTTPHdrVal::TQConv(0.9));
			}
		
		headers.SetParamL(headerSF, paramSF, val, aPartIndex);
		CleanupStack::PopAndDestroy(&paramSF);
		CleanupStack::PopAndDestroy(&headerSF);
		}

	else
		{
		RStringF valueSF = iStringPool.OpenFStringL(aValue);
		CleanupClosePushL(valueSF);
		val.SetStrF(valueSF);	
		
		headers.SetParamL(headerSF, paramSF, val, aPartIndex);
	
		CleanupStack::PopAndDestroy(&valueSF);
		CleanupStack::PopAndDestroy(&paramSF);
		CleanupStack::PopAndDestroy(&headerSF);
		}	
	}


void CUpnpResponseComposerTest::ConstructResponseBodyL(const TBool aResponseBodyPresence)
	{
	TPtrC body;
	
	if(iResponseBody)
		{
		delete iResponseBody;
		iResponseBody = NULL;
		}

	if(aResponseBodyPresence)
		{
		if(!GetStringFromConfig(ConfigSection(), KBody, body))
			{
			ERR_PRINTF1(_L("===> Problem in reading response body from ini file <===="));
			SetTestStepResult(EFail);
			}
		iResponseBody = CnvUtfConverter::ConvertFromUnicodeToUtf8L(body);
		}
	else
		{
		iResponseBody = CnvUtfConverter::ConvertFromUnicodeToUtf8L(KNullDesC);
		}
	}


void CUpnpResponseComposerTest::ComposingConcluded()
	{
	INFO_PRINTF1(_L("\n********** RESPONSE COMPOSING CONCLUDED **********"));
	CActiveScheduler::Stop();
	
	INFO_PRINTF1(_L("\n===> Entire Response: C:\\logs\\UpnpMessageTestLogs\\composedresponses.dump <==="));
	
	RFs fs;
	
	TInt error = fs.Connect();
	if(error == KErrNone)
		{
		error = fs.MkDir(KDirName);
		if(error == KErrNone)
			{
			RFile file;
			TInt err = file.Create(fs, KFileName, EFileWrite);
			if(err == KErrAlreadyExists )
				{
				if(ConfigSection().Compare(KRCTest) == 0)
					{
					file.Replace(fs, KFileName, EFileWrite);	
					}
				else
					{
					file.Open(fs, KFileName, EFileWrite);	
					}	
				}
			TInt pos = 0;
			file.Seek(ESeekEnd, pos);
			file.Write(pos, _L8("\n"));
			file.Write(pos, iResponseBuffer);
			file.Close();
			}
		fs.Close();
		}
	iResponseComposer->ResetComposer();
	}


void CUpnpResponseComposerTest::MessageDataReadyL(RBuf8& aData)
	{
	INFO_PRINTF1(_L("\n********** MESSAGE DATA READY **********"));
	
	/*
	TBuf8<KMaxMsgLen> buf;
	buf.SetLength(aData.Length());
	buf.FillZ(aData.Length());
	aData.CopyOut(buf);
	*/
	
	TInt responseBufferLen = iResponseBuffer.Length();
	//if( (responseBufferLen + buf.Length()) > KCMaxMsgLen )
	if( (responseBufferLen + aData.Length()) > KCMaxMsgLen )
		{
		iResponseBuffer.ReAlloc(responseBufferLen + aData.Length());	
		}
	//iResponseBuffer.Append(buf);
	iResponseBuffer.Append(aData);
	iResponseComposer->ResponseDataSent();
	}


void CUpnpResponseComposerTest::ComposerError(TInt aError)
	{
	INFO_PRINTF1(_L("\n********** RESPONSE COMPOSER ERROR **********"));
	INFO_PRINTF2(_L("\n===> Error: %d <==="), aError);
	SetTestStepResult(EFail);
	CActiveScheduler::Stop();
	}


TBool CUpnpResponseComposerTest::GetNextDataPart(TPtrC8& aDataPart)
	{
	aDataPart.Set(iResponseBody->Des());
	return ETrue;
	}


void CUpnpResponseComposerTest::ReleaseData()
	{
	// do nothing
	}


TInt CUpnpResponseComposerTest::OverallDataSize()
	{
	return iResponseBody->Length();
	}


TInt CUpnpResponseComposerTest::Reset()
	{
	return KErrNotSupported;
	}

