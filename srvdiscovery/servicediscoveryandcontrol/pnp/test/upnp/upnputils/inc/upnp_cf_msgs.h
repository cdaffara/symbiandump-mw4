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
// UPnP-proprietary messages
// //Pls.Pls..In Product Delivery before using these Messages check existing Comms Message
// //support in ss_nodemessages.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __UPNPCFMSGS_H
#define __UPNPCFMSGS_H

#include <comms-infras/ss_nodemessages.h>
//#include <es_mbuf.h>
//#include <comms-infras/ss_fact2.h>
#include <appprotintf/app_protintf_msgs.h>
//#include <cchunkmanager.h>
#include <rmemchunk.h>

#include "ctransaction.h"

using namespace ESock;
using namespace Messages;

const TInt KUpnpMessagesImplementationUid = 0x2000D060;


struct TSsdpInfo
	{

	TSsdpInfo ()
	{
	}

	TSsdpInfo ( RMemChunk& aSearchTarget )
	: iSearchTarget ( aSearchTarget )
	{
	}

	TSsdpInfo ( TInt aMx, RMemChunk& aSearchTarget )
	: iMx ( aMx ),
	iSearchTarget ( aSearchTarget )
	{
	}

	TSsdpInfo ( RMemChunk& aSearchTarget, const TAppProtAddr& aAddr )
	:iSearchTarget ( aSearchTarget ),
	iAddr ( aAddr )
	{
	}

	TSsdpInfo ( TInt aMx, RMemChunk& aLocation, RMemChunk& aUsn, RMemChunk& aSearchTarget )
	: iMx ( aMx ),
	iLocation ( aLocation ),
	iUsn ( aUsn ),
	iSearchTarget ( aSearchTarget )
	{
	}

	TSsdpInfo ( RMemChunk& aUsn, RMemChunk& aSearchTarget )
	: iUsn ( aUsn ),
	iSearchTarget ( aSearchTarget )
	{
	}

	TSsdpInfo ( TInt aMx, RMemChunk& aLocation, RMemChunk& aUsn, RMemChunk& aSearchTarget, const TAppProtAddr& aAddr )
	: iMx ( aMx ),
	iLocation ( aLocation ),
	iUsn ( aUsn ),
	iSearchTarget ( aSearchTarget ),
	iAddr ( aAddr )
	{

	}

	TInt 		iMx;
	RMemChunk 	iLocation;
	RMemChunk 	iUsn;
	RMemChunk	iSearchTarget;
	TAppProtAddr iAddr;
	};



struct SMetaDataNetCtorExt : public Meta::SMetaDataNetCtor
	{
	public:
	IMPORT_C static Meta::SMetaDataNetCtor* LoadL(TPtrC8& aDes);
	};

	
// UPnP Custom Message Signatures
DECLARE_MESSAGE_SIG_1 ( SigParamSet, CSubConExtensionParameterSet*, ParamSet )
DECLARE_MESSAGE_SIG_3 ( SigNumberVersionMemChunk, TInt, Status, TVersion, Version, RMemChunk, BodyData )
DECLARE_MESSAGE_SIG_2 ( SigParamSetFlow, CSubConExtensionParameterSet*, ParamSet, TNodeId, FlowId)
DECLARE_MESSAGE_SIG_2 ( SigRMemChunkFlow, RMemChunk, MemChunk, TNodeId, FlowId)
DECLARE_MESSAGE_SIG_1 ( SigSsdpInfo, TSsdpInfo, SsdpInfo )

DECLARE_MESSAGE_SIG_2 ( SigParamSetAny, CSubConExtensionParameterSet*, ParamSet, TAny*, Ptr)
DECLARE_MESSAGE_SIG_2 ( SigTUint8Size, const TUint8*, Ptr, TInt, Size )


struct TSigUPnPResponseInfo: public Messages::TSignatureBase
    {
protected:
	inline TSigUPnPResponseInfo () : iInfo(NULL) {	}
	explicit TSigUPnPResponseInfo ( const Messages::TNodeSignal::TMessageId& aMessageId, CTransaction* aTrans, TInt aStatus, SMetaDataNetCtorExt* aInfo )
	: Messages::TSignatureBase ( aMessageId ),
	  iTrans ( aTrans ), iStatus ( aStatus ), iInfo ( aInfo )
		{	}
public:
	DECLARE_MVIP_CTR ( TSigUPnPResponseInfo )
   	EXPORT_DATA_VTABLE_AND_FN
	CTransaction* 		iTrans;
	TInt 				iStatus;
	SMetaDataNetCtorExt* iInfo;
    };
    
template<TInt id, TInt32 realm>
struct TMessageSigUPnPResponseInfo : public TSigUPnPResponseInfo, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
    {
	explicit TMessageSigUPnPResponseInfo ( CTransaction* aTrans, TInt aStatus, SMetaDataNetCtorExt* aInfo )
	: TSigUPnPResponseInfo ( Messages::TNodeSignal::TMessageId ( id, realm ), aTrans, aStatus, aInfo )
    	{	}
    };

struct TUPnPFlowQuery: public TFactoryQueryBase
    {
public:
	inline TUPnPFlowQuery () {	}
	explicit TUPnPFlowQuery ( TNodeId aSCPRId, TInt aDataClientType, CChunkManager* aChunkManager ) : iSCPRId ( aSCPRId ), iDataClientType ( aDataClientType ), iChunkManager ( aChunkManager)
		{	}
	IMPORT_C Factories::MFactoryQuery::TMatchResult Match ( Factories::TFactoryObjectInfo& aObjectInfo );

	TNodeId                       iSCPRId;
	TInt                          iDataClientType;
	CChunkManager*  			  iChunkManager;

	DECLARE_MVIP_CTR ( TUPnPFlowQuery )
	EXPORT_DATA_VTABLE_AND_FN
    };

struct THttpUdpFlowQuery: public TUPnPFlowQuery
	{
	public :
	inline THttpUdpFlowQuery () {	}
	explicit THttpUdpFlowQuery ( const TNodeId aSCPRId, TInt aDataClientType, CChunkManager* aChunkManager=NULL ) : TUPnPFlowQuery( aSCPRId, aDataClientType, aChunkManager ) 
		{	}
	Factories::MFactoryQuery::TMatchResult Compare ( Factories::TFactoryObjectInfo& aObjectInfo );

	DECLARE_MVIP_CTR ( THttpUdpFlowQuery )
	EXPORT_DATA_VTABLE_AND_FN
	};

struct THttpClientFlowQuery: public TUPnPFlowQuery
	{
	public :
	inline THttpClientFlowQuery () {	}
	explicit THttpClientFlowQuery ( TAppProtAddr aSockAddr, TNodeId aSCPRId, TInt aDataClientType, TInt aClientFlags, CChunkManager* aChunkManager=NULL ) : iSockAddr ( aSockAddr ), iClientFlags(aClientFlags), TUPnPFlowQuery ( aSCPRId, aDataClientType, aChunkManager )
		{	}
	Factories::MFactoryQuery::TMatchResult Compare ( Factories::TFactoryObjectInfo& aObjectInfo );

	enum TClientFlags
		{
		ECreateNew,
		EUseExisting,
		};

	TAppProtAddr	iSockAddr;
	TInt 	 		iClientFlags;

	DECLARE_MVIP_CTR ( THttpClientFlowQuery )
	EXPORT_DATA_VTABLE_AND_FN
	};

struct THttpServerFlowQuery: public TUPnPFlowQuery
	{
	public :
	inline THttpServerFlowQuery (): iUri ( NULL ) {	}
	explicit THttpServerFlowQuery ( const TUint8* aUri, TInt aLength, TNodeId aCPRId, TInt aDataClientType, CChunkManager* aChunkManager=NULL )
	: TUPnPFlowQuery ( aCPRId, aDataClientType, aChunkManager ), iUri ( aUri ), iLength ( aLength )
		{	}
	Factories::MFactoryQuery::TMatchResult Compare ( Factories::TFactoryObjectInfo& aObjectInfo );

	const TUint8* 	iUri;
	TInt 			iLength;

	DECLARE_MVIP_CTR ( THttpServerFlowQuery )
	EXPORT_DATA_VTABLE_AND_FN
	};

struct TSigNumberMemChunk: public TSigNumberVersionMemChunk
    {
protected:
	inline TSigNumberMemChunk () {	};
	TSigNumberMemChunk ( const Messages::TNodeSignal::TMessageId& aMessageId, TInt aStatus, TVersion aVersion, RMemChunk aBodyData, TInt aTimeOut, RMemChunk aURN )
	: TSigNumberVersionMemChunk ( aMessageId, aStatus, aVersion, aBodyData ),
      iTimeOut ( aTimeOut ), iURN ( aURN )
		{	}
public:
	TInt		iTimeOut;
	RMemChunk 	iURN;

	DECLARE_MVIP_CTR ( TSigNumberMemChunk )
   	EXPORT_DATA_VTABLE_AND_FN
    };

template<TInt id, TInt32 realm>
struct TMessageSigNumberMemChunk: public TSigNumberMemChunk,
								  public Messages::TNodeSignal::TTypeToMessageId<id, realm>
    {
	explicit TMessageSigNumberMemChunk ( TInt aStatus, TVersion aVersion, RMemChunk aBodyData, TInt aTimeOut, RMemChunk aURN )
	: TSigNumberMemChunk ( Messages::TNodeSignal::TMessageId ( id, realm ), aStatus, aVersion, aBodyData, aTimeOut, aURN )
		{	}
    };


// Subscription response info
struct TSubsribeResponseInfo: public SMetaDataNetCtorExt
	{
public:
	inline TSubsribeResponseInfo() {}
	/*
	explicit TSubsribeResponseInfo ( RMemChunk& aSid, TInt aTimeout): iSid(aSid),iTimeout(aTimeout)
			{
			}
	*/
	TInt 		iTimeout;
	RMemChunk 	iSid;

   DECLARE_MVIP_CTR ( TSubsribeResponseInfo )
   EXPORT_DATA_VTABLE_AND_FN
	};

struct TSigUPnPClientRequestInfo: public Messages::TSignatureBase
    {
protected:
	inline TSigUPnPClientRequestInfo () : iInfo ( NULL ) {	}
	explicit TSigUPnPClientRequestInfo ( const Messages::TNodeSignal::TMessageId& aMessageId, TInt aRequestMethod, RMemChunk& aUri, RMemChunk& aBody, SMetaDataNetCtorExt* aInfo )
	: Messages::TSignatureBase ( aMessageId ),
	  iRequestMethod ( aRequestMethod ), iUri ( aUri ), iBody ( aBody ), iInfo ( aInfo )
		{	}
public:
	TInt iRequestMethod;
   	RMemChunk iUri;
   	RMemChunk iBody;
   	SMetaDataNetCtorExt* iInfo;

   	DECLARE_MVIP_CTR ( TSigUPnPClientRequestInfo )
   	EXPORT_DATA_VTABLE_AND_FN
   	};
template<TInt id, TInt32 realm>
struct TMessageSigUPnPClientRequestInfo: public TSigUPnPClientRequestInfo,
									     public Messages::TNodeSignal::TTypeToMessageId<id, realm>
    {
	explicit TMessageSigUPnPClientRequestInfo ( TInt aRequestMethod, RMemChunk& aUri, RMemChunk& aBody, SMetaDataNetCtorExt* aInfo )
	: TSigUPnPClientRequestInfo ( Messages::TNodeSignal::TMessageId ( id, realm ), aRequestMethod, aUri, aBody, aInfo )
        {	}
    };

struct TSubscribeRequestInfo: public SMetaDataNetCtorExt
	{
public:
	inline 	TSubscribeRequestInfo () {	}
	/*
	explicit TSubscribeRequestInfo ( const Messages::TNodeSignal::TMessageId& aMessageId, TInt aTimeOut, RMemChunk& aCallback, RMemChunk& aSID )
		: iMessageId ( aMessageId ), iTimeOut ( aTimeOut ), iCallback ( aCallback ), iSid ( aSID )
	{	}
	*/

	Messages::TNodeSignal::TMessageId iMessageId;
	TInt 		iTimeOut;
	RMemChunk 	iCallback;
	RMemChunk 	iSid;

	DECLARE_MVIP_CTR ( TSubscribeRequestInfo )
	EXPORT_DATA_VTABLE_AND_FN
	};

struct TEventNotifyRequestInfo: public SMetaDataNetCtorExt
	{
public:
	inline 	TEventNotifyRequestInfo () {	}
	/*
	explicit TEventNotifyRequestInfo ( const Messages::TNodeSignal::TMessageId& aMessageId, TInt aSeqKey, RMemChunk& aSid )
	: iMessageId ( aMessageId ), iSeqKey ( aSeqKey ), iSid ( aSid )
		{	}
	*/
	Messages::TNodeSignal::TMessageId iMessageId;
	TInt		iSeqKey;
	RMemChunk	iSid;

	DECLARE_MVIP_CTR ( TEventNotifyRequestInfo )
	EXPORT_DATA_VTABLE_AND_FN
	};

/*
@internalComponent
*/
class TUpnpMessage
	{
  public:
    enum { ERealmId = 0x2000D05B }; // Same as DLL Uid

  	enum
  	    {
  	    // 0 is associated with Messages::TNodeSignal::TMessageId::KCFNullMessageId		
		ESearchRequest		= 1,
		EDescribeRequest	= 2,
		EDescribeResponse	= 3,
		ESubmitRequest		= 4,
		EUPnPStopSearch		= 5,
		EResponse			= 6,
		ESubscribeResponse  = 7,
		EFindOrCreateFlow	= 8,
		EStopSearch			= 9,
		ESubscribeRequest	= 10,
		EUnSubscribeRequest = 11,		
		ENotifyRequest		= 12,
		EUPnPRequest		= 13,
		EUPnPResponse		= 14,
		ECreateNotifyFlow	= 15,
		ESPDeviceRequest 	= 16,
		ECreateServerFlow	= 17,
		EUPnPSearchRequest	= 18,
		EUPnPSearchResponse	= 19,
		EUPnPPublishAliveRequest 	= 20,
		EUPnPPublishByeRequest 		= 21,
		EUPnPNotifyRegistration		= 22,
		EUPnPPublishRegistration	= 23,
		ECancelParamSet				= 24,
		EUPnPClientRequest			= 25,
		EStartTimer					= 26,
		EStopTimer					= 27,
		ERegisterRootDevice 		= 28,
		ERootDeviceRegistered 		= 29,
		EServiceJoinRequest			= 30,
		EDeviceJoinRequest 			= 31,
		EServiceValidated 			= 32,
		ERegisterService			= 33,
		ERegisterDevice 			= 34,
		EServiceRegistered			= 35,
		EServiceRegistration		= 36,
		EDeviceRegistration			= 37,
		EUnregisterService			= 38,
		EUnregisterDevice			= 39,
		EDeviceRepublish			= 40,
		EDeviceUnpublish			= 41,
		ERequestLocalHost			= 42,
		ELocalHost					= 43,
		EActivityStop				= 44,
		EActivityStopped			= 45,
		ECancelRequest				= 46,
		ECancelled					= 47,
 	    };

	enum TUpnpActivity
    	{
    	// Simple Activities
	    EActivityFindOrCreateFlow = ESock::ECFActivityCustom + 1,
	    EActivityCreateServerFlow,
	    EActivityServiceJoinRequest,	    		    	    
	    EActivityNewServiceRegister,	    
	    EActivityServiceRegistration,	    
	    EActivityDeviceRegistration,	    
	    EActivitySubscriptionTimer,	    
	    EActivityDeviceRepublish,	    
        EActivityRegisterRootDevice,
        
        
        // End of Simple Activities
        // New Simple Activities, should be dclared before EndOfList
        EActivityEndOfList = 0xA0,
        
    	// UPnP Parallel Activities
	    EActivitySearchRequest = EActivityEndOfList + 1,
	    EActivitySearchResponse,
	    EActivityRegisterNotifyRequest,	    
	    EActivityDescribeRequest,
	    EActivitySubscribeRequest,
	    EActivityUnSubscribeRequest,
	    EActivityRequestInfo,
	    EActivityNotifyRequest,
	    EActivityEventNotify,
	    };

  public:
  	IMPORT_C static void RegisterL ( Den::CCommonWorkerThread *aWorkerThread );
    IMPORT_C static void DeRegister ( );
	
	//UPnP Search related Messages
	typedef TMessageSigParamSet < ESearchRequest, ERealmId > TSearchRequest;
	//UPnP describe related Messages
 	typedef TMessageSigParamSet < EDescribeRequest, ERealmId > TDescribeRequest;
	// TCP flow finder
	typedef TMessageSigParamSet < ESubscribeRequest, ERealmId > TSubscribeRequest;
	typedef TMessageSigParamSet < EUPnPNotifyRegistration, ERealmId > TRegisterNotifyRequest;
	typedef TMessageSigParamSet < ECancelParamSet, ERealmId > TCancelParamSet;

	// SCPR - TCP Flow Interface
	typedef TMessageSigNumberVersionMemChunk < EResponse, ERealmId > TResponse;
	typedef TMessageSigNumberMemChunk < ESubscribeResponse, ERealmId > TSubscribeResponse;
	typedef TMessageSigAny < ENotifyRequest, ERealmId > TNotifyRequest;
	typedef TMessageSigAny < ECreateNotifyFlow, ERealmId > TCreateNotifyFlow;
	typedef TMessageSigSsdpInfo < EUPnPSearchRequest, ERealmId > TUPnPSearchRequest;
	typedef TMessageSigSsdpInfo < EUPnPSearchResponse, ERealmId > TUPnPSearchResponse;
	
	typedef TMessageSigSsdpInfo < EUPnPPublishAliveRequest, ERealmId > TUPnPPublishAliveRequest;
	typedef TMessageSigSsdpInfo < EUPnPPublishByeRequest, ERealmId > TUPnPPublishByeRequest;
	typedef TMessageSigSsdpInfo < EUPnPNotifyRegistration, ERealmId > TUPnPNotifyRegistration;
	typedef TMessageSigSsdpInfo < EUPnPPublishRegistration, ERealmId > TUPnPPublishRegistration;

	// Server related messages
	typedef TMessageSigAny < EUPnPRequest, ERealmId > TUPnPRequestInfo;
	typedef TMessageSigUPnPResponseInfo < EUPnPResponse, ERealmId > TUPnPResponseInfo;

	typedef TMessageSigVoid < ECreateServerFlow, ERealmId > TCreateServerFlow;
	typedef TMessageSigVoid < ECancelRequest, ERealmId > TCancelRequest;
	typedef TMessageSigVoid < ECancelled, ERealmId > TCancelled;
	typedef TMessageSigUPnPClientRequestInfo < EUPnPClientRequest, ERealmId > TUPnPClientRequestInfo;

	typedef TMessageSigVoid < EStartTimer, ERealmId > TStartTimer;
	typedef TMessageSigVoid < EStopTimer, ERealmId > TStopTimer;
	typedef TMessageSigVoid < EActivityStopped, ERealmId > TActivityStopped;

	typedef TMessageSigAny < ERegisterRootDevice, ERealmId > TRegisterRootDevice;
	typedef TMessageSigNumber < ERootDeviceRegistered, ERealmId > TRootDeviceRegistered;
	typedef TMessageSigParamSet < EServiceJoinRequest, ERealmId > TServiceJoinRequest;
	typedef TMessageSigParamSet < EDeviceJoinRequest, ERealmId > TDeviceJoinRequest;
	typedef TMessageSigNumber < EServiceValidated, ERealmId > TServiceValidated;
	typedef TMessageSigParamSetAny < ERegisterService, ERealmId > TRegisterService;
	typedef TMessageSigParamSetAny < ERegisterDevice, ERealmId > TRegisterDevice;
	typedef TMessageSigNumber < EServiceRegistered, ERealmId > TServiceRegistered;
	typedef TMessageSigParamSet < EServiceRegistration, ERealmId > TServiceRegistration;
	typedef TMessageSigParamSet < EDeviceRegistration, ERealmId > TDeviceRegistration;
	typedef TMessageSigTUint8Size < EUnregisterService, ERealmId > TUnregisterService;
	typedef TMessageSigTUint8Size < EUnregisterDevice, ERealmId > TUnregisterDevice;
	typedef TMessageSigVoid < EDeviceRepublish, ERealmId > TDeviceRepublish;
	typedef TMessageSigVoid < ERequestLocalHost, ERealmId > TRequestLocalHost;
	typedef TMessageSigVoid < ELocalHost, ERealmId > TLocalHost;
	};

#endif // UPNPCFMSGS_H
