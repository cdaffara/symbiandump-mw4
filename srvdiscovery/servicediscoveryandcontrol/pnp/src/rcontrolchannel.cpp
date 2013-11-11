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

#include <pnp/ccontrolchannelbase.h>
#include <pnp/pnpconstants.h>
#include "rcontrolchannel.h"


EXPORT_C RControlChannel::RControlChannel ()
: iControlChannelImpl ( NULL )
	{	
	}

EXPORT_C void RControlChannel::Recv ( TControlMessage& aMessage, TRequestStatus& aStatus )
	{
	__ASSERT_DEBUG ( iControlChannelImpl, User::Panic( KControlChannelHandleNotOpen, KErrControlChannelNotOpen) );
	iControlChannelImpl->Recv ( aMessage, aStatus );	
	}

EXPORT_C void RControlChannel::Send ( TControlMessage& aMessage, TRequestStatus& aStatus )
	{
	__ASSERT_DEBUG ( iControlChannelImpl, User::Panic( KControlChannelHandleNotOpen, KErrControlChannelNotOpen) );
	iControlChannelImpl->Send ( aMessage, aStatus );
	}
	
EXPORT_C void RControlChannel::Close ()
	{
	delete iControlChannelImpl;
	iControlChannelImpl = NULL;
	}
EXPORT_C void RControlChannel::CancelRecv ()
	{
	iControlChannelImpl->CancelRecv();	
	}

EXPORT_C void RControlChannel::CancelSend ()
	{
	iControlChannelImpl->CancelSend();	
	}
void RControlChannel::Attach(CControlChannelBase& aChannelBase)
    {
    iControlChannelImpl = &aChannelBase;
	}
	
// -----------------------------------------------------

EXPORT_C TControlMessage::TControlMessage ()
: iPtr(NULL, 0),
iMaxLength (KErrUnknown),
iFlags(0)
	{
		
	}

EXPORT_C TControlMessage::~TControlMessage ()
	{
		
	}

EXPORT_C void TControlMessage::SetMessageDes ( const TDesC8& aData )
	{
	iPtr.Set((TUint8*)aData.Ptr(), aData.Length(), aData.Length() );
	}

EXPORT_C void TControlMessage::SetMessagePtr ( TDes8& aData )
	{
	iPtr.Set((TUint8*)aData.Ptr(), aData.Length(), aData.MaxLength() );
	}

EXPORT_C const TDesC8& TControlMessage::MessageDes () const
	{
	return iPtr;	
	}

EXPORT_C TDes8& TControlMessage::MessagePtr ()
	{
	return iPtr;	
	}	

EXPORT_C TBool TControlMessage::IsLast() const
	{
	return ( iFlags & 1 ); // 0th bit ( LSB )
	}
	
EXPORT_C void TControlMessage::SetLast()
	{
	iFlags |= 1; // set the 0th bit
	}

EXPORT_C void TControlMessage::ClearFlags ()
	{
	// clear set last flag
	iFlags &= ~1;
	}

EXPORT_C void TControlMessage::SetMaxLength ( TInt aLen )
	{
	iMaxLength = aLen;	
	}

EXPORT_C TInt TControlMessage::MaxLength () const
	{
	return iMaxLength;	
	}
	

// ----------------------------------------------------

