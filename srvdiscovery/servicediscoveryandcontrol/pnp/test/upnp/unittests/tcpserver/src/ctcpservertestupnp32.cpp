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
#include "CTcpServerTestUPnP32.h"

using namespace Messages;

//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP32 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

_LIT8 ( KTestUPnPServiceUri,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KData, "SUBSCRIBE /testserviceuri1 HTTP/1.1\r\nHOST: 127.0.0.1:80\r\nSID: testserviceuri1\r\nCALLBACK: <http://10.192.204.79:80>\r\n\r\n");
_LIT8 ( KExpectedResponse, "HTTP/1.1 400 Bad Request");

CTcpServerTestUPnP32* CTcpServerTestUPnP32::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP32* ret = new ( ELeave ) CTcpServerTestUPnP32 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP32::CTcpServerTestUPnP32 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP32::~CTcpServerTestUPnP32 ()
	{

	}
	
void CTcpServerTestUPnP32::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP32::RunTestL ()
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
	
	
void CTcpServerTestUPnP32::RecvComplete ( RMBufChain& aData )
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


	
