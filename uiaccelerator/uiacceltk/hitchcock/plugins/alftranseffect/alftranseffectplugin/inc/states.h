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


#ifndef _STATES_H_
#define _STATES_H_

#include "statehandler.h"

/**
 * Resting state
 */
NONSHARABLE_CLASS(CRestingState) : public CStateBase
	{
	CRestingState(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
public:	
	static CStateBase* NewL(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
	CStateBase* SignalL(TSignal aSignal);
	};

/**
 * Control Transition ongoing
 */
NONSHARABLE_CLASS(CControlState) : public CStateBase
	{
	CControlState(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
public:	
	static CStateBase* NewL(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
	CStateBase* SignalL(TSignal aSignal);
	};

/**
 * First phase of fullscreen transition 
 * (BeginFullscreen received, but not yet EndFullscreen)
 */
NONSHARABLE_CLASS(CPhase1State) : public CStateBase
	{
	CPhase1State(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
public:	
	static CStateBase* NewL(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
	CStateBase* SignalL(TSignal aSignal);
	};

/**
 * Second phase of fullscreen transition
 * (EndFullscreen has been received.)
 */
NONSHARABLE_CLASS(CPhase2State) : public CStateBase
	{
	CPhase2State(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
public:	
	static CStateBase* NewL(MAlfDrawerEngine& aEngine, CStateHandler& aHandler);
	CStateBase* SignalL(TSignal aSignal);
	};



#endif