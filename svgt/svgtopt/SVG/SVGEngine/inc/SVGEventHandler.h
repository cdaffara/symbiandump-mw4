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
* Description:  SVG Engine header file
 *
*/


#ifndef __INC_CSVGEVENTHANDLER__
#define __INC_CSVGEVENTHANDLER__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGEvent.h"



class MSvgElement;
class MSvgEventReceiver;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgEventHandler : public CBase
    {
    private:
        class TEventReceiverListItem
            {
            public:
                MSvgEventReceiver*                          iElement;
                TUint32                                     iBeginTime;
                TUint32                                     iAbsoluteEnd;
                MSvgEventReceiver*                          iTargetElement;
                TUint8                                      iEventMask;
                TUint16                                     iSubEventMask;
                TUint8										iWasDeleted;	
            };

    public:

        /**
         * Two phase constructor
         *
         * @since 1.0
         * @param None
         * @return An instance of the CSVgEventHandler class
         */
        static CSvgEventHandler*                    NewLC();

        /**
         * Two phase constructor
         *
         * @since 1.0
         * @param None
         * @return An instance of the CSVgEventHandler class
         */
        static CSvgEventHandler*                    NewL();

        /**
         * Destructor
         *
         * @since 1.0
         * @param None
         * @return None
         */
                                                    ~CSvgEventHandler();

    public:
		TInt32										Count();

		TInt32										AnimationElementsCount();
    
        void    ResetTimes();
        /**
         * Process events received from the client
         *
         * @since 1.0
         * @param aEvent - An object containing event information (implements MSvgEvent interface)
         * @return A boolean indicating success/failure in handling the event
         */
        TBool                                       ProcessEventL( MSvgEvent* aEvent );

        /**
         * Add an object to the list of objects that are notified of an event
         *
         * @since 1.0
         * @param aElement - An SVG element that wants to be registered for an event (implements MSvgEventReceiver interface)
         * @param aEventMask - A bit mask that indicates the types of events an object listens for
         * @return None
         */
        void                                        AddToEventReceiverListL( MSvgEventReceiver* aElement,
                                                                            TUint8 aEventMask,
                                                                            TUint32 aBeginTime = 0,
                                                                            TUint32 aEndTime = 0);

        /**
         * Add an object to the list of objects that are notified of an event
         *
         * @since 3.2
         * @param aElement - An SVG element that wants to be registered for an event (implements MSvgEventReceiver interface)
         * @param aEvent - An SVG Event that the target element needs to generate
         * @param aEventMask - A bit mask that indicates the types of events an object listens for
         * @return None
         */
        void                                        AddToEventReceiverListL( MSvgEventReceiver* aElement,
                                                                            TSvgEvent aEvent,
                                                                            TUint8 aEventMask,
                                                                            TUint32 aBeginTime = 0,
                                                                            TUint32 aEndTime = 0);

        /**
         * Remove an object from the list of objects that are notified of an event
         *
         * @since 1.0
         * @param aElement - An SVG element that wants to be unregistered for an event (implements MSvgEventReceiver interface)
         * @return None
         */
        void                                        RemoveFromEventReceiverList( MSvgEventReceiver* aElement );

		void										ClearNullsFromEventReceiverList();
		
        /**
         * Provide the event end time information to the Event Handler
         *
         * @since 1.0
         * @param aElement - An element that, ultimately, processes the event (eg. an animation element)
         * @param aTime - The end time for an event on the target element
         * @param aTargetElement - The element for which the event is processed
         * @return
         */
        void                                        AddEventBeginTime(MSvgEventReceiver* aElement,
                                                                    TUint32 aTime, MSvgEventReceiver* aTargetElement );


        /**
         * Reset the status of the event handler and all the event receivers
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void                                        Reset();
        void 										SetCurrentTime(TInt32 aTime);
		void 										Reset(MSvgEvent* aEvent);

        /**
         * An internal method that sorts the events in a time scale
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void                                        SortEventList();

        /**
         * Standard active object DoCancel method
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void DoCancel();

        /**
         * Standard avtive object RunL method
         *
         * @since 1.0
         * @param
         * @return
         */
        void RunL();

       /**
         * returns the next (or prev) focussable object starting with a given index
         * search wraps around to start or end
         *
         * @since 1.0
         * @param aIndex - search for a focussable object with this index
         * @param aNext - Etrue (next) EFalse (prev) (go forward or backward for search)
         * @param aEventMask - Event mask for event types the client is interested in
         * @param aNewIndex - Return the actual index where the object was found
         * @return The focussable object or NULL if not found
         */

        MSvgEventReceiver* GetEventReceiver(TInt32 aIndex, TBool aNext, TUint8 aMask, TInt32& aNewIndex);

		void ReInitialize();
		void DeactivateAnimations();

        /** 
         * EventMask: Return the event mask of the registered element
         *
         * @param MSvgEventReceiver the target element.
         * @returns: TUnit8, the event mask
         */
        TUint8 EventMask(MSvgEventReceiver* aElement);
        
        /** 
         * Checks whether the element is interactive or not and if 
         * interactive updates aSubEventMask.
         * @since v3.2
         * @param CSvgElementImpl the element.
         * @param aSubEventMask - contains the list of interactive events 
         *                        that the element can respond to.
         *                        It is a bitmask using TSvgInteractionEvent
         * @returns: ETrue if interactive otherwise EFalse.
         */
        TBool CheckInteractivityAndGetSubEventMask(CSvgElementImpl* aElement,
                                                    TUint16& aSubEventMask);
         /**
         * Checks whether the aSubEventMask is has a valid value set
         * @since v3.2
         * @param : aSubEventMask - The bitmap mask which contains the list
         *                          of interactive events if any with the element.  
         * @return: Etrue - if aSubEventMask has some valid bits set else EFalse.
         */
        TBool IsValidSubEventMask(TUint16 aSubEventMask);
        
    private:

        /**
         * Private constructor
         *
         * @since 1.0
         * @param None
         * @return None
         */
                                                    CSvgEventHandler();

        /**
         * A private constructor that constructs heap objects
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void                                        ConstructL();

        CArrayFixFlat<TEventReceiverListItem>*      iEventReceiverList;
        TUint8                                      iInProcess;
        TInt32                                      iCurrentTime;
    public:
        void DoAnimProcL(MSvgEvent*  aEvent);
    };

#endif //__INC_CSVGEventHandler__
