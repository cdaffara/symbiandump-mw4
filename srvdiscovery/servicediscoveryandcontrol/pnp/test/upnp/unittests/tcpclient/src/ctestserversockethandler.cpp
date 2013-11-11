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


#include "ctestserversockethandler.h"
#include "mtcpclienttestcase.h"

_LIT8(KTxtConnectionClose, "Connection: Close");
_LIT8(KTxtPost, "POST");
_LIT8(KTxtMPost, "M-POST");
_LIT8(KTxt100Continue, "Expect: 100-Continue");

CTestServerSocketHandler* CTestServerSocketHandler::NewL ( MTcpClientTestCase* aTestCase, RInternalSocket& aIncomingConnection, CTestExecuteLogger&	aLogger )
	{
	CTestServerSocketHandler* self = new ( ELeave ) CTestServerSocketHandler ( aTestCase, aIncomingConnection, aLogger );
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop ( self );
	return self;
	}

CTestServerSocketHandler::CTestServerSocketHandler ( MTcpClientTestCase* aTestCase, RInternalSocket& aIncomingConnection, CTestExecuteLogger& aLogger )
	: CTimer ( EPriorityNormal ), iIncomingConnection ( aIncomingConnection ), iSocketHandler ( *this ), iTestCase ( aTestCase ), iLogger(aLogger)
	{
	CActiveScheduler::Add ( this );
	iTransCount = iTestCase->TransactionCount ();
	iSocketHandler.Attach(iIncomingConnection);
	}

CTestServerSocketHandler::~CTestServerSocketHandler ()
	{
	Cancel ();
	iSocketHandler.CancelAll ();
	iIncomingConnection.Close ();
	delete iDataStore;
	delete iDataToSend;
	}

void CTestServerSocketHandler::ConstructL ()
	{
	CTimer::ConstructL ();
	iSocketHandler.Recv ();
	}
	
void CTestServerSocketHandler::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 aBuffer;
	aBuffer.CreateMax ( aData.Length () );
	aData.CopyOut ( aBuffer );
	aData.Free ();
	TPtrC8 ptr(aBuffer);
	iLogger.WriteFormat(_L("<i> RecvComplete:Received data from client </i>"));

	if ( iDataStore == NULL )
		iDataStore = aBuffer.Alloc(); // First data chunk received
	else
		{
		// Already got data before so append the data to the existing data
		TInt newLength = ( iDataStore->Length () ) + aBuffer.Length ();
		iDataStore = iDataStore->ReAlloc ( newLength );
		TPtr8 buf = iDataStore->Des ();
		buf.Append ( aBuffer );
		}		
	
	_LIT ( KTxtTitle, "Defect Fix INC036954" );
	if ( iTestCase->TestCaseName ().Match ( KTxtTitle ) == 0 )
		{
		// iInputStream->ShutdownReq ();
		iSocketHandler.CancelAll ();
		iIncomingConnection.Close ();
		}
	else
		{
		// Try processing the requests
		TRAP_IGNORE(SendDataL ());
		}
	
	if ( !iCloseConnection )
		iSocketHandler.Recv ();
	}
	
void CTestServerSocketHandler::SendDataL ()
	{
	Cancel ();
	
	_LIT ( KTcTitle, "CRecvTimeOut" );
	if ( iTestCase->TestCaseName ().Match ( KTcTitle ) == 0 )
		{
		After ( 61000000 );
		return;
		}

	// Try processing the requests
	if ( ProcessRequestL () )
		{
		iLogger.WriteFormat(_L("<i>SendDataL:Sent data to client..... </i>"));
		iSendChain.CreateL ( *iDataToSend );
		iSocketHandler.Send ( iSendChain );
		}
	else if ( iSend100Continue )
		{
		TPtrC8 rawResponse ( _L8 ("HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 Ok\r\n\r\n") );

		iDataToSend = rawResponse.Alloc();
		
		iLogger.WriteFormat(_L("<i>SendDataL:Sent data to client..... </i>"));
		iSendChain.CreateL ( *iDataToSend );
		iSocketHandler.Send ( iSendChain );
		iSend100Continue = EFalse;
		}
	}
	
TBool CTestServerSocketHandler::ProcessRequestL ()
	{
	TBool processingRequest = ETrue;
	_LIT ( KTcTitle, "CTcpClientTestUPnP8" );	//For Resubmit
	_LIT ( KTcTitle1, "CTcpClientTestMWrites" );	//For multiple chunks
	
	iLogger.WriteFormat(_L("<i>ProcessRequestL..... </i>"));
	while ( processingRequest &&  iCurrentTrans < iTransCount )
		{
		// Do we have enough data to respond to the current transaction?
		TPtrC8 rawRequest = iTestCase->GetRawRequest ( iCurrentTrans );
		TInt requestLength = rawRequest.Length ();
		TPtrC8 dataWindow = iDataStore->Mid ( iDataPos );
		
		if(iPost && ((iTestCase->TestCaseName ().Match ( KTcTitle1 ) == 0 ) || (iTestCase->TestCaseName ().Match ( KTcTitle ) == 0 )))
			{
			if(iDataStore->Length() < (requestLength + 330)) //Hard coded length for this case.
				break;
			}
		
		if ( requestLength <= dataWindow.Length () )
			{
			// Check that the raw request and the actual request match
			if (!iPost && dataWindow.FindF ( rawRequest ) != 0 )
				{
				User::Leave ( KErrNotFound );
				}

			// Prepare the response data to send
			iDataPos += requestLength;
			
			processingRequest = ETrue;
			
			if((dataWindow.FindF(KTxtPost)!= KErrNotFound) || (dataWindow.FindF(KTxtMPost)!= KErrNotFound))
				{
				iPost = ETrue;
				return EFalse;
				}
			else
				iPost = EFalse;
			

			TPtrC8 rawResponse = iTestCase->GetRawResponse ( iCurrentTrans );
			
			if ( iDataToSend == NULL )
				{
				iDataToSend = rawResponse.AllocL ();
				}
			else
				{
				TInt responseLength = rawResponse.Length ();
				iDataToSend = iDataToSend->ReAllocL ( (iDataToSend->Length () ) + responseLength );
				TPtr8 buffer = iDataToSend->Des ();
				buffer.Append ( rawResponse );
				}
			
			++iCurrentTrans;
			// Check for a Connection: Close in the request
			iCloseConnection = IsConnectionCloseInData ( rawRequest, rawResponse );
			if ( iCloseConnection )
				processingRequest = EFalse;
			
			iSend100Continue = Is100ContinueInRequestData ( rawRequest );
			}
		else
			{
			// No more requests can be processed
			processingRequest = EFalse;
			}
		
		if(iTestCase->TestCaseName ().Match ( KTcTitle ) == 0 )
			break;
		}

	if ( iDataToSend != NULL )
		return ETrue;

	return EFalse;
	}

TBool CTestServerSocketHandler::IsConnectionCloseInData ( const TDesC8& aRequest, const TDesC8& aResponse ) const
	{
	
	if ( ( aRequest.FindF ( KTxtConnectionClose () ) != KErrNotFound ) || ( aResponse.FindF ( KTxtConnectionClose () ) != KErrNotFound ) )
		{
		return ETrue;
		}
	return EFalse;
	}

TBool CTestServerSocketHandler::Is100ContinueInRequestData ( const TDesC8& aRequest ) const
	{	
	if ( aRequest.FindF ( KTxt100Continue () ) != KErrNotFound )
		{
		return ETrue;
		}
	return EFalse;
	}

void CTestServerSocketHandler::SendComplete ( TInt /* aLength */ )
	{

	iSendChain.Init ();
	delete iDataToSend;
	iDataToSend = NULL;
	
	//Exclusively For M-Post Case.
	delete iDataStore;
	iDataStore = NULL;
	iDataPos = 0;
	
	// Do we need to close the connection
	if ( iCloseConnection )
		{
		iSocketHandler.CancelAll ();
		iIncomingConnection.Close ();
		}			
	}
	
void CTestServerSocketHandler::RunL ()
	{
	_LIT ( KTcTitle, "CRecvTimeOut" );
	if ( iTestCase->TestCaseName ().Match ( KTcTitle ) == 0 )
		{
		return;
		}

	if( iStatus.Int () == KErrNone )
		{
		// The connection has timed out.
		iSocketHandler.CancelAll ();
		iIncomingConnection.Close ();
		}
	}
	


