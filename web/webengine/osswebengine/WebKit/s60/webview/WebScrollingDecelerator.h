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
* Description:   Handles Web Scrolling Deceleration
*
*/


#ifndef WEBSCROLLINGDECELERATOR_H
#define WEBSCROLLINGDECELERATOR_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>
#include <w32std.h>

// MACROS

// FORWARD DECLARATIONS
class WebView;

// CLASS DECLARATION
class WebScrollingDecelerator: public CTimer
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        */
        static WebScrollingDecelerator* NewL(WebView& webView);

        /**
        * Destructor.
        */
        virtual ~WebScrollingDecelerator();

    private:

        /**
        * C++ default constructor.
        */
        WebScrollingDecelerator(WebView& webView);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // Main functions
        /**
        * @since 5.0
        * param pointerEvent
        * return void
        */
        void updatePos();
        
        /**
        * @since 5.0
        * param none
        * return void
        */
        void startDecelL();
        
        /**
        * @since 5.0
        * param none
        * return void
        */
        void stopDecelL() { m_decelelatorSwitch = false; Cancel(); }
        
        /**
        * @since 5.0
        * param none
        * return void
        */
        void RunL();
        
    private:    // Data

        // Pointer to owning view
        WebView& m_webView; // not owned
        
        // Track pointer drag events
        RArray<const TPoint> m_dragPosition;
        
        // scroll delta
        TPoint m_scrolldelta;
        
        // count of number of scoll steps
        TUint m_numscrollsteps;
        
        // decelator status flag
        bool m_decelelatorSwitch;
        
        //normalized current position to minimize rounding error
        TPoint m_normalizedCurrentPosition;
        
    };

#endif      // WEBSCROLLINGDECELERATOR_H

// End of File
