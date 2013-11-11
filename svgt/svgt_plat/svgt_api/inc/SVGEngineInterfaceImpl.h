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


#ifndef SVGENGINEINTERFACEIMPL_H
#define SVGENGINEINTERFACEIMPL_H

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

//does this need to be in a #define
#include <e32cons.h>
#include <w32std.h>
#include <badesca.h>
#include "SVGRendererId.h"
#include "SVGListener.h"

const TInt KSvgEngineUidValue=0x040039CE;
const TUid KSvgEngineUid={KSvgEngineUidValue};

typedef TUint           TRenderingQuality;
const TRenderingQuality KLowNoAA                = 0;
const TRenderingQuality KHighNoAA               = 1;
const TRenderingQuality KLowAA                  = 2;
const TRenderingQuality KHighAA                 = 3;

const TInt KInvalidEnumAttribute = -10000;
const TReal32 KInvalidFloatAttribute = -10000;
#define KSVGColorNone  2
#define KSVGCurrentColor 3
#define KSVGColorInherit 4
#define KSVGAttributeInherit    5
#define __TLV_ // Needed by orbit.

enum TSvgErrorCode
{
    ESvgNoError = 0,
    ESvgFileNotFound,
    ESvgDocumentNotValid,
    ESvgDocumentNotAvailable,
    ESvgNoMemory,
    ESvgDiskFull,
    ESvgUnknown,
    ESvgMissingRequiredAttribute,
    ESvgInvalidAttributeValue,
    ESvgbFileNotValid,
    ESvgDRMFailure,
    ESvgThumbNailRestriction
};

enum TSvgPreserveAspectAlignType
    {
    ESvgPreserveAspectRatio_None,
    ESvgPreserveAspectRatio_Unknown,
    ESvgPreserveAspectRatio_XmaxYmax,
    ESvgPreserveAspectRatio_XmaxYmid,
    ESvgPreserveAspectRatio_XmaxYmin,
    ESvgPreserveAspectRatio_XmidYmax,
    ESvgPreserveAspectRatio_XmidYmid,	//default
    ESvgPreserveAspectRatio_XmidYmin,
    ESvgPreserveAspectRatio_XminYmax,
    ESvgPreserveAspectRatio_XminYmid,
    ESvgPreserveAspectRatio_XminYmin
    };

enum TSvgMeetOrSliceType
    {
    ESvgMeetOrSlice_Meet,
    ESvgMeetOrSlice_Slice,
    ESvgMeetOrSlice_Unknown
    };	

class       MSvgHyperlinkListener;
class       MSvgTextAreaListener;
class       MSvgTextListener;
class       MSvgAnimationListener;
class     MSvgListener;
class       CGfxGeneralPath;
class       CFbsBitmap;
class       CSvgEngineImpl;
//class     CSvgGcStack;
class       CSvgElementImpl;
class       MSvgEventReceiver;
class       MSvgRequestObserver;
class       CSvgTimer;
class       MXmlElement;
class       CSvgDocumentImpl;
class       CXmlElementImpl;
class       CSvgErrorImpl;
class       CSvgTextElementImpl;

class 		CSvgBitmapFontProvider;

class       CSvgtBitmap;

class MRect
    {
    public:

        /**
         * Get the four points for a rectangle
         *
         * @since 1.0
         * @return : void
         */
        virtual void         GetPoints(TPoint& aPoint1, TPoint& aPoint2, TPoint& aPoint3, TPoint& aPoint4) const = 0;

        virtual TBool        Intersects( const MRect& aRect ) = 0;

        virtual void         Center( TPoint& aCenter ) = 0;

    };


class MSvgError
    {
    public:

        /**
         * Determine wether an error is indicated by this object.
         *
         * @since 1.0
         * @return : ETrue, if ErrorCode() != ESvgNoError
         */
        virtual TBool         HasError() = 0;

        /**
         * Determine wether an error is only a warning.
         * This should be a state when the svg may be display, despite
         * a conflict.
         *
         * @since 1.0
         * @return ETrue if HasError() is only a warning.
         */
        virtual TBool         IsWarning() = 0;

        /**
         * Get the error code contained by this object.
         *
         * @since 1.0
         * @return : the TSvgErrorCode value
         */
        virtual TSvgErrorCode ErrorCode() = 0;

        /**
         * Get the error description string for this object.
         *
         * @since 1.0
         * @return : error description string.
         */
        virtual TDesC&        Description() = 0;

        /**
         * Get the system error code contained by this object.  For example,
         * If ErrorCode() is ESvgDRMFailure, SystemErrorCode() will contain
         * the error-code returned by the system for a drm-failure.
         *
         * @since 1.0
         * @return : the TSvgErrorCode value
         */
        virtual TInt SystemErrorCode() = 0;
    };

/**
 * This class implements the interface for a SVGT Engine, providing methods
 * to load svg contents and manipulate the output, such as zooming, panning
 * and rotating the display.
 */
class CSvgEngineInterfaceImpl : public CBase
    {
    public:

        IMPORT_C MSvgError* CSvgEngineInterfaceImpl::SaveSvgDom(TInt aHandle, const TDesC& aFileName, TInt aEngine = NULL);

        /**
         * Two phase construction
         */
        // April 27th : Add thrid new parameter for specifying the Text height
        IMPORT_C static CSvgEngineInterfaceImpl* NewL( CFbsBitmap* aFrameBuffer,
                                                  MSvgRequestObserver* aReqObserver,  TFontSpec& aFontSpec  );
        IMPORT_C static CSvgEngineInterfaceImpl* NewL( CFbsBitmap* aFrameBuffer,
                                                  MSvgRequestObserver* aReqObserver,  TFontSpec& aFontSpec ,SVGRendererId aRendererType );

        /**
         * Create a new Svg Engine interface.
         *
         * @since 1.0
         * @param : aFrameBuffer -- bitmap to draw resulting svg image.
         * @param : aReqObserver -- interface for callbacks to retrieve info
         *                          only client can provide, such as opening files.
         * @param : aFontSpec -- Font spec to use for text drawing.
         * @return : none
         */
        IMPORT_C static CSvgEngineInterfaceImpl* NewLC( CFbsBitmap* aFrameBuffer,
                                                   MSvgRequestObserver* aReqObserver,  TFontSpec& aFontSpec  );
        // NGA:: overloading NewLC to include a renderer selector parameter
        IMPORT_C static CSvgEngineInterfaceImpl* NewLC( CFbsBitmap* aFrameBuffer,
                                                   MSvgRequestObserver* aReqObserver,  TFontSpec& aFontSpec ,SVGRendererId aRendererType );

       /**
         * Added for getting the NVG-TLV from S60SVGTEngine in QT S60 baseport.
         * @since 1.0
         * @return : Pointer to the tlv data
         */
                
        IMPORT_C const TPtrC8 TLVEncodedData() const;
       
        /**
         * Svg Engine interface Destructor.
         *
         * @since 1.0
         * @param : None
         * @return : none
         */
        IMPORT_C                            ~CSvgEngineInterfaceImpl();

        /**
         * Load a svg document from a file.  An error is return if one is encountered.
         * Some errors may be more like warnings, which may allow the svg content
         * to display nevertheless.
         *
         * @since 1.0
         * @param aFileName -- Name of svg file
         * @return : Error code if any
         */
        IMPORT_C MSvgError*                  Load( const TDesC& aFileName );

        /**
         * Load a svg document from a byte array.  An error is return if one is encountered.
         * Some errors may be more like warnings, which may allow the svg content
         * to display nevertheless.
         *
         * @since 1.0
         * @param  aByteData -- Byte array of svg document.
         * @return : Error code if any
         */
        IMPORT_C MSvgError*                  Load( const TDesC8& aByteData );

        /**
         * Load a svg document from from the RFile Handle passed.  An error is return if one is encountered.
         * Some errors may be more like warnings, which may allow the svg content
         * to display nevertheless.
         *
         * @since 1.0
         * @param  aFileHandle -- RFile handle of svg document.
         * @return : Error code if any
        **/
        IMPORT_C MSvgError*                  Load( RFile& aFileHandle );

        /**
         * Destroy the currently "loaded" svg document.  Prepared documents,
         * must be destroy through DeleteDom, or the engine will destroy them
         * in its destructor function.
         *
         * @since 1.0
         * @param : none
         * @return : none
         */
        IMPORT_C void                       Destroy();


        /**
         * Request the SVG Engine to begin an animation.
         *
         * @since 1.0
         * @param : aEngine -- NULL to start internal svg-engine
         * @param : aIsMainThread -- EFalse to start engine asynchronously,
         * defaulted to EFalse.
         * @return : none
         */
        IMPORT_C void                       Start( CSvgEngineImpl* aEngine = NULL,
                                                   TBool aIsMainThread = ETrue );

        /**
         * Request the SVG Engine to begin an animation. 
         * This method returns an error code.
         * @since 1.0
         * @param : MSvgError*& -- Pointer to Error object ,
         * @param : aEngine -- NULL to start internal svg-engine,
         * @return : none
         */
         
         IMPORT_C void                        Start( MSvgError*& aError,
                                                    CSvgEngineImpl* aEngine = NULL
                                                   );
        /**
         * Request the SVG Engine to stop an animation.
         *
         * @since 1.0
         * @param : none
         * @return : none
         */
        IMPORT_C void                       Stop( CSvgEngineImpl* aEngine = NULL );

        /**
         * Request the SVG Engine to pause an animation.
         *
         * @since Series 60 3.0
         * @param : none
         * @return : none
         */
        IMPORT_C void                       Pause( CSvgEngineImpl* aEngine = NULL );

        /**
         * Request the SVG Engine to resume an animation.
         *
         * @since 1.0
         * @param : none
         * @return : none
         */
        IMPORT_C void                       Resume( CSvgEngineImpl* aEngine = NULL );

        /**
         * Get the animation duration of a svg content, in milliseconds,
         * for non-indefinite animations.
         *
         * @since 1.0
         * @param
         * @return the animation duration.
         */
        IMPORT_C TUint32                    Duration( TInt aEngine = NULL );

        /**
         * Reset the view to the default values.  Redraw() must be called to
         * update the output image.
         *
         * @since 1.0
         * @param
         * @return the animation duration.
         */
        IMPORT_C void                       OriginalView( TInt aEngine = NULL );


        /**
         * Request the SVG Engine to zoom-in on the content given the zoom factor.
         * The value range is greater than zero.  The current zoom factor is
         * multiplied with the given value to yield the final result.  Redraw()
         * must be called to update the output image.
         *
         * @since 1.0
         * @param : aScaleFactor -- A value greater than zero.
         * @return
         */
        IMPORT_C void                       Zoom( TReal32 aScaleFactor, TInt aEngine = NULL ) __SOFTFP;

        /**
         * Shift the center of the content in both x and y coordinates.  The
         * values are in user-coordinate values and may be negative.  Redraw()
         * must be called to update the output image.
         *
         * @since 1.0
         * @param : aX -- Number of pixels to pan left (negative) or right.
         * @param : aY -- Number of pixels to pan up (negative) or down.
         * @return
         */
        IMPORT_C void                       Pan( TInt aX, TInt aY, TInt aEngine = NULL );

       /**
         * Shift the center of the content in both x and y coordinates.  The
         * values are in user-coordinate values and may be negative.
         *
         * @since 1.0
         * @param : aX -- Number of pixels to pan left (negative) or right.
         * @param : aY -- Number of pixels to pan up (negative) or down.
         * @return TBool
         */
        IMPORT_C TBool                      IsPanPossible( TInt aX, TInt aY, TInt aEngine = NULL );

       /**
         *  Checks the panning possibility in four directions. A efficient
	 *  API for clients which want to show 4 way panning indicators.
	 *  It is efficient for these clients to get the all four direction
	 *  information at one go. Saves rendering time significantly per
	 *  frame.
	 *
         * @since 1.0
         * @param : left: would contain result of whether panning of one pixel
	 * 	possible to left.
         * @param : right: would contain result of whether panning of one pixel
	 * 	possible to right.
         * @param : up: would contain result of whether panning of one pixel
	 * 	possible to up.
         * @param : down: would contain result of whether panning of one pixel
	 * 	possible to down.
         * @param : 
         * @return 
         */
        IMPORT_C void                      IsPanPossibleFourWay
    ( TBool& left, TBool& right, TBool& up, TBool& down, TInt aEngine = NULL);

        /**
         * Rotate the content about the given point by the given angle.
         * The point is in user-coordinates.  Redraw() must be called to
         * update the output image.
         *
         * @since 1.0
         * @param : aAngle -- Number of gradients to rotate.
         * @param : aX -- X center of rotation
         * @param : aY -- Y center of rotation.
         * @return
         */
        IMPORT_C void                       Rotate( TReal32 aAngle,
                                                    TInt aX,
                                                    TInt aY,
                                                    TInt aEngine = NULL ) __SOFTFP;

        /**
         * Notify the SVG Engine that a "mouse down" event.  The engine will
         * initiate mouse-related events, such as links, if appropriate.
         *
         * @since 1.0
         * @param : aX -- x coordinate of mouse event.
         * @param : aY -- y coordinate of mouse event.
         * @return
         */
        IMPORT_C TInt                       MouseDown( TInt aX, TInt aY, TInt aEngine = NULL );

        /**
         * Notify the SVG Engine that a "mouse up" event.  The engine will
         * initiate mouse-related events, such as links, if appropriate.
         *
         * @since 1.0
         * @param : aX -- x coordinate of mouse event.
         * @param : aY -- y coordinate of mouse event.
         * @return
         */
        IMPORT_C void                       MouseUp( TInt aX, TInt aY, TInt aEngine = NULL );

        /**
         * Notify the SVG Engine that a "mouse move" event.  The engine will
         * initiate mouse-related events, such as links, if appropriate.
         *
         * @since 1.0
         * @param : aX -- x coordinate of mouse event.
         * @param : aY -- y coordinate of mouse event.
         * @return
         */
        IMPORT_C void                       MouseMove( TInt aX, TInt aY, TInt aEngine = NULL );

        /**
         * Notify the SVG Engine that a "key press" event.  The engine will
         * initiate key-related events if appropriate.
         * @since 1.0
         * @param : aKeyCode -- key code of key event.
         * @return
         */

        IMPORT_C void                       KeyPress( const TKeyEvent& aKeyEvent, TInt aEngine = NULL );

        /**
         * Retrieve the rendering quality setting.  See TRenderingQuality
         * for possible values.
         *
         * @since 1.0
         * @param
         * @return
         */
        IMPORT_C TRenderingQuality          GetRenderQuality( );

        /**
         * Set the rendering quality.  Low-level graphics is always set
         * to high -- no effect from this call.  Redraw() must be called to
         * update the output image.
         *
         * @since 1.0
         * @param : aQualityLevel -- rendering quality
         * @return
         */
        IMPORT_C void                       SetRenderQuality( TRenderingQuality aQualityLevel, TInt aEngine = NULL );

        /**
         * Retrieve the reference to the svg document object.
         *
         * @since 1.0
         * @param none
         * @return current document that was Loaded or Prepare/UseDom.
         */

         //DEPRECATED API...
        IMPORT_C CSvgDocumentImpl*          SvgDocument();

        /**
         * Set the minimum delay between each frame, in milliseconds.
         * Frames per Second = 1000 / aFrameDelay
         *
         * The default/minimum value is 67 milliseconds (15 fps).
         *
         * @since 1.0
         * @param : aFrameDelay -- animation frame duration.
         * @return none
         */
        IMPORT_C void                       SetAnimFrameDuration( TUint aAFDur, TInt aEngine = NULL );

        /**
         * Request for updating the off-screen image buffer with the
         * svg content.
         *
         * @since 1.0
         * @param aIsMainThread -- indicate whether this redraw request
         * is called in the main thread (executed immediately).
         * Otherwise, this request is to be executed asynchronously.
         * 'Main thread' used here means the thread which instantiated
         * this CSvgEngineInterfaceImpl object.
         * @return none
         */
        IMPORT_C void                       Redraw( TBool aIsMainThread = ETrue, TInt aEngine = NULL );

        /**
         * Switch the debugging mode on/off.
         *
         * @since 1.0
         * @param none
         * @return none
         */
        IMPORT_C void                       SwitchDebugInfo( TInt aEngine = NULL );

        /**
         * This is the integer version of ContentDimensions.
         * 1) Return the size of the <svg> width/height if given in non-percentage.
         *
         * 2) If <svg> width/height are given as percentages:
         *    A) If viewbox attribute is specified, return the viewbox width/height times the <svg> width/height percentages.
         *    B) Else return the content bounding-box size times times the <svg> width/height percentages.
         *
         * @since 1.0
         * @param none
         * @return TSize
         */
        IMPORT_C TSize                      ContentDimensions( TInt aEngine = NULL );

        /**
         * This is the floating-point version of ContentDimensions.
         * 1) Return the size of the <svg> width/height if given in non-percentage.
         *
         * 2) If <svg> width/height are given as percentages:
         *    A) If viewbox attribute is specified, return the viewbox width/height times the <svg> width/height percentages.
         *    B) Else return the content bounding-box size times times the <svg> width/height percentages.
         *
         * @since 1.0
         * @param none
         * @return
         */
        IMPORT_C void                       ContentDimensions( TReal32& aWidth, TReal32& aHeight, TInt aEngine = NULL ) __SOFTFP;

        /**
         * Retrieve the content size in user-coordinates if specified as
         * percentage.
         *
         * @since 3.1
         * @param none
         * @return TSize
         */
        IMPORT_C TSize                      ContentDimensionsInPercentage( TInt aEngine = NULL );


        /**
         * Set focus coordinates to the next focusable object.
         *
         * @since 1.0
         * @param none
         * @return rectangle area having the focus on an object.
         */
        IMPORT_C TRect FocusNext();

        /**
         * Set focus coordinates to the previous focusable object.
         *
         * @since 1.0
         * @param none
         * @return rectangle area having the focus on the previous object.
         */
        IMPORT_C TRect FocusPrevious();

        /**
         * Activate currently focussed object.
         *
         * @since 1.0
         * @param none
         * @return none
         */
        IMPORT_C void ActivateObjectInFocus();

        /**
         * Get the current animation frame time, in milliseconds.
         *
         * @since 1.0
         * @param none
         * @return current frame time.
         */
        IMPORT_C TInt32 MediaTime( TInt aEngine = NULL );

        /**
         * Set the current animation frame time, in millseconds.
         *
         * @since 1.0
         * @param aTime -- time to forward/rewind to.
         * @return none
         */
        IMPORT_C void SetMediaTime( TInt32 aTime, TInt aEngine = NULL );
        /**
         * Returns the Size of Viewport
         *
         * @since 1.0
         * @param none
         * @return viewport size
         */
        IMPORT_C TSize Size( TInt aEngine = NULL );

         /**
         * Returns the Position of Viewport
         *
         * @since 1.0
         * @param none
         * @return
         */
        IMPORT_C TPoint Position( TInt aEngine = NULL ) ;

        /**
         * Reset the focus list
         * This happens in the event of new additions to EventReceiver list and/or
         * sorting of this list
         *
         * @since 1.0
         * @param aIndex - index of object to reset focus.
         * @return
         */
        void ResetFocusIndex(TInt32 aIndex);

        /**
         * Generate mask bitmap from the current frame
         *
         * @since 1.0
         * @param aMask - bitmap to hold masking info.
         * @return none
         */
        IMPORT_C void GenerateMask( CFbsBitmap* aMask, TInt aEngine = NULL );

        IMPORT_C void GenerateMask( CSvgtBitmap* aMask, TInt aEngine = NULL );

        /**
         * Set the color for clearing background.
         *
         * @since 1.0
         * @param aRGBA8888Color - 32-bit color value
         * @return none
         */
        IMPORT_C void SetBackgroundColor( TUint32 aRGBA8888Color, CSvgEngineImpl* aEngine = NULL );

        /**
         * Returns SVG Engine State
         *
         * @since 1.0
         * @param none
         * @return 0 == SVG Running and 1 == SVG Paused
         */

        IMPORT_C TInt CurrentState( TInt aEngine = NULL );

        /**
         * Returns SVG Engine Pan Position
         *
         * @since 1.0
         * @param
         * @return TPoint
         */
        IMPORT_C TPoint PanPosition( TInt aEngine = NULL );

        /**
         * This method is a special case Constructor method used for polymorphic
         * DLL loading, which enforces this method to be public.
         *
         * @since 1.0
         * @return
         */
        IMPORT_C           CSvgEngineInterfaceImpl();

        /**
         * Replay the Current Animation.
         *
         * @since 1.0
         * @param :
         * @return : MSvgError* Error object specifying  the error occured during operation
         */
         IMPORT_C MSvgError* Replay( TInt aEngine = NULL );

         /**Added for DOM Caching API changes*/

        /**
         * Parses and Prepares DOM for given SVG or SVGB file.
         * Provides a Handle to the created DOM.  A prepared DOM may be
         * delete with DeleteDom or when the engine is deleted.
         *
         * @param aFileName: the name of the file  to be parsed
         * @param aHandle:  Handle to the created DOM.
         * @return: MSvgError* Error object specifying  the error occured during operation
         */
         IMPORT_C MSvgError* PrepareDom(const TDesC& aFileName, TInt& aHandle, TInt aEngine = NULL);

        /**
         * Parses and Prepares DOM for both svg and svg binary data.
         * Provides a Handle to the created DOM.  A prepared DOM may be
         * delete with DeleteDom or when the engine is deleted.
         *
         * @param aByteData: Svg/SvgBinary data.
         * @param aHandle:  Handle to the created DOM.
         * @return: MSvgError* Error object specifying  the error occured during operation
         */
         IMPORT_C MSvgError* PrepareDom(const TDesC8& aByteData, TInt& aHandle, TInt aEngine = NULL);

        /**
         * Parses and Prepares DOM for given SVG or SVGB file.
         * Provides a Handle to the created DOM.  A prepared DOM may be
         * delete with DeleteDom or when the engine is deleted.
         *
         * @param : aFileHandle -- File handle
         * @param : aHandle -- Handle to a DOM.
         * @return: MSvgError* Error object specifying  the error occured during operation
         */
         IMPORT_C MSvgError* PrepareDom( RFile& aFileHandle, TInt& aHandle, TInt aEngine = NULL );
         
        /**
         * Defines view box for SVG content associated with
         * handle if not set.
         */


         IMPORT_C void ChooseViewBoxIfNotSet(TInt aDomHandle);
        /**
         * Renders the DOM tree associated with the Handle (UseDom).
         *
         * @param : aHandle -- Handle to a DOM.
         * @param : aBitmap -- Bitmap to draw DOM content.
         *@param : aMaskBuffer - Buffer for mask (alpha values) of framebuffer result (optional).
         * @return: MSvgError* Error object specifying  the error occured during operation
         */
         IMPORT_C MSvgError* RenderDom(TInt aHandle, CFbsBitmap* aBitmap, CFbsBitmap* aMaskBuffer = NULL);

        /**
         * Deletes the DOM tree associated with the Handle.
         *
         * @param : aHandle -- Handle to a DOM.
         * @return: MSvgError* Error object specifying  the error occured during operation
         */
         IMPORT_C MSvgError* DeleteDom( TInt aHandle );

        /**
         * Enable/Disable DRM.
         *
         * @param : aEnable -- Flag to enable/distable DRM.
         * @return: none
         */
         IMPORT_C void SetDRMMode(TBool aEnable, TInt aEngine = NULL);

        /**
         * initialization of the engine according to a particular cached DOM tree.
         *
         *
         *@param  : aHandle -- Handle to DOM Tree.
         *@param : aRenderBuffer - Buffer for drawing the DOM Tree.
         *@param : aMaskBuffer - Buffer for mask (alpha values) of framebuffer result (optional).
         *@param : aEngine - SvgEngine handle to manager DOM (optional).
         *@return MSvgError* Error object specifying the error occured.
         */
         IMPORT_C MSvgError* UseDom(TInt aHandle, CFbsBitmap* aRenderBuffer, CFbsBitmap* aMaskBuffer = NULL, TInt aEngine = NULL);

         IMPORT_C MSvgError* UseDom(TInt aHandle,CFbsBitmap* aRenderBuffer, CFbsBitmap* aMaskBuffer,TSize aRenderBufferSize,TDisplayMode aRenderDisplayMode,TDisplayMode aMaskDisplayMode,TInt aEngine = NULL);   
        /**
         * Set the SVG dimensions to the root buffer size.
         *
         *@param aWidth - width to set root size.
         *@param aHeight - height to set root size.
         *@return  none
         */
          IMPORT_C void SetSvgDimensionToFrameBuffer(TUint aWidth, TUint aHeight, TInt aEngine = NULL);

        /**
         * Return the scaled bounding box for the whole svg document.
         *
         *@param none
         *@return the scaled bounding box info.
         */
          IMPORT_C TRect GetSvgBoundingBox( TInt aEngine = NULL );

        /**
         * Check for interactive/non-interactive content.
         *
         *@param aDocument - Document Pointer
         *@return wether document has interactive elements.
         */
         IMPORT_C TBool IsContentInteractive( CSvgDocumentImpl* aDocument, TInt aEngine = NULL );

        /**
         * Update the path information with the given element
         *
         *@param   hPath - path handle
         *@param   hElement - element handle
         *@return  none
         */
         IMPORT_C void UpdatePath(TInt hPath, CSvgElementImpl* hElement);

        /**
         * Returns the size (number of external elements) in the given document
         *
         *@param   aDocument - Document Pointer
         *@return  number of external elements
         */
         IMPORT_C TInt GetExternalListSize(CSvgDocumentImpl* aDocument);

        /**
         * Given an index number and a document handle returns an external resource uri associated
         * with an element
         *@param   aDocument - Document Pointer index - index in external list
         *@return  URI
         */
         IMPORT_C void GetExternalListItemL(
										CSvgDocumentImpl* aDocument, TInt aIndex, TPtrC16& aUri);

        /**
         * Given a document handle and a url this call attaches
         * the given data (ldata) to every element that references that url
         *
         *@param   aDocument - Document Pointer
         *@param   lUrl- uri string that is an external resource to be retrieved
         *@param   ldata - byte array containing the data that that external resource recieved
         *@return  none
         *         NULL if image-decoding is not initiated.
         */
         IMPORT_C CSvgElementImpl* AddExternalData( CSvgDocumentImpl* aDocument,
                                                    const TDesC& aUri,
                                                    const TDesC8& aData,
                                                    TBool aMakeCopy = EFalse,
                                                    TInt aSize = 0,
                                                    TInt aEngine = NULL );

         /**
         * Create an instance of a CSvgEngineImpl.
         *
         *@param  : None
         *@return Pointer to CSvgEngineImpl object if creation is succesful, otherwise
         *        the function leaves.
         */
         IMPORT_C CSvgEngineImpl* SvgEngineNewL();

        /**
         * Intialize the given SVG Engine.
         *
         *@param  : aEngine -- SVG Engine.
         *@return None
         */
         IMPORT_C void StartEngine( CSvgEngineImpl* aEngine );
         IMPORT_C void RenderFrame( CSvgEngineImpl* aEngine, TUint aCurrentTime );

         IMPORT_C void ViewportInit( CSvgDocumentImpl* aDocumentHandle );

        /**
         * Set the GDI context for the given SVG Engine.
         *
         *@param  : aEngine -- SVG Engine.
         *@param  : aFrameBuffer -- Bitmap to become gdi context.
         *@return None
         */
         IMPORT_C void SetGdiContextL( CSvgEngineImpl* aEngine, CFbsBitmap* aFrameBuffer );

         IMPORT_C void SetGdiContextL( CSvgEngineImpl* aEngine, CSvgtBitmap* aFrameBuffer );
         /**
         * Associate the given document with the given engine.
         *
         *@param  : aEngine -- SVG Engine.
         *@param  : aDocument -- SVG Document
         *@return None
         */
         IMPORT_C void SetDocument( CSvgEngineImpl* aEngine, CSvgDocumentImpl* aDocument );
         void SvgElementAssignDocument( CSvgElementImpl* aElement, CSvgDocumentImpl* aDoc );

         /**
         * Create an empty instance of a SVG document.
         *
         *@param  : aEngine -- SVG Engine.
         *@param  : aDocument -- SVG Document
         *@return Svg Document if successful, otherwise the function leaves.
         */
         IMPORT_C CSvgDocumentImpl* SvgDocumentNewL();

         /**
         * Fill a SVG document by parsing the given byte array.
         *
         *@param  : aEngine -- SVG Engine.
         *@param  : aDocument -- SVG Document
         *@return None if successful, otherwise the function leaves.
         */
         IMPORT_C void FillDocumentL( CSvgDocumentImpl* aDocument, const TDesC16& aByteData );

         /**
         * Destroy the given SVG document
         *
         *@param  : aDocument -- SVG document.
         *@return None .
         */
         IMPORT_C void DestroyDocument( CSvgDocumentImpl* aDocument );

         /**
         * Destroy the given SVG engine
         *
         *@param  : aEngine -- SVG Engine.
         *@return None .
         */
         IMPORT_C void DestroyEngine( CSvgEngineImpl* aEngine );

         /**
         * Return the root element of the given document.
         *
         *@param  : aDocument -- SVG document.
         *@return root element .
         */
         IMPORT_C CXmlElementImpl* GetRootElement( CSvgDocumentImpl* aDocument );

         /**
         * Set a default SvgSvg root element of the given document.
         *
         *@param  : aDocument -- SVG document.
         *@return void.
         */
         IMPORT_C void InitRootElement( CSvgDocumentImpl* aDocument );

         /**
         * Return the root element of the given document.
         *
         *@param  : aDocument -- SVG document.
         *@param  : aId -- id string to search for element.
         *@return element with given id .
         */
         IMPORT_C CXmlElementImpl* GetElementById( CSvgDocumentImpl* aDocument, const TDesC& aId );

         /**
         * Return the number of ids in a given document
         *
         *@param  : aDocument -- SVG document.
         *@return number of ids found.
         */
         IMPORT_C TInt GetNumberOfIds( CSvgDocumentImpl* aDocument );

         /**
         * Return the id at from the document at index
         *
         *@param  : aDocument -- SVG document.
         *@param  : index -- which id to return
         *@param  : myId -- id string to return by reference.
         *@return none
         */
         IMPORT_C TDesC* GetId( CSvgDocumentImpl* aDocument, TInt index );

         /**
         * Return parent element of the given element.
         *
         *@param  : aElement -- SVG element.
         *@return element with given child.
         */
         IMPORT_C CXmlElementImpl* GetParentElement( CXmlElementImpl* aElement );

         /**
         * Set parent element for the given element.
         *
         *@param  : aElement -- SVG element.
         *@param  : aParentElement -- SVG parent element.
         *@return void
         */
         IMPORT_C void SetParentElement( CXmlElementImpl* aElement, CXmlElementImpl* aParentElement );

		 /**
         * Set the first child for the given element.
         *
         *@param  : aFirstChildElement -- SVG element.
         *@param  : aParentElement -- SVG parent element.
         *@return void
         */
         IMPORT_C void SetFirstChildElement( CXmlElementImpl* aFirstChildElement, CXmlElementImpl* aParentElement );


         /**
         * Create an element of the given type.
         *
         *@param  : aDocument -- SVG element document.
         *@param  : aElementType -- SVG element type.
         *@return element.
         */
         IMPORT_C CXmlElementImpl* CreateElementL( CSvgDocumentImpl* aDocument, TInt aElementType );

         /**
         * Create an element of the given type.
         *
         *@param  : aDocument -- SVG element document.
         *@param  : aElementType -- SVG element type.
         *@return element.
         */
         IMPORT_C void DestroyElement( CXmlElementImpl* aElement );

         /**
         * Append the given child element to the given parent element.
         *
         *@param  : aParentElement -- SVG parent element.
         *@param  : aChildElement -- SVG child element.
         *@return None.
         */
         IMPORT_C void AppendChild( CXmlElementImpl* aParentElement, CXmlElementImpl* aChildElement, TBool aIsJSR226Element = EFalse);

         /**
         * Remove the given child element from the given parent element.
         *
         *@param  : aParentElement -- SVG parent element.
         *@param  : aChildElement -- SVG child element.
         *@return None.
         */
         IMPORT_C void RemoveChild( CXmlElementImpl* aParentElement, CXmlElementImpl* aChildElement);

         /**
         * Get the first child element in the given parent.
         *
         *@param  : aParentElement -- SVG parent element.
         *@return The first child element.
         */
         IMPORT_C CXmlElementImpl* GetFirstChild( CXmlElementImpl* aParentElement );

         /**
         * Get the next sibling element of the given element.
         *
         *@param  : aElement -- SVG element.
         *@return The next sibling element.
         */
         IMPORT_C CXmlElementImpl* GetNextSibling( CXmlElementImpl* aElement );

         /**
         * Set the next sibling element for the given element.
         *
         *@param  : aElement -- SVG element.
         *@param  : aSibling -- SVG sibling element.
         *@return None
         */
         IMPORT_C void SetNextSibling( CXmlElementImpl* aElement, CXmlElementImpl* aSibling );

         /**
         * Get the document that is associated with the given element.
         *
         *@param  : aElement -- SVG element.
         *@return svg document.
         */
         IMPORT_C CSvgDocumentImpl* GetOwnerDocument( CXmlElementImpl* aElement );

         /**
         * Get the element type, such as rect, circle, etc. for the given element.
         *
         *@param  : aElement -- SVG element.
         *@return element type
         */
         IMPORT_C TInt GetElementType( CXmlElementImpl* aElement );

         /**
         * Set the given request-observer for the given svg engine.
         *
         *@param  : aElement -- SVG element.
         *@return element type
         */
         IMPORT_C void SetRequestObserver( CSvgEngineImpl* aEngine, MSvgRequestObserver* aObserver );

         /**
         * Get the Bounding Box for an element.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aX -- X coordinate for the top left.
         *@param  : aY -- Y coordinate for the top left.
         *@param  : aWidth -- Width of the bounding rectangle.
         *@param  : aHeight -- Height for the bounding rectangle.
         *@return None.
         */
         IMPORT_C void GetElementBoundingbox(CSvgElementImpl* aElementHandle,TReal32& aX ,TReal32& aY,TReal32& aWidth,TReal32& aHeight) __SOFTFP;

				/**
         * Get the unscaled Bounding Box for an element.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aX -- X coordinate for the top left.
         *@param  : aY -- Y coordinate for the top left.
         *@param  : aWidth -- Width of the bounding rectangle.
         *@param  : aHeight -- Height for the bounding rectangle.
         *@return None.
         */

				IMPORT_C void GetElementUnScaledBoundingBox( CSvgElementImpl* aElementHandle,TReal32& aX ,TReal32& aY,TReal32& aWidth,TReal32& aHeight) __SOFTFP;

        /**
         * Get the Bounding Box for an element.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aPoint1 --  coordinate for the top left.
         *@param  : aPoint2 --  coordinate for the top right.
         *@param  : aPoint3 --  coordinate for the bottom left.
         *@param  : aPoint4 --  coordinate for the bottom right.
         *@return None.
         */
         IMPORT_C void GetFourPointElementBoundingBox(CSvgTextElementImpl* aElementHandle,TPoint& aPoint1 ,TPoint& aPoint2,TPoint& aPoint3,TPoint& aPoint4);

         /**
         * Get the Float attribute for the element.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aAttribute Id the attribute Id corresponding to the attribute.
         *@return  The float attribute value.
         */
         IMPORT_C TReal32 GetElementFloatAttribute( CSvgElementImpl* aElementHandle, const TInt aAttributeId) __SOFTFP;

         /**
         * Set the Float attribute for the element.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aAttribute Id the attribute Id corresponding to the attribute.
         *@param  : aValue  the value to for the float attribute to be set.
         *@return  None.
         */
         IMPORT_C void SetElementFloatAttribute( CSvgElementImpl* aElementHandle, const TInt aAttributeId, TReal32 aValue) __SOFTFP;

         /**
         * Set the Descriptor attribute for the element.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aAttribute Id the attribute Id corresponding to the attribute.
         *@param  : aAttributeValue  the value to for the Desc attribute to be set.
         *@return  None.
         */
         IMPORT_C void SetElementDesAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId, const TDesC& aAttributeValue);

         /**
         * Get the Desc attribute for the element.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aAttribute Id the attribute Id corresponding to the attribute.
         *@param  : aValue . reference to the attribute value.
         *@return : None.
         */
         IMPORT_C TInt GetElementDesAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TPtrC16& aValue);

         /**
         * Set the Color attribute for the element.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aAttribute Id the attribute Id corresponding to the attribute.
         *@param  : aColorValue  The Integer value corresponding to the color.
         *@return  None.
         */
         IMPORT_C void SetElementColorAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32 aColorValue);

         /**
         * Get the CoLor attribute for the element. // this basically means the fill and stroke.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aAttribute Id the attribute Id corresponding to the attribute.
         *@return : Integer containing the RGB value for the color.
         */
         IMPORT_C TInt32 GetElementColorAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId);

         /**
         * Get the Enum attribute for the element. // this basically means the fill and stroke.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aAttribute Id the attribute Id corresponding to the attribute.
         *@param : Integer containing the enum value for the attribute.
         */
         IMPORT_C void SetEnumAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32 lValue);

	     /**
         * Set the preserve aspect ratio setting for the current document
         *
         *@param  : aDocumentHandle
         *@param  : TPreserveAspectRatio a preserve aspect ratio setting
         */
		 IMPORT_C void SetPreserveAspectRatio( CSvgDocumentImpl* aDocument, TSvgPreserveAspectAlignType aPreserveAspectSetting, TSvgMeetOrSliceType aSmilFitSetting, TBool aFrameBufferOverridesViewport = EFalse );
		
         /**
         * Set the Enum attribute for the element. // this basically means the fill and stroke.
         *
         *@param  : aElementHandle -- SVG element.
         *@param  : aAttribute Id the attribute Id corresponding to the attribute.
         *@return : Integer containing the enum value for the attribute.
         */
         IMPORT_C  TInt GetEnumAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32& lValue);
         /**
         * Get the rect values.
         *
         *@param  : aElementHandle, aX, aY, aWidth, aHeight
         *@return  TBool a viewbox existed
         */
          IMPORT_C TBool GetRectAttribute( CXmlElementImpl* aElementHandle,
                                          float* aX, float* aY, float* aWidth, float* aHeight );

        /**
         * Set the rect values.
         *
         *@param  : aElementHandle, aX, aY, aWidth, aHeight
         *@return  none
         */
          IMPORT_C void SetRectAttribute( CXmlElementImpl* aElementHandle,
                                          float aX, float aY, float aWidth, float aHeight );

        /**
         * Get the matrix values.
         *
         *@param  : aElementHandle, aAVal, aBVal, aCVal, aDVal, aEVal, aFVal
         *@return  none
         */
          IMPORT_C void GetMatrixAttribute( CXmlElementImpl* aElementHandle, float* aAVal,
                                                           float* aBVal, float* aCVal, float* aDVal,
                                                           float* aEVal, float* aFVal );
                                                           
                                                           /**
         * Get the matrix values for a given matrix type.
         *
         *@param  : aElementHandle, aAVal, aBVal, aCVal, aDVal, aEVal, aFVal
         *@return  none
         */
          IMPORT_C void GetMatrixAttribute( CXmlElementImpl* aElementHandle, TInt aAttributeType,
          												   float* aAVal, float* aBVal, 
          												   float* aCVal, float* aDVal,
                                                           float* aEVal, float* aFVal );

        /**
         * Set the matrix values.
         *
         *@param  : aElementHandle, aAVal, aBVal, aCVal, aDVal, aEVal, aFVal
         *@return  none
         */
          IMPORT_C void SetMatrixAttribute( CXmlElementImpl* aElementHandle, float aAVal,
                                                           float aBVal, float aCVal, float aDVal,
                                                           float aEVal, float aFVal );

        /**
         * Get the Viewport Width .
         *
         *@param  : aDocumentHandle
         *@return  TInt  integer containing the width of the viewport. viewport means
         * svg element.
         */
        IMPORT_C TInt GetViewportWidth( CSvgDocumentImpl* aDocumentHandle );

        /**
         * Get the Viewport Height.
         *
         *@param  : aDocumentHandle
         *@return  TInt  integer containing the Height of the viewport. Viewport means
         * svg Element.
         */
        IMPORT_C TInt GetViewportHeight( CSvgDocumentImpl* aDocumentHandle );

        /**
         * Set the Viewport Width.
         *
         *@param  : aDocumentHandle
         *@param  TInt  integer containing the Width of the viewport. Viewport means
         * svg Element.
         *@return  none
         */
        IMPORT_C void SetViewportWidth( CSvgDocumentImpl* aDocumentHandle, TInt aWidth );

        /**
         * Set the Viewport Height.
         *
         *@param  : aDocumentHandle
         *@param  TInt  integer containing the Height of the viewport. Viewport means
         * svg Element.
         *@return  none
         */
        IMPORT_C void SetViewportHeight( CSvgDocumentImpl* aDocumentHandle, TInt aHeight );

        /**
         * Get the Viewport Units. This means the units in which the width and height of
         * <svg> element are specified.
         *
         *@param  : aDocumentHandle
         *@return  TInt  integer containing the units.
         */
        IMPORT_C TInt GetViewportUnits( CSvgDocumentImpl* aDocumentHandle );

        /**
         * Set the end time for an animation element.
         *
         *@param  : aElementHandle
         *@param  : aOffsetTime
         *@return  none.
         */
        IMPORT_C void SvgEndElementAt(CXmlElementImpl* aElementHandle , TUint32 aOffsetTime, CSvgDocumentImpl* aDocumentHandle = NULL);

        /**
         * Set the Begin time for an animation element.
         *
         *@param  : aElementHandle
         *@param  : aOffsetTime
         *@return  none.
         */
        IMPORT_C void SvgBeginElementAt(CXmlElementImpl* aElementHandle , TUint32 aOffsetTime, CSvgDocumentImpl* aDocumentHandle = NULL);
        /**
         * Sets the Media time for a Document.
         *
         *@param  : aDocumentHandle
         *@param  : aTimeInMilliSeconds. This represents the time in milliseconds.
         *@return  none.
         */
        IMPORT_C void SvgSetMediaTime(CSvgDocumentImpl* aDocumentHandle ,TUint32 aTimeInMilliSeconds);

        /**
         * Gets the Media time for a Document.
         *
         *@param  : aDocumentHandle
         *@return  TReal32 . Media time in seconds.
         */
        IMPORT_C TReal32 SvgGetMediaTime(CSvgDocumentImpl* aDocumentHandle ) __SOFTFP;

        /**
         * checks whether the document has any animation elements present.
         *
         *@param  : aDocumentHandle
         *@return  TBool . ETrue if animation present otherwise EFalse.
         */
        IMPORT_C TBool SvgHasAnimation(CSvgDocumentImpl* aDocumentHandle );


         /**
         * Return true if element is removeable (no id or children with ids)
         *
         *@param  : aElement -- SVG element.
         *@return true if removeable false if not removeable
         */
         IMPORT_C TBool IsRemoveable( CSvgElementImpl* hElement, TBool aCheckSibling = EFalse );

         /**
         * Return true if element is removeable (no id or children with ids)
         *
         *@param  : aElement -- SVG element.
         *@return true if removeable false if not removeable
         */
         IMPORT_C TBool IsElementActive( CSvgElementImpl* hElement );

        /**
         * returns the pointer to the focussed element.
         *
         *@param  : None.
         *@return : CXmlElementImpl* pointer to the focussed element at present.
         */
        IMPORT_C CXmlElementImpl* GetFocusedElement(CSvgDocumentImpl* aDocument);

        /**
         * Sets the focus on a specific element.
         *
         *@param  : aElement.
         *@return : None.
         */
        IMPORT_C  void SetFocusElement(CXmlElementImpl* aElement, CSvgDocumentImpl* aDocument);

        /**
         * checks whether a given element is present in DOM.
         *
         *@param  : aDocument. this is pointer to the document which is to be searched for the presence
         * of the element.
         *@param  : aElement. This represents the pointer to the element.
         *@return : TBool. True if element is present otherwise false.
         */
        IMPORT_C    TBool SVGElementInDom(CSvgDocumentImpl* aDocument,  CXmlElementImpl* aElement);

        /**
         * This API will start the animation if the focussed object is an animation element.
         *
         *@param  : aDocument. This is a pointer to the document. ?? this may not be needed.
         *@return : None.
         */
        IMPORT_C    void SvgActivateAnimation(CSvgDocumentImpl* aDocument);

       /**
        * SVGElement_GetUsedElement :- This API will return a pointer to the original
        * element which was cloned to use it. i.e.the cloned element is made a chld of the <use>
        * element.
        * @param : aElement :- this is handle to the cloned element. this is child of the
        *        : <use> element.
        * @return : CXmlElementImpl* this is the pointer to the actual element, which is
        *           cloned.
        */
        IMPORT_C    CXmlElementImpl* SVGElementGetUsedElement(CXmlElementImpl* aElement);

        /**
         * This API will add the given element to the event receiver list.
         *
         *@param  : aElement. This is pointer to the element which is to be added.
         *@param  : aEventMask. This is the event mask.
         *@return : None.
         */
        IMPORT_C    void AddToEventReceiverList(CXmlElementImpl* aElement, const TUint8 aEventMask);

        /**
         * This API will remove the given element to the event receiver list.
         *
         *@param  : aElement. This is pointer to the element which is to be removed.
         *@param  : aEventMask. This is the event mask.
         *@return : None.
         */
        IMPORT_C    void RemoveFromEventReceiverList(CXmlElementImpl* aElement);

        /**
         * DispatchMouseEventsAt This will send mouse events to the Engine at this points.
         *
         *@param  : aDocumentHandle. which document should get the engine.
         *@param  : aMouseX. X coordinate for the mouseEvents
         *@param  : aMouseY. Y coordinate for the mouseEvents
         *@return : int.
         */
        IMPORT_C    TInt DispatchMouseEventsAt(CSvgDocumentImpl* aDocumentHandle, TInt aMouseX, TInt aMouseY, MSvgMouseListener* aListener);

        /**
         * SvgCreatePath his will create a new empty path.
         *
         *@param  : None.
         *@return : None.
         */
        IMPORT_C    CGfxGeneralPath* SvgCreatePath();

        /**
         * SvgDestroyPath this will delete a path given a pointer to it.
         *
         *@param  : aPathHandle. Path element pointer.
         *@return : None.
         */
        IMPORT_C    void SvgDestroyPath(CGfxGeneralPath* aPathHandle);

        /**
         * GetSegmentCount Returns the number of segments in a PATH.
         *
         *@param  : aPathHandle. Path element pointer.
         *@return : integer representing the number of segments.
         */
        IMPORT_C    TInt GetSegmentCount(CGfxGeneralPath* aPathHandle);

        /**
         * GetSegmentType Returns the segment type for a specific segment in a path element.
         *
         *@param  : aPathHandle. Path element pointer.
         *@param  : aSegmentIndex , Segment index for the specific segments.
         *@return : integer representing the type of segments.
         */
        IMPORT_C    TInt GetSegmentType(CGfxGeneralPath* aPathHandle, TInt aSegmentIndex);

        /**
         * GetSegmentParameter Returns the segment parameter  for a specific segment in a path element.
         *
         *@param  : aPathHandle. Path element pointer.
         *@param  : aSegmentIndex , Segment index for the specific segments.
         *@param  : aSegmentParameterIndex. This gives the segment parameter index.
         *@return : TReal32 representing the  segment parameter.
         */
        IMPORT_C    TReal32 GetSegmentParameter(CGfxGeneralPath* aPathHandle,TInt aSegmentIndex,TInt aSegmentParameterIndex) __SOFTFP;

        /**
         * ADDMoveTo Adds a moveTo segment to a path element.
         *
         *@param  : aPathHandle. Path element pointer.
         *@param  : aX , x coordinate for the MoveTo command.
         *@param  : aY. Y coordinate for the MoveTo command.
         *@return : None.
         */
        IMPORT_C    void ADDMoveTo(CGfxGeneralPath* aPathHandle, TReal32 aX,TReal32 aY) __SOFTFP;

        /**
         * ADDLineTo Adds a LineTo segment to a path element.
         *
         *@param  : aPathHandle. Path element pointer.
         *@param  : aX , x coordinate for the LineTo command.
         *@param  : aY. Y coordinate for the LineTo command.
         *@return : None.
         */
        IMPORT_C    void ADDLineTo(CGfxGeneralPath* aPathHandle,TReal32 aX,TReal32 aY) __SOFTFP;

        /**
         * ADDQuadTo Adds a QuadTo segment to a path element.
         *
         *@param  : aPathHandle. Path element pointer.
         *@param  : aX1 , X coordinate for the first control point.
         *@param  : aY1 , Y coordinate for the first control point.
         *@param  : aX2 , X coordinate for the end point.
         *@param  : aY2 , Y coordinate for the end point.
         *@return : None.
         */
        IMPORT_C    void ADDQuadTo(CGfxGeneralPath* aPathHandle,TReal32 aX1,TReal32 aY1,TReal32 aX2,TReal32 aY2) __SOFTFP;

        /**
         * ADDCurveTo Adds a curveTo segment to a path element.
         *
         *@param  : aPathHandle. Path element pointer.
         *@param  : aX1 , X coordinate for the first  control point.
         *@param  : aY1 , Y coordinate for the first  control point.
         *@param  : aX2 , X coordinate for the second control point.
         *@param  : aY2 , Y coordinate for the second control point.
         *@param  : aX3 , X coordinate for the end            point.
         *@param  : aY3 , Y coordinate for the end            point.
         *@return : None.
         */
        IMPORT_C    void ADDCurveTo(CGfxGeneralPath* aPathHandle,TReal32 aX1,TReal32 aY1,TReal32 aX2,TReal32 aY2, TReal32 aX3, TReal32 aY3) __SOFTFP;

        /**
         * ADDCloseTo Closes a given path element.
         *
         *@param  : aPathHandle. Path element pointer.
         *@return : None.
         */

        IMPORT_C    void ADDCloseTo(CGfxGeneralPath* aPathHandle);

        /**
         * GetPathAttribute gets a pointer to the path attribuite for a given element.
         *
         *@param  : aElementHandle. Svg element pointer.
         *@param  : aAttributeId. This gives you the attributeId for the element to get.
         *@return : CGfxGeneralPath* this gives you the path attribute pointer fetched.
         */
        IMPORT_C    CGfxGeneralPath* GetPathAttribute(CXmlElementImpl* aElementHandle, TInt aAttributeId);

        /**
         * SetPathAttribute sets the path attribute for a given element.
         *
         *@param  : aElementHandle. Svg element pointer.
         *@param  : aAttributeId. This gives you the attributeId for the element to get.
         *@param  : PathHandle this gives you the path attribute pointer fetched.
         *@return : NONE.
         */
        IMPORT_C    void SetPathAttribute(CSvgElementImpl* aElementHandle, TInt lSvgAttrId, CGfxGeneralPath* aPathHandle);

        /**
         * Change the frame Buffer dynamically
         *
         *
         *
         *@aRenderBuffer :- Buffer for drawing the DOM Tree.
         */
         IMPORT_C void SetFrameBuffer(CFbsBitmap* aRenderBuffer, TInt aEngine = NULL);

         IMPORT_C void ClearFrameBuffer(CFbsBitmap* aFrameBuffer, TUint32 aClearingColor, TInt aEngine = NULL);

        /**
         * Search for all occurrences of a string in the current svg document,
         * in the cdata of the <text> elements.  The bounding-boxes are transformed,
         * accounted for zooming/panning.
         *
         *
         *@param  : aSearchString -- String to search
         *@param  : aBoundingBoxes -- Array to contain the bounding boxes of texts found.
         *          The bounding boxes are already transformed so that they're related
         *          to the frame buffer origin.
         *@param  : aTexts -- Array to contain cdata of the <text> elements containing
         *                    the search-string.
         *@param  : aCaseSensitive -- Flag to indicate whether to search with case-sensitivity
         *                            or not, defaults to ETrue.
         *@return ETrue if at least one occurrence is found, EFalse otherwise.
         */
         IMPORT_C TBool SearchForText( const TDesC& aSearchString,
                                       RPointerArray<MRect>& aBoundingBoxes,
                                       RArray<TPtrC>& aTexts,
                                       RArray<TInt>& aElementIds,
                                       TBool aCaseSensitive = ETrue,
                                       TInt aEngine = NULL );

        /**
         * Add a HyperlinkListener to the SVG Engine..
         *
         *@param  : aListener -- HyperlinkListener to receive callbacks.
         *@return None
         */
        IMPORT_C void AddHyperlinkListener( MSvgHyperlinkListener* aListener, TInt aEngine = NULL );

        /**
         * Add a TextAreaListener to the SVG Engine..
         *
         *@param  : aListener -- TextAreaListener to receive callbacks.
         *@return None
         */
        IMPORT_C TBool AddTextAreaListener( MSvgTextAreaListener* aListener, TInt aEngine = NULL );

        /**
         * Remove a TextAreaListener from the SVG Engine..
         *
         *@param  : aListener -- TextAreaListener to receive callbacks.
         *@return None
         */
        IMPORT_C TBool RemoveTextAreaListener( MSvgTextAreaListener* aListener, TInt aEngine = NULL );

        /**
         * Add a TextListener to the SVG Engine..
         *
         *@param  : aListener -- TextListener to receive callbacks.
         *@return None
         */
        IMPORT_C TBool AddTextListener( MSvgTextListener* aListener, TInt aEngine = NULL );

        /**
         * Remove a TextListener from the SVG Engine..
         *
         *@param  : aListener -- TextListener to receive callbacks.
         *@return None
         */
        IMPORT_C TBool RemoveTextListener( MSvgTextListener* aListener, TInt aEngine = NULL );


        /**
         * Add a text string to a text area
         *
         *@param  : aTextAreaElementId, aString
         *@return ETrue EFalse
         */
        IMPORT_C TBool SetTextForTextAreaElement( TInt aTextAreaElementId, TDesC& aXmlString );

        /**
         * Get the entire text string from the specified text area
         *
         *@param  : aTextAreaElementId, is text area editable, aString
         *@return is text area editable
         */
        IMPORT_C TBool GetTextForTextAreaElement( TInt aTextAreaElementId, TBool& editable, TDes& aXmlString );

         /**
         * Add a text string to a text element
         *
         *@param  : aTextElementId, aString
         *@return ETrue EFalse
         */
        IMPORT_C TBool SetTextForTextElement( TInt aTextElementId, TDesC& aXmlString );

        /**
         * Get the entire text string from the specified text element
         *
         *@param  : aTextElementId, is text editable, aString
         *@return is text editable
         */
        IMPORT_C TBool GetTextForTextElement( TInt aTextElementId, TBool& editable, TDes& aXmlString );

         /**
         * Remove a HyperlinkListener from the SVG Engine..
         *
         *@param  : aListener -- HyperlinkListener to receive callbacks.
         *@return None
         */
        IMPORT_C void RemoveHyperlinkListener( MSvgHyperlinkListener* aListener, TInt aEngine = NULL );

        /**
         * Add a HyperlinkListener to the SVG Engine..
         *
         *@param  : aListener -- HyperlinkListener to receive callbacks.
         *@return None
         */
        IMPORT_C void AddAnimationListener( MSvgAnimationListener* aListener, TInt aEngine = NULL );

         /**
         * Remove a HyperlinkListener from the SVG Engine..
         *
         *@param  : aListener -- HyperlinkListener to receive callbacks.
         *@return None
         */
        IMPORT_C void RemoveAnimationListener( MSvgAnimationListener* aListener, TInt aEngine = NULL );

         /**
         * Return whether an element is visible on the frame buffer.
         *
         *@param  : aElementHandle -- an SVG element identifier.
         *@return ETrue if element is visible, EFalse otherwise.
         */
        IMPORT_C TBool IsElementVisible( TInt aElementHandle, TInt aEngine = NULL );

         /**
         * Set the engine to load SVG contents as thumbnails:
         *     a) Only DRM protected contents are affected by this API.
         *     b) In thumbnail mode, DRM rights in not consumed.
         *     c) In thumbnail mode, the svg contents are drawn only
         *        to frame-buffers of 64x64 or smaller.
         *
         *@param  : aThumbNailMode -- Flag to turn on/off thumbnial mode.
         *@return : none
         */
        IMPORT_C void SetThumbNailMode( TBool aThumbNailMode, TInt aEngine = NULL );
        
        /**
        * Set the engine to enable/disable consuming DRM rights
        * 
        *@param : aEnable -- Flag to turn off/on consuming rights
        *@return: none
        */
        IMPORT_C void SetDRMRights(TBool aEnable);

        /**
         * Adds a text element to display the frames per second if isShowOn = true
         *
         *@param  : isShowOn specified whether or not to display fps
         *@return : frames per second in the engine
         */
        IMPORT_C TReal FramesPerSecond(TBool isShowOn = EFalse, TInt aEngine = NULL) __SOFTFP;

        /**
         * Used for testing purpose.
         *
         *@param  : aCustomOption specified whether or not to use OpenVG implementation
         *@return : none
         */
        EXPORT_C void CustomOption(TBool aCustomOption = EFalse, TInt aEngine = NULL);

        // return an array with all of the elements of a certain type
        // passing in a -1 for element id will return all elements
        IMPORT_C void FindAllElements( CSvgElementImpl* aStartElement, TInt aElementId,
                              RPointerArray<CSvgElementImpl>& aList, TInt aEngine = NULL );


        /**
         * Create an svg element that can be added to the a document
         * to draw the given rectangle.
         *
         *@param  : aRect - rectangle to request element.
         *@return : element represent rectangle.
         */
        IMPORT_C CSvgElementImpl* DrawBox(TRect aRect, TInt aEngine = NULL);

        /**
         *  Return the default size of the given document handle, defined
         *  by the union of the bounding boxes of the elements.  This function
         *  is independent of frame-buffer or any other document.
         *
         * @since 1.0
         * @param aDomHandle - Handle to svg document (thru PrepareDom)
         * @return Size of svg content
         */
         IMPORT_C TSize GetUnscaledContentSize( TInt aDomHandle );

        /**
        * Generate frames, defined by the given
        * duration, delay per seconds, etc.  The calling client will be
        * responsible for managing the bitmaps created by this method that
        * are appended to the given bitmap arrays.  The starting frame can
        * be specified with the starting time parameter.
        *
        * Note: Embedded images/svg files will not be rendered, due
        * to asynchronous image decoder.
        *
        * Redundant frames removal is implemented, defaulted to EFalse.
        *
        * aDomHandle             -- Handle to the svg dom.
        * aDimensionOfBitmaps    -- Size of bitmaps to generate (same for mask)
        * aStartTime             -- Start time of first frame (in milliseconds)
        * aDuration              -- Total time of animation (in milliseconds)
        * aMillisecondsPerFrame  -- Request delay time between each frame.
        * aColorDepth            -- Color display mode for main bitmaps
        * aMaskType              -- Color display mode for mask bitmaps
        * aBitmapFrames          -- Array to hold animation bitmaps
        * aMaskFrames            -- Array to hold mask bitmaps
        * aDelayIntervals        -- Delay interval between each frame (in milliseconds)
        * aRemoveRedundantFrames -- If ETrue, remove consecutive frames that are the same
        *                           and set the correct delay.
        * Return Non-NULL Pointer to a svg error (MSvgError*)
        */
       IMPORT_C MSvgError* RenderFrames( TInt aDomHandle,
                                         const TSize& aDimensionOfBitmaps,
                                         TUint aStartTime,
                                         TUint aDuration,
                                         TUint aMillisecondsPerFrame,
                                         TDisplayMode aColorDepth,
                                         TDisplayMode aMaskType,
                                         RPointerArray<CFbsBitmap>& aBitmapFrames,
                                         RPointerArray<CFbsBitmap>& aMaskFrames,
                                         RArray<TUint>& aDelayIntervals,
                                         TBool aRemoveRedundantFrames = EFalse );

       /**
         * Used when creating an element from the java interface
         * initializes all of the style properties attribute array to
         * null for a newly created element so that attributes can be added
         * to that element
         *
         *@param  : aElementHandle -- an SVG element identifier.
         *@return
         */
       IMPORT_C void InitSvgStylePropertiesWithNullL( CSvgElementImpl* aElement );

       /**
         * Add a specific event listener to the engine
         *
         *@param  : aListener, a listener type
         *@return : none
         */
       IMPORT_C void AddListener( const MSvgListener* aListener, TSvgListenerType aType, TInt aEngine = NULL );

       /**
         * Remove a specific event listener from the engine
         *
         *@param  : aListener, a listener type
         *@return : none
         */
       IMPORT_C void RemoveListener( const MSvgListener* aListener, TSvgListenerType aType, TInt aEngine = NULL );

        /**
         * Request the given engine (or internal engine) to
         * initialize from information in the svg-document attached
         * to it (if any).  This is used to setup the display information
         * from the <svg> tag.  Usually called from MSvgLoadingListener
         * method when <svg> reached.  If aEngine is NULL, then
         * the internal engine is used.
         *
         * @since 1.0
         * @param : aEngine -- svg engine to request initialization.
         * A NULL value, by default, indicates to use the internal
         * engine for initialization.
         * @param : aIsMainThread -- indicates whether this call is
         * made in the "main thread" or not, default to ETrue.
         * @return : none
         */
        IMPORT_C void InitializeEngine( CSvgEngineImpl* aEngine = NULL,
                                        TBool aIsMainThread = ETrue );

        /**
         * Set the timeout duration for fetching external data
         * (Fetchimage calls).  A value of zero,
         * indicates no timeout (by default).  This is designed mainly
         * for SVGT Plugin.
         * @param : aTimeoutSeconds --
         */
        IMPORT_C void SetDataRetrievalTimeOut( TUint aTimeoutSeconds, TInt aEngine = NULL );

        /**
         * Request canceling of parsing of the current document being
         * parsed (through Load API).  This API allows a different thread
         * other than the loading thread to cancel a Load call.
         * @since 1.0
         */
        IMPORT_C void CancelLoad( TInt aEngine = NULL );

        /**
         * Query wether a document is currently bieng parsed through
         * the Load API.  EFalse is returned when a document has completed
         * parsing or CancelLoad has completed successfully.
         * @since 1.0
         */
        IMPORT_C TBool IsLoading( TInt aEngine = NULL );

        /**
         * In _DEBUG mode all elements in the documents DOM will be printed out
         * this method may be used in the future to regenerate the DOM tree
         * could also be used as an encoder
         *
         *@param  : aDocument
         *@return : none
         */
        IMPORT_C void PrintAllElements( CSvgDocumentImpl* aDocument );

        /**
         * In _DEBUG mode print all of the values that are in the styles
         * for the element
         *
         *@param  : aElement
         *@return : none
         */
        IMPORT_C void PrintElementsStyles( CSvgElementImpl* aElement );

        /**
         * Set to indicate whether the contents should be rendered
         * or wait for all images in a content to be decoded, defaults
         * to waiting for all images.
         *
         *@param  : aBool -- indicates waiting for all images to finish
         *                   decoding before drawing any content.
         *@return : none
         */
        IMPORT_C void WaitForImages( TBool aBool = ETrue, TInt aEngine = NULL );

        /**
         * This API provides a delayed (asynchronous) assignment of image
         * data to <image> elements.  This is used when the clients
         * returns a non-KErrNone value for FetchImage callback, so that
         * FetchImage does not block the parsing thread.
         *
         *@param   aUri- uri string that is passed by FetchImage,
         *               the URI of the image files.
         *@param   aData - byte array containing the image data to
         *                 be decoded.  This object will be managed (deleted)
         *                 by svg-engine.
         *@return  none
         */
         IMPORT_C void AssignImageData( const TDesC& aUri, HBufC8* aData );

        /**
         * This API provides API for client to set RWindow so that svgengine
         * is aware of the window that client is using. This will benefit
         * video rendering.
         *
         *@param   aWindow- RWindowBase that retrieve from CCoeControl to derived
         * classes.
         *@return  none
         */
        IMPORT_C void SetClientWindow( RWindow* aWindow );

		/**
         * Method to output the currently loaded SVG content in standard SVG XML form
         * or output a binary encoded version to a file
         *@param   aIsEncodeOn tell whether or not to output binary or standard SVG file.
         *@return  none
         */
		IMPORT_C void SaveSvg( TBool aIsEncodeOn, const TDesC& aFileName, TInt aEngine );

        /**
         * This method sets the volume with user input as a percentage 
         * value (unsigned between 0 and 100 ) 
         *
         * @since S60 3.1
         * @param aPercentage percentage value of volume
         * @return none
         */
         IMPORT_C void SetAudioVolume( TInt aPercentage , TInt aEngine = NULL);

        /**
         * This method mutes the volume of the SVG content
         *
         * @since S60 3.1
         * @return none
         */
         IMPORT_C void MuteAudioVolume( TInt aEngine = NULL );
         
         	/**
         * This API provides client the functionality to trigger focus-in 
         * event for a particular element
         * 
         *
         *@param   aDocumentHandle- Pointer to the document of type SvgDocumentImpl
         *		   aElement- Pointer to the element for which focus needs to be set 
         * 		   
         *@return  none
	   */
		IMPORT_C void DispatchFocusInEvent(CSvgDocumentImpl* aDocumentHandle,CSvgElementImpl *aElement);
		
		/**
         * This API provides client the functionality to trigger focus-in 
         * event for a particular element
         * 
         *
         *@param   aDocumentHandle- Pointer to the document of type SvgDocumentImpl
         *		   aElement- Pointer to the element for which focus needs to be set 
         * 		   
         *@return  none
         */
		IMPORT_C void DispatchFocusOutEvent(CSvgDocumentImpl* aDocumentHandle,CSvgElementImpl *aElement);
		IMPORT_C void ResetContext(TInt aEngine = NULL );

		
    protected:
        /**
         * This method is a special case "ConstructL" method used for polymorphic
         * DLL loading, which enforces this method to be public.
         *
         * @since 1.0
         * @param : aFrameBuffer -- Bitmap to draw svg content.
         * @param : aReqObserver -- Request observer called by svg engine.
         * @return
         */
        IMPORT_C void                       ConstructL( CFbsBitmap* aFrameBuffer,
                                                        MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec  );
        //NGA :: ConstructL overload is added to include renderertype selector
        IMPORT_C void                       ConstructL( CFbsBitmap* aFrameBuffer,
                                                        MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec ,SVGRendererId aRendererType );
        /**
         * This method is a special case "ConstructL" method used for polymorphic
         * DLL loading, which enforces this method to be public.
         *
         * @since 1.0
         *
         * @return
         */
        IMPORT_C void                       ConstructL();
        IMPORT_C void 						ConstructL(TFontSpec& aFontSpec);

    private:

        /**
         * Adds a text element to display the frames per second if isShowOn = true
         *
         *@param  : isShowOn specified whether or not to display fps
         *@return : frames per second in the engine
         */
        TReal FramesPerSecondL( TInt aEngine = NULL );


        /**
         * Get the current focus object's bounding box.
         *
         * @since 1.0
         * @param ETrue - Get the next object
         * @param EFalse - Get the previous object
         * @return
         */
        TRect GetFocusBbox(TBool aNextObject, TInt aEngine = NULL );

         /**
         * Checks whether the Handle is associated with a DOM
         *
         * @param : aHandle -- Handle to a DOM.
         * @return: TBool boolean
         */
         TBool IsDomCached(TInt aHandle);

        /**
         * Request the SVG Engine to resume an animation after certain amount of time
         *
         * @since 1.0
         * @param : none
         * @return : none
         */
        void                       Resume(TInt32 aTime, TInt aEngine = NULL);

        /**
         * Utility function to clean-up after loading a file -- mainly,
         * removing the prepared dom from Dom-list, since it's loaded and
         * more prepared.
         *
         * @since 1.0
         * @param : aDocumentHandle -- document info to handle clean up
         * @return : none
         */
        void PostLoadProcessing( TInt aDocumentHandle );

        //
        /**
         * ImageLoadingObserver interface method.
         * Called from Image loading class to notify image loading
         * completion.
         *
         * @since 1.0
         * @param : aErrorStatus -- error status, if any
         * @return : none
         */
        void ImageLoadingCompleted( TInt aErrorStatus );

        /******* DOM FUNCTIONALITY / ELEMENT TRAVERSAL INTERFACE *****/
        /**
         *  Returns the first child element node of this element. null if this element has no child elements
         *
         * @since 1.2
         * @param aParentElement - a handle to a parent element
         * @return a handle to the first child of the parent element
         */
        CSvgElementImpl* FirstElementChild(CSvgElementImpl* aParentElement);

        /**
         *  last child element node of this element. null if this element has no child elements
         *
         * @since 1.2
         * @param aParentElement - a handle to a parent element
         * @return a handle to the first child of the parent element
         */
        //
        CSvgElementImpl* LastElementChild(CSvgElementImpl* aParentElement);

        /**
         *  Returns the next sibling element node of this element. null if this element has no element sibling nodes that come after this one in the document tree
         *
         * @since 1.2
         * @param aSiblingElement - a handle to a sibling element
         * @return a handle to the sibling before the sibling passed in
         */
        CSvgElementImpl* PreviousElementSibling(CSvgElementImpl* aSiblingElement);

        /**
         *  previous sibling element node of this element. null if this element has no element sibling nodes that come before this one in the document tree.
         *
         * @since 1.2
         * @param aParentElement - a handle to a sibling element
         * @return a handle to the next sibling of the parameter element
         */
        CSvgElementImpl* NextElementSibling(CSvgElementImpl* aSiblingElement);

        /**
         * Return whether the two given bitmaps are the same.
         *
         * @since 1.0
         * @param : aBitmapA -- Bitmap A
         * @param : aBitmapB -- Bitmap B
         * @return : ETrue both bitmaps are the same.
         */
        TBool AreBitmapsTheSame( CFbsBitmap& aBitmapA, CFbsBitmap& aBitmapB );

        /**
         * Create a new CFbsBitmap object with the specific size and color mode.
         *
         * @since 1.0
         * @param : aSize -- size of bitmap to create
         * @param : aColorMode : Color mode to create bitmap
         * @param : aError : Error object to hold error info, if an error occurs
         * @return : Non-Null CFbsBitmap object if no error was encounter
         */
        CFbsBitmap* CreateBitmapL( TSize aSize, TDisplayMode aColorMode,
                                  CSvgErrorImpl& aError );


        /**
         * This mathod restores some style properties back to the
         * original value of the animatied elements' iSvgStyleProperties.
         *
         * @since S60 3.0
         * @return none
         */
        void RestoreAnimStyleProperties( TInt aEngine = NULL );

        CSvgEngineImpl* ChooseEngine( TInt aEngine );

		//Utility method to write out the current SVG engine framebuffer to a file 	
		void ConvertBitmapToFileL(CFbsBitmap* aBitmap, const TDesC& aText);
	
    protected:
        // Current SvgEngine instance
        CSvgEngineImpl*                     iSvgEngine;
		// Current iSvgDocument through Load( ) method
		CSvgDocumentImpl*					iSvgLoadedDocument;

        // Flag to indicate if a content is present, to perform or
        // ignore operations.
        TBool                               iFileIsLoaded;


        // Error object to pass back to clients, maintained by this object.
        CSvgErrorImpl*                      iSvgError;

        // List of prepared DOMs (created by PrepareDom).
        // Deleted by destructor or calling DeleteDom.
        RPointerArray<CSvgDocumentImpl>     iSvgDocumentPointers;
		RPointerArray<CSvgEngineImpl>		iSvgEnginePointers;

        // Keeping track of rotation applied.
        TReal32                             iTotalRotation;

    public:

        IMPORT_C void                        Start(MSvgError*& aError, const TDesC8* aHeaderData ,
                                                   CSvgEngineImpl* aEngine = NULL );

        IMPORT_C void                       Start( const TDesC8* aHeaderData, CSvgEngineImpl* aEngine = NULL,
                                                   TBool aIsMainThread = ETrue);
    private:
        // DRM related
        TBool                               iDrmEnabled;

        // Thumbnail mode
        TBool                               iIsThumbNailMode;
        
        // DRM Rights consumption
        TBool         iDrmRightsConsumptionEnabled;

        // MSvgLoadingListener list
        RPointerArray<MSvgLoadingListener>  iSvgLoadingListeners;

        // Keep track of elements currently having mouse pointer inside
        // to report mouse-exit when it is moved.
        RPointerArray<CSvgElementImpl>      iSvgMouseEnteredElements;

        // Flag to indicate Load or PrepareDom call.
        TBool                               iIsLoadingRequest;

		TBool                               iMouseDownFlag;
		
			//Stores Font Type information
		CSvgBitmapFontProvider *iSvgBitmapFontProvider;
    };

#endif      // SVGENGINEINTERFACEIMPL_H
