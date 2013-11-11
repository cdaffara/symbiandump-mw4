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
* Description:  SVG Implementation source file
 *
*/



#include <e32base.h>

#include "SVGFitToViewBoxImpl.h"
#include "SVGSvgElementImpl.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFitToViewBoxImpl* CSvgFitToViewBoxImpl::NewL()
    {
    CSvgFitToViewBoxImpl* self = new ( ELeave ) CSvgFitToViewBoxImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFitToViewBoxImpl* CSvgFitToViewBoxImpl::NewLC()
    {
    CSvgFitToViewBoxImpl* self = new ( ELeave ) CSvgFitToViewBoxImpl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFitToViewBoxImpl::~CSvgFitToViewBoxImpl()
    {
    if ( iAspectRatio )
    {
        delete iAspectRatio;
        iAspectRatio = NULL;
	}

    }

// *******************************************************
// From SVG DOM

/// Returns (parameter 1):
///     preserveAspectio attribute value: if defined for this element
///     NULL: if not defined
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFitToViewBoxImpl::GetPreserveAspectRatio( MSvgPreserveAspectRatio*& aAspectRatio )
    {
    aAspectRatio = iAspectRatio;
    }

/// Returns:
///     TRUE: If Viewbox defined
///     FALSE: If Viewbox is not defined
///     aViewBox: Element's viewbox (if defined).
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgFitToViewBoxImpl::GetViewBox( TGfxRectangle2D& aViewBox )
    {
    aViewBox = iViewBox;
    return iViewBoxDefined;
    }

// *******************************************************
// SVG Implementation

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFitToViewBoxImpl::SetPreserveAspectRatioL( TSvgPreserveAspectAlignType aAlign,
                                                    TSvgMeetOrSliceType aMeetSlice )
    {
    iAspectRatio->SetAlign( aAlign );
    iAspectRatio->SetMeetOrSlice( aMeetSlice );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFitToViewBoxImpl::SetViewBoxL( TGfxRectangle2D aViewBox )
    {
    iViewBox = aViewBox;
    iViewBoxDefined = ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgFitToViewBoxImpl::ViewBoxDefined()
    {
    return iViewBoxDefined;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgFitToViewBoxImpl::SetViewBoxL( const TDesC& aAttributeName,
                                        const TDesC& aValue )
    {
    // If this attribute is not viewBox attribute, return immediately.
    _LIT( KViewBox, "viewBox" );
    _LIT( KPreserveAspectRatio, "preserveAspectRatio" );
    _LIT( KNone, "none" );
    _LIT( KXMidYMid, "xMidYMid" );
    _LIT( KXMidYMin, "xMidYMin" );
    _LIT( KXMidYMax, "xMidYMax" );
    _LIT( KXMinYMin, "xMinYMin" );
    _LIT( KXMinYMid, "xMinYMid" );
    _LIT( KXMinYMax, "xMinYMax" );
    _LIT( KXMaxYMin, "xMaxYMin" );
    _LIT( KXMaxYMid, "xMaxYMid" );
    _LIT( KXMaxYMax, "xMaxYMax" );

    _LIT( KMeet, "meet" );
	_LIT( KSlice, "slice" );

    TLex input( aValue );

    if ( aAttributeName == KViewBox )
        {
        TReal32 vBoxX=0;
        TReal32 vBoxY=0;
        TReal32 vBoxW=0;
        TReal32 vBoxH=0;

        input.SkipSpace();
        if (input.Val( vBoxX, '.' ) != KErrNone)
        	 return ETrue;

        if( input.Peek() == ',' )
        	input.Inc();
        input.SkipSpace();
        if( input.Peek() == ',' )
        	input.Inc();

        if (input.Val( vBoxY, '.' ) != KErrNone)
        	 return ETrue;


          if( input.Peek() == ',' )
        	input.Inc();
        input.SkipSpace();
        if( input.Peek() == ',' )
        	input.Inc();

        if (input.Val( vBoxW, '.' ) != KErrNone)
        	 return ETrue;

          if( input.Peek() == ',' )
        	input.Inc();
        input.SkipSpace();
        if( input.Peek() == ',' )
        	input.Inc();

        if (input.Val( vBoxH, '.' ) != KErrNone)
        	  return ETrue;
        //Error Checking
        if (vBoxW < 0)
            vBoxW = 0;
        if (vBoxH < 0)
            vBoxH = 0;
        

        

        TGfxRectangle2D tr = TGfxRectangle2D( vBoxX, vBoxY, vBoxW, vBoxH );
        this->SetViewBoxL( tr );
        return ETrue;
        }
    else if ( aAttributeName == KPreserveAspectRatio )
        {

        input.Mark();
        input.SkipCharacters();
        TChar test = input.Peek();
        TPtrC token = input.MarkedToken();    // then extract token

        if ( token == KNone )
            {
            this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_None,
                                           ESvgMeetOrSlice_Unknown );
            }
        else if ( token == KXMidYMid )
            {

            //check meet and slice parsing
            //AJD need to seperate SetPreserveAspectRatio calls too...
            TPtrC token = input.NextToken();

            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmid,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmid,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmid,
                                               ESvgMeetOrSlice_Meet );
            	}
            }
        else if ( token == KXMidYMin )
            {
            TPtrC token = input.NextToken();    // then extract token

            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmin,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmin,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmin,
                                               ESvgMeetOrSlice_Meet );
            	}
            }
        else if ( token == KXMidYMax )
            {
            TPtrC token = input.NextToken();    // then extract token

            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmax,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmax,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmidYmax,
                                               ESvgMeetOrSlice_Meet );
            	}
            }
        else if ( token == KXMinYMin )
            {
            TPtrC token = input.NextToken();    // then extract token
            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmin,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmin,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmin,
                                               ESvgMeetOrSlice_Meet );
            	}
            }
        else if ( token == KXMinYMid )
            {
            TPtrC token = input.NextToken();    // then extract token
            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmid,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmid,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmid,
                                               ESvgMeetOrSlice_Meet );
            	}
            }
        else if ( token == KXMinYMax )
            {
            TPtrC token = input.NextToken();    // then extract token
            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmax,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmax,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XminYmax,
                                               ESvgMeetOrSlice_Meet );
            	}
            }
        else if ( token == KXMaxYMin )
            {
            TPtrC token = input.NextToken();  // then extract token
            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmin,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmin,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmin,
                                               ESvgMeetOrSlice_Meet );
            	}
            }
        else if ( token == KXMaxYMid )
            {
            TPtrC token = input.NextToken();    // then extract token
            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmid,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmid,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmid,
                                               ESvgMeetOrSlice_Meet );
            	}
            }
        else if ( token == KXMaxYMax )
            {
            TPtrC token = input.NextToken();    // then extract token
            if ( token == KMeet )
                {
                this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmax,
                                               ESvgMeetOrSlice_Meet );
                }
            else if ( token == KSlice )
            	{
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmax,
                                               ESvgMeetOrSlice_Slice );
            	}
            else {
            	this->SetPreserveAspectRatioL( ESvgPreserveAspectRatio_XmaxYmax,
                                               ESvgMeetOrSlice_Meet );
            	}
            }

        return ETrue;
        }
    else
        return EFalse;
    }



/// Sets window-viewport transformation
/// Foley, Van Dam, 2nd ed, p 212
/// Currenly, only SVG element can have viewport.
/// aXYWH: x, y, width, height attributes of the element.
/// aTr: transformations up to element.
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFitToViewBoxImpl::SetWindowViewportTrans( TGfxRectangle2D aXYWH,
                                                   TGfxAffineTransform& aTr,
                                                   TSize aSize )
    {
    TGfxAffineTransform tr;
    TGfxAffineTransform tr2;

	//VIEWPORT NUMBERS
    TFloatFixPt lViewPortX = aXYWH.MinX();
    TFloatFixPt lViewPortY = aXYWH.MinY();
    TFloatFixPt lViewPortWidth = aXYWH.MaxX() - lViewPortX;
    TFloatFixPt lViewPortHeight = aXYWH.MaxY() - lViewPortY;

    tr.SetTransform( 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f );

    /* 1. Translate to origin */
    tr.Translate( lViewPortX, lViewPortY );

    /* 2. Scale */

    TFloatFixPt lViewBoxXmin;
    TFloatFixPt lViewBoxYmin;
    TFloatFixPt lViewBoxWidth;
    TFloatFixPt lViewBoxHeight;

    if ( iViewBoxDefined ) //
        {
        lViewBoxXmin = iViewBox.MinX();
        lViewBoxYmin = iViewBox.MinY();
        lViewBoxWidth = iViewBox.MaxX() - lViewBoxXmin;
        lViewBoxHeight = iViewBox.MaxY() - lViewBoxYmin;
        }
    else
        {
        //this will default viewBox to <svg> element width and height
        lViewBoxXmin = 0;
        lViewBoxYmin = 0;
        lViewBoxWidth = aSize.iWidth;
        lViewBoxHeight = aSize.iHeight;
        }

	#ifdef SVG_FLOAT_BUILD
    if ( lViewBoxWidth == TFloatFixPt(0.0f) || lViewBoxHeight == TFloatFixPt(0.0f) )
	#else
    if ( lViewBoxWidth == TFloatFixPt( 0 , ETrue ) || lViewBoxHeight == TFloatFixPt( 0,ETrue ) )
	#endif
        return;

	TFloatFixPt sx = TFloatFixPt(1.0f);
	TFloatFixPt sy = TFloatFixPt(1.0f);

    sx = lViewPortWidth / lViewBoxWidth;
    sy = lViewPortHeight / lViewBoxHeight;

	#ifdef SVG_FLOAT_BUILD
   if ( sx == TFloatFixPt(0.0f) || sy == TFloatFixPt(0.0f) )
	#else
   if ( sx == TFloatFixPt( 0 , ETrue ) || sy == TFloatFixPt( 0,ETrue ) )
	#endif
        return;

    TSvgPreserveAspectAlignType align = iAspectRatio->GetAlign();
	TSvgMeetOrSliceType meetSlice = iAspectRatio->GetMeetOrSlice();

    TFloatFixPt xtrans = TFloatFixPt( -1.0f ) * lViewBoxXmin;
    TFloatFixPt ytrans = TFloatFixPt( -1.0f ) * lViewBoxYmin;

    switch ( align )
        {
            case ESvgPreserveAspectRatio_None:
            /* Non uniform scaling */
            //none - Do not force uniform scaling.
            //Scale the graphic content of the given element
            //non-uniformly if necessary such that the element's
            //bounding box exactly matches the viewport rectangle.

            //(Note: if <align> is none, then the optional <meetOrSlice> value is ignored.)
            break;

            case ESvgPreserveAspectRatio_XminYmin:
            //Align the <min-x> of the element's viewBox with the smallest X value of the viewport.
            //Align the <min-y> of the element's viewBox with the smallest Y value of the viewport.

            if (meetSlice == ESvgMeetOrSlice_Meet)
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
            else if (meetSlice == ESvgMeetOrSlice_Slice)
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

            case ESvgPreserveAspectRatio_XmidYmin:
            //Align the midpoint X value of the element's viewBox with the midpoint X value of the viewport.
            //Align the <min-y> of the element's viewBox with the smallest Y value of the viewport.
            //Align the <min-x> of the element's viewBox with the smallest X value of the viewport.
            //Align the <min-y> of the element's viewBox with the smallest Y value of the viewport.

            if (meetSlice == ESvgMeetOrSlice_Meet)
            {
                if ( sx > sy )
                {
                    sx = sy;
	#ifdef SVG_FLOAT_BUILD	
                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) * TFloatFixPt(.5f) ) / sx - lViewBoxXmin;
	#else
                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) >> 1 ) / sx - lViewBoxXmin;
	#endif
                }
                else // ( sx < sy )
                {
                    sy = sx;
                    //no change for ytrans...default above
                }
            }
            else if (meetSlice == ESvgMeetOrSlice_Slice)
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
                    xtrans = xtrans/TFloatFixPt(2) - lViewBoxXmin;
                }
            }
            break;

            case ESvgPreserveAspectRatio_XmaxYmin:
            //Align the <min-x>+<width> of the element's viewBox with the maximum X value of the viewport.
            //Align the <min-y> of the element's viewBox with the smallest Y value of the viewport.
            if (meetSlice == ESvgMeetOrSlice_Meet)
            {
                if ( sx > sy )
                {
                    sx = sy;
	
                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) ) / sx - lViewBoxXmin;
                }
                else // ( sx < sy )
                {
                    sy = sx;
                    //no change for ytrans...default above
                }
            }
            else if (meetSlice == ESvgMeetOrSlice_Slice)
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

            case ESvgPreserveAspectRatio_XminYmid:
            //Align the <min-x> of the element's viewBox with the smallest X value of the viewport.
            //Align the midpoint Y value of the element's viewBox with the midpoint Y value of the viewport.
            if (meetSlice == ESvgMeetOrSlice_Meet)
            {
                if ( sx > sy )
                {
                    sx = sy;
                    //no change for xtrans...default above
                }
                else // ( sx < sy )
                {
                    sy = sx;
	#ifdef SVG_FLOAT_BUILD                    
                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) * TFloatFixPt(.5f) ) /sy - lViewBoxYmin;
	#else
                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) >> 1 ) /sy - lViewBoxYmin;
	#endif
                }
            }
            else if (meetSlice == ESvgMeetOrSlice_Slice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx;
                    ytrans = ytrans/TFloatFixPt(2) - lViewBoxYmin;
                }
                else // ( sx < sy )
                {
                    sx = sy;
                }
            }
            break;

            case ESvgPreserveAspectRatio_XmidYmid:
            //(default) case
            //Align the midpoint X value of the element's viewBox with the midpoint X value of the viewport.
            //Align the midpoint Y value of the element's viewBox with the midpoint Y value of the viewport.
            if (meetSlice == ESvgMeetOrSlice_Meet)
            {
                if ( sx > sy )
                {
                    sx = sy;
	#ifdef SVG_FLOAT_BUILD
                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) * TFloatFixPt(.5f) ) / sx - lViewBoxXmin;
	#else
                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) >> 1 ) / sx - lViewBoxXmin;
	#endif
                }
                else if ( sx < sy )
                {
                    sy = sx;
	#ifdef SVG_FLOAT_BUILD
	
                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) * TFloatFixPt(.5f) ) /sy - lViewBoxYmin;
	#else
                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) >> 1 ) /sy - lViewBoxYmin;
	#endif
                }
            }
            else if (meetSlice == ESvgMeetOrSlice_Slice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx;
                    ytrans = ytrans/TFloatFixPt(2) - lViewBoxYmin;
                }
                else // ( sx < sy )
                {
                    sx = sy;
                    xtrans = lViewPortWidth - sx*lViewBoxWidth;
                    xtrans = xtrans/sx;
                    xtrans = xtrans/TFloatFixPt(2) - lViewBoxXmin;
                }
            }
            break;

            case ESvgPreserveAspectRatio_XmaxYmid:
            //Align the <min-x>+<width> of the element's viewBox with the maximum X value of the viewport.
            //Align the midpoint Y value of the element's viewBox with the midpoint Y value of the viewport.
            if (meetSlice == ESvgMeetOrSlice_Meet)
            {
                if ( sx > sy )
                {
                    sx = sy;
                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) ) / sx - lViewBoxXmin;
                }
                else //( sx < sy )
                {
                    sy = sx;
	#ifdef SVG_FLOAT_BUILD

                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) * TFloatFixPt(.5f) ) /sy - lViewBoxYmin;
	#else
                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) >> 1 ) /sy - lViewBoxYmin;
	#endif
                }
            }
            else if (meetSlice == ESvgMeetOrSlice_Slice)
            {
                if ( sx > sy )
                {
                    sy = sx;
                    ytrans = lViewPortHeight - sx*lViewBoxHeight;
                    ytrans = ytrans/sx;
                    ytrans = ytrans/TFloatFixPt(2) - lViewBoxYmin;
                }
                else //( sx < sy )
                {
                    sx = sy;
                    xtrans = lViewPortWidth - sx*lViewBoxWidth;
                    xtrans = xtrans/sx - lViewBoxXmin;
                }
            }
            break;

            case ESvgPreserveAspectRatio_XminYmax:
            //Align the <min-x> of the element's viewBox with the smallest X value of the viewport.
            //Align the <min-y>+<height> of the element's viewBox with the maximum Y value of the viewport.
            if (meetSlice == ESvgMeetOrSlice_Meet)
            {
                if ( sx > sy )
                {
                    sx = sy;
                    //no change for xtrans...default above
                }
                else //( sx < sy )
                {
                    sy = sx;

                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) ) /sy - lViewBoxYmin;
                }
            }
            else if (meetSlice == ESvgMeetOrSlice_Slice)
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

            case ESvgPreserveAspectRatio_XmidYmax:
            //Align the midpoint X value of the element's viewBox with the midpoint X value of the viewport.
            //Align the <min-y>+<height> of the element's viewBox with the maximum Y value of the viewport.
            if (meetSlice == ESvgMeetOrSlice_Meet)
            {
                if ( sx > sy )
                {
                    sx = sy;
	#ifdef SVG_FLOAT_BUILD
	
                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) * TFloatFixPt(.5f) ) / sx - lViewBoxXmin;
	#else
                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) >> 1 ) / sx - lViewBoxXmin;

	#endif
                }
                else //( sx < sy )
                {
                    sy = sx;

                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) ) /sy - lViewBoxYmin;
                }
            }
            else if (meetSlice == ESvgMeetOrSlice_Slice)
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
                    xtrans = xtrans/TFloatFixPt(2) - lViewBoxXmin;
                }
            }
            break;

            case ESvgPreserveAspectRatio_XmaxYmax:
            //Align the <min-x>+<width> of the element's viewBox with the maximum X value of the viewport.
            //Align the <min-y>+<height> of the element's viewBox with the maximum Y value of the viewport.
            if (meetSlice == ESvgMeetOrSlice_Meet)
            {
                if ( sx > sy )
                {
                    sx = sy;

                    xtrans = ( ( TFloatFixPt )
                           ( lViewPortWidth - ( ( TFloatFixPt ) ( lViewBoxWidth / lViewBoxHeight ) * lViewPortHeight ) ) ) / sx - lViewBoxXmin;
                }
                else //( sx < sy )
                {
                    sy = sx;

                    ytrans = ( ( TFloatFixPt )
                           ( lViewPortHeight - ( ( TFloatFixPt ) ( lViewBoxHeight / lViewBoxWidth ) * lViewPortWidth ) ) ) /sy - lViewBoxYmin;
                }
            }
            else if (meetSlice == ESvgMeetOrSlice_Slice)
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

    tr2.SetTransform( sx, 0.0f, 0.0f, sy, 0.0f, 0.0f );
    tr.Concatenate( tr2 );

    /* 3. Translate to viewport origin */
    tr.Translate( xtrans, ytrans );

    //this aTr should be the CTM for the root element
    //and should change when the viewBox is set because that could cause scaling
    aTr.Concatenate( tr );
    }

// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFitToViewBoxImpl::ConstructL()
    {

    //defaults to xMidYMid and Meet
    iAspectRatio = CSvgPreserveAspectRatioImpl::NewL();

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFitToViewBoxImpl::CSvgFitToViewBoxImpl() : iViewBox( 0, 0, 0, 0 ),
                                               iViewBoxDefined( EFalse )

    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFitToViewBoxImpl* CSvgFitToViewBoxImpl::CloneL()
    {
    CSvgFitToViewBoxImpl* retValue = CSvgFitToViewBoxImpl::NewL();

        CleanupStack::PushL(retValue);

    // *** copy iAspectRatio
    if ( this->iAspectRatio != NULL )
        {
        if ( retValue->iAspectRatio != NULL )
            {
                delete retValue->iAspectRatio;
                retValue->iAspectRatio = NULL;
            }
        retValue->iAspectRatio = this->iAspectRatio->CloneL();
        }

    // *** copy other simple instance variables
    retValue->iViewBox = this->iViewBox;
    retValue->iViewBoxDefined = this->iViewBoxDefined;

    CleanupStack::Pop(retValue);

    return retValue;
    }

