/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   AlfSynchronizer waits for synchronization to complete
*
*/



#ifndef __ALFSYNCHRONIZER_H__
#define __ALFSYNCHRONIZER_H__

#include <e32base.h>
#include <e32property.h>

/**
 * Synchronization active object.
 */
NONSHARABLE_CLASS( CAlfSynchronizer ) : public CActive
    {
public:
    
    /**
     * Two phase constructor.
     */
    static CAlfSynchronizer* NewL();
    
    /**
     * C++ destructor
     */
    ~CAlfSynchronizer();

public:
    
    /**
     * Starts to wait for synchronization to complete
     * with particular identifier.
     * This will start nested active scheduler.
     */
    void Start(TInt aId);  
    
private:    

    /**
     * C++ constructor
     */
    CAlfSynchronizer();

    /**
     * ConstructL
     */
    void ConstructL();

    void RunL();
    void DoCancel();

    static TInt CallbackSyncTimeout( TAny* aPtr );
    void DoCallbackSyncTimeout();

private: // data

    /**
     * Active scheduler waiter.
     * Own.
     */
    CActiveSchedulerWait* iWait;
    
    /**
     * P&S property for monitoring a variable.
     * Own.
     */
    RProperty iProperty;
    
    /**
     * Timer to ensure that this synchronizer has finite duration.
     * Own.
     */
    CPeriodic* iTimeout;
    
    /**
     * Id waiting to be completed.
     */
    TInt iSynchId;
    
    /**
     * ETrue if @c iWait has been stop.
     * EFalse otherwise.
     */
    TBool iAsyncStopDone;
    };

#endif //__ALFSYNCHRONIZER_H__
