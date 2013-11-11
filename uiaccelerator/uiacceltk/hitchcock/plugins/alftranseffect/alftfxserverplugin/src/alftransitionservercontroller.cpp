/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the implementation of the API used by theme server
*                to control transition appearance.
*
*/



#include "alftransitionservercontroller.h"
#include "alftransitionserver.h"
#include "alftfxserverdef.h"
#include "alfpsobserver.h"

#include <e32property.h>
#include <avkondomainpskeys.h>
#include <pslninternalcrkeys.h>
#include <centralrepository.h>

#include <alf/alfclientbase.h>
#include <alflogger.h>
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfTransitionServerController* CAlfTransitionServerController::NewL(RWsSession& /*aWsSession*/)
    {
    __ALFFXLOGSTRING("CAlfTransitionServerController::NewL >>");
    CAlfTransitionServerController* self = new (ELeave) CAlfTransitionServerController();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    __ALFFXLOGSTRING("CAlfTransitionServerController::NewL <<");
    return self;
    }
    
CAlfTransitionServerController::~CAlfTransitionServerController()
    {
    __ALFFXLOGSTRING("CAlfTransitionServerController::~CAlfTransitionServerController >>");
    delete iTimer;
	delete iPsObserver;
	
    if(ServerRunning()) 
        {
        StopTransitionServer();
        }
            
	delete iRepository;
    __ALFFXLOGSTRING("CAlfTransitionServerController::~CAlfTransitionServerController <<");
    }

CAlfTransitionServerController::CAlfTransitionServerController()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfTransitionServerController::ConstructL()
    {
    __ALFFXLOGSTRING("CAlfTransitionServerController::ConstructL >>");
    iRepository = CRepository::NewL( KCRUidThemes );
    TInt err = iRepository->Get( KThemesTransitionEffects, iEffectValue );
    if ( err != 0 )
        {
        // Clean the value just to be sure it did not get messed up
        iEffectValue = 0;
        }
    __ALFFXLOGSTRING("CAlfTransitionServerController::ConstructL <<");
    }
    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerController::StartTransitionServer()
    {
	return StartTransitionServer(KDefaultMaxBytesOngoing, KDefaultMaxBytesBetween);
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerController::StopTransitionServer()
    {
    __ALFFXLOGSTRING("CAlfTransitionServerController::StopTransitionServer >>");
    //Shutting down.
	//Set property to "server not running"
    TInt val = 0;
    
    RProperty::Get(KPSUidAvkonDomain, KAknTfxServerStatus, val);
    iTransitionControlValue = val & ~ETfxSrvRunning;
	RProperty::Set(KPSUidAvkonDomain, KAknTfxServerStatus, iTransitionControlValue);
	__ALFFXLOGSTRING("CAlfTransitionServerController::StopTransitionServer <<");
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CAlfTransitionServerController::ServerRunning()
    {
    __ALFFXLOGSTRING("CAlfTransitionServerController::ServerRunning >>");
    TInt value = 0;
    RProperty::Get(KPSUidAvkonDomain, KAknTfxServerStatus, value);
    __ALFFXLOGSTRING1("CAlfTransitionServerController::ServerRunning - return: %d <<", value & ETfxSrvRunning);
    return (value & ETfxSrvRunning);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
TInt CAlfTransitionServerController::StartTransitionServer(TInt32 /*aMaxBytesOngoing*/, 
																 TInt32 /*aMaxBytesBetween*/)
	{
	__ALFFXLOGSTRING("CAlfTransitionServerController::StartTransitionServer >>");
	if( ServerRunning() )
		{
		__ALFFXLOGSTRING("CAlfTransitionServerController::StartTransitionServer - return KErrAlreadyExists <<");
		return KErrAlreadyExists;
		}
		
	TInt val = 0;
	TInt err = KErrNone;
	

    err = RProperty::Get(KPSUidAvkonDomain, KAknTfxServerStatus, val );
    if ( err != KErrNone )
        {
    	err = RProperty::Define(KPSUidAvkonDomain, KAknTfxServerStatus, RProperty::EInt, TSecurityPolicy(TSecurityPolicy::EAlwaysPass),TSecurityPolicy(TSecurityPolicy::EAlwaysPass));
    	// make sure the value is cleared if this is the first time the property is defined
    	val = 0;
        }
	
	if ( err == KErrAlreadyExists )
	    {
	    // if our publish/subscribe variable already exists, all is well
	    err = KErrNone;
	    }
	
	if( err != KErrNone )
		{
		__ALFFXLOGSTRING1("CAlfTransitionServerController::StartTransitionServer - return: %d <<", err);
		return err;
		}
		
	//Set property to "server running"
    iTransitionControlValue = val | ETfxSrvRunning;
    // if the plugin has already been loaded, don't mess up with the value
	RProperty::Set( KPSUidAvkonDomain, KAknTfxServerStatus,  val | ETfxSrvRunning );
    	
    if ( !iPsObserver )
        {
       	iPsObserver = CAlfPsObserver::New( this, KPSUidAvkonDomain, KAknTfxServerStatus );
    	if( !iPsObserver )
    		{
    		__ALFFXLOGSTRING("CAlfTransitionServerController::StartTransitionServer - return KErrNoMemory<<");
    		return KErrNoMemory;
    		}
        }
    
    if ( !iIsConnected )
        {
        err = iTfxServer.Open();
        }
        
    if ( err == KErrNone )
        {
        iIsConnected = ETrue;        
        // We don't need the server, we only want to know if it is running.
        iTfxServer.Close();
        }
    else
        {
        err = KErrNone;
        // start timer
        if ( !iTimer )
            {
            iTimer = CPeriodic::New( CActive::EPriorityStandard );
            if ( iTimer )
                {
                iTimer->Start( 1000000, 1000000,
                    TCallBack(CAlfTransitionServerController::ConnectionCallback, this ) );
                }
            }
        }
    __ALFFXLOGSTRING1("CAlfTransitionServerController::StartTransitionServer - return: %d <<", err);
    return err;
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerController::ConnectionCallback( TAny* aParameter )
    {
    __ALFFXLOGSTRING("CAlfTransitionServerController::ConnectionCallback");
    CAlfTransitionServerController* me = ((CAlfTransitionServerController*)(aParameter));
    return me->ConnectionAttempt();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerController::ConnectionAttempt()
    {
    __ALFFXLOGSTRING("CAlfTransitionServerController::ConnectionAttempt >>");
    TInt err = iTfxServer.Open();
    if ( err == KErrNone )
        {
        iIsConnected = ETrue;        
        __ALFFXLOGSTRING("CAlfTransitionServerController::ConnectionAttempt - Connected");
        // We don't need the server, we only want to know if it is running.
        iTfxServer.Close();
        delete iTimer;
        iTimer = NULL;
        err = iRepository->Get( KThemesTransitionEffects, iEffectValue );
        if ( err != 0 )
            {
            // Clean the value just to be sure it did not get messed up
            iEffectValue = 0;
            }
        iRepository->Set( KThemesTransitionEffects, KMaxTInt ); 
        iRepository->Set( KThemesTransitionEffects, iEffectValue );   
        }
    __ALFFXLOGSTRING1("CAlfTransitionServerController::ConnectionAttempt - state: %d, but returning KErrNone <<", err);
    return KErrNone;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfTransitionServerController::PsValueUpdated( const TUid /*aCategory*/, const TUint /*aKey*/, const TInt /*aVal*/ )
	{
	// This may not be needed as we poll the starting of alf.
	}


    


