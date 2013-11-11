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
#include <ecom/ecom.h>
#include <http/thttptable.h>
#include <upnp/tupnptable.h>

#include "ccodecdecodetest.h"

_LIT8(KHeaderSeparator,	"\n");

_LIT8(KUpnpCodecName, "HTTP/UPnP");
_LIT(KTestName, "CODEC_TEST_1");


CCodecDecodeTest* CCodecDecodeTest::NewL()
	{
	return (new(ELeave)CCodecDecodeTest());
	}


CCodecDecodeTest::CCodecDecodeTest()
	{
	SetTestStepName(KCodecDecodeTest);
	}


CCodecDecodeTest::~CCodecDecodeTest()
	{
	}


TVerdict CCodecDecodeTest::doTestStepPreambleL()
	{
	iSched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iSched);

	iPool.OpenL(TUPnPTable::Table());
	iPool.OpenL(THTTPTable::Table());
	
	iCodec = NULL;
	iRequest = NULL;
	iResponse = NULL;
	iReqParser = NULL;
	iRespParser = NULL;

	return TestStepResult();
	}


TVerdict CCodecDecodeTest::doTestStepL()
	{
	TInt noOfHdrValPairs;
	TBool requestHeader;
	
	if(!GetIntFromConfig(ConfigSection(), KNoOfHdrValPairs, noOfHdrValPairs) ||
		!GetBoolFromConfig(ConfigSection(), KRequestHeader, requestHeader)
		)
		{
		ERR_PRINTF3(_L("===> Problem in reading values from ini.			\
				\nExpected fields are: \n%S\n%S <==="
			  ), &KNoOfHdrValPairs, &KRequestHeader);
		
		SetTestStepResult(EFail);
		return TestStepResult();
		}

  	if(requestHeader)
  		{
  		// has to load the server codec
  		iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iPool);
  		iRequest = CRequest::NewL(*iCodec, iPool);
  		iRHeaders = iRequest->Handle().GetHeaderCollection();
  		iReqParser = CUpnpRequestParser::NewL(*this);
  		}
  	else
  		{
  		// has to load the client codec
  		iCodec = CHeaderCodecPlugin::NewL(KUpnpCodecName, iPool);
  		iResponse = CResponse::NewL(*iCodec, iPool);
  		iRHeaders = iResponse->Handle().GetHeaderCollection();
  		iRespParser = CUpnpResponseParser::NewL(*this);
  		}

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
		
	// The follwoing section of code has been written for code coverage.
	if(ConfigSection().Compare(KTestName())== 0)
		{
		RStringF s = iPool.StringF(HTTP::EAccept, THTTPTable::Table());
		_LIT8(KSomeVal, "some");
		iRHeaders.SetRawFieldL(s, KSomeVal, KHeaderSeparator);
		THTTPHdrVal aV;
		iRHeaders.GetField(s,0,aV);
		iRHeaders.RemoveField(s);
		
		RStringF m = iPool.StringF(UPnP::EMAN, TUPnPTable::Table());
		iRHeaders.RemoveField(m);
		_LIT8(KSomeMan, "ns=01");
		iRHeaders.SetRawFieldL(m, KSomeMan(), KHeaderSeparator);
		THTTPHdrVal mV;
		iRHeaders.GetField(m, 0, mV);
		iRHeaders.RemoveField(m);
		
		_LIT8(KSomeMan2, "some;ns=01;=;;");
		iRHeaders.SetRawFieldL(m, KSomeMan2(), KHeaderSeparator);
		iRHeaders.GetField(m, 0, mV);
		iRHeaders.RemoveField(m);
		
		if(requestHeader)
			{
			RStringF accept = iPool.StringF(HTTP::EAccept, THTTPTable::Table());
			RStringF textHtml = iPool.StringF(HTTP::ETextHtml, THTTPTable::Table());
			RStringF textPlain = iPool.StringF(HTTP::ETextPlain, THTTPTable::Table());
			RStringF eQ = iPool.StringF(HTTP::EQ, THTTPTable::Table());
			THTTPHdrVal q(THTTPHdrVal::TQConv(0.8));
			
			RStringF expect = iPool.StringF(HTTP::EExpect, THTTPTable::Table());
			RStringF so = iPool.OpenFStringL(_L8("some"));
			CleanupClosePushL(so);
			iRHeaders.FieldPartsL(expect);
			THTTPHdrVal x;
			iRHeaders.GetParam(so, textHtml, x, 0);
			CleanupStack::PopAndDestroy(&so);
			TPtrC8 pt;
			iRHeaders.GetRawField(expect, pt);
			iRHeaders.RemoveField(expect);
			iRHeaders.RemoveField(expect);
			iRHeaders.RemoveFieldPart(expect, 0);
			
			iRHeaders.SetFieldL(accept, THTTPHdrVal(textHtml));
			iRHeaders.SetFieldL(accept, THTTPHdrVal(textPlain));
			iRHeaders.RemoveFieldPart(accept, 0);
			iRHeaders.RemoveFieldPart(accept, 0);
			iRHeaders.RemoveField(accept);
			
			TRAP(err, iRHeaders.SetFieldL(accept, textHtml, eQ, q));
			if(err != KErrNone)
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
				
			TRAP(err, iRHeaders.SetFieldL(accept, textPlain));	
			if(err != KErrNone)
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
				
			THTTPHdrFieldIter iter = iRHeaders.Fields();
			while(!iter.AtEnd())
				{
				iter();
				++iter;	
				}
				
			++iter;
			iter();	
				
			THTTPHdrVal q2;
			iRHeaders.GetParam(accept, eQ, q2, 0);
			if((q == q2))
				{
				if(q != q2)
					{
					// do nothing.	
					}
				iRHeaders.RemoveFieldPart(accept, 0);	
				}
				
			TRAP(err, iRHeaders.SetFieldL(accept, textHtml, eQ, q));
			if(err != KErrNone)
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
				iRHeaders.GetParam(accept, eQ, q2, 1);
	
			RString someString = iPool.OpenStringL(_L8("some"));
			CleanupClosePushL(someString);
			THTTPHdrVal v(someString);
			v.SetStr(someString);
			THTTPHdrVal t;
			t = v.Copy();
			someString = v.Str();
		
			THTTPHdrVal s(someString);
			CleanupStack::PopAndDestroy(&someString);
			if(s == v)
				{
				// do nothing.	
				}
				
			TDateTime date(2008, EJune, 24, 9, 0, 0, 0);
			TDateTime date2(2007, EJune, 23, 9, 0, 0, 0);
			THTTPHdrVal dVal(date);
			THTTPHdrVal dVal3(date2);
			dVal.SetDateTime(date);
			date = dVal.DateTime();
			THTTPHdrVal dVal2;
			dVal2 = dVal.Copy();
			if(dVal == dVal2)
				{
				// do nothing.	
				}
			if(dVal == dVal3)
				{
				// do nothing.
				}
			
			THTTPHdrVal strFVal, strFVal1;
			strFVal.SetStrF(eQ);
			strFVal1 = strFVal.Copy();
			if(strFVal == strFVal1)
				{
				// do nothing.				
				}
				
			THTTPHdrVal sVal;
			THTTPHdrVal sVal2;
			if(sVal == dVal)
				{
				// do nothing.			
				}
			if(dVal == sVal)
				{
				// do nothing.	
				}
			
			if(ConfigSection().Compare(KTestName()) == 0)
				{
				if(sVal == sVal2)
					{
					// do nothing.
					}
				}
			}
		}
	SetTestStepResult(EPass);	
	return TestStepResult();
	}


TVerdict CCodecDecodeTest::doTestStepPostambleL()
	{
	if(iRequest)
		delete iRequest;
	
	if(iResponse)
		delete iResponse;
	
	iPool.Close();
	
	if(iRespParser)
		delete iRespParser;
	
	if(iReqParser)
		delete iReqParser;

	delete iCodec;
	delete iSched;

	REComSession::FinalClose();
	
	return TestStepResult();
	}


void CCodecDecodeTest::ConstructHeadersL(const TInt aNoOfHdrValPairs)
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


void CCodecDecodeTest::SetHeaderL(const TDesC8& aHeader, const TDesC8& aValue)
	{
	RStringF name = iPool.OpenFStringL(aHeader);
	CleanupClosePushL(name);
	iRHeaders.SetRawFieldL(name, aValue, KHeaderSeparator);
	CleanupStack::PopAndDestroy(&name);
	}


void CCodecDecodeTest::GetFieldL()
	{
	TBuf<KMaxHeaderNameLen>  fieldName16;
	TBuf<KMaxHeaderValueLen> fieldVal16;
	THTTPHdrFieldIter iter = iRHeaders.Fields();

	while(iter.AtEnd() == EFalse)
		{
		RStringTokenF fldName = iter();
		RStringF headerSF = iPool.StringF(fldName);
		THTTPHdrVal hdrVal;
		
		TInt partsIter = 0;
		
		FOREVER
			{
			if(iRHeaders.GetField(headerSF, partsIter, hdrVal) == KErrNone)
				{
				const TDesC8& fieldNameDesC = headerSF.DesC();
				fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));
				
				switch(hdrVal.Type())
					{
					case THTTPHdrVal::KTIntVal:
						INFO_PRINTF3(_L("\n===> %S: %u <==="), &fieldName16, hdrVal.Int());
						break;
					case THTTPHdrVal::KStrFVal:
						const TDesC8& fieldValDesC = hdrVal.StrF().DesC();
						fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
						INFO_PRINTF3(_L("\n===> %S: %S <==="), &fieldName16, &fieldVal16);
						break;
					}
				}
			else
				{
				break;	
				}
			++partsIter;		
			}
		++iter;	
		}
	}


void CCodecDecodeTest::GotBodyData()
	{
	User::Invariant();
	}


void CCodecDecodeTest::DataParsed()
	{
	User::Invariant();	
	}


void CCodecDecodeTest::GotHeaders()
	{
	User::Invariant();	
	}


void CCodecDecodeTest::ParsingComplete(RMemChunk& /*aExcessData*/)
	{
	User::Invariant();	
	}


void CCodecDecodeTest::ParserError(TInt /*aError*/)
	{
	User::Invariant();	
	}
