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



#ifndef __HUIREALRECT_H__
#define __HUIREALRECT_H__


#include <uiacceltk/HuiRealPoint.h>
#include <uiacceltk/HuiRealSize.h>


/**
 * Rectangle based on real numbers.
 */
NONSHARABLE_CLASS(THuiRealRect)
    {
public:
    inline THuiRealRect() 
        {
        }
        
    inline THuiRealRect(const THuiRealPoint& aTl,        
                        const THuiRealPoint& aBr)
            : iTl(aTl), iBr(aBr)
        {
        }
        
    inline THuiRealRect(const THuiRealPoint& aPoint, 
                        const THuiRealSize& aSize)
            : iTl(aPoint), iBr(aPoint)
        {
        iBr.iX += aSize.iWidth;
        iBr.iY += aSize.iHeight;
        }
        
    inline THuiRealRect(const TRect& aRect)
            : iTl(aRect.iTl), iBr(aRect.iBr)
        {
        }
        
    inline TReal32 Width() const
        {
        return iBr.iX - iTl.iX;
        }
        
    inline TReal32 Height() const
        {
        return iBr.iY - iTl.iY;
        }
        
    inline THuiRealPoint TopRight() const
        {
        return THuiRealPoint(iBr.iX, iTl.iY);
        }

    inline THuiRealPoint BottomLeft() const
        {
        return THuiRealPoint(iTl.iX, iBr.iY);
        }
        
    inline THuiRealSize Size() const
        {
        return THuiRealSize(Width(), Height());
        }

    inline THuiRealPoint Center() const
        {
        return iTl + THuiRealPoint(Width()/2, Height()/2);
        }

    inline void Grow(TReal32 aX, TReal32 aY)
        {
        iTl.iX -= aX;
        iTl.iY -= aY;
        iBr.iX += aX;
        iBr.iY += aY;
        }
        
    inline void Shrink(TReal32 aX, TReal32 aY)
        {
        iTl.iX += aX;
        iTl.iY += aY;
        iBr.iX -= aX;
        iBr.iY -= aY;
        }

    inline void Shrink(const TPoint& aPoint)
        {
        iTl.iX += aPoint.iX;
        iTl.iY += aPoint.iY;
        iBr.iX -= aPoint.iX;
        iBr.iY -= aPoint.iY;
        }
        
    inline void Shrink(const THuiRealPoint& aPoint)
        {
        iTl.iX += aPoint.iX;
        iTl.iY += aPoint.iY;
        iBr.iX -= aPoint.iX;
        iBr.iY -= aPoint.iY;
        }
        
    inline void Shrink(const THuiRealRect& aRect)
        {
        iTl.iX += aRect.iTl.iX;
        iTl.iY += aRect.iTl.iY;
        iBr.iX -= aRect.iBr.iX;
        iBr.iY -= aRect.iBr.iY;
        }
        
    inline void Move(TReal32 aDx, TReal32 aDy)
        {
        iTl.iX += aDx;
        iTl.iY += aDy;
        iBr.iX += aDx;
        iBr.iY += aDy;
        }
        
    inline operator TRect() const
        {
        return TRect(TPoint((TInt)iTl.iX, (TInt)iTl.iY), 
                     TPoint((TInt)iBr.iX, (TInt)iBr.iY));
        }
        
    inline TRect Round() const
        {
        TRect rounded;
        rounded.iTl.iX = HUI_ROUND_FLOAT_TO_INT( iTl.iX );
        rounded.iTl.iY = HUI_ROUND_FLOAT_TO_INT( iTl.iY );
        rounded.iBr.iX = HUI_ROUND_FLOAT_TO_INT( iBr.iX );
        rounded.iBr.iY = HUI_ROUND_FLOAT_TO_INT( iBr.iY );
        return rounded; 
        }

    inline void BoundingRect( const THuiRealRect& aRect )
        {
        iTl.iX = Min( iTl.iX, aRect.iTl.iX );
        iTl.iY = Min( iTl.iY, aRect.iTl.iY );
        iBr.iX = Max( iTl.iX, aRect.iTl.iX );
        iBr.iY = Max( iTl.iY, aRect.iTl.iY );
        }

public:    

    /** Top left corner. */
    THuiRealPoint iTl;

    /** Bottom right corner. */
    THuiRealPoint iBr;
    
    };

#endif // __HUIREALRECT_H__
