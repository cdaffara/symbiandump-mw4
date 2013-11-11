/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine source file
 *
*/


#include "SVGEventHandler.h"
#include "SVGEventReceiver.h"

#include "SVGElementImpl.h"
#include "SVGListener.h"
#include "SVGAnimationBase.h"
// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
CSvgEventHandler* CSvgEventHandler::NewLC()
    {
    CSvgEventHandler* self = new ( ELeave ) CSvgEventHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//
// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
CSvgEventHandler* CSvgEventHandler::NewL()
    {
    CSvgEventHandler* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Private method to build heap objects
// ---------------------------------------------------------------------------
void CSvgEventHandler::ConstructL()
    {
    iEventReceiverList = new ( ELeave ) CArrayFixFlat<TEventReceiverListItem>( 20 );
    }

//
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CSvgEventHandler::CSvgEventHandler() : iInProcess( 0 ), iCurrentTime( 0 )
    {
    }

//
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CSvgEventHandler::~CSvgEventHandler()
    {
	if ( iEventReceiverList )
		{
    	delete iEventReceiverList;
		iEventReceiverList = NULL;
		}
    }



// --------------------------------------------------------------------------
// void CSvgEventHandler::Reset()
// Reset the status of the event handler and all the event receivers
// ---------------------------------------------------------------------------
void CSvgEventHandler::Reset()
    {
    // Need to wait if other thread is changing tree.
	if( iEventReceiverList )
		{
		iEventReceiverList->Reset();
		}
    }

//
// --------------------------------------------------------------------------
// TBool CSvgEventHandler::ProcessEventL( MSvgEvent* aEvent )
// Process events received from the client
// ---------------------------------------------------------------------------
TBool CSvgEventHandler::ProcessEventL( MSvgEvent* aEvent )
    {
    TBool redrawNeeded = EFalse;
    if ( aEvent->EventType() == ESvgEngineEventScreenUpdate )
         return ETrue;       // No element will recieve this event

    if ( iEventReceiverList->Count() <= 0 )
        return redrawNeeded;    // No receiver is available

    // Timer event
    if ( aEvent->EventType() == ESvgEngineEventTimer )
        {
        // Update current time
        iCurrentTime = ( ( MSvgTimerEvent * ) aEvent )->Time();
        // call reset-reference-element to animation elements
        TInt eventReceiverListCnt = iEventReceiverList->Count();
		for ( TInt j = 0; j < eventReceiverListCnt; j++ )
            {
            if ( !(( *iEventReceiverList )[j].iWasDeleted) && 0 !=
                 ( aEvent->EventMask() |
                   ( *iEventReceiverList )[j].iEventMask ) )
                {
				if( (*iEventReceiverList )[j].iTargetElement )
					{
					(*iEventReceiverList )[j].iTargetElement->ResetReferenceAttrSet();
					}

                }
            }
		eventReceiverListCnt = iEventReceiverList->Count();            
        for ( TInt i = 0; i < eventReceiverListCnt; i++ )
            {
            if ( !(( *iEventReceiverList )[i].iWasDeleted) && 0 !=
                 ( aEvent->EventMask() |
                   ( *iEventReceiverList )[i].iEventMask ) )
                {
                ( *iEventReceiverList )[i].iElement->ResetReferenceElementL();
                }
            }
        }
    // Internal event & key event need time to be added
    if ( aEvent->EventType() == ESvgEngineInternalEvent &&
		             ! (( MSvgInternalEvent * )aEvent)->UserSeek() )
		{
        ( ( MSvgInternalEvent * ) aEvent )->SetTime( iCurrentTime );
		}
    if ( aEvent->EventType() == ESvgEngineEventKeyPress )
		{
        ( ( MSvgUiKeyEvent * ) aEvent )->SetTime( iCurrentTime );
		}

    // Sending events to elements
    for ( TInt i = 0; i < iEventReceiverList->Count(); i++ )
        {
        if ( !(( *iEventReceiverList )[i].iWasDeleted) && 0 !=
             ( aEvent->EventMask() | ( *iEventReceiverList )[i].iEventMask ) )
            {
            // Send event to list[i].iElement
            
            //check if the subeventmask is some valid bits set
            if( IsValidSubEventMask(( *iEventReceiverList )[i].iSubEventMask))
                {
                if(
                ( *iEventReceiverList )[i].iElement->
                ReceiveEventL(aEvent,
                             ( *iEventReceiverList )[i].iSubEventMask  ) )
                    {
                    redrawNeeded=ETrue;    
                    }
                
                }
            else if ( ( *iEventReceiverList )[i].iElement->ReceiveEventL( aEvent) )
                redrawNeeded = ETrue;
            
            }
        }

	ClearNullsFromEventReceiverList();
	
    return redrawNeeded;
    }

// -------------------------------------------------------------------------
// TBool IsValidSubEventMask(TUint16 aSubEventMask)
//
// Checks whether aSubEventMask contains some valid bits set for 
// interactive events
// 
// Returns: True if any valid bit is set else false.
//
// --------------------------------------------------------------------------
TBool CSvgEventHandler::IsValidSubEventMask(TUint16 aSubEventMask)
    {
    TUint16 temp=0x1FF;
    temp=temp&aSubEventMask;
    if(temp==aSubEventMask)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// --------------------------------------------------------------------------
// void CSvgEventHandler::AddToEventReceiverListL( MSvgEventReceiver* aElement,
//
// AddToEventReceiverListL: Add an object to the list of objects that are
// notified of an event.
//
// Returns: none
//
// ---------------------------------------------------------------------------
void CSvgEventHandler::AddToEventReceiverListL( MSvgEventReceiver* aElement,
                                                TUint8 aEventMask,
                                                TUint32 aBeginTime,
                                                TUint32 aEndTime)
    {
    // check to see if receiver is already in list and simply return
    TInt eventReceiverListCnt = iEventReceiverList->Count();
    for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        if ( !(( *iEventReceiverList )[i].iWasDeleted) && (*iEventReceiverList)[i].iElement == aElement )
            {
            return;
            }
        }

    TEventReceiverListItem listitem;
    listitem.iSubEventMask=0xFFFF;
    listitem.iElement = aElement;
    listitem.iEventMask = aEventMask;
    listitem.iAbsoluteEnd = aEndTime;
	listitem.iTargetElement = NULL;
	listitem.iBeginTime = aBeginTime;
	listitem.iWasDeleted = 0;
    iEventReceiverList->AppendL( listitem );
    }

// --------------------------------------------------------------------------
// void CSvgEventHandler::AddToEventReceiverListL( MSvgEventReceiver* aElement,
//
// AddToEventReceiverListL: Add an object to the list of objects that are
// notified of an event.
//
// Returns: none
//
// ---------------------------------------------------------------------------
void CSvgEventHandler::AddToEventReceiverListL( MSvgEventReceiver* aElement,
                                                TSvgEvent aEvent,
                                                TUint8 aEventMask,
                                                TUint32 aBeginTime,
                                                TUint32 aEndTime)
    {
    TEventReceiverListItem listitem;
    listitem.iElement = aElement;
    listitem.iSubEventMask=0xFFFF;
    TSvgInteractionEvent lsubeventtype;
    switch(aEvent)
        {
        case ESvgEventFocusin:
        lsubeventtype=ESvgInteractiveEventFocusin;
        break;
                
        case ESvgEventFocusout:
        lsubeventtype=ESvgInteractiveEventFocusout;
        break;
                
        case ESvgEventActivate:
        lsubeventtype=ESvgInteractiveEventActivate;
        break;
                
        case ESvgEventClick:
        lsubeventtype=ESvgInteractiveEventClick;
        break;
                
        case ESvgEventMousedown:
        lsubeventtype=ESvgInteractiveEventMousedown;
        break;
                
        case ESvgEventMouseup:
        lsubeventtype=ESvgInteractiveEventMouseup;
        break;
                
        case ESvgEventMouseover:
        lsubeventtype=ESvgInteractiveEventMouseover;
        break;
                
        case ESvgEventMousemove:
        lsubeventtype=ESvgInteractiveEventMousemove;
        break;
                
        case ESvgEventMouseout:
        lsubeventtype=ESvgInteractiveEventMouseout;
        break;
                
        default:
        lsubeventtype=(TSvgInteractionEvent)0;
        break;
        }
    // check to see if receiver is already in list and just update the 
    // iSubEventMask and return
    TInt eventReceiverListCnt = iEventReceiverList->Count();
    for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        if ( !(( *iEventReceiverList )[i].iWasDeleted) && (*iEventReceiverList)[i].iElement == aElement )
            {
         
                (*iEventReceiverList)[i].iSubEventMask=
                ( *iEventReceiverList)[i].iSubEventMask | lsubeventtype;               
            
            return;
            }
        }
    listitem.iSubEventMask=listitem.iSubEventMask & lsubeventtype;
    listitem.iEventMask = aEventMask;
    listitem.iAbsoluteEnd = aEndTime;
	listitem.iTargetElement = NULL;
	listitem.iBeginTime = aBeginTime;
	listitem.iWasDeleted = 0;
    iEventReceiverList->AppendL( listitem );
    }
// --------------------------------------------------------------------------
// TUint16 CSvgEventHandler::CheckInteractivityAndGetSubEventMask
// (CSvgElementImpl* aElement,
//
// CheckInteractivityAndGetSubEventMask: Check whether the element 
// is interactive or not and get the iSubEventmask
//
// Returns: ETrue if interactive else False
//
// ---------------------------------------------------------------------------
TBool CSvgEventHandler::CheckInteractivityAndGetSubEventMask(
                                        CSvgElementImpl* aElement, TUint16& aSubEventMask )
    {
    TInt eventReceiverListCnt = iEventReceiverList->Count();
    for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        if((*iEventReceiverList)[i].iElement==aElement && 
           IsValidSubEventMask((*iEventReceiverList)[i].iSubEventMask) )
            {
            aSubEventMask=(*iEventReceiverList)[i].iSubEventMask;
            return ETrue;
            }
        }
        return EFalse;
    }

// --------------------------------------------------------------------------
// TUint8 CSvgEventHandler::EventMask(MSvgEventReceiver* aElement)
// EventMask: Return the event mask of the registered element
//
// Returns: TUnit8, the event mask
//
// ---------------------------------------------------------------------------
TUint8 CSvgEventHandler::EventMask(MSvgEventReceiver* aElement)
    {
    
    TInt eventReceiverListCnt = iEventReceiverList->Count();
    for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        if ( !(( *iEventReceiverList )[i].iWasDeleted) && (*iEventReceiverList)[i].iElement == aElement )
            {
            return (*iEventReceiverList)[i].iEventMask;
            }
        }
    return 0;
    }



//
// --------------------------------------------------------------------------
// void CSvgEventHandler::RemoveFromEventReceiverList( MSvgEventReceiver* aElement )
// Remove an object to the list of objects that are notified of an event
// ---------------------------------------------------------------------------
void CSvgEventHandler::RemoveFromEventReceiverList( MSvgEventReceiver* aElement )
    {
    TInt eventReceiverListCnt = iEventReceiverList->Count();
    for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        if ( !(( *iEventReceiverList )[i].iWasDeleted) && (*iEventReceiverList)[i].iElement == aElement )
            {
            //just setting to NULL for now because this could be called inside of ReceiveEvent
            //loop that already has the iEventReceiverList count.
            
            (*iEventReceiverList)[i].iWasDeleted = 1;
            //iEventReceiverList->Delete( i );
			break;
            }
        }
    }

// --------------------------------------------------------------------------
// void CSvgEventHandler::ClearNullsFromEventReceiverList( )
// Remove a null object
// ---------------------------------------------------------------------------
void CSvgEventHandler::ClearNullsFromEventReceiverList()
    {
    TInt eventReceiverListCnt = iEventReceiverList->Count();
    for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        if ( (*iEventReceiverList)[i].iWasDeleted )
            {
            iEventReceiverList->Delete( i );
            i = i - 1;
            eventReceiverListCnt = eventReceiverListCnt - 1;
            }
        }
    }


// --------------------------------------------------------------------------
// void  CSvgEventHandler::AddEventBeginTime(MSvgEventReceiver* aElement,  TUint32 aTime, MSvgEventReceiver* aTargetElement )
// Provide the event end time information to the Event Handler
// ---------------------------------------------------------------------------
void  CSvgEventHandler::AddEventBeginTime(MSvgEventReceiver* aElement,  TUint32 aTime, MSvgEventReceiver* aTargetElement )
    {

    TInt eventReceiverListCnt = iEventReceiverList->Count();
    for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        if( !(( *iEventReceiverList )[i].iWasDeleted) && aElement == iEventReceiverList->operator[](i).iElement && (iEventReceiverList->operator[](i).iEventMask == KSvgEventMaskTimer ))
            {
             iEventReceiverList->operator[](i).iBeginTime = aTime;
             iEventReceiverList->operator[](i).iTargetElement = aTargetElement;
			 break;
            }
        }
    }
// --------------------------------------------------------------------------
// TInt32 CSvgEventHandler::Count()
// gives the number of elements in the eent receiver list
// ---------------------------------------------------------------------------
TInt32 CSvgEventHandler::Count()
    {
	if(!iEventReceiverList) return 0;
	return  iEventReceiverList->Count();
    }
// --------------------------------------------------------------------------
// void CSvgEventHandler::SortEventList()
// An internal method that sorts the events in a time scale
// ---------------------------------------------------------------------------
void CSvgEventHandler::SortEventList()
    {

	ClearNullsFromEventReceiverList();
	
  // this algorithm is slighlty modified to use insertion sort.
 TInt i, j, tmpItemIndex;
  TInt eventReceiverListCnt = iEventReceiverList->Count();
  for (i=1; i < eventReceiverListCnt; i++)
  {

	TEventReceiverListItem  tmpitem;
    j = i;
       tmpitem.iElement = iEventReceiverList->operator[](i).iElement;
       tmpitem.iEventMask = iEventReceiverList->operator[](i).iEventMask;
       tmpitem.iAbsoluteEnd = iEventReceiverList->operator[](i).iAbsoluteEnd;
       tmpitem.iTargetElement = iEventReceiverList->operator[](i).iTargetElement;
	   tmpitem.iBeginTime = iEventReceiverList->operator[](i).iBeginTime;
	   tmpitem.iWasDeleted = iEventReceiverList->operator[](i).iWasDeleted;
       tmpItemIndex = i;

			    while (j > 0)
			    	{
					if( (tmpitem.iTargetElement == iEventReceiverList->operator[](j-1).iTargetElement )   &&
		                (tmpitem.iEventMask == iEventReceiverList->operator[](j-1).iEventMask) )


				    {
					     if(tmpitem.iBeginTime <
			                        iEventReceiverList->operator[](j-1).iBeginTime)
			             	{

						   iEventReceiverList->operator[](tmpItemIndex).iElement 		= iEventReceiverList->operator[](j-1).iElement;
				           iEventReceiverList->operator[](tmpItemIndex).iEventMask 		= iEventReceiverList->operator[](j-1).iEventMask;
				           iEventReceiverList->operator[](tmpItemIndex).iAbsoluteEnd 	= iEventReceiverList->operator[](j-1).iAbsoluteEnd;
				           iEventReceiverList->operator[](tmpItemIndex).iTargetElement 	= iEventReceiverList->operator[](j-1).iTargetElement;
						   iEventReceiverList->operator[](tmpItemIndex).iBeginTime		=  iEventReceiverList->operator[](j-1).iBeginTime;
						   tmpItemIndex = j-1;
						    j = j - 1;
						 	}
				   		 else
				    		{
				    		break;
				    		}
				    }
				    else
				    	{
				    	j--;
				    	}
			  		}
			   iEventReceiverList->operator[](tmpItemIndex).iElement 		= tmpitem.iElement;
			   iEventReceiverList->operator[](tmpItemIndex).iEventMask 		= tmpitem.iEventMask;
			   iEventReceiverList->operator[](tmpItemIndex).iAbsoluteEnd 	= tmpitem.iAbsoluteEnd;
			   iEventReceiverList->operator[](tmpItemIndex).iTargetElement	= tmpitem.iTargetElement;
			   iEventReceiverList->operator[](tmpItemIndex).iBeginTime		= tmpitem.iBeginTime;

  }

    }


// ---------------------------------------------------------------------------
// returns the next (or prev) focussable object starting with a given index
// search wraps around to start or end
// ---------------------------------------------------------------------------
MSvgEventReceiver* CSvgEventHandler::GetEventReceiver( TInt32 aIndex, TBool aNext, TUint8 aMask, TInt32& aNewIndex )
    {
    // Check underflow
    if ( aIndex < 0 || !iEventReceiverList->Count() )
        {
        return NULL;
        }

    // This will ensure we loop around to start
    if( aIndex >= iEventReceiverList->Count() )
        {
        aIndex = iEventReceiverList->Count() - 1;
        }

    // Search forward or backward
    if( aNext )
        {
        while( aIndex < iEventReceiverList->Count() )
            {
            if( !(( *iEventReceiverList )[aIndex].iWasDeleted) && iEventReceiverList->operator[]( aIndex ).iEventMask & aMask )
                {
                aNewIndex = aIndex;
                return iEventReceiverList->operator[]( aIndex ).iElement;
                }

            aIndex++;
            }
        }
    else
        {
        while( aIndex >= 0 )
            {
            if( !(( *iEventReceiverList )[aIndex].iWasDeleted) && iEventReceiverList->operator[]( aIndex ).iEventMask & aMask )
                {
                aNewIndex = aIndex;
                return iEventReceiverList->operator[]( aIndex ).iElement;
                }

            aIndex--;
            }
        }

    return NULL;
    }
// --------------------------------------------------------------------------
// void CSvgEventHandler::Reset(MSvgEvent* aEvent)
// ---------------------------------------------------------------------------
void CSvgEventHandler::Reset(MSvgEvent* aEvent)
	{

	this->DeactivateAnimations();

    TInt eventReceiverListCnt = iEventReceiverList->Count();
	for ( TInt j = 0; j < eventReceiverListCnt; j++ )
            {
            if ( !(( *iEventReceiverList )[j].iWasDeleted) && 0 !=
                 ( aEvent->EventMask() |
                   ( *iEventReceiverList )[j].iEventMask ) )
                {
				if( (*iEventReceiverList )[j].iTargetElement )
					{
					(*iEventReceiverList )[j].iTargetElement->ResetReferenceAttrSet();
					}

                }
            }
	
	eventReceiverListCnt = iEventReceiverList->Count();
	for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        	if (!(( *iEventReceiverList )[i].iWasDeleted))
        	{
              // Send event to list[i].iElement
             ( *iEventReceiverList )[i].iElement->Reset( aEvent );
        	}
        	
        	if (iEventReceiverList->Count() != eventReceiverListCnt)
        	{
        		//get a new count because the list changed size in the reset call above
        		eventReceiverListCnt = iEventReceiverList->Count();
        		//start back over at zero so we make sure all elements get their reset called
        		i = 0;
        	}
        }
}

// --------------------------------------------------------------------------
// void CSvgEventHandler::ReInitialize()
// ---------------------------------------------------------------------------
void CSvgEventHandler::ReInitialize()
	{
	TInt eventReceiverListCnt = iEventReceiverList->Count();
	for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        	if (!(( *iEventReceiverList )[i].iWasDeleted))
        	{
		 	iEventReceiverList->operator[](i).iBeginTime = 0;
         ( *iEventReceiverList )[i].iElement->ReInitializeAnimation();
        	}
        }
	}

// --------------------------------------------------------------------------
// void CSvgEventHandler::DeactivateAnimations()
// ---------------------------------------------------------------------------
void CSvgEventHandler::DeactivateAnimations()
{
TInt eventReceiverListCnt = iEventReceiverList->Count();
for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        	if (!(( *iEventReceiverList )[i].iWasDeleted))
        	{
        		iEventReceiverList->operator[](i).iBeginTime = 0;
         		( *iEventReceiverList )[i].iElement->DeactivateAnimation();
        	}
        }
}
// --------------------------------------------------------------------------
// void CSvgEventHandler::SetCurrentTime(TInt32 aTime)
// ---------------------------------------------------------------------------
void CSvgEventHandler::SetCurrentTime(TInt32 aTime)
{
iCurrentTime = aTime;
}
// --------------------------------------------------------------------------
// void CSvgEventHandler::DoAnimProcL(MSvgEvent*  aEvent)
// ---------------------------------------------------------------------------
void CSvgEventHandler::DoAnimProcL(MSvgEvent*  aEvent)
{
if(aEvent->EventType() != ESvgEngineEventTimer)return;
TInt eventReceiverListCnt = iEventReceiverList->Count();
for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
          if (!(( *iEventReceiverList )[i].iWasDeleted))
          {	
		  	( *iEventReceiverList )[i].iElement->DoAnimProcL(aEvent);
          }
        }
}

// --------------------------------------------------------------------------
// TInt32 CSvgEventHandler::AnimationElementsCount()
// ---------------------------------------------------------------------------
TInt32 CSvgEventHandler::AnimationElementsCount()
    {
	TInt32 lCount = 0;
	if(iEventReceiverList)
		{
		TInt aEventReceiverListCnt = iEventReceiverList->Count();
		for(TInt i=0; i < aEventReceiverListCnt; i++)
			{
			if (!(( *iEventReceiverList )[i].iWasDeleted))
        		{
				CSvgElementImpl* lElem= NULL;
				lElem  = (CSvgElementImpl*) (*iEventReceiverList)[i].iElement;
				if(lElem != NULL)
					{
					if( lElem->IsAnimatedElement() )
						{
               			lCount++;
						}
					}
        		}
			}
		}
	return lCount;

    }
void CSvgEventHandler::ResetTimes()
    {
    TInt eventReceiverListCnt = iEventReceiverList->Count();
    for ( TInt i = 0; i < eventReceiverListCnt; i++ )
        {
        if (!(( *iEventReceiverList )[i].iWasDeleted) )
        	{
    	    CSvgElementImpl* lElem= NULL;
			lElem  = (CSvgElementImpl*) (*iEventReceiverList)[i].iElement;
			
            if( lElem->IsAnimatedElement() )
			    {
			     CSvgAnimationBase *temp =  (CSvgAnimationBase *)((*iEventReceiverList )[i].iElement);
        	     temp->ResetTimes();
			    }
        	}
        }
    }
