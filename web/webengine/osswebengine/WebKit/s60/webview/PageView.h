/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Page overview control
*
*/



#ifndef PAGEVIEW_H
#define PAGEVIEW_H

//  INCLUDES
#include <e32base.h>
#include <bitdev.h>
#include <coecntrl.h>


// CONSTANTS

// FORWARD DECLARATIONS


// CLASS DECLARATION

class WebView;

/**
*  Displays the Page overview
*
*  @lib browserview.lib
*  @since Series 60 3.0
*/
class CPageView : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPageView* NewL( WebView& aView );

        /**
        * Destructor.
        */
        virtual ~CPageView();

    public: // New functions

        /**
        * Draws the offscreen bitmap to the screen
        * @since 3.0
        * @param aGc System graphics context
        * @param aRect rectangle to be drawn
        * @return void
        */
        virtual void Draw( const TRect& aRect ) const;

        /**
        * HandlePointerEventL
        * From CCoeControl
        *
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);


    private:

        /**
        * Constructor
        * @since 3.x
        * @param aWebKitControl
        * @param aRect
        * @param aMode display mode
        */
        CPageView( WebView& aView );

        /**
        * ConstructL
        * @since 3.x
        */
        void ConstructL( );

        void SizeChanged();

    private:    // Data


        WebView* m_webView;
    };

#endif      // HISTORYVIEW_H

// End of File
