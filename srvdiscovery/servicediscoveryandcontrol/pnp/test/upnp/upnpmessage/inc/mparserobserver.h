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

#ifndef __MPARSEROBSERVER_H_
#define __MPARSEROBSERVER_H_

#include <e32std.h>
//#include <es_mbuf.h>
#include <rmemchunk.h>
//#include <comms-infras/mbufchain.h>

/**
The MParserObserver class is the observer API for the upnp request and response parser.
It allows the upnp request and response parser to notify its observer when it has found the
headers, bodydata etc.
*/
class MParserObserver
	{
public:
/**
	The request/response headers have been found. The observer can gain access to the headers
	through the RHTTPHeaders class.
*/	
	virtual void GotHeaders() = 0;
	
/**
	The body data has been found. The observer can gain access to the body through 
	the MHTTPDataSupplier class.
*/
	virtual void GotBodyData() = 0;
	
/**
	The parser has parsed the supplied data and is waiting for more data from the observer.
	This API does not indicate the end of the complete request or response parsing. It only
	indicates that the data supplied is insufficient and more data is required for complete
	parsing.
*/	
	virtual void DataParsed() = 0;
	
/**
	The parser has completed the parsing of a single request/response. Any exess data will be
	supplied to the observer through the aExcessData. This data could be the start of the next 
	message.
	@param aExcessData The excess data within the request/response if any.
*/	
	virtual void ParsingComplete(RMemChunk& aExcessData) = 0;
	
/**
	Error notifier. The parser has experienced an error.
	@param aError The error code.Possible error codes are:
	KErrNoMemory - When there is insufficient memory for allocations,
	KErrCorrupt - If the protocol in the response is not HTTP.
    KErrNotFound - When the Locate() and Find() APIs of TDesC8 fail.
*/	
	virtual void ParserError(TInt aError) = 0;
	};

#endif /*MPARSEROBSERVER_H_*/
