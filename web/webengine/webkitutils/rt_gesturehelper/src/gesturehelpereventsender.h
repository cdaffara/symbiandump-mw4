/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture helper implementation
*
*/

#ifndef GESTUREHELPEREVENTSENDER_H_
#define GESTUREHELPEREVENTSENDER_H_

#include <e32base.h>
#include <coemain.h>
#include <rt_gesturehelper.h>

namespace RT_GestureHelper
{
class MGestureObserver;
class CGestureEvent;

NONSHARABLE_CLASS(CGestureEventSender) : public CActive
    {
public:
    enum TEventSenderState
        {
        ENoEvents,
        EEventsReady,
        EBusy
        };
    static CGestureEventSender* NewL( MGestureObserver& aObserver );
    ~CGestureEventSender();
    TInt AddEvent(const TGestureEvent& aGestureEvent);
protected:    
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
private:
    CGestureEventSender(MGestureObserver& aObserver);
    void Complete();
    void EmitEventL( const TGestureEvent& aGestureEvent );
    
    RArray< TGestureEvent >  iEvents;
    MGestureObserver& iObserver;
    TEventSenderState iState;
    };
}
#endif /* GESTUREHELPEREVENTSENDER_H_ */
