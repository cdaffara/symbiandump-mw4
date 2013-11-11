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
* Description:  The Toolbar button class
*
*/

#ifndef __TOOLBARBUTTON
#define __TOOLBARBUTTON

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CWindowGc;
class CToolBar;
class CToolBarButton;
class CFbsBitmapDevice;
class CFbsBitGc;
class CFbsBitmap;
class CGulIcon;
class CDropDownButton;
class TBrCtlSubscribeTo;
class CRecentUrlStore;
class CIdle;

// CLASS DECLARATION
/**
*  CDropDownList
*
*  @lib toolbar.dll
*  @since 3.1
*/
class CDropDownList : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEntry object.
        */
        static CDropDownList* NewL( CDropDownButton& aParent, CArrayPtrFlat<HBufC>* aItemText,
            CArrayPtrFlat<CGulIcon>* aItemIcon = NULL );

        /**
        * Destructor.
        */
        virtual ~CDropDownList();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void MakeVisible( TBool aVisible );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetFocus( TBool aFocusOn );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TKeyResponse HandleOfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aEventCode );
        
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool HandlePointerEventL( const TPointerEvent& aPointerEvent );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
      //  void Draw( CWindowGc& aGc ) ;
        void Draw() ;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt SelectedItem() const { return iSelectedItem; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        const HBufC& SelectedItemText() const { return *iItemText->At( iSelectedItem ); }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void UpdateIconsL( CArrayPtrFlat<CGulIcon>* aItemIcon );
    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CDropDownList object.
        */
        CDropDownList( CDropDownButton& aParent, CArrayPtrFlat<HBufC>* aItemText,
                CArrayPtrFlat<CGulIcon>* aItemIcon = NULL );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void AnimateDropDown();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TInt DropDownCb( TAny* aPtr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void DrawDropDownListL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TSize CalculateListSize();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void CreateBitmapL();
        
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ConstructSprite();

    private:    // Data

        CDropDownButton* iParent;   // not owned

        CArrayPtrFlat<HBufC>* iItemText; // owned
        CArrayPtrFlat<CGulIcon>* iItemIcon; // owned

        TBool        iVisible;

        TInt         iDropSlide;

        TPoint       iPosition;

        TSize        iSize;

        TInt         iSelectedItem;
        
        TInt          iOldSelectedItem;

        CPeriodic*   iAnimTimer;

        CFbsBitmapDevice*        iDropDownBitmapDevice;     // owned

        CFbsBitGc*               iDropDownBitmapContext; // owned

        CFbsBitmap*              iDropDownBitmap; //owned
        
        CFbsBitmapDevice*        iSpriteBitmapDevice;     // owned

        CFbsBitGc*               iSpriteBitmapContext; // owned

        CFbsBitmap*              iSpriteBitmap; //owned
        
        CFbsBitmapDevice*        iSpriteMaskBitmapDevice;     // owned

        CFbsBitGc*               iSpriteMaskBitmapContext; // owned

        CFbsBitmap*              iSpriteMaskBitmap; //owned
        
        CFbsBitmapDevice*        iMaskBitmapDevice;     // owned

        CFbsBitGc*               iMaskBitmapContext; // owned

        CFbsBitmap*              iMaskBitmap; //owned
        
        RWsSprite                iDropSprite;

        TBool                    iDropUp;

        TBool                     iSpriteInitialised;
    };

/**
*  This is the toolbar class
*  @since 3.1
*/
class CToolBarButton : public CBase
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CToolBarButton* NewL( CToolBar& aParent, TInt aIconIndex, const TDesC& aToolTip );

        /**
        * Destructor.
        */
        virtual ~CToolBarButton();

        /**
        * SetPosition
        * Sets the position of the toolbar button
        * @since 3.1
        * @param TPoint &aPoint
        * @return void
        */
        void SetPosition( const TPoint& aPoint ) { iPosition = aPoint; }

        /**
        * Position
        * Gets the position of the toolbar button
        * @since 3.1
        * @return TPoint
        */
        TPoint Position() const { return iPosition; }

        /**
        * Size
        * Gets the size of the button
        * @since 3.1
        * @return TPoint
        */
        TSize Size() const { return iToolBarButton->SizeInPixels(); }

        /**
        * Draw
        * Refreshes the button
        * @since 3.1
        * @return CWebKitToolTip
        */
        CToolBar& ToolBar() const { return *iParent; };

        /**
        * ToolBarImage
        * Gets the canned image for the button
        * @since 3.1
        * @return CWebKitToolTip
        */
        CFbsBitmap& ButtonImage() const { return *iToolBarButton; }

        /**
        * ToolBarImage
        * Gets the canned image for the button
        * @since 3.1
        * @return CWebKitToolTip
        */
        CFbsBitmap& ButtonMaskImage() const { return *iToolBarButtonMask; }

        /**
        * HandleOfferKeyEventL
        * Handles key events
        * @since 3.1
        * @return CWebKitToolTip
        */
        virtual TKeyResponse HandleOfferKeyEventL( const TKeyEvent& /*aKeyEvent*/, TEventCode /*aEventCode*/ )
            { return EKeyWasNotConsumed; }

        /**
        * SetFocus
        * Puts focus on the button
        * @since 3.1
        * @return CWebKitToolTip
        */
        virtual void SetFocus( TBool aFocusOn );

        /**
        * Draw
        * Refreshes the button
        * @since 3.1
        * @return CWebKitToolTip
        */
 //       virtual void Draw( CWindowGc& /*aGc*/ ) const {}
        virtual void Draw() const {}

        /**
        * ShowToolTip
        * Method to enable tooltip visibility
        * @since 3.1
        * @param aVisible show or hide tooltip - default is to show tool tip
        * @return void
        */
        virtual void ShowToolTip(TBool aVisible = ETrue); 
        
        /**
        * RefreshToolTip
        * re-draws tool tip
        * @since 3.1
        * @param
        * @return
        */
        virtual void RefreshToolTip();

        /**
        * HandlePointerEventL
        * Handles pointer events
        * @since 3.1
        * @return TBool
        */
        virtual TBool HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ ){ return ETrue; }

    protected:

        /**
        * Two-phased constructor.
        */
        CToolBarButton( CToolBar& aParent );

        /**
        * Two-phased constructor.
        */
        void BaseConstructL( TInt aButtonCmd, const TDesC& aToolTip );

    protected:
        //The canned image for this button
        CFbsBitmap* iToolBarButton;
        //The bitmap mask for this button
        CFbsBitmap* iToolBarButtonMask;
        //A pointer to the tooltip
        HBufC* iToolTip; //owned
        // pointer to the toolbar
        CToolBar* iParent; // not owned
        //Position of this button
        TPoint iPosition;
        
        CFbsBitmapDevice*        iToolTipBitmapDevice;     // owned

        CFbsBitGc*               iToolTipBitmapContext; // owned

        CFbsBitmap*              iToolTipBitmap; //owned
        
        CFbsBitmapDevice*        iToolTipMaskDevice;     // owned

        CFbsBitGc*               iToolTipMaskContext; // owned

        CFbsBitmap*              iToolTipMask; //owned
        
        RWsSprite iToolTipSprite;
        
        TBool iToolTipSpriteVisible;        
        
        TPoint                   iToolTipPos;
        
    };

/**
*  CDropDownButton
*
*  @lib webkit.dll
*  @since 3.0
*/
class CDropDownButton: public CToolBarButton
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CDropDownButton();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TKeyResponse HandleOfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aEventCode );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SetFocus( TBool aFocusOn );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
    //    virtual void Draw( CWindowGc& aGc ) const;
        virtual void Draw() const;
                
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void ActivatedL( TInt aSelectedItem ) = 0;
        
        /**
        * HandlePointerEventL
        * Handles pointer events
        * @since 3.1
        * @return TBool
        */
        virtual TBool HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
        /**
        * ShowToolTip
        * Method to enable tooltip visibility
        * @since 3.1
        * @return CWebKitToolTip
        */
        virtual void ShowToolTip(); 
         
    protected:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CDropDownList object.
        */
        CDropDownButton( CToolBar& aParent );

    protected:

        CDropDownList* iDropDown; // owned
    };

/**
*  CBackButton
*
*  @lib webkit.dll
*  @since 3.0
*/
class CVisitedButton : public CDropDownButton
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEntry object.
        */
        static CVisitedButton* NewL( CToolBar& aParent, TInt aIconIndex, const TDesC& aToolTip );

        /**
        * Destructor.
        */
        virtual ~CVisitedButton();

    protected:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CDropDownList object.
        */
        CVisitedButton( CToolBar& aParent );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TInt aButtonCmd, const TDesC& aToolTip );
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ActivatedL( TInt aSelectedItem );
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool ReadNextFaviconL();
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TBool ReadFaviconCb( TAny* aAny );
    private:

        CArrayPtrFlat<HBufC>* iVisitedUrls; // owned
        //
        CRecentUrlStore* iRecentUrlStore; //owned
        //
        TInt      iCurrentFavicon;
        //
        CArrayPtrFlat<CGulIcon>* iListIcons; // owned
        //
        CIdle*    iFaviTimer;  // owned
    };

/**
*      
*
*  @lib webkit.dll
*  @since 3.0
*/
class CRssButton : public CDropDownButton
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEntry object.
        */
        static CRssButton* NewL( CToolBar& aParent, TInt aIconIndex, const TDesC& aToolTip,
                const RPointerArray<TBrCtlSubscribeTo>& aRssArray  );

        /**
        * Destructor.
        */
        virtual ~CRssButton();

    protected:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return      object.
        */
        CRssButton( CToolBar& aParent );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TInt aButtonCmd, const TDesC& aToolTip, const RPointerArray<TBrCtlSubscribeTo>& aRssArray );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ActivatedL( TInt aSelectedItem );

    private:

        CArrayPtrFlat<HBufC>* iSubscribeToItemsUrls; // owned
    };
    
/**
*  CBookmarksButton
*
*/
class CBookmarksButton : public CDropDownButton
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEntry object.
        */
        static CBookmarksButton* NewL( CToolBar& aParent, TInt aIconIndex, const TDesC& aToolTip );

        /**
        * Destructor.
        */
        virtual ~CBookmarksButton();

    protected:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CDropDownList object.
        */
        CBookmarksButton( CToolBar& aParent );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TInt aButtonCmd, const TDesC& aToolTip );
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ActivatedL( TInt aSelectedItem );
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool ReadNextFaviconL();
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TBool ReadFaviconCb( TAny* aAny );
    private:
    
        CArrayPtrFlat<HBufC>* iBookmarkUrls; // owned
        //
        TInt      iCurrentFavicon;
        //
        CArrayPtrFlat<CGulIcon>* iListIcons; // owned
        //
        CIdle*    iFaviTimer;  // owned
    };
    
#endif
