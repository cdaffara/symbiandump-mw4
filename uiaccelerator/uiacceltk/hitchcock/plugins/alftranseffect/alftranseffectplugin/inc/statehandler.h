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


#ifndef _STATEHANDLER_H_
#define _STATEHANDLER_H_

#include "alfdrawerengine.h"

class CStateHandler;
class CUidBlock;
class CUidException;

/**
 * This class should be overriden to implement new states
 */
NONSHARABLE_CLASS(CStateBase) : public CBase
	{
public:
	CStateBase(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
	virtual ~CStateBase();
public:
	enum TSignal 
		{
		EBeginFullscreen,
		EEndFullscreen,
		EFinishFullscreen,
		EBeginComponent,
		EFinishComponent,
		EAbortFullscreen,
		EAbortComponent,
		EAbort,
		};
	enum TState
		{
		EResting,
		EComponent,
		EFullscreenPhase1,
		EFullscreenPhase2
		};
	struct TStateInfo
		{
		TState iState;	
		TUint iAction;
		};

	/**
	 * Handles the signal.
	 * You must override this one.
	 *  - return new state to transition
	 *  - return NULL to stay in the same state.
	 */
	virtual CStateBase* SignalL(TSignal aSignal) = 0;
	
	/**
	 * Checks the block list if the uid pair should have a transition or not.
	 */
	virtual TBool IsBlocked(const TUid& aFromUid, const TUid& aToUid);
	
	inline TStateInfo& StateInfo();
protected:	
	TStateInfo iStateInfo;
	MAlfDrawerEngine& iEngine;
	CStateHandler& iHandler;
	};
	
////
CStateBase::TStateInfo& CStateBase::StateInfo()
	{
	return iStateInfo;
	}
	

/**
 * This is the state machine interface
 */
NONSHARABLE_CLASS(CStateHandler) : public CBase
	{
public:
	enum TFullscreenType 
		{
		ENone,
		EError,
		EActivation,
		EStart,
		EExit,
		EAppSpecific,
		ELayoutSwitch,
		};
	
	void Signal(CStateBase::TSignal aSignal);

	TBool IsBlocked(const TUid& aFromUid, const TUid& aToUid);
	///Some block list update functions here
	
	void AddBlockUid(const TUid& aUid);
	void RemoveBlockUid(const TUid& aUid);

	void AddBlockUidPair(const TUid& aFromUid, const TUid& aToUid);
	
	void AddExceptionUid(const TUid& aUid);
	void AddExceptionUidPair(const TUid& aFromUid, const TUid& aToUid);

	TFullscreenType GetFullscreenType(TUint aAction);
	TFullscreenType GetCurrentFullscreenType();
	CStateBase::TState GetState();
public:
	static CStateHandler* NewL(MAlfDrawerEngine* aEngine);	
	CStateHandler(MAlfDrawerEngine* aEngine);
	virtual ~CStateHandler();
	void ConstructL();
private:
	CStateHandler();

private: // data	
	CStateBase* iState;
	MAlfDrawerEngine& iEngine;
	RPointerArray<CUidBlock> iBlockList;
	RPointerArray<CUidException> iExceptionList;
	
	};
	

	
#endif
