/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics Extension Library source file
*
*/



#include "GfxGradientPaint.h"
#include "GfxFloatFixPt.h"

static inline void svgMatrix2x3Identity(SVGMatrix2x3* m)
    {
    TInt i, j;
    for (i=0; i<2; i++)
        for (j=0; j<3; j++)
            m->matrix[i][j] = (i == j) ? KFloatFixOne : KFloatFixZero;
    }

// ==========================================================================
// Common Constructor
// ==========================================================================
// --------------------------------------------------------------------------
// TGfxGradientPaint::TGfxGradientPaint():	iGradientUnits(KobjectBoundingBox),
// ---------------------------------------------------------------------------
 TGfxGradientPaint::TGfxGradientPaint():	iGradientUnits(KobjectBoundingBox),
										iSpreadMethod(KspreadMethodPad)
	{

	svgMatrix2x3Identity(&iGradientTransform);

	}

 TGfxGradientPaint::~TGfxGradientPaint()
	{
	}

// --------------------------------------------------------------------------
//  void TGfxColor::SetFill(VGPaint aFillPaint)
// ---------------------------------------------------------------------------
 void TGfxGradientPaint::SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* Renderer)
	{
	TGfxRectangle2D lBox = aBBox;
	TUint32 flatColor = GetColor();
	TUint32 opacity = (TInt)(aOpacity * TFloatFixPt(255.0f));
//	VGuint fillColor =  ((flatColor&0x000000FF)<<24) |
//				 		((flatColor&0x0000FF00)<<8) |
//				 		((flatColor&0x00FF0000)>>8) |
//				 		opacity;
    CVGRenderer *aVgRenderer=(CVGRenderer*) Renderer;


	

	aVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );	

	aVgRenderer->vgSetColor( aFillPaint, flatColor << 8 | opacity );

	aVgRenderer->vgSetPaint( aFillPaint, VG_FILL_PATH );

	}


// ==========================================================================
// Linear Gradient Constructor
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxLinearGradientPaint::TGfxLinearGradientPaint():TGfxGradientPaint(),
// ---------------------------------------------------------------------------
 TGfxLinearGradientPaint::TGfxLinearGradientPaint():TGfxGradientPaint(),
															iX1(0),
															iY1(0),
															iX2(1),
															iY2(0)
	{
	}

// --------------------------------------------------------------------------
//  void TGfxColor::SetFill(VGPaint aFillPaint)
// ---------------------------------------------------------------------------

 void TGfxLinearGradientPaint::SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* aPseudo)
	{
    const TFloatFixPt KInverse255 = TFloatFixPt( 1.0f / 255.0f );

    CVGRenderer *iVgRenderer=(CVGRenderer*) aPseudo;
	VGint colorRamp = VG_COLOR_RAMP_SPREAD_PAD; // default value
	VGfloat offsetVal = 0.0f;
	VGfloat rVal = 0.0f;
	VGfloat gVal = 0.0f;
	VGfloat bVal = 0.0f;

	VGfloat fX0 = 0.0f;
	VGfloat fY0 = 0.0f;
	VGfloat fX1 = 0.0f;
	VGfloat fY1 = 0.0f;

	iVgRenderer->vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
    //vgLoadIdentity();

	// need to determine the gradient units... determines how the gradient is applied.
	if(iGradientUnits==KobjectBoundingBox)
        {
        /*
        fX0 = (VGfloat)( aBBox.iX + ( aBBox.iWidth * iX1 ) );
        fY0 = (VGfloat)( aBBox.iY + ( aBBox.iHeight* iY1 ) );
        fX1 = (VGfloat)( aBBox.iX + ( aBBox.iWidth * iX2 ) );
        fY1 = (VGfloat)( aBBox.iY + ( aBBox.iHeight* iY2 ) );
        */
        fX0 = (VGfloat)( iX1 );
        fY0 = (VGfloat)( iY1 );
        fX1 = (VGfloat)( iX2 );
        fY1 = (VGfloat)( iY2 );

        VGfloat gradientTrMatrix[] =
            { svgFixedToFloat(iGradientTransform.matrix[0][0]) , svgFixedToFloat(iGradientTransform.matrix[1][0]), 0,
              svgFixedToFloat(iGradientTransform.matrix[0][1]) , svgFixedToFloat(iGradientTransform.matrix[1][1]), 0,
              svgFixedToFloat(iGradientTransform.matrix[0][2]) , svgFixedToFloat(iGradientTransform.matrix[1][2]), 1 };

		VGfloat boundsMatrix[] =
            { svgFixedToFloat(aBBox.iWidth.iValue) , 0, 0,
              0 , svgFixedToFloat(aBBox.iHeight.iValue), 0,
              svgFixedToFloat(aBBox.iX.iValue) , svgFixedToFloat(aBBox.iY.iValue), 1 };

        iVgRenderer->vgLoadMatrix(boundsMatrix);
        iVgRenderer->vgMultMatrix(gradientTrMatrix);

        }
    else //KuserSpaceOnUse
		{
		fX0 = (VGfloat)iX1;
		fY0 = (VGfloat)iY1;
        fX1 = (VGfloat)iX2;
        fY1 = (VGfloat)iY2;
        VGfloat gradientMatrix[] =
            { svgFixedToFloat(iGradientTransform.matrix[0][0]) , svgFixedToFloat(iGradientTransform.matrix[1][0]), 0,
              svgFixedToFloat(iGradientTransform.matrix[0][1]) , svgFixedToFloat(iGradientTransform.matrix[1][1]), 0,
              svgFixedToFloat(iGradientTransform.matrix[0][2]) , svgFixedToFloat(iGradientTransform.matrix[1][2]), 1 };

        iVgRenderer->vgLoadMatrix(gradientMatrix);
		}

	// format { x0, y0, x1, y1}
	VGfloat gradient[4] = { fX0, fY0, fX1, fY1 };

	if (!iStopData)
		return;

	TInt stopCount = iStopData->Count();



	//If no 'stop' elements are defined, then painting shall occur as if 

	//none were specified as the paint style.

	if(0 == stopCount)
	{
		iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
		VGfloat colorRGBA[] = {0.0f, 0.0f, 0.0f, 0.0f};
		iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_COLOR, 4, colorRGBA);
		iVgRenderer->vgSetPaint( aFillPaint, VG_FILL_PATH );
		return ;
	}
	// If one 'stop' is defined, then painting shall occur with the solid
	// color fill using the color defined for that gradient stop. 
	else if(1 == stopCount)
	{
		VGfloat colorRGBA[4];
		TInt col,r,g,b;
		VGfloat alpha;
				
		col = iStopData->operator[](0).iStopColor;
    	alpha = (VGfloat)iStopData->operator[](0).iStopOpacity;
    	r = (col & 0x00ff0000)>>16;
    	g = (col & 0x0000ff00)>>8;
    	b = (col & 0x000000ff);

		colorRGBA[0] = (VGfloat)(TFloatFixPt( r ) * KInverse255);
    	colorRGBA[1] = (VGfloat)(TFloatFixPt( g ) * KInverse255);
    	colorRGBA[2] = (VGfloat)(TFloatFixPt( b ) * KInverse255);
    	colorRGBA[3] = alpha * (VGfloat)aOpacity;
    	
		iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
		iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_COLOR, 4, colorRGBA);
		iVgRenderer->vgSetPaint( aFillPaint, VG_FILL_PATH );
		return ;
	}
	else if(stopCount > 0)
		{
		
		    // Check if X1=X2 and Y1=Y2 , then the painting shall occur with the solid
		    // color fill using the color defined in the last gradient stop.
		    if( ( iX1 == iX2 ) && ( iY1 == iY2 ) && ( iGradientUnits == KuserSpaceOnUse) )
		    {
		        VGfloat colorRGBA[4];
		        TInt col,r,g,b;
		        VGfloat alpha;
				
		        col = iStopData->operator[]( stopCount - 1 ).iStopColor;
    	        alpha = (VGfloat)iStopData->operator[]( stopCount - 1 ).iStopOpacity;
    	        r = (col & 0x00ff0000)>>16;
    	        g = (col & 0x0000ff00)>>8;
    	        b = (col & 0x000000ff);

		        colorRGBA[0] = (VGfloat)(TFloatFixPt( r ) * KInverse255);
    	        colorRGBA[1] = (VGfloat)(TFloatFixPt( g ) * KInverse255);
    	        colorRGBA[2] = (VGfloat)(TFloatFixPt( b ) * KInverse255);
    	        colorRGBA[3] = alpha * (VGfloat)aOpacity;
    	
		        iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
		        iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_COLOR, 4, colorRGBA);
		        iVgRenderer->vgSetPaint( aFillPaint, VG_FILL_PATH );
		        return ;
		     }
        // Using non-ELeave version and checking for NULL to avoid (ELeave) and TRAP
        VGfloat *stops = new VGfloat [(stopCount+2)*5];
        if ( !stops )
            {
            return;
            }

		VGfloat *lclStop = stops;

		TInt i=0;
		
		TInt col,r,g,b;
		VGfloat alpha = 0.0f;
		TInt additionalStopsAdded = 0;
		
		if( iStopData->operator[](0).iOffset != TFloatFixPt(0) )
		{
			//if the first stop didn't have an offset == 0 then we need to force a zero on it
			//for safety in OpenVG and VGR
			*lclStop++ = 0.0f;
        	col = iStopData->operator[](0).iStopColor;
        	alpha = (VGfloat)iStopData->operator[](0).iStopOpacity;
        	r = (col & 0x00ff0000)>>16;
        	g = (col & 0x0000ff00)>>8;
        	b = (col & 0x000000ff);

			rVal = (VGfloat)(TFloatFixPt( r ) * KInverse255);
        	*lclStop++ = rVal;

        	gVal = (VGfloat)(TFloatFixPt( g ) * KInverse255);
        	*lclStop++ = gVal;

        	bVal = (VGfloat)(TFloatFixPt( b ) * KInverse255);
        	*lclStop++ = bVal;

			*lclStop++ = alpha * (VGfloat)aOpacity;
			additionalStopsAdded++;
	    }
	    
	    for( ; i < stopCount; i++)
		    {
	    	offsetVal = (VGfloat)(iStopData->operator[](i).iOffset);
	    	*lclStop++ = offsetVal;

	        col = iStopData->operator[](i).iStopColor;
        	alpha = (VGfloat)iStopData->operator[](i).iStopOpacity;
		    r = (col & 0x00ff0000)>>16;
		    g = (col & 0x0000ff00)>>8;
	        b = (col & 0x000000ff);

            rVal = (VGfloat)(TFloatFixPt( r ) * KInverse255);
            *lclStop++ = rVal;

            gVal = (VGfloat)(TFloatFixPt( g ) * KInverse255);
            *lclStop++ = gVal;

			bVal = (VGfloat)(TFloatFixPt( b ) * KInverse255);
            *lclStop++ = bVal;

		    *lclStop++ = alpha * (VGfloat)aOpacity;
		    }
		    
		if (offsetVal != 1.0f)
		{
			//if the last stop didn't have an offset == 1 then we need to force one on it
			//for safety in OpenVG and VGR
			*lclStop++ = 1.0f;
	    	col = iStopData->operator[](i-1).iStopColor;
			r = (col & 0x00ff0000)>>16;
			g = (col & 0x0000ff00)>>8;
        	b = (col & 0x000000ff);

			rVal = (VGfloat)(TFloatFixPt( r ) * KInverse255);
        	*lclStop++ = rVal;

        	gVal = (VGfloat)(TFloatFixPt( g ) * KInverse255);
        	*lclStop++ = gVal;

        	bVal = (VGfloat)(TFloatFixPt( b ) * KInverse255);
        	*lclStop++ = bVal;

        	*lclStop++ = alpha * (VGfloat)aOpacity;
        	additionalStopsAdded++;
		}
		
		iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_COLOR_RAMP_STOPS, (stopCount+additionalStopsAdded)*5, stops );
		delete [] stops;
		}

    iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT );

	if(iSpreadMethod==KspreadMethodReflect)
		{
		colorRamp = VG_COLOR_RAMP_SPREAD_REFLECT;
		}
	else if(iSpreadMethod==KspreadMethodRepeat)
		{
		colorRamp = VG_COLOR_RAMP_SPREAD_REPEAT;
		}




	iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_COLOR_RAMP_SPREAD_MODE, colorRamp );
	iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_LINEAR_GRADIENT, 4, gradient);
	iVgRenderer->vgSetPaint( aFillPaint, VG_FILL_PATH );
	}

// ==========================================================================
// Radial Gradient Constructor
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxRadialGradientPaint::TGfxRadialGradientPaint():TGfxGradientPaint(),
// ---------------------------------------------------------------------------
 TGfxRadialGradientPaint::TGfxRadialGradientPaint():TGfxGradientPaint(),
															iCx(TReal32(0.5)),
															iCy(TReal32(0.5)),
															iR(TReal32(0.5)),
															iFx( KMAXFLOATFIX ),
															iFy( KMAXFLOATFIX )

//															iFx(0x7fff),
//															iFy(0x7fff)
//															iFx(1e38),
//															iFy(1e38)

	{
	}



// --------------------------------------------------------------------------
//  void TGfxColor::SetFill(VGPaint aFillPaint)
// ---------------------------------------------------------------------------

 void TGfxRadialGradientPaint::SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* aPseudo)
	{
    const TFloatFixPt KInverse255 = TFloatFixPt( 1.0f /  255.0f );
    CVGRenderer *iVgRenderer=(CVGRenderer*) aPseudo;
	VGint colorRamp = VG_COLOR_RAMP_SPREAD_PAD; // default value

	//TGfxRectangle2D lBox = aBBox;
	TFloatFixPt invalid(KMAXFLOATFIX);
	VGfloat offsetVal = 0.0f;
	VGfloat rVal = 0.0f;
	VGfloat gVal = 0.0f;
	VGfloat bVal = 0.0f;
	VGfloat fCx = 0.0f;
	VGfloat fCy = 0.0f;
	VGfloat fR  = 0.0f;
	VGfloat fFx = 0.0f;
	VGfloat fFy = 0.0f;


	iVgRenderer->vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
    //vgLoadIdentity();

	if(iGradientUnits==KobjectBoundingBox)
		{
		fCx = (VGfloat)iCx;
		fCy = (VGfloat)iCy;

		fR = (VGfloat)iR;

		fFx = (VGfloat)iFx;
		if(iFx == invalid)
        	fFx = fCx; //Fx was not set
		else
			fFx = (VGfloat)iFx;

		fFy = (VGfloat)iFy;
		if(iFy == invalid)
        	fFy = fCy; //Fy was not set
    	else
			fFy = (VGfloat)iFy;
	    VGfloat gradientTrMatrix[] =
	        { svgFixedToFloat(iGradientTransform.matrix[0][0]) , svgFixedToFloat(iGradientTransform.matrix[1][0]), 0,
	          svgFixedToFloat(iGradientTransform.matrix[0][1]) , svgFixedToFloat(iGradientTransform.matrix[1][1]), 0,
	          svgFixedToFloat(iGradientTransform.matrix[0][2]) , svgFixedToFloat(iGradientTransform.matrix[1][2]), 1 };

		VGfloat boundsMatrix[] =
	        { svgFixedToFloat(aBBox.iWidth.iValue) , 0, 0,
	          0 , svgFixedToFloat(aBBox.iHeight.iValue), 0,
	          svgFixedToFloat(aBBox.iX.iValue) , svgFixedToFloat(aBBox.iY.iValue), 1 };

		iVgRenderer->vgLoadMatrix(boundsMatrix);
		iVgRenderer->vgMultMatrix(gradientTrMatrix);	
			
		}
	else //if(iGradientUnits==KuserSpaceOnUse)
		{
        // values are user defined coordinates
		fCx = (VGfloat)iCx;
		fCy = (VGfloat)iCy;

		fR = (VGfloat)iR;

		fFx = (VGfloat)iFx;
    	if(iFx == invalid)
	        fFx = fCx; //Fx was not set
    	else
			fFx = (VGfloat)iFx;

		fFy = (VGfloat)iFy;
    	if(iFy == invalid)
	        fFy = fCy; //Fy was not set
	    else
			fFy = (VGfloat)iFy;

        VGfloat gradientMatrix[] =
            { svgFixedToFloat(iGradientTransform.matrix[0][0]) , svgFixedToFloat(iGradientTransform.matrix[1][0]), 0,
              svgFixedToFloat(iGradientTransform.matrix[0][1]) , svgFixedToFloat(iGradientTransform.matrix[1][1]), 0,
              svgFixedToFloat(iGradientTransform.matrix[0][2]) , svgFixedToFloat(iGradientTransform.matrix[1][2]), 1 };
        iVgRenderer->vgLoadMatrix(gradientMatrix);
        }

    // format { cx, cy, fx, fy, r}
	VGfloat gradient[5] = { fCx, fCy, fFx, fFy, fR};

	if (!iStopData)
	return;




	TInt stopCount = iStopData->Count();
	
	//If no 'stop' elements are defined, then painting shall occur as if 
	//none were specified as the paint style.
	if(0 == stopCount)
	{
		iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
		VGfloat colorRGBA[] = {0.0f, 0.0f, 0.0f, 0.0f};
		iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_COLOR, 4, colorRGBA);
		iVgRenderer->vgSetPaint( aFillPaint, VG_FILL_PATH );
		return ;
	}
	// If one 'stop' is defined, then painting shall occur with the solid
	// color fill using the color defined for that gradient stop. 
	else if(1 == stopCount)
	{
		VGfloat colorRGBA[4];
		TInt col,r,g,b;
		VGfloat alpha=0.0f;
				
		col = iStopData->operator[](0).iStopColor;
    	alpha = (VGfloat)iStopData->operator[](0).iStopOpacity;
    	r = (col & 0x00ff0000)>>16;
    	g = (col & 0x0000ff00)>>8;
    	b = (col & 0x000000ff);

		colorRGBA[0] = (VGfloat)(TFloatFixPt( r ) * KInverse255);
    	colorRGBA[1] = (VGfloat)(TFloatFixPt( g ) * KInverse255);
    	colorRGBA[2] = (VGfloat)(TFloatFixPt( b ) * KInverse255);
    	colorRGBA[3] = alpha * (VGfloat)aOpacity;
    	
		iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
		iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_COLOR, 4, colorRGBA);
		iVgRenderer->vgSetPaint( aFillPaint, VG_FILL_PATH );
		return ;
	}
    else if(stopCount > 0)
        {
        // Using non-ELeave version and checking for NULL to avoid (ELeave) and TRAP
        VGfloat *stops = new VGfloat [(stopCount+2)*5];
        if ( !stops )
            {
            return;
            }

        VGfloat *lclStop = stops;

		TInt i=0;
		TInt col,r,g,b;
		VGfloat alpha;
		TInt additionalStopsAdded = 0;
		
		if( iStopData->operator[](0).iOffset != TFloatFixPt(0) )
		{
			//if the first stop didn't have an offset == 0 then we need to force a zero on it
			//for safety in OpenVG and VGR
			*lclStop++ = 0.0f;
	    	col = iStopData->operator[](0).iStopColor;
        	alpha = (VGfloat)iStopData->operator[](0).iStopOpacity;
			r = (col & 0x00ff0000)>>16;
			g = (col & 0x0000ff00)>>8;
        	b = (col & 0x000000ff);

			rVal = (VGfloat)(TFloatFixPt( r ) * KInverse255);
        	*lclStop++ = rVal;

        	gVal = (VGfloat)(TFloatFixPt( g )* KInverse255);
        	*lclStop++ = gVal;

        	bVal = (VGfloat)(TFloatFixPt( b ) * KInverse255);
        	*lclStop++ = bVal;

			*lclStop++ = alpha * (VGfloat)aOpacity;
			additionalStopsAdded++;
			
		}
	    for( ; i < stopCount; i++)
	    {
	    	offsetVal = (VGfloat)(iStopData->operator[](i).iOffset);
	    	*lclStop++ = offsetVal;

	        col = iStopData->operator[](i).iStopColor;
			alpha = (VGfloat)iStopData->operator[](i).iStopOpacity;
		    r = (col & 0x00ff0000)>>16;
		    g = (col & 0x0000ff00)>>8;
            b = (col & 0x000000ff);

            rVal = (VGfloat)(TFloatFixPt( r ) * KInverse255);
            *lclStop++ = rVal;

            gVal = (VGfloat)(TFloatFixPt( g ) * KInverse255);
            *lclStop++ = gVal;

			bVal = (VGfloat)(TFloatFixPt( b ) * KInverse255);
            *lclStop++ = bVal;

		    *lclStop++ = alpha * (VGfloat)aOpacity;
	    }
	    
	    if (offsetVal != 1.0f)
		{
			//if the last stop didn't have an offset == 1 then we need to force one on it
			//for safety in OpenVG and VGR
			*lclStop++ = 1.0f;
	    	col = iStopData->operator[](i-1).iStopColor;
			alpha = (VGfloat)iStopData->operator[](i-1).iStopOpacity;
			r = (col & 0x00ff0000)>>16;
			g = (col & 0x0000ff00)>>8;
        	b = (col & 0x000000ff);

			rVal = (VGfloat)(TFloatFixPt( r ) * KInverse255);
        	*lclStop++ = rVal;

        	gVal = (VGfloat)(TFloatFixPt( g ) * KInverse255);
        	*lclStop++ = gVal;

        	bVal = (VGfloat)(TFloatFixPt( b ) * KInverse255);
        	*lclStop++ = bVal;

			*lclStop++ = alpha * (VGfloat)aOpacity;
			additionalStopsAdded++;
		}
		
		iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_COLOR_RAMP_STOPS, (stopCount+additionalStopsAdded)*5, stops );
		delete [] stops;
		}

    iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT );

	if(iSpreadMethod==KspreadMethodReflect)
		{
		colorRamp = VG_COLOR_RAMP_SPREAD_REFLECT;
		}
	else if(iSpreadMethod==KspreadMethodRepeat)
		{
		colorRamp = VG_COLOR_RAMP_SPREAD_REPEAT;
		}


	iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_COLOR_RAMP_SPREAD_MODE, colorRamp );
	iVgRenderer->vgSetParameterfv( aFillPaint, VG_PAINT_RADIAL_GRADIENT, 5, gradient);
	iVgRenderer->vgSetPaint( aFillPaint, VG_FILL_PATH );
	}
