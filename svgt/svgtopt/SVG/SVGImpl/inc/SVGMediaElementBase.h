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
* Description:  SVGT Media Elements common implementation
*
*/



#ifndef CSVGMEDIAELEMENTBASE_H
#define CSVGMEDIAELEMENTBASE_H

//  INCLUDES
#include <e32std.h>
#include <e32def.h>
#include "SVGAnimationBase.h"
#include "SVGTimedEntityInterface.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// CLASS DECLARATION

/**
*  Base class for media elements.
*  The base class providing the run time synchronization property handling 
*  functionality. The media elements like audio, video, animation derive 
*  from this class.
*
*  @lib SVGTEngine.lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CSvgMediaElementBase ) : 
    public CSvgAnimationBase,
    public MSvgTimedEntityInterface
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CSvgMediaElementBase();

    public: // New functions

        /**
        * Set the Synchronised behaviour for the media element
        * @since S60 3.2
        * @param aValue "canSlip" | "locked" | "independent" | "default"
        * @return none.
        */
        virtual void SetSyncBehavior( const TDesC& aValue );
        
        /**
        * Set the Synchronised Tolerance for the media element
        * @since S60 3.2
        * @param aValue Clock-value | "default"
        * @return none.
        */        
        virtual void SetSyncTolerance( const TDesC& aValue );
        
        /**
        * Set the media element as Synchronised Master
        * @since S60 3.2
        * @param aValue "true" | "false"
        * @return none.
        */        
        virtual void SetSyncMaster( const TDesC& aValue );

        /**
        * Set the Synchronised behaviour for the media element
        * @since S60 3.2
        * @param aValue canSlip | locked | independent | default
        * @return none.
        */
        virtual void SetSyncBehavior( 
            const TSvgSyncBehaviour aValue );
        
        /**
        * Set the Synchronised Tolerance for the media element
        * @since S60 3.2
        * @param aValue Clock-value 
        *        aUseDefault True/False indicates use default from 
        *                    parent svg element.
        * @return none.
        */        
        virtual void SetSyncTolerance( const TInt32 aValue ,
             const TBool aUseDefault);
        
        /**
        * Set the media element as Synchronised Master
        * @since S60 3.2
        * @param aValue true | false
        * @return none.
        */        
        virtual void SetSyncMaster( const TBool aValue );

        /**
        * Accessor for the Synchronised behaviour for the media element
        * @since S60 3.2
        * @param none. 
        * @return TSvgSyncBehavior "canSlip" | "locked" | "independent" | "default"
        */
        virtual TSvgSyncBehaviour SyncBehavior();
        
        /**
        * Accessor for the Synchronised Tolerance for the media element
        * @since S60 3.2
        * @param none.
        * @return Clock-value.
        */        
        virtual TInt32 SyncTolerance();
        
        /**
        * Accessor for the media element is a Synchronised Master
        * @since S60 3.2
        * @param none.
        * @return TBool "true" | "false"
        */        
        virtual TBool SyncMaster();
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CSvgAnimationBase Called by Contenthandler/decoder to set 
        * attributes
        * @since S60 3.2
        * @param aName Name of attribute
        * @param aValue Value of attribute
        * @return TInt Error Code
        */
        TInt SetAttributeL( const TDesC& aName, const TDesC& aValue );

        
    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CSvgMediaElementBase( CSvgDocumentImpl* aDoc );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TUint8 aElemID );
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
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


    public:

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
        * Check if timed entity can provide timing ticks to rest of time 
        * container elements. This behavior could change dynamically. 
        * For example, if audio clip is over, the audio element can't generate 
        * ticks for others.
        * @since Series 3.2
        * @param none.
        * @return TBool True if can generate timing tick.
        */
        TBool CanGenerateTick();
        

    protected:

        /**
        * From MSvgTimedEntityInterface
        * When the timed entity acts as timing master in the time container, 
        * the time container gets the timed entity clock using this method 
        * and feeds to rest of timed entities.
        * @since Series 3.2
        * @param aEntityCurTime Current Entity Time in msecs. 
        * @return none.
        */
        void GetEntityCurrentTime( TUint32& 
            aEntityCurTime );

        

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
        From MSvgTimedEntityInterface
        Would return the type of object
        @param none
        @return type of object
        */
        TSvgObjectType ObjectType();

        
        CSvgTimeContainer* GetChildTimeContainer();
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:


        // Prohibit copy constructor if not deriving from CBase.
        // CSvgMediaElementBase( const CSvgMediaElementBase& );
        // Prohibit assigment operator if not deriving from CBase.
        // CSvgMediaElementBase& operator=( const CSvgMediaElementBase& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        // The media element's sync behavior with respect to its parent 
        // time container.
        TSvgSyncBehaviour iSyncBehavior;
        // Boolean indicating whether the media element is configured 
        // (as per DOM tree) as a Sync Master
        TBool iSyncMasterConfig;
        // Boolean indicating whether media element is currently the Sync Master 
        // in the time container
        TBool iSyncMasterCurrent;
        // Sync Tolerance configured for the media element (in msecs)
        TUint32 iSyncTolerance;
        // how much out-of-sync is this media element (in msecs) 
        TUint32 iSyncOffset;


    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
         
        // Reserved pointer for future extension
        //TAny* iReserved;
    };

#endif      // CSVGMEDIAELEMENTBASE_H   
            
// End of File
