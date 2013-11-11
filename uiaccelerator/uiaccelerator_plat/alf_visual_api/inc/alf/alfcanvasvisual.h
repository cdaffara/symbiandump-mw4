/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Canvas visual
*
*/



#ifndef C_ALFCANVASVISUAL_H
#define C_ALFCANVASVISUAL_H

#include <alf/alfvisual.h>
#include <alf/alftimedvalue.h>
#include <alf/alfrealline.h>
#include <alf/alfgc.h>
#include <gdi.h>

class CAlfControl;
class CAlfLayout;
class CAlfGc;
class CAlfCanvasVisual;
class CAlfTexture;
class CAlfTextStyle;


/**
 * Provides callback method for getting notifications when there is a need 
 * to update the canvas content. 
 *
 * Implementation of this method should do the drawing using given canvas
 * and its drawing methods.
 *
 * @param aCanvas Canvas visual which is about to be refreshed.
 */
class MAlfCanvasObserver
    {
public:
    virtual void UpdateCanvas(CAlfCanvasVisual& aCanvas) = 0;
    };

/**
 *  Canvas visual can be used to draw custom graphics such as images, texts, 
 *  lines, polygons,...into the display using simple drawing primitives.
 *
 *  Usage:
 *  @code
 *  void CMyCanvasControl::ConstructL()
 *     {
 *     ...    
 *     iCanvasVisual = CAlfCanvasVisual::AddNewL(*this, iLayout);
 *     iCanvasVisual->SetCanvasObserver(this);
 *     UpdateCanvas(*iCanvasVisual);
 *     ...
 *     }
 *   
 *  void CMyCanvasControl::UpdateCanvas(CAlfCanvasVisual& aCanvas)
 *      {
 *      aCanvas.BeginDraw();
 *      aCanvas.SetPenColor(KRgbBlue);
 *      aCanvas.DrawText(_L("Hello world !"), iTextRect);    
 *      aCanvas.EndDraw();
 *      }
 *
 *  @endcode
 * 
 *  @lib alfclient.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CAlfCanvasVisual ) : public CAlfVisual
    {
public:
    
    /**
     * Constructor, which gives ownership to the control.
     *
     * @param aOwnerControl The control
     * @param aParentLayout If given, the parent layout.
     * @return New instance. Ownership NOT transreffed (owned by control)
     */
    IMPORT_C static CAlfCanvasVisual* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfCanvasVisual();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAlfCanvasVisual();
    
    /**
     * Sets canvas observer which is responsible for doing the actual drawing
     * commands into canvas.
     *
     * @param aObserver Canvas observer.
     */
    IMPORT_C void SetCanvasObserver( MAlfCanvasObserver* aObserver );
    
    /**
     * Prepares the canvas for drawing.
     * This method should be called before calling any drawing commands
     */
    IMPORT_C void BeginDraw();

    /**
     * Ends the actual drawing on the canvas.
     * This method should be called after calling last drawing method. 
     *
     */
    IMPORT_C void EndDraw();

    /**
     * Draws an image on the canvas visual. For more flexible image drawing 
     * consider using CAlfImageVisual. 
     *
     * @param aTexture Image Texture
     * @param aDestinationRect Bounding rectangle for the image
     */
    IMPORT_C void DrawImage(const CAlfTexture& aTexture, const TAlfRealRect& aDestinationRect);    
    
    /**
     * Draws a text on the canvas visual. For more flexible text drawing 
     * consider using CAlfTextVisual.  
     *
     * @param aText Text to be displayed
     * @param aDestinationRect Bounding rectangle for the text
     */    
    IMPORT_C void DrawText(const TDesC& aText, const TAlfRealRect& aDestinationRect);  
    
   /**
     * Draws lines on the canvas visual 
     * @param aLines Array of Lines which is split into the number of lines specified in aLineCount
     * @param aLineCount Number of linesl
     */
    IMPORT_C void DrawLines(const TAlfRealLine* aLines, TInt aLineCount);
        
   /**
    * Draws ellipse on the canvas visual 
    * @param aDestinationRect Bounding rectangle for the ellipse
    */
    IMPORT_C void DrawEllipse(const TAlfRealRect& aDestinationRect);
    
   /**
    * Draws points  on the canvas visual 
    * @param aPoints Array of Points to be drawn
    * @param aPointcount Number of Points
    */    
    IMPORT_C void DrawPoints(const TAlfRealPoint* aPoints, TInt aPointCount); 

   /**
    * Draws a polygon on the canvas visual 
    * @param aPoints Array of points defining the polygon
    * @param aPointcount Number of Points
    */
    IMPORT_C void DrawPolygon(const TAlfRealPoint* aPoints, TInt aPointCount);

   /**
    * Draws rectangles on the canvas visual
    * @param aRects Array of rectangles to be drawn
    * @param aRectCount Number of rectangles
    */
    IMPORT_C void DrawRects(const TAlfRealRect* aRects, TInt aRectCount);

   /**
    * Sets the Text style. 
    * Subsequent calls to DrawText will use this Text style
    * @param aTextStyle Text style
    */
    IMPORT_C void SetTextStyle(const CAlfTextStyle& aTextStyle);

   /**
    * Sets the pen color for an object to be drawn
    * Subsequent calls to Draw methods will use this pen color
    * @param 
    */	
    IMPORT_C void SetPenColor(const TRgb& aColor);

   /**
    * Sets the pen width for an object to be drawn
    * Subsequent calls to Draw methods will use this pen width
    * @param 
    */
	IMPORT_C void SetPenWidth(const TReal32& aWidth);

   /**
    * Sets the opacity for an object to be drawn
    * Subsequent calls to Draw methods will use this opacity
    * @param
    */
	IMPORT_C void SetPenOpacity(const TReal32& aOpacity);

   /**
    * Sets the mode for filling a polygon
    * @param aPolygonDrawMode Even-Odd fill or Non-Zero fill
    */
    IMPORT_C void SetPolygonDrawMode(TAlfPolygonDrawMode aPolygonDrawMode);

   /**
    * Sets the allignment of Text to be drawn
    * Subsequent DrawText will draw text with this allignment
    * @param aAlignHorizontal horizontal allignment
    * @param aAlignVertical Vertical allignment
    */
  	IMPORT_C void SetTextAlign(TAlfAlignHorizontal aAlignHorizontal, TAlfAlignVertical aAlignVertical);

   /**
    * Loads identity matrix for canvas draw methods. Note that this does not
    * affect on the visuals transformation that can be accessed using 
    * CAlfVisual::Transfromation().
    */
 	IMPORT_C void LoadIdentity();

   /**
    * Translates the canvas
    * @param aX X-axis offset
    * @param aY Y-axis offset
    * @param aZ Z-axis offset
    */	
    IMPORT_C void Translate(const TReal32& aX, const TReal32& aY, const TReal32& aZ);

   /**
    * Scales the canvas
    * @param aX X-axis offset
    * @param aY Y-axis offset
    * @param aZ Z-axis offset
    */
    IMPORT_C void Scale(const TReal32& aX, const TReal32& aY, const TReal32& aZ);

   /**
    * Rotates the canvas
    * @param aX X-axis offset
    * @param aY Y-axis offset
    * @param aZ Z-axis offset
    */
    IMPORT_C void Rotate(const TReal32& aAngle, const TReal32& aX, const TReal32& aY, const TReal32& aZ);

    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);


protected:

    void SendBuffer( const TPtrC8& aBuffer );

    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

    
private:

    struct TCanvasVisualPrivateData;
    TCanvasVisualPrivateData* iCanvasVisualData;    
    };


#endif // C_ALFCANVASVISUAL_H
