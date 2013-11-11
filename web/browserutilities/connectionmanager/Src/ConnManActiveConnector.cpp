/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     This class provides connection/session related information.
*	  Both the requested and the stored AP's are here.
*	
*
*/



#include "ConnManActiveConnector.h"
#include "ConnectionManagerLogger.h"

#include <nifvar.h>
#include <commdbconnpref.h>

//--------------------------------------------------------------------------
//CConnManActiveConnector::CConnManActiveConnector()
//--------------------------------------------------------------------------
CConnManActiveConnector::CConnManActiveConnector( RConnection& aConnection, 
                                                  TInt aPriority )
    : CActive( aPriority )
    , iConnection( aConnection )
	{
	CLOG_CREATE;
	CActiveScheduler::Add( this );//inserting this into the queue
	}

//--------------------------------------------------------------------------
//CConnManActiveConnector::~CConnManActiveConnector()
//--------------------------------------------------------------------------
CConnManActiveConnector::~CConnManActiveConnector()
	{
	Cancel();//The standard way of destroying an Active object
	CLOG_CLOSE;
	}

//--------------------------------------------------------------------------
//CConnManActiveConnector::StartConnection()
//--------------------------------------------------------------------------
void CConnManActiveConnector::StartConnection( TCommDbConnPref* aSettings, TRequestStatus& aStatus)
	{
	CLOG_WRITE( "CConnManActiveConnector:StartConnection is called");
	iExternalRequestStatus = &aStatus;
	
    if( aSettings )
        {
#ifdef __WINS__
     //  aSettings->SetDialogPreference( ECommDbDialogPrefPrompt );
#endif

	    iConnection.Start( *aSettings, iStatus );
        }
    else
        {
        iConnection.Start( iStatus );
        }

    SetActive();

	*iExternalRequestStatus = KRequestPending;
	}
	
//--------------------------------------------------------------------------
//CConnManActiveConnector::StartConnection()
//--------------------------------------------------------------------------
void CConnManActiveConnector::StartConnection( TConnSnapPref* aSettings, TRequestStatus& aStatus)
	{
	CLOG_WRITE( "CConnManActiveConnector:StartConnection is called");
	iExternalRequestStatus = &aStatus;
	
    if( aSettings )
        {
	    iConnection.Start( *aSettings, iStatus );
        }
    else
        {
        iConnection.Start( iStatus );
        }

    SetActive();

	*iExternalRequestStatus = KRequestPending;
	}	
	
//--------------------------------------------------------------------------
//CConnManActiveConnector::DoCancel()
//--------------------------------------------------------------------------
void CConnManActiveConnector::DoCancel()
	{
	CLOG_WRITE( "CConnManActiveConnector: DoCancel called");
	iConnection.Close();
	User::RequestComplete( iExternalRequestStatus, KErrCancel );//completing user req
	CLOG_WRITE( "CConnManActiveConnector: DoCancel returned");
	}

//--------------------------------------------------------------------------
//CConnManActiveConnector::RunL()
//--------------------------------------------------------------------------
void CConnManActiveConnector::RunL()
	{
    CLOG_WRITE_1( "CConnManAct::RunL(): %d", iStatus.Int() );
	User::RequestComplete( iExternalRequestStatus, iStatus.Int() );
	}

//------------------------------------------------------ CActiveConnectorSyncWrapper -------------------------

//--------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::NewL()
//--------------------------------------------------------------------------
CActiveConnectorSyncWrapper* 
        CActiveConnectorSyncWrapper::NewL( RConnection& aConnection
                                           , TInt aPriority )
	{
	CActiveConnectorSyncWrapper * self = CActiveConnectorSyncWrapper::NewLC( aConnection, aPriority );
	CleanupStack::Pop();
	return self;
	}


//--------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::NewLC()
//--------------------------------------------------------------------------	
CActiveConnectorSyncWrapper * CActiveConnectorSyncWrapper::NewLC( RConnection& aConnection, TInt aPriority)
	{
    CActiveConnectorSyncWrapper * self = new ( ELeave )CActiveConnectorSyncWrapper( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aConnection );
    return self;
	}


//--------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::Connect()
//--------------------------------------------------------------------------		
TInt CActiveConnectorSyncWrapper::Connect( TCommDbConnPref* aSettings )
	{
	iActiveConnector->StartConnection( aSettings, iStatus );
	SetActive();
	iWait.Start();
	return iStatus.Int();
	}


//--------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::ConnectSnap()
//--------------------------------------------------------------------------

TInt CActiveConnectorSyncWrapper::ConnectSnap( TConnSnapPref* aSettings )
	{
	iActiveConnector->StartConnection( aSettings, iStatus );
	SetActive();
	iWait.Start();
	return iStatus.Int();
	}

//--------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::~CActiveConnectorSyncWrapper()
//--------------------------------------------------------------------------
CActiveConnectorSyncWrapper::~CActiveConnectorSyncWrapper()
	{
	Cancel();
	delete iActiveConnector;
	}

//--------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::RunL()
//--------------------------------------------------------------------------
void CActiveConnectorSyncWrapper::RunL()
	{
	 
	 if(iWait.IsStarted())
	 {
		iWait.AsyncStop();	
	 }
	
	}


//--------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::DoCancel()
//--------------------------------------------------------------------------
void CActiveConnectorSyncWrapper::DoCancel()
	{
    
    iActiveConnector->Cancel();
	
	if(iWait.IsStarted())
	  {
	   iWait.AsyncStop();	
	  }
	
	}

//--------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::ConstructL()
//--------------------------------------------------------------------------
void CActiveConnectorSyncWrapper::ConstructL( RConnection& aConnection )
    {
	iActiveConnector = new ( ELeave ) CConnManActiveConnector( aConnection, Priority() );
	CLOG_ATTACH( iActiveConnector, this );
	}


//-------------------------------------------------------------------------------
//CActiveConnectorSyncWrapper::CActiveConnectorSyncWrapper()
//-------------------------------------------------------------------------------
CActiveConnectorSyncWrapper::CActiveConnectorSyncWrapper( TInt aPriority ): CActive( aPriority )
	{
	CActiveScheduler::Add( this );
	}


//EOF
