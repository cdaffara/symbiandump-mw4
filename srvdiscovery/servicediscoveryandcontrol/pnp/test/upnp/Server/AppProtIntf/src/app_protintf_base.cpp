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

#include "app_protintf_base.h"
#include "app_protintf_fact.h"
#include "app_protintf_msgs.h"
#include <in_sock.h>

using namespace ESock;
using namespace Messages;

CApplicationProtocolIntfBase::CApplicationProtocolIntfBase ( TUint aPort, TInt aProtocolType )
	: CFactoryObject ( aProtocolType ),
 	iSocketHandler ( *this ),
	iPort ( aPort )
	{

	}

CApplicationProtocolIntfBase::~CApplicationProtocolIntfBase ()
	{
	delete iSocketOpener;
	iLinks.Close ();
	Shutdown();
	}

void CApplicationProtocolIntfBase::Shutdown ()
	{
	iSocketHandler.CancelAll();
	iSocket.Close ();
	}

void CApplicationProtocolIntfBase::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	switch ( aMessage.MessageId ().MessageId () )
		{
		case TAppProtIntfMessage::TClientLeavingRequest::EId:
			{
			HandleClientLeavingRequest ( address_cast<TNodeCtxId>(aSender) );
			}
			break;

		case TAppProtIntfMessage::TDestroy::EId:
			{
			CloseLink ( address_cast<TNodeId>(aSender) );
			}
		break;
		}
	}


// MSocketHandlerObserver
void CApplicationProtocolIntfBase::OpenComplete ( RInternalSocket& aSocket )
	{
	iSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	TInetAddr addr ( iPort );

	addr.SetAddress ( KInetAddrAny );
	TInt err = aSocket.Bind ( addr );

	if ( err == KErrNone )
		err = Startup ();

	if ( err == KErrNone )
		{
		// Post to all all links that we have successfully opened and bound to the socket. ie; Join is
		// successful
		JoinCompleted ();
		}
	else
		{
		// Post the error to all links
		CApplicationProtocolIntfBase::Error ( EOpenByProtocol, err );
		}
	}

void CApplicationProtocolIntfBase::Error ( TOperation /* aOperation */, TInt aError  )
	{
	// This error means something went seriously wrong. We will close down everything and post
	// an error to all links.
	TAppProtIntfMessage::TError msg ( aError );
	PostToAllLinks ( msg );

	// no other new link should get access to this object. So mark for deletion
	// MarkMeForDeletion ();
	Shutdown ();
	}

void CApplicationProtocolIntfBase::JoinCompleted ()
	{	
	PostToAllLinks ( TAppProtIntfMessage::TJoinComplete ().CRef () );
	}

TInt CApplicationProtocolIntfBase::AddLink ( const TNodeId& aSender )
	{
	if ( KErrNotFound == FindLink ( aSender ) )
		{
		RNodeInterface newLink;
    	newLink.Open ( aSender );
    	iLinks.Append ( newLink );
		}

	return iLinks.Count ();
	}

TInt CApplicationProtocolIntfBase::FindLink ( const TNodeId& aSender )
	{
	for ( TInt i = 0; i < iLinks.Count(); ++i )
		{
		if ( aSender == iLinks[i].RecipientId () )
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt CApplicationProtocolIntfBase::RemoveLink ( const TNodeId& aSender )
	{
	for ( TInt i = 0; i < iLinks.Count(); ++i )
		{
		if ( aSender == iLinks[i].RecipientId () )
			{
			iLinks.Remove (i);
			break;
			}
		}
	return iLinks.Count();
	}

void CApplicationProtocolIntfBase::PostToLink ( const TNodeCtxId& aRecipient, const TSignatureBase& aMessage )
	{
	RClientInterface::OpenPostMessageClose ( NodeId (), aRecipient, aMessage );
	}

void CApplicationProtocolIntfBase::PostToLink (const RNodeInterface& aRecipient, const TSignatureBase& aMessage )
	{
	RClientInterface::OpenPostMessageClose ( NodeId (), aRecipient.RecipientId (), aMessage );	
	}

void CApplicationProtocolIntfBase::PostToAllLinks ( const TSignatureBase& aMessage )
	{
	for ( TInt i = 0; i < iLinks.Count(); ++i )
		{
		// PostTo links
		PostToLink ( iLinks[i], aMessage );
		}
	}

TInt CApplicationProtocolIntfBase::OpenSocket ( TUint aAddrFamily, TUint aSockType, TUint aProtocol )
	{
	TInt err = KErrNone;
	if ( NULL == iSocketOpener )
		{
		TRAP ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );
		}

	if ( err == KErrNone )
		iSocketOpener->MakeSocket ( aAddrFamily, aSockType, aProtocol );

	return err;
	}

void CApplicationProtocolIntfBase::InitiateLinkL ( const TNodeCtxId& aSender, TUint aSockType, TUint aProtocol )
	{
	const TInt KFirstLink = 1;
	if ( AddLink ( aSender ) == KFirstLink )
		{
		// Open the socket
		User::LeaveIfError( OpenSocket ( KAfInet, aSockType, aProtocol ) );
		}
	else
		{		
		PostToLink ( aSender, TAppProtIntfMessage::TJoinComplete ().CRef () );
		}
	}

void CApplicationProtocolIntfBase::HandleClientLeavingRequest ( const TNodeCtxId& aSender )
	{
	// set client flags to leaving and post leave complete
	TInt idx = FindLink ( aSender );
	__ASSERT_DEBUG ( idx != KErrNotFound, User::Invariant() );

	iLinks[idx].SetFlags ( TClientType::ELeaving );
	PostToLink ( aSender, TAppProtIntfMessage::TLeaveComplete ().CRef () ); // activity id is missing
	}

void CApplicationProtocolIntfBase::CloseLink ( const TNodeId& aSender )
	{
	if ( RemoveLink ( aSender ) == 0 )
		{
		delete this; // Do a self destruction now.
		}
	}
