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
// httpudpflow.h
// @file
// @internalComponent
// 
//

#ifndef __HTTPMUDPFLOW_H_
#define __HTTPMUDPFLOW_H_

//System Includes
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_protflow.h>
#include<in_sock.h>
#include<es_sock.h>
#include <rmemchunk.h>

#include "upnpflowbase.h"
#include "cupnprequestparser.h"
#include "upnp_cf_msgs.h"
#include "httpudpflow.h"
#include "ctimermappeduri.h"

class CCallbackArgument;
NONSHARABLE_CLASS ( CRecvElement ) : public CBase
	{
	public:

	CRecvElement ( RMemChunk& aData, const TAppProtAddr& aAddr )
	: iData ( aData ),
	iAddr ( aAddr )
	{
	}

	~CRecvElement ()
		{
		if ( !iData.IsEmpty () )
			{
			iData.Free ();
			}
		}
	const TAppProtAddr& Address () const
		{
		return iAddr;
		}

	RMemChunk& Data ()
		{
		return 	iData;
		}

	private:
	RMemChunk 		iData;
	TAppProtAddr	iAddr;
	};

typedef RArray < CRecvElement* > CRecvElements;

NONSHARABLE_CLASS ( CHttpMUdpFlow ) : public CUPnPFlowBase,
						 			 public MParserObserver
	{
	friend class CUPnPFlowFactory;

	public:
	static CHttpMUdpFlow* NewL ( CSubConnectionFlowFactoryBase& aFactory,
							  CUPnPProtocolIntfBase* aProtocolIntf,
						  		const TNodeId& aSubConnId );
	~CHttpMUdpFlow ();

	private:
	CHttpMUdpFlow ( CSubConnectionFlowFactoryBase& aFactory,
					CUPnPProtocolIntfBase* aProtocolIntf,
					const TNodeId& aSubConnId );

	void ConstructL ();
	protected:

	// From CSubConnectionFlowBase
	void ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage );

	// From MParserObserver
	virtual void GotHeaders ();
	inline virtual void GotBodyData ();
	virtual void DataParsed();
	virtual void ParsingComplete ( RMemChunk& aExcessData );
	virtual void ParserError ( TInt aError );

	void StartFlow ( TNodeCtxId aSender );
	void StopFlow ( TNodeCtxId aSender );
	void ParseIfOneRecvElement ();
	void ParseElement ();
	void RemoveFirstRecvElement ();

	TInt ValidateRequest ();
	TInt ValidateNotifyRequest ();
	TInt ValidateMSearchRequest ();

	// This should be moved
	TInt GetHeaderValue ( const CRequest& aResponse, TInt aFieldIndex, THTTPHdrVal& aFieldVal, const TStringTable& aTable ) const;
	TInt GetParamValue ( const CRequest& aResponse, TInt aFieldIndex, TInt aParamIndex, THTTPHdrVal& aParamVal, const TStringTable& aTable ) const;
	TBool IsHeaderPresent ( const CRequest& aResponse, TInt aFieldIndex, const TStringTable& aTable ) const;
	TBool IsValidCacheControlHeader ( const CRequest& aResponse ) const;
	TBool IsValidManHeader ( const CRequest& aResponse ) const;
	TBool IsValidHostHeader ( CRequest& aRequest ) const;

	void CreateMulticastListener ();

	void NotifyClients ();
	void HandleNotifyRequestL ();
	static TInt HandleMSearchRequestL (  TAny* aPtr );
	void RegisterL ( TNodeCtxId aChannelId, RMemChunk& aST, CSearchTargetArray& aArray );
	void Unregister ( TNodeCtxId aChannelId );
	void ReadRequestValues ( RStringF& aLocation, TInt& aMaxAge );
	void ReadRequestValues ( RStringF& aSt, RStringF& aUsn, RStringF& aNts);
	void ReadRequestValues ( TInt& aMaxAge, RStringF& aSt );
private:
	void CreateTimerEntryL( );
	TInt AdjustedDelay( TInt aTime );
	inline RPointerArray<CCallbackArgument>& GetArgumentArray();
	inline RPointerArray<CTimerMappedUri>& GetTimerArray();
	inline CSearchTargetArray& GetSearchTargetArray();
private:
	CUpnpRequestParser* 	iRequestParser;

	CHeaderCodec* 			iCodec;

	CSearchTargetArray		iSearchTargetArray;
	CSearchTargetArray		iNotifyTargetArray;
	RArray <TNodeCtxId>		iListenRequestors; // This array will be used untill the flow binds to
													// UDP listener.
	RMBufChain				iReceivedData;
	CRequest*				iUPnPRequest; // Used for parsing the request
	CRecvElements			iRecvElements;
	TNodeId					iAppProtIntfId;

	TBool					iIsStarted;
	TBool					iIsLeaving;
	TNodeCtxId				iLastLeavingClient;
	RStringPool				iStringPool;
	CDeltaTimer* 			iDeltaTimer;
	RPointerArray<CTimerMappedUri> 		iTimerMappedArray;
	RPointerArray<CCallbackArgument> 	iCallbackArray;
	};

void CHttpMUdpFlow::GotBodyData ()
	{
	ASSERT (0);
	}
RPointerArray<CCallbackArgument>& CHttpMUdpFlow::GetArgumentArray()
	{
	return iCallbackArray;
	}
RPointerArray<CTimerMappedUri>& CHttpMUdpFlow::GetTimerArray()
	{
	return iTimerMappedArray;
	}
CSearchTargetArray& CHttpMUdpFlow::GetSearchTargetArray()
	{
	return iSearchTargetArray;
	}

/*
  This class holds the *This pointer of the object calling it so that the static
  methods being called on expiry of CDeltaTimer can operate on the attributes of 
  class
 
  @internalComponent
 */
class CCallbackArgument: public CBase
	{
public:
	static CCallbackArgument* NewL(TAny* aThis ,const TAppProtAddr& aAddress , const CSearchTarget* aStTarget);
	inline ~CCallbackArgument();
	inline TAny* GetThisPointer() const;
	inline const TAppProtAddr& GetAddress() const;
	inline const CSearchTarget* SearchTarget() const ;
	inline void SetTimerEntry( TDeltaTimerEntry* aEntry);
	inline const TDeltaTimerEntry* GetTimerEntry() const;
private:
	inline CCallbackArgument (TAny* aThis , const TAppProtAddr& aAddress , const CSearchTarget* aStTarget);
		
	TAny* iThisPointer;
	TAppProtAddr iAddress;
	const CSearchTarget* iSearchTarget;
	TDeltaTimerEntry* iTimerEntry;
	};

/* Returns the pointer to the object it contains. */
TAny* CCallbackArgument::GetThisPointer() const
	{
	return iThisPointer;
	}

const TAppProtAddr& CCallbackArgument::GetAddress() const
	{
	return iAddress;
	}

const CSearchTarget* CCallbackArgument::SearchTarget() const
	{
	return iSearchTarget;
	}

void CCallbackArgument::SetTimerEntry( TDeltaTimerEntry* aEntry)
	{
	iTimerEntry = aEntry;
	}
const TDeltaTimerEntry* CCallbackArgument::GetTimerEntry() const
	{
	return iTimerEntry;
	}
CCallbackArgument::CCallbackArgument (TAny* aThis , const TAppProtAddr& aAddress , const CSearchTarget* aStTarget):iThisPointer(aThis)
		,iAddress( aAddress ), iSearchTarget( aStTarget )
	{
	
	}

CCallbackArgument::~CCallbackArgument()
	{
	}
#endif // __HTTPMUDPFLOW_H
