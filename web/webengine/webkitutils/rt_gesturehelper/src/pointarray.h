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
* Description:  Array of points
*
*/


#ifndef _POINTARRAY_H_
#define _POINTARRAY_H_

#include <e32std.h>

namespace RT_GestureHelper
{

/** 
 * Represents a single point given at certain time
 */
struct TPointEntry 
    {
    /** constructor */
    inline TPointEntry( const TPoint& aPos, const TTime& aTime ) 
            : iPos( aPos ), iTime( aTime )
        {
        }
    
    TPoint iPos;
    TTime iTime;
    };
    
/**
 * Array of points that wraps another point array
 * The purpose of this class is to allow deriving classes to override []
 * and in that member function adjust the point, e.g., by setting one
 * axis to always a constant value. This allows providing a sequence of
 * points to the recogniser in which the other axis is ignored.
 */
NONSHARABLE_CLASS( TPointArray )
    {
public:
    /** 
     * @param aPoints points array to wrap (and filter)
     */
    TPointArray( const RArray< TPointEntry >& aPoints ); 
    /** @returns length of the array */
    TInt Count() const;
    /** 
     * @returns a filtered point at aIndex. Default implementation same
     *          as Raw(...)
     * virtual so deriving classes can modify the point (e.g., filter one axis)
     */
    virtual TPoint operator[]( TInt aIndex ) const;
    /** @returns an raw, non-filtered point at aIndex */
    TPoint Raw( TInt aIndex ) const;
    
private:
    const RArray< TPointEntry >& iPoints;
    };

} // namespace GestureHelper

#endif // _POINTARRAY_H_
