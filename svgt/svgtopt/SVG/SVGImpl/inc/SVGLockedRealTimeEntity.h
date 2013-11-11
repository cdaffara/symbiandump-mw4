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
#include "SVGTimedEntityInterface.h"
#include "SVGTimeContainer.h"

class CSvgDocumentImpl;

NONSHARABLE_CLASS( CSvgLockedRealTimeEntity ): public CSvgMediaElementBase                                
{
    
    public:
        static CSvgLockedRealTimeEntity* NewL(CSvgDocumentImpl* aDoc);
        
        void ConstructL(CSvgDocumentImpl* aDoc);
        
        CSvgLockedRealTimeEntity(CSvgDocumentImpl* aDoc);
        
        ~CSvgLockedRealTimeEntity();
        
        
       
        //from svgelementimpl
        void Print(TInt aValue) ;

                
    public: // from svgtimedentityinterface

        /**
        * The parent time container provides the timing clock to 
        * the timed entity (audio, video, animation, document) 
        * using this routine.
        * @since S60 3.2
        * @param aTick Current tick information 
        * @return none.
        */
        void ParentTimeContainerTick( TSvgTick aTick );
        /**
        * Returns the sync behavior of the entity.
        * @since S60 3.2
        * @param none. 
        * @return TSvgSyncBehaviour Element's Sync Behaviour.
        */
        TSvgSyncBehaviour GetEntitySyncBehavior();
        
        /**
        * When the timed entity acts as timing master in the time container, 
        * the time container gets the timed entity clock using this method 
        * and feeds to rest of timed entities.
        * @since S60 3.2
        * @param aEntityCurTime Current Entity Time in msecs. 
        * @return none.
        */
        void GetEntityCurrentTime( TUint32& 
            aEntityCurTime );
        /**
        * Returns the configured sync master value(as per DOM tree) as specified 
        * in the SVG content.
        * @since S60 3.2
        * @param aIsSyncMaster Indicates whether the element is configured as 
        *                      Sync Master. 
        * @return none.
        */
        void GetCnfSyncMasterStatus( TBool& aIsSyncMaster );

        /**
        * Check if timed entity is going to act as timing master in the 
        * time container. This behavior could change dynamically.
        * @since S60 3.2
        * @param aIsSyncMaster Indicates whether the element is currrently Sync Master. 
        * @return none.
        */
        void GetCurSyncMasterStatus( TBool& isSyncMaster );

        
        /** If some other element is resolved as syncMaster in the time container group, 
        * this element can not act as sync master. 
        * @since S60 3.2
        * @param aSyncMasterStatus Indicates whether the element is currrently 
        *                          Sync Master. 
        * @return none.
        */
        void SetCurSyncMasterStatus( TBool aSyncMasterStatus );

        /**
        * Check if timed entity can provide timing ticks to rest of time 
        * container elements. This behavior could change dynamically. 
        * For example, if audio clip is over, the audio element can't generate 
        * ticks for others.
        * @since S60 3.2
        * @param none.
        * @return TBool True if can generate timing tick.
        */
        TBool CanGenerateTick();
        
        /**
        * Check if timed entity can use its parent's tick. Usually only the
        * parent document should return true for this function.
        * All other elements return false
        * @since S60 3.2
        * @param none.
        * @return TBool True if can use parent's timing tick.
        */
        TBool CanUseParentTick();
        
        /**
        * If the timed entity needs to be in sync with the time container and 
        * it has slipped beyond the sync tolerance limit, the method is called to 
        * bring the element in sync with the time container.
        * @since S60 3.2
        * @param aSynctime Time for resync in msecs.
        * @return none.
        */
        void ResyncTimedEntity( TUint32 aSynctime );

        /**
        * This would be used for pausing the locked timed entity while other locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void PauseTimedEntity();

        /**
        * This would be used for resuming the locked timed entity once all locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void ResumeTimedEntity();

        /**
        * This would be used for stopping the timed entity.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void StopTimedEntity() ;
        

	    /* Return the type of class
        @Return Type Enumeration of object type
        @Parameters none
        */
        TSvgObjectType ObjectType();
        
        CSvgTimeContainer* GetChildTimeContainer() ; 
        
        TBool AnimProcL( MSvgTimerEvent* aEvent );
        void ResetAnimationL();
        
        private:
        
        // Current Tick stored here so that it can be accessed by 
        // timed entities which are offset
        TSvgTick iStoredTick;
        
        CSvgDocumentImpl* iOwnerDocument;
        
        TBool iIsPaused;
        
        TBool iSyncMasterCurrent;
};
