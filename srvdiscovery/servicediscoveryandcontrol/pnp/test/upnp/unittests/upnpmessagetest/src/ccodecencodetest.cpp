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

#include <http/framework/cheadercodecplugin.h>
#include <upnp/tupnptable.h>

#include "ccodecencodetest.h"
#include "CHeaderField.h"

_LIT8(KUpnpCodecName, "HTTP/UPnP");
_LIT8(KHeaderSeparator,	"\n");
_LIT(KCoverageTests, "TEST_COVERAGE");


CCodecEncodeTest* CCodecEncodeTest::NewL()
	{
	return (new (ELeave) CCodecEncodeTest());
	}


CCodecEncodeTest::CCodecEncodeTest()
	{
	SetTestStepName(KCodecEncodeTest);
	}


CCodecEncodeTest::~CCodecEncodeTest()
	{
	}


TVerdict CCodecEncodeTest::doTestStepPreambleL()
	{
	iSched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iSched);
	
	iPool.OpenL(TUPnPTable::Table());
	iPool.OpenL(THTTPTable::Table());

	iCodec = NULL;
	iResponse = NULL;
	iRequest = NULL;
	iRespComposer = NULL;
	iReqComposer = NULL;
	
	return TestStepResult();
	}


TVerdict CCodecEncodeTest::doTestStepL()
	{
	TInt noOfHdrValPairs;
	TBool requestHeader;
	
	if(ConfigSection().Compare(KCoverageTests()) != 0)
		{
		if( !GetIntFromConfig(ConfigSection(), KNoOfHdrValPairs, noOfHdrValPairs) ||
		!GetBoolFromConfig(ConfigSection(), KRequestHeader, requestHeader)
		)
			{
			ERR_PRINTF3(_L("===> Problem in reading values from ini.			\
					\nExpected fields are: \n%S\n%S <==="
				  ), &KNoOfHdrValPairs, &KRequestHeader);
			
			SetTestStepResult(EFail);
			return TestStepResult();
			}

		if(!requestHeader)
			{
			iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iPool);
			iResponse = CResponse::NewL(*iCodec, iPool);
			iRHeaders = iResponse->Handle().GetHeaderCollection();
			iRespComposer = CUpnpResponseComposer::NewL(*this);
			}
		else
			{
			iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iPool);
			iRequest = CRequest::NewL(*iCodec, iPool);
			iRHeaders = iRequest->Handle().GetHeaderCollection();		
			iReqComposer = CUpnpRequestComposer::NewL(*this);
			}
		
		RStringF s = iPool.StringF(HTTP::EAccept, THTTPTable::Table());
		_LIT8(KSomeVal, "some");
		iRHeaders.SetRawFieldL(s, KSomeVal, KHeaderSeparator);
		THTTPHdrVal aV;
		iRHeaders.GetField(s,0,aV);

		TRAPD(err, ConstructHeadersL(noOfHdrValPairs));
		if(err != KErrNone)
			{
			SetTestStepResult(EFail);
			return TestStepResult();
			}

		TRAP(err, GetFieldL());
		if(err != KErrNone)
			{
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		}
	
	// The follwoing section of code has been written for code coverage.
	if(ConfigSection().Compare(KCoverageTests())== 0)
		{
		iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iPool);
		iResponse = CResponse::NewL(*iCodec, iPool);
		iRHeaders = iResponse->Handle().GetHeaderCollection();
		iRespComposer = CUpnpResponseComposer::NewL(*this);

		THTTPHdrVal va;
		CHeaderFieldPart* hdrFldPart = CHeaderFieldPart::NewL(va);
		hdrFldPart->NumParameters();
		THeaderFieldParamIter t = hdrFldPart->Parameters();
		delete hdrFldPart; 
		
		RStringF man = iPool.StringF(UPnP::EMAN, TUPnPTable::Table());
		RStringF soapaction = iPool.StringF(UPnP::ESoapAction, TUPnPTable::Table());
		RStringF mx = iPool.StringF(UPnP::EMX, TUPnPTable::Table());
		RStringF st = iPool.StringF(UPnP::EST, TUPnPTable::Table());
		RStringF timeout = iPool.StringF(UPnP::ETimeout, TUPnPTable::Table());
		RStringF callback = iPool.StringF(UPnP::ECallback, TUPnPTable::Table());
		RStringF ns = iPool.StringF(UPnP::ENS, TUPnPTable::Table());
		
		RStringF someParam = iPool.OpenFStringL(_L8("someParam"));
		CleanupClosePushL(someParam);
		RStringF someValue = iPool.OpenFStringL(_L8("some"));
		CleanupClosePushL(someValue);
		THTTPHdrVal nullVal;
		TPtrC8 retVal;
		THTTPHdrVal someVal(someValue);
		
		TRAP_IGNORE(iRHeaders.SetFieldL(man, nullVal));
		iRHeaders.GetRawField(man, retVal);
		iRHeaders.RemoveField(man);

		TRAP_IGNORE(iRHeaders.SetFieldL(man, nullVal));
		iRHeaders.GetRawField(man, retVal);
		iRHeaders.RemoveField(man);

		TRAP_IGNORE(iRHeaders.SetFieldL(soapaction, nullVal));
		iRHeaders.GetRawField(soapaction, retVal);
		iRHeaders.RemoveField(soapaction);

		TRAP_IGNORE(iRHeaders.SetFieldL(mx, nullVal));
		iRHeaders.GetRawField(mx, retVal);
		iRHeaders.RemoveField(mx);

		TRAP_IGNORE(iRHeaders.SetFieldL(st, nullVal));
		iRHeaders.GetRawField(st, retVal);
		iRHeaders.RemoveField(st);

		TRAP_IGNORE(iRHeaders.SetFieldL(timeout, nullVal));
		iRHeaders.GetRawField(timeout, retVal);
		iRHeaders.RemoveField(timeout);

		TRAP_IGNORE(iRHeaders.SetFieldL(callback, nullVal));
		iRHeaders.GetRawField(callback, retVal);
		iRHeaders.RemoveField(callback);

		TRAP_IGNORE(iRHeaders.SetFieldL(man, someVal, ns, nullVal));
		iRHeaders.GetRawField(man, retVal);
		iRHeaders.RemoveField(man);

		TRAP_IGNORE(iRHeaders.SetFieldL(man, someVal, someParam, nullVal));
		iRHeaders.GetRawField(man, retVal);
		iRHeaders.RemoveField(man);
		
		TRAP_IGNORE(iRHeaders.SetFieldL(man, someVal, someParam, someVal));
		iRHeaders.GetRawField(man, retVal);
		iRHeaders.RemoveField(man);

		CleanupStack::PopAndDestroy(&someValue);
		CleanupStack::PopAndDestroy(&someParam);
		}
	
	SetTestStepResult(EPass);	
	return TestStepResult();
	}


TVerdict CCodecEncodeTest::doTestStepPostambleL()
	{
	if(iRequest)
		delete iRequest;
	
	if(iResponse)
		delete iResponse;
	
	iPool.Close();
	
	if(iRespComposer)
		delete iRespComposer;
	
	if(iReqComposer)
		delete iReqComposer;

	delete iCodec;
	delete iSched;

	REComSession::FinalClose();
	return TestStepResult();
	}


void CCodecEncodeTest::ConstructHeadersL(const TInt aNoOfHdrValPairs)
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
		InetProtTextUtils::RemoveWhiteSpace(headerValPairPtr, InetProtTextUtils::ERemoveBoth);
		
		// The left over data is the value for the header.
		SetHeaderL(header, headerValPairPtr);
		CleanupStack::PopAndDestroy(headerValPairBuf);
						
		headersCounter++;
		}
	}


void CCodecEncodeTest::SetHeaderL(const TDesC8& aHeader, const TDesC8& aValue)
	{
	RStringF name = iPool.OpenFStringL(aHeader);
	CleanupClosePushL(name);
	THTTPHdrVal val;
	
	if((name == iPool.StringF(UPnP::EMX, TUPnPTable::Table())) || (name == iPool.StringF(UPnP::ESEQ, TUPnPTable::Table())) || (name == iPool.StringF(UPnP::ETimeout, TUPnPTable::Table())))
		{
		TInt value;
		InetProtTextUtils::ConvertDescriptorToInt(aValue, value);
		val.SetInt(value);	
		iRHeaders.SetFieldL(name, val);
		}
		
	else
		{
		RStringF value = iPool.OpenFStringL(aValue);
		CleanupClosePushL(value);
		
		val.SetStrF(value);
		iRHeaders.SetFieldL(name, value);

		CleanupStack::PopAndDestroy(&value);
		}
		
	if(name == iPool.StringF(UPnP::EMAN, TUPnPTable::Table()))
		{
		RStringF ns = iPool.StringF(UPnP::ENS, TUPnPTable::Table());
		TInt value = 01;
		THTTPHdrVal val(value);
		iRHeaders.SetParamL(name, ns, val, 0);
		}
	
	CleanupStack::PopAndDestroy(&name);
	}


void CCodecEncodeTest::GetFieldL()
	{
	TBuf<KMaxHeaderNameLen>  fieldName16;
	TBuf<KMaxHeaderValueLen> fieldVal16;
	THTTPHdrFieldIter iter = iRHeaders.Fields();
		
	while(iter.AtEnd() == EFalse)
		{
		RStringTokenF fldName = iter();
		RStringF headerSF = iPool.StringF(fldName);
		TPtrC8 rawData8;
		
		User::LeaveIfError(iRHeaders.GetRawField(headerSF, rawData8));

		const TDesC8& fieldNameDesC = headerSF.DesC();
		fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));
		fieldVal16.Copy(rawData8);
		
		INFO_PRINTF3(_L("\n===> %S: %S <==="), &fieldName16, &fieldVal16);
		++iter;	
		}
	}


void CCodecEncodeTest::MessageDataReadyL(RBuf8& /*aData*/)
	{
	User::Invariant();
	}


void CCodecEncodeTest::ComposingConcluded()
	{
	User::Invariant();
	}


void CCodecEncodeTest::ComposerError(TInt /*aError*/)
	{
	User::Invariant();
	}
