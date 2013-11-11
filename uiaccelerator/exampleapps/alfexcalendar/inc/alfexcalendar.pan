/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Resource headers for project ?myapp
*
*/



#ifndef __ALFEXCALENDAR_PAN__
#define __ALFEXCALENDAR_PAN__

/** alfexcalendar application panic codes */
enum TAlfExCalendarPanics 
    {
    EAlfExCalendarBasicUi = 1
    // add further panics here
    };

inline void Panic( TAlfExCalendarPanics aReason )
    {
	_LIT( applicationName,"AlfExCalendar" );
    User::Panic( applicationName, aReason );
    }

#endif // __ALFEXCALENDAR_PAN__
