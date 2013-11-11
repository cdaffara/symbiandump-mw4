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
#include <inetprottextutils.h>
#include <httperr.h>
#include <rmemcell.h>

#include "cupnpresponseparser.h"
#include "tupnpmessagepanic.h"

_LIT8(KHeaderSeparator,	"\n");

__FLOG_STMT(_LIT8(KSubsys,"UPnPResParser");)
__FLOG_STMT(_LIT8(KComponent,"UPnPMessage");)

_LIT8(KHTTP, "HTTP");

EXPORT_C CUpnpResponseParser* CUpnpResponseParser::NewL(MParserObserver& aObserver)
	{
	CUpnpResponseParser* self = new(ELeave) CUpnpResponseParser(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUpnpResponseParser::CUpnpResponseParser(MParserObserver& aObserver)
	:iObserver(aObserver)
	{
	}

EXPORT_C CUpnpResponseParser::~CUpnpResponseParser()
	{
	iMessageParser.Close();
	iBodyParts.Close();
	if(!iMsgBuf.IsEmpty())
		{
		iMsgBuf.Free();	
		}
	iRawDataArray.Close();
	__FLOG(_L8("-> Response parser destroyed"));
	__FLOG_CLOSE;
	}

void CUpnpResponseParser::ConstructL()
	{
	iMessageParser.OpenL(*this);
	__FLOG_OPEN(KSubsys, KComponent);
	__FLOG(_L8("-> Response parser created"));
	}

void CUpnpResponseParser::ResetData()
	{
	iMessageParser.Reset();
	iBodyParts.Reset();
	iOverallDataSize = 0;
	iFlags = 0;
	}

TBool CUpnpResponseParser::GetNextDataPart(TPtrC8& aDataPart)
	{
	__FLOG(_L8("-> Supplying response body part"));
	
	TInt bodyPartsCount = iBodyParts.Count();
	__ASSERT_ALWAYS((bodyPartsCount > 0), TUPnPMessagePanic::Panic(TUPnPMessagePanic::ENoBodyPartInDataSupplier));
	
	// Provide the first chunk.
	aDataPart.Set(iBodyParts[0]);
	__FLOG_1(_L8("%S"), &aDataPart);
	
	return (BodyComplete() && bodyPartsCount == 1);
	}

void CUpnpResponseParser::ReleaseData()
	{
	__FLOG(_L8("-> Releasing response body part"));
	
	// Remove the oldest chunk.
	if( iBodyParts.Count() > 0 )
		{
		iBodyParts.Remove(0);
		}
	
	// Are there any more chunks?
	if(iBodyParts.Count() > 0)	
		{
		// Notify the sender about the presence of the body. He can then call
		// MHTTPDataSupplier::GetNextDataPart() to get the body data.
		iObserver.GotBodyData();
		}

	else
		{
		// Does this data packet need to be released?
		if( NotifyReleaseData() )
			{
			// Notify sender that the current data is no longer needed.
			// Can release as there are no body chunks waiting to be passed to the
			// client.
			iObserver.DataParsed();
			iFlags &= ~ENotifyReleaseData;
			}
		}
	}
	
TInt CUpnpResponseParser::OverallDataSize()
	{
	return (iOverallDataSize < 0) ? KErrNotFound : iOverallDataSize;
	}

TInt CUpnpResponseParser::Reset()
	{
	return KErrNotSupported;
	}

void CUpnpResponseParser::GetDataPacket(TPtrC8& aData)
	{
	__FLOG(_L8("-> Supplying response data to the HTTP message parser"));
	aData.Set(iRawDataArray[0]);
	iRawDataArray.Remove(0);
	}

void CUpnpResponseParser::ReleaseDataPacket()
	{
	__FLOG(_L8("-> Releasing response data"));
	
	if( ConsumingResponse() && MessageComplete() )
		{
		// Ok, the response was a 1xx message which has been consumed. Reset the
		// this object and continue parsing the next part of the response.
		ResetData();
		iMessageParser.ReceivedMessageData();		 
		}
	else if(iBodyParts.Count() == 0)	
		{
		if(iRawDataArray.Count() > 0 && !IsExcessData())
			{
			iMessageParser.ReceivedMessageData();
			}
		else
			{
			// Notify sender that the current data is no longer needed.
			// Can release as there are no body chunks waiting to be passed to the
			// client.
			iObserver.DataParsed();
			}
		}
	else
		{
		// Flag that the data needs to be released
		iFlags |= ENotifyReleaseData;
		}
	}

void CUpnpResponseParser::StartLineL(const TDesC8& aStartLine)
	{
	// The RFC2616 defines the Status-Line as follows - 
	// 
	// Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
	//
	// First extract the HTTP-Version
	__FLOG(_L8("-> Got the reponse start line"));
	__FLOG_1(_L8("%S"), &aStartLine);
	
	TPtrC8 data = aStartLine;
	User::LeaveIfError(InetProtTextUtils::RemoveWhiteSpace(data, InetProtTextUtils::ERemoveLeft));
	
	// HTTP-Version = "HTTP" "/" 1*DIGIT "." 1*DIGIT
	TInt consumed = data.Locate('/');
	User::LeaveIfError(consumed);
	
	if((consumed > KHTTP().Length()) || (consumed < KHTTP().Length()))
		User::Leave(KErrCorrupt);
	
	TPtrC8 http = data.Left(consumed);
	if( http.CompareF(iResponse->StringPool().StringF(HTTP::EHTTP, THTTPTable::Table()).DesC()) != 0 )
		User::Leave(KErrCorrupt);
		
	// Skip past the HTTP and "/"
	data.Set(data.Mid(consumed + 1));
	
	// Extract the major version number
	TInt major;
	consumed = InetProtTextUtils::ConvertDescriptorToInt(data, major);
	User::LeaveIfError(consumed);
	
	// Skip past major version number and the "."
	data.Set(data.Mid(consumed + 1));

	// Extract the minor version number
	TInt minor;
	consumed = InetProtTextUtils::ConvertDescriptorToInt(data, minor);
	User::LeaveIfError(consumed);

	// Skip past minor version number and the SP
	data.Set(data.Mid(consumed + 1));
	
	// Clear any extra surrounding whitespace
	User::LeaveIfError(InetProtTextUtils::RemoveWhiteSpace(data, InetProtTextUtils::ERemoveBoth));
	
	// Extract the status code
	TInt status;
	consumed = InetProtTextUtils::ConvertDescriptorToInt(data, status);
	User::LeaveIfError(consumed);
	
	if( data.Length() > consumed )
		{
		// Skip past status code and the SP
		data.Set(data.Mid(consumed + 1));
	
		// Remaining data is the status reason - trim any leading whitespace as right side already trimmed
		User::LeaveIfError(InetProtTextUtils::RemoveWhiteSpace(data, InetProtTextUtils::ERemoveLeft));
		}
	else
		{
		// No reason phrase following the status code
		data.Set(KNullDesC8());
		}
	// Populate the Status-Line info
	RStringF reason = iResponse->StringPool().OpenFStringL(data);
	
	RResponse response = iResponse->Handle();

	TVersion version(major, minor, 0);
	response.SetVersion(version);
	response.SetStatusCode(status);
	response.SetStatusText(reason);
	reason.Close();
	
	// Check to see if a body is expected
	if( HTTPStatus::IsInformational(status) )
		{
		// 1xx status - no body and need to consume this response
		iFlags |= EConsumingResponse;
		}
	}

void CUpnpResponseParser::HeaderL(const TDesC8& aFieldName, TDesC8& aFieldValue)
	{
	// Only set the headers if we are not consuming the response.
	__FLOG(_L8("-> Got header value pair"));
	__FLOG_2(_L8("%S: %S"), &aFieldName, &aFieldValue);
	
	if( !ConsumingResponse() )
		{
		RStringF name = iResponse->StringPool().OpenFStringL(aFieldName);
		CleanupClosePushL(name);
		
		iResponse->Handle().GetHeaderCollection().SetRawFieldL(name, aFieldValue, KHeaderSeparator);
		
		CleanupStack::PopAndDestroy(&name);
		}
	}

TInt CUpnpResponseParser::BodySizeL()
	{
	if( !ConsumingResponse() )
		{
		// Notify the sender that all the response headers have been parsed.
		iObserver.GotHeaders();
		}

	// Check for a body...
	if( ConsumingResponse() ||	// this implies that the status code was 1xx - no body
		iResponse->Handle().StatusCode() == 204 || 
		iResponse->Handle().StatusCode() == 304 && 
		// if 2xx response...
		( HTTPStatus::IsSuccessful(iResponse->Handle().StatusCode()) ) )
		{
		// No entity body is expected as specified in RFC2616 section 4.4.
		iOverallDataSize = MHttpMessageParserObserver::ENoBody;
		iFlags |= EBodyComplete;
		__FLOG(_L8("-> Response has no body"));
		return iOverallDataSize;
		}
		
	// A body is expected - find the length. First check for a Transfer-Encoding
	// header field.
	iResponse->Handle().SetBody(*this);
	THTTPHdrVal value;
	RStringF name = iResponse->StringPool().StringF(HTTP::ETransferEncoding, THTTPTable::Table());
	
	TInt err = iResponse->Handle().GetHeaderCollection().GetField(name, 0, value);
	if( err != KErrNone && err != KErrNotFound )
		User::Leave(err);

	// It exists - what's the value?
	if( err == KErrNone && value.Type() == THTTPHdrVal::KStrFVal &&
		value.StrF().Index(THTTPTable::Table()) == HTTP::EChunked )
		{
		// The Transfer-Encoding header is Chunked and as the chunked
		// encoding is removed, we remove the header.
		iResponse->Handle().GetHeaderCollection().RemoveField(name);
			
		// As the entity body is chunked the overall data size is unknown.
		iOverallDataSize = MHttpMessageParserObserver::EChunked;
		__FLOG(_L8("-> Response has chunked body"));
		return iOverallDataSize;			
		}

	// Either no Transfer-Encoding header was present - now check for a 
	// Content-Length header.
	err = KErrNone;
	name = iResponse->StringPool().StringF(HTTP::EContentLength, THTTPTable::Table());
	err = iResponse->Handle().GetHeaderCollection().GetField(name, 0, value);
	
	if( err != KErrNone && err != KErrNotFound )
		User::Leave(err);
		
	if( err == KErrNone && value.Type() == THTTPHdrVal::KTIntVal )
		{
		// Content-Length header value specified the length of entity in bytes.
		iOverallDataSize = value.Int();
		__FLOG_1(_L8("-> Response body length = %d"), iOverallDataSize);
		return iOverallDataSize;
		}
		
	// There was no Content-Length header either, so the server will signal the
	// end of the message by closing the connection - overall data size unknown.
	iOverallDataSize = MHttpMessageParserObserver::EUnknown;
	__FLOG(_L8("-> Response body size is unknown"));
	return iOverallDataSize;
	}

void CUpnpResponseParser::BodyChunkL(const TDesC8& aData)
	{
	__FLOG(_L8("-> Got response body chunk"));
	__FLOG_1(_L8("%S"), &aData);

	iFlags |= EBodyPresent;
	if (aData.Length() > 0)
		{
		iBodyParts.Append(aData);
		}

	// Only notify the client that there is a body part if there is one.
	if(iBodyParts.Count() > 0)
		{
		// Notify the sender about the presence of the body. He can then call
		// MHTTPDataSupplier::GetNextDataPart() to get the body data.
		iObserver.GotBodyData();
		}
	}

void CUpnpResponseParser::BodyCompleteL()
	{
	__FLOG(_L8("-> Response body complete"));
	iFlags |= EBodyComplete;
	}

void CUpnpResponseParser::MessageCompleteL(const TPtrC8& aExcessData)
	{
	__FLOG(_L8("-> Message complete"));
	
	iFlags |= EMessageComplete;

	if ( aExcessData.Length() > 0 || iRawDataArray.Count() > 0 )
		{
		__FLOG(_L8("->Excess Data:"));
		__FLOG_1(_L8("%S"), &aExcessData);

		iFlags |= EExcessData;
		}
	else
		{
		__FLOG(_L8("-> No excess data"));
		}
	
	TInt excessDataLen = aExcessData.Length();
	TInt count = iRawDataArray.Count();
	while(count > 0)
		{
		excessDataLen += iRawDataArray[count-1].Length();
		count--;
		}
	TInt lenToTrim = iMsgBuf.Length() - excessDataLen;
	iMsgBuf.TrimStart(lenToTrim);

	if( !ConsumingResponse() )
		{
		iObserver.ParsingComplete(iMsgBuf);
		}
	else
		{
		// There could be excess data - this data is for this response and needs
		// to be parsed. Set it as the raw data.
		iRawDataArray.Reset();
		TPtrC8 rMBufPtr;
		RMemCell* rMBuf;
		TMemCellIterator mBufIter(iMsgBuf);
		while((rMBuf = mBufIter++) != NULL)
			{
			rMBufPtr.Set(rMBuf->Ptr(), rMBuf->Length());
			iRawDataArray.AppendL(rMBufPtr);
			}
		}
	}

TInt CUpnpResponseParser::HandleParserError(TInt aError)
	{
	__FLOG_1(_L8("-> HTTP message parser received error: %d"), aError);
	iRawDataArray.Reset();
	iBodyParts.Reset();
	iObserver.ParserError(aError);
	return KErrNone;
	}

EXPORT_C void CUpnpResponseParser::ParseResponse(RMemChunk& aMessage, CResponse* aResponse)
	{
	__FLOG(_L8("-> Parsing response"));
	__ASSERT_DEBUG(aResponse, TUPnPMessagePanic::Panic(TUPnPMessagePanic::EMissingResponse));
	__ASSERT_DEBUG(!aMessage.IsEmpty(), TUPnPMessagePanic::Panic(TUPnPMessagePanic::ENoMessage));
	
	iResponse = aResponse;
	if(!iMsgBuf.IsEmpty())
		{
		iMsgBuf.Free();	
		}
	iMsgBuf.Assign(aMessage);
	TPtrC8 rMBufPtr;
	RMemCell* rMBuf;
	TMemCellIterator mBufIter(iMsgBuf);
	TInt err = KErrNone;
	while((rMBuf = mBufIter++) != NULL)
		{
		rMBufPtr.Set(rMBuf->Ptr(), rMBuf->Length());
		err = iRawDataArray.Append(rMBufPtr);
		if(err != KErrNone)
			{
			HandleParserError(err);
			break;
			}
		}
	
	if(err == KErrNone)
		{
		// Initiate the message parser
		iMessageParser.ReceivedMessageData();
		}
	}
 
EXPORT_C void CUpnpResponseParser::ResetParser()
	{
	__FLOG(_L8("-> Response parser reset"));
	iMessageParser.Reset();
	iRawDataArray.Reset();
	iBodyParts.Reset();
	iOverallDataSize = 0;
	iFlags &= ~EExcessData;
	}
