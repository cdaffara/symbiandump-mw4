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



#ifndef WEBPAGEFULLSCREENHANDLER_H
#define WEBPAGEFULLSCREENHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>
#include <w32std.h>
#include "WebSprite.h"
#include "WebCannedImages.h"

// MACROS

// FORWARD DECLARATIONS
class WebView;
class WebFrame;

// CLASS DECLARATION
class WebPageFullScreenHandler: public CWebSprite  
{
public:  // Constructor and destructor

    /**
     * Two-phased constructor.
     **/
    static WebPageFullScreenHandler* NewL(WebView& webView);

    /**
     * Destructor.
     **/
    virtual ~WebPageFullScreenHandler();

private:  // Constructors

    /**
     * C++ default constructor.
     **/
    WebPageFullScreenHandler(WebView& webView);

    /**
     * By default Symbian 2nd phase constructor is private.
     **/
    void ConstructL();
    
    TPoint CalculatePosition();

public: // New functions


    /**
     * @since 5.0
     * @param
     * @return
     **/
    void  showEscBtnL(void);

    /**
     * @since 5.0
     * @param
     * @return
     **/
    void  hideEscBtnL(void);

    /**
     * @since 5.0
     * @param
     * @return
     **/
     bool isFullScreenMode(void);

    /**
     * @since 5.0
     * @param
     * @return
     **/
    void SizeChanged(void);
    
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
private:   

    // Pointer to owning view

    WebView*                m_webView;     // not owned
    TBool                   m_isTouchDown;
    TCannedImageData        m_buttonIcon;
};

#endif      //WEBPAGEFULLSCREENHANDLER_H

// End of File
