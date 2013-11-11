/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   SIT thread death observer
*
*/



// @file sitdeathobserver.h

#ifndef __SIT_DEATH_OBSERVER_H__
#define __SIT_DEATH_OBSERVER_H__

#include <e32base.h>

class MSitDeathListener
    {
public:
    virtual void SitDied() = 0;
    };
   
/**
 * SIT thread death observer
 */
NONSHARABLE_CLASS(CSitDeathObserver) : public CActive
    {
public:
    CSitDeathObserver(TThreadId aThreadId, MSitDeathListener* aSitDeathListener);
    ~CSitDeathObserver();

    void StartObservingL();

private: // From CActive
    void DoCancel();
    void RunL();

private:
    RThread iThreadToObserve;
    TThreadId iThreadId;
    MSitDeathListener* iSitDeathListener;
    };

#endif // __SIT_DEATH_OBSERVER_H__
