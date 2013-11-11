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
* Description:  The Toolbar class
*
*/


#ifndef __TOOLBAR
#define __TOOLBAR

//  INCLUDES
#include <e32base.h>
#include <bitstd.h>
#include <w32std.h>
#include <coedef.h>
#include <AknsConstants.h>
#include <AknsItemID.h>
#include <AknsConstants.h>
#include "BrCtlDefs.h"
#include <eikmobs.h>
#include <webkitutils.rsg>
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CToolBarButton;
class CPeriodic;
class CWindowGc;
class CCoeControl;
class CFbsBitmapDevice;
class TBrCtlSubscribeTo;
class CGulIcon;
class CEikButtonGroupContainer;

// CLASS DECLARATION

/**
*
*
*  @lib webkit.dll
*  @since 3.1
*/
class MToolBarCallback
    {
    public:
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual TPoint CursorPosition() = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SetAndUpdateCursorPosition(const TPoint& aPoint) = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual const RPointerArray<TBrCtlSubscribeTo>& SubscribeToMenuItemsL() = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual CCoeControl& View() = 0;
        
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual CCoeControl& CCoeControlParent() = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void CloseToolBarL() = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual CGulIcon* GetFaviconL(const TDesC& aUrl) = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void SendCommandsToClient(TBrCtlDefs::TBrCtlClientCommands aCommand,
            const CArrayFix<TPtrC>& aAttributesNames,
            const CArrayFix<TPtrC>& aAttributeValues) = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void LoadUrlL( const TDesC& aUrl, TBrCtlDefs::TBrCtlCacheMode aCacheMode ) = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual HBufC* ResolveUrlL( const TDesC& aUrl) = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void OpenPageViewL() = 0;
        
        
        /**
        *
        * @since 3.2
        * @param
        * @return
        */
        virtual TUint GetBrowserSettingL(TUint aSetting) = 0;

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        virtual void HandleHistoryCommandL(TBrCtlDefs::TBrCtlCommands aCommand) = 0;

        /**
        *
        * @since 5.0
        * @param
        * @return
        */
        virtual void ViewImagesL() = 0;

        /**
        *
        * @since 5.0
        * @param
        * @return
        */
        virtual TBool wmlMode() = 0;
    };

/**
*  This is the toolbar class
*  @since 3.1
*/
class CToolBar: public CBase, MEikMenuObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CToolBar* NewL( MToolBarCallback& aToolBarCallBack );

        /**
        * Destructor.
        */
        virtual ~CToolBar();

    public: //from MEikMenuObserver
    
        void ProcessCommandL(TInt aCommand);
        void SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/);
        void DynInitMenuPaneL( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ );

        
    public: // New functions

        /**
        * Draw
        * Draws the toolbar
        * @since 3.1
        * @param CWindowGc& aGc
        * @return void
        */
      //  IMPORT_C void Draw( CWindowGc& aGc ) const;
        IMPORT_C void Draw( ) const;

        /**
        * HandleOfferKeyEventL
        * Handles key events
        * @since 3.1
        * @param aKeyEvent
        * @param param
        * @return if key response has been consumed or not
        */
        IMPORT_C TKeyResponse HandleOfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode param );

        /**
        * AnimateToolBarClosing
        * Starts the tooltip timer
        * @since 3.1
        * @return void
        */
        IMPORT_C void AnimateToolBarClosing(TBool aActivateButton);

        /**
        * HandlePointerEventL
        * Handles pointer events
        * @since 3.1
        * @param aPointerEvent
        * @return void
        */
        IMPORT_C void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        
        /**
        * Draw
        * Animates the toolbar position
        * @since 3.1
        * @return EFalse if animation is done or ETrue if animation is not complete
        */
        TBool ToolBarAnimation();

        /**
        * MakeToolTipVisible
        * Makes the tooltip visible, tooltip is displayed after a delay
        * This method is called by the tooltip timer
        * Also called by the toolbar button drop down list to hide the tool tip
        * @since 3.1
        * @param aVisible show or hide tooltip - default is to show tooltip
        * @return void
        */
        void MakeToolTipVisible(TBool aVisible = ETrue);

        /**
        * StartToolTipTimer
        * Starts the tooltip timer
        * @since 3.1
        * @return void
        */
        void StartToolTipTimer();

        /**
        * Parent
        *
        * @since 3.1
        * @return void
        */
        MToolBarCallback& ToolBarCallback() const { return *iToolBarCallback; }

        /**
        * Size
        *
        * @since 3.1
        * @return void
        */
        TSize Size() const { return iToolBarSize; }

        /**
        * Position
        *
        * @since 3.1
        * @return void
        */
        TPoint Position() const { return iPosition; }
        
        /**
        * UpdateCursorPosition
        * Updates the cursor position based on the focused index
        * @since 3.1
        */
        void UpdateCursorPosition();
        
        /**
        * Cursor Position
        *
        * @since 3.2
        * @return void
        */
        TPoint CursorPosition() const { return iCursorPosition; }

        /**
        * HandleButtonActivation
        * Handles navigation inside the toolbar
        * @since 3.1
        * @param ETrue if events was consumer EFalse if the event was not consumed
        */
        void HandleButtonActivation();

        /**
        * HandleButtonActivation
        * Handles navigation inside the toolbar
        * @since 3.1
        * @param ETrue if events was consumer EFalse if the event was not consumed
        */
        void GetButtonImage( TInt aImageId, CFbsBitmap*& aImage ,CFbsBitmap*& aImageMask);

        /**
        * SelectableItemHighlighted
        * cursor is highlighting something
        * @since 3.2
        * @param ETrue if highlighted element is selectable. default is true
        */
        void SelectableItemHighlighted( TBool aSelectable = ETrue );
        
        //Enums used to represent button commands
        enum TToolBarButtonType
            {
            EToolBarNone,
            EToolBarRecenUrls,
            EToolBarMiniatureShow,
            EToolBarReload,
            EToolBarFindKeyword,
            EToolBarGoToWebAddress,
            EToolBarManageBookmarks,
            EToolBarListBookmarks,
            EToolBarSaveAsBookmark,
            EToolBarSettings,
            EToolBarPreviousPage,
            EToolBarVisualHistory,
            EToolBarGoToHompage,
            EToolBarZoomIn,
            EToolBarZoomOut,
            EToolBarZoomMode,
            EToolBarRotateScreen,
            EToolBarViewImages,
            EToolBarSavePage,
            EToolBarSwitchWindow,
            EToolBarSubscribeToFeeds,
            EToolBarShowKeymap,
            EToolBarShowHelp,
            EToolBarFullScreen
            };

    protected:

        /**
        * Constructor
        *
        */
        CToolBar(MToolBarCallback& aToolBarCallback);

        /**
        * Two-phase construction
        */
        void ConstructL();

    private:
         /**
        * ConstructSprite
        * Construct Sprite for toolbar bitmap
        * @since 3.1
        */
         void ConstructSprite();
         
        /**
        * HandleCursorMovement
        * Handles navigation inside the toolbar
        * @since 3.1
        * @param lr = 1 Right lr = -1 Left tb = 1 Down tb = -1 Up
        */
        void HandleCursorMovement(TInt lr,TInt tb);

        /**
        * InitToolbarL
        * Initializes the toolbar
        * @since 3.1
        */
        void InitToolbarL();

        /**
        * DrawToolbarToBitmap
        * Draws the empty toolbar to the bitmap
        * @since 3.1
        */
        void DrawToolbarToBitmap();

        /**
        * DrawButtonsToBitmap
        * Draws the buttons to the bitmap
        * @since 3.1
        */
        void DrawButtonsToBitmap();

        /**
        * LoadSvg
        * Draws the buttons to the bitmap
        * @since 3.1
        */
        void LoadSvg( TInt aImageId, TAknsItemID aAknsId, const TDesC& aFileName, CFbsBitmap*& aImage, CFbsBitmap*& aImageMask );

        /**
        * GetCannedImageDirL
        * Draws the buttons to the bitmap
        * @since 3.1
        */
        HBufC* GetCannedImageDirL();

        /**
        * GetCannedImageDirL
        * Draws the buttons to the bitmap
        * @since 3.1
        */
        void LoadResourceFileL();

        /**
        * GetCannedImageDirL
        * Draws the buttons to the bitmap
        * @since 3.1
        */
        TBool CanActivateButton();
        
        /**
        * ButtonOnPointerEvent(const TPointerEvent& aPointerEvent)
        * Activates the Button on pointer events
        * @since 3.1
        * @return void
        */
        void ButtonOnPointerEvent(const TPointerEvent& aPointerEvent);
        
        void CreateCbaL();
        void SetCba(TInt aResourceId);
        
    private:

        //A pointer to the view
        MToolBarCallback* iToolBarCallback;

        //Tool bar bitmap
        CFbsBitmap* iOffScreenBitmap;
        CFbsBitmapDevice* iBitmapDevice;
        CFbsBitGc* iBitmapGc;
        
        //Tool bar mask bitmap
        CFbsBitmap* iMaskBitmap;
        CFbsBitmapDevice* iMaskBitmapDevice;
        CFbsBitGc* iMaskBitmapGc;

        //Array of toolbar buttons
        CArrayPtrFlat<CToolBarButton> *iButtonArray;

        //Position and Size of toolbar
        TSize iToolBarSize;
        TPoint iPosition;
        TPoint iCursorPosition;

        //Current focussed button index
        TInt iFocusedButtonIndex;
        //Saved cursor position
        TPoint iOrigCursorPos;

        //Direction of toolbar animation
        TInt iAnimDirX; // 1 From Left -1 From Right
        TInt iAnimDirY; // 1 From top -1 from Bottom
        
        //AO for the toolbar animationm
        CIdle *iAnimationTimer;

        //Timer for showing the tooltip
        CPeriodic* iToolTipTimer;

        TBool iCanCloseToolBar;

        TBool iActivateButton;

        TKeyEvent iLastEventKey;

        HBufC* iMbmFile;

        HBufC* iSvgMbmFile;

        TInt iResourceFile;
        
        RWsSprite iSprite;
        
        CEikButtonGroupContainer* iCba;
        
        //Button Configuration Array - stores which buttons are on by button index
        CArrayFixFlat<TUint> *iButtonConfigArray;
    };
    
// Toolbar button      translation matrix
// 
//  translates cenrep value to button tooltip string resource id 
//  for each toolbar button
//
//  NOTE: THIS MUST STAY IN SYNC WITH SAME DEFINITION IN browserpreferences.h in UI.
// 
typedef const TInt TToolbarCommandDecodeEntry[2];

#define KToolbarCommandDecodeMatrixCount  23
#define KToolbarCommandDecodeMatrixCmdVal  0  // First entry is the Command Value
#define KToolbarCommandDecodeMatrixStringId  1 // Second entry is the Setting String Id

static const TToolbarCommandDecodeEntry    ToolbarCommandDecodeMatrix[] =
{
    // Index #               Value               Tooltip String Id
    // = cenrep value
       /* 0  */       { CToolBar::EToolBarNone,               0 /* N/A */                          },
       /* 1  */       { CToolBar::EToolBarShowHelp,           R_BROWSER_TOOLTIP_SHOW_HELP          },
       /* 2  */       { CToolBar::EToolBarSwitchWindow,       R_BROWSER_TOOLTIP_SWITCH_WINDOW      },
       /* 3  */       { CToolBar::EToolBarSavePage,           R_BROWSER_TOOLTIP_SAVE_PAGE          },
       /* 4  */       { CToolBar::EToolBarViewImages,         R_BROWSER_TOOLTIP_VIEW_IMAGES        },
       /* 5  */       { CToolBar::EToolBarVisualHistory,      R_BROWSER_TOOLTIP_VISUAL_HISTORY     },
       /* 6  */       { CToolBar::EToolBarManageBookmarks,    R_BROWSER_TOOLTIP_MANAGE_BOOKMARKS   },
       /* 7  */       { CToolBar::EToolBarSubscribeToFeeds,   R_BROWSER_TOOLTIP_SUBSCRIBE_TO_FEEDS },
       /* 8  */       { CToolBar::EToolBarRotateScreen,       R_BROWSER_TOOLTIP_ROTATE_SCREEN      },
       /* 9  */       { CToolBar::EToolBarSettings,           R_BROWSER_TOOLTIP_SETTINGS           },
       /* 10 */       { CToolBar::EToolBarZoomOut,            R_BROWSER_TOOLTIP_ZOOM_OUT           },
       /* 11 */       { CToolBar::EToolBarZoomIn,             R_BROWSER_TOOLTIP_ZOOM_IN            },
       /* 12 */       { CToolBar::EToolBarGoToHompage,        R_BROWSER_TOOLTIP_GO_TO_HOMPAGE      },
       /* 13 */       { CToolBar::EToolBarPreviousPage,       R_BROWSER_TOOLTIP_PREVIOUS_PAGE      },
       /* 14 */       { CToolBar::EToolBarMiniatureShow,      R_BROWSER_TOOLTIP_MINIATURE_SHOW     },
       /* 15 */       { CToolBar::EToolBarReload,             R_BROWSER_TOOLTIP_RELOAD             },
       /* 16 */       { CToolBar::EToolBarFindKeyword,        R_BROWSER_TOOLTIP_FIND_KEYWORD       },
       /* 17 */       { CToolBar::EToolBarRecenUrls,          R_BROWSER_TOOLTIP_RECENT_URLS        }, 
       /* 18 */       { CToolBar::EToolBarSaveAsBookmark,     R_BROWSER_TOOLTIP_SAVE_AS_BOOKMARK   },
       /* 19 */       { CToolBar::EToolBarListBookmarks,      R_BROWSER_TOOLTIP_LIST_BOOKMARKS     }, 
       /* 20 */       { CToolBar::EToolBarGoToWebAddress,     R_BROWSER_TOOLTIP_GO_TO_WEB_ADDRESS  },
       /* 21 */       { CToolBar::EToolBarShowKeymap,         R_BROWSER_TOOLTIP_SHOW_KEYMAP        }, 
       /* 22 */       { CToolBar::EToolBarFullScreen,         R_BROWSER_TOOLTIP_FULL_SCREEN        },                  
};    

#endif
