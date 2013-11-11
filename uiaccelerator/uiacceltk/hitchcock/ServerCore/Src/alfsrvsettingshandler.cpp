/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Settings handler
*
*/



#include "alfsrvsettingshandler.h"
#include "uiacceltkdomaincrkeys.h"
#include "alf/alfappui.h"

#include <centralrepository.h>

const TInt KAlfSrvDefaultRenderer = EHuiRendererBitgdi;
const TInt KAlfSrvDefaultFrameRate = 22;
const TInt KAlfSrvDefaultCpuUsage = 75;
const TInt KAlfSrvDefaultResourceCacheSize = 1000;
const TInt KAlfSrvDefaultComplexityFactor = 7;
const TInt KAlfSrvDefaultHwConfiguration = 0;

const TUint KAlfSrvRndBitMask = 0x00ff; // the lowest 8 bits are for the complexity factor

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfSrvSettingsHandler::CAlfSrvSettingsHandler( MAlfSrvSettingsObserver& aObserver )
 : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlfSrvSettingsHandler::ConstructL()
    {
    // TRAP before the cenrep key exists in builds
    TRAP_IGNORE( iCentralRepository = CRepository::NewL( KCRUidUIAccelTK ) )
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfSrvSettingsHandler* CAlfSrvSettingsHandler::NewL( MAlfSrvSettingsObserver& aObserver )
    {
    CAlfSrvSettingsHandler* self = new( ELeave ) CAlfSrvSettingsHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfSrvSettingsHandler::~CAlfSrvSettingsHandler()
    {
    Cancel();
    delete iCentralRepository;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CAlfSrvSettingsHandler::StartObserving()
    {
    if ( !IsActive() )
        {
        // order notification for all keys in repository  
        iCentralRepository->NotifyRequest(0x00000000, 0x00000000, iStatus);
        SetActive();
        }
    }

THuiRenderer CAlfSrvSettingsHandler::Renderer()
    {
    TInt result = KAlfSrvDefaultRenderer;
    if ( !iCentralRepository ) 
        {
        return static_cast<THuiRenderer>(result);
        }
        
    if ( iCentralRepository->Get( KUIAccelTKRenderer,result ) == KErrNone )
        {
        StartObserving();
        }
    
    return static_cast<THuiRenderer>(result);
    }
    
TUint CAlfSrvSettingsHandler::DefaultFramerate()
    {
    TInt result = KAlfSrvDefaultFrameRate;
    if ( !iCentralRepository ) 
        {
        return result;
        }
        
    if ( iCentralRepository->Get( KUIAccelTKDefaultFrameRate,result ) == KErrNone )
        {
        StartObserving();
        }
    
    return result;
    }
    
TUint CAlfSrvSettingsHandler::MaxCpuUsage()
    {
    // Update cached value only if it is not initialized
    TInt result = KAlfSrvDefaultCpuUsage;
    if (iMaxCpuUsage == 0)
    	{    	
    	if ( !iCentralRepository ) 
        	{
    		iMaxCpuUsage = result;
        	return result;
        	}
        	
    	if ( iCentralRepository->Get( KUIAccelTKMaxCpuUsage, result ) == KErrNone )
        	{
        	// Store value to cache (iMaxCpuUsage)
        	iMaxCpuUsage = result;
        	StartObserving();
        	}
    	}
    else
        {
        // Use cached value
        result = iMaxCpuUsage;    
        }	
    	
    return result;
    }
    
TUint CAlfSrvSettingsHandler::MaxResourceCacheSize()
    {
    TInt result = KAlfSrvDefaultResourceCacheSize;
    if ( !iCentralRepository ) 
        {
        return result;
        }
        
    if ( iCentralRepository->Get( KUIAccelTKMaxResourceCache,result ) == KErrNone )
        {
        StartObserving();
        }
    
    return result;
    }
    
TUint CAlfSrvSettingsHandler::EffectsComplexityFactor()
    {
    TInt result = KAlfSrvDefaultComplexityFactor;
    if ( !iCentralRepository ) 
        {
        return result;
        }
        
    if ( iCentralRepository->Get( KUIAccelTKEffectsComplexityFactor,result ) == KErrNone )
        {
        result = result&KAlfSrvRndBitMask; // remove Rnd flags
        
        StartObserving();
        }
    
    return result;
    }
    
TUint CAlfSrvSettingsHandler::HWDriverConfiguration()
    {
    TInt result = KAlfSrvDefaultHwConfiguration;
    if ( !iCentralRepository ) 
        {
        return result;
        }
        
    if ( iCentralRepository->Get( KUIAccelTKHWConfigurationFlags,result ) == KErrNone )
        {
        StartObserving();
        }
    
    return result;
    }
    
TUint CAlfSrvSettingsHandler::RndFlags()
    {
    TInt result = 0;
    if ( !iCentralRepository ) 
        {
        return result;
        }
        
    if ( iCentralRepository->Get( KUIAccelTKEffectsComplexityFactor,result ) == KErrNone )
        {
        result = result&~KAlfSrvRndBitMask; // remove complexity factor flags.
        
        StartObserving();
        }
    
    return result;
    }
    
// ---------------------------------------------------------------------------
// From class CActive 
// Active object RunL
// ---------------------------------------------------------------------------
//
void CAlfSrvSettingsHandler::RunL()
    {
    TInt newValue = KAlfSrvDefaultComplexityFactor;
    TAlfSrvSetting settingChanged = EAlfSrvSettingEffectsComplexityFactor;
    
    // iStatus should hold the information about the changed key. Get the new value.    
    switch ( iStatus.Int() )
        {
        case KUIAccelTKRenderer: // flow through
        case KUIAccelTKHWConfigurationFlags:
            // do not do anything
            return;
            
        case KUIAccelTKDefaultFrameRate:
            settingChanged = EAlfSrvSettingDefaultFramerate;
    		User::LeaveIfError( iCentralRepository->Get( iStatus.Int(), newValue ) );
            break;
        case KUIAccelTKMaxCpuUsage:
            settingChanged = EAlfSrvSettingMaxCpuUsage;
    		User::LeaveIfError( iCentralRepository->Get( iStatus.Int(), newValue ) );
    		iMaxCpuUsage = (TUint)newValue;
            break;
        case KUIAccelTKMaxResourceCache:
            settingChanged = EAlfSrvSettingMaxResourceCacheSize;
    		User::LeaveIfError( iCentralRepository->Get( iStatus.Int(), newValue ) );
            break;
        case KUIAccelTKEffectsComplexityFactor:
            settingChanged = EAlfSrvSettingEffectsComplexityFactor;
    		// get the new value    
    		User::LeaveIfError( iCentralRepository->Get( iStatus.Int(), newValue ) );
            newValue = newValue&KAlfSrvRndBitMask; // remove Rnd flags
            break;
        
        default:
            // Error?
            User::LeaveIfError( iStatus.Int() );
            return; 
        }
            
    StartObserving();
    iObserver.MAlfSrvUintSettingChangedL( settingChanged, newValue );
    }

    
void CAlfSrvSettingsHandler::DoCancel()
    {
    iCentralRepository->NotifyCancelAll();
    }

// ---------------------------------------------------------------------------
// From class CActive 
// Error handler for RunL
// ---------------------------------------------------------------------------
//
TInt CAlfSrvSettingsHandler::RunError(TInt aError)
    {
    // will likely lead to inconsistent state and thus reset anyway
    RDebug::Print(_L("CAlfSrvSettingsHandler::RunError( %d )"),aError);
    aError -= (aError);
    return aError; //KErrNone;
    }
