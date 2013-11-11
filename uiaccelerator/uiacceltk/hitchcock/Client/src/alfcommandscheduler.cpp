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
* Description:   Command scheduler.
*
*/



#include "alfcommandscheduler.h"
#include "alf/alfcommand.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

// Internal flags.
enum TAlfSchedulerFlags
   {
   EAlfSchedulerPrimaryLightsOn        = 0x01,
   EAlfSchedulerSecondaryLightsOn      = 0x02,
   EAlfSchedulerApplicationForeground  = 0x04,
   EAlfSchedulerRunning                = 0x08
   };

// ======== CTimedEvent MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CAlfCommandScheduler::CTimedEvent* CAlfCommandScheduler::CTimedEvent::NewLC( 
            const TAlfCommand& aCommand,
            CAlfCommandScheduler& aScheduler )
    {
    CTimedEvent* self = new (ELeave) CTimedEvent(aScheduler);
    CleanupStack::PushL( self );
    self->ConstructL( aCommand );
    return self;
    }
  
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//  
CAlfCommandScheduler::CTimedEvent::~CTimedEvent()
    {
    Cancel();
    delete iCommand;
    }
  
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//  
CAlfCommandScheduler::CTimedEvent::CTimedEvent( CAlfCommandScheduler& aScheduler )
 : CTimer( EPriorityStandard ), iScheduler( aScheduler )
    {
    CActiveScheduler::Add( this );
    }
  
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//  
void CAlfCommandScheduler::CTimedEvent::ConstructL(const TAlfCommand& aCommand)
    {
    CTimer::ConstructL();
    
    // Make a copy of the command.
    TAlfCommand* data = (TAlfCommand*) new (ELeave) TUint8[aCommand.Size()];
    Mem::Copy((TAny*)data, (TAny*)&aCommand, aCommand.Size());
    iCommand = data;
    }
 
// ---------------------------------------------------------------------------
// Called when timer triggers.
// ---------------------------------------------------------------------------
//   
void CAlfCommandScheduler::CTimedEvent::RunL()
    {
    iScheduler.ExecuteEventL( *iCommand );
    delete this;
    }

// ---------------------------------------------------------------------------
// Called when RunL leaves
// ---------------------------------------------------------------------------
//     
TInt CAlfCommandScheduler::CTimedEvent::RunError(TInt aError)
    {
    __ALFLOGSTRING1( "CAlfCommandScheduler::ExecuteEventL leaves with %d", aError )
    delete this;
    return aError;
    }

// ---------------------------------------------------------------------------
// Execute command after given timed.
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::CTimedEvent::ExecuteAfter( 
    TInt aIntervalInMilliSeconds, 
    TBool aStartPaused )
    {
    __ASSERT_ALWAYS( !IsActive(), USER_INVARIANT() );
    iTimeLeftInMilliSeconds = aIntervalInMilliSeconds; 
    
    // If the scheduler is not paused...
    if ( !aStartPaused )
        {
        // ...start the timer.
        After( aIntervalInMilliSeconds * 1000 );
        iLastStartTime.UniversalTime(); // Update the start-time stamp.
        }
    }

// ---------------------------------------------------------------------------
// Pauses the time
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::CTimedEvent::Pause()
    {
    // If not paused...
    if ( IsActive() )
        {
        // ... cancel the timer and update the time left.
        Cancel();
        TTime timeNow;
        timeNow.UniversalTime();
        iTimeLeftInMilliSeconds -= 
            timeNow.MicroSecondsFrom( iLastStartTime ).Int64()/1000;
        }
    }

// ---------------------------------------------------------------------------
// Continue timer.
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::CTimedEvent::Continue()
    {
    // If not running...
    if ( !IsActive() )
        {
        // ... execute the event...
        if ( iTimeLeftInMilliSeconds <= 0 )
            {
            // ...now
            TRAP_IGNORE(iScheduler.ExecuteEventL( *iCommand ))
            delete this;
            }
        else
            {
            // ...after the time left.
            After( iTimeLeftInMilliSeconds * 1000 );
            iLastStartTime.UniversalTime(); // Update the start-time stamp.
            }
        }
    }

// ---------------------------------------------------------------------------
// Calculates the time left.
// ---------------------------------------------------------------------------
//    
TInt CAlfCommandScheduler::CTimedEvent::TimeLeftInMilliSeconds() const
    {
    TInt timeLeftInMilliSeconds = 0;
    if ( IsActive() )
        {
        TTime timeNow;
        timeNow.UniversalTime();
        timeLeftInMilliSeconds = 
            iTimeLeftInMilliSeconds - 
            timeNow.MicroSecondsFrom( iLastStartTime ).Int64()/1000;
        }
    else
        {
        timeLeftInMilliSeconds = iTimeLeftInMilliSeconds;
        }
             
    if ( timeLeftInMilliSeconds < 0 )
        {
        timeLeftInMilliSeconds = 0;
        }
    return timeLeftInMilliSeconds;
    }

// ======== CAlfCommandScheduler MEMBER FUNCTIONS ========
    
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfCommandScheduler* CAlfCommandScheduler::NewL( CAlfEnv& aEnv)
    {
    CAlfCommandScheduler* self = new (ELeave) CAlfCommandScheduler( aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//   
CAlfCommandScheduler::CAlfCommandScheduler( CAlfEnv& aEnv)
 : iEnv(aEnv)
    {
    }
 
// ---------------------------------------------------------------------------
// ConstructL
// Update internal flags.
// ---------------------------------------------------------------------------
//   
void CAlfCommandScheduler::ConstructL()
    {
    iLight = CHWRMLight::NewL(this); // Calls LightStatusChanged()
    
    iFlags |= EAlfSchedulerApplicationForeground;
    iFlags |= EAlfSchedulerRunning;

    // Workaround for non-working light status reports  
    iFlags |= EAlfSchedulerPrimaryLightsOn; 
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CAlfCommandScheduler::~CAlfCommandScheduler()
    {
    iEvents.ResetAndDestroy();
    delete iLight;
    }
 
// ---------------------------------------------------------------------------
// Executes given command after given time
// ---------------------------------------------------------------------------
//   
void CAlfCommandScheduler::ScheduleCommandL( 
    const TAlfCommand& aCommand, 
    TInt aTimeInMilliSeconds )
    {
    if (aTimeInMilliSeconds == 0)
        {
        __ALFLOGSTRING( "CAlfCommandScheduler::ScheduleCommandL execute now" )
        // Execute now
        aCommand.ExecuteL( iEnv );
        return;
        }
    
    // Create timed event
    __ALFLOGSTRING1( "CAlfCommandScheduler::ScheduleCommandL in %dms", aTimeInMilliSeconds )
    CTimedEvent* event = CTimedEvent::NewLC( aCommand, *this );
    iEvents.AppendL( event );
    CleanupStack::Pop( event );
    event->ExecuteAfter( aTimeInMilliSeconds, !(iFlags&EAlfSchedulerRunning) );
    }

// ---------------------------------------------------------------------------
// Cancel all object commands.
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::CancelCommands( TAny* aObject )
    {
    for ( TInt i = iEvents.Count() - 1; i >= 0; i--)
        {
        const TAlfObjectCommand* objectCommand = 
            iEvents[i]->iCommand->ObjectCommand();
        if(objectCommand && objectCommand->Object() == aObject)
            {
            delete iEvents[i];
            iEvents.Remove( i );
            }
        }
    iEvents.Compress();
    }

// ---------------------------------------------------------------------------
// Cancels specific operation for given object
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::CancelCommands(TAny* aObject, TAlfOp aCommandOperation)
    {
    for(TInt i = iEvents.Count() - 1; i >= 0; --i)
        {
        const TAlfObjectCommand* objectCommand = 
            iEvents[i]->iCommand->ObjectCommand();
        if(objectCommand && 
           objectCommand->Object() == aObject &&
           objectCommand->Operation() == aCommandOperation)
            {
            // Cancel this one.
            delete iEvents[i];
            iEvents.Remove( i );
            }
        }
    iEvents.Compress();
    }
    
// ---------------------------------------------------------------------------
// Cancels given command types from given object
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::CancelCommands(TAny* aObject,
                                   TAlfCommandType aCommandType,
                                   TInt aParam)
    {
    for(TInt i = iEvents.Count() - 1; i >= 0; --i)
        {
        const TAlfObjectCommand* objectCommand = 
            iEvents[i]->iCommand->ObjectCommand();

        if(objectCommand && 
           objectCommand->Object() == aObject &&
           objectCommand->Type() == aCommandType)
            {
            if(objectCommand->Type() == EAlfCommandTypeCustomEvent)
                {
                const TAlfCustomEventCommand* cec = 
                    (const TAlfCustomEventCommand*) objectCommand;
                    
                if(cec->Param() != aParam)
                    {
                    // Not this one, wrong parameter.
                    continue;
                    }
                }
            // Cancel this one.
            delete iEvents[i];
            iEvents.Remove( i );
            }
        }
        
    iEvents.Compress();
    }

// ---------------------------------------------------------------------------
// Returns time left ot the command
// ---------------------------------------------------------------------------
//     
TInt CAlfCommandScheduler::MilliSecondsUntilCommand( 
        TAny* aObject )
    {
    TInt returnValue = KErrNotFound;
    
    for ( TInt i = iEvents.Count() - 1; i >= 0; i--)
        {
        const TAlfObjectCommand* objectCommand = 
            iEvents[i]->iCommand->ObjectCommand();
        if(objectCommand && objectCommand->Object() == aObject)
            {
            TInt timeLeft = iEvents[i]->TimeLeftInMilliSeconds();
            if ( returnValue == KErrNotFound || timeLeft < returnValue )
                {
                returnValue = timeLeft;
                }
            }
        }
        
    return returnValue;
    }

// ---------------------------------------------------------------------------
// Returns time left ot the command
// ---------------------------------------------------------------------------
//    
TInt CAlfCommandScheduler::MilliSecondsUntilCommand( 
        TAny* aObject, 
        TAlfOp aCommandOperation )
    {
    TInt returnValue = KErrNotFound;
    
    for(TInt i = iEvents.Count() - 1; i >= 0; --i)
        {
        const TAlfObjectCommand* objectCommand = 
            iEvents[i]->iCommand->ObjectCommand();
        if(objectCommand && 
           objectCommand->Object() == aObject &&
           objectCommand->Operation() == aCommandOperation)
            {
            TInt timeLeft = iEvents[i]->TimeLeftInMilliSeconds();
            if ( returnValue == KErrNotFound || timeLeft < returnValue )
                {
                returnValue = timeLeft;
                }
            }
        }
    
    return returnValue;
    }

// ---------------------------------------------------------------------------
// Returns time left ot the command
// ---------------------------------------------------------------------------
//      
TInt CAlfCommandScheduler::MilliSecondsUntilCommand( 
        TAny* aObject, 
        TAlfCommandType aCommandType, 
        TInt aParam )
    {
    TInt returnValue = KErrNotFound;
    
    for(TInt i = iEvents.Count() - 1; i >= 0; --i)
        {
        const TAlfObjectCommand* objectCommand = 
            iEvents[i]->iCommand->ObjectCommand();

        if(objectCommand && 
           objectCommand->Object() == aObject &&
           objectCommand->Type() == aCommandType)
            {
            if(objectCommand->Type() == EAlfCommandTypeCustomEvent)
                {
                const TAlfCustomEventCommand* cec = 
                    (const TAlfCustomEventCommand*) objectCommand;
                    
                if(cec->Param() != aParam)
                    {
                    // Not this one, wrong parameter.
                    continue;
                    }
                }
            TInt timeLeft = iEvents[i]->TimeLeftInMilliSeconds();
            if ( returnValue == KErrNotFound || timeLeft < returnValue )
                {
                returnValue = timeLeft;
                }
            }
        }
    
    return returnValue;
    }
  
// ---------------------------------------------------------------------------
// Executes command
// ---------------------------------------------------------------------------
//  
void CAlfCommandScheduler::ExecuteEventL( TAlfCommand& aCommand )
    { 
    for ( TInt i = 0 ; i < iEvents.Count() ; i++ )
        {
        if ( iEvents[i]->iCommand == &aCommand )
            {
            iEvents.Remove( i );
            iEvents.Compress();
            break;
            }
        }
    
    aCommand.ExecuteL( iEnv );
    }     

// ---------------------------------------------------------------------------
// Called when foreground status changes.
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::AppicationOnForeground( TBool aForeground )
    {
    if ( aForeground )
        {
        iFlags |= EAlfSchedulerApplicationForeground;
        }
    else
        {
        iFlags &= ~EAlfSchedulerApplicationForeground;
        }
        
    UpdateSchedulerState();
    }

// ---------------------------------------------------------------------------
// Called when display light status changes.
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::LightStatusChanged(
    TInt aTarget, 
    CHWRMLight::TLightStatus aStatus )
    {
    if( aTarget&CHWRMLight::EPrimaryDisplay )
        {
        if( aStatus == CHWRMLight::ELightOn || 
            aStatus == CHWRMLight::ELightStatusUnknown )
            {
            iFlags |= EAlfSchedulerPrimaryLightsOn;
            }
        else if( aStatus == CHWRMLight::ELightOff )
            {
            iFlags &= ~EAlfSchedulerPrimaryLightsOn;
            }
        else
            {
            // for PC lint
            }
        }
        
    if( aTarget&CHWRMLight::ESecondaryDisplay )
        {
        if( aStatus == CHWRMLight::ELightOn || 
            aStatus == CHWRMLight::ELightStatusUnknown )
            {
            iFlags |= EAlfSchedulerSecondaryLightsOn;
            }
        else if( aStatus == CHWRMLight::ELightOff )
            {
            iFlags &= ~EAlfSchedulerSecondaryLightsOn;
            }
        else
            {
            // for PC lint
            }
        }
        
    UpdateSchedulerState();
    }

// ---------------------------------------------------------------------------
// Updates scheduling state based on the internal flags.
// ---------------------------------------------------------------------------
//    
void CAlfCommandScheduler::UpdateSchedulerState()
    {
    if ( (iFlags&EAlfSchedulerPrimaryLightsOn ||
          iFlags&EAlfSchedulerSecondaryLightsOn ) && 
         iFlags&EAlfSchedulerApplicationForeground )
        {
        // Run the scheduler
        Run();
        }
    else
        {
        // Pause schduler
        Pause();
        }
    }
 
// ---------------------------------------------------------------------------
// Restarts the scheduling.
// ---------------------------------------------------------------------------
//   
void CAlfCommandScheduler::Run()
    {
    if ( iFlags&EAlfSchedulerRunning )
        {
        return; // Already running
        }
        
    iFlags |= EAlfSchedulerRunning;

    for(TInt i = iEvents.Count() - 1; i >= 0; --i)
        {
        iEvents[i]->Continue();
        
        // Executing a command may have potentially altered the iEvents array,
        // e.g. application may have canceled other commands when one command was
        // executed. We must make sure that we don't overindex here !
        if (i > iEvents.Count() + 1)
            {
            i = iEvents.Count();    
            }
        }
    }
 
// ---------------------------------------------------------------------------
// Pause scheduling.
// ---------------------------------------------------------------------------
//   
void CAlfCommandScheduler::Pause()
    {
    if ( !(iFlags&EAlfSchedulerRunning) )
        {
        return; // Already paused
        }

    iFlags &= ~EAlfSchedulerRunning;
    for(TInt i = iEvents.Count() - 1; i >= 0; --i)
        {
        iEvents[i]->Pause();
        }

    }
