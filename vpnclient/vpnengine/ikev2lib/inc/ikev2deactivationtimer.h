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
* Description:  Timer to trigger cancellation of session deletion.
*
*/

#ifndef C_IKEV2DEACTIVATIONTIMER_H
#define C_IKEV2DEACTIVATIONTIMER_H

#include <e32base.h>

/**
 *  Deactivation timer callback interface.
 *  Callback interface which is used by CIkev2DeactivationTimer object to
 *  notify about deactivation timeout.
 * 
 */
NONSHARABLE_CLASS(MIkev2DeactivationTimerCallback)
    {
public:        
    /**
     * Notifies about deactivation timeout.
     */
    virtual void DeactivationTimeout() = 0;
    };


/**
 *  Deactivation timer.
 * 
 */
NONSHARABLE_CLASS(CIkev2DeactivationTimer) : public CTimer
{
public:
    static CIkev2DeactivationTimer* NewL(MIkev2DeactivationTimerCallback& aCallback);
	~CIkev2DeactivationTimer();
    
	/**
	 * Issues a request to the timer.
	 */
	void IssueRequest();
	
protected:
    void RunL();
    
private:
    CIkev2DeactivationTimer(MIkev2DeactivationTimerCallback& aCallback);
    
    MIkev2DeactivationTimerCallback& iCallback;

};

#endif // C_IKEV2DEACTIVATIONTIMER_H
