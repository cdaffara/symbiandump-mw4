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
* Description:   State machine state implementations
*
*/



#include  <akntranseffect.h> // for Transition effect enumerations
#include <alflogger.h>
#include "states.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CRestingState construction
// ---------------------------------------------------------------------------
//
CRestingState::CRestingState(MAlfDrawerEngine& aEngine, CStateHandler& aHandler) : 
	CStateBase(aEngine,aHandler)
		{
		}

CStateBase* CRestingState::NewL(MAlfDrawerEngine& aEngine, CStateHandler& aHandler)
	{
	CRestingState* self = new (ELeave) CRestingState(aEngine,aHandler);
	self->iStateInfo.iState = EResting;
	return self;	
	}
	
// ---------------------------------------------------------------------------
// CRestingState Signal handling
// ---------------------------------------------------------------------------
//
CStateBase* CRestingState::SignalL(TSignal aSignal)
	{
	__ALFFXLOGSTRING1("CRestingState::SignalL %d (Alf)>>", aSignal);
	TInt error = KErrNone;
	switch( aSignal ) 
		{
		case EBeginFullscreen:
            if( !IsBlocked( iEngine.FromUid(), iEngine.ToUid() ) ) 
		        {
    			iEngine.SendBeginFullscreen();
			    return CPhase1State::NewL( iEngine,iHandler );
			    }
		    return NULL;

		case EBeginComponent:
		    error = iEngine.SendBeginControlTransition();
		    if ( error == KErrNone )
		        {
         		return CControlState::NewL( iEngine, iHandler );
		        }
		    return NULL;    

		case EEndFullscreen:
//		    iEngine.SendEndFullscreen(); // TODO: revise?
//		    return CRestingState::NewL( iEngine, iHandler );

		case EAbortFullscreen:
			iEngine.SendAbortFullscreen();
			return CRestingState::NewL( iEngine, iHandler );
		}
	return NULL;
	}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CControlState construction
// ---------------------------------------------------------------------------
//
CControlState::CControlState(MAlfDrawerEngine& aEngine, CStateHandler& aHandler) : 
	CStateBase(aEngine,aHandler)
		{
		}

CStateBase* CControlState::NewL(MAlfDrawerEngine& aEngine, CStateHandler& aHandler)
	{
	CControlState* self = new (ELeave) CControlState(aEngine,aHandler);
	self->iStateInfo.iState = EComponent;
	return self;	
	}
	
// ---------------------------------------------------------------------------
// CControlState signal handling
// ---------------------------------------------------------------------------
//
CStateBase* CControlState::SignalL(TSignal aSignal)
	{
    __ALFFXLOGSTRING1("CControlState::SignalL %d (Alf)>>", aSignal);

    TInt error = KErrNone;	
	switch( aSignal )
		{
		case EAbort:
		case EAbortComponent:
			iEngine.SendAbortControlTransition();
			return CRestingState::NewL( iEngine, iHandler );
		case EFinishComponent:
		    // do something to finish
		    iEngine.SendFinishControlTransition();
			return CRestingState::NewL( iEngine, iHandler );
		case EBeginFullscreen:
			if( !IsBlocked( iEngine.FromUid(), iEngine.ToUid() ) )
				{
				iEngine.SendBeginFullscreen();
				return CPhase1State::NewL( iEngine,iHandler );
				}
			return NULL;
		case EBeginComponent:
		    // we don't limit control transitions
		    // we don't have an ending signal for the controls now
		    // We may need something to be able to call
		    // SendFinishControlTransition
		    error = iEngine.SendBeginControlTransition();
		    if ( error == KErrNone )
		        {
         		return CControlState::NewL( iEngine, iHandler );
		        }
		    return NULL;    
		}
	return NULL;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CPhase1State construction
// ---------------------------------------------------------------------------
//
CPhase1State::CPhase1State(MAlfDrawerEngine& aEngine, CStateHandler& aHandler) : 
	CStateBase(aEngine,aHandler)
		{
		}

CStateBase* CPhase1State::NewL(MAlfDrawerEngine& aEngine, CStateHandler& aHandler)
	{
	CPhase1State* self = new (ELeave) CPhase1State(aEngine,aHandler);
	self->iStateInfo.iState = EFullscreenPhase1;
	self->iStateInfo.iAction = aEngine.Action();
	return self;	
	}

// ---------------------------------------------------------------------------
// CPhase1State Signal handling
// ---------------------------------------------------------------------------
//
CStateBase* CPhase1State::SignalL(TSignal aSignal)
	{
    TInt error = KErrNone;	
    __ALFFXLOGSTRING1("CPhase1State::SignalL(TSignal %d) (Alf)", aSignal);
	switch(aSignal)
		{
		case EAbort:
		case EAbortFullscreen:
   			iEngine.SendAbortFullscreen();
			return CRestingState::NewL(iEngine,iHandler);
        case EFinishFullscreen:
		    if ( !iEngine.EffectFinishedFullScreen() )
		        {
    			iEngine.SendAbortFullscreen();
		        }
            return CRestingState::NewL(iEngine,iHandler);
		case EEndFullscreen:
			TInt triggeredLongStartEffect = iEngine.SendEndFullscreen();
			__ALFFXLOGSTRING1("CPhase1State::SignalL - Was long app start triggered? Stay in Phase1", triggeredLongStartEffect);
			if (triggeredLongStartEffect)
			    {
			    return CPhase1State::NewL(iEngine,iHandler);
			    }
			else
			    {
			    return CPhase2State::NewL(iEngine,iHandler);
			    }
		case EBeginFullscreen:
			if(!IsBlocked(iEngine.FromUid(),iEngine.ToUid())) 
				{
    		    iEngine.SendBeginFullscreen();
				}
			return NULL;
		case EBeginComponent:
		    error = iEngine.SendBeginControlTransition();
		    if ( error == KErrNone )
		        {
         		return CControlState::NewL( iEngine, iHandler );
		        }
		    return NULL;    
		}
	return NULL;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CPhase2State construction
// ---------------------------------------------------------------------------
//
CPhase2State::CPhase2State(MAlfDrawerEngine& aEngine, CStateHandler& aHandler) : 
	CStateBase(aEngine,aHandler)
		{
		}

CStateBase* CPhase2State::NewL(MAlfDrawerEngine& aEngine, CStateHandler& aHandler)
	{
	CPhase2State* self = new (ELeave) CPhase2State(aEngine,aHandler);
	self->iStateInfo.iState = EFullscreenPhase2;
	self->iStateInfo.iAction = aEngine.Action();
	return self;	
	}

// ---------------------------------------------------------------------------
// CPhase2State Signal handling
// ---------------------------------------------------------------------------
//
CStateBase* CPhase2State::SignalL(TSignal aSignal)
	{
    TInt error = KErrNone;
    __ALFFXLOGSTRING1("CPhase2State::SignalL %d (Alf)>>", aSignal);
	switch(aSignal)
		{
		case EAbort:
		case EAbortFullscreen:
			iEngine.SendAbortFullscreen();
			return CRestingState::NewL(iEngine,iHandler);
		case EFinishFullscreen:
		    if ( ( iEngine.Action() == 1001 || iEngine.Action() == AknTransEffect::EApplicationExit ) &&
		        !iEngine.EffectFinishedFullScreen() )
		        {
		        iEngine.SendAbortFullscreen();
		        }
			return CRestingState::NewL(iEngine,iHandler);
		case EBeginFullscreen:
			if(!IsBlocked(iEngine.FromUid(),iEngine.ToUid()))
				{
				iEngine.SendBeginFullscreen();
				return CPhase1State::NewL(iEngine,iHandler);
				}
			return NULL;
	     case EEndFullscreen:
	            iEngine.SendEndFullscreen();
	        return NULL;
		case EBeginComponent:
		    error = iEngine.SendBeginControlTransition();
		    if ( error == KErrNone )
		        {
         		return CControlState::NewL( iEngine, iHandler );
		        }
		    return NULL;    
		}
	return NULL;
	}
