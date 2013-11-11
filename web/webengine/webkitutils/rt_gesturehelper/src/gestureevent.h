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

#ifndef GESTUREEVENT_H_
#define GESTUREEVENT_H_

#include <e32base.h>
#include "rt_gestureobserver.h"
#include "pointarray.h"

namespace RT_GestureHelper
{
NONSHARABLE_CLASS( CGestureEvent ) : public CBase, public MGestureEvent
    {
public:    
    // From MGestureEvent
    TGestureCode Code( TAxis /*aRelevantAxis*/ ) { return iCode; };
    TBool IsHolding() const { return iIsHolding; };
    TPoint StartPos() const { return iStartPos; };
    TPoint CurrentPos() const { return iCurrPos; }; 
    TRealPoint Speed() const { return iSpeed; };
    TRealPoint SpeedPercent( const TRect& /*aEdges*/ ) const { return iSpeedPercent; };
    TPoint Distance() const { return iDistance; }; 
    CAlfVisual* Visual() const { return iVisual; };

    
    TGestureCode   iCode;
    TBool          iIsHolding;
    TPoint         iStartPos;
    TPoint         iCurrPos;
    TRealPoint     iSpeed;
    TRealPoint     iSpeedPercent;
    TPoint         iDistance;
    CAlfVisual*    iVisual;
    };
}


#endif /* GESTUREEVENT_H_ */
