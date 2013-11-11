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
* Description:   State machine interface. Used for the statemachine in the 
*                server drawer to keep track on when a transition should be made
*                and when not.
*
*/


#include "statehandler.h"
#include "states.h"
#include "uidblock.h"
#include "uidexception.h"
#include <alflogger.h>

// ---------------------------------------------------------------------------
// This LUT defines what fullscreen transitions is of wich type.
// It is needed by the filter, but is part of platform requirements,
// so it feels logical to keep it here.
// ---------------------------------------------------------------------------
//
#include  <akntranseffect.h> 
const CStateHandler::TFullscreenType FullscreenTypeLUT[] = 
	{
	CStateHandler::ENone,  						//none
	CStateHandler::EActivation, //activation
	CStateHandler::EError,   	//removed action
	CStateHandler::EStart, 		//start
	CStateHandler::EStart, 		//start embedded
	CStateHandler::EExit, 		//exit
	CStateHandler::EStart, 		//start rect
	CStateHandler::EStart, 		//start switch
	CStateHandler::EStart, 		//start switch rect
	CStateHandler::EExit, 		//exit for end key
	CStateHandler::EExit,		//exit embedded
	CStateHandler::EError,      //right?
	CStateHandler::ELayoutSwitch, //ELayoutSwitch
	CStateHandler::ELayoutSwitch, //ELayoutSwitch
	CStateHandler::ELayoutSwitch, //ELayoutSwitch
	};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CStateBase::CStateBase(MAlfDrawerEngine& aEngine, CStateHandler& aHandler) :
	iEngine(aEngine),
	iHandler(aHandler)
	{
	}

CStateBase::~CStateBase()
	{
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CStateBase::IsBlocked(const TUid& aFromUid, const TUid& aToUid)
	{
	return iHandler.IsBlocked(aFromUid, aToUid);
	}
	


// ---------------------------------------------------------------------------
// StateHandler Implementation:
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CStateHandler::Signal(CStateBase::TSignal aSignal)
	{
	CStateBase* newstate = NULL;
	TRAPD(err, newstate = iState->SignalL(aSignal));
	if((KErrNone == err) && (NULL != newstate))
		{
		delete iState;
		iState = newstate;
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CStateHandler::IsBlocked(const TUid& aFromUid, const TUid& aToUid)
	{
	__ALFFXLOGSTRING2("CStateHandler::IsBlocked - aFromUid: 0x%x, aToUid: 0x%x", aFromUid, aToUid );
	TInt flags = iEngine.Flags();
	if(flags & AknTransEffect::TParameter::EActivateExplicitContinue)
		{
//		return EFalse;
		}
		
    for(TInt i = iExceptionList.Count() - 1; i >= 0; i--) 
    	{
    	if(iExceptionList[i]->IsAllowed(aFromUid,aToUid)) 
    		{
    		__ALFFXLOGSTRING("CStateHandler::IsBlocked - return EFalse <<");
    	    return EFalse;
    		}
    	}
		
	for(TInt i = iBlockList.Count() - 1; i >= 0; i--) 
		{
		if(iBlockList[i]->IsBlocked(aFromUid,aToUid)) 
			{
			__ALFFXLOGSTRING("CStateHandler::IsBlocked - return ETrue <<");
			return ETrue;
			}
		}
	__ALFFXLOGSTRING("CStateHandler::IsBlocked - return EFalse <<");
	return EFalse;
	}

// ---------------------------------------------------------------------------
// OOM condition means block will not be added, this is ok.
// ---------------------------------------------------------------------------
//
void CStateHandler::AddBlockUid(const TUid& aUid)
	{
	CFullUidBlock* uidblock = new CFullUidBlock(aUid);
	if(uidblock)
		{
		if(KErrNone != iBlockList.Append(uidblock)) 
			{
			//Failed to add, forget about it.
			delete uidblock;
			}
		}
	}

// ---------------------------------------------------------------------------
// OOM condition means block will not be added, this is ok.
// ---------------------------------------------------------------------------
//
void CStateHandler::AddBlockUidPair(const TUid& aFromUid, const TUid& aToUid)
	{
	CPairedUidBlock* uidblock = new CPairedUidBlock(aFromUid, aToUid);
	if(uidblock)
		{
		if(KErrNone != iBlockList.Append(uidblock))
			{
			//Failed to add, forget about it.
			delete uidblock;
			}
		}
	}
	
// ---------------------------------------------------------------------------
// OOM condition means exception will not be added, this is ok.
// ---------------------------------------------------------------------------
//
void CStateHandler::AddExceptionUid(const TUid& aUid)
	{
	CFullUidException* uidexception = new CFullUidException(aUid);
	if(uidexception)
		{
		if(KErrNone != iExceptionList.Append(uidexception)) 
			{
			//Failed to add, forget about it.
			delete uidexception;
			}
		}
	}	
	
// ---------------------------------------------------------------------------
// OOM condition means exception will not be added, this is ok.
// ---------------------------------------------------------------------------
//	
void CStateHandler::AddExceptionUidPair(const TUid& aFromUid, const TUid& aToUid)
    {
	CPairedUidException* uidexception = new CPairedUidException(aFromUid, aToUid);
	if(uidexception)
		{
		if(KErrNone != iExceptionList.Append(uidexception))
			{
			//Failed to add, forget about it.
			delete uidexception;
			}
		}    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CStateHandler::RemoveBlockUid(const TUid& aUid)
	{
	for(TInt i = iBlockList.Count() - 1; i >= 0; i--) 
		{
		if(iBlockList[i]->IsBlocked(aUid,aUid)) 
			{
			delete iBlockList[i];
			iBlockList.Remove(i);
			}
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CStateHandler::TFullscreenType CStateHandler::GetFullscreenType(TUint aAction)
	{
	TFullscreenType type = EError;
	if(aAction < (sizeof(FullscreenTypeLUT) / sizeof(TInt)))
		{
		type = FullscreenTypeLUT[aAction];
		}
	if(aAction >= AknTransEffect::EAppSpesificEvent)
		type = EAppSpecific;
	return type;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CStateHandler::TFullscreenType CStateHandler::GetCurrentFullscreenType()
	{
	return GetFullscreenType(iState->StateInfo().iAction);
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CStateHandler* CStateHandler::NewL(MAlfDrawerEngine* aEngine)
	{
	CStateHandler* self = new (ELeave) CStateHandler(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CStateBase::TState CStateHandler::GetState()
	{
	return iState->StateInfo().iState;
	}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CStateHandler::CStateHandler(MAlfDrawerEngine* aEngine) :
	iEngine(*aEngine)
	{
	}

void CStateHandler::ConstructL()
	{
	iState = CRestingState::NewL(iEngine,*this);
	}

CStateHandler::~CStateHandler()
	{
	iBlockList.ResetAndDestroy();
	iExceptionList.ResetAndDestroy();
	delete iState;
	}
	
	


