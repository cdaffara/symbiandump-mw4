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

#ifndef __UPNPSPSUBSCRIBEINFO_H_
#define __UPNPSPSUBSCRIBEINFO_H_

#include <appprotintf/app_protintf_msgs.h>

using namespace Messages;

class CUPnPSubscribeInfoElement : public CBase
	{
	private:
	RBuf8 			iSubscriptionIdentifier;
	RBuf8			iCallBackUrl;
	TAppProtAddr	iRemoteAddr;
	TInt			iEventKey;
	TInt			iSubscriptionDuration;
	TNodeId	iFlowId;

	public:

	// Internal APIs
	inline static CUPnPSubscribeInfoElement* NewL ( );
	// D'tor
	inline ~CUPnPSubscribeInfoElement ( );
	
	// Get and set methods
	inline void SetSubscriptionIdentifierL ( const TDesC8& aSubscriptionIdentifier );
	inline void SetCallBackUrlL ( const TDesC8& aCallBackUrl );
	inline void SetRemoteAddr ( TAppProtAddr aRemoteAddr );
	inline void SetEventKey ( TInt aEventKey );
	inline void SetSubscriptionDuration ( TInt aSubscriptionDuration );
	inline void SetFlowId ( TNodeId aFlowId );
	
	inline const TDesC8& SubscriptionIdentifier ( ) const;	
	inline const TDesC8& CallBackUrl ( );	
	inline TAppProtAddr RemoteAddr ( ) const;
	inline TInt EventKey ( ) const;
	inline TInt SubscriptionDuration ( ) const;
	inline TNodeId FlowId ( );	
	};

#include "upnpspsubscribeinfo.inl"

#endif /*UPNPSPSUBSCRIBEINFO_H_*/
