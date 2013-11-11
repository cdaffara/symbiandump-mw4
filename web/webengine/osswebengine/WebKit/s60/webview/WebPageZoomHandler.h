/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements page scrolling functionality
*
*/



#ifndef WEBPAGEZOOMHANDLER_H
#define WEBPAGEZOOMHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>
#include <w32std.h>

// MACROS

// FORWARD DECLARATIONS
class WebView;
class WebFrame;
class CAknVolumePopup;

// CLASS DECLARATION
class WebPageZoomHandler: public CBase, MCoeControlObserver {
public:  // Constructor and destructor

    /**
     * Two-phased constructor.
     **/
    static WebPageZoomHandler* NewL(WebView& webView);

    /**
     * Destructor.
     **/
    virtual ~WebPageZoomHandler();

private:  // Constructors

    /**
     * C++ default constructor.
     **/
    WebPageZoomHandler(WebView& webView);

    /**
     * By default Symbian 2nd phase constructor is private.
     **/
    void constructL();
public: 
    /**
    *  From MCoeControlObserver
     **/

    virtual void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);


public: // New functions

    /**
     * @since 5.0
     * @param
     * @return
     **/
    bool handlePageZoomingL(const TPointerEvent& pointerEvent);

    /**
     * @since 5.0
     * @param
     * @return
     **/
    void  showZoomSliderL();

    /**
     * @since 5.0
     * @param
     * @return
     **/
    void  hideZoomSliderL();

    /**
     * @since 5.0
     * @param
     * @return
     **/

    void setZoomLevel(TInt zoomLevel);

    /**
     * @since 5.0
     * @param
     * @return
     **/
    void updateBitmap();

    /**
     * @since 5.0
     * @param
     * @return
     **/
    WebView* view();

    /**
     * @since 5.0
     * @param
     * @return
     **/
    bool isActive();


    int stepSize();

private:   

    // Pointer to owning view

    WebView*         m_webView;     // not owned

    CAknVolumePopup* m_zoomSlider;  // owned

    CPeriodic*       m_bitmapUpdateTimer; // owned 

    int              m_stepSize;
};

#endif      //WEBPAGEZOOMHANDLER_H

// End of File
