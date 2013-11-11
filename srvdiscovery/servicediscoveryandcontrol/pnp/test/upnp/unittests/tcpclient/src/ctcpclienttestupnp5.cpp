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
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <httpstringconstants.h>

#include "ptcpclienttests.h"	
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "httpclientflow.h"
#include "upnpflowfactory.h"
#include "CTcpClientTestUPnP5.h"

#include "csoapparser.h"

const TUint KTransactionCount = 1;
_LIT8 ( KTxtRawRequest, "" );
_LIT8 ( KTxtRawResponse1, "" );

_LIT8 ( KData1, "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body1></s:Envelope>" );


CTcpClientTestUPnP5* CTcpClientTestUPnP5::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP5* ret = new ( ELeave ) CTcpClientTestUPnP5 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP5::CTcpClientTestUPnP5 ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP5::~CTcpClientTestUPnP5 ()
	{	
	//iTimer.Close ();
	}

void CTcpClientTestUPnP5::ConstructL()
	{	
	//iTimer.CreateLocal ();
	}

TVerdict CTcpClientTestUPnP5::RunTestL()
	{
	switch ( iState )
		{
		case ECreateTestServer:
			{
			CSoapParser* soapParser;
			TInt err;
			soapParser = CSoapParser::NewL();
			
			TRAP(err,soapParser->ParseSoapL(KData1));
			TRAP(err,soapParser->ParseSoapL(KData1));
			delete soapParser;
			
			soapParser = CSoapParser::NewL();
			//TRAP(err,soapParser->ParseSoapL(KData2));
			TRAP(err,soapParser->ParseSoapL(KTxtRawRequest));
			delete soapParser;
			iLogger.WriteFormat(_L("<i>TestCase: Complete..... </i>"));
			return EPass;
			}

		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CTcpClientTestUPnP5::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP5::GetTestCase ()
	{
	return this;
	}
	
CTestExecuteLogger& CTcpClientTestUPnP5::GetLogger ( )
	{
	return iLogger;
	}


void CTcpClientTestUPnP5::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TResponse > () )
		{
		
		CompleteSelf(EPass);
		}
	else if ( aMessage.IsMessage<TEBase::TError > () )
		{
		iLogger.WriteFormat(_L("<i>TEBase::TError..... </i>"));
		CompleteSelf(EFail);
		}

	}
	
const TDesC& CTcpClientTestUPnP5::TestCaseName () const
	{
	_LIT ( KTxtTitle, "Defect Fix CINC077703More" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP5::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP5::GetRawRequest ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawRequest ();
	}
	
const TDesC8& CTcpClientTestUPnP5::GetRawResponse ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawResponse1 ();
	}

TInt CTcpClientTestUPnP5::TransactionCount () const
	{ 
	return KTransactionCount; 
	}










