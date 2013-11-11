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
* Description:   AlfEffectUtils contains small classes that enable layout switch and transition effects.
*
*/
#include <e32cmn.h>
#include <s32mem.h>
#include "alfeffectutils.h"
#include <akntransitionutils.h>
#include <alflogger.h>

// Delay after which first end check is performed.
const TInt KRosterFreezeEndTimeoutInMs = 100;
// Interval between end checks.
const TInt KRosterFreezeEndIntervalInMs = 50;
// Maximum amount of end checks to be performed. 
// This can be lower, should be visible enough to see, if heuristics fail.
const TInt KRosterFreezeEndAttempts = 50;

// ---------------------------------------------------------
// CAlfRosterFreezeEndTimer
// ---------------------------------------------------------
//
CAlfRosterFreezeEndTimer::CAlfRosterFreezeEndTimer( CAlfBridge& aBridge )
    :CTimer ( EPriorityStandard ),
    iBridge( aBridge )
    {   
    }

void CAlfRosterFreezeEndTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

CAlfRosterFreezeEndTimer* CAlfRosterFreezeEndTimer::NewL( CAlfBridge& aBridge )
    {
    CAlfRosterFreezeEndTimer* self = new ( ELeave ) CAlfRosterFreezeEndTimer( aBridge );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfRosterFreezeEndTimer::~CAlfRosterFreezeEndTimer()
    {
    Cancel();        
    }

void CAlfRosterFreezeEndTimer::Start( TTimeIntervalMicroSeconds32 aPeriod, TCallBack aCallBack  )
    {
    iCallBack = aCallBack;
    __ALFLOGSTRING("CAlfRosterFreezeEndTimer::Start");
    if (!IsActive())
        {
        iSafeCounter = 0;
        iSafeCounterDelta = Min(1, aPeriod.Int() / (KRosterFreezeEndIntervalInMs*1000));
        After( aPeriod );
        }
    }

void CAlfRosterFreezeEndTimer::RunL()
    {
    __ALFLOGSTRING("CAlfRosterFreezeEndTimer::RunL");
	
	TBool timeout = EFalse;
	
	if (iSafeCounter >= 0)	
	    {
		iSafeCounter += iSafeCounterDelta;
		if (iSafeCounter >= KRosterFreezeEndAttempts)
			{
			timeout = ETrue;
			iSafeCounter = 0;
			}
		}

    if (!iBridge.IsLayoutSwitchReady(iSafeCounter) && !timeout)
        {
        __ALFLOGSTRING("CAlfRosterFreezeEndTimer::RunL - Not ready in new orientation. waiting 50ms more");
        iSafeCounterDelta = 1;
        After( KRosterFreezeEndIntervalInMs * 1000 );
        return;
        }
    else
        {
        __ALFLOGSTRING("CAlfRosterFreezeEndTimer::RunL - Ready in new orientation.");
        iCallBack.CallBack();
        }
    }

void CAlfRosterFreezeEndTimer::DoCancel()
    {
    CTimer::DoCancel();
    }



CAlfLayoutSwitchEffectCoordinator::CAlfLayoutSwitchEffectCoordinator( CAlfBridge& aBridge ) :
    iBridge( aBridge ),
    iLayoutSwitchEffectContext(AknTransEffect::ENone),
    iCurrentState( EStateIdle )
    {
    RThread me = RThread();
    iOriginalPriority = me.Priority();    
    me.Close();
    }

CAlfLayoutSwitchEffectCoordinator::~CAlfLayoutSwitchEffectCoordinator()
    {   
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::Blank
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::Blank(TBool aEnabled)
    {
    __ALFLOGSTRING1("CAlfLayoutSwitchEffectCoordinator::Blank %d", aEnabled);
    iBlankEnabled = aEnabled;
    Event( aEnabled ? EEventBlankOn : EEventBlankOff );
    }
    
// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::LowMemoryEvent
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::LowMemoryEvent()
    {
    __ALFLOGSTRING("CAlfLayoutSwitchEffectCoordinator::LowMemory");
    Event( EEventLowMemory );
    }
    
// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::BeginLayoutSwitch
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::BeginLayoutSwitch()
    {
    __ALFLOGSTRING("CAlfLayoutSwitchEffectCoordinator::BeginLayoutSwitch");
    Event( EEventLayoutSwitch );      
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::Event
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::Event(TEvent aEvent)
    {
    // States:
    // - EStateIdle - No layout switch effect ongoing
    // - EStateFreezeFx - Roster frozen, periodic fullscreen completeness checking
    // - EStateBlankFx - Roster frozen, expecting client to say when ready to unfreeze
    // - EStateThemeFx - Theme effect in use

    // Events:
    // - EEventBlankOn - Client requested blank on
    // - EEventBlankOff - Client requested blank off
    // - EEventLayoutSwitch - Layout switch noticed
    // - EEventLowMemory - Low memory indication
    // and implicit Finish - Effect finished
    
    // Transitions:
    // From EStateIdle:
    // - EEventBlankOn: EStateBlankFx
    // - EEventLayoutSwitch: If effect available, EStateThemeFx. Otherwise EStateFreezeFx.
    //
    // From EStateFreezeFx:
    // - implicit Finish - EStateIdle
    //
    // From EStateBlankFx:
    // - EEventBlankOff: EStateFreezeFx (we use freeze to ensure that result is ok)
    // - EEventLayoutSwitch: If effect available, EStateThemeFx. Otherwise EStateFreezeFx.
    //
    // From EStateThemeFx:
    // - EEventLowMemory: If blank still on, EStateBlankFx. Otherwise EStateFreezeFx.
    // - implicit Finish - EStateIdle
    //
    // There is special handling if layout switch event is received while previous
    // layout switch is ongoing.

    TState nextState = EStateIdle;
    
    switch ( iCurrentState )
        {    
    case EStateFreezeFx:
        nextState = NextFreezeState(aEvent);
        break;
        
    case EStateBlankFx:
        nextState = NextBlankState(aEvent);
        break;
        
    case EStateThemeFx:
        nextState = NextThemeState(aEvent);
        break;
        
    case EStateIdle:
    default:
        nextState = NextIdleState(aEvent);   
        break;
        }
    
    if ( nextState != iCurrentState )
        {
        Transition( nextState, iCurrentState );
        }
    else
        {
        // We stay in the same state, but still need special handling in a couple of scenarios
        if ( nextState == EStateFreezeFx )
            {
            HandleFreezeEvent( aEvent );
            }
        if ( nextState == EStateThemeFx )
            {
            HandleThemeEvent( aEvent );
            }                        
        }
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::HandleFreezeEvent
// Special handling for Freeze effect.
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::HandleFreezeEvent(TEvent aEvent)
    {
    if ( aEvent == EEventLayoutSwitch && iRosterFreezeEndTimer && iRosterFreezeEndTimer->IsActive() )
        {                                
        // New layout switch while previous is ongoing => restart freeze end timer.
        __ALFLOGSTRING("CAlfLayoutSwitchEffectCoordinator::HandleFreezeEvent restart timer");
        iRosterFreezeEndTimer->Cancel();
        iRosterFreezeEndTimer->Start(KRosterFreezeEndTimeoutInMs*1000, TCallBack(DoFreezeFinished, this));                
        }
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::HandleFreezeEvent
// Special handling for Theme effect.
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::HandleThemeEvent(TEvent aEvent)
    {
    if ( aEvent == EEventLayoutSwitch )
        {                                
        // If layout switch occurs while freeze end timer is ongoing, restart.
        if ( iRosterFreezeEndTimer && iRosterFreezeEndTimer->IsActive() )
            {
            __ALFLOGSTRING("CAlfLayoutSwitchEffectCoordinator::HandleThemeEvent restart timer");
            iRosterFreezeEndTimer->Cancel();
            iRosterFreezeEndTimer->Start(KRosterFreezeEndTimeoutInMs*1000, TCallBack(DoNextLayoutSwitchContext, this));                
            }

        // If layout switch occurs while theme effect is ongoing in exit phase, restart.
        if ( iLayoutSwitchEffectContext == AknTransEffect::ELayoutSwitchExit )
            {
            __ALFLOGSTRING("CAlfLayoutSwitchEffectCoordinator::HandleThemeEvent restart themefx");
            TBool oldLayoutSwitchNotCompleted = iLayoutSwitchNotCompleted;
            iLayoutSwitchNotCompleted = EFalse;
                
            Transition( EStateIdle, iCurrentState );
            Transition( EStateThemeFx, iCurrentState );
                
            if (oldLayoutSwitchNotCompleted)
                {
                iLayoutSwitchNotCompleted = ETrue;
                }  
            }
        }
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::Transition
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::Transition(
        TState aNewState, TState aPreviousState)
    {
    __ALFLOGSTRING2("CAlfLayoutSwitchEffectCoordinator::Transition from: %d to: %d", aPreviousState, aNewState);
    iCurrentState = aNewState;
        
    // Undo previous state - don't unfreeze roster.
    switch ( aPreviousState )
        {
    case EStateFreezeFx:
        if ( iRosterFreezeEndTimer )
            {
            iRosterFreezeEndTimer->Cancel();
            }
        break;
        
    case EStateBlankFx:        
        iBridge.iHuiEnv->iPauseDrawing = EFalse;
        break;
        
    case EStateThemeFx:
        {
        RThread me = RThread();
        me.SetPriority(iOriginalPriority);    
        me.Close();

        if ( iRosterFreezeEndTimer )
            {
            iRosterFreezeEndTimer->Cancel();
            }

        SetLayoutSwitchEffect( AknTransEffect::ENone );
        }
        break;
        
    case EStateIdle:
    default:    
        break;
        }
    
    // Do next state actions
    switch ( iCurrentState )
        {    
    case EStateFreezeFx:
        {
        if (!iRosterFreezeEndTimer)
            {
            TRAP_IGNORE(iRosterFreezeEndTimer = CAlfRosterFreezeEndTimer::NewL(iBridge));
            }
            
        if (iRosterFreezeEndTimer)
            {
            iRosterFreezeEndTimer->Cancel();
            
            FreezeRoster(ETrue);
                
            // Remove all other effects
            iBridge.HandleGfxStopEvent( EFalse );
            iBridge.CleanAllFxVisuals();

            // Set remove freeze timer
            __ALFLOGSTRING("CAlfLayoutSwitchEffectCoordinator::Transition - Freeze timer started");
            iRosterFreezeEndTimer->Start(KRosterFreezeEndTimeoutInMs*1000, TCallBack(DoFreezeFinished, this)); 
            }
        }
        break;
        
    case EStateBlankFx:
        {
        // Start blanker
        iBridge.iHuiEnv->iPauseDrawing = ETrue;
        FreezeRoster(ETrue);        
        }
        break;
        
    case EStateThemeFx:
        {
        // Boost priority so that we are able to draw more frames for the effect
        RThread me = RThread();
        me.SetPriority(EPriorityAbsoluteHigh);    
        me.Close();
            
        // Freeze visual content
        iBridge.iHuiEnv->Display(0).SetDirty();
        FreezeRoster(ETrue);
            
        // Remove all other effects
        iBridge.HandleGfxStopEvent( EFalse );
        iBridge.CleanAllFxVisuals();
            
        // Set first layout switch effect 
        SetLayoutSwitchEffect(AknTransEffect::ELayoutSwitchStart);
        }
        break;
        
    case EStateIdle:
    default:
        {
        // Undo everything to be sure
        RThread me = RThread();
        me.SetPriority(iOriginalPriority);    
        me.Close();
        
        SetLayoutSwitchEffect( AknTransEffect::ENone );
        if ( iRosterFreezeEndTimer )
            {
            iRosterFreezeEndTimer->Cancel();
            }            
        
        iBridge.iHuiEnv->iPauseDrawing = EFalse;
        FreezeRoster(EFalse);
        iBridge.iHuiEnv->Display(0).SetDirty();
        iBridge.SetVisualTreeVisibilityChanged(ETrue);
        iBridge.AsynchRefresh();
        }
        break;
        }    
    
    // Inform bridge about layout switch actions
    if ( aNewState != EStateIdle && aPreviousState == EStateIdle )
        {
        iLayoutSwitchNotCompleted = ETrue;
        iBridge.LayoutSwitchStart();
        }
    if ( aNewState == EStateIdle && iLayoutSwitchNotCompleted )
        {
        iLayoutSwitchNotCompleted = EFalse;
        iBridge.LayoutSwitchComplete();
        }
    
    __ALFLOGSTRING("CAlfLayoutSwitchEffectCoordinator::Transition end");        
    }



// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::NextIdleState
// ---------------------------------------------------------
//
CAlfLayoutSwitchEffectCoordinator::TState 
    CAlfLayoutSwitchEffectCoordinator::NextIdleState(
        TEvent aEvent)
    {
    TState state = EStateIdle;
    
    switch ( aEvent )
        {
    case EEventLayoutSwitch:
        if ( IsThemeEffectEnabled() )
            {
            state = EStateThemeFx;
            }
        else
            {
            state = EStateFreezeFx;
            }
        break;
        
    case EEventBlankOn:
        state = EStateBlankFx;
        break;
        
    case EEventBlankOff:
    case EEventLowMemory:
    default:
        break;     
        }
    
    return state;
    }
    
// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::NextFreezeState
// ---------------------------------------------------------
//
CAlfLayoutSwitchEffectCoordinator::TState 
    CAlfLayoutSwitchEffectCoordinator::NextFreezeState(
        TEvent aEvent)
    {
    TState state = EStateFreezeFx;
    
    switch ( aEvent )
        {
    case EEventBlankOn:
    case EEventLayoutSwitch:    
    case EEventBlankOff:
    case EEventLowMemory:
    default:
        break;     
        }
    
    return state;
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::NextBlankState
// ---------------------------------------------------------
//
CAlfLayoutSwitchEffectCoordinator::TState 
    CAlfLayoutSwitchEffectCoordinator::NextBlankState(
        TEvent aEvent)
    {
    TState state = EStateBlankFx;
    
    switch ( aEvent )
        {
    case EEventLayoutSwitch:
        if ( IsThemeEffectEnabled() )
            {
            state = EStateThemeFx;
            }
        else
            {
            state = EStateFreezeFx;
            }
        break;
    
    case EEventBlankOff:
        state = EStateFreezeFx;
        break;
        
    case EEventBlankOn:        
    case EEventLowMemory:
    default:
        break;     
        }
    
    return state;
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::NextThemeState
// ---------------------------------------------------------
//
CAlfLayoutSwitchEffectCoordinator::TState 
    CAlfLayoutSwitchEffectCoordinator::NextThemeState(
        TEvent aEvent)       
    {
    TState state = EStateThemeFx;
    
    switch ( aEvent )
        {
    case EEventLowMemory:
        state = iBlankEnabled ? EStateBlankFx : EStateFreezeFx;
        break;
            
    case EEventLayoutSwitch:    
    case EEventBlankOn:
    case EEventBlankOff:
    default:
        break;     
        }
    
    return state;
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::FreezeFinished
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::FreezeFinished()
    {
    Transition(EStateIdle, iCurrentState);
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::SetLayoutSwitchEffectL
//
// This method sets correct effect based on the given 
// layout switch context.
//
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::SetLayoutSwitchEffect(AknTransEffect::TContext aContext)
    {
    MHuiEffectable* effectable = iBridge.iHuiEnv->Display(0).Roster().Effectable();
    CHuiFxEffect* effect = NULL;
    CHuiFxEngine* engine = iBridge.iHuiEnv->EffectsEngine();
    
    if (!effectable || !engine)
        {
        return;
        }    
            
    // Update current context
    iLayoutSwitchEffectContext = aContext;           
    
    if (aContext == AknTransEffect::ENone)
        {
        // Just remove effect
        //RDebug::Print(_L("CAlfLayoutSwitchEffectCoordinator::NextLayoutSwitchEffectL - removing effect"));
        effectable->EffectSetEffect(NULL); // This calls AlfGfxEffectEndCallBack         
        }
    else
        {    
        // Load correct effect
        for ( TInt i = 0; i<iBridge.iAlfRegisteredEffects.Count(); i++ )
            {             
            if ( iBridge.iAlfRegisteredEffects[i].iAction == aContext)
                {
                //RDebug::Print(_L("CAlfLayoutSwitchEffectCoordinator::SetLayoutSwitchEffectL - loading effect"));
                TRAP_IGNORE(engine->LoadEffectL(*iBridge.iAlfRegisteredEffects[i].iEffectFile, effect, effectable, NULL, this, iLayoutSwitchEffectContext, KHuiFxDelayRunUntilFirstFrameHasBeenDrawn ) );                    
                break;
                }
            }
        }    
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::AlfGfxEffectEndCallBack
//
// This method is callback which gets called when layout 
// switch effect has ended.
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::AlfGfxEffectEndCallBack( TInt aHandle )
    {
    //RDebug::Print(_L("CAlfLayoutSwitchEffectCoordinator::AlfGfxEffectEndCallBack")); 
    // KErrNotFound indicates callback from DoNextLayoutSwitchContext
    if (iLayoutSwitchEffectContext == aHandle || aHandle == KErrNotFound)
        {
        AknTransEffect::TContext nextContext = NextLayoutSwitchContext();

        // Unfreeze visible content. This reveals real roster content (in new orientation).
        if (nextContext == AknTransEffect::ELayoutSwitchExit)
            {
            #ifdef HUI_DEBUG_TRACK_DRAWING
            RDebug::Print(_L("CAlfLayoutSwitchEffectCoordinator::AlfGfxEffectEndCallBack unfreezing roster content"));
            #endif
            iBridge.iHuiEnv->Display(0).SetDirty();
            FreezeRoster(EFalse);
            iBridge.SetVisualTreeVisibilityChanged(ETrue);
            if ( iLayoutSwitchNotCompleted )
                {
                iLayoutSwitchNotCompleted = EFalse;
                iBridge.LayoutSwitchComplete();
                }
            }
        
        // Set next effect
        SetLayoutSwitchEffect(nextContext);
        
        if (nextContext == AknTransEffect::ENone)
            {
            // Restore normal priority
            if ( iCurrentState == EStateThemeFx )
                {
                Transition(EStateIdle, iCurrentState);
                }
            
            RThread me = RThread();
            me.SetPriority(iOriginalPriority);    
            me.Close();

            // Just in case refresh everything
            iBridge.iHuiEnv->Display(0).SetDirty();
            }        
        }
    else
        {
        //RDebug::Print(_L("CAlfLayoutSwitchEffectCoordinator::AlfGfxEffectEndCallBack - got different handle (normal, dont worry...) - %i"), aHandle);        
        }
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::NextLayoutSwitchContext
//
// This method automatically selects the next context in the 
// layout switch procedure.
//
// Contextes change in the following order during layout switch:
//
// 1. AknTransEffect::ENone
// 2. AknTransEffect::ELayoutSwitchStart
// 3. AknTransEffect::ELayoutSwitchExit
// 4. AknTransEffect::ENone
//
// After new context is selected, appropriate effect is set 
// (and/or removed) from the roster.
//
// ---------------------------------------------------------
//
// todo: rename
TInt CAlfLayoutSwitchEffectCoordinator::DoNextLayoutSwitchContext(
        TAny* aLayoutSwitchCoordinator)
    {
    CAlfLayoutSwitchEffectCoordinator* coordinator = (CAlfLayoutSwitchEffectCoordinator*)aLayoutSwitchCoordinator;
    coordinator->AlfGfxEffectEndCallBack(KErrNotFound);
    return 0;
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::NextLayoutSwitchContext
// ---------------------------------------------------------
//
AknTransEffect::TContext CAlfLayoutSwitchEffectCoordinator::NextLayoutSwitchContext()
    {
    // Resolve next context based on current context
    AknTransEffect::TContext newContext = AknTransEffect::ENone;    
    switch (iLayoutSwitchEffectContext)
        {
        case AknTransEffect::ENone:
            {
            newContext = AknTransEffect::ELayoutSwitchStart;            
            break;
            }
        case AknTransEffect::ELayoutSwitchStart:
            {
            newContext = AknTransEffect::ELayoutSwitch;
            if (!iRosterFreezeEndTimer)
                {
                TRAP_IGNORE(iRosterFreezeEndTimer = CAlfRosterFreezeEndTimer::NewL(iBridge));
                }
            if (iRosterFreezeEndTimer)
                {
                iRosterFreezeEndTimer->Cancel();
                iRosterFreezeEndTimer->Start(KRosterFreezeEndTimeoutInMs*1000, TCallBack(DoNextLayoutSwitchContext, this));
                }
            else
                {
                newContext = AknTransEffect::ELayoutSwitchExit;
                }
            break;
            }
        case AknTransEffect::ELayoutSwitch:
            {
            newContext = AknTransEffect::ELayoutSwitchExit;                    
            break;
            }
        case AknTransEffect::ELayoutSwitchExit: // fallthrough
        default:
            {
            newContext = AknTransEffect::ENone;            
            break;
            }              
        }

    //RDebug::Print(_L("CAlfLayoutSwitchEffectCoordinator::NextLayoutSwitchEffectL old ctx = %i, new ctx = %i"), iLayoutSwitchEffectContext, newContext);
    return newContext;
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::IsThemeEffectEnabled()
// Returns true if layout switch theme effect may be used
// ---------------------------------------------------------
//
TBool CAlfLayoutSwitchEffectCoordinator::IsThemeEffectEnabled() const
    {
    TBool memoryOk = !( iBridge.iHuiEnv->MemoryLevel() <= EHuiMemoryLevelLow );
    TBool tfxOn = CAknTransitionUtils::TransitionsEnabled(AknTransEffect::ELayoutswitchTransitionsOff );
    TBool tfxExists = LayoutSwitchEffectsExist();

    return 
        memoryOk && tfxOn && tfxExists;
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::LayoutSwitchEffectsExist()
// Returns true if layout switch effect exists
// ---------------------------------------------------------
//
TBool CAlfLayoutSwitchEffectCoordinator::LayoutSwitchEffectsExist() const
    {
    TBool appearExists = EFalse;
    TBool disAppearExists = EFalse;
    
    for ( TInt i = 0; i<iBridge.iAlfRegisteredEffects.Count(); i++ )
        {             
        if ( iBridge.iAlfRegisteredEffects[i].iAction == AknTransEffect::ELayoutSwitchStart)
            {
            disAppearExists = ETrue;
            break;
            }
        else if ( iBridge.iAlfRegisteredEffects[i].iAction == AknTransEffect::ELayoutSwitchExit)
            {
            appearExists = ETrue;
            break;
            }
        }
    
    return (appearExists || disAppearExists);    
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::FreezeRoster()
// Freeze visible content in roster.
// ---------------------------------------------------------
//
void CAlfLayoutSwitchEffectCoordinator::FreezeRoster(TBool aFrozen)
    {
    TRAP_IGNORE(iBridge.iHuiEnv->Display(0).Roster().FreezeVisibleContentL(aFrozen));
    }

// ---------------------------------------------------------
// CAlfLayoutSwitchEffectCoordinator::DoFreezeFinished()
// ---------------------------------------------------------
//
TInt CAlfLayoutSwitchEffectCoordinator::DoFreezeFinished(TAny* aAny)
    {
    static_cast<CAlfLayoutSwitchEffectCoordinator*>(aAny)->FreezeFinished();
    return KErrNone;
    }

// ---------------------------------------------------------
// CAlfFinishTimer
// ---------------------------------------------------------
//
CAlfEffectEndTimer::CAlfEffectEndTimer( CAlfBridge& aBridge )
    :CTimer(EPriorityHigh), 
    iBridge(aBridge)
    {   
    }

void CAlfEffectEndTimer::ConstructL()
    {
    CTimer::ConstructL();
    iHandles.ReserveL(5);
    CActiveScheduler::Add( this );
    }

CAlfEffectEndTimer* CAlfEffectEndTimer::NewL( CAlfBridge& aBridge )
    {
    CAlfEffectEndTimer* self = new ( ELeave ) CAlfEffectEndTimer( aBridge );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfEffectEndTimer::~CAlfEffectEndTimer()
    {
    Cancel();
    iHandles.Close();
    }

void CAlfEffectEndTimer::Start( TTimeIntervalMicroSeconds32 aPeriod )
    {
    After( aPeriod );
    }

void CAlfEffectEndTimer::AddFinishedHandleL(TInt aHandle)
    {
    iHandles.Append(aHandle);
    }

void CAlfEffectEndTimer::RunL()
    {
    //
    // timer completes and control is returned to caller
    //
    while(iHandles.Count())
        {
        iBridge.TransitionFinishedHandlerL( iHandles[0]);
        iHandles.Remove(0);
        }
    // We don't become active unless we are explicitly restarted
    }

void CAlfEffectEndTimer::DoCancel()
    {
    CTimer::DoCancel();
    }


CEffectState::CEffectState()
    {
    // CBase clears all variables
    }

CEffectState::~CEffectState()
    {
    delete iEffectName;
    }

void CEffectState::ResolveFileNameL(RMemReadStream& aStream)
    {
    HBufC* effectDirectory = HBufC::NewLC(aStream, 256);
    HBufC* effectFile = HBufC::NewLC(aStream, 256);

    // Add one extra because we want to be able to append a number to the filename
    HBufC* effectFullName = HBufC::NewL(effectDirectory->Des().Length()
            + effectFile->Des().Length() + 1);
    CleanupStack::PushL(effectFullName);

    effectFullName->Des().Copy(*(effectDirectory));
    effectFullName->Des().Append(*(effectFile));
    delete iEffectName;
    iEffectName = effectFullName; // ownership transferred
    CleanupStack::Pop(effectFullName);
    CleanupStack::PopAndDestroy(2, effectDirectory);
    }


CFullScreenEffectState::~CFullScreenEffectState()
    {
    }

void CFullScreenEffectState::ConstructL(const CFullScreenEffectState& aEffectState)
    {
    iAction = aEffectState.iAction;
    iHandle = aEffectState.iHandle;
    iType = aEffectState.iType;
    iLongAppStartTimeout = aEffectState.iLongAppStartTimeout;
    iTimeout = aEffectState.iTimeout;
    iToWg = aEffectState.iToWg;
    iFromWg = aEffectState.iFromWg;
    iToAppId = aEffectState.iToAppId;
    iFromAppId = aEffectState.iFromAppId;

    iToSecureId = aEffectState.iToSecureId;
    iFromSecureId = aEffectState.iFromSecureId;
    
    iRect.iTl.iX = aEffectState.iRect.iTl.iX;
    iRect.iTl.iY = aEffectState.iRect.iTl.iY;
    iRect.iBr.iX = aEffectState.iRect.iBr.iX;
    iRect.iBr.iY = aEffectState.iRect.iBr.iY;
    iState = aEffectState.iState;
    iEffectName = aEffectState.iEffectName->AllocL();
    iEffectType = aEffectState.iEffectType;
    iCompletionHandle = aEffectState.iCompletionHandle;
    iTwoPhaseEffect = CFullScreenEffectState::EOnlyOnePart;
    }

void CFullScreenEffectState::ConstructL(
        TInt aAction,
        RMemReadStream& aStream)
    {
    iAction = aAction;
    iHandle = aStream.ReadInt32L();
    iType = aStream.ReadInt32L();
    iLongAppStartTimeout = aStream.ReadInt32L();
    iTimeout = aStream.ReadInt32L();
    iToWg = aStream.ReadInt32L();
    iFromWg = aStream.ReadInt32L();
    iToAppId = aStream.ReadInt32L();
    iFromAppId = aStream.ReadInt32L();

    iToSecureId = aStream.ReadInt32L();
    iFromSecureId = aStream.ReadInt32L();
    
    // TInt flags
    aStream.ReadInt32L();
    iRect.iTl.iX = aStream.ReadInt32L();
    iRect.iTl.iY = aStream.ReadInt32L();
    iRect.iBr.iX = aStream.ReadInt32L();
    iRect.iBr.iY = aStream.ReadInt32L();

    ResolveFileNameL(aStream);

    iCompletionHandle = iHandle;
    iTwoPhaseEffect = CFullScreenEffectState::EOnlyOnePart;
    }

void CFullScreenEffectState::SetState(TEffectState aState)
    {
    if (aState >= iState)
        {
        __ALFFXLOGSTRING2("CFullScreenEffectState::SetState - Set %d -> %d", iState, aState);
        iState = aState;
        }
    else
        {
        __ALFFXLOGSTRING2("CFullScreenEffectState::SetState - WARNING! Trying to set to previous state. %d -> %d", iState, aState);
        }
    };
        
TEffectState CFullScreenEffectState::State()
    {
    __ALFFXLOGSTRING1("CFullScreenEffectState::State - %d", iState);
    return iState;
    }
      

void CControlEffectState::ConstructL(TInt aAction,
        RMemReadStream& aStream)
    {
    iAction = aAction;
    TInt operation = aStream.ReadInt32L();
    iHandle = aStream.ReadInt32L();
    iClientHandle = aStream.ReadInt32L();
    iClientGroupHandle = aStream.ReadInt32L();
    TInt screenNumber = aStream.ReadInt32L(); // this has always value 0
    iSetDistractionWindow = ENoOperation;
    // Are Symbian full filename+directory combinations still max 256 characters long?
    ResolveFileNameL(aStream);
    }

void CControlEffectState::ConstructL(TUint32 aClientHandle, TUint32 aClientGroupHandle, TInt aSetDistractionWindow)
    {
    iClientHandle = aClientHandle;
    iClientGroupHandle = aClientGroupHandle;
    if (aSetDistractionWindow)
        {
        iSetDistractionWindow = CControlEffectState::ESetDistractionWindow;
        }
    else
        {
        iSetDistractionWindow = CControlEffectState::ERemoveDistractionWindow;
        }
    }
