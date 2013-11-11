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
#include "SVGLockedRealTimeEntity.h"

CSvgLockedRealTimeEntity* CSvgLockedRealTimeEntity::NewL(CSvgDocumentImpl* aDoc)
    {
    CSvgLockedRealTimeEntity* self = new (ELeave) CSvgLockedRealTimeEntity(aDoc);
    CleanupStack::PushL(self);
    self->ConstructL(aDoc);
    CleanupStack::Pop(self);
    return self;
    }
//==========================================================================
void  CSvgLockedRealTimeEntity::ConstructL(CSvgDocumentImpl* aDoc)
    {
        iOwnerDocument = aDoc;
    }
//==========================================================================
CSvgLockedRealTimeEntity::CSvgLockedRealTimeEntity(CSvgDocumentImpl* aDoc): CSvgMediaElementBase( aDoc ),
                            iIsPaused(EFalse),
                            iSyncMasterCurrent(EFalse)
    {
    //SyncBehaviour is locked
	//MediaState is ready always

    }
//==========================================================================
CSvgLockedRealTimeEntity::~CSvgLockedRealTimeEntity()
    {
    
    }

//==========================================================================
//from svgelementimpl
void CSvgLockedRealTimeEntity::Print(TInt /*aValue*/)
    {
    
    }

//==========================================================================
/**
* The parent time container provides the timing clock to 
* the timed entity (audio, video, animation, document) 
* using this routine.
* @since S60 3.2
* @param aTick Current tick information 
* @return none.
*/
void CSvgLockedRealTimeEntity::ParentTimeContainerTick( TSvgTick /*aTick*/ )
{
    
}

//==========================================================================
/**
* Returns the sync behavior of the entity.
* @since S60 3.2
* @param none. 
* @return TSvgSyncBehaviour Element's Sync Behaviour.
*/
TSvgSyncBehaviour CSvgLockedRealTimeEntity::GetEntitySyncBehavior()
{
    return ESvgSyncLocked;
}
        
//==========================================================================
/**
* When the timed entity acts as timing master in the time container, 
* the time container gets the timed entity clock using this method 
* and feeds to rest of timed entities.
* @since S60 3.2
* @param aEntityCurTime Current Entity Time in msecs. 
* @return none.
*/
void  CSvgLockedRealTimeEntity::GetEntityCurrentTime( TUint32& aEntityCurTime )
    {
    //Keep returning the same time if the entity is paused
    if(iIsPaused)
        {
            aEntityCurTime = iStoredTick.iParentTcTick;
        }
    //add the relative time difference in realtimetick to parenttick
    else
        {
            TSvgTick curTick = iOwnerDocument->TimeContainer()->GetCurTick();
            TInt32 delta = curTick.iRealTimeTick - iStoredTick.iRealTimeTick;
            
            if(delta < 0)
                delta = 0;
            
            aEntityCurTime = iStoredTick.iParentTcTick + delta;
            
        }
    }
//==========================================================================
/**
* Returns the configured sync master value(as per DOM tree) as specified 
* in the SVG content.
* @since S60 3.2
* @param aIsSyncMaster Indicates whether the element is configured as 
*                      Sync Master. 
* Locked Real Time Entity can never be configured as syncmaster hence
* This implementation is NULL
* @return none.
*/
void CSvgLockedRealTimeEntity::GetCnfSyncMasterStatus( TBool& /*aIsSyncMaster*/ )
{
    
}

//==========================================================================
/**
* Check if timed entity is going to act as timing master in the 
* time container. This behavior could change dynamically.
* @since S60 3.2
* @param aIsSyncMaster Indicates whether the element is currrently Sync Master. 
* @return none.
*/
void CSvgLockedRealTimeEntity::GetCurSyncMasterStatus( TBool& isSyncMaster )
{
        isSyncMaster = iSyncMasterCurrent;
}

//==========================================================================
/** If some other element is resolved as syncMaster in the time container group, 
* this element can not act as sync master. 
* @since S60 3.2
* @param aSyncMasterStatus Indicates whether the element is currrently 
*                          Sync Master. 
* @return none.
*/
void CSvgLockedRealTimeEntity::SetCurSyncMasterStatus( TBool aSyncMasterStatus )
{
    //iSyncMasterCurrent stores current Mastership status of LRT
    // only if LRT is getting set as syncmaster then only we store the current tick
    if(aSyncMasterStatus && !iSyncMasterCurrent)
        {
        iStoredTick = iOwnerDocument->TimeContainer()->GetCurTick();
        #ifdef _DEBUG
        RDebug::Printf("LRT gets set as Syncmaster \n");
        #endif
	    }
    if(iSyncMasterCurrent != aSyncMasterStatus)
        {
        iSyncMasterCurrent = aSyncMasterStatus;    
        }
    #ifdef _DEBUG
	RDebug::Printf("====================================");
	RDebug::Printf("IStoredTick.iRealTimeTick %d", iStoredTick.iRealTimeTick);
	RDebug::Printf("iStoredTick.iParentTcTick %d", iStoredTick.iParentTcTick);
	RDebug::Printf("====================================");
	#endif
}

        
//==========================================================================
/**
* Check if timed entity can provide timing ticks to rest of time 
* container elements. This behavior could change dynamically. 
* For example, if audio clip is over, the audio element can't generate 
* ticks for others.
* @since S60 3.2
* @param none.
* @return TBool True if can generate timing tick.
*/
// LRT can always generate tick, it acts as a pseudo realtime
TBool  CSvgLockedRealTimeEntity::CanGenerateTick()
    {
  return ETrue;  
    }
//==========================================================================
/**
* Check if timed entity can use its parent's tick. Usually only the
* parent document should return true for this function.
* All other elements return false
* @since S60 3.2
* @param none.
* @return TBool True if can use parent's timing tick.
*/
//only used in animation purposes
TBool CSvgLockedRealTimeEntity::CanUseParentTick()
{
    return EFalse;
}
    
//==========================================================================
/**
* If the timed entity needs to be in sync with the time container and 
* it has slipped beyond the sync tolerance limit, the method is called to 
* bring the element in sync with the time container.
* @since S60 3.2
* @param aSynctime Time for resync in msecs.
* @return none.
*/

void CSvgLockedRealTimeEntity::ResyncTimedEntity( TUint32 /*aSynctime*/ )
{
    
}
    
//==========================================================================
/**
* This would be used for pausing the locked timed entity while other locked
* timed entities get loaded.
* @since S60 3.2
* @param none.
* @return none.
*/
void  CSvgLockedRealTimeEntity::PauseTimedEntity()
    {
        iStoredTick =  iOwnerDocument->TimeContainer()->GetCurTick();
        iIsPaused = ETrue;
    }
//==========================================================================
/**
* This would be used for resuming the locked timed entity once all locked
* timed entities get loaded.
* @since S60 3.2
* @param none.
* @return none.
*/
void  CSvgLockedRealTimeEntity::ResumeTimedEntity()
    {
        iStoredTick =  iOwnerDocument->TimeContainer()->GetCurTick();
        iIsPaused = EFalse;
    }
//==========================================================================
/**
* This would be used for stopping the timed entity.
* @since S60 3.2
* @param none.
* @return none.
*/
void CSvgLockedRealTimeEntity::StopTimedEntity()
    {
        iStoredTick.iParentTcTick = 0;
        iStoredTick.iRealTimeTick = 0;
    }

//==========================================================================
/* Return the type of class
@Return Type Enumeration of object type
@Parameters none
*/
TSvgObjectType  CSvgLockedRealTimeEntity::ObjectType()
    {
    return ESvgLockedRealTimeEntity;
    }
    
//==========================================================================
//for animation element
CSvgTimeContainer* CSvgLockedRealTimeEntity::GetChildTimeContainer()
    {
    return NULL;
    }
TBool CSvgLockedRealTimeEntity::AnimProcL( MSvgTimerEvent* /*aEvent*/ )
    {
    return ETrue;
    }
void CSvgLockedRealTimeEntity::ResetAnimationL()  
    {
    
    }

/*******************************End of file*********************************/
