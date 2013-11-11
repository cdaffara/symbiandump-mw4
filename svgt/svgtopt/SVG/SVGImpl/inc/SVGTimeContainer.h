/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines an encapsulation for timed entities belonging to a 
*                logical group
*
*/



#ifndef CSVGTIMECONTAINER_H
#define CSVGTIMECONTAINER_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include "SVGTimedEntityInterface.h"
#include "SVGTimer.h"



#include "SVGMediaElementBase.h"




//User state    
enum TSvgEntityUserState 
{ 
	ESvgUserRunning, 
	ESvgUserPaused, 
	ESvgUserStopped
};

//Media state
enum TSvgEntityMediaState 
{ 
	ESvgMediaIdle, 
	ESvgMediaNotReady,
    ESvgMediaReady 
};



// FORWARD DECLARATIONS
class CSvgDocumentImpl;
        
class CSvgLockedRealTimeEntity;        
// CLASS DECLARATION
/**
 *  Defines a timed entity list item. It stores the timed entity pointer and
 *  state(NotReady/Ready).
 */
class TSvgTimedEntityListItem
    {
    public:

        // Pointer to timed entity
        MSvgTimedEntityInterface* iTimedEntity;
        // Indicates the user state of the timed entity ( Running/Paused/Stopped)
        TSvgEntityUserState iUserState;
        // Indicates the Media state of the timed entity (Ready/NotReady/Idle)
        TSvgEntityMediaState iMediaState;
        // Indicates the sync status of the timed entity (True = Paused/False = Playing)
        TBool iIsSyncPaused;


    };

/**
*  This interface can be to observe the time container state changes. 
*  For eg. by animation element to observe the child time container state 
*  changes and propogate the same to parent time container.
*  @lib SVGTEngine.lib
*  @since S60 3.2
*/
class MSvgTimeContainerObserver
    {
   public: // New functions
        /**
        * Indicates that all locked elements are ready to render.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        virtual void TimeContainerReady() = 0;

        /**
        * Indicates that at least one locked element is not ready to render.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        virtual void TimeContainerNotReady() = 0;
    };
/**
*  Defines an encapsulation for timed entities belonging to a logical group.
*  The time container decides the timing clock fed to the timed entities belonging 
*  to it based on which entity has been resolved as timing master within the group. 
*  The time container could belong to another parent time container. The child 
*  container behaves as a timed entity from point of view of the parent time 
*  container.
*
*  @lib SVGTEngine.lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CSvgTimeContainer ) : public CBase,
                          public MSvgTimedEntityInterface
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aOwnerDoc Owner SVG Document
        *        aHasParent Indicates whether this timecontainer is the top level
        *                   container
        *        aSyncBehavior Sync Behaviour of this container wrt to parent
        *        aIsSyncMaster Is this TC a sync master wrt to parent
        *        aSyncTolerance Tolerance configured for the TC (in msecs)
        * @return CSvgTimeContainer* Pointer to constructed object
        */
        static CSvgTimeContainer* NewL( CSvgDocumentImpl* aOwnerDoc,
            const TBool aHasParent,
            const TSvgSyncBehaviour aSyncBehavior = ESvgSyncLocked,
            const TBool aIsSyncMaster = EFalse,
            const TUint32 aSyncTolerance = KSvgDefaultSyncTolerance );
        
        /**
        * Destructor.
        */
        virtual ~CSvgTimeContainer();

    public: // New functions
        /**
        * Resolves the sync master within the time container and returns
        * the same.
        * @since S60 3.2
        * @param none.
        * @return MSvgTimedEntityInterface* Pointer to Sync Master Implementation
        */
        MSvgTimedEntityInterface* GetSyncMasterForTimeContainer();
        
        /**
        * Add a timed entity to the time container.
        * @since S60 3.2
        * @param aTimedEntity An SVG Timed entity that should be added to this 
        *                     container(that implements the 
        *                     MSvgTimedEntityInterface)
        * @return None
        */
        void AddTimedEntityL( MSvgTimedEntityInterface* aTimedEntity );

        /**
        * Removes the timed entity from the time container.
        * @since S60 3.2
        * @param aTimedEntity An SVG Timed entity that should be added to this 
        *                     container(that implements the 
        *                     MSvgTimedEntityInterface)
        * @return None
        */
        void DelTimedEntity( MSvgTimedEntityInterface* aTimedEntity );
        
        
        /**
        * Removes all the timed entities from the time container.
        * @since S60 3.2
        * @param None
        * @return None
        */
        void CleanupTimedEntities( );
        /**
        * Set Function for the TC's current time
        * @since S60 3.2
        * @param aCurTime Current Time to be set in msecs
        * @return none.
        */
        void SetCurrentTime( const TInt32 aCurTime );

        /**
        * Accessor Function for the TC's current time
        * @since S60 3.2
        * @param none.
        * @return TInt32 Current Time to be set in msecs.
        */
        TInt32 CurrentTime();

        /**
        * Set the Synchronised behaviour for the time container
        * @since S60 3.2
        * @param aValue canSlip | locked | independent 
        * @return none.
        */
        void SetSyncBehavior( const TSvgSyncBehaviour aValue );
        
        /**
        * Set the Synchronised Tolerance for the time container
        * @since S60 3.2
        * @param aValue Clock-value in milliseconds
        * @return none.
        */        
        void SetSyncTolerance( const TUint32 aValue );
        
        /**
        * Set the time container as Synchronised Master
        * @since S60 3.2
        * @param aValue true | false
        * @return none.
        */        
        void SetSyncMaster( const TBool aValue );

        /**
        * Accessor function to get the current tick in the TC
        * @since S60 3.2
        * @param none.
        * @return TSvgTick Current tick information.
        */        
        TSvgTick GetCurTick();
        
        /**
        * Accessor function to get the owner svg doc of the TC
        * @since S60 3.2
        * @param none.
        * @return CSvgDocumentImpl* Document Pointer
        */        
        CSvgDocumentImpl* Document();

        /**
        * Informing the time container that timed entity is not ready
        * @since S60 3.2
        * @param aTimeDuration time duration in msecs.
        * @return none.
        */ 
        void TimedEntityNotReady( MSvgTimedEntityInterface* aTimedEntity );
        
        /**
        * Informing the time container that timed entity is ready
        * @since S60 3.2
        * @param aTimeDuration time duration in msecs.
        * @return none.
        */ 
        void TimedEntityReady( MSvgTimedEntityInterface* aTimedEntity );


      /**
        * Sets the observer for the time container state changes.
        * @since Series 3.2
        * @param aTCObserver Time container observer(NULL if not used)        
        * @return none.
        */        
        void SetTcObserver( MSvgTimeContainerObserver* aTCObserver );

        /**
        * Called to handle user/client generated stop event
        * @since Series 3.2
        * @param none.
        * @return none.
        */        
        void UserStop();
        
        /**
        * Called to handle user/client generated resume event
        * @since Series 3.2
        * @param aTime Time in msecs to resume after.
        * @return none.
        */        
        void UserResume( TInt32 aTime = KMinSleepDurationInMsecs );

        /**
        * Called to handle user/client generated play event
        * @since Series 3.2
        * @param none.
        * @return none.
        */        
        void UserPlay();

        /**
        * Called to handle user/client generated pause event
        * @since Series 3.2
        * @param none.
        * @return none.
        */        
        void UserPause();

        /**
        * Called to handle user/client generated seek event
        * @since Series 3.2
        * @param aTime Time expressed in msecs.
        * @return none.
        */        
        void UserSeek( TInt aTime );

        /**
        * Resets the time variables in the timer
        * @since S60 3.2
        * @param none.
        * @return none.
        */               
        void UserResetTime();
        
        /**
        * Changes the frame duration in the timer
        * @since S60 3.2
        * @param aTimeDuration time duration in msecs.
        * @return none.
        */               
        void UserChangeFrameDuration( TUint32 aTimerDuration );
        
        /**
        * Debug api to obtain FPS info
        * @since S60 3.2
        * @param none.
        * @return TUint FPS info.
        */               
        TUint UserFps();
                
              
        /**Element to resume after it gets a goahead from sync
        * @since S60 3.2
        * @param TSvgTimedEntityListItem
        * @return none
        */
		void SyncResumeElement(TSvgTimedEntityListItem* aTimedEntity);
		
		/**Element to pause after it gets Paused due to sync
		* @since S60 3.2
        * @param TSvgTimedEntityListItem
        * @return none
        */
		void SyncPauseElement(TSvgTimedEntityListItem* aTimedEntity);
		
		/**User calls resume
		* @since S60 3.2
        * @param TSvgTimedEntityListItem
        * @return none
        */
		void UserResumeElement(TSvgTimedEntityListItem* aTimedEntity);
		
		/**User Stop is called
		* @since S60 3.2
        * @param TSvgTimedEntityListItem
        * @return none
        */
		void UserStopElement(TSvgTimedEntityListItem* aTimedEntity);
		
		/**User Pause
		* @since S60 3.2
        * @param TSvgTimedEntityListItem
        * @return none
        */
		void UserPauseElement(TSvgTimedEntityListItem* aTimedEntity);
		
		/**To pause tc 
		* @since S60 3.2
        * @param none
        * @return none
        */
		void SyncPause();
		
		/**to resume tc
		* @since S60 3.2
        * @param none
        * @return none
        */
		void SyncResume();
        

    public: // Functions from base classes
        /**
        * From MSvgTimedEntityInterface
        * The parent time container provides the timing clock to 
        * the timed entity (audio, video, animation, document) 
        * using this routine.
        * @since Series 3.2
        * @param aTick Current tick information 
        * @return none.
        */
        void ParentTimeContainerTick( TSvgTick aTick );

        /**
        * From MSvgTimedEntityInterface
        * Returns the sync behavior of the entity.
        * @since Series 3.2
        * @param none. 
        * @return TSvgSyncBehaviour Element's Sync Behaviour.
        */
        TSvgSyncBehaviour GetEntitySyncBehavior();
        
        /**
        * From MSvgTimedEntityInterface
        * When the timed entity acts as timing master in the time container, 
        * the time container gets the timed entity clock using this method 
        * and feeds to rest of timed entities.
        * @since Series 3.2
        * @param aEntityCurTime Current Entity Time in msecs. 
        * @return none.
        */
        void GetEntityCurrentTime( TUint32& aEntityCurTime );

        /**
        * From MSvgTimedEntityInterface
        * Returns the configured sync master value(as per DOM tree) as specified 
        * in the SVG content.
        * @since Series 3.2
        * @param aIsSyncMaster Indicates whether the element is configured as 
        *                      Sync Master. 
        * @return none.
        */
        void GetCnfSyncMasterStatus( TBool& aIsSyncMaster );

        /**
        * From MSvgTimedEntityInterface
        * Check if timed entity is going to act as timing master in the 
        * time container. This behavior could change dynamically.
        * @since Series 3.2
        * @param aIsSyncMaster Indicates whether the element is currrently Sync Master. 
        * @return none.
        */
        void GetCurSyncMasterStatus( TBool& isSyncMaster );

        /**
        * From MSvgTimedEntityInterface
        * If some other element is resolved as syncMaster in the time container group, 
        * this element can not act as sync master. 
        * @since Series 3.2
        * @param aSyncMasterStatus Indicates whether the element is currrently 
        *                          Sync Master. 
        * @return none.
        */
        void SetCurSyncMasterStatus( TBool aSyncMasterStatus );

        /**
        * From MSvgTimedEntityInterface
        * Check if timed entity can provide timing ticks to rest of time 
        * container elements. This behavior could change dynamically. 
        * For example, if audio clip is over, the audio element can't generate 
        * ticks for others.
        * @since Series 3.2
        * @param none.
        * @return TBool True if can generate timing tick.
        */
        TBool CanGenerateTick();

        /**
        * Check if timed entity can use its parent's tick. Usually only the
        * parent document should return true for this function.
        * All other elements return false
        * @since Series 3.2
        * @param none.
        * @return TBool True if can use parent's timing tick.
        */
        TBool CanUseParentTick();

        /**
        * From MSvgTimedEntityInterface
        * If the timed entity needs to be in sync with the time container and 
        * it has slipped beyond the sync tolerance limit, the method is called to 
        * bring the element in sync with the time container.
        * @since Series 3.2
        * @param aSynctime Time for resync in msecs.
        * @return none.
        */
        void ResyncTimedEntity( TUint32 aSynctime );

        /**
        * From MSvgTimedEntityInterface
        * This would be used for pausing the timed entity while other locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void PauseTimedEntity();

        /**
        * From MSvgTimedEntityInterface
        * This would be used for resuming the timed entity once all locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void ResumeTimedEntity();

        /**
        * From MSvgTimedEntityInterface
        * This would be used for stopping the timed entity.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void StopTimedEntity();


        /*
        * From MSvgTimedEntityInterface
        * Would return the type of object
        * @param none
        * @return type of object
        */
        TSvgObjectType ObjectType();


        /*
        * From MsvgTimedEntityInterface
        * @param none
        * @return CSvgTimeContainer
        */
        CSvgTimeContainer* GetChildTimeContainer();
        
    protected:  // New functions
        
        

    protected:  // Functions from base classes
        
        

    private:
        
        /**
        * C++ default constructor.
        * @param aOwnerDoc  Owner SVG Document
        *        aHasParent Indicates whether this timecontainer is the top level
        *                   container        
        *        aSyncBehavior Sync Behaviour of this container wrt to parent
        *        aIsSyncMaster Is this TC a sync master wrt to parent
        *        aSyncTolerance Tolerance configured for the TC (in msecs)        
        */
        CSvgTimeContainer( CSvgDocumentImpl* aOwnerDoc,
            const TBool aHasParent, const TSvgSyncBehaviour aSyncBehavior, 
            const TBool aIsSyncMaster, const TUint32 aSyncTolerance );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CSvgTimeContainer( const CSvgTimeContainer& );
        // Prohibit assigment operator if not deriving from CBase.
        // CSvgTimeContainer& operator=( const CSvgTimeContainer& );

        /**
        * Propogate tick to all elements in time container
        * @param aTick Tick information to be propogated
        */
        void PropogateTickToContainerElements( TSvgTick& aTick );

        /**
        * Rearrange the timed entities as per post order
        * @param aPostOrderList - List of CSvgElementImpl( which are Media 
        *      Elements) in post order
        */
        void RearrangeTimedEntityList( RPointerArray<CSvgElementImpl>& 
            aPostOrderList );
        
        /**
        * Pauses the time container's locked entities. Called by 
        * animation element to indicate that the PauseTimedEntity()
        * is called on it.
        * @since Series 3.2
        * @param none.
        * @return none.
        */        
        void PauseLockedEntities();

        /**
        * Resumes the time container's locked entities. Called by 
        * animation element to indicate that the ResumeTimedEntity()
        * is called on it.
        * @since Series 3.2
        * @param none.
        * @return none.
        */        
        void ResumeLockedEntities();
        

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

        /*
        * Sets the time continer user state to the specified state
        * @param user state to be set
        * @return None
        */
        void SetUserState(TSvgEntityUserState state);
        
        /*
        * Sets the time continer media state to the specified state
        * @param media state to be set
        * @return None
        */
        void SetMediaState(TSvgEntityMediaState state);
        
        /*
        * Sets the time continer Sync state to the specified state
        * @param sync state to be set
        * @return None
        */
        void SetSyncState(TBool state);
        
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    private:


    private:    // Data
        // The time container's sync behavior with respect to its parent 
        // time container.
        TSvgSyncBehaviour iTcSyncBehavior;
        // Sync Tolerance configured for the time container (in msecs)
        TUint32 iTcSyncTolerance;
        // The time container's sync Master behavior defined in SVG file.
        TBool iTcSyncMasterConfig;
        // Is the timer container acting as timing master for parent 
        // time container currently?
        TBool iTcSyncMasterCurrent;
        // Sync Behavior default value for the timed entities within the 
        // time container, if they do not have the sync behavior defined for 
        // them.
        TSvgSyncBehaviour iTcSyncBehaviorDefault;
        // Sync Tolerance limit default value (in msecs) for the timed 
        // entities within the time container, if they do not have the sync 
        // tolerance defined for them.
        TUint32 iTcSyncToleranceDefault;
        // Pointer to the timed entity acting as the timing master within the 
        // time container. The sync master element could change dynamically.
        MSvgTimedEntityInterface* iTcSyncMaster;
        // The list of the timed entities belonging to this time container.
        RArray<TSvgTimedEntityListItem> iTcTimedEntities;
        // The flag defining whether the time container has a parent time 
        // container to which it belongs.
        TBool iIsParentTcPresent;
        // If there is no parent time container to which the time container 
        // belongs, then the time container creates its own timing clock by 
        // installing this timer.
        CSvgTimer* iTimer;
        // Current time in msecs
        TInt32 iCurrentTime;
        // Current Tick stored here so that it can be accessed by 
        // timed entities which are offset
        TSvgTick iCurrentTick;
        // Owner SVG Document Impl
        CSvgDocumentImpl* iOwnerDoc;
        // Time container observer implementation pointer
        MSvgTimeContainerObserver* iObserver;
        // Time container Media state information
        TSvgEntityMediaState iMediaState;
        //Time Container User state information
        TSvgEntityUserState iUserState;
        //Sync information abt the timecontainer
        TBool iIsSyncPaused;

        // The Locked real time entity is a special timed entity 
        // and the list item entry is owned by the time container        
        TSvgTimedEntityListItem iLrteListItem;

        // Locked real time entity pointer - owned
        CSvgLockedRealTimeEntity* iLrte;

        // Reserved pointer for future extension
        //TAny* iReserved;
        

		
    };

#endif      // CSVGTIMECONTAINER_H   
            
// End of File
