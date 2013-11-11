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



#ifndef __HUIREALSIZE_H__
#define __HUIREALSIZE_H__

/** 
 * Rounds a float (TReal32) correctly into an integer (TInt). Takes care of both negative
 * and positive real number.
 */
#define HUI_ROUND_FLOAT_TO_INT(a) (a < 0 ? (TInt(a - 0.5f)) : (TInt(a + 0.5f)))
    

/**
 * 2D floating-point size.
 */
NONSHARABLE_CLASS(THuiRealSize)
    {
public:
    inline THuiRealSize() 
            : iWidth(0), iHeight(0) 
        {
        }
        
    inline THuiRealSize(TReal32 aWidth, TReal32 aHeight)
            : iWidth(aWidth), iHeight(aHeight)
        {
        }
        
    inline THuiRealSize(const TSize& aSize)
            : iWidth((TReal32)aSize.iWidth), iHeight((TReal32)aSize.iHeight)
        {
        }
        
    inline operator TSize() const
        {
        // here is a rounding error!
        return TSize((TInt)iWidth, (TInt)iHeight);
        }
        
    inline TSize Round() const
        {
        TSize rounded;
        rounded.iWidth = HUI_ROUND_FLOAT_TO_INT( iWidth );
        rounded.iHeight = HUI_ROUND_FLOAT_TO_INT( iHeight );
        return rounded; 
        }

    inline THuiRealSize operator * (TReal32 aFactor) const
        {
        return THuiRealSize(iWidth * aFactor, iHeight * aFactor);
        }
        
public:    

    /** Width. */    
    TReal32 iWidth;

    /** Height. */
    TReal32 iHeight;
    
    };

#endif // __HUIREALSIZE_H__
