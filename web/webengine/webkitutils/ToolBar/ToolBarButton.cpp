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

// INCLUDE FILES
#include <e32std.h>
#include <AknUtils.h>
#include <StringLoader.h>
#include "ToolBarButton.h"
#include "ToolBar.h"
#include "w32std.h"
#include "RecentUrlStore.h"
#include <uri16.h>
#include <gulicon.h>
#include <AknBidiTextUtils.h>
#include "brctlinterface.h"
#include <FavouritesItem.h>
#include <FavouritesItemList.h>
#include <FavouritesSession.h>
#include <FavouritesDb.h>

#include "eikon.hrh"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KListItemMargin = 9;
const TInt KListMargin = 10;
const TInt KBorderSize = 5;
const TInt KCursorOffset = 8;
const TInt KDropdownSlidePixel = 20;
_LIT(KTitle,"title");
_LIT(KUrl,"url");
const TInt KMaxBorderAlpha = 128; // how dark is the shadow around the sprite
const TInt KDefaultTransparency = 10; // % transparent
const TInt KToolTipBorder = 4;
const TInt KTextMargin = 3;


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDropDownList::CDropDownList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDropDownList::CDropDownList(
    CDropDownButton& aParent,
    CArrayPtrFlat<HBufC>* aItemText,
    CArrayPtrFlat<CGulIcon>* aItemIcon )
    : iParent( &aParent ), iItemText( aItemText ), iItemIcon( aItemIcon ), iSize( -1, -1 )
    {
    }

// -----------------------------------------------------------------------------
// CDropDownList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDropDownList::ConstructL()
    {
    iAnimTimer = CPeriodic::NewL( CActive::EPriorityHigh );
    }

// -----------------------------------------------------------------------------
// CDropDownList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDropDownList* CDropDownList::NewL(
    CDropDownButton& aParent,
    CArrayPtrFlat<HBufC>* aItemText,
    CArrayPtrFlat<CGulIcon>* aItemIcon )
    {
    CDropDownList *self = new CDropDownList( aParent, aItemText, aItemIcon );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CDropDownList::~CDropDownList()
    {
    if ( iItemText )
        {
        iItemText->ResetAndDestroy();
        delete iItemText;
        }
    if ( iItemIcon )
        {
        iItemIcon->ResetAndDestroy();
        delete iItemIcon;
        }
    if ( iAnimTimer )
        {
        iAnimTimer->Cancel();
        delete iAnimTimer;
        }
    delete iDropDownBitmapDevice;
    delete iDropDownBitmapContext;
    delete iDropDownBitmap;

    delete iSpriteBitmapDevice;
    delete iSpriteBitmapContext;
    delete iSpriteBitmap;

    delete iSpriteMaskBitmapDevice;
    delete iSpriteMaskBitmapContext;
    delete iSpriteMaskBitmap;

    delete iMaskBitmapDevice;
    delete iMaskBitmapContext;
    delete iMaskBitmap;

    iDropSprite.Close();
    }

// -----------------------------------------------------------------------------
// CDropDownList::MakeVisible
//
// -----------------------------------------------------------------------------
//
void CDropDownList::MakeVisible(
    TBool aVisible )
    {
    if ( iVisible == aVisible || iItemText->Count() == 0 )
        {
        return;
        }

    // We have toggled visibility for this dropdown list
    iVisible = aVisible;

    if ( !iVisible )
        {
        // If requesting to close dropdown list, close it without animation
        iAnimTimer->Cancel();
        //iDropSlide = 0;
        iDropSprite.Close();
           iSpriteInitialised = EFalse;
        return;
        }

    if ( !iAnimTimer->IsActive() )
        {
        // If not currently animating, animate dropdown list open
        iAnimTimer->Start( 0, 1000, TCallBack( &DropDownCb, this ) );
        }

    if ( iVisible )
        {
        TBool build( EFalse );
        // get position
        iPosition.iX = iParent->ToolBar().Position().iX + iParent->Position().iX;
        iPosition.iY = iParent->ToolBar().Position().iY + iParent->ToolBar().Size().iHeight - KBorderSize;
        if ( iSize == TSize( -1, -1 ) )
            {
            iSize = CalculateListSize();
            TRAP_IGNORE(CreateBitmapL());
            }
        // adjust position
        CCoeControl& view = iParent->ToolBar().ToolBarCallback().View();
        // out of the view case
        if ( iPosition.iX + iSize.iWidth > view.Size().iWidth )
            {
            iPosition.iX -= ( ( iPosition.iX + iSize.iWidth ) - view.Size().iWidth );
            }
        // y
        if ( iPosition.iY + iSize.iHeight > view.Size().iHeight )
            {
            build = !iDropUp;
            iDropUp = ETrue;
            iPosition.iY = iParent->ToolBar().Position().iY - iSize.iHeight + KBorderSize;
            }
        else
            {
            build = iDropUp;
            iDropUp = EFalse;
            }

        if ( build || iSelectedItem != -1 )
            {
            iSelectedItem = -1;
            TRAP_IGNORE(DrawDropDownListL());
            }

        ConstructSprite();
        }
    }


// -----------------------------------------------------------------------------
// CDropDownList::HandleOfferKeyEventL
//
// -----------------------------------------------------------------------------
//
TKeyResponse CDropDownList::HandleOfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
    {
    if ( !iVisible )
        {
        return EKeyWasNotConsumed;
        }
    // The arrow key events
    if ( aEventCode == EEventKey )
        {
        if (    aKeyEvent.iCode == EKeyLeftUpArrow          // Northwest
             || aKeyEvent.iCode == EStdKeyDevice10          //   : Extra KeyEvent supports diagonal event simulator wedge
             || aKeyEvent.iCode == EKeyUpArrow              // North
             || aKeyEvent.iCode == EKeyRightUpArrow         // Northeast
             || aKeyEvent.iCode == EStdKeyDevice11          //   : Extra KeyEvent supports diagonal event simulator wedge

             || aKeyEvent.iCode == EKeyLeftDownArrow        // Southwest
             || aKeyEvent.iCode == EStdKeyDevice13          //   : Extra KeyEvent supports diagonal event simulator wedge
             || aKeyEvent.iCode == EKeyDownArrow            // South
             || aKeyEvent.iCode == EKeyRightDownArrow       // Southeast
             || aKeyEvent.iCode == EStdKeyDevice12 )        //   : Extra KeyEvent supports diagonal event simulator wedge
            {
            iParent->ToolBar().MakeToolTipVisible(EFalse);
            if (    ( !iDropUp && (    aKeyEvent.iCode == EKeyLeftUpArrow       // Northwest
                                    || aKeyEvent.iCode == EStdKeyDevice10       //   : Extra KeyEvent supports diagonal event sim
                                    || aKeyEvent.iCode == EKeyUpArrow           // North
                                    || aKeyEvent.iCode == EKeyRightUpArrow      // Northeast
                                    || aKeyEvent.iCode == EStdKeyDevice11 ) )   //   : Extra KeyEvent supports diagonal event sim

                 || (  iDropUp && (    aKeyEvent.iCode == EKeyLeftDownArrow     // Southwest
                                    || aKeyEvent.iCode == EStdKeyDevice13       //   : Extra KeyEvent supports diagonal event sim
                                    || aKeyEvent.iCode == EKeyDownArrow         // South
                                    || aKeyEvent.iCode == EKeyRightDownArrow    // Southeast
                                    || aKeyEvent.iCode == EStdKeyDevice12 ) ) ) //   : Extra KeyEvent supports diagonal event sim
                {
                // can go down to -1
                iSelectedItem = --iSelectedItem >= -1 ? iSelectedItem : -1;
                }
            else
                {
                iSelectedItem = ++iSelectedItem > iItemText->Count() - 1 ? 0 : iSelectedItem;
                }
            DrawDropDownListL();
            // update cursor unless we move out of the dropdown
            if ( iSelectedItem != -1 )
                {
                TSize bmSize( iDropDownBitmap->SizeInPixels() );
                const CFont* font = LatinBold16();
                TInt height( font->AscentInPixels() + KListItemMargin );
                TPoint curPos( bmSize.iWidth/2, KListMargin + iSelectedItem * height + KCursorOffset );
                if ( iDropUp )
                    {
                    curPos.iY = bmSize.iHeight - curPos.iY - height + KCursorOffset;
                    }
                curPos+= iPosition;
                iParent->ToolBar().ToolBarCallback().SetAndUpdateCursorPosition( curPos );

                //iParent->ToolBar().Draw(*(CWindowGc*)NULL);
                iParent->ToolBar().Draw();
                iParent->ToolBar().SelectableItemHighlighted();
                }
             else
                {
                iParent->ToolBar().UpdateCursorPosition();
                iParent->ToolBar().ToolBarCallback().View().DrawDeferred();
                }
            }
        else if ( aKeyEvent.iCode == EKeyDevice3 && SelectedItem() != -1 )
            {
            // The activate
            iParent->ActivatedL( SelectedItem() );
            return EKeyWasConsumed;
            }
        }
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CDropDownList::HandlePointerEventL
//
// -----------------------------------------------------------------------------
//
TBool CDropDownList::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    TPointerEvent::TType pEventType = aPointerEvent.iType;
    if ( pEventType == TPointerEvent::EButton1Up )
        {
        TRect listRect(iPosition, iSize);
        if (listRect.Contains(aPointerEvent.iPosition))
            {
            TInt itemCount = iItemText->Count();
            TInt itemHight = (listRect.Height()-KBorderSize)/itemCount;
            TSize itemSize(listRect.Width(), itemHight);
            TPoint cPosition = iPosition;
            iOldSelectedItem = SelectedItem();

            TInt n;
            for ( n = iDropUp ? (itemCount - 1):0; iDropUp ? n >= 0:n <= itemCount ; iDropUp ? n--:n++)
                {
                TRect itemRect(cPosition, itemSize);
                if (itemRect.Contains(aPointerEvent.iPosition))
                    {
                    iSelectedItem = n;
                    DrawDropDownListL();
                    iParent->ToolBar().ToolBarCallback().View().DrawNow();
                    if (SelectedItem() == iOldSelectedItem && iSelectedItem != -1)
                        {
                        iParent->ActivatedL( SelectedItem() );
                        }
                    return EFalse;
                    }
                cPosition.iY += itemHight;
                }
            }
        }
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CDropDownList::Draw
//
// -----------------------------------------------------------------------------
//
//void CDropDownList::Draw(
  //  CWindowGc& /*aGc*/ )
void CDropDownList::Draw()
    {
    if ( iVisible || iAnimTimer->IsActive() )
        {
        TSpriteMember spriteMem;
        TSize bmSize( iSpriteBitmap->SizeInPixels() );
        //
        iSpriteBitmapContext->Clear();
        iSpriteMaskBitmapContext->Clear();
        if ( iDropUp )
            {
            iSpriteBitmapContext->BitBlt( TPoint( 0, bmSize.iHeight - iDropSlide/* - yOffset*/ ), *iDropDownBitmapContext,
                TRect( TPoint( 0, 0 ), TSize( bmSize.iWidth, iDropSlide )  ) );
            iSpriteMaskBitmapContext->BitBlt( TPoint( 0, bmSize.iHeight - iDropSlide /*- yOffset*/ ), iMaskBitmap,
                TRect( TPoint( 0, 0 ), TSize( bmSize.iWidth, iDropSlide )  ) );
            }
        else
            {
            iSpriteBitmapContext->BitBlt( TPoint( 0, 0 ), *iDropDownBitmapContext,
                TRect( TPoint( 0, bmSize.iHeight - iDropSlide  ), TSize( bmSize.iWidth, iDropSlide )  ) );
            iSpriteMaskBitmapContext->BitBlt( TPoint( 0, 0 ), iMaskBitmap,
                TRect( TPoint( 0, bmSize.iHeight - iDropSlide ), TSize( bmSize.iWidth, iDropSlide )  ) );
            }

        spriteMem.iBitmap = iSpriteBitmap;
        spriteMem.iMaskBitmap = iSpriteMaskBitmap;
        if (iSpriteInitialised)
            {
            iDropSprite.UpdateMember(0, spriteMem);
            }
        else
            {
            iSpriteInitialised = ETrue;
            iDropSprite.AppendMember(spriteMem);
            iDropSprite.SetPosition(iPosition);
            iDropSprite.Activate();
            }
        }
    iParent->RefreshToolTip();

    }

// -----------------------------------------------------------------------------
// CDropDownList::UpdateIconsL
//
// -----------------------------------------------------------------------------
//
void CDropDownList::UpdateIconsL(
    CArrayPtrFlat<CGulIcon>* aItemIcon )
    {
    delete iItemIcon;
    iItemIcon = aItemIcon;
    //
    CCoeControl& view = iParent->ToolBar().ToolBarCallback().View();
    TSize size( CalculateListSize() );
    if ( iSize != size )
        {
        TInt diff( size.iWidth - iSize.iWidth );
        iSize = size;
        CreateBitmapL();
        // adjust dropdown x pos
        if ( iPosition.iX - diff > 0 )
            {
            iPosition.iX -= diff;
            }
        }
    DrawDropDownListL();

   iParent->ToolBar().Draw();
    }
// -----------------------------------------------------------------------------
// CDropDownList::DropDownCb
//
// -----------------------------------------------------------------------------
//
void CDropDownList::AnimateDropDown()
    {
    TSize bmSize( iDropDownBitmap->SizeInPixels() );

    if ( iVisible )
        {
        iDropSlide+=KDropdownSlidePixel;
        if ( iDropSlide >= bmSize.iHeight )
            {
            iDropSlide = Min( iDropSlide, bmSize.iHeight );
            iAnimTimer->Cancel();
            }
        }
    else
        {
        iDropSlide-=KDropdownSlidePixel;
        if ( iDropSlide <= 0 )
            {
            iDropSlide = Max( iDropSlide, 0 );
            iAnimTimer->Cancel();
            }
        }

        iParent->ToolBar().Draw();
    }

// -----------------------------------------------------------------------------
// CDropDownList::DropDownCb
//
// -----------------------------------------------------------------------------
//
TInt CDropDownList::DropDownCb(
    TAny* aPtr )
    {
    static_cast<CDropDownList*>(aPtr)->AnimateDropDown();
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CDropDownList::CreateBitmapL
//
// -----------------------------------------------------------------------------
//
void CDropDownList::CreateBitmapL()
    {

    delete iDropDownBitmapDevice;
    iDropDownBitmapDevice = NULL;
    delete iDropDownBitmapContext;
    iDropDownBitmapContext = NULL;
    delete iDropDownBitmap;
    iDropDownBitmap = NULL;
    iDropDownBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError( iDropDownBitmap->Create( iSize, EColor16MA ) );
    iDropDownBitmapDevice = CFbsBitmapDevice::NewL( iDropDownBitmap );
    User::LeaveIfError( iDropDownBitmapDevice->CreateContext( iDropDownBitmapContext ) );

    // draw shadows only once
    TRect r( TPoint( 0, 0 ), iDropDownBitmap->SizeInPixels() );
    iDropDownBitmapContext->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    iDropDownBitmapContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iDropDownBitmapContext->SetPenStyle( CGraphicsContext::ENullPen );
    iDropDownBitmapContext->SetBrushColor( TRgb( 242, 244, 242, 20 ) );
    iDropDownBitmapContext->DrawRect( r );

    iDropDownBitmapContext->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    iDropDownBitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
    iDropDownBitmapContext->SetBrushStyle( CGraphicsContext::ENullBrush );
    // wider shadow to bottom, no shadow to top
    TInt bw = KBorderSize;
    for (TInt n=1;n<=2*bw-1;n++)
        {
        iDropDownBitmapContext->SetPenColor(TRgb(0,0,0,n*n*KMaxBorderAlpha/(2*2*bw*bw)));
        iDropDownBitmapContext->DrawRect( r );
        r.iTl.iX += n%2;
        r.iBr.iX -= n%2;
        r.iBr.iY -= 1;
        }

    delete iSpriteBitmapDevice;
    iSpriteBitmapDevice = NULL;
    delete iSpriteBitmapContext;
    iSpriteBitmapContext = NULL;
    delete iSpriteBitmap;
    iSpriteBitmap = NULL;
    iSpriteBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError( iSpriteBitmap->Create( iSize, EColor16MA ) );
    iSpriteBitmapDevice = CFbsBitmapDevice::NewL( iSpriteBitmap );
    User::LeaveIfError( iSpriteBitmapDevice->CreateContext( iSpriteBitmapContext ) );
    iSpriteBitmapContext->CopySettings(*iDropDownBitmapContext);

    //mask
    delete iSpriteMaskBitmapDevice;
    iSpriteMaskBitmapDevice = NULL;
    delete iSpriteMaskBitmapContext;
    iSpriteMaskBitmapContext = NULL;
    delete iSpriteMaskBitmap;
    iSpriteMaskBitmap = NULL;
    iSpriteMaskBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError( iSpriteMaskBitmap->Create( iSize, EColor256 ) );
    iSpriteMaskBitmapDevice = CFbsBitmapDevice::NewL( iSpriteMaskBitmap );
    User::LeaveIfError( iSpriteMaskBitmapDevice->CreateContext( iSpriteMaskBitmapContext ) );

    delete iMaskBitmapDevice;
    iMaskBitmapDevice = NULL;
    delete iMaskBitmapContext;
    iMaskBitmapContext = NULL;
    delete iMaskBitmap;
    iMaskBitmap = NULL;
    iMaskBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError( iMaskBitmap->Create( iSize, EColor256 ) );
    iMaskBitmapDevice = CFbsBitmapDevice::NewL( iMaskBitmap );
    User::LeaveIfError( iMaskBitmapDevice->CreateContext( iMaskBitmapContext ) );

    // draw shadows only once
    r.SetRect( TPoint( 0, 0 ), iDropDownBitmap->SizeInPixels() );
    iMaskBitmapContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iMaskBitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
    iMaskBitmapContext->SetPenColor( TRgb( 30, 30, 30 ) );
    iMaskBitmapContext->SetBrushColor( TRgb( 30, 30, 30 ) );
    iMaskBitmapContext->DrawRect( r );

    iMaskBitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
    iMaskBitmapContext->SetBrushStyle( CGraphicsContext::ENullBrush );

    // wider shadow to bottom, no shadow to top
    for (TInt n=1;n<=2*bw-1;n++)
        {
        TInt grade = 85*n/(2*bw);
        iMaskBitmapContext->SetPenColor(TRgb(255-grade,255-grade,255-grade));
        iMaskBitmapContext->DrawRect( r );
        r.iTl.iX += n%2;
        r.iBr.iX -= n%2;
        r.iBr.iY -= 1;
        }
    }

// -----------------------------------------------------------------------------
// CDropDownList::ConstructSprite
//
// -----------------------------------------------------------------------------
//
void CDropDownList::ConstructSprite()
    {
    CToolBar& toolbar = iParent->ToolBar();
    CCoeControl& view = toolbar.ToolBarCallback().View();

    iDropSprite = RWsSprite(view.ControlEnv()->WsSession());
    RWindowTreeNode *window =  (RDrawableWindow* )toolbar.ToolBarCallback().CCoeControlParent().DrawableWindow();
    iDropSprite.Construct(*window,iPosition,ESpriteNoChildClip);

    }

// -----------------------------------------------------------------------------
// CDropDownList::DrawDropDownListL
//
// -----------------------------------------------------------------------------
//
void CDropDownList::DrawDropDownListL()
    {
    if ( iItemText->Count() )
        {
        TSize bmSize( iDropDownBitmap->SizeInPixels() );

        TRect r( TPoint( KBorderSize, 2 ), iDropDownBitmap->SizeInPixels() - TSize( 2*KBorderSize, 2*KBorderSize ) );
        iDropDownBitmapContext->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        iDropDownBitmapContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
        iDropDownBitmapContext->SetPenStyle( CGraphicsContext::ENullPen );
        iDropDownBitmapContext->SetBrushColor( TRgb( 242, 244, 242, 255*(100-KDefaultTransparency)/100 ) );
        iDropDownBitmapContext->DrawRect( r );

        const CFont* font = LatinBold16();

        iDropDownBitmapContext->UseFont( font );
        iDropDownBitmapContext->SetDrawMode( CGraphicsContext::EDrawModePEN );
        iDropDownBitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
        TInt height( font->AscentInPixels() + KListItemMargin );
        TInt x( KBorderSize + KListMargin );
        x += iItemIcon ? ( height ) : 0;
        TInt y( KListMargin - KListItemMargin );
        for ( TInt i = 0; i < iItemText->Count(); i++ )
            {
            y += height;
            iDropDownBitmapContext->SetPenColor( TRgb( 0,0,0 ) );
            TPoint upperXY( KBorderSize, y - height +  KListItemMargin / 2 );
            if ( iDropUp )
                {
                upperXY.iY = bmSize.iHeight - y - height + KListItemMargin / 2;
                }
            if ( i == iSelectedItem )
                {
                iDropDownBitmapContext->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
                iDropDownBitmapContext->SetBrushStyle( CGraphicsContext::ESolidBrush );

                TSize selSize( bmSize.iWidth - 2*KBorderSize, height );

                for ( TInt j = selSize.iHeight/2; j >= 0 ; j-- )
                    {
                    iDropDownBitmapContext->SetPenColor( TRgb( 50 + j*12, 50 + j*12, 255, 255*(100-KDefaultTransparency)/100) );
                    iDropDownBitmapContext->DrawRect( TRect( upperXY + TPoint( 0, j ), TSize( selSize.iWidth, 1 ) ) );
                    iDropDownBitmapContext->DrawRect( TRect( upperXY + TPoint( 0, selSize.iHeight - j ), TSize( selSize.iWidth, 1 ) ) );
                    }
                iDropDownBitmapContext->SetDrawMode( CGraphicsContext::EDrawModePEN );
                iDropDownBitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
                iDropDownBitmapContext->SetPenColor( TRgb( 0xff, 0xff, 0xff ) );
                }
            // item icon
            if ( iItemIcon && iItemIcon->At( i ) )
                {
                TInt iconSize( Min( height, iItemIcon->At( i )->Bitmap()->SizeInPixels().iHeight ) );

                iDropDownBitmapContext->BitBltMasked( upperXY + TPoint( KBorderSize, height/2 - iconSize/2 ),
                    iItemIcon->At( i )->Bitmap(),
                    TRect( TPoint( 0, 0 ), TSize( iconSize, iconSize ) ),
                    iItemIcon->At( i )->Mask(), EFalse );
                }
            CGraphicsContext::TDrawTextExtendedParam aParam;
            TChar a( (iItemText->At( i ))->Des()[0]);
            TChar::TBdCategory cat = a.GetBdCategory();
            if (cat == TChar::ERightToLeft || cat == TChar::ERightToLeftArabic || cat == TChar::ERightToLeftEmbedding || cat == TChar::ERightToLeftOverride)
               {
               aParam.iParRightToLeft = ETrue;
               iDropDownBitmapContext->DrawTextExtended(*(iItemText->At( i )),TPoint( x, iDropUp ? bmSize.iHeight - y : y ),aParam);
               }
            else
               {
               iDropDownBitmapContext->DrawText( *(iItemText->At( i )), TPoint( x, iDropUp ? bmSize.iHeight - y : y ) );
               }
            }
        iDropDownBitmapContext->DiscardFont();
        }
    }

// -----------------------------------------------------------------------------
// CDropDownList::CalculateListSize
//
// -----------------------------------------------------------------------------
//
TSize CDropDownList::CalculateListSize()
    {
    CToolBar& toolbar = iParent->ToolBar();
    CCoeControl& view = toolbar.ToolBarCallback().View();
    TInt maxWidth( view.Size().iWidth / 3 * 2 );
    TInt maxHeight = Max( toolbar.Position().iY, ( view.Size().iHeight - toolbar.Position().iY - toolbar.Size().iHeight ) );
    const CFont* font = LatinBold16();
    TInt itemHeight( font->AscentInPixels() + KListItemMargin );
    //
    TSize strSize( 0, 2*KBorderSize + 2*KListMargin );
    TInt count( iItemText->Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        TPtr text = iItemText->At( i )->Des();
        // pick the longest text
        TInt width( font->TextWidthInPixels( text ) + 2*KBorderSize + 2*KListMargin );
        if ( strSize.iWidth < width )
            {
            if ( width > maxWidth )
                {
                TextUtils::ClipToFit( text, *font, maxWidth - 2*KBorderSize - 2*KListMargin );
                width = maxWidth;
                }
            strSize.iWidth = width;
            }
        if ( strSize.iHeight + itemHeight > maxHeight )
            {
            // remove extra items
            for ( TInt j = count - 1; j >= i ; j-- )
                {
                HBufC* text = iItemText->At( j );
                iItemText->Delete( j );
                delete text;
                }
            break;
            }
        strSize.iHeight += itemHeight;
        }
    // remove item margin from the last item
    strSize.iHeight -= KListItemMargin;
    if ( iItemIcon )
        {
        // icon is square. use item height for width
        strSize.iWidth += itemHeight;
        }
    return strSize;
    }

// -----------------------------------------------------------------------------
// CToolBarButton::CToolBarButton
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CToolBarButton::CToolBarButton(
    CToolBar& aParent ) : iParent( &aParent )
    {
    }

// -----------------------------------------------------------------------------
// CToolBarButton::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CToolBarButton::BaseConstructL(
    TInt aButtonCmd,
    const TDesC& aToolTip )
    {
    iParent->GetButtonImage(aButtonCmd,iToolBarButton,iToolBarButtonMask);
    iToolTip = aToolTip.AllocL();

    }

// -----------------------------------------------------------------------------
// CToolBarButton::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CToolBarButton* CToolBarButton::NewL(
    CToolBar& aParent,
    TInt aButtonCmd,
    const TDesC& aToolTip )
    {
    CToolBarButton *self = new CToolBarButton( aParent );
    CleanupStack::PushL(self);
    self->BaseConstructL( aButtonCmd, aToolTip );
    CleanupStack::Pop();
    return self;
    }

// Destructor
CToolBarButton::~CToolBarButton()
    {
    delete iToolTip;
    delete iToolBarButton;
    delete iToolBarButtonMask;

    delete iToolTipBitmapDevice;
    delete iToolTipBitmapContext;
    delete iToolTipBitmap;

    delete iToolTipMaskDevice;
    delete iToolTipMaskContext;
    delete iToolTipMask;
    }

// -----------------------------------------------------------------------------
// CToolBarButton::SetFocus
//
// -----------------------------------------------------------------------------
//
void CToolBarButton::SetFocus(
    TBool aFocusOn )
    {
    if ( !aFocusOn)
        {
        ShowToolTip(EFalse);
        }
    }

//-------------------------------------------------------------------------------
// CToolBarButton::ShowToolTip()
// Method that draws the tooltip
//-------------------------------------------------------------------------------
void CToolBarButton::ShowToolTip(TBool aVisible)
    {
    if (aVisible)
        {
        delete iToolTipMask;
        iToolTipMask = 0;
        delete iToolTipBitmap;
        iToolTipBitmap = 0;
        delete iToolTipMaskDevice;
        iToolTipMaskDevice = 0;
        delete iToolTipBitmapDevice;
        iToolTipBitmapDevice = 0;
        delete iToolTipMaskContext;
        iToolTipMaskContext = 0;
        delete iToolTipBitmapContext;
        iToolTipBitmapContext = 0;

        const CFont* myFont = LatinBold13();

        TPoint cp = iParent->ToolBarCallback().CursorPosition();

        TRect rect = iParent->ToolBarCallback().View().Rect();

        TInt sW = iParent->ToolBarCallback().View().Rect().Width();
        TInt sH = iParent->ToolBarCallback().View().Rect().Height();


        TInt w = myFont->TextWidthInPixels(*iToolTip) + 2*KToolTipBorder + 2*KTextMargin;
        TInt h = myFont->HeightInPixels() + 2*KTextMargin  + 2*KToolTipBorder;
        HBufC* visualText= 0;
        TRAPD(err, visualText = HBufC::NewL( iToolTip->Length() + KAknBidiExtraSpacePerLine);
        //
        TPtr visualTextPtr = visualText->Des();
        // convert the text to lines
        AknBidiTextUtils::ConvertToVisualAndClip( *iToolTip,visualTextPtr,*myFont, w,w););

        if ( err != KErrNone)
            {
            return;
            }

        TPoint p;
        p.iX = Min(sW - w, Max(0,cp.iX - w/2));
        p.iY = Max(iParent->Position().iY - h,0);
        if (p.iY == 0)
            {
            // show tool tip below toolbar
            p.iY = iParent->Position().iY + iParent->Size().iHeight;
            }

        TRect boundingRect(TPoint(0,0),TSize(w,h));

        //mask
        TRAP( err, 
            {
            iToolTipMask = new (ELeave) CFbsBitmap;
            if(KErrNone == iToolTipMask->Create( TSize(w,h), EColor16MA ))
                {
                iToolTipMaskDevice = CFbsBitmapDevice::NewL( iToolTipMask );
                User::LeaveIfError( iToolTipMaskDevice->CreateContext( iToolTipMaskContext ) );
                }
            }
        );
        if( KErrNone != err)
            {
            delete iToolTipMask;
            iToolTipMask = NULL;
            delete iToolTipMaskDevice;
            iToolTipMaskDevice = NULL;
            }
        else
            {
            iToolTipMaskContext->SetPenColor(KRgbBlack);
            iToolTipMaskContext->SetBrushColor(KRgbBlack);
            iToolTipMaskContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
            iToolTipMaskContext->SetPenStyle(CGraphicsContext::ESolidPen);
            iToolTipMaskContext->DrawRoundRect(boundingRect,TSize(4,4));

            //bitmap
            TRAP( err,
                {
                iToolTipBitmap = new (ELeave) CFbsBitmap;
                if( KErrNone == iToolTipBitmap->Create( TSize(w,h), EColor16MA ))
                    {
                    iToolTipBitmapDevice = CFbsBitmapDevice::NewL( iToolTipBitmap );
                    User::LeaveIfError( iToolTipBitmapDevice->CreateContext( iToolTipBitmapContext ) );
                    }
                }
            );
            if( KErrNone != err )
                {
                delete iToolTipBitmap;
                iToolTipBitmap = NULL;
                delete iToolTipBitmapDevice;
                iToolTipBitmapDevice = NULL;
                }
            else
                {
                iToolTipBitmapContext->SetPenColor(KRgbBlack);
                iToolTipBitmapContext->SetBrushColor(TRgb(0xfa,0xfa,0xd2));
                iToolTipBitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
                iToolTipBitmapContext->SetPenStyle(CGraphicsContext::ESolidPen);
                iToolTipBitmapContext->DrawRoundRect(boundingRect,TSize(4,4));

                //add text
                iToolTipBitmapContext->SetPenColor(KRgbBlack);
                iToolTipBitmapContext->SetBrushStyle(CGraphicsContext::ENullBrush);

                iToolTipBitmapContext->UseFont(myFont);
                TPoint pt = boundingRect.iTl;
                pt += TPoint(KToolTipBorder + KTextMargin,KToolTipBorder + KTextMargin);
                iToolTipBitmapContext->DrawText(*visualText,
                    TPoint(pt.iX,pt.iY + myFont->AscentInPixels()));
                iToolTipBitmapContext->DiscardFont();

                CCoeControl& view = iParent->ToolBarCallback().View();

                iToolTipSprite = RWsSprite(view.ControlEnv()->WsSession());
                RWindowTreeNode *window =  (RDrawableWindow* )iParent->ToolBarCallback().CCoeControlParent().DrawableWindow();
                iToolTipSprite.Construct(*window,p, ESpriteNoChildClip);

                TSpriteMember spriteMem;
                spriteMem.iBitmap = iToolTipBitmap;
                spriteMem.iMaskBitmap = iToolTipMask;
                spriteMem.iInvertMask = ETrue;

                iToolTipSprite.AppendMember(spriteMem);
                iToolTipSprite.Activate();
                delete visualText;//visual text
                }
            }
        }
    else  // aVisible = false
        {
        iToolTipSprite.Close();
        }

    iToolTipSpriteVisible = aVisible;
    }

// -----------------------------------------------------------------------------
// CDropDownButton::RefreshToolTip
//
// -----------------------------------------------------------------------------
//
void CToolBarButton::RefreshToolTip()
    {
    if (iToolTipSpriteVisible)
        {
        ShowToolTip(EFalse);// hide
        }
    }

// -----------------------------------------------------------------------------
// CDropDownButton::CDropDownButton
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDropDownButton::CDropDownButton(
    CToolBar& aParent ) : CToolBarButton( aParent )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CDropDownButton::~CDropDownButton()
    {
    delete iDropDown;
    }

// -----------------------------------------------------------------------------
// CDropDownButton::Focus
//
// -----------------------------------------------------------------------------
//
void CDropDownButton::SetFocus(
    TBool aFocus )
    {
    CToolBarButton::SetFocus( aFocus );
    iDropDown->MakeVisible( aFocus );
    }

// -----------------------------------------------------------------------------
// CDropDownButton::HandleOfferKeyEventL
//
// -----------------------------------------------------------------------------
//
TKeyResponse CDropDownButton::HandleOfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
    {
    return iDropDown->HandleOfferKeyEventL( aKeyEvent, aEventCode );
    }

TBool CDropDownButton::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    return iDropDown->HandlePointerEventL(aPointerEvent);
    }


// -----------------------------------------------------------------------------
// CDropDownButton::Draw
//
// -----------------------------------------------------------------------------
//
//void CDropDownButton::Draw(
  //  CWindowGc& aGc ) const
void CDropDownButton::Draw() const
    {
    iDropDown->Draw();
    }

// -----------------------------------------------------------------------------
// CDropDownButton::ShowToolTip
//
// -----------------------------------------------------------------------------
//
void CDropDownButton::ShowToolTip( )
    {
    if ( iDropDown->SelectedItem() < 0)
       {
       CToolBarButton::ShowToolTip();
       }
    }

// -----------------------------------------------------------------------------
// CBackButton::CBackButton
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVisitedButton::CVisitedButton(
    CToolBar& aParent ) : CDropDownButton( aParent ), iCurrentFavicon( -1 )
    {
    }

// -----------------------------------------------------------------------------
// CVisitedButton::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVisitedButton::ConstructL(
    TInt aButtonCmd,
    const TDesC& aToolTip )
    {
    BaseConstructL( aButtonCmd, aToolTip );

    iRecentUrlStore = CRecentUrlStore::NewL();

    iListIcons = new( ELeave) CArrayPtrFlat<CGulIcon>(5);
    CArrayPtrFlat<HBufC>* listItems = new( ELeave) CArrayPtrFlat<HBufC>(5);
    CleanupStack::PushL( listItems );
    CArrayPtrFlat<HBufC>* hosts = new( ELeave) CArrayPtrFlat<HBufC>(5);
    CleanupStack::PushL( hosts );

    iVisitedUrls = new( ELeave) CArrayPtrFlat<HBufC>(5);

    CDesCArrayFlat* visitedNames = new( ELeave ) CDesCArrayFlat( 20 );
    CleanupStack::PushL( visitedNames );
    CDesCArrayFlat* visitedUrls = new( ELeave ) CDesCArrayFlat( 20 );
    CleanupStack::PushL( visitedUrls );

    iRecentUrlStore->GetData(*visitedUrls, *visitedNames);

    TInt visitedPageCount( visitedUrls->Count() );
    // latest on top
    for ( TInt i = 0; i < visitedPageCount ; i++ )
        {
        HBufC* url = (*visitedUrls)[ i ].AllocLC();
        HBufC* name = (*visitedNames)[ i ].AllocLC();
        TPtr namePtr( name->Des() );
        TPtr urlPtr( url->Des() );
        TUriParser parser;
        TInt status( parser.Parse( urlPtr ) );

        if ( status == KErrNone )
            {
            TPtrC host = parser.Extract( EUriHost );
            TBool found( EFalse );
            for ( TInt j = 0; j < hosts->Count(); j++ )
                {
                if ( host == *(hosts->At( j )) )
                    {
                    found = ETrue;
                    break;
                    }
                }
            if ( !found )
                {
                // save this host
                hosts->AppendL( host.AllocL() );

                // remove the trailing '/' character
                TInt len( urlPtr.Length() - 1 );
                if ( urlPtr[ len ] == '/' )
                    {
                    urlPtr = urlPtr.Left( len );
                    }
                // use url if name is empty
                if ( namePtr.Length() == 0 )
                    {
                    CleanupStack::PopAndDestroy(); // name
                    name = NULL;
                    name = urlPtr.AllocLC();
                    }

                listItems->AppendL( name );
                CleanupStack::Pop(); // name
                iVisitedUrls->AppendL( url );
                CleanupStack::Pop(); // url
                }
            else
                {
                CleanupStack::PopAndDestroy(); // name
                CleanupStack::PopAndDestroy(); // url
                }

            }
        else
            {
            CleanupStack::PopAndDestroy(); // name
            CleanupStack::PopAndDestroy(); // url
            }
        }
    CleanupStack::PopAndDestroy( 3 ); // visitedNames, visitedUrls, hosts

    iDropDown = CDropDownList::NewL( *this, listItems );
    CleanupStack::Pop(); // listItem
    iFaviTimer = CIdle::NewL( CActive::EPriorityLow );
    iFaviTimer->Start( TCallBack( &ReadFaviconCb, this ) );
    }

// -----------------------------------------------------------------------------
// CVisitedButton::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVisitedButton* CVisitedButton::NewL(
    CToolBar& aParent,
    TInt aButtonCmd,
    const TDesC& aToolTip )
    {
    CVisitedButton *self = new CVisitedButton( aParent );
    CleanupStack::PushL(self);
    self->ConstructL(aButtonCmd,aToolTip);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CVisitedButton::~CVisitedButton()
    {
    delete iRecentUrlStore;
    if ( iVisitedUrls )
        {
        iVisitedUrls->ResetAndDestroy();
        delete iVisitedUrls;
        }
    if ( iListIcons )
        {
        iListIcons->ResetAndDestroy();
        delete iListIcons;
        }
    delete iFaviTimer;
    }

// -----------------------------------------------------------------------------
// CVisitedButton::ActivatedL
//
// -----------------------------------------------------------------------------
//
void CVisitedButton::ActivatedL(
    TInt aSelectedItem )
    {
    ToolBar().ToolBarCallback().LoadUrlL( *(iVisitedUrls->At( aSelectedItem )), TBrCtlDefs::ECacheModeNormal );
    ToolBar().ToolBarCallback().CloseToolBarL();
    }
//-------------------------------------------------------------------------------
// CVisitedButton::ReadNextFavicon
//
//-------------------------------------------------------------------------------
TBool CVisitedButton::ReadNextFaviconL()
    {
    TBool next( ++iCurrentFavicon < iVisitedUrls->Count() );
    if ( next )
        {
        CGulIcon* icon = NULL;
        icon = iParent->ToolBarCallback().GetFaviconL( *(iVisitedUrls->At( iCurrentFavicon ) ) );
        CleanupStack::PushL( icon );
        iListIcons->AppendL( icon );
        CleanupStack::Pop(); //icon
        }
    else if ( iListIcons->Count() > 0 )
        {
        iDropDown->UpdateIconsL( iListIcons );
        iListIcons = NULL;
        }
    return next;
    }
//-------------------------------------------------------------------------------
// CVisitedButton::ReadFaviconCb
//
//-------------------------------------------------------------------------------
TBool CVisitedButton::ReadFaviconCb(
    TAny* aAny )
    {
    TBool result( EFalse );
    TRAP_IGNORE( result = static_cast<CVisitedButton*>(aAny)->ReadNextFaviconL() );
    return result;
    }

// -----------------------------------------------------------------------------
// CRssButton::CRssButton
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRssButton::CRssButton(
    CToolBar& aParent ) : CDropDownButton( aParent )
    {
    }

// -----------------------------------------------------------------------------
// CRssButton::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRssButton::ConstructL(
    TInt aButtonCmd,
    const TDesC& aToolTip,
    const RPointerArray<TBrCtlSubscribeTo>& aRssArray )
    {
    BaseConstructL( aButtonCmd, aToolTip );

    CArrayPtrFlat<HBufC>* listItems = new( ELeave) CArrayPtrFlat<HBufC>(5);
    CleanupStack::PushL( listItems );

    iSubscribeToItemsUrls = new( ELeave) CArrayPtrFlat<HBufC>(5);

    // Add the items.
    for ( TInt i = 0; i < aRssArray.Count(); i++ )
        {
        // Resolve the url and append it to iSubscribeToItemsUrls.  The resolved
        // url is added to this list to store the value after this method returns
        // and so the memory can be cleaned up later.
        HBufC* resolvedUrl = ToolBar().ToolBarCallback().ResolveUrlL( aRssArray[i]->Url() );
        CleanupStack::PushL( resolvedUrl );
        iSubscribeToItemsUrls->AppendL(resolvedUrl);
        CleanupStack::Pop();

        if (aRssArray[i]->Title().Length())
            {
            listItems->AppendL( aRssArray[i]->Title().AllocL() );
            }
        else
            {
            listItems->AppendL( aRssArray[i]->Url().AllocL() );
            }
        }

    iDropDown = CDropDownList::NewL( *this, listItems );
    CleanupStack::Pop(); // listItem
    }

// -----------------------------------------------------------------------------
// CRssButton::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRssButton* CRssButton::NewL(
    CToolBar& aParent,
    TInt aButtonCmd,
    const TDesC& aToolTip,
    const RPointerArray<TBrCtlSubscribeTo>& aRssArray )
    {
    CRssButton *self = new CRssButton( aParent );
    CleanupStack::PushL(self);
    self->ConstructL(aButtonCmd,aToolTip,aRssArray );
    CleanupStack::Pop();
    return self;
    }

// Destructor
CRssButton::~CRssButton()
    {
    if (iSubscribeToItemsUrls)
        {
        iSubscribeToItemsUrls->ResetAndDestroy();
        delete iSubscribeToItemsUrls;
        }
    }

// -----------------------------------------------------------------------------
// CRssButton::ActivatedL
//
// -----------------------------------------------------------------------------
//
void CRssButton::ActivatedL(
    TInt aSelectedItem )
    {
    CArrayFixFlat<TPtrC> *attrNames = new (ELeave) CArrayFixFlat<TPtrC>(1);
    CleanupStack::PushL(attrNames);
    CArrayFixFlat<TPtrC> *attrValues = new (ELeave) CArrayFixFlat<TPtrC>(1);
    CleanupStack::PushL(attrValues);

    attrNames->AppendL(KTitle());
    attrNames->AppendL(KUrl());

    const HBufC& text = iDropDown->SelectedItemText();
    TPtrC title;
    title.Set(*(text.AllocL()));
    attrValues->AppendL(title);

    TPtrC url( *iSubscribeToItemsUrls->At( aSelectedItem )->AllocL() );
    attrValues->AppendL(url);

    ToolBar().ToolBarCallback().SendCommandsToClient(TBrCtlDefs::EClientCommandSubscribeToFeeds,*attrNames,*attrValues);

    CleanupStack::PopAndDestroy(2);
    }

////// BOOKMARKS IN TOOLBAR



// -----------------------------------------------------------------------------
// CBookmarksButton::CBookmarksButton
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBookmarksButton::CBookmarksButton(
    CToolBar& aParent ) : CDropDownButton( aParent ), iCurrentFavicon( -1 )
    {
    }

// -----------------------------------------------------------------------------
// CBookmarksButton::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBookmarksButton::ConstructL(
    TInt aButtonCmd,
    const TDesC& aToolTip )
    {
    BaseConstructL( aButtonCmd, aToolTip );

    // MJB: before from favouritesengine/clientserver/internal/...

    // ----------------------------------------------------------
    // CFavouritesEngineTestAppUi::TestSomethingL()
    // ----------------------------------------------------------
    //


    CFavouritesItemList* bookmarks = new (ELeave) CFavouritesItemList();
    CleanupStack::PushL( bookmarks );
    RFavouritesSession session;       ///< Session.

    User::LeaveIfError( session.Connect() );

    RFavouritesDb bookmarksDb;
    TInt err = bookmarksDb.Open( session, KBrowserBookmarks );
    if ( err )
        {
        return;
        }

    CleanupClosePushL<RFavouritesDb>( bookmarksDb );

    err = bookmarksDb.GetAll( *bookmarks, KFavouritesRootUid, CFavouritesItem::EItem);

    CleanupStack::PopAndDestroy();  // Close db.
    session.Close();

    iListIcons = new( ELeave) CArrayPtrFlat<CGulIcon>(5);
    CArrayPtrFlat<HBufC>* listItems = new( ELeave) CArrayPtrFlat<HBufC>(5);
    CleanupStack::PushL( listItems );

    iBookmarkUrls = new( ELeave) CArrayPtrFlat<HBufC>(5);

    TInt bookmarksCount( bookmarks->Count() );
    // latest on top
    for ( TInt i = 0; i < bookmarksCount ; i++ )
        {
        HBufC* url = (*bookmarks)[ i ]->Url().AllocLC();
        HBufC* name = (*bookmarks)[ i ]->Name().AllocLC();

        listItems->AppendL( name );
        CleanupStack::Pop(); // name

        iBookmarkUrls->AppendL( url );
        CleanupStack::Pop(); // url
        }

    iDropDown = CDropDownList::NewL( *this, listItems );
    CleanupStack::Pop(); // listItems
    CleanupStack::PopAndDestroy();  // bookmarks
    iFaviTimer = CIdle::NewL( CActive::EPriorityLow );
    iFaviTimer->Start( TCallBack( &ReadFaviconCb, this ) );
    }

// -----------------------------------------------------------------------------
// CBookmarksButton::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBookmarksButton* CBookmarksButton::NewL(
    CToolBar& aParent,
    TInt aButtonCmd,
    const TDesC& aToolTip )
    {
    CBookmarksButton *self = new CBookmarksButton( aParent );
    CleanupStack::PushL(self);
    self->ConstructL(aButtonCmd,aToolTip);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CBookmarksButton::~CBookmarksButton()
    {
    if ( iBookmarkUrls )
        {
        iBookmarkUrls->ResetAndDestroy();
        delete iBookmarkUrls;
        }
    if ( iListIcons )
        {
        iListIcons->ResetAndDestroy();
        delete iListIcons;
        }
    delete iFaviTimer;
    }

// -----------------------------------------------------------------------------
// CBookmarksButton::ActivatedL
//
// -----------------------------------------------------------------------------
//
void CBookmarksButton::ActivatedL(
    TInt aSelectedItem )
    {
    ToolBar().ToolBarCallback().LoadUrlL( *(iBookmarkUrls->At( aSelectedItem )), TBrCtlDefs::ECacheModeNormal );
    ToolBar().ToolBarCallback().CloseToolBarL();
    }
//-------------------------------------------------------------------------------
// CBookmarksButton::ReadNextFavicon
//
//-------------------------------------------------------------------------------
TBool CBookmarksButton::ReadNextFaviconL()
    {
    TBool next( ++iCurrentFavicon < iBookmarkUrls->Count() );
    if ( next )
        {
        CGulIcon* icon = NULL;
        icon = iParent->ToolBarCallback().GetFaviconL( *(iBookmarkUrls->At( iCurrentFavicon ) ) );
        CleanupStack::PushL( icon );
        iListIcons->AppendL( icon );
        CleanupStack::Pop(); //icon
        }
    else if ( iListIcons->Count() > 0 )
        {
        iDropDown->UpdateIconsL( iListIcons );
        iListIcons = NULL;
        }
    return next;
    }
//-------------------------------------------------------------------------------
// CBookmarksButton::ReadFaviconCb
//
//-------------------------------------------------------------------------------
TBool CBookmarksButton::ReadFaviconCb(
    TAny* aAny )
    {
    TBool result( EFalse );
    TRAP_IGNORE( result = static_cast<CBookmarksButton*>(aAny)->ReadNextFaviconL() );
    return result;
    }

//  End of File






