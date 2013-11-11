// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:
// @internalComponent
// 
//

#include "cupnptimer.h"

CUPnPTimer::CUPnPTimer(MUPnPTimerObserver& aObserver) 
: CActive ( EPriorityStandard ),
	iObserver ( aObserver )
	{
	CActiveScheduler::Add(this);
	iTimer.CreateLocal();
	}

CUPnPTimer* CUPnPTimer::NewL(MUPnPTimerObserver& aObserver)
	{
	return new (ELeave) CUPnPTimer(aObserver);
	}

CUPnPTimer::~CUPnPTimer()
	{
	Cancel();
	iTimer.Close();	
	}

void CUPnPTimer::RunL()
	{
	if ( iStatus.Int () == KErrNone )
		iObserver.TimeOut();
	}

void CUPnPTimer::DoCancel()
	{
	iTimer.Cancel();
	}

void CUPnPTimer::StartTimer(TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	if(!IsActive())
		{
		iTimer.After(iStatus, aTimeInterval);	
		SetActive();
		}
	}

void CUPnPTimer::StopTimer()
	{
	DoCancel ();
	}







