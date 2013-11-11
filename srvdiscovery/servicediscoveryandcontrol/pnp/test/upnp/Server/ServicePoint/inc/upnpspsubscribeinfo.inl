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

#ifndef __UPNPSPSUBSCRIBEINFO_INL_
#define __UPNPSPSUBSCRIBEINFO_INL_


CUPnPSubscribeInfoElement* CUPnPSubscribeInfoElement::NewL ( )
	{
	return new ( ELeave ) CUPnPSubscribeInfoElement ( );
	}

CUPnPSubscribeInfoElement::~CUPnPSubscribeInfoElement ( )
	{
	iSubscriptionIdentifier.Close ( );
	iCallBackUrl.Close ( );		
	}

void CUPnPSubscribeInfoElement::SetSubscriptionIdentifierL ( const TDesC8& aSubscriptionIdentifier )
	{
	iSubscriptionIdentifier.Close ( );
	iSubscriptionIdentifier.CreateL ( aSubscriptionIdentifier );
	}
	
void CUPnPSubscribeInfoElement::SetCallBackUrlL ( const TDesC8& aCallBackUrl )
	{
	iCallBackUrl.Close ( );
	iCallBackUrl.CreateL ( aCallBackUrl );
	}
	
const TDesC8& CUPnPSubscribeInfoElement::CallBackUrl ( )
	{
	return iCallBackUrl;
	}
	
void CUPnPSubscribeInfoElement::SetRemoteAddr ( TAppProtAddr aRemoteAddr )
	{
	iRemoteAddr = aRemoteAddr;
	}
	
void CUPnPSubscribeInfoElement::SetEventKey ( TInt aEventKey )
	{
	iEventKey = aEventKey;
	}
	
void CUPnPSubscribeInfoElement::SetSubscriptionDuration ( TInt aSubscriptionDuration )
	{
	iSubscriptionDuration = aSubscriptionDuration;
	}

void CUPnPSubscribeInfoElement::SetFlowId ( TNodeId aFlowId )
	{
	iFlowId = aFlowId;
	}

const TDesC8& CUPnPSubscribeInfoElement::SubscriptionIdentifier ( ) const 
	{
	return iSubscriptionIdentifier;
	}
	
TAppProtAddr CUPnPSubscribeInfoElement::RemoteAddr ( ) const 
	{
	return iRemoteAddr;
	}
	
TInt CUPnPSubscribeInfoElement::EventKey ( ) const 
	{
	return iEventKey;
	}
	
TInt CUPnPSubscribeInfoElement::SubscriptionDuration ( ) const 
	{
	return iSubscriptionDuration;
	}
	
TNodeId CUPnPSubscribeInfoElement::FlowId ( )
	{
	return iFlowId;
	}


#endif /*UPNPSPSUBSCRIBEINFO_INL_*/
