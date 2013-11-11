/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Algorithm to recognise gesture from a stream of points
*
*/


#ifndef _GESTURERECOGNISER_H_
#define _GESTURERECOGNISER_H_

#include <e32std.h>
#include "rt_gestureobserver.h"

namespace RT_GestureHelper
{

class TPointArray;

/**
 * Note: Not a static class or a function pointer, just to make it sligthly 
 * easier to replace it with a heavier implementation
 */
NONSHARABLE_CLASS( TGestureRecogniser )
    {
public:
    /** 
     * Translates points into a gesture code
     * @param aPoints Points that form the gestures. Client is not required 
     *                to pass in repeated points (sequential points that are 
     *                almost in the same place)
     * @param aIsHolding ETrue if gesture was ended at pointer being held down at same position
     *                   EFalse if gesture ended at pointer being released 
     * @return recognised gesture id or EUnknownGesture
     */
    TGestureCode GestureCode( const TPointArray& aPoints ) const;
    };
    
} // namespace GestureHelper

#endif // _GESTURERECOGNISER_H_
