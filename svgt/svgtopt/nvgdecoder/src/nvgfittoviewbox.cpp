/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder source file
 *
*/


#include "nvgfittoviewbox.h"

const TReal KZero  = 0.0;
const TReal KOne   = 1.0;


// ---------------------------------------------------------------------------
// 2 Phase Constructor
// ---------------------------------------------------------------------------
CNvgFitToViewBoxImpl* CNvgFitToViewBoxImpl::NewL()
    {
    CNvgFitToViewBoxImpl* self = new ( ELeave ) CNvgFitToViewBoxImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }



// ---------------------------------------------------------------------------
// 2 Phase Constructor
// ---------------------------------------------------------------------------
CNvgFitToViewBoxImpl* CNvgFitToViewBoxImpl::NewLC()
    {
    CNvgFitToViewBoxImpl* self = new ( ELeave ) CNvgFitToViewBoxImpl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }



// ---------------------------------------------------------------------------
// Constuctor
// ---------------------------------------------------------------------------
CNvgFitToViewBoxImpl::CNvgFitToViewBoxImpl(): iM00(KOne),
                                            iM01(KZero),
                                            iM02(KZero),
                                            iM10(KZero),
                                            iM11(KOne),
                                            iM12(KZero),
                                            iViewBoxDefined(EFalse),
                                            iAlign(ENvgPreserveAspectRatio_XmidYmid),
                                            iMeetSlice(ENvgMeet)
    {
    ivbX    = 0.0;
    ivbY    = 0.0;
    ivbW   = 0.0;
    ivbH    = 0.0;

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CNvgFitToViewBoxImpl::ConstructL()
    {

    }



// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CNvgFitToViewBoxImpl::~CNvgFitToViewBoxImpl()
    {
    
    }



// ---------------------------------------------------------------------------
// Computation of the viewport to viewbox transformation matrix
// ---------------------------------------------------------------------------
void CNvgFitToViewBoxImpl::SetWindowViewportTrans(TRect aViewPort, TSize aSize)
    {
    
    //VIEWPORT NUMBERS
    TReal lViewPortX = aViewPort.iTl.iX;
    TReal lViewPortY = aViewPort.iTl.iY;
    TReal lViewPortWidth = aViewPort.Width();
    TReal lViewPortHeight = aViewPort.Height();
    
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgTranslate(lViewPortX, lViewPortY );

    /* 2. Scale */

    TReal lViewBoxXmin;
    TReal lViewBoxYmin;
    TReal lViewBoxWidth;
    TReal lViewBoxHeight;

    if ( iViewBoxDefined ) 
        {
        lViewBoxXmin = ivbX;
        lViewBoxYmin = ivbY;
        lViewBoxWidth = ivbW;
        lViewBoxHeight = ivbH;
        }
    else
        {
        //this will default viewBox to <svg> element width and height
        lViewBoxXmin = 0;
        lViewBoxYmin = 0;
        lViewBoxWidth = aSize.iWidth;
        lViewBoxHeight = aSize.iHeight;
        }

	
    if ( lViewBoxWidth == 0.0f || lViewBoxHeight == 0.0f )
        {
        return;    
        }
	

	TReal sx = lViewPortWidth / lViewBoxWidth;
	TReal sy = lViewPortHeight / lViewBoxHeight;
	
    if ( sx == 0.0f || sy == 0.0f )
        {
        return;    
        }
    

    
    TReal xtrans = TReal( -1.0f ) * lViewBoxXmin;
    TReal ytrans = TReal( -1.0f ) * lViewBoxYmin;

    switch ( iAlign )
        {
            case ENvgPreserveAspectRatio_None:
            /* Non uniform scaling */
            //none - Do not force uniform scaling.
            //Scale the graphic content of the given element
            //non-uniformly if necessary such that the element's
            //bounding box exactly matches the viewport rectangle.

            //(Note: if <align> is none, then the optional <meetOrSlice> value is ignored.)
            break;

            case ENvgPreserveAspectRatio_XminYmin:
            //Align the <min-x> of the element's viewBox with the smallest X value of the viewport.
            //Align the <min-y> of the element's viewBox with the smallest Y value of the viewport.

            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;
                    //no change for xtrans...default above
                }
                else // ( sx < sy )
                {
                    sy = sx;
                    //no change for ytrans...default above
                }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if (sx > sy)
                {
                    sy = sx;
                }
                else // ( sx < sy )
                {
                    sx = sy;
                }
            }
            break;

            case ENvgPreserveAspectRatio_XmidYmin:
            //Align the midpoint X value of the element's viewBox with the midpoint X value of the viewport.
            //Align the <min-y> of the element's viewBox with the smallest Y value of the viewport.
            //Align the <min-x> of the element's viewBox with the smallest X value of the viewport.
            //Align the <min-y> of the element's viewBox with the smallest Y value of the viewport.

            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;
	                xtrans = ( ( lViewPortWidth - (( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) )\
                           * 0.5 ) / sx - lViewBoxXmin;
	            }
                else // ( sx < sy )
                {
                    sy = sx;
                    //no change for ytrans...default above
                }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                }
                else //( sx < sy )
                {
                    sx = sy;
                    xtrans = lViewPortWidth - sx*lViewBoxWidth;
                    xtrans = xtrans/sx;
                    xtrans = xtrans/2.0 - lViewBoxXmin;
                }
            }
            break;

            case ENvgPreserveAspectRatio_XmaxYmin:
            //Align the <min-x>+<width> of the element's viewBox with the maximum X value of the viewport.
            //Align the <min-y> of the element's viewBox with the smallest Y value of the viewport.
            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;
	
                    xtrans = (( lViewPortWidth - ( ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) ) / sx\
                           - lViewBoxXmin;
                }
                else // ( sx < sy )
                {
                    sy = sx;
                    //no change for ytrans...default above
                }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    //no change for ytrans...default above
                }
                else // ( sx < sy )
                {
                    sx = sy;
                    xtrans = lViewPortWidth - sx*lViewBoxWidth;
                    xtrans = xtrans/sx - lViewBoxXmin;
                }
            }
            break;

            case ENvgPreserveAspectRatio_XminYmid:
            //Align the <min-x> of the element's viewBox with the smallest X value of the viewport.
            //Align the midpoint Y value of the element's viewBox with the midpoint Y value of the viewport.
            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;
                    //no change for xtrans...default above
                }
                else // ( sx < sy )
                {
                    sy = sx;
	                ytrans = ( ( TReal )
                           ( lViewPortHeight - ( ( TReal ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) )\
                           * TReal(.5) ) /sy - lViewBoxYmin;
                }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if ( sx > sy )
                    {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx;
                    ytrans = ytrans/2.0 - lViewBoxYmin;
                    }
                else
                    {
                    sx = sy;
                    }
            }
            break;

            case ENvgPreserveAspectRatio_XmidYmid:
            //(default) case
            //Align the midpoint X value of the element's viewBox with the midpoint X value of the viewport.
            //Align the midpoint Y value of the element's viewBox with the midpoint Y value of the viewport.
            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;
                    xtrans = (( lViewPortWidth - (( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) * \
                            0.5 ) / sx - lViewBoxXmin;

                }
                else if ( sx < sy )
                {
                    sy = sx;
	
                    ytrans = (( lViewPortHeight - (( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) * \
                           0.5 ) /sy - lViewBoxYmin;
	            }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx;
                    ytrans = ytrans/2.0 - lViewBoxYmin;
                }
                else // ( sx < sy )
                {
                    sx = sy;
                    xtrans = lViewPortWidth - sx*lViewBoxWidth;
                    xtrans = xtrans/sx;
                    xtrans = xtrans/2.0 - lViewBoxXmin;
                }
            }
            break;

            case ENvgPreserveAspectRatio_XmaxYmid:
            //Align the <min-x>+<width> of the element's viewBox with the maximum X value of the viewport.
            //Align the midpoint Y value of the element's viewBox with the midpoint Y value of the viewport.
            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;
                    xtrans = (( lViewPortWidth - (( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) ) / sx \
                           - lViewBoxXmin;
                }
                else //( sx < sy )
                {
                    sy = sx;
	
                    ytrans = (( lViewPortHeight - (( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) \
                           * 0.5 ) /sy - lViewBoxYmin;
	            }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx;
                    ytrans = ytrans/2.0 - lViewBoxYmin;
                }
                else //( sx < sy )
                {
                    sx = sy;
                    xtrans = lViewPortWidth - sx*lViewBoxWidth;
                    xtrans = xtrans/sx - lViewBoxXmin;
                }
            }
            break;

            case ENvgPreserveAspectRatio_XminYmax:
            //Align the <min-x> of the element's viewBox with the smallest X value of the viewport.
            //Align the <min-y>+<height> of the element's viewBox with the maximum Y value of the viewport.
            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;
                    //no change for xtrans...default above
                }
                else //( sx < sy )
                {
                    sy = sx;

                    ytrans = (( lViewPortHeight - (( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) ) /sy \
                           - lViewBoxYmin;
                }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx - lViewBoxYmin;
                }
                else //( sx < sy )
                {
                    sx = sy;
                }
            }
            break;

            case ENvgPreserveAspectRatio_XmidYmax:
            //Align the midpoint X value of the element's viewBox with the midpoint X value of the viewport.
            //Align the <min-y>+<height> of the element's viewBox with the maximum Y value of the viewport.
            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;
	
                    xtrans = (( lViewPortWidth - (( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) \
                           * 0.5 ) / sx - lViewBoxXmin;
	            }
                else //( sx < sy )
                {
                    sy = sx;

                    ytrans = (( lViewPortHeight - (( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) ) /sy \
                           - lViewBoxYmin;
                }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx - lViewBoxYmin;
                }
                else //( sx < sy )
                {
                    sx = sy;
                    xtrans = lViewPortWidth - sx*lViewBoxWidth;
                    xtrans = xtrans/sx;
                    xtrans = xtrans/2.0 - lViewBoxXmin;
                }
            }
            break;

            case ENvgPreserveAspectRatio_XmaxYmax:
            //Align the <min-x>+<width> of the element's viewBox with the maximum X value of the viewport.
            //Align the <min-y>+<height> of the element's viewBox with the maximum Y value of the viewport.
            if (iMeetSlice == ENvgMeet)
            {
                if ( sx > sy )
                {
                    sx = sy;

                    xtrans = (( lViewPortWidth - (( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) ) / sx\
                           - lViewBoxXmin;
                }
                else //( sx < sy )
                {
                    sy = sx;

                    ytrans = (( lViewPortHeight - (( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) ) /sy \
                           - lViewBoxYmin;
                }
            }
            else if (iMeetSlice == ENvgSlice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx - lViewBoxYmin;
                }
                else //( sx < sy )
                {
                    sx = sy;
                    xtrans = lViewPortWidth - sx*lViewBoxWidth;
                    xtrans = xtrans/sx - lViewBoxXmin;
                }
            }
            break;

        default:
            break;
        }
    vgScale( sx, sy);

    vgTranslate( xtrans, ytrans );

    }

 void CNvgFitToViewBoxImpl::Concatenate( TReal aM00, TReal aM01, TReal aM02, TReal aM10, TReal aM11, TReal aM12 )
    {
    TReal m0, m1;
    m0  = iM00;
    m1  = iM01;
    iM00 = aM00 * m0 + aM10 * m1;
    iM01 = aM01 * m0 + aM11 * m1;
    iM02 += aM02 * m0 + aM12 * m1;
    m0 = iM10;
    m1 = iM11;
    iM11 = aM01 * m0 + aM11 * m1;
    iM10 = aM00 * m0 + aM10 * m1;
    iM12 += aM02 * m0 + aM12 * m1;
    }
//--------------------------------EndOfFile------------------------------------
