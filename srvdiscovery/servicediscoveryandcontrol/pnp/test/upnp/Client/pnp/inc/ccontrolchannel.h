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

#ifndef __CCONTROLCHANNEL_H_
#define __CCONTROLCHANNEL_H_

#include <e32base.h>
#include <es_sock.h>
#include "rcontrolchannel.h"

class CControlChannelHandler : public CActive
	{	
	protected:
		CControlChannelHandler ( RSocket& aSocket, TBool aChannelType );		
		TInt RunError(TInt aError);
	protected:
		RSocket& 			iSocket;  // Not owned
		TBool				iChannelType; // EFalse for Client usage
										  // ETrue for server usage	
		TControlMessage* 	iMessage; // Not owned.
		TRequestStatus*		iChannelStatus;  // Not owned	
		TInt 				iDataLength;	// Actual buffer size. this is used for internal purpose to find the
										// end of the message. This variable will get incremented whenever we
										// receive/send data from/to the socket and will be reset to 0 once the end of
										// message is reached.		
	};

class CControlChannelReader : public CControlChannelHandler 
	{
	public:
		static CControlChannelReader* NewL ( RSocket& aSocket, TBool aChannelType );
		~CControlChannelReader ();
		
		void Read ( TControlMessage& aMessage, TRequestStatus& aStatus );		
	private:
		CControlChannelReader ( RSocket& aSocket, TBool aChannelType );		
		
		// From CActive
		void RunL ();
		void DoCancel ();
			
	private:
		TSockXfrLength  iBytesReceived; // Socket recv buffer size.
 	};

// ---------------------------------------------------------------

class CControlChannelWriter : public CControlChannelHandler
	{
	public:
		static CControlChannelWriter* NewL ( RSocket& aSocket, TBool aChannelType );
		~CControlChannelWriter ();
		
		void Write ( TControlMessage& aMessage, TRequestStatus& aStatus );
	private:
	  	CControlChannelWriter ( RSocket& aSocket, TBool aChannelType );
	  		  	
	  	// From CActive
		void RunL ();
		void DoCancel ();

	private:
		RBuf8	iBuf; 		  
	};

// ---------------------------------------------------------------

class CControlChannel : public CBase
{
public:
	static CControlChannel* NewL ( RSocketServ& aSocketServ, RSubConnection& aSubConnection, TUint aAddrFamily, TBool aChannelType, TUint aProtocol, const TConnectionDetails& aConnInfo );
	virtual ~CControlChannel ();
	
	void Recv ( TControlMessage& aMessage, TRequestStatus& aStatus );
	void Send ( TControlMessage& aMessage, TRequestStatus& aStatus );
private:
	CControlChannel ( );
	void ConstructL ( RSocketServ& aSocketServ, RSubConnection& aSubConnection, TUint aAddrFamily, TBool aChannelType, TUint aProtocol, const TConnectionDetails& aConnInfo );
private:
	RSocket 				iSocket;
	CControlChannelReader* 	iReader;
	CControlChannelWriter* 	iWriter;	
};
#endif