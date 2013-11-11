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
// @internalTechnology
// 
//

#ifndef __CUPNPTIMER_H_
#define __CUPNPTIMER_H_

#include <e32base.h>

NONSHARABLE_CLASS ( MUPnPTimerObserver )
	{
	public:
		virtual void TimeOut () =0;
	};


NONSHARABLE_CLASS ( CUPnPTimer ) : public CActive
{
public:
	// Construct/destruct
	static CUPnPTimer* NewL(MUPnPTimerObserver& aObserver);
	~CUPnPTimer(); 
	
	void StartTimer(TTimeIntervalMicroSeconds32 aTimeInterval);  
	void StopTimer();    
    
private:
	// Construct/destruct
	CUPnPTimer(MUPnPTimerObserver& aObserver);
	
	// From CActive
	void RunL();
	void DoCancel();   
	
private:
	RTimer iTimer;      // Has
	MUPnPTimerObserver& iObserver;
};


#endif /*__CUPNPTIMER_H_*/

