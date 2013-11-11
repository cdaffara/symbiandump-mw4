/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Represents the History View
*
*/



#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

//  INCLUDES
#include <e32base.h>
#include <bitdev.h>
#include <coecntrl.h>

enum THistoryViewComponent
    {
    EHistoryComponentNone,
    EHistoryComponentLeftArrow,
    EHistoryComponentRightArrow,
    EHistoryComponentLeftPlaceHolder,
    EHistoryComponentMiddlePlaceHolder,
    EHistoryComponentRightPlaceHolder
    };


// CONSTANTS
const int KCenterThumbnailWidthPercent = 50;
const int KCenterThumbnailHeightPercent = 90;
const int KSideThumbnailWidthPercent = 25;
const int KSideThumbnailHeightPercent = 70;

// FORWARD DECLARATIONS
class HistoryController;
class HistoryEntry;
class WebSurface;

// CLASS DECLARATION

/**
*  Displays the Graphival History List
*
*  @lib browserview.lib
*  @since Series 60 3.0
*/
class HistoryView : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static HistoryView* NewL( HistoryController& historyController, bool previous);

        /**
        * Destructor.
        */
        virtual ~HistoryView();

    public: // New functions

       /**
        *
        * Pass key events to the Browser Control
        * @since 3.0
        * @param aKeyEvent The key event
        * @param aType The key code
        * @return If the Browser Control consumed the key event or not
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aEventCode);

        /**
        * Draws the offscreen bitmap to the screen
        * @since 3.0
        * @param aGc System graphics context
        * @param aRect rectangle to be drawn
        * @return void
        */
        virtual void Draw( const TRect& aRect ) const;

        /**
        * GetCenterEntryTitle
        *
        * @since 3.x
        * @return page title of center entry
        */
        TPtrC getCenterEntryTitle();

        /**
        * CalcRepaintRect
        * Method to do transtion effect when the history view is launched
        * @since 3.x
        * @return ETrue when animation is continuing and EFalse when animation completes
        */
        bool calcRepaintRect();

        /**
        * AnimatePlaceHolderPosition
        * Method to do transtion effect when user navigates in the history view
        * @since 3.x
        * @return ETrue when animation is continuing and EFalse when animation completes
        */
        bool animatePlaceHolderPosition();

        /**
        * AnimatePlaceHolderPosition
        * Method to do transtion effect when user navigates in the history view
        * @since 3.x
        * @return ETrue when animation is continuing and EFalse when animation completes
        */
        int historyLoadOffset() { return ( m_centerPageIndex-m_historyStackIndex ); }

        /**
        * HandlePointerEventL
        * From CCoeControl
        *
        */
        virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        /**
        * AutoScroll
        * Called v=by timer callback when user presses an arrow and does not let go
        * @since 3.2
        * @param
        * @return
        */
        void autoScroll();
        
        /**
        * Perform transition
        * Method to do layout of the history view place holders
        * @since 3.x
        * @return void
        */
        void performTransition(int dir);
        
        void updateState(int dir);
       

    private:

        /**
        * Constructor
        * @since 3.x
        * @param aWebKitControl
        * @param aRect
        * @param aMode display mode
        */
        HistoryView( HistoryController& aHistoryController, bool aPrevious );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

        /**
        * Updates the offscreen bitmap with the thumbnails to be drawn
        * @since 3.x
        * @return void
        */
        void updateDisplay();

        /**
        * Creates offscreen bitmap
        * @since 3.x
        * @return void
        */
        void updateOffscreenBitmapL( );

        /**
        *   From CCoeControl, CountComponentControls
        */
        int CountComponentControls() const {return 0;}

        /**
        *   From CCoeControl, SizeChanged
        */
        virtual void SizeChanged();

        /**
        *   From CCoeControl, ComponentControl
        */
        CCoeControl* ComponentControl(int /*aIndex*/) const { return NULL;}

        /**
        * CalculateLayout
        * Method to do layout of the history view place holders
        * @since 3.x
        * @return void
        */
        void calculateLayout();

        /**
        * LoadUrl
        * Method to load an url
        * @since 3.x
        * @return void
        */
        void loadUrl();

    private:    // Data


        CFbsBitmapDevice*        m_bitmapDevice;     // owned

        WebSurface*              m_renderTarget;        // no owned

        CFbsBitGc*               m_bitmapContext; // owned

        HistoryController*       m_historyController;// not owned

        // indicates the index of the center thumbnail
        int                      m_centerPageIndex;

        TRect                    m_offscreenRect;

        // represents the entry corresponding to the selected page in view
        HistoryEntry*            m_centerEntry; // not owned
        // timer used to do animation
        CIdle*                   m_repaintTimer;
        //repaint rect using during animating initial display of history view
        TRect                    m_repaintRect;
        //resize factor applied during initial display animation
        int                      m_resizeFactor;
        //left place holder rectangle
        TRect                    m_leftPlaceHolderRect;
        //left place holder rectangle
        TRect                    m_centerPlaceHolderRect;
        //left place holder rectangle
        TRect                    m_rightPlaceHolderRect;
        //Left Arrow position
        TPoint                   m_leftArrow[3];
        //Right arrow position
        TPoint                   m_rightArrow[3];
        //Factor used during animation of place holders
        int                      m_placeHolderResizeFactor;
        //Direction of movement
        int                      m_direction;
        //Flag to check if fast animation is needed or not
        bool                     m_fastScroll;
        // Initial position of History Stack
        int                      m_historyStackIndex;
        // Save the last pointer event
        TPointerEvent            m_pointerEvent;
        // Component selected by MouseDown event
        THistoryViewComponent    m_historyViewComponent;
        // Timer for repeated scrolling
        CPeriodic*               m_autoScrollPeriodic;

        TPoint m_lastpointerposition;
    };

#endif      // HISTORYVIEW_H

// End of File
