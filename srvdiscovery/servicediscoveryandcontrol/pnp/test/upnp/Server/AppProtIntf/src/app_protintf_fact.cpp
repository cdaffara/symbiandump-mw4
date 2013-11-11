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

#include "app_protintf_fact.h"
#include "app_protintf_msgs.h"
//#include <ss_fact2.h>
#include <in_sock.h>
#include "app_protintf_tcp.h"
#include "app_protintf_udp.h"

//using namespace ESock;

CApplicationProtocolIntfFactory* CApplicationProtocolIntfFactory::NewL ( )
	{
	CApplicationProtocolIntfFactory* self = new (ELeave) CApplicationProtocolIntfFactory( );
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop ();
	return self;
	}

void CApplicationProtocolIntfFactory::ConstructL ()	
	{
	TAppProtIntfMessage::RegisterL (); // Register the message with COMMS transport		
	}

CApplicationProtocolIntfFactory::~CApplicationProtocolIntfFactory ()
	{
	TAppProtIntfMessage::DeRegister	();
	}

CApplicationProtocolIntfFactory::CApplicationProtocolIntfFactory ( )
	{
	}

// From CFactory
CFactoryObject* CApplicationProtocolIntfFactory::DoCreateFactoryObjectL ( Factories::MFactoryQuery& aFactoryQuery )
	{
	const TAppProtIntfQuery& query = static_cast < const TAppProtIntfQuery& > ( aFactoryQuery );
	CFactoryObject* obj = NULL;
	// Factory is having the knowledge of how to create concrete factory objects
	switch ( query.iProtocol )
		{
		case KProtocolInetTcp:
		obj = CApplicationProtocolIntfTcp::NewL ( query.iPort );
		break;
		
		case KProtocolInetUdp:
		obj = CApplicationProtocolIntfUdp::NewL ( query.iPort );		
		break;
		
		default:
		User::Leave ( KErrNotSupported );
		break;
		}
	return 	obj;
	}

