/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <ss_glob.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include "ptcpservertests.h"
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "upnpflowfactory.h"
#include "CTcpServerTestCase.h"

#include <httperr.h>
#include "CTcpServerTestUPnP29.h"

using namespace Messages;

//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP29 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

_LIT8 ( KTestUPnPServiceUri,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KData, "GET /testserviceuri1 HTTP/1.1\r\nHOST: 222:128:0.0.1:\r\n\r\n");
_LIT8 ( KExpectedResponse, "HTTP/1.1 404 Not Found");
//_LIT8 ( KResponseData, "Test Server");

CTcpServerTestUPnP29* CTcpServerTestUPnP29::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP29* ret = new ( ELeave ) CTcpServerTestUPnP29 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP29::CTcpServerTestUPnP29 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP29::~CTcpServerTestUPnP29 ()
	{

	}
	
void CTcpServerTestUPnP29::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP29::RunTestL ()
	{
	switch ( iState )
		{
			
		case ESendData:
			{			
			iSendChain.CreateL ( KData );
			iSocketHandler.Send ( iSendChain );
			
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
					
		default:
			{
			return CTcpServerTestCase::RunTestL();
			}
		}
	}
	
	
void CTcpServerTestUPnP29::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse ) == KErrNotFound )
		{
		iResponse = EFail; // test case failed
		}
	else
		{
		iResponse = EPass;	// test case passed
		}
	
	responseBuf.Close ();
	CompleteSelf ( KErrNone );
	}


	

