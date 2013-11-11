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

#ifndef __CUPNPREQUESTPARSER_H_
#define __CUPNPREQUESTPARSER_H_

// System includes
#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/framework/cheadercodec.h>
#include <comms-infras/commsdebugutility.h>
#include <http/thttptable.h>
#include <rmemchunk.h>
#include <rmemcell.h>

// User includes
#include "mhttpmessageparserobserver.h"
#include "CRequest.h"
#include "rhttpmessageparser.h"
#include "mparserobserver.h"


class CUpnpRequestParser: public CBase,
							public MHttpMessageParserObserver,
							public MHTTPDataSupplier
	{
public:
	IMPORT_C static CUpnpRequestParser* NewL(MParserObserver& aObserver);
	IMPORT_C virtual ~CUpnpRequestParser();
	IMPORT_C void ParseRequest(RMemChunk& aMessage, CRequest* aRequest);
	IMPORT_C void ResetParser();
	inline void DataFailed ();
	
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
	CUpnpRequestParser(MParserObserver& aObserver);
	void ConstructL();
	
	inline TBool BodyComplete();
	inline TBool MessageComplete();
	inline TBool NotifyReleaseData();
    inline TBool IsExcessData();

private:

	enum THttpResponseFlags
		{
		EBodyComplete			= 0x02,
	    ENotifyReleaseData		= 0x10,
		EBodyPresent			= 0x20,
		EExcessData				= 0x40,
		};

private:
	CRequest* 				iRequest;
	RHttpMessageParser		iMessageParser;
	TInt					iFlags;
	TInt					iOverallDataSize;
	MParserObserver& 		iObserver;
	RArray<TPtrC8>			iBodyParts;	
	RMemChunk				iMsgBuf;
	RArray<TPtrC8>			iRawDataArray;
	
	__FLOG_DECLARATION_MEMBER;
	};
	
inline TBool CUpnpRequestParser::BodyComplete()
	{
	return iFlags & EBodyComplete;
	}

inline TBool CUpnpRequestParser::NotifyReleaseData()
	{
	return iFlags & ENotifyReleaseData;
	}

inline TBool CUpnpRequestParser::IsExcessData()
 	{
 	return iFlags & EExcessData;
 	}
 	
// Free up corrupted UDP data packet
void CUpnpRequestParser::DataFailed ()
	{
	if ( !iMsgBuf.IsEmpty() )
		{
		iMsgBuf.Free ();
		}
	}
		
#endif /*CUPNPREQUESTPARSER_H_*/
