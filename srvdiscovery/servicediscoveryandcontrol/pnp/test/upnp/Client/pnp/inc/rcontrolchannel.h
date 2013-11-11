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


#ifndef __RCONTROLCHANNEL_H_
#define __RCONTROLCHANNEL_H_
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>

const TInt KMaxLength = 100;
const TInt KIntSize = 4;


// Option names. Used for Get & Set options from the socket.
/*
Set the absolute URI
*/
const TUint KCHAbsoluteUri = 1;

/*
The maximum receive length that is expected. When it is called with GetOpt
it can return KErrUnknown otherwise a valid length
*/
const TUint KCHMaxRecvLength = 2;

/*
The message is last or not
*/
const TUint KCHLastMessage = 3;


/*
Option level --- Always 0.
*/
const TUint KCHOptionLevel = 0; // Always 0.

struct TCHMessageOption
	{
	TCHMessageOption ( TUint aId, TInt aValue )
	: iId ( aId ),
	iValue ( aValue )
	{		
	}
	
	TUint iId;
	TInt  iValue; 	
	};

/* 
 This class is used to wrap the TDes8 data that is used for transmission
 in the sockets
 @publishedPartner
 @prototype
*/
class TControlMessage
	{
	friend class CControlChannelReader;
	friend class CControlChannelWriter;
public:
	IMPORT_C TControlMessage ();
	IMPORT_C ~TControlMessage ();
	IMPORT_C void SetMessageDes ( const TDesC8& aData );
	IMPORT_C void SetMessagePtr ( TDes8& aData );	
	IMPORT_C const TDesC8& MessageDes () const;
	IMPORT_C TDes8& MessagePtr ();	
	IMPORT_C TBool IsLast() const;
	IMPORT_C void SetLast();
	IMPORT_C void SetMaxLength ( TInt aLen );
	IMPORT_C TInt MaxLength () const;
	IMPORT_C void ClearFlags ();
	
private:	
	// Not for external use. Only for internal purpose
	void SetId ( TUint aId );
	TUint Id () const;
	void SetDirty ();
	TBool IsDirty () const;
	
private:
	TPtr8 		iPtr;
	TUint		iId;
	TInt 		iMaxLength;
	TUint	 	iFlags;
	};


/* The connection details. 

@internalTechnology
*/

struct TConnectionDetails
	{
	TConnectionDetails ( TSockAddr* aAddr, const TDesC8& aUri )
	: iAddr ( aAddr ),
	iUri ( aUri )
	{ }
	
	TConnectionDetails ( )
	: iAddr ( NULL ),
	iUri ( KNullDesC8 () )
	{ }
	
	TSockAddr* 	iAddr;
	TPtrC8 		iUri;
	};

class CControlChannel;

/** This class acts as a wrapper over rsocket and is used to send and receive SOAP messages
 @publishedPartner
 @prototype
 */
class RControlChannel
	{
public:
	IMPORT_C RControlChannel ( );
	IMPORT_C void Recv ( TControlMessage& aMessage, TRequestStatus& aStatus );
	IMPORT_C void Send ( TControlMessage& aMessage, TRequestStatus& aStatus );
	IMPORT_C void Close ();	
	
	/** Open is used to open up the underlying socket of RControlChannel and get it ready for data transfer
	@internalComponent
	@prototype
	*/
	
	IMPORT_C TInt Open (  RSocketServ& aSocketServ, RSubConnection& aSubConnection, TUint aAddrFamily, TBool aChannelType, TUint aProtocol, const TConnectionDetails& aConnInfo );
	
private:
	CControlChannel* iControlChannelImpl;
	};

_LIT_SECURITY_POLICY_PASS ( KAllowAllPolicy );
	

#endif /*RCONTROLCHANNEL_H_*/
