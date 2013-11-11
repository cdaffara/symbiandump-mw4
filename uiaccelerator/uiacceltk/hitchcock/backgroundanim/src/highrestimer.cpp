/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include "highrestimer.h"


CHighResTimer* CHighResTimer::NewL(TCallBack aCallBack, TInt aPriority)
	{
	CHighResTimer* self = new(ELeave) CHighResTimer(aCallBack, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHighResTimer::~CHighResTimer()
	{
	Cancel();
	}

void CHighResTimer::CallBack(const TInt aWaitInMilliSecs)
	{
	TInt wait = aWaitInMilliSecs*1000;
	if (wait < 1000)
	    {
	    wait = 1000;
	    }
	if (wait > 5000000)
	    {
	    wait = 5000000;
	    }
            
    HighRes( TTimeIntervalMicroSeconds32( wait ) );
	}
	
CHighResTimer::CHighResTimer(TCallBack aCallBack, TInt aPriority) 
	: CTimer(aPriority), iCallBack(aCallBack)
		{
		CActiveScheduler::Add(this);
		}

void CHighResTimer::RunL()
	{
	if (iStatus.Int() != KErrCancel)
	    {
	    iCallBack.CallBack();
	    }
	}
	
