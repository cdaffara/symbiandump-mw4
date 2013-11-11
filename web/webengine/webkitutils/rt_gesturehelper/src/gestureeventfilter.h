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
 * gestureeventfilter.h
 *
 *  Created on: Jan 22, 2009
 *      Author: tyutyuni
 */

#include <e32base.h>
#include <e32std.h>
#include <w32std.h>


#ifndef GESTUREEVENTFILTER_H_
#define GESTUREEVENTFILTER_H_




namespace RT_GestureHelper
{

enum TFilterReason
    {
    EFilteredNoReason,
    EFilteredBySize,
    EFilteredByTime
    };

NONSHARABLE_CLASS(CGestureEventFilter) : public CBase
    {
    public:
        CGestureEventFilter(TInt aTapSize);
        ~CGestureEventFilter();
        TBool FilterDrag(const TPointerEvent& aPointerEvent, TTime& eventTime, TInt& aReason);
        TBool FilterDrag(const TPointerEvent& aPointerEvent, TInt& aReason);
    private:
        TTime  iTouchDownTime;
        TPoint iTouchDownPos;
        TBool  iIsTouchDown;
        TTime  iLastEventTime;
        TInt   iTapSize;
    };

}
#endif /* GESTUREEVENTFILTER_H_ */
