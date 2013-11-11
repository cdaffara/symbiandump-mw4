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
* Description:  Provides the base class implementation for the media elements.
*
*/



// INCLUDE FILES
#include "SVGMediaElementBase.h"
#include "SVGAnimTimingParser.h"
#include "SVGTimeContainer.h"

_LIT( KTmpCanSlip, "canSlip" );
_LIT( KTmpLocked, "locked" );
_LIT( KTmpIndependent, "independent" );
_LIT( KTmpDefault, "default" );
_LIT( KTmpSyncBehavior, "syncBehavior" );
_LIT( KTmpSyncTolerance, "syncTolerance" );
_LIT( KTmpSyncMaster, "syncMaster" );
// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSvgMediaElementBase::CSvgMediaElementBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSvgMediaElementBase::CSvgMediaElementBase( CSvgDocumentImpl* aDoc ):
    CSvgAnimationBase( aDoc ),
    iSyncMasterConfig( EFalse ),
    iSyncMasterCurrent( EFalse ),
    iSyncOffset( 0 )
    {
    // Obtain the default sync behaviour/tolerance from the root element
    CSvgSvgElementImpl* lSvgEle =  (CSvgSvgElementImpl* )aDoc->RootElement();
    if ( lSvgEle )
        {
        iSyncBehavior = lSvgEle->SyncBehaviorDefault();
        iSyncTolerance = lSvgEle->SyncToleranceDefault();
        }    
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSvgMediaElementBase::ConstructL( const TUint8 aElemID )
    {
    CSvgAnimationBase::ConstructL( aElemID );
    // Add this element to the time container to recieve runtime sync events
    ((CSvgDocumentImpl* )iOwnerDocument)->TimeContainer()->AddTimedEntityL( this );
    }

// Destructor
CSvgMediaElementBase::~CSvgMediaElementBase()
    {
    // Remove this element from the time container
    ((CSvgDocumentImpl* )iOwnerDocument)->TimeContainer()->DelTimedEntity( this );
    }
    
// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SetSyncBehavior
// Set the Synchronised behaviour for the media element
// -----------------------------------------------------------------------------
//        
void CSvgMediaElementBase::SetSyncBehavior( const TDesC& aValue )
    {
    
	if ( !aValue.CompareF( KTmpCanSlip ) )
	    {
        iSyncBehavior = ESvgSyncCanSlip;        
	    }
	else if ( !aValue.CompareF( KTmpLocked ) )
	    {
        iSyncBehavior = ESvgSyncLocked;
	    }
	else if ( !aValue.CompareF( KTmpIndependent ) )
	    {
        iSyncBehavior = ESvgSyncIndependent;
	    }
	else if ( !aValue.CompareF( KTmpDefault ) )
	    {
        // Obtain the sync behaviour from the root of time container
        CSvgDocumentImpl* lSvgDoc = (( CSvgDocumentImpl* ) 
            iOwnerDocument);
        CSvgSvgElementImpl* lSvgEle =  (CSvgSvgElementImpl* )lSvgDoc->RootElement();
        if ( lSvgEle )
            {
            iSyncBehavior = lSvgEle->SyncBehaviorDefault();
            }
	    }
    }
        
// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SetSyncTolerance
// Set the Synchronised Tolerance for the media element
// -----------------------------------------------------------------------------
//        
void CSvgMediaElementBase::SetSyncTolerance( const TDesC& aValue )
    {
	if ( aValue.CompareF( KTmpDefault ) )
	    {
	    CSvgAnimTimingParser* lATParserPtr = NULL;
	    // Treat as clock value
	    TRAPD( lCreateATPErr, 
	        {
	        lATParserPtr = CSvgAnimTimingParser::NewL( aValue, this );
	        } );
	    
	    if ( lCreateATPErr != KErrNone )
	        {
	        // Error Processing
	        return;
	        }
        
        TInt32 lClockValue;
        TLex lLex( aValue );
        lATParserPtr->ParseClockValue( lLex, lClockValue );
        delete lATParserPtr;
        iSyncTolerance = lClockValue; 
	    }
	else
	    {
	    // Get clock value from parent
        CSvgDocumentImpl* lSvgDoc = (( CSvgDocumentImpl* ) 
            iOwnerDocument);
        CSvgSvgElementImpl* lSvgEle =  (CSvgSvgElementImpl* )lSvgDoc->RootElement();
        if ( lSvgEle )
            {
            iSyncTolerance = lSvgEle->SyncToleranceDefault();
            }	    
	    }
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SetSyncMaster
// Set the media element as Synchronised Master
// -----------------------------------------------------------------------------
//        
void CSvgMediaElementBase::SetSyncMaster( const TDesC& aValue )
    {
    if ( !aValue.CompareF(_L("true")) )
        {
        iSyncMasterConfig = ETrue;
        }
    else
        {
        iSyncMasterConfig = EFalse;
        }
    }
    
// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SetSyncBehavior
// Set the Synchronised behaviour for the media element
// -----------------------------------------------------------------------------
//        
void CSvgMediaElementBase::SetSyncBehavior( 
    const TSvgSyncBehaviour aValue )
    {
    if ( aValue == ESvgSyncDefault )
        {
        // Obtain the sync behaviour from the root of time container
        CSvgDocumentImpl* lSvgDoc = (( CSvgDocumentImpl* ) 
            iOwnerDocument);
        CSvgSvgElementImpl* lSvgEle =  (CSvgSvgElementImpl* )lSvgDoc->RootElement();
        if ( lSvgEle )
            {
            iSyncBehavior = lSvgEle->SyncBehaviorDefault();
            }
        }
     else
        {
        iSyncBehavior = aValue;
        }
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SetSyncTolerance
// Set the Synchronised Tolerance for the media element
// -----------------------------------------------------------------------------
//        
void CSvgMediaElementBase::SetSyncTolerance( const TInt32 aValue , 
    const TBool aUseDefault )
    {
    if ( aUseDefault )
        {
	    // Get clock value from parent
        CSvgDocumentImpl* lSvgDoc = (( CSvgDocumentImpl* ) 
            iOwnerDocument);
        CSvgSvgElementImpl* lSvgEle =  (CSvgSvgElementImpl* )lSvgDoc->RootElement();
        if ( lSvgEle )
            {
            iSyncTolerance = lSvgEle->SyncToleranceDefault();
            }	    
        }
        else
            {
            iSyncTolerance = aValue;            
            }
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SetSyncMaster
// Set the media element as Synchronised Master
// -----------------------------------------------------------------------------
//        
void CSvgMediaElementBase::SetSyncMaster( const TBool aValue )    
    {
    iSyncMasterConfig = aValue;
    }
    
// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SyncBehavior
// Accessor for the Synchronised behaviour for the media element
// -----------------------------------------------------------------------------
//        
TSvgSyncBehaviour CSvgMediaElementBase::SyncBehavior()    
    {
    return iSyncBehavior;
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SyncTolerance
// Accessor for the Synchronised Tolerance for the media element
// -----------------------------------------------------------------------------
//        
TInt32 CSvgMediaElementBase::SyncTolerance()    
    {
    return iSyncTolerance;
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SyncMaster
// Accessor for the Synchronised Mastership for the media element
// -----------------------------------------------------------------------------
//        
TBool CSvgMediaElementBase::SyncMaster()    
    {
    return iSyncMasterConfig;
    }
    
// -----------------------------------------------------------------------------
// SetAttributeL: Called by Contenthandler or decoder to set attributes
// From CSvgAnimationBase
// -----------------------------------------------------------------------------
//
TInt CSvgMediaElementBase::SetAttributeL( const TDesC& aName,
                                        const TDesC& aValue )
    {
    
    if ( !aName.CompareF( KTmpSyncBehavior ) )
        {
        SetSyncBehavior( aValue );
        }
    else if ( !aName.CompareF( KTmpSyncTolerance ) )
        {
        SetSyncTolerance( aValue );
        }
    else if ( !aName.CompareF( KTmpSyncMaster ) )
        {
        SetSyncMaster( aValue );
        }
    else
        {
        return CSvgAnimationBase::SetAttributeL( aName, aValue );
        }
        
    return KErrNone;
    }
                                        
// -----------------------------------------------------------------------------
// CSvgMediaElementBase::ParentTimeContainerTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//
void CSvgMediaElementBase::ParentTimeContainerTick( 
    TSvgTick aTick ) // Current tick information 
    {
    // Check if the element is currently acting as syncMaster for the locked 
    // group it is part of.
    // Note element having syncMaster property set doesn't make it master. It 
    // needs to be resolved.
    TBool lIsCurSyncMaster = EFalse;

    GetCurSyncMasterStatus( lIsCurSyncMaster );
    
     if ( lIsCurSyncMaster || iSyncBehavior != ESvgSyncLocked ) 
     {
        // Could be locked, or not locked. In both cases, use media 
        // element clock.
        if ( CanGenerateTick() ) 
            {
            //No action.
            }
        else
            {
            // Use the Real Time Tick.
            // Create a timer event with real time tick
            // Send this event to engine
            }
     }
     
    if( !lIsCurSyncMaster  && iSyncBehavior == ESvgSyncLocked)
     {
        // The element needs to be in sync with Parent.
        if ( CanUseParentTick() )
        {
            // Run the media element with parent's tick.
            // Create a timer event with parent's tick
            // Send this event to engine
            // Only required at Document Level.
         }
        else
        {
            // Calculate SyncOffset value
            // Get the current tick on the element
            TUint32 lEntityTime;
            GetEntityCurrentTime( lEntityTime ); 
            // Calculate the drift
            TInt32 iSyncOffset = aTick.iParentTcTick - lEntityTime;
            
            // Media element running with its own clock, need 
            // to resynchronize if the element is out of sync 
            // wrt to parent clock.
            if ( Abs( iSyncOffset ) > iSyncTolerance )
                {
                // Resynchronize the Media Element wrt to 
                // Parent Clock.
                ResyncTimedEntity( aTick.iParentTcTick );
                }
          }
       }
     
    }
    
// -----------------------------------------------------------------------------
// CSvgMediaElementBase::GetEntitySyncBehavior
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
TSvgSyncBehaviour CSvgMediaElementBase::GetEntitySyncBehavior()
    {
    return iSyncBehavior; 
    }
        

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::GetEntityCurrentTime
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
void CSvgMediaElementBase::GetEntityCurrentTime( TUint32& 
            /* aEntityCurTime*/ ) // Current Entity Time in msecs. 
    {
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::GetCnfSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
void CSvgMediaElementBase::GetCnfSyncMasterStatus( 
            TBool& aIsSyncMaster ) // Indicates whether the element is configured as 
                                   // Sync Master. 
    {
    aIsSyncMaster = iSyncMasterConfig;
    }                                   

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::GetCurSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgMediaElementBase::GetCurSyncMasterStatus( 
            TBool& aIsSyncMaster ) // Indicates whether the element is currrently 
                                  // Sync Master. 
    {
    aIsSyncMaster = iSyncMasterCurrent;
    }        


// -----------------------------------------------------------------------------
// CSvgMediaElementBase::SetCurSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgMediaElementBase::SetCurSyncMasterStatus( 
    TBool aSyncMasterStatus ) //  Indicates whether the element is currrently 
                              // Sync Master.
    {
    iSyncMasterCurrent = aSyncMasterStatus;
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::CanGenerateTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
TBool CSvgMediaElementBase::CanGenerateTick()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::CanUseParentTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
TBool CSvgMediaElementBase::CanUseParentTick()
    {
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CSvgMediaElementBase::ResyncTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgMediaElementBase::ResyncTimedEntity( 
            TUint32 /* aSynctime */ ) // Time for resync in msecs.
    {
    
    }
    
// -----------------------------------------------------------------------------
// CSvgMediaElementBase::PauseTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgMediaElementBase::PauseTimedEntity()
    {
    
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::ResumeTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgMediaElementBase::ResumeTimedEntity()
    {
    
    }

// -----------------------------------------------------------------------------
// CSvgMediaElementBase::StopTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgMediaElementBase::StopTimedEntity()
    {
    
    }
 

//From MSvgTimedEntityInterface
TSvgObjectType CSvgMediaElementBase::ObjectType()
{
	return ESvgInvalidReference ;	
}


//returns the child time container of the element 
//used in timecontainer
CSvgTimeContainer* CSvgMediaElementBase::GetChildTimeContainer()
{
    return NULL;
}
    
//  End of File  
