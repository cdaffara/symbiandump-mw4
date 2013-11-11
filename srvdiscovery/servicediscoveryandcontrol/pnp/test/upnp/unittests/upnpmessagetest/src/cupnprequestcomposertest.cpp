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

#include "cupnprequestcomposertest.h"
#include "rhttpheaders.h"

_LIT(KFileName, "C:\\logs\\UpnpMessageTestLogs\\composedrequests.dump");
_LIT(KRCTest, "REQUEST_COMPOSER_TEST_1");

_LIT8(KUpnpCodecName, "HTTP/UPnP");


CUpnpRequestComposerTest* CUpnpRequestComposerTest::NewL()
	{
	return (new(ELeave)CUpnpRequestComposerTest());
	}


CUpnpRequestComposerTest::CUpnpRequestComposerTest()
	{
	SetTestStepName(KRequestComposerTest);
	}

CUpnpRequestComposerTest::~CUpnpRequestComposerTest()
	{
	}

TVerdict CUpnpRequestComposerTest::doTestStepPreambleL()
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

	iRequestBuffer.Create(KCMaxMsgLen);

	iStringPool.OpenL(TUPnPTable::Table());
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iStringPool);
	iRequest = CRequest::NewL(*iCodec, iStringPool);
	iRequestComposer = CUpnpRequestComposer::NewL(*this);
	
	iRequestBody = NULL;

	return TestStepResult();
	}

TVerdict CUpnpRequestComposerTest::doTestStepL()
	{
	TBool requestBodyPresence;
	TPtrC requestMethod;
	TPtrC uri;
	TInt noOfHdrValPairs;
	TInt noOfHdrParamValPairs;
	RMemoryAllocator allocator ( iMBufMgr );
	
	if( !GetStringFromConfig(ConfigSection(), KRequestMethod, requestMethod) ||
		!GetStringFromConfig(ConfigSection(), KUri, uri) ||
		!GetBoolFromConfig(ConfigSection(), KRequestBodyPresence, requestBodyPresence) ||
		!GetIntFromConfig(ConfigSection(), KNoOfHdrValPairs, noOfHdrValPairs) ||
		!GetIntFromConfig(ConfigSection(), KNoOfHdrParamValPairs, noOfHdrParamValPairs)
			)
		{
		ERR_PRINTF6(_L("===> Problem in reading values from ini.			\
				\nExpected fields are: \n%S\n%S\n%S\n%S\n%S <==="
			  ), &KRequestMethod, &KUri, &KRequestBodyPresence,
			  &KNoOfHdrValPairs, &KNoOfHdrParamValPairs);
		
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	SetTestStepResult(EPass);
	
	// Construct and set the request method
	TRAPD(err, ConstructRequestMethodL(requestMethod));
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// Construct and set the request uri
	TRAP(err, ConstructRequestUriL(uri));
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();	
		}
	
	// Construct and set the request headers
	TRAP(err, ConstructRequestHeadersL(noOfHdrValPairs, noOfHdrParamValPairs));
	if( err != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// Construct the request body reqd from the ini file
	TRAP(err, ConstructRequestBodyL(requestBodyPresence));
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	if(requestBodyPresence)
		{
		RMemChunk tmpRequestBody;
		tmpRequestBody.CreateL(*iRequestBody, allocator);
		iRequest->AddBody(this);
		}
	
	TRAPD(error, iRequestComposer->ComposeRequest(iRequest));
	if(error != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	CRequest& request = iRequestComposer->Request();
	CActiveScheduler::Start();
	return TestStepResult();
	}


TVerdict CUpnpRequestComposerTest::doTestStepPostambleL()
	{
	if(iRequest)
		delete iRequest;
	
	iStringPool.Close();
	
	if(iRequestComposer)
		delete iRequestComposer;
	
	if(iMBufMgr)
		delete iMBufMgr;
	
	delete iCodec;
	delete iSched;
	
	if(iRequestBody)
		delete iRequestBody;
	
	iRequestBuffer.Close();
	
#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
	
	REComSession::FinalClose();
	return TestStepResult();
	}


void CUpnpRequestComposerTest::ConstructRequestMethodL(const TDesC& aRequestMethod)
	{
	RRequest request = iRequest->Handle();
		
	// set the request method
	HBufC8* requestMethod8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aRequestMethod);
	CleanupStack::PushL(requestMethod8);
	RStringF method = iStringPool.OpenFStringL(requestMethod8->Des());
	request.SetMethod(method);
	method.Close();
	CleanupStack::PopAndDestroy(requestMethod8);
	}


void CUpnpRequestComposerTest::ConstructRequestUriL(const TDesC& aRequestUri)
	{
	RRequest request = iRequest->Handle();

	// set the request Uri
	HBufC8* uri8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aRequestUri);
	CleanupStack::PushL(uri8);
	TUriParser8 uri;
	uri.Parse(uri8->Des());
	request.SetURIL(uri);
	CleanupStack::PopAndDestroy(uri8);
	}


void CUpnpRequestComposerTest::ConstructRequestHeadersL(const TInt aNoOfHdrValPairs, const TInt aNoOfHdrParamValPairs)
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
		HBufC8* headerValuePairBuf;
		headerValuePairBuf = CnvUtfConverter::ConvertFromUnicodeToUtf8L(headerValuePair);
		CleanupStack::PushL(headerValuePairBuf);
		TPtrC8 headerValPairPtr(headerValuePairBuf->Des());
		TInt consumed = headerValPairPtr.Locate(colon);
		User::LeaveIfError(consumed);
		
		// Get the header. i.e, the string before the ':'
		TPtrC8 header = headerValPairPtr.Left(consumed);
		
		if(header.Compare(_L8("EXT")) == 0)
			{
			SetHeaderL(header, KNullDesC8);	
			}
		else
			{
			headerValPairPtr.Set(headerValPairPtr.Mid(consumed+1));
			User::LeaveIfError(InetProtTextUtils::RemoveWhiteSpace(headerValPairPtr, InetProtTextUtils::ERemoveBoth));
		
			// The left over data is the value for the header.
			SetHeaderL(header, headerValPairPtr);
			}
		CleanupStack::PopAndDestroy(headerValuePairBuf);
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

void CUpnpRequestComposerTest::SetHeaderL(const TDesC8& aHeader, const TDesC8& aValue)
	{
	RRequest request = iRequest->Handle();
	RHTTPHeaders headers = request.GetHeaderCollection();
	THTTPHdrVal val;

	RStringF headerSF = iStringPool.OpenFStringL(aHeader);
	CleanupClosePushL(headerSF);
	
	if( (headerSF == iStringPool.StringF(UPnP::EMX, TUPnPTable::Table())) || (headerSF == iStringPool.StringF(HTTP::EContentLength, THTTPTable::Table())) || (headerSF == iStringPool.StringF(UPnP::ETimeout, TUPnPTable::Table())))
		{
		TInt intVal;
		InetProtTextUtils::ConvertDescriptorToInt(aValue, intVal);
		val.SetInt(intVal);	
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


void CUpnpRequestComposerTest::SetParamL(const TDesC8& aHeader, const TDesC8& aParam, const TDesC8& aValue, const TInt aPartIndex)
	{
	RRequest request = iRequest->Handle();
	RHTTPHeaders headers = request.GetHeaderCollection();

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


void CUpnpRequestComposerTest::ConstructRequestBodyL(const TBool aRequestBodyPresence)
	{
	TPtrC body;
	if(iRequestBody)
		{
		delete iRequestBody;
		iRequestBody = NULL;
		}

	if(aRequestBodyPresence)
		{
		if(!GetStringFromConfig(ConfigSection(), KBody, body))
			{
			ERR_PRINTF1(_L("===> Problem in reading request body from ini file <===="));
			SetTestStepResult(EFail);
			}
		iRequestBody = CnvUtfConverter::ConvertFromUnicodeToUtf8L(body);
		}
	else
		{
		iRequestBody = CnvUtfConverter::ConvertFromUnicodeToUtf8L(KNullDesC);
		}
	}


void CUpnpRequestComposerTest::ComposingConcluded()
	{
	INFO_PRINTF1(_L("\n********** REQUEST COMPOSING CONCLUDED **********"));
	INFO_PRINTF1(_L("\n===> Entire Request: C:\\logs\\UpnpMessageTestLogs\\composedrequests.dump <==="));
	CActiveScheduler::Stop();
	
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
			file.Write(iRequestBuffer);
			file.Close();
			}
		fs.Close();
		}
	iRequestComposer->ResetComposer();
	}


void CUpnpRequestComposerTest::MessageDataReadyL(RBuf8& aData)
	{
	INFO_PRINTF1(_L("\n********** MESSAGE DATA READY **********"));
	
	/*
	TBuf8<KMaxMsgLen> buf;
	buf.SetLength(aData.Length());
	buf.FillZ(aData.Length());
	aData.CopyOut(buf);
	*/
	
	TInt responseBufferLen = iRequestBuffer.Length();
	if( (responseBufferLen + aData.Length()) > KCMaxMsgLen )
		{
		iRequestBuffer.ReAlloc(responseBufferLen + aData.Length());	
		}
	
	iRequestBuffer.Append(aData);
	iRequestComposer->RequestDataSent();
	}


void CUpnpRequestComposerTest::ComposerError(TInt aError)
	{
	INFO_PRINTF1(_L("\n********** REQUEST COMPOSER ERROR **********"));
	INFO_PRINTF2(_L("\n===> Error: %d <==="), aError);
	SetTestStepResult(EFail);
	CActiveScheduler::Stop();
	}


TBool CUpnpRequestComposerTest::GetNextDataPart(TPtrC8& aDataPart)
	{
	aDataPart.Set(iRequestBody->Des());
	return ETrue;
	}


void CUpnpRequestComposerTest::ReleaseData()
	{
	// do nothing
	}


TInt CUpnpRequestComposerTest::OverallDataSize()
	{
	return iRequestBody->Length();
	}


TInt CUpnpRequestComposerTest::Reset()
	{
	return KErrNotSupported;
	}
