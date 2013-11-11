/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __INC_MSVGLISTENER__
#define __INC_MSVGLISTENER__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "xml/mxmlattributes.h"

class CSvgElementImpl;

// Typedef xml attribute list to allow switching of parser without
// having to change APIs for MLoadingListener.
typedef MXMLAttributes MSvgAttributeList;

/**
 * This empty virtual class is defined to be the root class for
 * SVG Listener interfaces (all pure virtual functions).  The MSvgListener
 * name will be used for parameters of AddListener and RemoveListener
 * along with a type parameter to specific the subclass.
 *
 * This will allow new Listener class to be added without having to
 * add new Add/Remove listener methods to the exported interfaces.
 *
 * Existing listeners (Hyperlink, etc.) and their Add/Remove methods
 * will remain the same.
 */
class MSvgListener
    {
    };

/**
 * Enumeration to define supported listener classes for
 * AddListener/RemoveListener methods.
 */
enum TSvgListenerType
    {
    ESvgLoadingListener = 0,
    ESvgMouseListener,
    ESvgHyperlinkListener,
    ESvgTextAreaListener,
    ESvgTextListener,
    ESvgInteractiveElementListener,
    ESvgViewPortListener    

    };

/**
 * This interface is the callback mechanism for the SVG Engine to communicate to
 * interested clients about svg-file loading/parsing progress.  The client (listener) is
 * notified at the start and end of the document, at the start and end of an element.
 *
 * The ReportAllElements method controls if all elements should be reported or only
 * the direct children of the root <svg> element is reported.

 * Included in this interface are notifications for external data requests: when one is
 * initiated, received or failed.  External data requests include embedded images or svg-files.
 *
 *  @lib SVGEngine.lib
 *  @since 3.1
 */
class MSvgLoadingListener : public MSvgListener
    {
    public:

        /**
         * Query the listener if every element is to be reported or only
         * the children of <svg> tag.
         *
         * @since 1.0
         * @return : ETrue if every element is to be reported, otherwise
         *           only the children of <svg> are reported.
         */
        virtual TBool ReportAllElements() = 0;

        /**
         * Query the listener if client will call 'AssignImageData' to decode images
         * separately from parsing.  This is used to avoid non-thread-safe calls in
         * creating bitmap and decoding images, which must be executed in the main thread.
         *
         * @since 1.0
         * @return : ETrue to use 'ImageDataReference' instead of 'FetchImage'.
         */
        virtual TBool WillAssignImageData() = 0;

        /**
         * Callback when the href:xlink attribute is encountered for the <image>
         * element.  This method is used to notify clients of image data references
         * needed by <image> element.  This method is called only when 'WillAssignImageData'
         * returns ETrue.
         *
         * @since 1.0
         * @return : none.
         */
        virtual void ImageDataReference( const TDesC& aUri ) = 0;

        /**
         * Notified when the start of a svg document(<svg> tag) is encountered.
         *
         * @since 1.0
         * @return : For future use.  Value is ignored.
         */
        virtual TBool DocumentStart() = 0;

        /**
         * Notified when the end of a svg document(</svg> tag) is encountered.
         *
         * @since 1.0
         * @return : For future use.  Value is ignored.
         */
        virtual TBool DocumentEnd() = 0;

        /**
         * Notified when the start of a svg element is encountered.
         *
         * @since 1.0
         * @param : aTagName -- name of svg tag
         * @param : aAttributeList -- attribute list.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool ElementStart( const TDesC& aTagName,
                                    MSvgAttributeList& aAttributeList) = 0;

        /**
         * Notified when the end of a svg element is encountered.
         * activated.
         *
         * @since 1.0
         * @param : aTagName -- name of svg tag
         * @return : For future use.  Value is ignored.
         */
        virtual TBool ElementEnd( const TDesC& aTagName ) = 0;

        /**
         * Notified when an external data is needed by the svg document,
         * such as a image-file or an embedded svg-file.
         *
         * @since 1.0
         * @param : aUri -- URI string of external data
         * @return : For future use.  Value is ignored.
         */
        virtual TBool ExternalDataRequested( const TDesC& aUri ) = 0;

        /**
         * Notified when an external data has been retrieved,
         * such as a image-file or an embedded svg-file.
         *
         * @since 1.0
         * @param : aUri -- URI string of external data
         * @return : For future use.  Value is ignored.
         */
        virtual TBool ExternalDataReceived( const TDesC& aUri ) = 0;

        /**
         * Notified when an external data request has failed.
         *
         * @since 1.0
         * @param : aUri -- URI string of external data
         * @return : For future use.  Value is ignored.
         */
        virtual TBool ExternalDataRequestFailed( const TDesC& aUri ) = 0;

        /**
         * Notified when an unsupported element is encountered.
         *
         * @since 1.0
         * @param : aTagName -- tag name of element
         * @param : aAttributeList -- attribute list.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool UnsupportedElement( const TDesC& aTagName,
                                          MSvgAttributeList& aAttributeList ) = 0;
				
        virtual void ImagesLoaded(TInt aError)=0;

    };

/**
 * This interface is the callback mechanism for the SVG Engine to communicate to
 * interested clients about mouse clicking
 *
 *
 *  @lib SVGEngine.lib
 *  @since 3.1
 */
class MSvgMouseListener : public MSvgListener
    {
    public:
        /**
         * Notified when the mouse pointer enters a visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool MouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY ) = 0;

        /**
         * Notified when the mouse pointer exits a visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool MouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt aX, TInt aY ) = 0;

        /**
         * Notified when the mouse pointer has already entered a visible
         * svg element and just moved but remains inside its bounding-box.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool MouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                                  TInt aX, TInt aY ) = 0;

        /**
         * Notified when the mouse pointer is pressed down on visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool MousePressed( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY ) = 0;

        /**
         * Notified when the mouse pointer is released on on visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool MouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY ) = 0;

    };

/**
 * This interface is the callback mechanism for the SVG Engine to communicate to
 * interested clients about text area interaction.
 *
 *  @lib SVGEngine.lib
 *  @since 3.1
 */
class MSvgTextAreaListener : public MSvgListener
    {
    public:
        /**
         * Notified when a pointer enters a textbox element.
         *
         * @since 1.0
         * @param : aUri -- handle of a textbox
         * @return : For future use.  Value is ignored.
         */
        virtual TBool TextAreaEntered( TInt aTextAreaHandle ) = 0;

        /**
         * Notified when a pointer exits a textbox element.
         *
         * @since 1.0
         * @param : aTextBox -- handle of a textbox
         * @return : For future use.  Value is ignored.
         */
        virtual TBool TextAreaExited( TInt aTextAreaHandle ) = 0;

        /**
         * This method is called to notify the client that a link has been
         * activated.
         *
         * @since 1.0
         * @param : aUri -- handle of a textbox
         * @return : For future use.  Value is ignored.
         */
        virtual TBool TextAreaActivated( TInt aTextAreaHandle ) = 0;

    };

/**
 * This interface is the callback mechanism for the SVG Engine to communicate to
 * interested clients about text element interaction.
 *
 *  @lib SVGEngine.lib
 *  @since 3.1
 */
class MSvgTextListener : public MSvgListener
    {
    public:
        /**
         * Notified when a pointer enters a text element.
         *
         * @since 1.0
         * @param : aUri -- handle of a text
         * @return : For future use.  Value is ignored.
         */
        virtual TBool TextEntered( TInt aTextHandle ) = 0;

        /**
         * Notified when a pointer exits a text element.
         *
         * @since 1.0
         * @param : aText -- handle of a text
         * @return : For future use.  Value is ignored.
         */
        virtual TBool TextExited( TInt aTextHandle ) = 0;

        /**
         * This method is called to notify the client that a link has been
         * activated.
         *
         * @since 1.0
         * @param : aUri -- handle of a text
         * @return : For future use.  Value is ignored.
         */
        virtual TBool TextActivated( TInt aTextHandle ) = 0;

    };
 class MSvgViewPortListener : public MSvgListener
 {
    public:
        /**
         * 
         *
         * @since 1.0
         * @param aErrorStatus : error status if any.
         * @return
         */
        virtual void GetViewPort(TInt getWidth, TInt getHeight, TBool isWidthInPer, TBool isHeightInPer, TInt &setWidth, TInt &setHeight) = 0;
 };

/**
 * This interface is the callback mechanism for the SVG Engine to communicate to
 * interested clients about interactive elements in the svg content.
 *
 *  @lib SVGEngine.lib
 *  @since v3.2
 */
class MSvgInteractiveElementListener: public MSvgListener
    {
    public:
        
        /**
         * Notified when a pointer enters an interactive element.
         * @Since v3.2
         * @param : aElementIdPtr - Pointer to the xml Id of the element 
         *                          if exists else points to an empty string
         * @param : aInteractonTypeList - Indicates the the type interactions 
         *                                possible with that element
         *          e.g: aInteractonTypeList=0x9 then "focusin" and "click" 
         *               events are possible interaction types for the element                                                                                                                                   *interactive events 
         * @return: For future use. Value can be ignored
         */
        virtual TBool InteractiveElementEntered(TPtrC aElementIdPtr, TUint16 aInteractonTypeList ) = 0;

         /**
         * Notified when a pointer exits from an interactive element.
         * @Since v3.2
         * @param : aElementIdPtr - Pointer to the xml Id of the element 
         *                          if exists else points to an empty string
         * @return: For future use.Value can be ignored
         */

        virtual TBool InteractiveElementExited(TPtrC aElementIdPtr ) = 0;

     };

/**
 * Enumeration to define supported interactive events
 * for the elements.
 */
enum TSvgInteractionEvent
    {
    ESvgInteractiveEventFocusin                            = 0x1,
    ESvgInteractiveEventFocusout                           = 0x2,
    ESvgInteractiveEventActivate                           = 0x4,
    ESvgInteractiveEventClick                              = 0x8,
    ESvgInteractiveEventMousedown                          = 0x10,
    ESvgInteractiveEventMouseup                            = 0x20,
    ESvgInteractiveEventMouseover                          = 0x40,
    ESvgInteractiveEventMousemove                          = 0x80,
    ESvgInteractiveEventMouseout                           = 0x100
    };

#endif /*__INC_MSVGLISTENER__*/
