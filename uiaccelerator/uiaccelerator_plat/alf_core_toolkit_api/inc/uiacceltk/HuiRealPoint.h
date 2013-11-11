/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#ifndef __HUIREALPOINT_H__
#define __HUIREALPOINT_H__


#include <uiacceltk/HuiRealSize.h>


/**
 * Point composed of two floating-point components.
 */
NONSHARABLE_CLASS(THuiRealPoint)
    {
public:
    inline THuiRealPoint() 
            : iX(0), iY(0) 
        {
        }

    inline THuiRealPoint(TReal32 aX, TReal32 aY) 
            : iX(aX), iY(aY) 
        {
        }
        
    inline THuiRealPoint(const TPoint& aPoint)
            : iX((TReal32)aPoint.iX), iY((TReal32)aPoint.iY)
        {
        }

    inline THuiRealSize AsSize() const
        {
        return THuiRealSize(iX, iY);
        }
        
    inline operator TPoint() const
        {
        // Here is a rounding error!
        return TPoint((TInt)iX, (TInt)iY);
        }

    inline TPoint Round() const
        {
        TPoint rounded;
        rounded.iX = HUI_ROUND_FLOAT_TO_INT( iX );
        rounded.iY = HUI_ROUND_FLOAT_TO_INT( iY );
        return rounded; 
        }

    inline THuiRealPoint operator + (const THuiRealPoint& aOther) const
        {
        return THuiRealPoint(iX + aOther.iX, iY + aOther.iY);
        }

    inline THuiRealPoint& operator += (const THuiRealPoint& aOther) 
        {
        iX += aOther.iX;
        iY += aOther.iY;
        return *this;
        }

    inline THuiRealPoint& operator -= (const THuiRealPoint& aOther) 
        {
        iX -= aOther.iX;
        iY -= aOther.iY;
        return *this;
        }
        
    inline THuiRealPoint operator - () const
        {
        return THuiRealPoint(-iX, -iY);
        }

    inline THuiRealPoint operator - (const THuiRealPoint& aOther) const
        {
        return THuiRealPoint(iX - aOther.iX, iY - aOther.iY);
        }
        
    inline THuiRealPoint operator * (TReal32 aFactor) const
        {
        return THuiRealPoint(iX * aFactor, iY * aFactor);
        }
            
    inline THuiRealPoint Abs() const
        {
        return THuiRealPoint(iX<0?-iX:iX, iY<0?-iY:iY);
        }

public:    

    /** X coordinate. */    
    TReal32 iX;

    /** Y coordinate. */
    TReal32 iY;
    
    };

#endif // __HUIREALPOINT_H__
