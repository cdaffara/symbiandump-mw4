/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Keepalive object for Nokia IPsec over NAT
*
*/


#ifndef C_IKEV1NOKIANATTKEEPALIVE_H
#define C_IKEV1NOKIANATTKEEPALIVE_H

#include <in_sock.h> // TInetAddr

class CIkev1PluginSession;
class MIkeDebug;

/**
 * Nokia IPsec over NAT keepalive object. Sends
 * UDP packets to specified host with specified
 * interval to keep NAT mapping alive. Constructing
 * the object starts sending and sending is stopped
 * when destruction happens.
 * @internalComponent
 */
NONSHARABLE_CLASS(CIkev1NokiaNattKeepAlive) : public CBase
	{
	
public: // Public constructor and destructor
	static CIkev1NokiaNattKeepAlive* NewL( CIkev1PluginSession&  aPluginSession, 
			                          TInetAddr& aDestAddr,
			                          TUint16 aPort,
			                          TUint aInterval,
			                          MIkeDebug& aDebug );
	virtual ~CIkev1NokiaNattKeepAlive();

private: // Private constructors

	CIkev1NokiaNattKeepAlive( CIkev1PluginSession&  aPluginSession,
	                          TInetAddr& aDestAddr, 
	                          TUint16 aPort,
	                          MIkeDebug& aDebug );
	void ConstructL( TUint aInterval );
	
private:
	
	/**
	 * Send
	 * Sends keepalive packet
	 */
	void Send();

	/**
	 * PeriodicCallback
	 * Callback function which is called everytime
	 * the interval is reached.
	 */
	static TInt PeriodicCallback(TAny *aPtr);

private:
    CIkev1PluginSession&  iPluginSession;

	TInetAddr iDestAddr;
	TUint iPort;
	
	/** Data to send in keepalive packet */
	TBuf8<1> iMsg;
	
	CPeriodic *iTimer;
	
	MIkeDebug& iDebug;
	
	};

#endif // C_IKEV1NOKIANATTKEEPALIVE_H
