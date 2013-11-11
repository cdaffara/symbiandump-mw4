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


#ifndef __INC_CSVGENGINEIMPL__
#define __INC_CSVGENGINEIMPL__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include <fbs.h>


#include "SVGEngineInterfaceImpl.h"
#include "GfxAffineTransform.h"
#include "SVGEvent.h"
#include "SVGDocumentImpl.h"
#include "SVGRequestObserver.h"
#include "SVGImageElementImpl.h"
#include "SVGFourPointRect.h"
#include "SVGListener.h"
#include "SVGFontHashMap.h"
#include "SvgBitmapFontProvider.h"
#include <SVGRendererId.h>

// Svg Engine states
enum TSvgEngineState
    {
    ESVGEngineRunning,
    ESVGEnginePaused,
    ESVGEngineNotStarted,
    ESVGEngineStopped
    };
typedef TUint8      TAnimStatus;


class   CSvgSchemaData;
class   CSvgElementImpl;
class   MXmlDOMImplementation;
class   CGfx2dGc;

class   CSvgEventHandler;
class   MSvgEventReceiver;
class   CSvgErrorImpl;
class   CSvgAElementImpl;
class   CSvgTextAreaElementImpl;
class   CSvgTextElementImpl;

class   CSvgExternalDataHandler;
class   MSvgDataRequester;

// temporary for debugging
class   CSvgTimer;
// temporary for debugging

class CSvgtBitmap;
//This includes the main rendering loop.

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgEngineImpl : public CBase, public MSVGImageLoadingObserver
    {
    public: // Constructor & Destructor

        static CSvgEngineImpl* NewL(CSvgBitmapFontProvider *aSvgBitmapFontProvider);

        static CSvgEngineImpl* NewLC(CSvgBitmapFontProvider *aSvgBitmapFontProvider);

        void ConstructL(CSvgBitmapFontProvider *aSvgBitmapFontProvider);

        /**
         * Two phase constructor
         *
         * @since 1.0
         * @param aFrameBuffer - Pointer to a buffer where the SVG document raster is drawn
         * @param aReqObserver - Pointer to a client side object that implements the MSvgRequestObserver interface
         * @return An instance of the SVG Engine
         */
        static CSvgEngineImpl*  NewL( CFbsBitmap* aFrameBuffer,
                                      MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec, CSvgBitmapFontProvider *aSvgBitmapFontProvider );

        /**
         * Two phase constructor
         *
         * @since 1.0
         * @param aFrameBuffer - Pointer to a buffer where the SVG document raster is drawn
         * @param aReqObserver - Pointer to a client side object that implements the MSvgRequestObserver interface
         * @return An instance of the SVG Engine
         */
        static CSvgEngineImpl* NewLC( CFbsBitmap* aFrameBuffer,
                                       MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec, CSvgBitmapFontProvider *aSvgBitmapFontProvider );

        /**
         * Destructor
         *
         * @since 1.0
         * @param None
         * @return None
         */

        virtual ~CSvgEngineImpl();

        static CSvgEngineImpl*  NewL( CFbsBitmap* aFrameBuffer,
                                      MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec, CSvgBitmapFontProvider *aSvgBitmapFontProvider,SVGRendererId aRendererType );
        static CSvgEngineImpl* NewLC( CFbsBitmap* aFrameBuffer,
                                       MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec, CSvgBitmapFontProvider *aSvgBitmapFontProvider,SVGRendererId aRendererType );
    public: // New Functions
        /**
         * Get the NVG-TLV data
         *
         * @since 1.0
         * @return descriptor pointing to the encoded data.
         */
        const TPtrC8 TLVEncodedData() const;
		
        /**
         * Kick off the process of rasterizing the SVG document elements
         *
         * @since 1.0
         * @param aRootElement - Root element of the SVG document
         * @return None
         */
        void DrawElementsL( CSvgElementImpl* aRootElement);
        /**
         * Kick off the process of placing graphical objects in their positions for rasterization
         *
         * @since 1.0
         * @param aRootElement - Root element of the SVG document
         * @return None
         */
        void UpdateCTM(CSvgDocumentImpl* aSvgDocument );


        /**
         * Invalidate the current raster and hence kick off another round of rasterization
         * process
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void RedrawL();


        /**
         * Destroy the current representation of the SVG document
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void Destroy();

        /**
         * An acessor to the document object that represents the current SVG source
         *
         * @since 1.0
         * @param None
         * @return Pointer to the document object instance.
         */
        CSvgDocumentImpl* Document();

        /**
         * Overwrite the current path information in the given element
         *
         *
         * @param hPath - handle to a path
         * @param hElement - handle to a path element
         * @return none
         */
        void UpdatePath( CGfxGeneralPath* hPath, CSvgElementImpl* hElement );

        /**
         * Kick off the processing of SVG source, and animation time frames
         *
         * @since 1.0
         * @param None
         * @return A boolean value that indicates success/failure
         */

        TBool StartFrameLoopL();

        /**
         * Obtain an image source embedded within a SVG source (JPEG, PNG)
         *
         * @since 1.0
         * @param aUri - A string buffer with the URI of the image source
         * @param aFileName - A string buffer with the contents of the image source
         * @return ETrue - Success, EFalse - Failure
         *
         */
        TBool FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );

        /**
         * Return the animation duration for the given document, if NULL (default)
         * then use the document attached to engine.  The value 0xFFFFFFFF is
         * return is the animation is indefinite.
         *
         * @since 1.0
         * @param aDocumentElement - document to find animation duration.
         * @return An integer containing the aniamtion duration value
         */
        TUint32 Duration( CSvgDocumentImpl* aDocumentElement = NULL );

        /**
         * Accessor for the Graphics Context object
         *
         * @since 1.0
         * @param None
         * @return A pointer to the Graphics Context object
         */
        CGfx2dGc* GraphicsContext();

        /**
         * Internally render SVG source into a buffer (rasterization)
         *
         * @since 1.0
         * @param aSvgData - A string buffer containing the SVG source
         * @param aFrameBuffer - A bitmap passed by the client that will contain the raster
         * @param aPreserveAspectRatio - The aspect ratio (see SVG spec for more) for the raster
         * @return TBool - ETrue/EFalse indicate presence/absence of the resource
         */
        TInt RenderFileToBuffer( const TDesC8& aSvgData,
                                   CFbsBitmap* aFrameBuffer,
                                   CFbsBitmap* aMask,
                                   TBool aPreserveAspectRatio );


		void RenderFileL( const TDesC8& aSvgData,
                                           CFbsBitmap* aFrameBuffer,
                                           CFbsBitmap* aMask,
                                           TBool aPreserveAspectRatio);


        /**
         * A private method that toggles debug info for developers
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void SwitchDebugInfo(TBool debug);

        /**
         * A private method that switch between OpenVG and VGR implementation
         *
         * @since 1.0
         * @param TBool aCustomOption ETrue set OpenVG
         * @return None
         */
        void CustomOption( TBool aCustomOption );

        /**
         * This method is for future extension, in which an external script engine
         * could be used to evaluate a script description.
         *
         * @since 1.0
         * @param : aScript -- A description of script from the "on" attribute.
         *
         * @param : aCallerElement -- The element that calls the script by a
         *                            an event of the "on" attribute.
         * @return : For future use.  Value is ignored.
         */
        TBool ScriptCall( const TDesC& aScript,
                                            CSvgElementImpl* aCallerElement );

        /**
         * Set the URI information in the Engine
         *
         * @since 1.0
         * @param
         * @return
         */
        void LinkRequest( const TDesC& aUri );
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void LinkRequestWithShow(const TDesC& aUri, const TDesC& aLinkShow);

        /**
         * Accessor for the current engine time
         *
         * @since 1.0
         * @param None
         * @return An integer that represents the current engine time
         */
        TInt32 CurrentTIme();

        /**
         * Set SVG Dimensions to Frame buffer size
         *
         * @since 1.0
         * @param None
         * @return An integer that represents the current engine time
         */

        void SetSvgDimensionToFrameBufferL(TUint aWidth, TUint aHeight);

        /**
         * Generate mask bitmap from alpha channel of the framebuffer
         *
         * @since
         * @param
         * @return
         */
        void GenerateMask( CFbsBitmap* aMask );

        void GenerateMask(CSvgtBitmap* aMask);
        
        /**
         * Set background color
         *
         * @since
         * @param
         * @return
         */
        void SetBackgroundColor(TUint32 aRGBA8888Color);

        /**
         * Returns SVG Engine State
         *
         * @since 1.0
         * @param None
         * @return An integer that represents SVG Engine state
         */
         TSvgEngineState SVGEngineState();

         /**
         * Sets SVG Engine state
         *
         * @since 1.0
         * @param An integer that represents SVG Engine state
         * @return void
         */
         void SetSVGEngineState(TSvgEngineState aState);

         /**
         * Sets the GDI context from the Bitmap.
         *
         * @since 1.0
         * @param : aFrameBuffer -- Bitmap to draw svg content.
         * @param : aReqObserver -- Request observer called by svg engine.
         * @return
         */
         void SetGdiContextL(CFbsBitmap* aCurrentBitmap, CFbsBitmap* aMask = NULL);

         void SetGdiContextL(CFbsBitmap* aCurrentBitmap, CFbsBitmap* aMask,TSize aCurrentBitmapSize,TDisplayMode aRenderDspMode,TDisplayMode aMaskDspMode);
         
         void SetGdiContextL(CSvgtBitmap* aCurrentBitmap, CSvgtBitmap* aMask  = NULL);

        /**
         * Start the Engine so that the first frame is drawn when the call is finished.
         *
         *
         * @param : aError
         * @return: None
         */
         void StartEngine(CSvgErrorImpl* aError = NULL);
        void RenderFrame( TUint aCurrentTime );

        /**
         * Set the Document
         *
         *
         * @param : CSvgDocumentImpl* aDocument
         * @return: None
         */
        void SetDocument(CSvgDocumentImpl* aDocument);

        /**
         * Given a document handle and a url this call attaches
         * the given data (ldata) to every element that references that url
         *@param   aDocument - Document Pointer
         *@param   lUrl- uri string that is an external resource to be retrieved
         *@param   ldata - byte array containing the data that that external resource recieved
         *@return  none
         */

        void ChangeDuration( TUint32 aTimerDuration );

        void Resume(TInt32 aTime);

        void SeekEngine( TUint32 aTime);

        void SetMediaTime(TUint32 aTimeInMilliSeconds);

        /**
         * Initializes the SVG Engine primarily with width and height informtion
         * This is based on the attribute specifications for the root 'svg' element
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void InitializeEngineL();

        /**
         * Get all the bounding-boxes for the given text and the contexts include
         * the text was found.  The bounding-boxes are transformed, taking zooming/panning
         * in account.
         *
         * @param : aString -- String to search in the document.
         * @param : aBoundingBoxes -- Array to hold bounding-boxes info.
         * @param : aTexts -- Array to hold complete text of cdata where aString was found.
         * @return: None
         */
         /*
         void FindBBoxesForHorizontalText( const TDesC& aString,
                                 RArray<TRect>& aBoundingBoxes,
                                 RArray<TPtrC>& aTexts,
                                 RArray<TInt>& aElementIds,
                                 TBool aCaseSensitive );
		*/
        /**
         * Get all the bounding-boxes for the given text and the contexts include
         * the text was found.  The bounding-boxes are transformed, taking zooming/panning
         * in account.
         *
         * @param : aString -- String to search in the document.
         * @param : aBoundingBoxes -- Array to hold bounding-boxes info.
         * @param : aTexts -- Array to hold complete text of cdata where aString was found.
         * @return: None
         */
         void FindBBoxesForRotatedText( const TDesC& aString,
                                 RPointerArray<MRect>& aBoundingBoxes,
                                 RArray<TPtrC>& aTexts,
                                 RArray<TInt>& aElementIds,
                                 TBool aCaseSensitive );

        /**
         * Process the given coordinate to determine if it is inside/outside a
         * hyperlink; then, notify the hyperlink listeners.
         *
         * @param : aX -- x coordinate
         * @param : aY -- y coordinate
         * @return: None
         */
        void CheckForHyperlinkMouseover( TInt aX, TInt aY );

        /**
         * Add a hyperlink listener.
         *
         * @param : aListener -- hyperlink listener
         * @param : aY -- y coordinate
         * @return: None
         */
        void AddHyperlinkListener( MSvgHyperlinkListener* aListener );

        /**
         * Add a textbox listener.
         *
         * @param : aListener -- textbox listener
         * @return: None
         */
        void AddTextAreaListener( MSvgTextAreaListener* aListener );

        /**
         * Remove a textbox listener.
         *
         * @param : aListener -- textbox listener
         * @return: None
         */
        void RemoveTextAreaListener( MSvgTextAreaListener* aListener );

        void NotifyTextAreaEntered( CSvgTextAreaElementImpl* aTextAreaElement );

        void NotifyTextAreaExited( CSvgTextAreaElementImpl* aTextAreaElement );

        void AddTextListener( MSvgTextListener* aListener );

        void RemoveTextListener( MSvgTextListener* aListener );

        void NotifyTextEntered( CSvgTextElementImpl* aTextElement );

        void NotifyTextExited( CSvgTextElementImpl* aTextElement );
        
        /**
         * Add an interactive element listener.
         * @since v3.2
         * @param : aListener - interactive element listener
         * @return: None
         */
         void AddInteractiveElementListener( MSvgInteractiveElementListener* aListener );
         
        /**
         * Notify the client when the pointer enters an interactive element.
         * @since v3.2
         * @param : aElement - The element which has some animation
         * @return: None
         */
         void NotifyInteractiveElementEntered(CSvgElementImpl* aElement);
        
        /**
         * Notify the client when the pointer exists an interactive element.
         * @since v3.2
         * @param : aElement - The element which has some animation
         * @return: None
         */
         void NotifyInteractiveElementExited(CSvgElementImpl* aElement);
         
        /**
         * Remove an interactive element listener.
         * @since v3.2
         * @param : aListener - interactive element listener
         * @return: None
         */
         void RemoveInteractiveElementListener( MSvgInteractiveElementListener* aListener );
         
         /**
         * Add a hyperlink listener.
         *
         * @param : aListener -- hyperlink listener
         * @param : aY -- y coordinate
         * @return: None
         */
        void RemoveHyperlinkListener( MSvgHyperlinkListener* aListener );

        void NotifyElementActivatedLinkEntered( CSvgElementImpl* aElement);

        void NotifyElementActivatedLinkExited(  CSvgElementImpl* aElement);
        /**
         * Add a hyperlink listener.
         *
         * @param : aListener -- hyperlink listener
         * @param : aY -- y coordinate
         * @return: None
         */
        void AddAnimationListener( MSvgAnimationListener* aListener );

        /**
         * Add a hyperlink listener.
         *
         * @param : aListener -- hyperlink listener
         * @param : aY -- y coordinate
         * @return: None
         */
        void RemoveAnimationListener( MSvgAnimationListener* aListener );

        void NotifyAnimationStarted();

        void NotifyAnimationPaused();

        void NotifyAnimationEnded();

        //ViewPort Listener
        void GetViewPort(TInt getWidth, TInt getHeight, TBool isWidthInPer, TBool isHeightInPer, TInt &setWidth, TInt &setHeight);

        // Return ETrue if the given element (id) is visible on the frame buffer.
        TBool IsElementVisible( TInt aElementId );

         /**
         * Get Size
         *
         * @since 1.0
         * @param None
         * @return None
         */
        TSize GetSize();
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void UpdatePresentation(const TInt32& aParam);

        /**
         * Return the data pointed to by the given URI string,
         * from an external source.  Calling client takes ownership
         * of returned HBufC8 pointer.
         *
         * @since 1.0
         * @param
         * @return
         */
        HBufC8*                 FetchExternalDataL( const TDesC& aUri );

        
        void AddViewPortListener(MSvgViewPortListener* aListener);
        
        void RemoveViewPortListener(MSvgViewPortListener* aListener);
        
        /**
         * This method sets the volume with user input as a percentage 
         * value (unsigned between 0 and 100 ) for the currently loaded
         * document 
         *
         * @since S60 3.1
         * @param aPercentage percentage value of volume
         * @return none
         */
         void SetAudioVolume( TInt aPercentage );
         TBool IsSVGEnginePaused();

         void SetBitmapHeader(const TDesC* aHeaderData);
         
         void EnableTargetRendering(TBool aTargetRendering );
         TBool IsTargetRenderingEnabled() const;
         
    public: // Functions from base classes
        // From CSvgElementImpl
        /**
         * Process various events received by the client for the SVG image
         *
         * @since 1.0
         * @param aSvgDocument Pointer to the SVG Document on which event is
         *                     to be processed.
         * @param aEvent - Pointer to the object that contains SVG event information (implements MSvgEvent interface)
         * @return None
         */
        void ProcessEventL( CSvgDocumentImpl* aSvgDocument, MSvgEvent* aEvent, 
            TBool aRedraw = ETrue );

        /**
         * Returns false only when document was loaded/prepared with in thumbnail mode
         * and the frame buffer is larger than 80x80
         * Probably move 80x80 size to .mmp file (compiler option)
         *
         * @since 1.0
         * @return EFalse if thumb nail restrictions apply to the current document.
         */
        TBool PassesThumbNailRestriction();

        /**
         * Obtain a font source embedded within an SVG source
         *
         * @since 1.0
         * @param aUri - A string buffer with the URI of the font source
         * @param aFileHandle
         * @return ETrue - Success, EFalse - Failure
         *
         */
        TBool FetchFont( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );

        /**
         * Set to ETrue to ignore notification to UpdateScreen in RequestObserver
         *
         * @param : aBool -- flag to indicate ignore UpdateScreen call
         * @return: None
         */
        void SetIgnoreUpdateScreen( TBool aBool );

        /**
         * Add a MSvgMouseListener to this SvgEngine
         *
         * @param : aListener -- listener to add to listener list
         * @return: None
         */
        void AddMouseListener( const MSvgMouseListener* aListener );

        /**
         * Remove a MSvgMouseListener to this SvgEngine
         *
         * @param : aListener -- listener to remove from listener list
         * @return: None
         */
        void RemoveMouseListener( const MSvgMouseListener* aListener );

        /**
         * Return number of mouselisteners.
         *
         * @return: mouse-listener count
         */
        TInt MouseListenerCount();

        /**
         * Return the (first) viewable element at the given point.
         * visible svg element.
         *
         * @param : aElements -- list to contain viewable elements with bounding-boxes containing
         * the given point.
         * @param : aX -- x coordinate
         * @param : aY -- y coordinate
         * @return: NULL, if none, lowest-level element containing point.
         */
        void GetViewableElementsAtPoint(CSvgDocumentImpl* aSvgDocument, RPointerArray<CSvgElementImpl>& aElements, TInt aX, TInt aY );

        /**
         * Find all elements in the subtree for "viewable" elements, including the given
         * starting node element.  Viewable element are concrete elements that are drawn,
         * such as <rect>, <circle>, <path>, etc.
         * This function only checks for containing of the elements' bounding-boxes.
         *
         * @param : aStartElement -- starting node to search in subtree.
         * @param : aElementList -- List to containing "viewable" elements.
         * @return: None
         */
        void FindAllViewableElements( CSvgElementImpl* aStartElement, RPointerArray<CSvgElementImpl>& iElementList );

        /**
         * Find all elements in the subtree for non-viewable elements, including the given
         * starting node element.  Viewable element are abstract elements that are not drawn,
         * such as <g>, <animate>, <animateMotion>, etc.
         *
         * @param : aStartElement -- starting node to search in subtree.
         * @param : aElementList -- List to containing non-viewable elements.
         * @return: None
         */
        void FindAllNonViewableElements( CSvgElementImpl* aStartElement, RPointerArray<CSvgElementImpl>& iElementList );

        /**
         * Send to mouse listeners of a mouse-pressed event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyMousePressed( RPointerArray<CSvgElementImpl>& aElements,
                                 TInt aX, TInt aY );

        /**
         * Send to mouse listeners of a mouse-released event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyMouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                  TInt aX, TInt aY );

        /**
         * Send to mouse listeners of a mouse-entered event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyMouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                 TInt aX, TInt aY );
        /**
         * Send to mouse listeners of a mouse-exited event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyMouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                TInt aX, TInt aY );

        /**
         * Send to mouse listeners of a mouse-moved event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyMouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                               TInt aX, TInt aY );

        void NotifyHyperlinkEntered( CSvgAElementImpl* aAElementHandle );
        void NotifyHyperlinkEntered( const TDesC& aUri );

        void NotifyHyperlinkExited( CSvgAElementImpl* aAElementHandle );
        void NotifyHyperlinkExited( const TDesC& aUri );

        void NotifyHyperlinkActivated( CSvgAElementImpl* aAElementHandle );
        void NotifyHyperlinkActivated( const TDesC& aUri );

        void NotifyHyperlinkActivatedWithShow( const TDesC& aUri, const TDesC& aShow );

        void NotifyTextAreaActivated( CSvgTextAreaElementImpl* aTextArea);

        void NotifyTextActivated( CSvgTextElementImpl* aTextElement);


        /**
         * Returns whether engine is ready to render
         * Used by SvgTimer
         */
        TBool ReadyToRender();

        /**
         * Reset animation timer to the beginning.
         */
        void ResetTimer();

        /**
        *Save Svg File After doing the text editing
        */
   void SaveSvgL( const TDesC& aFileName );

        /*
        *Write the buffer to the stream - saving the file -svg -text editing
        */
        TInt WriteToStream(RFileWriteStream &aStream,TPtrC aWriteBuffer);

        /*
        *Function to process the <text> tag and do the needful
        */

        HBufC* TextTagProcessingL(TPtrC* aCData, TInt &aStartOffset, TInt aOrgLength,
                                TBool aEndingWithTag);

        /*
        *Function to process the <textArea> tag and do the needful
        */
        HBufC* TextAreaTagProcessingL(TPtrC* aCData, TInt &aStartOffset, TInt aOrgLength,
                                TBool aEndingWithTag);
		
		void ImageLoadingCompleted( TInt /*aErrorStatus*/ );                               
		/*
		*  Function used to set the Rendering Quality
		*/                              
		
		void SetRenderingQuality( const TInt32 aRenderQuality);
		void CSvgEngineImpl::ResetContext();

    private:

        /**
         * Private constructor
         *
         * @since 1.0
         * @return
         */
        CSvgEngineImpl();

        /**
         * Private constructor
         *
         * @since 1.0
         * @param aFrameBuffer - Pointer to a buffer where the SVG document raster is drawn
         * @param aReqObserver - Pointer to a client side object that implements the MSvgRequestObserver interface
         * @return
         */
         CSvgEngineImpl( CFbsBitmap* aFrameBuffer,
                         MSvgRequestObserver* aReqObserver );

        /**
         * A private constructor that constructs heap objects
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void ConstructL(TFontSpec& aFontSpec, CSvgBitmapFontProvider *aSvgBitmapFontProvider );

        void ConstructL(TFontSpec& aFontSpec, CSvgBitmapFontProvider *aSvgBitmapFontProvider,SVGRendererId aRendererType );
        //
        void InitializeEventReceiverListL(CSvgElementImpl *aElement);

        /**
         * Initiate the process of opening embedded links in the SVG source
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void DoHyperLinkingL();

        /**
         * FInd out whether a given element is animation type
         *
         * @since 1.0
         * @param aElement - A pointer to an element
         * @return Boolean returning a True/False
         */
        TBool IsAnimationElement( CSvgElementImpl* aElement );

        /**
         * A complex method that does color calculations for anti aliasing
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void FilteredBlitXRGB4444() const;

        /**
         * A complex method that does color calculations for anti aliasing
         *
         * @since 1.0
         * @param
         * @return
         */
        void FilteredBlitRGB565() const;

        

        // opacity set routines
        CFbsBitmap* CreateOpacityFrameBufferL();
        void CopyBuffer( TUint32* aSrc, TUint32* aDest, const TSize aSize );


        void FindEditableElements( CSvgElementImpl* aStartElement,
                                      RPointerArray<CSvgElementImpl>& aList );
        //for SaveSvgL
        TBool IsEndedWithCompleteTextTag(TDes &bufferptr,TInt EndOffset);

    public:
        CSvgElementImpl*    iTextAreaHandle;

        RPointerArray<TSvgFourPointRect> iSvgTextBoundingBoxes;

        CSvgTimer*              iTimer;
        MSvgRequestObserver*    iRequestObserver;

        CFbsBitmap*             iFrameBuffer;
        CFbsBitmap*             iMask;
        
        TSize                   iFrameBufferSize;  //NGA
        TDisplayMode            iRenderDspMode;
        TDisplayMode            iMaskDspMode;
        TFontSpec               iBitmapFontSpec;
        CSvgFontHashMap* iFontHashMap;
private:

        CSvgDocumentImpl*       iSvgDocument;
        CGfx2dGc*               iGfxContext;
        TPtrC                   iLinkUri;
        TUint32                 iBackgroundColor;
        TPtrC                   iLinkShow;
        TBool                   iShowDebugInfo;

        //M2G: target bitmap buffer and mask buffer
        CSvgtBitmap* iTargetBitmapBuffer;
        CSvgtBitmap* iTargetMaskBuffer;
        
        TGfxRectangle2D         iClipRect;
        TSvgEngineState     iSvgEngineState;
        TAnimStatus iAnimationState;
        TBufC<1>                NullString;
        CSvgErrorImpl*          iSvgError;
        // Pointer/Hyperlink related
        RPointerArray<MSvgHyperlinkListener> iHyperlinkListeners;
        RPointerArray<MSvgTextAreaListener> iTextAreaListeners;
        RPointerArray<MSvgTextListener> iTextListeners;
        CSvgElementImpl*               iMouseoverElement;
        //Animation related
        RPointerArray<MSvgAnimationListener> iAnimationListeners;

        //ViewPort
        MSvgViewPortListener* iViewPortListener;
        // MSvgMouseListener list
        RPointerArray<MSvgMouseListener>    iSvgMouseListeners;
        
        //MSvgInteractiveElementListener list
        RPointerArray<MSvgInteractiveElementListener> iInteractiveElementListeners;
        
        // flag to prevent calling updatescreen (when ETrue)
        TBool iIgnoreUpdateScreen;

        TUint                       iTimeoutSeconds;
        TInt 						iRenderQuality; // Rendering Quality

        //Stores Font Type information as passed by CSvgEngineInterfaceImpl
        CSvgBitmapFontProvider *iSvgBitmapFontProvider;
        TBool iIsTargetRenderingEnabled;
    public:
        TBool                       iCustomOption;
				TBool												iFrameBufferOverridesViewport;
		TBool   iClientDefinedViewPort;
		CDesC16ArrayFlat* iSvgNames;
    };

#endif //__INC_CSVGENGINEIMPL__
