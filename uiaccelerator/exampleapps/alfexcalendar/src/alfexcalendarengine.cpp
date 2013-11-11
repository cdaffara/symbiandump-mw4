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



// INCLUDE FILES
#include <EikEnv.h>
#include <barsread.h>
#include <e32math.h>

#include <alfexcalendar.rsg>
#include "alfexcalendarengine.h"

// CONSTANTS
const TReal KAlfExCalendarEngineChangeDay = 0.7;
const TReal KAlfExCalendarEngineDayRandomizeLoopCount = 4;
const TReal KAlfExCalendarEngineDayRandomize = 10;
const TReal KAlfExCalendarEngineDayTenDays = 10;
const TReal KAlfExCalendarEngineArrayGranularity = 4;

// LOCAL CONSTANTS AND MACROS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CAlfExCalendarEngine::NewL
// ----------------------------------------------------
CAlfExCalendarEngine* CAlfExCalendarEngine::NewL()
    {
    CAlfExCalendarEngine* self = CAlfExCalendarEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ----------------------------------------------------
// CAlfExCalendarEngine::NewLC
// ----------------------------------------------------
CAlfExCalendarEngine* CAlfExCalendarEngine::NewLC()
    {
    CAlfExCalendarEngine* self = new (ELeave) CAlfExCalendarEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
    
// ----------------------------------------------------
// CAlfExCalendarEngine::CAlfExCalendarEngine
// ----------------------------------------------------
CAlfExCalendarEngine::CAlfExCalendarEngine()
    {
    iCalendarEventArray.Reset();
    }

    
// ----------------------------------------------------
// CAlfExCalendarEngine::ConstructL
// ----------------------------------------------------
void CAlfExCalendarEngine::ConstructL()
    {
    TReal   randomNumber;
    TInt    randomNumberInt;
    TInt64  seed=0;
    
    CDesCArrayFlat* array = new(ELeave) CDesCArrayFlat(KAlfExCalendarEngineArrayGranularity);
    CleanupStack::PushL( array );
    
    CEikonEnv* coeEnv = CEikonEnv::Static();
    TResourceReader reader;
    coeEnv->CreateResourceReaderLC(reader, R_ALFEXCALENDAR_CALENDAR_EVENTS_ARRAY);
    
    const TInt count = reader.ReadInt16();
    for(TInt loop = 0; loop < count; loop++)
        {
        HBufC* txt = reader.ReadHBufCL();
        CleanupStack::PushL(txt);
        array->AppendL(*txt);
        CleanupStack::PopAndDestroy(txt);
        }
    CleanupStack::PopAndDestroy(); //reader2

    TAlfExCalendarEngineListItem event;
    event.iItemDay.HomeTime();
    seed = event.iItemDay.Int64();
    randomNumber = count * Math::FRand(seed);
    randomNumberInt = (TInt) randomNumber;
    
    TTimeIntervalDays days(KAlfExCalendarEngineDayTenDays);
    event.iItemDay -= days; // lets start 10 days before present
    
    for(TInt i=0; i<KAlfExCalendarEngineDayRandomizeLoopCount; i++)
        {
        for(TInt loop = 0; loop < count; loop++)
            {
            randomNumber = Math::FRand(seed);
            if(randomNumber < KAlfExCalendarEngineChangeDay)
                {
                randomNumber *= KAlfExCalendarEngineDayRandomize;
                randomNumberInt = (TInt) randomNumber;
                days = TTimeIntervalDays(randomNumberInt);
                event.iItemDay += days;
                }
            event.iItemText.Copy((*array)[loop]);
            iCalendarEventArray.AppendL(event);
            }
        }
    CleanupStack::PopAndDestroy(array);
    }

    
// ----------------------------------------------------
// CAlfExCalendarEngine::~CAlfExCalendarEngine
// ----------------------------------------------------
CAlfExCalendarEngine::~CAlfExCalendarEngine()
    {
    iCalendarEventArray.Close();
    }

// ----------------------------------------------------
// CAlfExCalendarEngine::LoadCalendarEventsL
// ----------------------------------------------------
void CAlfExCalendarEngine::LoadCalendarEventsL( const TTime& aDate )
    {
    TReal   randomNumber;
    TInt    randomNumberInt;
    TInt64  seed=0;
    
    CDesCArrayFlat* array = new(ELeave) CDesCArrayFlat(KAlfExCalendarEngineArrayGranularity);
    CleanupStack::PushL( array );
    
    CEikonEnv* coeEnv = CEikonEnv::Static();
    TResourceReader reader;
    coeEnv->CreateResourceReaderLC(reader, R_ALFEXCALENDAR_CALENDAR_EVENTS_ARRAY);
    
    const TInt count = reader.ReadInt16();
    for(TInt loop = 0; loop < count; loop++)
        {
        HBufC* txt = reader.ReadHBufCL();
        CleanupStack::PushL(txt);
        array->AppendL(*txt);
        CleanupStack::PopAndDestroy(txt);
        }
    CleanupStack::PopAndDestroy(); //reader2

    TAlfExCalendarEngineListItem event;
    event.iItemDay = aDate; //.HomeTime();
    seed = event.iItemDay.Int64();
    randomNumber = count * Math::FRand(seed);
    randomNumberInt = (TInt) randomNumber;
    
    TTimeIntervalDays days(KAlfExCalendarEngineDayTenDays);
    event.iItemDay -= days; // lets start 10 days before present
    
    for(TInt i=0; i<KAlfExCalendarEngineDayRandomizeLoopCount; i++)
        {
        for(TInt loop = 0; loop < count; loop++)
            {
            randomNumber = Math::FRand(seed);
            if(randomNumber < KAlfExCalendarEngineChangeDay)
                {
                randomNumber *= KAlfExCalendarEngineDayRandomize;
                randomNumberInt = (TInt) randomNumber;
                days = TTimeIntervalDays(randomNumberInt);
                event.iItemDay += days;
                }
            event.iItemText.Copy((*array)[loop]);
            iCalendarEventArray.AppendL(event);
            }
        }
    CleanupStack::PopAndDestroy(array);
    }

// ----------------------------------------------------
// CAlfExCalendarEngine::EventsAvailable
// ----------------------------------------------------
TBool CAlfExCalendarEngine::EventsAvailable(const TTime& aDate)
    {
    TBool ret( EFalse );
    TDateTime requestedDate = aDate.DateTime();
    const TInt arrayCount = iCalendarEventArray.Count();
    for(TInt loop = 0; loop < arrayCount; loop++)
        {
        TDateTime eventDate = iCalendarEventArray[loop].iItemDay.DateTime();
        if(eventDate.Day() == requestedDate.Day() && 
           eventDate.Month() == requestedDate.Month() &&
           eventDate.Year() == requestedDate.Year())
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// ----------------------------------------------------
// CAlfExCalendarEngine::NumberOfEvents
// ----------------------------------------------------
TInt CAlfExCalendarEngine::NumberOfEvents(const TTime& aDate)
    {
    TInt count = 0;
    TDateTime requestedDate = aDate.DateTime();
    const TInt arrayCount = iCalendarEventArray.Count();
    for(TInt loop = 0; loop < arrayCount; loop++)
        {
        TDateTime eventDate = iCalendarEventArray[loop].iItemDay.DateTime();
        if(eventDate.Day() == requestedDate.Day() && 
           eventDate.Month() == requestedDate.Month() &&
           eventDate.Year() == requestedDate.Year())
            {
            count++;
            }
        }
    return count;
    }

    
// ----------------------------------------------------
// CAlfExCalendarEngine::NumberOfEvents
// ----------------------------------------------------
void CAlfExCalendarEngine::GetEventInformation( 
    const TTime& aDate, 
    TInt aIndex, 
    TDes& aTextBuffer)
    {
    aTextBuffer.Zero();
    TInt count = KErrNotFound;
    TDateTime requestedDate = aDate.DateTime();
    const TInt arrayCount = iCalendarEventArray.Count();
    for(TInt loop = 0; loop < arrayCount; loop++ )
        {
        TDateTime eventDate = iCalendarEventArray[loop].iItemDay.DateTime();
        if(eventDate.Day() == requestedDate.Day() && 
           eventDate.Month() == requestedDate.Month() &&
           eventDate.Year() == requestedDate.Year())
            {
            count++;
            }
        if(aIndex == count)
            {
            aTextBuffer.Copy(iCalendarEventArray[loop].iItemText);
            loop = arrayCount;
            }
        }
    }
    
    
// end of file    