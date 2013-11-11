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

#ifndef __CUPNPRESPONSEPARSER_H_
#define __CUPNPRESPONSEPARSER_H_

// System includes
#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/framework/cheadercodec.h>
#include <comms-infras/commsdebugutility.h>
#include <http/thttptable.h>
#include <rmemchunk.h>

// User includes
#include "mhttpmessageparserobserver.h"
#include "CResponse.h"
#include "rhttpmessageparser.h"
#include "mparserobserver.h"


class CUpnpResponseParser: public CBase,
							public MHttpMessageParserObserver,
							public MHTTPDataSupplier
	{
public:
	IMPORT_C static CUpnpResponseParser* NewL(MParserObserver& aObserver);
	IMPORT_C virtual ~CUpnpResponseParser();
	IMPORT_C void ParseResponse(RMemChunk& aMessage, CResponse* aResponse);
	IMPORT_C void ResetParser();
	inline CResponse& Response();
	
private: 
	// methods from MHTTPDataSupplier
	TBool GetNextDataPart(TPtrC8& aDataPart);
	void ReleaseData();
	TInt OverallDataSize();
	TInt Reset();

	// methods from MHttpMessageParserObserver
	void GetDataPacket(TPtrC8& aData);
	void ReleaseDataPacket();
	void StartLineL(const TDesC8& aStartLine);
	void HeaderL(const TDesC8& aFieldName, TDesC8& aFieldValue);
	TInt BodySizeL();
	void BodyChunkL(const TDesC8& aData);
	void BodyCompleteL();
	void MessageCompleteL(const TPtrC8& aExcessData);
	TInt HandleParserError(TInt aError);
	
private:
	CUpnpResponseParser(MParserObserver& aObserver);
	void ConstructL();

	void ResetData();
	inline TBool ConsumingResponse();
	inline TBool BodyComplete();
	inline TBool MessageComplete();
	inline TBool GotTrailers();
	inline TBool NotifyReleaseData();
	inline TBool IsExcessData();
    
private:

	enum THttpResponseFlags
		{
		EConsumingResponse		= 0x01,
		EBodyComplete			= 0x02,
		EMessageComplete		= 0x04,
	    ENotifyReleaseData		= 0x10,
		EBodyPresent			= 0x20,
		EExcessData				= 0x40		
		};

private:
	CResponse* 				iResponse;
	RHttpMessageParser		iMessageParser;
	TInt					iFlags;
	TInt					iOverallDataSize;
	MParserObserver& 		iObserver;
	RArray<TPtrC8>			iBodyParts;	
	RMemChunk				iMsgBuf;
	RArray<TPtrC8>			iRawDataArray;
	
	__FLOG_DECLARATION_MEMBER;
	};
	
inline TBool CUpnpResponseParser::ConsumingResponse()
	{
	return iFlags & EConsumingResponse;
	}

inline TBool CUpnpResponseParser::BodyComplete()
	{
	return iFlags & EBodyComplete;
	}

inline TBool CUpnpResponseParser::MessageComplete()
	{
	return iFlags & EMessageComplete;
	}

inline TBool CUpnpResponseParser::NotifyReleaseData()
	{
	return iFlags & ENotifyReleaseData;
	}
		
inline CResponse& CUpnpResponseParser::Response()
	{
	return (*iResponse);
	}
	
inline TBool CUpnpResponseParser::IsExcessData()
	{
	return iFlags & EExcessData;	
	}

#endif /*CUPNPRESPONSEPARSER_H_*/
