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
* Description:   Implements Pointer event handler
*
*/



#ifndef WEBPOPUPDRAWER_H
#define WEBPOPUPDRAWER_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>
#include <w32std.h>


// MACROS

// FORWARD DECLARATIONS
class WebView;
class WebFrame;

// CLASS DECLARATION
class WebPopupDrawer: public CBase
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        */
        static WebPopupDrawer* NewL( WebView& webView, TPtrC& textToDraw, TRect& elRect );

        /**
        * Destructor.
        */
        virtual ~WebPopupDrawer();

    private:  // Constructors

        /**
        * C++ default constructor.
        */
        WebPopupDrawer( WebView& webView, TRect& elRect );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void constructL(TPtrC& textToDraw);

    public: // New functions

        /**
        * @since 3.2
        * @param
        * @return
        */
        void drawPopup();

        /**
        * @since 3.2
        * @param
        * @return
        */
        void removePopup();

        /**
        * @since 3.2
        * @param
        * @return
        */
        void sizeChangedL();

        /**
        * HandleOfferKeyEventL
        * Handles key events
        * @since 3.2
        * @return EKeyWasConsumed
        */
        TKeyResponse handleOfferKeyEventL( const TKeyEvent& keyEvent, TEventCode eventCode );

        /**
        * HandlePointerEventL
        * Handles pointer events
        * @since 3.2
        * @return 
        */
        void handlePointerEventL( const TPointerEvent& pointerEvent);

    private: // New functions

        /**
        * @since 3.2
        * @param
        * @return
        */
        void prepareTextL();

        /**
        * @since 3.2
        * @param
        * @return
        */
        void createBitmapAndContextL();

        /**
        * @since 3.2
        * @param
        * @return
        */
        void constructSprite();

        /**
        * @since 3.2
        * @param
        * @return
        */
        void createIconIfNeededL();

    private:    // Data

        // Pointer to owning view
        WebView& m_webView; // not owned
        CPeriodic* m_removePopupPeriodic; // owned
        HBufC* m_textToDraw; // owned
        CArrayFixFlat<TInt>* m_lineWidthArray; // owned
        const CFont* m_font; // owned
        CArrayFixFlat<TPtrC>* m_wrappedArray; //owned
        CGulIcon* m_icon; // owned
        RWsSprite m_sprite;
        CFbsBitmapDevice* m_spriteBitmapDevice;     // owned
        CFbsBitGc* m_spriteBitmapContext; // owned
        CFbsBitmap* m_spriteBitmap; //owned
        CFbsBitmapDevice* m_spriteMaskBitmapDevice;     // owned
        CFbsBitGc* m_spriteMaskBitmapContext; // owned
        CFbsBitmap* m_spriteMaskBitmap; //owned
        TRect m_webViewRect;
        TSize m_size;
        TPoint m_position;
        TRect m_elRect;
    };

#endif      // WEBPOPUPDRAWER_H

// End of File
