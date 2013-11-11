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



#include <nifman.h>
#include <nifvar.h>
#include <in_sock.h>

#include "ConnectionObservers.h"
#include "ConnectionManagerLogger.h"


//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::NewL()
//--------------------------------------------------------------------------
EXPORT_C CConnectionStageNotifierWCB* CConnectionStageNotifierWCB::NewL( TInt aPriority )
	{
	CConnectionStageNotifierWCB* self = CConnectionStageNotifierWCB::NewLC( aPriority );
	CleanupStack::Pop();
	return self;
	}

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::NewLC()
//--------------------------------------------------------------------------
EXPORT_C CConnectionStageNotifierWCB* CConnectionStageNotifierWCB::NewLC( TInt aPriority )
	{
	CConnectionStageNotifierWCB* self = new ( ELeave ) CConnectionStageNotifierWCB( aPriority );	
	CleanupStack::PushL( self );
	return self;
	}

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::StartNotificationL()
//--------------------------------------------------------------------------
EXPORT_C void 
    CConnectionStageNotifierWCB::StartNotificationL( TInt /*aStageToObserve*/, 
                                                     MConnectionStageObserver* /*aObserver*/, 
                                                     TBool /*aCompleteIfAlreadyAtStage*/ )	
	{
    User::Leave( KErrNotSupported );
	}

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::~CConnectionStageNotifierWCB()
//--------------------------------------------------------------------------
EXPORT_C CConnectionStageNotifierWCB::~CConnectionStageNotifierWCB()
	{
	CLOG_WRITE("~CConnectionStageNotifierWCB");
	
	Cancel();
	
	CLOG_WRITE("Canceled");
    // if there's nothing to cancel
    // DoCancel is not called.
    // We need to close agent manually from here
    DoCloseAgent();
    
    CLOG_CLOSE;
	}

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::CConnectionStageNotifierWCB()
//--------------------------------------------------------------------------
CConnectionStageNotifierWCB::CConnectionStageNotifierWCB( TInt aPriority ) : CActive( aPriority )
	{
	CLOG_CREATE;
	CLOG_NAME_1( _L("StageNotifier_%x"), this );
	
	CActiveScheduler::Add( this );
	}

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::DoCancel()
//--------------------------------------------------------------------------
void CConnectionStageNotifierWCB::DoCancel()
	{
	CLOG_ENTERFN( "DoCancel()" );
	iConnection.CancelProgressNotification();
    DoCloseAgent();

    iMultiObserver = NULL;
    iStages = NULL;
    iNumOfStages = 0;
	}

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::RunL()
//--------------------------------------------------------------------------
void CConnectionStageNotifierWCB::RunL()
	{
	CLOG_ENTERFN("CConnectionStageNotifierWCB::RunL()");
    CLOG_WRITE_1( "CConnectionStageNotifierWCB Stage: %d", iProgressBuf().iStage );

    if( !iMultiObserver )
        {
	    if ( iProgressBuf().iStage == iStageToObserve )
		    {
            CLOG_WRITE( "Stage achived" );
		    DoCloseAgent();
		    iObserver->ConnectionStageAchievedL();
		    }
	    else//reissuing request
		    {
            CLOG_WRITE( "Restarting progress notif." );
		    iConnection.ProgressNotification(  iProgressBuf, iStatus );//reissuing request
		    SetActive();
		    }
        }
    else
        {
        if( IsAnyStageReached( iProgressBuf().iStage ) )
            {
            iMultiObserver->ConnectionStageAchievedL( iProgressBuf().iStage );
            }
		else{
			//reissuing request
			iConnection.ProgressNotification( iProgressBuf, iStatus );//issuing request
			SetActive();
			}
        }
	}


//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::StartNotificationL()
//--------------------------------------------------------------------------
EXPORT_C void 
    CConnectionStageNotifierWCB::StartNotificationL( TName* aConnName, 
                                                     TInt aStageToObserve, 
                                                     MConnectionStageObserver* aObserver, 
                                                     TBool aCompleteIfAlreadyAtStage )	
	{
    CLOG_ENTERFN( "StartNotificationL1" );

	__ASSERT_DEBUG( aObserver, User::Panic(  KNullDesC, KErrArgument ) );
	iObserver = aObserver;
	iStageToObserve = aStageToObserve;


    DoOpenAgentL( aConnName );
      
    TNifProgress progress;

    iConnection.Progress( progress );//initial progress may be important to save whether it is unconnected now
	if ( progress.iStage == aStageToObserve && aCompleteIfAlreadyAtStage )
		{
		DoCloseAgent();
		TRAP_IGNORE( iObserver->ConnectionStageAchievedL() );
		}
	else
		{
			
			/* No need to call Setactive() in the case if connection is already active, that is why checking isActive 
			connection, if not then only activating it, For instance Incase of Connection Change from real IAP to VPN on the 
			same real IAP using which VPN configurations were made then the existing connection would not be disconnected 
			but a new VPN tunnel will be created on top of it.So connection is already active, it should not call SetActive() 
			if the active object request has been issued already, new request should be issued only incase of a new connection altogether*/
			if(!IsActive())
			{
				iConnection.ProgressNotification( iProgressBuf, iStatus );//issuing request
				SetActive();
			}
		}
    }

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::StartNotificationL()
//--------------------------------------------------------------------------
EXPORT_C void 
    CConnectionStageNotifierWCB::StartNotificationL( TName* aConnName, 
                                                     TInt* aStages, 
                                                     TInt aNumOfStages,
                                                     MConnectionMultiStageObserver* aObserver,
                                                     TBool aCompleteIfAlreadyAtStage )	
	{
    CLOG_ENTERFN( "StartNotificationL2" );

    __ASSERT_DEBUG( aStages, User::Panic(  KNullDesC, KErrArgument ) );
    __ASSERT_DEBUG( aNumOfStages > 0 , User::Panic(  KNullDesC, KErrArgument ) );
    __ASSERT_DEBUG( aObserver, User::Panic(  KNullDesC, KErrArgument ) );

    iStages = aStages;
    iNumOfStages = aNumOfStages;
    iMultiObserver = aObserver;

    DoOpenAgentL( aConnName );

	TNifProgress progress;
	iConnection.Progress( progress );//initial progress may be important to save whether it is unconnected now
	if ( IsAnyStageReached( progress.iStage ) && aCompleteIfAlreadyAtStage )
		{
		DoCloseAgent();
		TRAP_IGNORE( iMultiObserver->ConnectionStageAchievedL( progress.iStage ) );
		}
	else
		{
		if(!IsActive())
			{
		    iConnection.ProgressNotification( iProgressBuf, iStatus );//issuing request
		    SetActive();
		    }
		}
    }

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::DoOpenAgent
//--------------------------------------------------------------------------
void CConnectionStageNotifierWCB::DoOpenAgentL( TName* aConnName )
    {
    CLOG_ENTERFN( "CConnectionStageNotifierWCB::DoOpenAgentL" );
    TInt err( KErrNone );
    if( (err = iServer.Connect()) == KErrNone )
        {
        err = iConnection.Open( iServer, *aConnName );
        if( !err )
            {
            TBuf<64> query;
            TUint32 apId;
            TUint32 netId;

            query.Format( _L("%s\\%s"), IAP, COMMDB_ID );
            if( iConnection.GetIntSetting( query, apId ) == KErrNone )
                {
                query.Format( _L("%s\\%s"), IAP, IAP_NETWORK );
                if( iConnection.GetIntSetting( query, netId ) == KErrNone )
                    {
                    TPckgBuf<TConnectionInfo> connInfo;

                    connInfo().iIapId = apId;
                    connInfo().iNetId = netId;
                    CLOG_WRITE_2( "IAP ID: %d, NET ID: %d", connInfo().iIapId, connInfo().iNetId );

                    iConnection.Close();
                    if( iConnection.Open( iServer, KAfInet ) == KErrNone )
                        {
                        err = iConnection.Attach( connInfo, RConnection::EAttachTypeMonitor );
                        CLOG_WRITE_1( "Attach result: %d", err );
                        }
                    }
                }
            }
        }

    if( err )
        {
        iConnection.Close();
        iServer.Close();
        User::Leave( err );
        }
    }

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::DoCloseAgent
//--------------------------------------------------------------------------
void CConnectionStageNotifierWCB::DoCloseAgent()
    {
    CLOG_ENTERFN( "CConnectionStageNotifierWCB::DoCloseAgent()" );
    iConnection.Close();
    iServer.Close();
    }

//--------------------------------------------------------------------------
//CConnectionStageNotifierWCB::IsAnyStageReached()
//--------------------------------------------------------------------------
TBool CConnectionStageNotifierWCB::IsAnyStageReached( TInt aCurrentStage )
    {
    for( TInt i = 0; i < iNumOfStages; ++i )
        {
        if( iStages[i] == aCurrentStage )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

//EOF
