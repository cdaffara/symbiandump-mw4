/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Header file of the class CHttpTransferTimer.
*
*/


#ifndef C_HTTPTRANSFERTIMER_H
#define C_HTTPTRANSFERTIMER_H

// system includes
#include <e32base.h>
#include <e32cons.h>

// forward declarations
class MHttpTransferTimerObserver;

/**
 * This class is a timer that launches new transfers in CHttpUploader
 * and CHttpDownloader
 *
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 */
class CHttpTransferTimer : public CBase,
                           public MBeating
    {
public:

    /**
     * Static NewL method to construct the timer.
     *
     * @since S60 3.2
     * @param aInterval Timer interval in seconds 
     * @param aCallback The call back reference 
     * @return CHttpTransferTimer* timer instance
     */
    static CHttpTransferTimer* NewL( 
                                TInt aInterval,
                                MHttpTransferTimerObserver* aCallback );

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CHttpTransferTimer();

private:

    /**
     * Second phase constructor
     *
     * @since S60 v3.2
     */
    void ConstructL();

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aInterval Interval when call back is called
     * @param aCallback Callback interface
     */
    CHttpTransferTimer( TInt aInterval,
                        MHttpTransferTimerObserver* aCallback );

private: 

// From base class MBeating

    /**
     * Called when the beat is in sync.
     *
     * @since S60 3.0
     */
    void Beat();

    /**
     * Called when the beat needs to be syncronized.
     *
     * @since S60 3.0
     */
    void Synchronize() {}

private: 
    
    /**
     * Heartbeat active object
     * Own.
     */
    CHeartbeat* iHeartbeat;

    /**
     * Call back pointer.
     * Not own.
     */
    MHttpTransferTimerObserver *iCallback;

    /**
     * Interval in seconds
     */
    TInt iInterval;
    
    /**
     * Counter
     */
    TInt iCounter;
     
    };

#endif // C_HTTPTRANSFERTIMER_H

// End of file
