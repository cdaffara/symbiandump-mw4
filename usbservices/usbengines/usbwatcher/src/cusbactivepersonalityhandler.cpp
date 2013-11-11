/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This implements UsbActivePersonalityHandler class.
*
*/


// INCLUDE FILES
#include <etelmm.h>             // for fetching the IMEI code
#include <mmtsy_names.h>        // for RTelServer names
#include <UsbWatcherInternalCRKeys.h>
#include <cusbpersonalitynotifier.h>
#include <cusbpersonalityplugin.h>
#include <tusbpersonalityparams.h>
#include <startupdomainpskeys.h> //for global system state
#include "cusbactivepersonalityhandler.h"
#include "cusbglobalsystemstateobserver.h"
#include <usbuinotif.h>

// CONSTANTS
const TInt KSerialNumberLength = 12;

const TUid KUsbmanSvrUid = {0x101fe1db};

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CUsbActivePersonalityHandler::CUsbActivePersonalityHandler(
    RUsb& aUsbMan, CUsbWatcher& aOwner )
    : CActive( EPriorityStandard )
    , iUsbMan( aUsbMan )
    , iOwner( aOwner )
    , isFailureCleanup( EFalse )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::ConstructL()
    {
    LOG_FUNC

    iPersonalityNotifier = CUsbPersonalityNotifier::NewL();
    iPersonalityParams = new ( ELeave ) TUsbPersonalityParams( iUsbMan,
            *iPersonalityNotifier );

    ConstructUsbSerialNumberL();
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUsbActivePersonalityHandler* CUsbActivePersonalityHandler::NewL(
        RUsb& aUsbMan, CUsbWatcher& aOwner )
    {
    LOG_FUNC

    CUsbActivePersonalityHandler* self = new ( ELeave )
            CUsbActivePersonalityHandler( aUsbMan, aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // pop self
    return self;
    }

// Destructor
CUsbActivePersonalityHandler::~CUsbActivePersonalityHandler()
    {
    LOG_FUNC

    Cancel(); // cancel any outstanding requests

    delete iCurrentPersonalityHandler;
    delete iPersonalityNotifier;
    delete iPersonalityParams;
    }

// ----------------------------------------------------------------------------
// Construct USB serial number. Some of the personalities may need this.
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::ConstructUsbSerialNumberL()
    {
    LOG_FUNC

    RTelServer telServer;
    RMobilePhone phone;

    LEAVEIFERROR( telServer.Connect() );
    CleanupClosePushL( telServer );

    LEAVEIFERROR( telServer.LoadPhoneModule( KMmTsyModuleName ) );

    LEAVEIFERROR( phone.Open( telServer, KMmTsyPhoneName ) );
    CleanupClosePushL( phone );

    // to store the serial number to be published as the USB device's serial
    // number, fetch it from telephony server.
    TRequestStatus status;
    phone.GetPhoneId( status, iPhoneInfo );
    User::WaitForRequest( status );

    // make sure that the serial number fulfills USB requirements and then
    // convert the serial number so that it can be printed to log
    TLex lex( iPhoneInfo.iSerialNumber );
    TInt length = iPhoneInfo.iSerialNumber.Length();

    if( length < KSerialNumberLength )
        {
        // In GSM, the complete IMEI can be used as USB serial
        // number. But in CDMA, the ESN is too short for a valid Mass
        // Storage serial number (Mass-Storage and Bulk Only Transport
        // specs both require minimum 12 byte number), so it is
        // extended with trailing zeroes. When doing this, make sure
        // not to write anything over descriptor's max length
        if( iPhoneInfo.iSerialNumber.MaxLength() < KSerialNumberLength )
            {
            iPhoneInfo.iSerialNumber.SetLength( KSerialNumberLength );
            }
        while( length < KSerialNumberLength )
            {
            iPhoneInfo.iSerialNumber.Append( '0' );
            ++length;
            }
        }

    CleanupStack::PopAndDestroy( 2, &telServer );
    }

// ----------------------------------------------------------------------------
// Confirm that personality can be loaded.
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::ConfirmPersonalityUnload( TRequestStatus&
        aStatus )
    {
    LOG_FUNC

    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

    if( iState != EUsbPersonalityStarted )
        {
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete( status, KErrGeneral );
        return;
        }

    // Cancel all notes before confirmation
    iPersonalityParams->PersonalityNotifier().CancelAll();

    if( iCurrentPersonalityHandler )
        {
        iCurrentPersonalityHandler->ConfirmPersonalityUnload( iStatus );
        SetActive();
        }
    else
        {
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete( status, KErrNone );
        }
    }

// ----------------------------------------------------------------------------
// Start personality.
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::StartPersonality( TInt& aPersonalityId,
        TRequestStatus& aStatus )
    {
    LOG_FUNC

    LOG1( "PersonalityId = %d ", aPersonalityId);

    // Remove all notes.
    iPersonalityNotifier->CancelAll();
     

    iPersonalityId = &aPersonalityId;
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

    // prepare current personality for start and return
    if( iCurrentPersonalityHandler )
        {
        LOG( "Previous PersonalityHandler not deleted" );
        User::RequestComplete( iRequestStatus, KErrGeneral );
        return;
        }

    TRAPD( ret, ( iCurrentPersonalityHandler = NewPersonalityHandlerL(
            *iPersonalityId ) ) );

    if( ( ret == KErrNone) && iCurrentPersonalityHandler )
        {
        LOG( "PersonalityHandler created" );
        iCurrentPersonalityHandler->PreparePersonalityStart( iStatus );
        iState = EUsbPersonalityPrepareStart;
        isFailureCleanup = EFalse;
        SetActive();
        }
    else
        {
        LOG( "Error: PersonalityHandler create failed" );
        User::RequestComplete( iRequestStatus, KErrGeneral );
        }
    }

// ----------------------------------------------------------------------------
// Stop current personality.
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::StopPersonality( TRequestStatus& aStatus )
    {
    LOG_FUNC

    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

    iState = EUsbPersonalityPrepareStop;
    isFailureCleanup = EFalse;
    
    // prepare current personality for stop and return
    if( iCurrentPersonalityHandler )
        {
        LOG( "Call PersonalityPlugin to prepare stop" );
        iCurrentPersonalityHandler->PreparePersonalityStop( iStatus );
        SetActive();
        }
    else
        {
        LOG( "No current PersonalityPlugin, return immediately" );
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete( status, KErrNone );
        }
    }

// ----------------------------------------------------------------------------
// Indicates USB device state change to personality when USB is started.
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::StateChangeNotify(
        TUsbDeviceState aStateOld, TUsbDeviceState aStateNew )
    {
    LOG_FUNC

    iDeviceState = aStateNew;        
    switch ( aStateNew )
        {
        case EUsbDeviceStateConfigured:
            {
            if ((aStateOld != EUsbDeviceStateSuspended) && (ESwStateCharging
                    != CUsbGlobalSystemStateObserver::GlobalSystemState()))
                {
                iPersonalityParams->PersonalityNotifier().ShowQuery(
                        KCableConnectedNotifierUid, iDummy,
                        iPersonalityPckgBuf);
                }
            break;
            }
		case EUsbDeviceStateUndefined:
            {
            iPersonalityNotifier->CancelQuery(KQueriesNotifier);
            break;
			}
       default:
            // We do not handle other state here
			LOG( "DeviceStatechange ignored by ActivePersonalityhandler or EUsbDeviceStateConfigured" );
            break;
        }

    if( iCurrentPersonalityHandler )
        {
        LOG( "Notifying PersonalityPlugin of the new state" );
        iCurrentPersonalityHandler->StateChangeNotify( aStateNew );
        }
    }

// ----------------------------------------------------------------------------
// Standard active object error function. Handle error and complete
// outstanding request. We must not come here.
// ----------------------------------------------------------------------------
//
TInt CUsbActivePersonalityHandler::RunError( TInt aError )
    {
    LOG_FUNC
    
    LOG2("Returned error: %d, iState: %d", aError, iState);

    switch ( aError )
        {
        case KErrNoMemory:
            iQueryParams().iQuery = EUSBNotEnoughRam;
            iPersonalityParams->PersonalityNotifier().ShowQuery(KQueriesNotifier, 
    	                iQueryParams, iDummyBuf);
            break;
        case KErrDiskFull:
            if (ESwStateCharging
                    != CUsbGlobalSystemStateObserver::GlobalSystemState())
                {
                iQueryParams().iQuery = EUSBDiskFull;
                iPersonalityParams->PersonalityNotifier().ShowQuery(
                        KQueriesNotifier, iQueryParams, iDummyBuf);
                }
            break;
        default:
        	LOG( "Ignored" );
        }

    //only handle error when TryStart fails now
	//clean up work to be done in the personality
    if (iState == EUsbPersonalityStartUsb)
        {
        iState = EUsbPersonalityPrepareStop;
		isFailureCleanup = ETrue;
		
	    // prepare current personality for stop and return
	    if( iCurrentPersonalityHandler )
	        {
	        LOG( "Call PersonalityPlugin to prepare stop" );
	        iCurrentPersonalityHandler->PreparePersonalityStop( iStatus );
	        SetActive();
	        }
	    else
	    	{    
            LOG( "No current PersonalityPlugin" );
            }

        //complete StartPersonality with aError
        User::RequestComplete( iRequestStatus, aError );
        }
    else
        {
	    LOG( "Ignore error in other states" );
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// Executed when iStatus is completed.
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::RunL()
    {
    LOG_FUNC

    TInt ret = iStatus.Int();
    
    LOG2( "CUsbActivePersonalityHandler::RunL iStatus = %d, iState = %d", ret, iState );

    switch( iState )
        {
        case EUsbPersonalityIdle:
            break;

        case EUsbPersonalityPrepareStart:
            {
            LOG( "EUsbPersonalityPrepareStart" );
            // then write the serial number to P&S for USB Manager
            PublishSerialNumber();
            iUsbMan.TryStart( *iPersonalityId, iStatus );
            iState = EUsbPersonalityStartUsb;
            SetActive();
            }
            break;

        case EUsbPersonalityStartUsb:
            LEAVEIFERROR( ret );
            LOG( "EUsbPersonalityStartUsb" );
            iState = EUsbPersonalityFinishStart;
            if( iCurrentPersonalityHandler )
                {
                iCurrentPersonalityHandler->FinishPersonalityStart( iStatus );
                SetActive();
                }
            else
                {
                TRequestStatus* status = &iStatus;
                SetActive();
                User::RequestComplete( status, KErrNone );
                }
            break;

        case EUsbPersonalityFinishStart:
            LOG( "EUsbPersonalityFinishStart" );
            User::RequestComplete( iRequestStatus, ret );
            iState = EUsbPersonalityStarted;
            break;

        case EUsbPersonalityPrepareStop:
            LOG( "EUsbPersonalityPrepareStop" );
            iUsbMan.TryStop( iStatus );
            iState = EUsbPersonalityStopUsb;
            SetActive();
            break;

        case EUsbPersonalityStopUsb:
            LOG( "EUsbPersonalityStopUsb" );
            iState = EUsbPersonalityFinishStop;
            if( iCurrentPersonalityHandler )
                {
                iCurrentPersonalityHandler->FinishPersonalityStop( iStatus );
                SetActive();
                }
            else
                {
                TRequestStatus* status = &iStatus;
                SetActive();
                User::RequestComplete( status, KErrNone );
                }
            break;

        case EUsbPersonalityFinishStop:
            LOG( "EUsbPersonalityFinishStop" );

            delete iCurrentPersonalityHandler;
            iCurrentPersonalityHandler = NULL;

            if ( iDeviceState == EUsbDeviceStateUndefined )
                {
	            iPersonalityParams->PersonalityNotifier().CancelQuery(
	                    KCableConnectedNotifierUid );
                }
            //the request should be completed with error code in RunError if failed
            if ( !isFailureCleanup )
                {
                User::RequestComplete( iRequestStatus, ret );
                }
            
            iState = EUsbPersonalityIdle;
            break;

        case EUsbPersonalityStarted:
            // This must unload event. Let's complete.
            User::RequestComplete( iRequestStatus, ret );
            break;

        default:
            LOG( "ERROR: unexpected state" );
            PANIC( KErrGeneral );
            break;
        }
    }

// ----------------------------------------------------------------------------
// Standard active object cancellation function.
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::DoCancel()
    {
    LOG_FUNC

    LOG1( "CUsbActivePersonalityHandler::iState = %d", iState );
    switch( iState )
        {
        case EUsbPersonalityFinishStart:
            {
            TRequestStatus status;

            iUsbMan.TryStop( status );
            SetActive();
            User::WaitForRequest( status );
            }
            // Don't break. We need to cancel outstanding request.

        case EUsbPersonalityStarted:
        case EUsbPersonalityPrepareStop:
        case EUsbPersonalityFinishStop:
            if( iCurrentPersonalityHandler )
                {
                iCurrentPersonalityHandler->Cancel();
                }
            break;

        case EUsbPersonalityStopUsb:
            iUsbMan.CancelInterest( RUsb::ETryStop );
            break;

        case EUsbPersonalityStartUsb:
            iUsbMan.StartCancel();
            break;

        default:
            break;
        }

    if( iCurrentPersonalityHandler && ( iState != EUsbPersonalityStarted ) )
        {
        delete iCurrentPersonalityHandler;
        iCurrentPersonalityHandler = NULL;
        }

    // if started then this must unload confirmation
    if( iState != EUsbPersonalityStarted )
        {
        iState = EUsbPersonalityIdle;
        }

    // When cancel happens it means that we can cancel all queries & notes
    iPersonalityParams->PersonalityNotifier().CancelAll();

    User::RequestComplete( iRequestStatus, KErrCancel );
    }

// ----------------------------------------------------------------------------
// Creates and returns a class handler object for the given personality.
// ----------------------------------------------------------------------------
//
CUsbPersonality* CUsbActivePersonalityHandler::NewPersonalityHandlerL(
        TInt aPersonality )
    {
    LOG_FUNC

    TInt personalityCount = iOwner.Personalities().Count();

    for (TInt i = 0; i < personalityCount; i++)
        {
        const TUsbSupportedPersonalityInf& pinf = iOwner.Personalities()[i];
        if( pinf.iPersonalityId == aPersonality )
            {
            iPersonalityParams->SetPersonalityId( aPersonality );
            CUsbPersonalityPlugin* plugin = CUsbPersonalityPlugin::NewL(
                    *iPersonalityParams, pinf.iPersonalityUid );
            iUseSerialNumber = pinf.iUseSerialNumber;
            return plugin;
            }
        }

    return NULL;
    }

// ----------------------------------------------------------------------------
// Publish serial number for USB Manager, if needed.
// ----------------------------------------------------------------------------
//
TInt CUsbActivePersonalityHandler::PublishSerialNumber()
    {
    LOG_FUNC

    TInt err = KErrNone;

    if( !iUseSerialNumber && iSerialNumberWritten )
        {
        // We are either in test mode or going to start up PC Suite
        // personality -> delete USB Manager's serial number P&S key
        // (if necessary)
        LOG( "Deleting published USB serial number" );
        err = RProperty::Delete( KUidSystemCategory, KUsbmanSvrUid.iUid );
        iSerialNumberWritten = EFalse;
        }
    else if( iUseSerialNumber && !iSerialNumberWritten )
        {
        // to finish, define and write the serial number P&S so that USB
        // Manager can fetch it
        _LIT_SECURITY_POLICY_PASS( KAPReadPolicy );
        _LIT_SECURITY_POLICY_PASS( KAPWritePolicy );

        err = RProperty::Define( KUidSystemCategory, KUsbmanSvrUid.iUid,
                RProperty::EText, KAPReadPolicy, KAPWritePolicy,
                KUsbStringDescStringMaxSize );

        if( !err )
            {
            err = RProperty::Set( KUidSystemCategory, KUsbmanSvrUid.iUid,
                    iPhoneInfo.iSerialNumber );
            iSerialNumberWritten = ( err == KErrNone );
            }
        }

    LOG1(" ret = %d", err );

    return err;
    }

// ----------------------------------------------------------------------------
// Cancel cable connected notifier
// ----------------------------------------------------------------------------
//
void CUsbActivePersonalityHandler::CancelCableConnectedNotifier()
    {
    LOG_FUNC

    iPersonalityParams->PersonalityNotifier().CancelQuery(
            KCableConnectedNotifierUid );
    }

// End of file
