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
* Description:  Declaration of class CCodView.   
*
*/


#ifndef COD_VIEW_H
#define COD_VIEW_H

// INCLUDES

#include <coecntrl.h>

// FORWARD DECLARATION

class CCodData;
class CEikRichTextEditor;
class CEikScrollBarFrame;
class MObjectProvider;
class CStyleList;
class CParagraphStyle;

// CLASS DECLARATION

/**
* Control displaying COD details.
*/
NONSHARABLE_CLASS( CCodView ): public CCoeControl
    {

    public:     // Constructors and destructor

        /**
        * Constructor.
        * @param aMopParent MOP Parent.
        */
        CCodView();

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

        /**
        * Construct from resource. Uses resource STRUCT RTXTED.
        */
        virtual void ConstructFromResourceL( TResourceReader& aReader );
        
        /**
        * Destructor.
        */
        virtual ~CCodView();

    public:     // new methods

        /**
        * Display this data.
        * @param aCod COD flag (show Price).
        * @param aData COD data to display. Ownership not taken.
        */
        void SetDataL( const CCodData& iData, TBool aCod );

        /**
        * Clear view (display nothing).
        */
        void Clear();

    public:     // from CCoeControl

        /**
        * Return minimum size.
        * @return Minimum size.
        */
        TSize MinimumSize();

        /**
        * Size changed. Lay out and draw components.
        */
        void SizeChanged();

        /**
        * Count component controls.
        * @return Number of component controls.
        */
        TInt CountComponentControls() const;

        /**
        * Return component control.
        * @param aIndex Index of component control.
        * @param Component control.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Handle key event.
        * @param aKeyEvent Key event.
        * @param aType Event type.
        * @return Key response.
        */
        TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Draw this control.
        * @param aRect Rectangle to draw.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * Handle resource change.
        * @param aType Resource type.
        */
        void HandleResourceChange( TInt aType );

    private:    // new methods

        /**
        * Update styles (reload fontspecs).
        */
        void SetStylesL();

        /**
        * Add one item to the editor.
        * @param aNameResId Resource id of attribute name resource (TBUF).
        * @param aValue Attribute value.
        */
        void AddLineL( TInt aNameResId, const TDesC& aValue );

        /**
        * Update the scrollbar.
        * @param aTile If ETrue, scrollbar is tiled.
        */
        void UpdateScrollBarL( TBool aTile );

        /**
        * Allocate and return human-readable name for a MIME type.
        * @param MIME type.
        * @return Allocated human-readable name in a buffer. Ownership passed
        * to the caller.
        */
        HBufC* AllocTypeNameL( const TDesC8& aMimeType );

        /**
        * Scroll up.
        */
        void ScrollUpL();

        /**
        * Scroll down.
        */
        void ScrollDownL();

        /**
        * Get MIME type to be displayed. (First type which is 'meaningless'
        * to the user, e.g. leave out wrappers like DRM).
        * @param aData COD data.
        * @return MIME type.
        */
        TPtrC8 DisplayType( const CCodData& aData );

    private:    // data

        CEikRichTextEditor* iEditor;    ///< Editor. Owned.
        CEikScrollBarFrame* iSbFrame;   ///< Scrollbar frame. Owned.
        CStyleList* iStyles;            ///< Styles. Owned.
        CParagraphStyle* iNameStyle;    ///< Attribute name style. Owned.
        CParagraphStyle* iValueStyle;   ///< Attribute value style. Owned.
        HBufC* iUndefinedValue;         ///< "Undefined" value string. Owned.

    };

#endif /* def COD_VIEW_H */
