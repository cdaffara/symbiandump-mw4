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
* Description:  Timer class which implements a timeout for an application close action
*
*/


#ifndef GOOMAPPCLOSETIMER_H_
#define GOOMAPPCLOSETIMER_H_

#include <e32base.h>

class CGOomCloseApp;

/**
 *  A simple timer class which implements a timeout for an application close action.
 *
 *  If the application has not been closed within the period this timer is started with, 
 *  then we complete the action as if he had been successful, allowing us to move on to the 
 *  action or set of actions.
 *
 *  @lib oommonitor.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomAppCloseTimer) : public CTimer
    {
public:
    static CGOomAppCloseTimer* NewL(CGOomCloseApp& aMonitor);
    
    enum TGOomAppCloseState
        {
        EGOomAppClosing = 0,
        EGOomAppKilled
        };
    
    void SetState(TGOomAppCloseState);
    
private:
    CGOomAppCloseTimer(CGOomCloseApp& aMonitor);
    void RunL();
    
private: //data
	
    /**
     * A reference to the owning close app action
     */
    CGOomCloseApp& iMonitor;
    
    TBool iState;
    };

#endif /*GOOMAPPCLOSETIMER_H_*/
