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
* Description: 
*
*/




#ifndef __ALFEXCALENDARENGINE_H__
#define __ALFEXCALENDARENGINE_H__

#include <e32base.h>

/* Forward declarations. */


//
const TInt KAlfExCalendarEngineTextBufferLength = 50;

/**
* CAlfExCalendarEngine is a class that gives data to be displayed in 
* alfexcalendar example application.
*/
class CAlfExCalendarEngine : public CBase
    {
public:
    /* Constructors and destructor. */

    /**
     * Constructor
     */	
    static CAlfExCalendarEngine* NewL();
    
    /**
     * Constructor
     */
    static CAlfExCalendarEngine* NewLC();
    
    /**
     * Destructor.
     */
	virtual ~CAlfExCalendarEngine();
	
public:
	
	
    /* Methods. */

    /** @beginAPI */

    /**
     * Loads calendar events for current month.
     *
     * @param aDate: day information
     */
    void LoadCalendarEventsL( const TTime& aDate );

    /**
     * Returns true if calendar event is available for requested day. 
     *
     * @param aDate: day information
     * @return true or false
     */
    TBool EventsAvailable( const TTime& aDate );
    
    /**
     * Returns number of calendar events in requested day. 
     *
     * @param aDate: day information
     * @return number of events in requested day.
     *
     * @todo: or should we just return array of texts ?
     */
    TInt NumberOfEvents( const TTime& aDate );
    
    /**
     * Returns number of calendar events in requested day. 
     *
     * @param aDate: day information
     * @param aIndex: index of the event between 0...x-1, where x=NumberOfEvents().
     * @param aTextBuffer: text buffer where event information text is copied. 
     *        Must be at least XX characters long.
     *
     * @todo: should this return HBufC* instead of aTextBuffer parameter?
     */
    void GetEventInformation( const TTime& aDate, TInt aIndex, TDes& aTextBuffer );

    /** @endAPI */

private:
    /* Constructors. */

    /**
     * C++ constructor
     */
	CAlfExCalendarEngine();
	
    /**
     * Second-phase constructor. 
     */
	void ConstructL();
	
private:
    /**
    * Class to embed event text and event time. 
    * In this implementation only date is significant
    */
    class TAlfExCalendarEngineListItem
        {
        public:
            /**
            *  Date of the event
            */
            TTime   iItemDay;
            
            /**
            * Event text visible for the user
            */
            TBuf<KAlfExCalendarEngineTextBufferLength> iItemText;
        };

    /**
    * Array definition for the calendar events
    */
    typedef RArray<TAlfExCalendarEngineListItem> RAlfExCalendarEngineListItemArray;
    
private:
    /** Array of events */
    RAlfExCalendarEngineListItemArray iCalendarEventArray;
};

#endif // __ALFEXCALENDARENGINE_H__
