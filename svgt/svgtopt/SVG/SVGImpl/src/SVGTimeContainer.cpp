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
* Description:  Provides implementation of a time container
*
*/



// INCLUDE FILES
#include "SVGTimeContainer.h"
#include "SVGTimer.h"
#include "SVGEvent.h"
#include "SVGDocumentImpl.h"

#include "SVGLockedRealTimeEntity.h"

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
// CSvgTimeContainer::CSvgTimeContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSvgTimeContainer::CSvgTimeContainer( CSvgDocumentImpl* aOwnerDoc,
    const TBool aHasParent,
    const TSvgSyncBehaviour aSyncBehavior,
    const TBool aIsSyncMaster,
    const TUint32 aSyncTolerance ) : 
          iTcSyncBehavior( aSyncBehavior ),
          iTcSyncTolerance( aSyncTolerance ),
          iTcSyncMasterConfig( aIsSyncMaster ),
          iIsParentTcPresent( aHasParent ),
          iOwnerDoc( aOwnerDoc ),
          iMediaState( ESvgMediaReady ),
          iUserState(ESvgUserRunning),
          iIsSyncPaused(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::ConstructL()
    {
    //Add locked real time entity(LRTE) here
    iLrte = CSvgLockedRealTimeEntity::NewL(iOwnerDoc);
    // Populate the entity list item corresponding to LRTE
    iLrteListItem.iTimedEntity = iLrte;
    iLrteListItem.iMediaState = ESvgMediaIdle;
    iLrteListItem.iUserState =ESvgUserRunning ;
    iLrteListItem.iIsSyncPaused = EFalse;
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSvgTimeContainer* CSvgTimeContainer::NewL( CSvgDocumentImpl* aOwnerDoc,
    const TBool aHasParent, 
    const TSvgSyncBehaviour aSyncBehavior,
    const TBool aIsSyncMaster,
    const TUint32 aSyncTolerance )
    {
    CSvgTimeContainer* self = new( ELeave ) CSvgTimeContainer( aOwnerDoc,
        aHasParent, aSyncBehavior, aIsSyncMaster, aSyncTolerance );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CSvgTimeContainer::~CSvgTimeContainer()
    {
    // Reset the observer
    iObserver = NULL;
    // Delete the locked realtime timed entity 
    delete iLrte;
    // Cleanup the list of timed entities
    CleanupTimedEntities();
    
    // Delete the timer if present
    if ( iTimer )
        {
        delete iTimer;
        iTimer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::GetSyncMasterForTimeContainer
// Resolves the sync master within the time container and returns the same.
// -----------------------------------------------------------------------------
//
MSvgTimedEntityInterface* CSvgTimeContainer::GetSyncMasterForTimeContainer()
    {
    // First check the number of elements in the time container
  TInt lTimedEntityListCnt = iTcTimedEntities.Count();

    // if there is only one element in the list(which is the document),
    // return NULL to avoid list traversal
    if ( lTimedEntityListCnt <= 1 )
        {
        return NULL;
        }
        
    TInt lFirstAudioIndex = iTcTimedEntities.Count();    // Store invalid value
    MSvgTimedEntityInterface* lCurSyncMaster = NULL;    

  // Within the locked group of timed entities within the time 
  // container, need to find out which element is acting as master. 

  // For All Timed Entities
  // Get the first eligible sync master
  for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
      {
    //get each element
    MSvgTimedEntityInterface* lCurTimedEntity = 
        (iTcTimedEntities[ lItemIndex ]).iTimedEntity ;
    
    // In case an audio element is found
    //      check whether is is locked and can generate tick
    //      If so, it is an eligible sync master
    if( lFirstAudioIndex == lTimedEntityListCnt && 
        lCurTimedEntity->ObjectType() == ESvgAudioElement && 
        lCurTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked &&
        lCurTimedEntity->CanGenerateTick() )
        {
        // Remember this so it can be used later if 
        // no eligible syncmaster found
        lFirstAudioIndex = lItemIndex;
        }

      TBool lIsSyncMaster = EFalse;
      lCurTimedEntity->GetCnfSyncMasterStatus( lIsSyncMaster );
      // An element is an eligible sync master if:
      //      a. It is locked wrt the TC
      //      b. It is configured as a sync master
      //      c. It can generate tick i.e. it is active and has implicit duration.
        if ( lCurTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked 
            && lIsSyncMaster 
            && lCurTimedEntity->CanGenerateTick() )
            {
            lCurSyncMaster = lCurTimedEntity;
            break;
            }
    }
    

    if ( !lCurSyncMaster ) 
    {
      if( lFirstAudioIndex != lTimedEntityListCnt ) 
        {
        // Did not find a sync master, and audio element available
        lCurSyncMaster = iTcTimedEntities[ lFirstAudioIndex ].iTimedEntity;
        }   
        //Locked real time entityas syncmaster
      else
        {
            lCurSyncMaster = iLrteListItem.iTimedEntity;
        }  
        
    } 
      
    if ( lCurSyncMaster )
        {
        // Sync Master found
        // Store the syncMaster in iTcSyncMaster.
        iTcSyncMaster = lCurSyncMaster;

        // Set the state in the sync master that it is the chosen one.
        iTcSyncMaster->SetCurSyncMasterStatus( ETrue );

        // For All Timed Entities other than syncMaster in TC group
        //          SetCurSyncMasterStatus = False;
        for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
            {
            MSvgTimedEntityInterface* lCurTimedEntity = 
                (iTcTimedEntities[ lItemIndex ]).iTimedEntity ;
            if ( lCurTimedEntity != iTcSyncMaster )
                {
                lCurTimedEntity->SetCurSyncMasterStatus( EFalse );            
                }
            }
            
            if(iLrteListItem.iTimedEntity != lCurSyncMaster)
            {
                iLrteListItem.iTimedEntity->SetCurSyncMasterStatus(EFalse);
            }    
        }
    

    return iTcSyncMaster;
    }
        
// -----------------------------------------------------------------------------
// CSvgTimeContainer::AddTimedEntityL
// Add a timed entity to the time container.
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::AddTimedEntityL( 
    MSvgTimedEntityInterface* aTimedEntity )
    {
    if ( !aTimedEntity )
        {
        User::Leave( KErrArgument );
        }
    
    // check to see if entity is already in list and simply return
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
        if ( (iTcTimedEntities[ lItemIndex ]).iTimedEntity == aTimedEntity )
            {
            return;
            }
        }
   
    TSvgTimedEntityListItem lListItem;
    lListItem.iTimedEntity = aTimedEntity;

    //Media State initially Idle
    lListItem.iMediaState = ESvgMediaIdle;
    
    //User state initially is running
    lListItem.iUserState =ESvgUserRunning ;
    
    // Sync State is Playing
    lListItem.iIsSyncPaused = EFalse;

    // Append to Timed Entity List
    iTcTimedEntities.AppendL( lListItem );
    
    // Obtain the post order list of media elements 
     RPointerArray<CSvgElementImpl> lMediaEleList;
    lMediaEleList.Reset(); 
    
    CSvgElementImpl *handle = (CSvgElementImpl *)(iOwnerDoc->RootElement());
    iOwnerDoc->ParsePostOrderMediaElements( handle, lMediaEleList );
    
    // Rearrange as per Post order
    RearrangeTimedEntityList( lMediaEleList );
    
    // Done with the list
    lMediaEleList.Close();  
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::DelTimedEntity
// Removes the timed entity from the time container.
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::DelTimedEntity( 
    MSvgTimedEntityInterface* aTimedEntity )
    {
    // check whether the entity exists and then remove it from the list
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
        if ( (iTcTimedEntities[ lItemIndex ]).iTimedEntity == aTimedEntity )
            {
            iTcTimedEntities.Remove( lItemIndex );
      break;
            }
        }
    
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::CleanupTimedEntities
// Removes all the timed entities from the time container.
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::CleanupTimedEntities( )
    {
    // Delete the entries in the timed entity list; does not delete the 
    // timed entities.
    iTcTimedEntities.Reset();
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::SetCurrentTime
// Set Function for the TC's current time
// ---------------------------------------------------------------------------
void CSvgTimeContainer::SetCurrentTime( TInt32 aCurTime )
    {
    iCurrentTime = aCurTime;
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::CurrentTime
// Accessor for the TC's current time
// ---------------------------------------------------------------------------
TInt32 CSvgTimeContainer::CurrentTime()
    {
    return iCurrentTime;
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::SetSyncBehavior
// Set the Synchronised behaviour for the time container
// ---------------------------------------------------------------------------
void CSvgTimeContainer::SetSyncBehavior( const TSvgSyncBehaviour aValue )
    {
    iTcSyncBehavior = aValue;
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::SetSyncTolerance
// Set the Synchronised Tolerance for the time container
// ---------------------------------------------------------------------------
void CSvgTimeContainer::SetSyncTolerance( const TUint32 aValue )
    {
    iTcSyncTolerance = aValue;    
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::SetSyncMaster
// Set the time container as Synchronised Master
// ---------------------------------------------------------------------------
void CSvgTimeContainer::SetSyncMaster( const TBool aValue )
    {
    iTcSyncMasterConfig = aValue;
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::GetCurTick
// Accessor function to get the current tick
// ---------------------------------------------------------------------------
TSvgTick CSvgTimeContainer::GetCurTick()
    {
    return iCurrentTick;
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::Document
// Accessor function to get the owner doc of the TC
// ---------------------------------------------------------------------------
CSvgDocumentImpl* CSvgTimeContainer::Document()
    {
    return iOwnerDoc;
    }


// ---------------------------------------------------------------------------
// CSvgTimeContainer::TimedEntityNotReady
// Informing the time container that timed entity is not ready
  /*
    If this element is locked
    {
      PauseLockedElements
        {
          (Whether ready/not ready)
                 For all locked elements:
          SyncPauseElement.
        }
        If TC Observer present
          Give Not Ready Event 
    }

    */
 // ---------------------------------------------------------------------------
void CSvgTimeContainer::TimedEntityNotReady( 
    MSvgTimedEntityInterface* aTimedEntity )
    {
    
    
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
      //get current element
        if ( (iTcTimedEntities[ lItemIndex ]).iTimedEntity == aTimedEntity )
            {
          //set current element as not ready
          (iTcTimedEntities[ lItemIndex ]).iMediaState = ESvgMediaNotReady;
          
          //if current element is locked
          if((iTcTimedEntities[ lItemIndex ]).iTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked )
              {
              //Indicate the timecontainer is not ready
            SetMediaState(ESvgMediaNotReady);
            
            PauseLockedEntities();
            
            // Inform the observer that the time container is not ready
          if ( iObserver )
                {
                    iObserver->TimeContainerNotReady();
                }
              }
          //Once we have got the right timed entity on which not ready was called
      //exit the loop
      break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::SyncpauseElement
//Pauses running syncronised elements
  /*
      Pauses a single element
    
    If ( syncStatus is not paused AND ( userStatus is not paused or not stopped ) )
      Pause Element
      Set syncStatus = Paused // iIsSyncPaused 
      
      
      If Element type = Animation
          {
              Get Child TC Pointer.
              If Valid,
                Call Child Tc->SyncPause
          }

  
  */
  // -----------------------------------------------------------------------------
//


void CSvgTimeContainer::SyncPauseElement(TSvgTimedEntityListItem* aTimedEntityItem)
    {
      if (
         (!aTimedEntityItem->iIsSyncPaused)  
      && (aTimedEntityItem->iUserState == ESvgUserRunning)
         )
        {
        aTimedEntityItem->iTimedEntity->PauseTimedEntity();
        }
      aTimedEntityItem->iIsSyncPaused = ETrue;
      
      if(aTimedEntityItem->iTimedEntity->ObjectType() == ESvgMediaAnimationElement)  
        {
        CSvgTimeContainer* lAnimTimeContainer = aTimedEntityItem->iTimedEntity->GetChildTimeContainer();
        if(lAnimTimeContainer)
            {
            lAnimTimeContainer->SyncPause();
            }
        }
    }
    

// ---------------------------------------------------------------------------
// CSvgTimeContainer::TimedEntityReady
// Informing the time container that timed entity is ready
    /*
    If this element is locked
    {
     If all locked elements ready
    {
      ResumeLockedElements
       {
            For all locked elements:
                 SyncResumeElement.

        }
      If TC Observer present
        Give ready of TC
    }
     }
    */
// ---------------------------------------------------------------------------
void CSvgTimeContainer::TimedEntityReady( 
    MSvgTimedEntityInterface* aTimedEntity )
    {

    TBool lIsOkToResume = ETrue;

    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
  for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
      {
    //get current element from entities list
      if ( (iTcTimedEntities[ lItemIndex ]).iTimedEntity == aTimedEntity )
        {
        //set current element as not ready
        (iTcTimedEntities[ lItemIndex ]).iMediaState = ESvgMediaReady;
          
        //if current element is locked
        if((iTcTimedEntities[ lItemIndex ]).iTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked )
            {
        //for all timed entities
            for ( lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
              {
              // check if any timed entities are locked and not ready
              // Idle elements cannot be checked as they are not active
              if(
              ( (iTcTimedEntities[ lItemIndex ]).iTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked )
              && ((iTcTimedEntities[lItemIndex].iMediaState == ESvgMediaNotReady))
              )
                {
                //Indicate cant resume now as some locked entities are not ready
                lIsOkToResume = EFalse;
                //exit loop
                break;
                }
              }
            }
            //Once we have got the right timed entity on which ready was called
            //exit the loop
            break;
          }
      }
    
      if ( lIsOkToResume )
        {
        //Indicate the timecontainer is ready as all elements are ready
      SetMediaState(ESvgMediaReady);
      
      ResumeLockedEntities();

      // Inform the observer that the time container is ready
         if ( iObserver )
          {
          iObserver->TimeContainerReady();
          }
        }
    }
    
//-------------------------------------------------------------------------------
//void CSvgTimeContainer::SyncResumeElement
  /*
    Resumes a single element
  
  if ( syncStatus is not running AND (userStatus is running))
  AND (TC.syncStatus is running) AND (TC.userStatus is running))
    {

    Resume Element
  }
    Set syncStatus = running 
    If Element type = Animation
  {
          Get Child TC Pointer.
          If Valid,
    Call Child Tc->SyncResume
  }

	*/
//-------------------------------------------------------------------------------
void CSvgTimeContainer::SyncResumeElement(TSvgTimedEntityListItem* aTimedEntity)
    {
    if(
     (aTimedEntity->iIsSyncPaused) 
    && (aTimedEntity->iUserState == ESvgUserRunning)
    && (!iIsSyncPaused)
    && (iUserState == ESvgUserRunning )
    )
        {
        aTimedEntity->iTimedEntity->ResumeTimedEntity();
        }
    aTimedEntity->iIsSyncPaused = EFalse;
      
    if(aTimedEntity->iTimedEntity->ObjectType() == ESvgMediaAnimationElement)  
        {
        CSvgTimeContainer* AnimTimeContainer = aTimedEntity->iTimedEntity->GetChildTimeContainer();
        if(AnimTimeContainer)
            {
            AnimTimeContainer->SyncResume();
            }
        }
    }   
    
    
// -----------------------------------------------------------------------------
// CSvgTimeContainer::PauseLockedEntities
// Pauses the time container's locked entities
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::PauseLockedEntities()    
    {
     /*
     (Whether ready/not ready)
            For all locked elements:
            SyncPauseElement.
     */
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
        if((iTcTimedEntities[ lItemIndex ]).iTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked ) 
            {
            SyncPauseElement(&iTcTimedEntities[ lItemIndex ]);
            }
        }
    SyncPauseElement(&iLrteListItem);
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::ResumeLockedEntities
// Resumes the time container's locked entities
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::ResumeLockedEntities()    
    {
  /*
    For all locked elements:
               SyncResumeElement.
  */
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
        if((iTcTimedEntities[ lItemIndex ]).iTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked ) 
            {
            SyncResumeElement(&iTcTimedEntities[ lItemIndex ]);
            }
        }
    SyncResumeElement(&iLrteListItem) ;
     
          
    }      


    
// -----------------------------------------------------------------------------
// CSvgTimeContainer::SetTcObserver
// Set the observer for the time container
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::SetTcObserver( MSvgTimeContainerObserver* aTCObserver )
    {
    iObserver = aTCObserver;
  // Inform the observer that the time container is ready
     if ( iObserver )
        {
        if ( iMediaState == ESvgMediaReady)
            {
            iObserver->TimeContainerReady();
            }
        else
            {
            iObserver->TimeContainerNotReady();
            }
        }    
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::UserStop
// Called to handle user/client generated stop event
/*
      UserStop()
      {
      
                  Set the TC.UserState too 
                  This.UserState = Paused.

        
        For all timed entities
    {
        UserStopElement()
      {
        StopElement()
        Set userStatus = Stopped
        If Element type = Animation       
                {
                Get Child TC Pointer.
                If Valid,
              Call Child Tc->UserStop
            }

      }
        }
       }
    */
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::UserStop()
    {
    SetUserState(ESvgUserStopped);
    // Cancel the timer
    if ( iTimer )
        {
        iTimer->SVGStop();
        }    
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
        UserStopElement(&iTcTimedEntities[lItemIndex]);
        }
    UserStopElement(&iLrteListItem);
    
    }


void CSvgTimeContainer::UserStopElement(TSvgTimedEntityListItem* aTimedEntity)
{
  aTimedEntity->iTimedEntity->StopTimedEntity();
  
  aTimedEntity->iIsSyncPaused = EFalse;
  
  SetUserState(ESvgUserStopped);
  
  if(aTimedEntity->iTimedEntity->ObjectType() == ESvgMediaAnimationElement)  
    {
          CSvgTimeContainer* AnimTimeContainer = aTimedEntity->iTimedEntity->GetChildTimeContainer();
          if(AnimTimeContainer)
          {
              AnimTimeContainer->UserStop();
          }
    }
}

// -----------------------------------------------------------------------------
// CSvgTimeContainer::UserResume
// Called to handle user/client generated resume event
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::UserResume( TInt32 aTime )
    {
        
    SetUserState(ESvgUserRunning);
    if ( iTimer )
        {
        iTimer->SVGResume( aTime );
        }
        
    
    
    /*
    UserResume()
    {
      For all timed entities
    {
      UserResumeElement()
         }
    }
  */
  
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
      {
      UserResumeElement(&iTcTimedEntities[ lItemIndex ]);
      }
      
      
    UserResumeElement(&iLrteListItem);

    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::UserResumeElement
/*
UserResumeElement()
       {
          If(Element is locked)
              {
                If ( syncStatus is running AND TC.syncStatus is running)
                    Resume Element
              }
              Else
              {
                ResumeElement()
              }
  
              Set userStatus = running
                If Element type = Animation
            {
                Get Child TC Pointer.
                If Valid,
                  Call Child Tc->UserResume
            }

      }
*/
// -----------------------------------------------------------------------------
//

void CSvgTimeContainer::UserResumeElement(TSvgTimedEntityListItem* aTimedEntity)
  {
  aTimedEntity->iUserState = ESvgUserRunning ;
      
  if(//Element is locked
    (aTimedEntity->iTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked)
    &&((aTimedEntity->iIsSyncPaused) || (iIsSyncPaused))
     //element or timecontainer is syncpaused
  )
    {
    //Do not resume
    }
  else
    {
    aTimedEntity->iTimedEntity->ResumeTimedEntity();
    }

  
  if(aTimedEntity->iTimedEntity->ObjectType() == ESvgMediaAnimationElement)  
    {
    CSvgTimeContainer* AnimTimeContainer = aTimedEntity->iTimedEntity->GetChildTimeContainer();
    if(AnimTimeContainer)
        {
        AnimTimeContainer->UserResume();
        }
    }
  }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::UserPlay
// Called to handle user/client generated play event
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::UserPlay()
    {

    
    if ( !iTimer )
        {
        // Check whether this time container has a parent
        // then we do not have to create the timer
        if ( !iIsParentTcPresent )
            {
            // Obtain the post order list of media elements 
             RPointerArray<CSvgElementImpl> lMediaEleList;
            lMediaEleList.Reset(); 
            
            CSvgElementImpl *handle = (CSvgElementImpl *)(iOwnerDoc->RootElement());
            
            //Parse postorderr only if there are media elements
            if(iTcTimedEntities.Count() > 1 )
            {
              iOwnerDoc->ParsePostOrderMediaElements( handle, lMediaEleList );    
            }
            
            
            // Rearrange as per Post order
            RearrangeTimedEntityList( lMediaEleList );
            
            // Done with the list
            lMediaEleList.Close();  
            
            TRAPD( lTimerCreateErr, iTimer = CSvgTimer::NewL( this ) );
            
            if ( lTimerCreateErr != KErrNone )
                {
                // Error Processing
                return;
                }
            
            iTimer->SetStartTime();
            
            // the current home time needs to be updated.
            if ( iOwnerDoc && iCurrentTime != 0 )
                {
                iCurrentTime = 0;
                iCurrentTick.iParentTcTick = 0;
                iCurrentTick.iRealTimeTick = 0;
                TSvgTimerEvent timeEvent( iCurrentTime );
                iOwnerDoc->Reset( &timeEvent );
                }            

            if ( !iTimer->IsActive() )
                {
                iTimer->After( 1000 );
                }           
            }
        }
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::UserPause

 /*
 
                   Set the TC.UserState too 
                  This.UserState = Paused.

    UserPause()
    {
      For all timed entities
    {
         UserPauseElement()     iIsSyncPaused is syncStatus 
      
    }
    }
    */
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::UserPause()
{
  SetUserState(ESvgUserPaused);
  if ( iTimer )
    {
  	iTimer->SVGStop();
    }
 
  TInt lTimedEntityListCnt = iTcTimedEntities.Count();
  for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
    {
    UserPauseElement(&iTcTimedEntities[ lItemIndex ]);
    }
    
    UserPauseElement(&iLrteListItem);

}
    
// -----------------------------------------------------------------------------
// CSvgTimeContainer::UserPauseElement
// Called to handle user/client generated pause event
// 
/*
    UserPauseElement()     iIsSyncPaused is syncStatus 
      {
          If ( syncStatus is not Paused AND userStatus is not paused or stopped )
          Pause Element
           Set userStatus = Paused
           
               If Element type = Animation
                {
                    Get Child TC Pointer.
                    If Valid,
                      Call Child Tc->UserPause
                }

      }
*/

//-----------------------------------------------------------------------------
//
   
 void CSvgTimeContainer::UserPauseElement(TSvgTimedEntityListItem* aTimedEntity)
{
  if(
      !(aTimedEntity->iIsSyncPaused )
    &&  (aTimedEntity->iUserState == ESvgUserRunning)
      )
    {
      aTimedEntity->iTimedEntity->PauseTimedEntity();
          
    } 
    aTimedEntity->iUserState = ESvgUserPaused;
    
    if(aTimedEntity->iTimedEntity->ObjectType() == ESvgMediaAnimationElement)  
    {
        CSvgTimeContainer* AnimTimeContainer = aTimedEntity->iTimedEntity->GetChildTimeContainer();
        if(AnimTimeContainer)
        {
            AnimTimeContainer->UserPause();
      }
    }
    
}

// -----------------------------------------------------------------------------
// CSvgTimeContainer::UserSeek
// Called to handle user/client generated seek event
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::UserSeek( 
    TInt aTime ) // Time in msecs to seek
    {
    if ( iTimer )
        {
        //if ( iOwnerDoc && iCurrentTime != aTime )
        if ( iOwnerDoc)
            {
            iCurrentTime = aTime;
            iCurrentTick.iParentTcTick = aTime;
            iCurrentTick.iRealTimeTick = aTime;
            TSvgTimerEvent timeEvent( iCurrentTime );
            iOwnerDoc->Reset( &timeEvent );
            }            

        iTimer->SetSeekTime( aTime );
        }
    // Need to send user resume events to timed entities
    SetUserState(ESvgUserRunning);
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
    	{
		UserResumeElement( &iTcTimedEntities[ lItemIndex ] );
    	}
    
    UserResumeElement(&iLrteListItem);
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::UserResetTime
// Resets the time variables
// ---------------------------------------------------------------------------
void CSvgTimeContainer::UserResetTime()
    {
    if ( iTimer )
        {
        iTimer->ResetTime();
        }
    }

// ---------------------------------------------------------------------------
// CSvgTimeContainer::UserChangeFrameDuration
// Changes the frame duration in the timer
// ---------------------------------------------------------------------------
void CSvgTimeContainer::UserChangeFrameDuration( TUint32 aTimerDuration )
    {
    if ( iTimer )
        {
        iTimer->ChangeFrameDuration( aTimerDuration );
        }    
    }
// ---------------------------------------------------------------------------
// CSvgTimeContainer::UserFps
// Returns number of frames per sec(debug api)
// ---------------------------------------------------------------------------
TUint CSvgTimeContainer::UserFps()
    {
    if ( iTimer )
        {
        return iTimer->Fps();
        }
    return 0;        
    }
    
// -----------------------------------------------------------------------------
// CSvgTimeContainer::ParentTimeContainerTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//
void CSvgTimeContainer::ParentTimeContainerTick( 
    TSvgTick aTick ) // Current tick information 
    {
    // Update the member variable keeping track of current tick
    iCurrentTick.iRealTimeTick = aTick.iRealTimeTick;
    
    // If the TC is locked wrt to its parent and parent is present
    if ( ( iTcSyncBehavior == ESvgSyncLocked ) && iIsParentTcPresent )
        {
        iCurrentTick.iParentTcTick = aTick.iParentTcTick;
        // Propogate the tick to all timed elements in the container
        PropogateTickToContainerElements( iCurrentTick );
        }
    else
        {
        // Container not locked with respect to parent.
        // Get Sync Master Entity in the group and use its clock as timing
        // reference for the Time container. 
        MSvgTimedEntityInterface* lSyncMaster = GetSyncMasterForTimeContainer();

        if ( !lSyncMaster )
            {
            // No Sync Master
            iCurrentTick.iParentTcTick = aTick.iRealTimeTick;
            // Propogate the tick to all timed elements in the container
            PropogateTickToContainerElements( iCurrentTick );
            }
        else
            {
            // Sync Master Present, check whether it can generate tick
            if ( lSyncMaster->CanGenerateTick() )
                {
                TUint32 lMasterCurTime = 0;
                lSyncMaster->GetEntityCurrentTime( lMasterCurTime );
                iCurrentTick.iParentTcTick = lMasterCurTime;
                // Propogate the tick to all timed elements in the container
                PropogateTickToContainerElements( iCurrentTick );
                }
            else
                {
                // Sync Master present but cannot generate tick, so use
                // real time tick
                iCurrentTick.iParentTcTick = aTick.iRealTimeTick;
                // Propogate the tick to all timed elements in the container
                PropogateTickToContainerElements( iCurrentTick );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::GetEntitySyncBehavior
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
TSvgSyncBehaviour CSvgTimeContainer::GetEntitySyncBehavior()
    {
    return iTcSyncBehavior; 
    }
        

// -----------------------------------------------------------------------------
// CSvgTimeContainer::GetEntityCurrentTime
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
void CSvgTimeContainer::GetEntityCurrentTime( TUint32& 
            aEntityCurTime ) // Current Entity Time in msecs. 
    {
    // IF GetCurSyncMasterStatus == FALSE, Error. // Shouldn't be happening.
    TBool lCurSyncMasterStatus = EFalse;
    GetCurSyncMasterStatus( lCurSyncMasterStatus );

    if ( lCurSyncMasterStatus )
        {
        // Resolve the sync Master for the container.  (Should have happened?)
        MSvgTimedEntityInterface* lSyncMaster = GetSyncMasterForTimeContainer();
        // IF no syncMaster, Error (Shouldn't be happening.)
        if ( !lSyncMaster )
            {
            return;
            }
        lSyncMaster->GetEntityCurrentTime( aEntityCurTime );
        }
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::GetCnfSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
void CSvgTimeContainer::GetCnfSyncMasterStatus( 
            TBool& aIsSyncMaster ) // Indicates whether the element is configured as 
                                   // Sync Master. 
    {
    iTcSyncMasterConfig = aIsSyncMaster;
    }                                   

// -----------------------------------------------------------------------------
// CSvgTimeContainer::GetCurSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgTimeContainer::GetCurSyncMasterStatus( 
            TBool& aIsSyncMaster ) // Indicates whether the element is currrently 
                                  // Sync Master. 
    {
    aIsSyncMaster = iTcSyncMasterCurrent;
    }        


// -----------------------------------------------------------------------------
// CSvgTimeContainer::SetCurSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgTimeContainer::SetCurSyncMasterStatus( 
    TBool aSyncMasterStatus ) //  Indicates whether the element is currrently 
                              // Sync Master.
    {
    iTcSyncMasterCurrent = aSyncMasterStatus;
    // Update the state of current sync master if there is one
    if ( iTcSyncMaster )
        {
        iTcSyncMaster->SetCurSyncMasterStatus( aSyncMasterStatus );
        }
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::CanGenerateTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
TBool CSvgTimeContainer::CanGenerateTick()
    {
    MSvgTimedEntityInterface* lSyncMaster = GetSyncMasterForTimeContainer();
    if(lSyncMaster == iLrteListItem.iTimedEntity)
        {
        return EFalse;
        }
    return ( lSyncMaster ? ETrue:EFalse );
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::CanUseParentTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
TBool CSvgTimeContainer::CanUseParentTick()
    {
    // This function returns true if it does not have a parent time container
    // Hence the root time container creates the timer and hence can use
    // parent tick
    return ( !iIsParentTcPresent );
    }
    
// -----------------------------------------------------------------------------
// CSvgTimeContainer::ResyncTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgTimeContainer::ResyncTimedEntity( 
            TUint32 /*aSynctime */) // Time for resync in msecs.
    {
    // TODO: Resync Logic
    }
    

// -----------------------------------------------------------------------------
// CSvgTimeContainer::PauseTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgTimeContainer::PauseTimedEntity()
    {

    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::ResumeTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgTimeContainer::ResumeTimedEntity()
    {
 
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::StopTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgTimeContainer::StopTimedEntity()
    {

    }


//From MSvgTimedEntityInterface
TSvgObjectType CSvgTimeContainer::ObjectType()
{
  return ESvgInvalidReference;  
}

//returns the child time container of the element 
//used in timecontainer
CSvgTimeContainer* CSvgTimeContainer::GetChildTimeContainer()
{
    return NULL;
}

//-------------------------------------------------------------------------------
/*
TC::SyncPause()
{
     tc.SyncStatus = Paused;

     For All Locked Elements
  {
      Pause Element; 
       Note: No element level state change, as element level syncStatus reflects only
       syncStatus evaluated in TC group level. ,e.g. syncStatus could be running in this
                             case 
        If element = animation 
    {
        Get Child TC pointer
        If Child TC
            childTc->SyncPause;
    }

  }
}


*/
//-------------------------------------------------------------------------------
void CSvgTimeContainer::SyncPause()
    {
    //iIsSyncPaused = ETrue;
    SetSyncState(ETrue);
    
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
        if((iTcTimedEntities[ lItemIndex ]).iTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked )
            {
            iTcTimedEntities[lItemIndex].iTimedEntity->PauseTimedEntity();
            }
        
        if(iTcTimedEntities[lItemIndex].iTimedEntity->ObjectType() == ESvgMediaAnimationElement)  
            {
            CSvgTimeContainer* AnimTimeContainer = iTcTimedEntities[lItemIndex].iTimedEntity->GetChildTimeContainer();
            if(AnimTimeContainer)
                {
                AnimTimeContainer->SyncPause();
                }
            }    
        }
    iLrteListItem.iTimedEntity->PauseTimedEntity()  ;
    
    }

//----------------------------------------------------------------------------------
//
/*
TC::SyncResume()
{
     tc.SyncStatus = Running;
     For all locked elements
     {
        If (syncStatus == running AND userStatus == running AND TC.userStatus == running)
        {
          Resume Element
      }
  
        If element = animation 
      {
        Get Child TC pointer
        If Child TC
            childTc->SyncResume;
      }
     }
}


*/
//----------------------------------------------------------------------------------
void CSvgTimeContainer::SyncResume()
    {
    SetSyncState(EFalse);
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
        if((iTcTimedEntities[ lItemIndex ]).iTimedEntity->GetEntitySyncBehavior() == ESvgSyncLocked )
            {
            if(
            !(iTcTimedEntities[lItemIndex].iIsSyncPaused)
            &&(iTcTimedEntities[lItemIndex].iUserState == ESvgUserRunning)
            &&(iUserState == ESvgUserRunning)
            )
                {
                iTcTimedEntities[lItemIndex].iTimedEntity->ResumeTimedEntity();    
                }
            
            }
        
        if(iTcTimedEntities[lItemIndex].iTimedEntity->ObjectType() == ESvgMediaAnimationElement)  
            {
            CSvgTimeContainer* AnimTimeContainer = iTcTimedEntities[lItemIndex].iTimedEntity->GetChildTimeContainer();
            if(AnimTimeContainer)
                {
                AnimTimeContainer->SyncResume();
                }
            }    
        }
        
        iLrteListItem.iTimedEntity->ResumeTimedEntity();
    
}
//---------------------------------------------------------------
//Sets the time container User state
//void CSvgTimeContainer::SetUserState(TSvgEntityUserState state)
//---------------------------------------------------------------
void CSvgTimeContainer::SetUserState(TSvgEntityUserState state)
{
    iUserState = state;
  /*  #ifdef _DEBUG
    RDebug::Print(_L("CSvgTimeContainer User State changed to %d"),state);
    RDebug::Print(_L("\n0.ESvgUserRunning 1.ESvgUserPaused 2.ESvgUserStopped\n\n"));
    #endif
  */  
}
 

//---------------------------------------------------------------
//Sets the time container Media state
//void CSvgTimeContainer::SetMediaState(TSvgEntityMediaState state)
//---------------------------------------------------------------
void CSvgTimeContainer::SetMediaState(TSvgEntityMediaState state)
{
    iMediaState = state;
    /*
    #ifdef _DEBUG
    RDebug::Print(_L("CSvgTimeContainer Media State changed to %d"),state);
    RDebug::Print(_L("\n0.ESvgMediaIdle 1.ESvgMediaNotReady 2.ESvgMediaReady\n\n"));
    #endif
    */
}
        
//---------------------------------------------------------------
//Sets the time container sync state state
//void CSvgTimeContainer::SetSyncState(TBool state)
//---------------------------------------------------------------
void CSvgTimeContainer::SetSyncState(TBool state)
{
    iIsSyncPaused = state;
/*    #ifdef _DEBUG
    RDebug::Print(_L("CSvgTimeContainer iIsyncPaused  changed to %d"),state);
    RDebug::Print(_L("\n0 False 1 True\n\n"));
    #endif
 */   
}
        
// -----------------------------------------------------------------------------
// CSvgTimeContainer::PropogateTickToContainerElements
// Propogate tick to all elements in time container
// -----------------------------------------------------------------------------
void CSvgTimeContainer::PropogateTickToContainerElements( TSvgTick& aTick )
    {
    // Propogate the tick to all container elements
    TInt lTimedEntityListCnt = iTcTimedEntities.Count();
    for ( TInt lItemIndex = lTimedEntityListCnt - 1; lItemIndex >= 0 ; lItemIndex-- )
        {
        (iTcTimedEntities[ lItemIndex ]).iTimedEntity->ParentTimeContainerTick( aTick );
        }
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::RearrangeTimedEntityList
// Rearrange the timed entities as per post order
// -----------------------------------------------------------------------------
void CSvgTimeContainer::RearrangeTimedEntityList( RPointerArray<CSvgElementImpl>& 
            aPostOrderList )
    {
    TUint lCurElement = 0;
    TUint lCurTimedEntity = 0;
    for ( ;lCurElement < aPostOrderList.Count(); lCurElement++, lCurTimedEntity++ )
        {
        CSvgMediaElementBase* lCurMediaEle = ( CSvgMediaElementBase* )
            aPostOrderList[ lCurElement ];
        
        // Start looking in the timed entity list from lCurTimedEntity
        TUint lListIndex = lCurTimedEntity; 
        
        // Find the element in the current timed entity list
        /*for ( ; ( ( iTcTimedEntities[ lListIndex ].iTimedEntity != 
                ( MSvgTimedEntityInterface* ) lCurMediaEle ) && 
                    ( lListIndex < iTcTimedEntities.Count() ) ); 
            lListIndex++ )
            {
            }
        */
        do
				{
					if( iTcTimedEntities[ lListIndex ].iTimedEntity == ( MSvgTimedEntityInterface* ) lCurMediaEle ) 
						break;
					lListIndex++; 
				}while(lListIndex < iTcTimedEntities.Count() ) ;

        
        if ( lListIndex != iTcTimedEntities.Count() )    
            {
            if ( lListIndex != lCurTimedEntity )
                {
                // Not in same position, need to swap 
                TSvgTimedEntityListItem lTempItem = iTcTimedEntities[ lCurTimedEntity ];
                iTcTimedEntities[ lCurTimedEntity ] = iTcTimedEntities[ lListIndex ];
                iTcTimedEntities[ lListIndex ] = lTempItem;
                }
            }
        else
            {
            // Can occur in case of Delete Timed Entity -> ignore
            }
        }
    }

// -----------------------------------------------------------------------------
// CSvgTimeContainer::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
// ========================== OTHER EXPORTED FUNCTIONS =========================



//  End of File  
