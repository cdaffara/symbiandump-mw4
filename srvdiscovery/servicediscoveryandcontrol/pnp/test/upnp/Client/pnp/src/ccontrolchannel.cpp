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

#include "ccontrolchannel.h"
#include <es_sock.h>
#include <e32const.h>
#include <upnpconstants.h>

CControlChannelHandler::CControlChannelHandler ( RSocket &aSocket, TBool aChannelType )
:CActive ( EPriorityNormal ), iSocket ( aSocket ), iChannelType ( aChannelType )
	{
	CActiveScheduler::Add(this);
	}

TInt CControlChannelHandler::RunError ( TInt aError )
	{
	User::RequestComplete ( iChannelStatus, aError );
	return KErrNone;	
	}
// -------------------------------------------------------------
CControlChannelReader* CControlChannelReader::NewL ( RSocket& aSocket, TBool aChannelType )
	{
	return new ( ELeave ) CControlChannelReader ( aSocket, aChannelType );
	}
		
CControlChannelReader::CControlChannelReader ( RSocket& aSocket, TBool aChannelType )
	: CControlChannelHandler ( aSocket, aChannelType )
	{
	}

CControlChannelReader::~CControlChannelReader ()
	{
	Cancel ();	
	}
	
void CControlChannelReader::Read ( TControlMessage& aMessage, TRequestStatus& aStatus )
	{
	if ( iChannelType == EFalse )
		{	
		__ASSERT_DEBUG ( !aMessage.IsDirty (), User::Panic ( KUPnPControlChannel, KErrInUse ) );
		}
		
	iMessage 		= &aMessage;
	iChannelStatus 	= &aStatus;	
	*iChannelStatus = KRequestPending;
	
	iMessage->MessagePtr().SetLength ( 0 );
	iSocket.RecvOneOrMore ( iMessage->MessagePtr(), 0, iStatus, iBytesReceived );
	SetActive ();
	}

void CControlChannelReader::RunL ()
	{
	User::LeaveIfError ( iStatus.Int() );	
	TPtrC8 buffer ( iMessage->MessagePtr() );
	TInt lenToSkip = 0;
	if ( iDataLength == 0 )  // ie; first message
		{
		// We should have more than 4 bytes ( ie; the identifier, which is of TUint,
		// size. here, sizeof (TUint) )
		__ASSERT_DEBUG ( buffer.Length () >= sizeof(TUint), User::Invariant() );
		
		// Extract the first 4 bytes. we will have our identifier with it.
		TLex8 parser ( buffer );
		TUint id = 0;
		User::LeaveIfError ( parser.Val ( id, EDecimal ) );
		iMessage->SetId( id );
		lenToSkip = sizeof (TUint);
		
		// Check how we are going to end with the message. checking the TControlMessage::MaxLength
		// or by explicitly specifying it.
		
		// Get the Maxlength and set it to the iMessage
		TInt maxLen = 0;
		User::LeaveIfError ( iSocket.GetOpt ( KCHMaxRecvLength, KCHOptionLevel, maxLen ) ); 
		iMessage->SetMaxLength ( maxLen );
		}
	TPtrC8 bufferToCopy ( buffer.Ptr() + lenToSkip, buffer.Length() - lenToSkip );
	iMessage->MessagePtr().Copy ( bufferToCopy );
	iDataLength += bufferToCopy.Length ();
	TInt maxLen = iMessage->MaxLength ();
	if ( maxLen == KErrUnknown )
		{
		// We have to issue a GetOpt to see whether we complete with the message.
		TBool last = EFalse;
		User::LeaveIfError ( iSocket.GetOpt ( KCHLastMessage, KCHOptionLevel, last ) );
		if ( last )
			iMessage->SetLast ();
		}
	else
		{
		if ( iDataLength == maxLen )
			{
			// We completed with the message. Set the message end flag as ETrue
			// and reset the iRecvLength
			iDataLength = 0;
			iMessage->SetLast ();
			}
		}
		
	if ( iMessage->IsLast () && iChannelType == EFalse )
		{
		// client usage is done with the current control message
		// We have to set a dirty bit as well for not reusing this message again
		iMessage->SetDirty ();
		}
	
	User::RequestComplete ( iChannelStatus, KErrNone );
	}
	
void CControlChannelReader::DoCancel ()
	{
	iSocket.CancelRecv ();
	User::RequestComplete ( iChannelStatus, KErrCancel );	
	}

// ---------------------------------------------------------------
	
CControlChannelWriter* CControlChannelWriter::NewL ( RSocket& aSocket, TBool aChannelType )
	{
	return new ( ELeave ) CControlChannelWriter ( aSocket, aChannelType );
	}
		
CControlChannelWriter::CControlChannelWriter ( RSocket& aSocket, TBool aChannelType )
	: CControlChannelHandler ( aSocket, aChannelType)
	{
	}

CControlChannelWriter::~CControlChannelWriter ()
	{
	Cancel ();
	}	
	  
void CControlChannelWriter::Write ( TControlMessage& aMessage, TRequestStatus& aStatus )
	{
	if ( iChannelType )
		{
		__ASSERT_DEBUG ( !aMessage.IsDirty (), User::Panic ( KUPnPControlChannel, KErrInUse ) );
		}
		
	iMessage 		= &aMessage;
	iChannelStatus 	=  &aStatus;	
	*iChannelStatus = KRequestPending;
	
	if ( iMessage->Id () == 0 )
		{
		// Message ID is not set. Typically when we do the first send after connect 
		// ie; Client side RControlChannel operation.
		iMessage->SetId ( iSocket.SubSessionHandle () );							
		}

	if ( iMessage->MaxLength() != KErrUnknown )
		{
		TCHMessageOption option ( iMessage->Id (), iMessage->MaxLength () );
		TPckg < TCHMessageOption > optionBuf ( option );
		// Set the total size if set in the TControlMessage
		iSocket.SetOpt ( KCHMaxRecvLength, KCHOptionLevel, optionBuf );		
		}
	
	if ( iMessage->IsLast () && iChannelType )
		{
		// server usage is done with the current control message
		// We have to set a dirty bit as well for not reusing this message again
		iMessage->SetDirty ();
		}
		
	iSocket.Send ( iMessage->MessageDes(), iMessage->Id(), iStatus );
	SetActive ();
	}

void CControlChannelWriter::RunL ()
	{
	User::LeaveIfError ( iStatus.Int() );	
	if ( iMessage->MaxLength() == KErrUnknown && iMessage->IsLast () )
		{
		TCHMessageOption option ( iMessage->Id (), iMessage->IsLast () );
		TPckg < TCHMessageOption > optionBuf ( option );

		// SetOpt to know whether we are sending the last message
		User::LeaveIfError ( iSocket.SetOpt( KCHLastMessage, KCHOptionLevel, optionBuf ) );
		}
	
	User::RequestComplete ( iChannelStatus, KErrNone );
	}

void CControlChannelWriter::DoCancel ()
	{
	iSocket.CancelSend ();
	User::RequestComplete ( iChannelStatus, KErrCancel );
	}

// ----------------------------------------------------------------

CControlChannel* CControlChannel::NewL ( RSocketServ& aSocketServ, RSubConnection& aSubConnection, TUint aAddrFamily, TBool aChannelType, TUint aProtocol, const TConnectionDetails& aConnInfo )
	{
	CControlChannel* self = new (ELeave) CControlChannel ( );
	CleanupStack::PushL ( self );
	self->ConstructL ( aSocketServ, aSubConnection, aAddrFamily, aChannelType, aProtocol, aConnInfo );
	CleanupStack::Pop (); // self
	return self;
	}

CControlChannel::~CControlChannel ()
	{
	delete iReader;
	delete iWriter;
	iSocket.Close ();		
	}

void CControlChannel::Recv ( TControlMessage& aMessage, TRequestStatus& aStatus )
	{
	iReader->Read ( aMessage, aStatus );	
	}

void CControlChannel::Send ( TControlMessage& aMessage, TRequestStatus& aStatus )
	{
	iWriter->Write ( aMessage, aStatus );
	}


CControlChannel::CControlChannel ( )
	{		
	}

void CControlChannel::ConstructL ( RSocketServ& aSocketServ, RSubConnection& aSubConnection, TUint aAddrFamily, TBool aChannelType, TUint aProtocol, const TConnectionDetails& aConnInfo )
	{
	User::LeaveIfError ( iSocket.Open( aSocketServ, aAddrFamily, KSockStream, aProtocol, aSubConnection ) );
	
	if ( aConnInfo.iAddr )
		{
		TRequestStatus connectStatus;
		iSocket.Connect ( *(aConnInfo.iAddr), connectStatus );
		User::WaitForRequest ( connectStatus );  // This is Ok to do... as in the esock server ( actually flow )will just store the 
												// connect address internally and not attempting to do an actual connect. As good as 
												// setopt but we need the socket to know that this is a connected socket.
		User::LeaveIfError ( connectStatus.Int() );					
		}
	
	if ( aConnInfo.iUri.Length() != 0 )
		{
		// Do a setopt to inform the esock about our URI
		User::LeaveIfError ( iSocket.SetOpt ( KCHAbsoluteUri, KCHOptionLevel, aConnInfo.iUri ) );
		}
		
	iReader = CControlChannelReader::NewL ( iSocket, aChannelType );
	iWriter = CControlChannelWriter::NewL ( iSocket, aChannelType );
	}
