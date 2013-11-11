/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "SenDateUtils.h"

namespace
    {
    _LIT8(KXmlDateTimeFormat8, "%04d-%02d-%02dT%02d:%02d:%02dZ"); // prior WS-* MT / SCT timestamp fix (2006-12-06) => Liberty IOP confirmed
    _LIT8(KXmlDateTimeFormat82, "%04d-%02d-%02dT%02d:%02d:%02d.%06dZ");
    }

EXPORT_C TTime SenDateUtils::FromXmlDateTimeL(const TDesC8& aXmlDateTime)
    {
    // Format: '2004-01-22T15:31:00Z' or '2004-01-22T15:31:00+02:00'

    TInt year;
    TInt month;
    TInt day;
    TInt hour;
    TInt min;
    TInt sec;
    TInt microsec;

    // Try to leave instead of panic in all places
    if(aXmlDateTime.Length() < 20) { User::Leave(KErrUnderflow); }

    TLex8 yearLex(aXmlDateTime.Mid(0, 4));
    User::LeaveIfError(yearLex.Val(year));

    if(aXmlDateTime[4] != '-') { User::Leave(KErrGeneral); }

    TLex8 monthLex(aXmlDateTime.Mid(5, 2));
    User::LeaveIfError(monthLex.Val(month));
    month--;
    if(month < 0) { User::Leave(KErrUnderflow); }
    if(month >= 12) { User::Leave(KErrOverflow); }

    if(aXmlDateTime[7] != '-') { User::Leave(KErrGeneral); }

    TLex8 dayLex(aXmlDateTime.Mid(8, 2));
    User::LeaveIfError(dayLex.Val(day));
    day--;
    if(day < 0) { User::Leave(KErrUnderflow); }
    if(day >= 31) { User::Leave(KErrOverflow); }

    if(aXmlDateTime[10] != 'T') { User::Leave(KErrGeneral); }

    TLex8 hourLex(aXmlDateTime.Mid(11, 2));
    User::LeaveIfError(hourLex.Val(hour));
    if(hour < 0) { User::Leave(KErrUnderflow); }
    if(hour >= 24) { User::Leave(KErrOverflow); }

    if(aXmlDateTime[13] != ':') { User::Leave(KErrGeneral); }

    TLex8 minLex(aXmlDateTime.Mid(14, 2));
    User::LeaveIfError(minLex.Val(min));
    if(min < 0) { User::Leave(KErrUnderflow); }
    if(min >= 60) { User::Leave(KErrOverflow); }

    if(aXmlDateTime[16] != ':') { User::Leave(KErrGeneral); }

    TLex8 secLex(aXmlDateTime.Mid(17, 2));
    User::LeaveIfError(secLex.Val(sec));
    if(sec < 0) { User::Leave(KErrUnderflow); }
    if(sec >= 60) { User::Leave(KErrOverflow); }

    TUint16 nextChar = aXmlDateTime[19];
    TUint16 tzChar = aXmlDateTime[19];
    
    TInt i = 19;
    while((tzChar != 'Z') && (tzChar != '+') && (tzChar != '-'))
        {
        i++;
        if(i < aXmlDateTime.Length()) tzChar = aXmlDateTime[i];
        else { User::Leave(KErrGeneral); }
        }
        
    if(nextChar == '.')
        {
        TInt lastMicrosec;
        if(i > 26) lastMicrosec = 26;
        else lastMicrosec = i;
        TLex8 microsecLex(aXmlDateTime.Mid(20, lastMicrosec-20));
        User::LeaveIfError(microsecLex.Val(microsec));
        TInt j = 26;
        while (j > lastMicrosec)
            {
            microsec = microsec * 10;
            j--;
            }
        }
    else
        {
        microsec = 0;
        }       

    TTime time(
        TDateTime(
            year,
            static_cast<TMonth>(month),
            day,
            hour,
            min,
            sec,
            microsec
            )
        );

    if(tzChar == 'Z')
        {
        // UTC
        if(aXmlDateTime.Length() > i+1) { User::Leave(KErrOverflow); }
        }
    else
        {
        // Time zone info is appended
        if(aXmlDateTime.Length() < i+6) { User::Leave(KErrUnderflow); }
        if(aXmlDateTime.Length() > i+6) { User::Leave(KErrOverflow); }
        TInt tzSign = 0;
        if(tzChar == '+') { tzSign = +1; }
        if(tzChar == '-') { tzSign = -1; }
        if(tzSign == 0) { User::Leave(KErrGeneral); }

        TInt tzHour;
        TInt tzMin;

        TLex8 tzHourLex(aXmlDateTime.Mid(i+1, 2));
        User::LeaveIfError(tzHourLex.Val(tzHour));
        if(tzHour < 0) { User::Leave(KErrUnderflow); }
        if(tzHour >= 24) { User::Leave(KErrOverflow); }

        if(aXmlDateTime[i+3] != ':') { User::Leave(KErrGeneral); }

        TLex8 tzMinLex(aXmlDateTime.Mid(i+4, 2));
        User::LeaveIfError(tzMinLex.Val(tzMin));
        if(tzMin < 0) { User::Leave(KErrUnderflow); }
        if(tzMin >= 60) { User::Leave(KErrOverflow); }

        TInt tzShiftMins = tzSign * (tzMin + tzHour * 60);
        time += TTimeIntervalMinutes(tzShiftMins);
        }

    return time;
    }

EXPORT_C void SenDateUtils::ToXmlDateTimeUtf8L(TDes8& aDest, const TTime& aSrc)
    {
    if(aDest.MaxLength() < KXmlDateTimeMaxLength)
        {
        User::Leave(KErrOverflow);
        }

    TDateTime dt = aSrc.DateTime();
    aDest.Format(
        KXmlDateTimeFormat8,
        dt.Year(),
        dt.Month() + 1,
        dt.Day() + 1,
        dt.Hour(),
        dt.Minute(),
        dt.Second()
        );
}
    
    
EXPORT_C void SenDateUtils::ToXmlDateTimeUtf82L(TDes8& aDest, const TTime& aSrc)
    {
    if(aDest.MaxLength() < KXmlDateTimeMaxLength)
        {
        User::Leave(KErrOverflow);
        }

    TDateTime dt = aSrc.DateTime();
    aDest.Format(
        KXmlDateTimeFormat82,
        dt.Year(),
        dt.Month() + 1,
        dt.Day() + 1,
        dt.Hour(),
        dt.Minute(),
        dt.Second(),
		dt.MicroSecond()   // Added in WS-* MT / SCT fix (2006-12-06)
        );
    }
    

// End of File

