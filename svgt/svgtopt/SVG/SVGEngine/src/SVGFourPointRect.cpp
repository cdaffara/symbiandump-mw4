/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine header file
 *
*/


#include "SVGFourPointRect.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
TSvgFourPointRect::TSvgFourPointRect( )
{
	iPoint1.iX = 0;
   	iPoint1.iY = 0;

   	iPoint2.iX = 0;
   	iPoint2.iY = 0;

   	iPoint3.iX = 0;
   	iPoint3.iY = 0;

   	iPoint4.iX = 0;
   	iPoint4.iY = 0;
}


// --------------------------------------------------------------------------
// TSvgFourPointRect::TSvgFourPointRect( TGfxRectangle2D aRect )
// ---------------------------------------------------------------------------
TSvgFourPointRect::TSvgFourPointRect( TGfxRectangle2D aRect )
{
    	iPoint1.iX = aRect.iX;
    	iPoint1.iY = aRect.iY;

    	iPoint2.iX = aRect.iX + aRect.iWidth;
    	iPoint2.iY = aRect.iY;

    	iPoint3.iX = aRect.iX;
    	iPoint3.iY = aRect.iY + aRect.iHeight;

    	iPoint4.iX = aRect.iX + aRect.iWidth;
    	iPoint4.iY = aRect.iY + aRect.iHeight;

}

// --------------------------------------------------------------------------
// void TSvgFourPointRect::SetRectPoints( TGfxRectangle2D aRect )
// ---------------------------------------------------------------------------
void TSvgFourPointRect::SetRectPoints( TGfxRectangle2D aRect )
{
    	iPoint1.iX = aRect.iX;
    	iPoint1.iY = aRect.iY;

    	iPoint2.iX = aRect.iX + aRect.iWidth;
    	iPoint2.iY = aRect.iY;

    	iPoint3.iX = aRect.iX;
    	iPoint3.iY = aRect.iY + aRect.iHeight;

    	iPoint4.iX = aRect.iX + aRect.iWidth;
    	iPoint4.iY = aRect.iY + aRect.iHeight;

}

// --------------------------------------------------------------------------
// TSvgFourPointRect::TSvgFourPointRect( TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight )
// ---------------------------------------------------------------------------
TSvgFourPointRect::TSvgFourPointRect( TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight )
{
		iPoint1.iX = aX;
    	iPoint1.iY = aY;

    	iPoint2.iX = aX + aWidth;
    	iPoint2.iY = aY;

    	iPoint3.iX = aX;
    	iPoint3.iY = aY + aHeight;

    	iPoint4.iX = aX + aWidth;
    	iPoint4.iY = aY + aHeight;
}

// --------------------------------------------------------------------------
// void TSvgFourPointRect::SetRectPoints( TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight )
// ---------------------------------------------------------------------------
void TSvgFourPointRect::SetRectPoints( TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight )
{
		iPoint1.iX = aX;
    	iPoint1.iY = aY;

    	iPoint2.iX = aX + aWidth;
    	iPoint2.iY = aY;

    	iPoint3.iX = aX;
    	iPoint3.iY = aY + aHeight;

    	iPoint4.iX = aX + aWidth;
    	iPoint4.iY = aY + aHeight;
}




// --------------------------------------------------------------------------
// TSvgFourPointRect::TSvgFourPointRect(TGfxPoint2D aPoint1, TGfxPoint2D aPoint2, TGfxPoint2D aPoint3, TGfxPoint2D aPoint4)
// ---------------------------------------------------------------------------
TSvgFourPointRect::TSvgFourPointRect(TGfxPoint2D aPoint1, TGfxPoint2D aPoint2, TGfxPoint2D aPoint3, TGfxPoint2D aPoint4)
{
	iPoint1 = aPoint1;
	iPoint2 = aPoint2;
	iPoint3 = aPoint3;
	iPoint4 = aPoint4;
}


// --------------------------------------------------------------------------
// void TSvgFourPointRect::SetRectPoints(TGfxPoint2D aPoint1, TGfxPoint2D aPoint2, TGfxPoint2D aPoint3, TGfxPoint2D aPoint4)
// ---------------------------------------------------------------------------
void TSvgFourPointRect::SetRectPoints(TGfxPoint2D aPoint1, TGfxPoint2D aPoint2, TGfxPoint2D aPoint3, TGfxPoint2D aPoint4)
{
	iPoint1 = aPoint1;
	iPoint2 = aPoint2;
	iPoint3 = aPoint3;
	iPoint4 = aPoint4;
}

// --------------------------------------------------------------------------
// TSvgFourPointRect::TSvgFourPointRect(TPoint aPoint1, TPoint aPoint2, TPoint aPoint3, TPoint aPoint4)
// ---------------------------------------------------------------------------
TSvgFourPointRect::TSvgFourPointRect(TPoint aPoint1, TPoint aPoint2, TPoint aPoint3, TPoint aPoint4)
{
	iPoint1.iX = TFloatFixPt(aPoint1.iX);
	iPoint2.iX = TFloatFixPt(aPoint2.iX);
	iPoint3.iX = TFloatFixPt(aPoint3.iX);
	iPoint4.iX = TFloatFixPt(aPoint4.iX);
	iPoint1.iY = TFloatFixPt(aPoint1.iY);
	iPoint2.iY = TFloatFixPt(aPoint2.iY);
	iPoint3.iY = TFloatFixPt(aPoint3.iY);
	iPoint4.iY = TFloatFixPt(aPoint4.iY);

}

// --------------------------------------------------------------------------
// void TSvgFourPointRect::SetRectPoints(TPoint aPoint1, TPoint aPoint2, TPoint aPoint3, TPoint aPoint4)
// ---------------------------------------------------------------------------
void TSvgFourPointRect::SetRectPoints(TPoint aPoint1, TPoint aPoint2, TPoint aPoint3, TPoint aPoint4)
{
	iPoint1.iX = TFloatFixPt(aPoint1.iX);
	iPoint2.iX = TFloatFixPt(aPoint2.iX);
	iPoint3.iX = TFloatFixPt(aPoint3.iX);
	iPoint4.iX = TFloatFixPt(aPoint4.iX);
	iPoint1.iY = TFloatFixPt(aPoint1.iY);
	iPoint2.iY = TFloatFixPt(aPoint2.iY);
	iPoint3.iY = TFloatFixPt(aPoint3.iY);
	iPoint4.iY = TFloatFixPt(aPoint4.iY);

}


// --------------------------------------------------------------------------
// void TSvgFourPointRect::GetPoints(TPoint& aPoint1, TPoint& aPoint2, TPoint& aPoint3, TPoint& aPoint4) const
// ---------------------------------------------------------------------------
void TSvgFourPointRect::GetPoints(TPoint& aPoint1, TPoint& aPoint2, TPoint& aPoint3, TPoint& aPoint4) const
{
		aPoint1.iX = (TInt)iPoint1.iX;
		aPoint1.iY = (TInt)iPoint1.iY;

		aPoint2.iX = (TInt)iPoint2.iX;
		aPoint2.iY = (TInt)iPoint2.iY;

		aPoint3.iX = (TInt)iPoint3.iX;
		aPoint3.iY = (TInt)iPoint3.iY;

		aPoint4.iX = (TInt)iPoint4.iX;
		aPoint4.iY = (TInt)iPoint4.iY;
}

// --------------------------------------------------------------------------
// void TSvgFourPointRect::GetPoints(TGfxPoint2D& aPoint1, TGfxPoint2D& aPoint2, TGfxPoint2D& aPoint3, TGfxPoint2D& aPoint4)
// ---------------------------------------------------------------------------
void TSvgFourPointRect::GetPoints(TGfxPoint2D& aPoint1, TGfxPoint2D& aPoint2, TGfxPoint2D& aPoint3, TGfxPoint2D& aPoint4)
{
		aPoint1 = iPoint1;
		aPoint2 = iPoint2;
		aPoint3 = iPoint3;
		aPoint4 = iPoint4;
}


// --------------------------------------------------------------------------
// void TSvgFourPointRect::GetTRect(TRect& aRect)
// ---------------------------------------------------------------------------
void TSvgFourPointRect::GetTRect(TRect& aRect)
{

	TFloatFixPt lMaxX = iPoint1.iX;

    if (iPoint2.iX > lMaxX)
    {
    	lMaxX = iPoint2.iX;
    }
    if (iPoint3.iX > lMaxX)
    {
    	lMaxX = iPoint3.iX;
    }
    if (iPoint4.iX > lMaxX)
    {
    	lMaxX = iPoint4.iX;
    }

    TFloatFixPt lMaxY = iPoint1.iY;

    if (iPoint2.iY > lMaxY)
    {
    	lMaxY = iPoint2.iY;
    }
    if (iPoint3.iY > lMaxY)
    {
    	lMaxY = iPoint3.iY;
    }
    if (iPoint4.iY > lMaxY)
    {
    	lMaxY = iPoint4.iY;
    }

   	TFloatFixPt lMinX = iPoint1.iX;

    if (iPoint2.iX < lMinX)
    {
    	lMinX = iPoint2.iX;
    }
    if (iPoint3.iX < lMinX)
    {
    	lMinX = iPoint3.iX;
    }
    if (iPoint4.iX < lMinX)
    {
    	lMinX = iPoint4.iX;
    }

    TFloatFixPt lMinY = iPoint1.iY;

    if (iPoint2.iY < lMinY)
    {
    	lMinY = iPoint2.iY;
    }
    if (iPoint3.iY < lMinY)
    {
    	lMinY = iPoint3.iY;
    }
    if (iPoint4.iY < lMinY)
    {
    	lMinY = iPoint4.iY;
    }

    aRect.iBr.iX = lMaxX;
    aRect.iBr.iY = lMaxY;
    aRect.iTl.iX = lMinX;
    aRect.iTl.iY = lMinY;

}


// --------------------------------------------------------------------------
// void TSvgFourPointRect::GetTRect(TGfxRectangle2D& aRect)
// ---------------------------------------------------------------------------
void TSvgFourPointRect::GetTRect(TGfxRectangle2D& aRect)
{

	TFloatFixPt lMaxX = iPoint1.iX;

    if (iPoint2.iX > lMaxX)
    {
    	lMaxX = iPoint2.iX;
    }
    if (iPoint3.iX > lMaxX)
    {
    	lMaxX = iPoint3.iX;
    }
    if (iPoint4.iX > lMaxX)
    {
    	lMaxX = iPoint4.iX;
    }

    TFloatFixPt lMaxY = iPoint1.iY;

    if (iPoint2.iY > lMaxY)
    {
    	lMaxY = iPoint2.iY;
    }
    if (iPoint3.iY > lMaxY)
    {
    	lMaxY = iPoint3.iY;
    }
    if (iPoint4.iY > lMaxY)
    {
    	lMaxY = iPoint4.iY;
    }

   	TFloatFixPt lMinX = iPoint1.iX;

    if (iPoint2.iX < lMinX)
    {
    	lMinX = iPoint2.iX;
    }
    if (iPoint3.iX < lMinX)
    {
    	lMinX = iPoint3.iX;
    }
    if (iPoint4.iX < lMinX)
    {
    	lMinX = iPoint4.iX;
    }

    TFloatFixPt lMinY = iPoint1.iY;

    if (iPoint2.iY < lMinY)
    {
    	lMinY = iPoint2.iY;
    }
    if (iPoint3.iY < lMinY)
    {
    	lMinY = iPoint3.iY;
    }
    if (iPoint4.iY < lMinY)
    {
    	lMinY = iPoint4.iY;
    }

    aRect.iX = lMinX;
    aRect.iY = lMinY;
    aRect.iWidth = lMaxX - lMinX;
    aRect.iHeight = lMaxY - lMinY;

}

// --------------------------------------------------------------------------
// void TSvgFourPointRect::Center(TPoint& aCenter)
// ---------------------------------------------------------------------------
void TSvgFourPointRect::Center(TPoint& aCenter)
{
	TFloatFixPt lMaxX = iPoint1.iX;

    if (iPoint2.iX > lMaxX)
    {
    	lMaxX = iPoint2.iX;
    }
    if (iPoint3.iX > lMaxX)
    {
    	lMaxX = iPoint3.iX;
    }
    if (iPoint4.iX > lMaxX)
    {
    	lMaxX = iPoint4.iX;
    }

    TFloatFixPt lMaxY = iPoint1.iY;

    if (iPoint2.iY > lMaxY)
    {
    	lMaxY = iPoint2.iY;
    }
    if (iPoint3.iY > lMaxY)
    {
    	lMaxY = iPoint3.iY;
    }
    if (iPoint4.iY > lMaxY)
    {
    	lMaxY = iPoint4.iY;
    }

   	TFloatFixPt lMinX = iPoint1.iX;

    if (iPoint2.iX < lMinX)
    {
    	lMinX = iPoint2.iX;
    }
    if (iPoint3.iX < lMinX)
    {
    	lMinX = iPoint3.iX;
    }
    if (iPoint4.iX < lMinX)
    {
    	lMinX = iPoint4.iX;
    }

    TFloatFixPt lMinY = iPoint1.iY;

    if (iPoint2.iY < lMinY)
    {
    	lMinY = iPoint2.iY;
    }
    if (iPoint3.iY < lMinY)
    {
    	lMinY = iPoint3.iY;
    }
    if (iPoint4.iY < lMinY)
    {
    	lMinY = iPoint4.iY;
    }

 	aCenter.iX = (( lMaxX - lMinX ) / TFloatFixPt(2) ) + lMinX;
 	aCenter.iY = (( lMaxY - lMinY ) / TFloatFixPt(2) ) + lMinY;
}

// --------------------------------------------------------------------------
// TBool TSvgFourPointRect::Intersects(const MRect& aFourPointRect)
// ---------------------------------------------------------------------------
TBool TSvgFourPointRect::Intersects(const MRect& aFourPointRect)
{
	TGfxRectangle2D rect;
	TGfxRectangle2D thisRect;

	GetTRect(thisRect);
	((TSvgFourPointRect&)aFourPointRect).GetTRect(rect);

    return ( thisRect.Intersects(rect) );
}
