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

#include <inetprottextutils.h>
#include <httperr.h>
#include <httpstringconstants.h>

#include "cupnprequestparser.h"
#include "tupnpmessagepanic.h"

_LIT8(KHeaderSeparator,	"\n");

__FLOG_STMT(_LIT8(KSubsys,"UPnPReqParser");)
__FLOG_STMT(_LIT8(KComponent,"UPnPMessage");)


EXPORT_C CUpnpRequestParser* CUpnpRequestParser::NewL(MParserObserver& aObserver)
	{
	CUpnpRequestParser* self = new(ELeave) CUpnpRequestParser(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUpnpRequestParser::CUpnpRequestParser(MParserObserver& aObserver)
	:iObserver(aObserver)
	{
	}

EXPORT_C CUpnpRequestParser::~CUpnpRequestParser()
	{
	iMessageParser.Close();
	iBodyParts.Close();
	if(!iMsgBuf.IsEmpty())
		{
		iMsgBuf.Free();	
		}
	iRawDataArray.Close();
	__FLOG(_L8("-> Request parser destroyed"));
	__FLOG_CLOSE;
	}

void CUpnpRequestParser::ConstructL()
	{
	iMessageParser.OpenL(*this);
	__FLOG_OPEN(KSubsys, KComponent);
	__FLOG(_L8("-> Request parser created"));
	}

TBool CUpnpRequestParser::GetNextDataPart(TPtrC8& aDataPart)
	{
	__FLOG(_L8("-> Supplying request body part"));
	
	TInt bodyPartsCount = iBodyParts.Count();
	__ASSERT_ALWAYS((bodyPartsCount > 0), TUPnPMessagePanic::Panic(TUPnPMessagePanic::ENoBodyPartInDataSupplier));
		
	// Provide the first chunk.
	aDataPart.Set(iBodyParts[0]);
	__FLOG_1(_L8("%S"), &aDataPart);
	
	return (BodyComplete() && bodyPartsCount == 1);
	}

void CUpnpRequestParser::ReleaseData()
	{
	__FLOG(_L8("-> Releasing request body part"));
	
	// Remove the oldest chunk.
	if( iBodyParts.Count () > 0 )
		{
		iBodyParts.Remove ( 0 ); 
		}
	
	// Are there any more chunks?
	if( iBodyParts.Count () > 0 )
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
	
TInt CUpnpRequestParser::OverallDataSize()
	{
	return (iOverallDataSize < 0) ? KErrNotFound : iOverallDataSize;
	}

TInt CUpnpRequestParser::Reset()
	{
	return KErrNotSupported;
	}

void CUpnpRequestParser::GetDataPacket(TPtrC8& aData)
	{
	__FLOG(_L8("-> Supplying request data to the HTTP message parser"));
	aData.Set(iRawDataArray[0]);
	iRawDataArray.Remove(0);
	}

void CUpnpRequestParser::ReleaseDataPacket()
	{
	__FLOG(_L8("-> Releasing request data"));
	if ( iBodyParts.Count () == 0 )
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

void CUpnpRequestParser::StartLineL(const TDesC8& aStartLine)
	{	
	// From RFC 2616 Sec 5.1 Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
	// Request-Line == Start-Line
	__FLOG(_L8("-> Got the request start line"));
	__FLOG_1(_L8("%S"), &aStartLine);
	
	TPtrC8 startLinePtr ( aStartLine );
	// Find the first space.
	TInt pos = startLinePtr.Locate ( ' ' );
	User::LeaveIfError ( pos );
	
	// Extract the method
	TPtrC8 method ( startLinePtr.Left ( pos ) );
	InetProtTextUtils::RemoveWhiteSpace ( method, InetProtTextUtils::ERemoveBoth );
		
	// Now move past the space that we located
	startLinePtr.Set ( startLinePtr.Mid ( pos + 1 ) );
		
	// Locate the next space.
	pos = startLinePtr.Locate ( ' ' );
	User::LeaveIfError ( pos );
	
	// Extract the request URI
	TPtrC8 requestUri ( startLinePtr.Left ( pos ) );
	InetProtTextUtils::RemoveWhiteSpace ( requestUri, InetProtTextUtils::ERemoveBoth );
	
	TUriParser8 uriParser;
	User::LeaveIfError ( uriParser.Parse ( requestUri ) );
	
	// Now move past the space that we located
	startLinePtr.Set ( startLinePtr.Mid ( pos + 1 ) );
	
	// Extract the version
	TPtrC8 versionPtr ( startLinePtr );
	if ( versionPtr.Length () == 0 )
		User::Leave ( KErrCorrupt );
	
	InetProtTextUtils::RemoveWhiteSpace ( requestUri, InetProtTextUtils::ERemoveBoth );


	RStringPool sp = iRequest->StringPool();

	pos = versionPtr.Locate('/');
	if( pos == KErrNotFound )
		User::Leave(KErrCorrupt);
	
	versionPtr.Set(versionPtr.Mid(pos + 1));

	// Extract the major version number
	TInt major;
	pos = InetProtTextUtils::ConvertDescriptorToInt(versionPtr, major);
	User::LeaveIfError(pos);
	
	// Skip past major version number and the "."
	versionPtr.Set(versionPtr.Mid(pos + 1));

	// Extract the minor version number
	TInt minor;
	pos = InetProtTextUtils::ConvertDescriptorToInt(versionPtr, minor);
	User::LeaveIfError(pos);

	RStringF methodStr = sp.OpenFStringL ( method );
	CleanupClosePushL(methodStr);
	
	RRequest request = iRequest->Handle ();
	request.SetMethod ( methodStr );
	request.SetURIL ( uriParser );
	
	TVersion version ( major, minor, 0);	
	request.SetVersion ( version );
	
	CleanupStack::PopAndDestroy(&methodStr);
	}

void CUpnpRequestParser::HeaderL(const TDesC8& aFieldName, TDesC8& aFieldValue)
	{
	__FLOG(_L8("-> Got header value pair"));
	__FLOG_2(_L8("%S: %S"), &aFieldName, &aFieldValue);
	
	RStringF name = iRequest->StringPool().OpenFStringL(aFieldName);
	CleanupClosePushL(name);
	
	iRequest->Handle().GetHeaderCollection().SetRawFieldL(name, aFieldValue, KHeaderSeparator);
	
	CleanupStack::PopAndDestroy(&name);
	}

TInt CUpnpRequestParser::BodySizeL()
	{
	// Notify the sender that all the request headers have been parsed.
	iObserver.GotHeaders();
	
	//First check for a Transfer-Encoding header field.
	iRequest->Handle().SetBody(*this);
	THTTPHdrVal value;
	RStringF name = iRequest->StringPool().StringF(HTTP::ETransferEncoding, THTTPTable::Table());

	if( iRequest->Handle().GetHeaderCollection().GetField(name, 0, value) == KErrNone )
		{
		// It exists - what's the value?
		if( value.Type() == THTTPHdrVal::KStrFVal &&
			value.StrF().Index(THTTPTable::Table()) == HTTP::EChunked )
			{
			// The Transfer-Encoding header is Chunked and as the chunked
			// encoding is removed, we remove the header.
			iRequest->Handle().GetHeaderCollection().RemoveField(name);
			
			__FLOG(_L8("-> Request has chunked body"));
			// As the entity body is chunked the overall data size is unknown.
			iOverallDataSize = MHttpMessageParserObserver::EChunked;
			}
		}

	else
		{
		// Transfer-Encoding header was not present - now check for a 
		// Content-Length header.
		name = iRequest->StringPool().StringF(HTTP::EContentLength, THTTPTable::Table());
		TInt err = iRequest->Handle().GetHeaderCollection().GetField(name, 0, value);
		
		if( err != KErrNone && err != KErrNotFound )
			User::Leave(err);
			
		if( err == KErrNone && value.Type() == THTTPHdrVal::KTIntVal )
			{
			// Content-Length header value specified the length of entity in bytes.
			iOverallDataSize = value.Int();
			__FLOG_1(_L8("-> Request body length = %d"), iOverallDataSize);
			}
		}
		
	return iOverallDataSize;	
	}

void CUpnpRequestParser::BodyChunkL(const TDesC8& aData)
	{
	__FLOG(_L8("-> Got request body chunk"));
	__FLOG_1(_L8("%S"), &aData);
	iFlags |= EBodyPresent;
	
	if (aData.Length() > 0)
		{
		iBodyParts.Append(aData);
		}

	// Only notify the client that there is a body part if there is one.
	if (iBodyParts.Count() > 0)
		{
		// Notify the sender about the presence of the body. He can then call
		// MHTTPDataSupplier::GetNextDataPart() to get the body data.
		iObserver.GotBodyData();
		}
	}

void CUpnpRequestParser::BodyCompleteL()
	{
	__FLOG(_L8("-> Request body complete"));
	iFlags |= EBodyComplete;
	}

void CUpnpRequestParser::MessageCompleteL(const TPtrC8& aExcessData)
	{
	__FLOG(_L8("-> Message complete"));
	
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
	
	// Post a message to the sender with aExcessData as the payload
	TInt excessDataLen = aExcessData.Length();
	TInt count = iRawDataArray.Count();
	while(count > 0)
		{
		excessDataLen += iRawDataArray[count-1].Length();
		count--;
		}	
	
	TInt lenToTrim = iMsgBuf.Length() - excessDataLen;
	iMsgBuf.TrimStart(lenToTrim);	
	iObserver.ParsingComplete(iMsgBuf);
	}

TInt CUpnpRequestParser::HandleParserError(TInt aError)
	{
	__FLOG_1(_L8("-> HTTP message parser received error: %d"), aError);
	iRawDataArray.Reset();
	iBodyParts.Reset();
	iObserver.ParserError(aError);
	return KErrNone;
	}

EXPORT_C void CUpnpRequestParser::ParseRequest(RMemChunk& aMessage, CRequest* aRequest)
	{
	__FLOG(_L8("-> Parsing request"));
	__ASSERT_DEBUG(aRequest, TUPnPMessagePanic::Panic(TUPnPMessagePanic::EMissingRequest));
	__ASSERT_DEBUG(!aMessage.IsEmpty(), TUPnPMessagePanic::Panic(TUPnPMessagePanic::ENoMessage));

	iRequest = aRequest;
	if(!iMsgBuf.IsEmpty())
		{
		iMsgBuf.Init();	
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
		// Initiate the message parser.
		iMessageParser.ReceivedMessageData();
		}
	}

EXPORT_C void CUpnpRequestParser::ResetParser()
	{
	__FLOG(_L8("-> Request parser reset"));
	iMsgBuf.Init();
	iMessageParser.Reset();
	iRawDataArray.Reset();
	iBodyParts.Reset();
	iOverallDataSize = 0;
	iFlags &= ~EExcessData;
	}
