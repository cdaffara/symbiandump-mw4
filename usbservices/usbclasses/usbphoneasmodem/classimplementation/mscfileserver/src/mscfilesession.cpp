// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Implements a Session of a Symbian OS server for the RUsbMassStorage API
// 
// 

#include <e32property.h>
#include <coreapplicationuisdomainpskeys.h>
#include "mscfilesession.h"
#include "mscfilecontroller.h"
#include "mscfileserver.h"
#include "usbmscfileshared.h"
#include "debug.h"

/**
 Construct a Symbian OS session object.
 
 @param	aServer		Service the session will be a member of
 @param	aMessage	The message from the client.
 @return	A new CMscFileSession object
 */
CMscFileSession* CMscFileSession::NewL(CMscFileServer& aServer)
	{
	CMscFileSession* r = new (ELeave) CMscFileSession(aServer);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

/**
 Constructor.
 
 @param	aServer	Service the session will be a member of
 */
CMscFileSession::CMscFileSession(CMscFileServer& aServer)
	: iMscFileServer( aServer )
	{
    TRACE_FUNC
	}


/**
 2nd Phase Construction.
 */
void CMscFileSession::ConstructL()
	{
	TRACE_FUNC
	iMscFileServer.IncrementSessionCount();
    if ( iMscFileServer.SessionCount() > 1 )
        {
        TRACE_ERROR(( _L( "SessionCount: %d" ), iMscFileServer.SessionCount() ))
        // Only one session is allowed
        User::Leave( KErrInUse );
        }        
 	}


/**
 Destructor.
 */
CMscFileSession::~CMscFileSession()
	{
	iMscFileServer.DecrementSessionCount();
	}

/**
 Called when a message is received from the client.
 
 @param	aMessage	Message received from the client
 */
void CMscFileSession::ServiceL( const RMessage2& aMessage )
	{
	TRAPD( err, DispatchMessageL( aMessage ) );
	
    aMessage.Complete( err );
	}

void CMscFileSession::DispatchMessageL( const RMessage2& aMessage )
    {
    switch (aMessage.Function())
        {
        case EMscFileSetupLu:
            SetupLogicalUnitL( aMessage );
            break;
            
        case EMscFileStart:
            StartL( aMessage );
            break;
            
        case EMscFileStop:
            LEAVE_IF_ERROR( Stop() );
            break;
            
        case EMscFileShutdown:
            LEAVE_IF_ERROR( Shutdown() );
            break;

        default:
            aMessage.Panic( KUsbMsCliPncCat, EUsbMsPanicIllegalIPC );
            break;
        }

    }

void CMscFileSession::SetupLogicalUnitL( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    TInt protocol;
    TInt lun;
    RBuf fullImageFileName;
    TInt len = aMessage.GetDesLength( 0 );
    fullImageFileName.CreateL( len );
    fullImageFileName.CleanupClosePushL();
    aMessage.ReadL( 0, fullImageFileName );
    protocol = aMessage.Int1();
    lun = aMessage.Int2();
    
    iMscFileServer.Controller().SetupLogicalUnitL( fullImageFileName, protocol, lun );
    CleanupStack::PopAndDestroy( &fullImageFileName );
    TRACE_FUNC_EXIT
    }
/**
 Client request to start the device.
 
 @return	Any error that occurred or KErrNone
 */
void CMscFileSession::StartL( const RMessage2& aMessage )
	{
	TRACE_FUNC

    User::LeaveIfError(RProperty::Set(KPSUidCoreApplicationUIs, 
        KCoreAppUIsUSBFileTransfer, 
        ECoreAppUIsUSBFileTransferActive));
	
	TMassStorageConfig msConfig;
	GetMsConfigL( aMessage, msConfig );
	LEAVE_IF_ERROR( iMscFileServer.Controller().Start( msConfig ) );
	}

/**
 Client request to stop the device.
 
 @return	Any error that occurred or KErrNone
 */
TInt CMscFileSession::Stop()
    {
    TRACE_FUNC

    RProperty::Set(KPSUidCoreApplicationUIs, 
        KCoreAppUIsUSBFileTransfer, 
        ECoreAppUIsUSBFileTransferNotActive);

	return iMscFileServer.Controller().Stop();
	}

/**
 Client request to shut down the server
 
 @return KErrNone
 */
TInt CMscFileSession::Shutdown()
    {
    TRACE_FUNC
    CActiveScheduler::Stop();
	return KErrNone;
	}

 /**
  Get mass storage configuration data from the received message
  */
 void CMscFileSession::GetMsConfigL( const RMessage2& aMessage, 
                                     TMassStorageConfig& aMsStorage )
 	{
 	aMessage.ReadL( 0, aMsStorage.iVendorId );
 	aMessage.ReadL( 1, aMsStorage.iProductId );
 	aMessage.ReadL( 2, aMsStorage.iProductRev );
 	}
