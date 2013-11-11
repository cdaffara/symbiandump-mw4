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
* Description:  This is the interface that is to be implemented by
*                all media elements.
*
*/



#ifndef MSVGTIMEDENTITYINTERFACE_H
#define MSVGTIMEDENTITYINTERFACE_H

//  INCLUDES
#include <e32std.h>
#include <e32def.h>

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
enum TSvgSyncBehaviour
    {
    ESvgSyncDefault = 0,
    ESvgSyncCanSlip,
    ESvgSyncLocked,
    ESvgSyncIndependent    
    };



enum TSvgObjectType
{
	ESvgInvalidReference = 0,
	ESvgAudioElement,
	ESvgMediaAnimationElement,
	ESvgDocumentElement,
	ESvgLockedRealTimeEntity
};


// CONSTANTS
//const ?type ?constant_var = ?constant;
// Default Sync Behaviour is locked
const TSvgSyncBehaviour KSvgDefaultSyncBehaviour = ESvgSyncLocked;
// Default Sync Tolerance is 2 sec
const TUint32 KSvgDefaultSyncTolerance = 2*1000; 
    
// CLASS DECLARATION
class CSvgTimeContainer;
/**
*  This class stores the current tick timestamp and the parent time container tick.
*  This is used to propogate the tick from sync master to the rest of the members 
*  in the time container (or) if the syncmaster cannot generate a tick then the
*  real time tick is used.
*
*  @lib SVGTEngine.lib
*  @since S60 3.2
*/
class TSvgTick
    {
    public: // Data
        // Real time tick expressed in msecs from start of timer
        TUint32 iRealTimeTick;
        // Parent time container tick expressed in msecs
        TUint32 iParentTcTick;
    };

    
/**
*  Interface exported by all media elements.
*  This interface class specifies the interface for implementing
*  runtime synchronisation. This interface is inherited by
*  media elements such as audio/animation which provide element 
*  specific processing.
*  Since the root SVG possesses an implicit time container the 
*  document also needs to implement this interface.
*
*  @lib SVGTEngine.lib
*  @since S60 3.2
*/
class MSvgTimedEntityInterface
    {
    public: // New functions
        /**
        * The parent time container provides the timing clock to 
        * the timed entity (audio, video, animation, document) 
        * using this routine.
        * @since S60 3.2
        * @param aTick Current tick information 
        * @return none.
        */
        virtual void ParentTimeContainerTick( TSvgTick aTick ) = 0;

        /**
        * Returns the sync behavior of the entity.
        * @since S60 3.2
        * @param none. 
        * @return TSvgSyncBehaviour Element's Sync Behaviour.
        */
        virtual TSvgSyncBehaviour GetEntitySyncBehavior() = 0;
        
        /**
        * When the timed entity acts as timing master in the time container, 
        * the time container gets the timed entity clock using this method 
        * and feeds to rest of timed entities.
        * @since S60 3.2
        * @param aEntityCurTime Current Entity Time in msecs. 
        * @return none.
        */
        virtual void GetEntityCurrentTime( TUint32& 
            aEntityCurTime ) = 0;

        /**
        * Returns the configured sync master value(as per DOM tree) as specified 
        * in the SVG content.
        * @since S60 3.2
        * @param aIsSyncMaster Indicates whether the element is configured as 
        *                      Sync Master. 
        * @return none.
        */
        virtual void GetCnfSyncMasterStatus( TBool& aIsSyncMaster ) = 0;

        /**
        * Check if timed entity is going to act as timing master in the 
        * time container. This behavior could change dynamically.
        * @since S60 3.2
        * @param aIsSyncMaster Indicates whether the element is currrently Sync Master. 
        * @return none.
        */
        virtual void GetCurSyncMasterStatus( TBool& isSyncMaster ) = 0;

        /**
        * If some other element is resolved as syncMaster in the time container group, 
        * this element can not act as sync master. 
        * @since S60 3.2
        * @param aSyncMasterStatus Indicates whether the element is currrently 
        *                          Sync Master. 
        * @return none.
        */
        virtual void SetCurSyncMasterStatus( TBool aSyncMasterStatus ) = 0;

        /**
        * Check if timed entity can provide timing ticks to rest of time 
        * container elements. This behavior could change dynamically. 
        * For example, if audio clip is over, the audio element can't generate 
        * ticks for others.
        * @since S60 3.2
        * @param none.
        * @return TBool True if can generate timing tick.
        */
        virtual TBool CanGenerateTick() = 0;

        /**
        * Check if timed entity can use its parent's tick. Usually only the
        * parent document should return true for this function.
        * All other elements return false
        * @since S60 3.2
        * @param none.
        * @return TBool True if can use parent's timing tick.
        */
        virtual TBool CanUseParentTick() = 0;
        
        /**
        * If the timed entity needs to be in sync with the time container and 
        * it has slipped beyond the sync tolerance limit, the method is called to 
        * bring the element in sync with the time container.
        * @since S60 3.2
        * @param aSynctime Time for resync in msecs.
        * @return none.
        */
        virtual void ResyncTimedEntity( TUint32 aSynctime ) = 0;

        /**
        * This would be used for pausing the locked timed entity while other locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        virtual void PauseTimedEntity() = 0;

        /**
        * This would be used for resuming the locked timed entity once all locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        virtual void ResumeTimedEntity() = 0;

        /**
        * This would be used for stopping the timed entity.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        virtual void StopTimedEntity() = 0;
        

	    /* Return the type of class
        @Return Type Enumeration of object type
        @Parameters none
        */
        virtual TSvgObjectType ObjectType() = 0;
        
        
        virtual CSvgTimeContainer* GetChildTimeContainer() = 0; 

    };

#endif      // MSVGTIMEDENTITYINTERFACE_H
            
// End of File
