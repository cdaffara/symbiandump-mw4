/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CUsbPersonalityNotifier class prevents confirmation notes
*                from overlapping.
*
*/


#include <cusbpersonalitynotifier.h>
#include "debug.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// C++ constructor
// ----------------------------------------------------------------------------
//
CUsbPersonalityNotifier::CUsbPersonalityNotifier()
    : CActive( EPriorityStandard )
    {
    LOG_FUNC
        
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUsbPersonalityNotifier::ConstructL()
    {
    LOG_FUNC
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CUsbPersonalityNotifier* CUsbPersonalityNotifier::NewL()
    {
    LOG_FUNC

    CUsbPersonalityNotifier* self = new ( ELeave ) CUsbPersonalityNotifier();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // pop self
    return self;
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
EXPORT_C CUsbPersonalityNotifier::~CUsbPersonalityNotifier()
    {
    LOG_FUNC
    
    Cancel();
    
    for( TInt i = 0; iNotifierClient.Count(); i++ )
        {
        delete iNotifierClient[i];
        }
        
    iNotifierClient.Reset();
    iNotifierClient.Close();
    }

// ----------------------------------------------------------------------------
// Called when information note or query is completed.
// ----------------------------------------------------------------------------
//
void CUsbPersonalityNotifier::RunL()
    {
    LOG_FUNC
        
    LOG1( "iStatus = %d", iStatus.Int() );
        
    if( iCallBack )
        {
        iCallBack->CallBack( iStatus.Int() );
        }
        
    iNotifier.CancelNotifier( iNotifierUid );
    iNotifier.Close();
    
    iState = EUsbPersonalityNotifierIdle;
    
    if( iRequestStatus )
        {
        User::RequestComplete( iRequestStatus, iStatus.Int() );
        iRequestStatus = NULL;
        }
    
    if( iNotifierClient.Count() )
        {        
        if( iNotifierClient[0]->iConfirmation )
            {
            DoShowQuery( iNotifierClient[0]->iCallBack,
                    iNotifierClient[0]->iNotifierUid,
                    iNotifierClient[0]->iBuffer,
                    iNotifierClient[0]->iResponse,
                    iNotifierClient[0]->iRequestStatus );
            }
        else
            {
            DoShowNote( iNotifierClient[0]->iNotifierUid,
                    iNotifierClient[0]->iBuffer,
                    iNotifierClient[0]->iResponse );
            }
        
        delete iNotifierClient[0];
        iNotifierClient.Remove( 0 );
        }
    }

// ----------------------------------------------------------------------------
// This method is never called in this implementation.
// ----------------------------------------------------------------------------
//
TInt CUsbPersonalityNotifier::RunError( TInt aError )
    {
    LOG_FUNC
    
    LOG1( "aError %d", aError );
    // Currently no leaving functions called in RunL, thus nothing should cause
    // this to be called -> return.
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// Called if there is outstanding request.
// ----------------------------------------------------------------------------
//
void CUsbPersonalityNotifier::DoCancel()
    {
    LOG_FUNC
    
    if( EUsbPersonalityNotifierStarted == iState )
        {
        LOG( "Canceling and closing notifier" );
        iNotifier.CancelNotifier( iNotifierUid );
        iNotifier.Close();
        iState = EUsbPersonalityNotifierIdle;
        
        if( iRequestStatus )
            {
            LOG( "Completing request" );
            User::RequestComplete( iRequestStatus, KErrCancel );
            iRequestStatus = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// Show query or queue it.
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUsbPersonalityNotifier::ShowQuery( TUid aNotifierUid,
        const TDesC8 &aBuffer, TDes8 &aResponse, 
        MUsbNotifierCallBack* aCallBack, TRequestStatus* aStatus )
    {
    LOG_FUNC

    TInt ret = KErrNone;
        
    if( aStatus )
        {
        // set to pending
        *aStatus = KRequestPending;
        }
    
    if( iState == EUsbPersonalityNotifierIdle )
        {
        // no request pending, don't que the requests
        ret = DoShowQuery( aCallBack, aNotifierUid, aBuffer, aResponse,
                aStatus );
        }
    else
        {
        // request pending, put request to the queue
        TNotifierClient* ptr;
        
        if( (ptr = new TNotifierClient( aCallBack, aNotifierUid, aBuffer,
                aResponse, aStatus, ETrue ) ) == NULL )
            {
            return KErrGeneral;
            }
                
        if( (ret = iNotifierClient.Append(ptr)) != KErrNone)
            {
            LOG( "append error");
            }
        
        }
    
    return ret;
    }

// ----------------------------------------------------------------------------
// Show note or queue it.
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUsbPersonalityNotifier::ShowNote( TUid aNotifierUid, 
        const TDesC8 &aBuffer, TDes8 &aResponse )
    {
    LOG_FUNC
        
    TInt ret = KErrNone;
    
    if( EUsbPersonalityNotifierIdle == iState )
        {
        // no request pending, don't que the requests
        ret = DoShowNote( aNotifierUid, aBuffer, aResponse );
        }
    else
        {
        // request pending, put request to the queue
        TNotifierClient* ptr;
        
        if( ( ptr = new TNotifierClient( NULL, aNotifierUid, aBuffer, 
                aResponse, NULL, EFalse ) ) == NULL )
            {
            return KErrGeneral;
            }
                
        if( ( ret = iNotifierClient.Append( ptr ) ) != KErrNone)
            {
            LOG( "append error");
            }
        }
    
    return ret;
    }

// ----------------------------------------------------------------------------
// Cancel currently on going query and all queued gueries and notes.
// ----------------------------------------------------------------------------
//
EXPORT_C void CUsbPersonalityNotifier::CancelAll()
    {
    LOG_FUNC

    Cancel();
    
    for (TInt i = 0; i < iNotifierClient.Count(); i++ )
        {
        LOG( "Deleting client entry" );
        
        if( iNotifierClient[i]->iRequestStatus )
            {
            LOG( "Completing request" );
            User::RequestComplete( iNotifierClient[i]->iRequestStatus, 
                KErrCancel );
            }
        
        delete iNotifierClient[i];
        iNotifierClient[i] = NULL;
        }
    
    iNotifierClient.Reset();
    
    }

// ----------------------------------------------------------------------------
// Cancel specific query.
// ----------------------------------------------------------------------------
//
EXPORT_C void CUsbPersonalityNotifier::CancelQuery( TUid aNotifierUid )
    {
    LOG_FUNC

    TBool done = EFalse;
    TInt i = 0;
    
    if( iNotifierUid == aNotifierUid )
        {
        Cancel();
        }

    while( !done )
        {
        for( i = 0; i < iNotifierClient.Count(); i++ )
            {
            LOG1( "CancelQuery i = %d", i );

            if(iNotifierClient[i]->iNotifierUid == aNotifierUid)
                {
                LOG( "Uid match" );
                if(iNotifierClient[i]->iRequestStatus)
                    {
                    User::RequestComplete(iNotifierClient[i]->iRequestStatus,
                        KErrCancel);
                    }

                delete iNotifierClient[i];
                iNotifierClient.Remove(i);
                break;
                }
            }
        
        if( i >= iNotifierClient.Count() )
            {
            done = ETrue;
            }
        }    
    }
    
// ----------------------------------------------------------------------------
// Cancel all other queued gueries and notes but the current.
// ----------------------------------------------------------------------------
//
EXPORT_C void CUsbPersonalityNotifier::CancelAllButCurrent()
    {
    LOG_FUNC

    if( iNotifierClient.Count() > 0 )
        {
        //The index 0 is the current, which is not deleted.
        for( TInt i = 1; i < iNotifierClient.Count(); i++ )
            {
           LOG( "Deleting client entry" );
            
            if( iNotifierClient[i]->iRequestStatus )
                {
                LOG( "Completing request" );
                User::RequestComplete( iNotifierClient[i]->iRequestStatus,
                        KErrCancel);
                }
            
            delete iNotifierClient[i];
        }

        //Remove all but the 1st
        TNotifierClient* ptr = iNotifierClient[0];
        iNotifierClient.Reset();
        TInt ret = iNotifierClient.Append( ptr );
        if (ret != KErrNone)
            {
            LOG("append error");
            }
        }
    }

// ----------------------------------------------------------------------------
// Return ETrue, if the notifier with the UID is currently showing.
// DEPRICATED
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CUsbPersonalityNotifier::IsShowing( TUid aNotifierUid )
    {
    LOG_FUNC

    return ( ( iState == EUsbPersonalityNotifierStarted ) 
        && ( iNotifierUid == aNotifierUid) );
    }

// ----------------------------------------------------------------------------
// Implementation for showing notes.
// ----------------------------------------------------------------------------
//
TInt CUsbPersonalityNotifier::DoShowNote( TUid aNotifierUid, 
    const TDesC8 &aBuffer, TDes8 &aResponse )
    {
    LOG_FUNC

    TInt ret = KErrNone;
            
    iCallBack = NULL;
    iNotifierUid = aNotifierUid;
    iRequestStatus = NULL;
    
    // initializations
    ret = iNotifier.Connect();
    
    if( ret != KErrNone )
        {
        LOG1( "ERROR: RNotifier::Connect = %d", ret );
        return ret;
        }
    
    ret = iNotifier.StartNotifier( aNotifierUid, aBuffer, aResponse );
    
    if( ret != KErrNone )
        {
        LOG1( "ERROR: StartNotifier() failed. Code: %d", ret );
        }
        
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, ret );
    SetActive();
    iState = EUsbPersonalityNotifierStarted;
    
    return ret;
    }

// ----------------------------------------------------------------------------
// Implementation for showing queries.
// ----------------------------------------------------------------------------
//
TInt CUsbPersonalityNotifier::DoShowQuery( MUsbNotifierCallBack* aCallBack, 
        TUid aNotifierUid, const TDesC8 &aBuffer, TDes8 &aResponse,
        TRequestStatus* aStatus )
    {
    LOG_FUNC
        
    TInt ret;
        
    iCallBack = aCallBack;
    iNotifierUid = aNotifierUid;
    iRequestStatus = aStatus;
    
    if( ( ret = iNotifier.Connect() ) != KErrNone )
        {
        LOG( "ERROR in notifier connection!" );
        return ret;
        }

    iNotifier.StartNotifierAndGetResponse( iStatus, aNotifierUid, aBuffer,
            aResponse );
    SetActive();
    iState = EUsbPersonalityNotifierStarted;
    
    return ret;
    }
    
// ----------------------------------------------------------------------------
// Constructor of TNotifierClient
// ----------------------------------------------------------------------------
//
CUsbPersonalityNotifier::TNotifierClient::TNotifierClient(
        MUsbNotifierCallBack* aCallBack, TUid aNotifierUid, 
        const TDesC8 &aBuffer, TDes8 &aResponse, TRequestStatus* aStatus,
        TBool aConfirmation )
    : iCallBack( aCallBack )
    , iNotifierUid( aNotifierUid )
    , iBuffer( aBuffer )
    , iResponse( aResponse )
    , iRequestStatus( aStatus )
    , iConfirmation( aConfirmation )
    {
    LOG_FUNC
        
    }

// End of file
