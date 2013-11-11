/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
/*
 * gestureeventfilter.cpp
 *
 *  Created on: Jan 22, 2009
 *      Author: tyutyuni
 */

#include "gesturedefs.h"
#include "gestureeventfilter.h"


using namespace RT_GestureHelper;

CGestureEventFilter::CGestureEventFilter(TInt aTapSize):iTapSize(aTapSize)
    {
    
    }

CGestureEventFilter::~CGestureEventFilter()
    {
    
    }


TBool CGestureEventFilter::FilterDrag(const TPointerEvent& aPointerEvent, TInt& aReason)
    {
    TTime now;
    now.HomeTime();
    return FilterDrag(aPointerEvent, now, aReason);
    }

TBool CGestureEventFilter::FilterDrag(const TPointerEvent& aPointerEvent, TTime& eventTime, TInt& aReason)
    {
    TBool filtered = EFalse;
    
    aReason = EFilteredNoReason;
    
    switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
            {
            iIsTouchDown = ETrue;
            iTouchDownTime = iLastEventTime;
            iTouchDownPos = aPointerEvent.iPosition;
            break;
            }
        case TPointerEvent::EDrag:
            {
            if (iIsTouchDown)
                {
                TPoint delta = aPointerEvent.iPosition - iTouchDownPos;
                filtered = (Abs(delta.iX) <= iTapSize && Abs(delta.iY) <= iTapSize);
                aReason = (filtered) ? EFilteredBySize : EFilteredNoReason;
                }
                
            if (!filtered)
                {
                filtered = (eventTime.MicroSecondsFrom(iLastEventTime) < KMinTimeBetweenDrags);
                aReason = (filtered) ? EFilteredByTime : EFilteredNoReason;
                }
            
            if (!filtered)
                {
                iLastEventTime = eventTime;
                }
            iIsTouchDown = filtered;
            
            break;
            }
        case TPointerEvent::EButton1Up:
            {
            iIsTouchDown = EFalse;
            break;
            }
        }
    
    
    return filtered;
    }
