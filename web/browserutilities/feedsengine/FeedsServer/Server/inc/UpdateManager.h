/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UpdateManager it controls the auto updation of feed.
*
*/


#ifndef _UPDATEMANAGER_H_
#define _UPDATEMANAGER_H_

//  INCLUDES
#include <e32property.h>  
#include <e32base.h>
#include "UpdateAllFeedsTask.h"
#include "etel3rdparty.h"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFeedsServer;
class CUpdateQueue;
class CUpdateAllFeedsTask;
class CFeedsDatabase;
class CServerHttpConnection;
class CUpdateTimer;
class CRoamingInfo;
// CLASS DECLARATION

/**
*  The feed UpdateManager.
*
*  \b Library: FeedsEngine.lib
*
*  @since 7.1
*/
class CUpdateManager: public CTimer, public MUpdateAllFeedsTaskObserver
    {

public:  // From CActive
    /**
      * Handles an active object's request completion event.
      *
      * @since 7.1
      * @return void.
      */
    void RunL();

public:  // From MUpdateAllFeedsTaskObserver
    /**
     * Called to notify the obsever how many feeds remain to be updated.
     *
     * @since 7.1
     * @param aStatusCode The completion status of the request.
     * @return Void
     */
    virtual void UpdateAllFeedsProgress(TInt aMaxCount, TInt aRemaining);

    /**
     * Called upon completion of the task.
     *
     * @since 7.1
     * @param aStatusCode The completion status of the request.
     * @return Void
     */
    virtual void UpdateAllFeedsCompleted(TInt aStatusCode);

    /**
     * Returns the http-connection instance;
     *
     * @since 7.1
     * @return The http-connection instance.
     */
    virtual CHttpConnection& HttpConnection();

public:

    /**
     * Two-phased constructor.
     * @since 7.1
     * @param aFolderListId The folder list associate with this background update.
     * @param aAutoUpdateFreq The frequency of this background update.
     * @param aAutoUpdateAP The access point to use for this background update.
     * @param aAutoUpdateWhileRoaming The flag.
     */
     static CUpdateManager* NewL( TInt aFolderListId, TUint32 aAutoUpdateAP, TInt aAutoUpdateFreq, 
             TBool aAutoUpdateWhileRoaming, CFeedsServer& aFeedsServer);

    /**
     *
     * @since 7.1
     * Destructor
     */
    ~CUpdateManager();

    /**
     * This Method calls update Feed
     *
     * @since 7.1
     * @ return Integer
     */
    TInt UpdateL();
   
    /**
     * This method handles the updation of feeds 
     *     
     * @since 7.1
     * @return Integer.
     */
    TInt StartL(); 

    /**
     * This method Stop the updation;
     *
     * @since 7.1
     * Stop the Update timer
     */
    void Stop();

    /**
     * This method Returns the FolderListId;
     *
     * @since 7.1
     * @return The folder list ID.
     */
    TInt FolderListId();

    /**
     * This method Updates the update queue;
     *
     * @since 7.1
     * @return The folder list ID.
     */
    void UpdateFeedL(TInt aFeedId, TBool aDeleteFeed);

    /**
     * This method is a call back;
     *
     * @since 7.1
     * @return The folder list ID.
     */
    static TBool LazyCallBack(TAny* aPtr);

    /**
     * This method Sets the next timer event;
     *
     * @since 7.1
     * @return The folder list ID.
     */
    void StartTimer();

    /**
     * This method Adds feeds in queue;
     *
     * @since 7.1
     * @return The folder list ID.
     */
    void AddFeedL(TInt aFeedId);

private:        
    /**
     *
     * @since 7.1
     * By default Symbian 2nd phase constructor is private.
     */ 
    void ConstructL( TUint32 aAutoUpdateAP);

    /**
     *
     * @since 7.1
     * C++ default constructor.
     */
    CUpdateManager( TInt aFolderListId, TInt aAutoUpdateFreq, TBool aAutoUpdateWhileRoaming, CFeedsServer& aFeedsServer);

private:
    TInt                        iAutoUpdateFreq;
    TBool                       iAutoUpdateWhileRoaming;
    TInt                        iFolderListId;
    TUint32                     iAutoUpdateAp;
    RPointerArray<CUpdateQueue> iQueueArray;
    CFeedsServer&               iFeedsServer;
    CUpdateAllFeedsTask*        iUpdateAllFeedTask;
    CServerHttpConnection*      iHttpConnection;

    CIdle*                      iLazyCaller;
    TInt                        iCurrentFeedCount;
    RArray<TInt>                iFeedIds;
    TInt                        iMins;

    CRoamingInfo*               iRoamingInfo;
    TTime                       iLastAutoUpdate;

    //friend class
    friend class CRoamingInfo;
    friend class CFeedsServer;
    };

/**
*  The feed RoamingInfo.
*
*  \b Library: FeedsEngine.lib
*
*  @since 7.1
*/
class CRoamingInfo: public CActive
    {
public:  // Constructors and destructor
   /**
    * Two-phased constructor.
    *
    * @since 7.1
    * @param aFolderListId The folder list associate with this background update.
    * @param aAutoUpdateFreq The frequency of this background update.
    * @param aAutoUpdateAP The access point to use for this background update.
    * @param aAutoUpdateWhileRoaming The flag.
    */
	static CRoamingInfo* NewL(CUpdateManager *aUpdateManager);

   /**
    * @since 7.1
    * Destructor.
    */
    virtual ~CRoamingInfo();

public:  // From CActive
   /**
    * Handles an active object's request completion event.
    *
    * @since 7.1
    * @return void.
    */
    void RunL();
    void DoCancel();

public:

   /**
    * Check for roaming.
    *
    * @since 7.1
    * @return void.
    */
	void CheckForRoaming();

private:
   /**
    * @since 7.1
    * C++ default constructor.
    *
    */
    CRoamingInfo(CUpdateManager *aUpdateManager);

   /**
    * @since 7.1
    * By default Symbian 2nd phase constructor is private.
    *
    */
    void ConstructL();

private:

	CTelephony							*iTelephony;
	CTelephony::TNetworkRegistrationV1	iRegStatus;
	CUpdateManager					    *iUpdateManager;
	
    };


#endif //_UPDATEMANAGER_H_