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


#ifndef WEBSCROLLINGDECELERATORGH_H
#define WEBSCROLLINGDECELERATORGH_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>
#include <w32std.h>
#include <rt_gestureobserver.h>

// MACROS

// FORWARD DECLARATIONS
class WebView;
class WebScrollbarDrawer;

// CLASS DECLARATION
class WebScrollingDeceleratorGH: public CBase
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        */
        static WebScrollingDeceleratorGH* NewL(WebView& webView);

        /**
        * Destructor.
        */
        virtual ~WebScrollingDeceleratorGH();

    private:

        /**
        * C++ default constructor.
        */
        WebScrollingDeceleratorGH(WebView& webView);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // Main functions
        
        void startDecel(RT_GestureHelper::TRealPoint& speed, WebScrollbarDrawer* scrollbarDrawer);
        
        void cancelDecel();
        
        void scroll();
        
        int getDecceleration();
        
    private:    // Data

        // Pointer to owning view
        WebView& m_webView; // not owned

	// scroll delta
        TPoint m_scrolldelta;
        
        // count of number of scoll steps
        TUint m_numscrollsteps;
        
        // decelator status flag
        bool m_decelelatorSwitch;
        
        //normalized current position to minimize rounding error
        TPoint m_normalizedCurrentPosition;
        
        RT_GestureHelper::TRealPoint m_initSpeed;
        
        CPeriodic*  m_decelTimer;
        
        TPoint m_lastPos;
        
        TPoint m_startPos;
        
        WebScrollbarDrawer* m_scrollbarDrawer;
    };



#endif      // WEBSCROLLINGDECELERATOR_GH_H

// End of File
