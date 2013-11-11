/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  UpdateQueue controls the feeds to be updated.
*
*/


#ifndef _UPDATEQUEUE_H_
#define _UPDATEQUEUE_H_

//  INCLUDES
#include <e32property.h>  
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFeedsServer;
class CUpdateQueue;
class CUpdateAllFeedsTask;

// CLASS DECLARATION

/**
*  The feed Update Queue.
*
*  \b Library: FeedsEngine.lib
*
*  @since 7.1
*/
class CUpdateQueue : public CBase
    {
public:

    /**
     *
     * @since 7.1
     * Two-phased constructor.
     */    
    static CUpdateQueue* NewL(CUpdateAllFeedsTask* aUpdateAllFeedTask, TInt aFreq);

    /**
     *
     * @since 7.1
     * Destructor
     */
    ~CUpdateQueue();

public:
	
	/**
     * This method adds the feed into RArray 
     *     
     * @since 7.1
     * @return Integer.
     */
	TInt AddFeed(TInt aFeedId);

	/**
     * This method delets the feed from RArray 
     *     
     * @since 7.1
     * @return Integer.
     */
	TInt RemoveFeed(TInt aFeedId);

	/**
     * This method decrements the timer counter
     *     
     * @since 7.1
     * @return Integer.
     */
	TInt UpdateL(TInt aMinutes);

	/**
     * This method return the frequency 
     *     
     * @since 7.1
     * @return Integer.
     */
	TInt GetFreq();

    /**
     * This method return the Count 
     *     
     * @since 7.1
     * @return Integer.
     */
    TInt Count();

    /**
     * This method resets all the auto update timers 
     *     
     * @since 7.1
     * @return void.
     */
    void ResetTimers();

private:
    
    /**
     *
     * @since 7.1
     * C++ default constructor.
     */ 
    CUpdateQueue(CUpdateAllFeedsTask* aUpdateAllFeedTask, TInt aFreq);

    /**
     *
     * @since 7.1
     * By default Symbian 2nd phase constructor is private.
     */ 
	void ConstructL();

private:
	RArray<TInt>         iFeedIds;
	TInt                 iFreq;
	TInt                 iCurrentState;
	CUpdateAllFeedsTask* iUpdateAllFeedsTask;
	TTime                iLastAutoUpdate;

	friend class CFeedsSever;
};
#endif //_UPDATEQUEUE_H_