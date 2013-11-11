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
* Description:   Manager for cross-session resources.
*
*/




#include "alfsrvresourcemanager.h"
#include "alflogger.h"
#include "alf/alfappsrvsessionbase.h"
#include <uiacceltk/HuiDisplay.h>

// Internal flags
enum TAlfSchedulerFlags
    {
    EAlfSchedulerFlagPrimaryDisplayLightsOn     = 0x01,
    EAlfSchedulerFlagSecondaryDisplayLightsOn   = 0x02,
    EAlfSchedulerFlagApplicationForeground      = 0x04
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSrvResourceManager::CAlfSrvResourceManager( CHuiEnv& aEnv )
 : iEnv( aEnv )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// Checks the initial flag states.
// ---------------------------------------------------------------------------
//
void CAlfSrvResourceManager::ConstructL()
    {
	// TODO: Causes random crash during boot, revise
    // iLight = CHWRMLight::NewL(this); // Calls LightStatusChanged()
    
    iFlags |= EAlfSchedulerFlagApplicationForeground;

    // Workaround for non-working light status reports  
    iFlags |= EAlfSchedulerFlagPrimaryDisplayLightsOn;
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfSrvResourceManager* CAlfSrvResourceManager::NewL( CHuiEnv& aEnv )
    {
    CAlfSrvResourceManager* self = new( ELeave ) CAlfSrvResourceManager( aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvResourceManager::~CAlfSrvResourceManager()
    {
    // delete iLight;
    iActiveSession = NULL;
    }


// ---------------------------------------------------------------------------
// Called when the application server's window visibility changes.
// ---------------------------------------------------------------------------
//
void CAlfSrvResourceManager::SetServerWindowPartiallyVisible( TBool aVisible )
    {
    __ALFLOGSTRING1( "CAlfSrvResourceManager::SetServerWindowPartiallyVisible %d", aVisible )
    if ( aVisible )
        {
        iFlags |= EAlfSchedulerFlagApplicationForeground;
        }
    else
        {
        iFlags &= ~EAlfSchedulerFlagApplicationForeground;
        }
        
    UpdateSchedulerState();
    }
 
// ---------------------------------------------------------------------------
// Called when active session changes.
// ---------------------------------------------------------------------------
//   
void CAlfSrvResourceManager::SetActiveSession( CAlfAppSrvSessionBase* aSession )
    {
    __ALFLOGSTRING1( "CAlfSrvResourceManager::SetActiveSession 0x%x", aSession )
    iActiveSession = aSession;
    }
  
// ---------------------------------------------------------------------------
// Checks the flags and restarts/pauses the scheduler accordingly.
// ---------------------------------------------------------------------------
//  
void CAlfSrvResourceManager::UpdateSchedulerState()
    {
    if ( LightsOn() && 
         iFlags&EAlfSchedulerFlagApplicationForeground )
        {
        // Run the scheduler
        Run();
        }
    // Run the scheduler if there is observer to Screen Buffer.
    // refer CAlfScreenBuffer and CAlfScreenBufferEventFetcher classes 
    else if(LightsOn() && iEnv.DisplayCount() > 0 && iEnv.PrimaryDisplay().ScreenBufferObserver() )
        {
        Run();
        }
    else
        {
        // Pause schduler
        Pause();
        }
    }

// ---------------------------------------------------------------------------
// Restarts the scheduler.
// ---------------------------------------------------------------------------
//    
void CAlfSrvResourceManager::Run()
    {
    __ALFLOGSTRING( "CAlfSrvResourceManager::Run")
    if ( iActiveSession )
        {
        if ( iEnv.RefreshMode() != iActiveSession->PreferredRefreshMode() )
            {
            iEnv.SetRefreshMode( iActiveSession->PreferredRefreshMode() );
            }
        }
    else
        {
        iEnv.SetRefreshMode( EHuiRefreshModeAutomatic );
        }
    }

// ---------------------------------------------------------------------------
// Pauses the scheduler.
// Current implementation set the refresh mode to manual.
// ---------------------------------------------------------------------------
//    
void CAlfSrvResourceManager::Pause()
    {
    if ( iEnv.RefreshMode() == EHuiRefreshModeManual )
        {
        return; // Already paused
        }
        
    __ALFLOGSTRING( "CAlfSrvResourceManager::Pause")
#ifndef SYMBIAN_BUILD_GCE
    iEnv.SetRefreshMode( EHuiRefreshModeManual );
#endif
    }

// ---------------------------------------------------------------------------
// Checks if either of the display lights are on.
// ---------------------------------------------------------------------------
// 
TBool CAlfSrvResourceManager::LightsOn() const
    {
    if ( iFlags&EAlfSchedulerFlagPrimaryDisplayLightsOn ||
         iFlags&EAlfSchedulerFlagSecondaryDisplayLightsOn )
        {
        return ETrue;
        }
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// From class MHWRMLightObserver.
// Called when the lights change.
// ---------------------------------------------------------------------------
//
void CAlfSrvResourceManager::LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus)
    {
    // If set at the end of the function, the UpdateSchedulerState() is called.
    TBool updateScheduler = EFalse;
    
    // Check the primary display
    if( aTarget&CHWRMLight::EPrimaryDisplay )
        {
        if( aStatus == CHWRMLight::ELightOn ||
            aStatus == CHWRMLight::ELightStatusUnknown )
            {
            iFlags |= EAlfSchedulerFlagPrimaryDisplayLightsOn;
            __ALFLOGSTRING( "CAlfSrvResourceManager::LightStatusChanged Primary ON")
            }
        else if( aStatus == CHWRMLight::ELightOff )
            {
            iFlags &= ~EAlfSchedulerFlagPrimaryDisplayLightsOn;
            __ALFLOGSTRING( "CAlfSrvResourceManager::LightStatusChanged Primary OFF")
            }
        else
            {
            // for PC lint
            }
        updateScheduler = ETrue;
        }
    
    // Check the secondary display
    if ( aTarget&CHWRMLight::ESecondaryDisplay )
        {
        if( aStatus == CHWRMLight::ELightOn ||
            aStatus == CHWRMLight::ELightStatusUnknown )
            {
            iFlags |= EAlfSchedulerFlagSecondaryDisplayLightsOn;
            __ALFLOGSTRING( "CAlfSrvResourceManager::LightStatusChanged Secondary ON")
            }
        else if( aStatus == CHWRMLight::ELightOff )
            {
            iFlags &= ~EAlfSchedulerFlagSecondaryDisplayLightsOn;
            __ALFLOGSTRING( "CAlfSrvResourceManager::LightStatusChanged Secondary OFF")
            }
        else
            {
            // for PC lint
            }
        updateScheduler = ETrue;
        }
        
    // update the scheduler state if the light status has been changed.
    if ( updateScheduler )
        {
        UpdateSchedulerState();
        }
    }

