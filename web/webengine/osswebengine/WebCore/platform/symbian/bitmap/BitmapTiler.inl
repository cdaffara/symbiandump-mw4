/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#define RGB565TO888(rgb565)             (((rgb565 << 8) & 0xf80000) | ((rgb565 << 5) & 0x00fc00) | ((rgb565 << 3) & 0x000f8))
#define RGBToRGB565(r,g,b)              ((r)<<11 | (g)<<5 | (b))
#define RGBToRGB888(r,g,b)              ( (((r) << 16) & 0xff0000) | (((g) << 8) & 0x00ff00) | ((b) & 0x000ff) )
#define GET_PIX_SCANLINE_GRAY2(p, x)    (((TUint16*)p)[x>>4] & (1<<(x&0x0f)))
#define GET_PIX_SCANLINE_64K(p, x)      RGB565TO888( (TUint32)(p[x]) )
#define DRAW_PIXEL_MASKED(p, c, m)      *p ^= c; *p &= m; *p ^= c;
#define BLEND(c1,c2,a)                  (((a)*((c1)-(c2))>>8)+(c2));
#define RED(c) (((c)>>16)&0xff)
#define GREEN(c) (((c)>>8)&0xff)
#define BLUE(c) ((c)&0xff)
#define RED5(c) (((c)>>11)&0x1f)
#define GREEN6(c) (((c)>>5)&0x3f)
#define BLUE5(c) ((c)&0x1f)

//=============================================================================
// TFastWrapUtil
//=============================================================================
void TFastWrapUtil::NextLine()
    {
    if( ++iDelta.iY == iSrcHeight ) iDelta.iY = 0;    
    iDelta.iX = iOrig.iX;
    }

void TFastWrapUtil::operator+=( TInt aSteps )
    {
    // this is a bit slow, fortunately it is only used when scaling images
    iDelta.iX += aSteps;
    if( iDelta.iX >= iSrcWidth ) iDelta.iX = iDelta.iX % iSrcWidth;   
    }

void TFastWrapUtil::JumpLines( TInt aSteps )
    {
    iDelta.iY += aSteps;
    if( iDelta.iY >= iSrcHeight ) iDelta.iY = iDelta.iY % iSrcHeight;
    iDelta.iX = iOrig.iX;
    }
    
//=============================================================================
// TSrcScanLine
//=============================================================================
void TSrcScanLine::Mix( TUint16* aTarget )
    {
    *aTarget = iSrcLine[iPos];
    }

void TSrcScanLine::Mix( TUint32* aTarget )
    {
    *aTarget = 0xff000000 | GET_PIX_SCANLINE_64K( iSrcLine, iPos );
    }

//=============================================================================
// TSrcScanLineMasked
//=============================================================================
void TSrcScanLineMasked::Mix( TUint16* aTarget )
    {
    TUint16 trgMask = GET_PIX_SCANLINE_GRAY2( iSrcMask, iPos );
    if( trgMask )
        *aTarget = iSrcLine[iPos];
    }

void TSrcScanLineMasked::Mix( TUint32* aTarget )
    {
    TUint32 trgCol = GET_PIX_SCANLINE_64K( iSrcLine, iPos );
    TUint32 trgMask = GET_PIX_SCANLINE_GRAY2( iSrcMask, iPos );
    if( trgMask )
        *aTarget = 0xff000000 | trgCol;
    //trgMask = trgMask ? 0xff000000 : 0xffffffff;    // NOT
    //DRAW_PIXEL_MASKED( aTarget, trgCol, trgMask );
    }


//=============================================================================
// TSrcScanLineAlpha
//=============================================================================
void TSrcScanLineAlpha::Mix( TUint16* aTarget )
    {
    TUint16 trgCol = iSrcLine[iPos];
    TUint16 trgAlpha = (TUint16)( iAlpha[iPos] );
    if( trgAlpha == 0xff )
        *aTarget = trgCol;
    else if( trgAlpha )
        {
        TInt col = *aTarget;
        TInt r = BLEND(RED5(trgCol),RED5(col),trgAlpha);
        TInt g = BLEND(GREEN6(trgCol),GREEN6(col),trgAlpha);
        TInt b = BLEND(BLUE5(trgCol),BLUE5(col),trgAlpha);
        *aTarget = RGBToRGB565(r,g,b); 
        }
    }

void TSrcScanLineAlpha::Mix( TUint32* aTarget )
    {
    TUint32 trgCol = GET_PIX_SCANLINE_64K( iSrcLine, iPos );
    TUint32 trgAlpha = iAlpha[iPos];
    if( trgAlpha == 0xff )
        *aTarget = 0xff000000 | trgCol;
    else if( trgAlpha )
        {
        TUint32 col = *aTarget;
        TUint32 r = BLEND(RED(trgCol),RED(col),trgAlpha);
        TUint32 g = BLEND(GREEN(trgCol),GREEN(col),trgAlpha);
        TUint32 b = BLEND(BLUE(trgCol),BLUE(col),trgAlpha);
        *aTarget = 0xff000000 | RGBToRGB888(r,g,b);
        }
    }

//=============================================================================
// TScanLineTiler<T>::MixLine
//=============================================================================
template <class S, typename T>
void TScanLineTiler<S, T>::MixLine( T& aTarget, S& aSrcLine, TFastWrapUtil& aWrap )
    {
    TInt count = iTrgWidth;
  
    while( count-- )
        {
        aSrcLine.Mix( aTarget );
        ++aTarget;
        ++aWrap;

        // fast wrapping
        const TPoint& pt = *aWrap;
        aSrcLine = pt.iX;
        }
    }

//=============================================================================
// TileBitmap
//=============================================================================
template <class S, typename T>
void TileBitmap( T& aTarget, TInt aStride, CScanLineFetcher& aFetcher, TScanLineTiler<S, T>& aTiler, 
                   TInt aTrgHeight, TInt aSrcWidth, TInt aSrcHeight, const TPoint& aOffset )
    {
    TInt count = aTrgHeight;
    S srcLine;
    
    // fast wraping
    TFastWrapUtil srcWrap( aOffset, aSrcWidth, aSrcHeight );
    while( count-- )
        {
        aFetcher.Fetch( srcLine );
        aTiler.MixLine( aTarget, srcLine, srcWrap );
        
        // next line, wrapped
        aTarget += aStride;
        srcWrap.NextLine();
        const TPoint& p  = *srcWrap;
        aFetcher = p.iY;
        }
    }
    
// END OF FILE    
