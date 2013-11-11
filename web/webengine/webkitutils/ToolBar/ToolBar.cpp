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


// INCLUDE FILES
#include <AknUtils.h>
#include <StringLoader.h>
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <AknsItemID.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <eikbtgpc.h>

#include <webkitutils.rsg>
#include <webkitutilsicons.mbg>

#include "ToolBar.h"
#include "ToolBarButton.h"
#include "BrCtlDefs.h"

#include "eikon.hrh"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT( KToolBarSvgFile, "webkitutilsicons.mif" );
_LIT( KToolBarBitmapsFile, "webkitutils.mbm" );

const TInt KMargin = 5;
const TInt KMaxBorderAlpha = 128; // how dark is the shadow around tooltip
const TInt KButtonCount = 7;
const TInt KAnimDeltaX = 25;
const TInt KAnimDeltaY = 12;
const TInt KShadow = 4;
const TInt KToolTipDelay = 1500000;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

//-------------------------------------------------------------------------------
// DoToolTipCb
// C-style TCallback function
//-------------------------------------------------------------------------------
TInt DoToolTipCb(
    TAny* aPtr )
    {
    CToolBar* t = static_cast<CToolBar*>(aPtr);
    t->MakeToolTipVisible();
    return EFalse;
    }

//-------------------------------------------------------------------------------
// Animate
// C-style TCallback function
//-------------------------------------------------------------------------------
TBool Animate(
    TAny* aAny )
    {
    CToolBar* toolBar = static_cast<CToolBar*> (aAny);
    TBool result = toolBar->ToolBarAnimation();
    return result;
    }

// ============================ MEMBER FUNCTIONS ===============================
//-------------------------------------------------------------------------------
// CToolBar* CToolBar::NewL(MToolBarCallBack& aToolBarCallBack)
// Two phase construction
//-------------------------------------------------------------------------------
EXPORT_C CToolBar* CToolBar::NewL(
    MToolBarCallback& aToolBarCallback)
    {
    CToolBar *self = new CToolBar(aToolBarCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

//-------------------------------------------------------------------------------
// CToolBar::CToolBar(MToolBarCallBack& aToolBarCallBack)
// Two phase construction
//-------------------------------------------------------------------------------
CToolBar::CToolBar(MToolBarCallback& aToolBarCallback)
    : iToolBarCallback(&aToolBarCallback),
    iOrigCursorPos(iToolBarCallback->CursorPosition()),
    iCba(NULL)
    {
    }

//-------------------------------------------------------------------------------
// CToolBar::ConstructL()
// Two phase construction
//-------------------------------------------------------------------------------
void CToolBar::ConstructL()
    {
    // move cursor off screen to hide it
    iToolBarCallback->SetAndUpdateCursorPosition(TPoint(-20,-20));

    LoadResourceFileL();
    CreateCbaL();

    iMbmFile = GetCannedImageDirL();
    iMbmFile->Des().Append( KToolBarBitmapsFile );
    iSvgMbmFile = GetCannedImageDirL();
    iSvgMbmFile->Des().Append( KToolBarSvgFile );

    iButtonArray = new (ELeave) CArrayPtrFlat<CToolBarButton>(KButtonCount);

    iButtonConfigArray = new (ELeave) CArrayFixFlat<TUint>(KButtonCount);

    InitToolbarL();

    iOffScreenBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iOffScreenBitmap->Create( iToolBarSize,  EColor16MA) );
    iBitmapDevice = CFbsBitmapDevice::NewL(iOffScreenBitmap);
    User::LeaveIfError(iBitmapDevice->CreateContext(iBitmapGc));

    iMaskBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iMaskBitmap->Create( iToolBarSize,  EColor256) );
    iMaskBitmapDevice = CFbsBitmapDevice::NewL(iMaskBitmap);
    User::LeaveIfError(iMaskBitmapDevice->CreateContext(iMaskBitmapGc));

    DrawToolbarToBitmap();
    DrawButtonsToBitmap();

    TInt x = iOrigCursorPos.iX;
    TInt y = iOrigCursorPos.iY;
    CCoeControl& view = iToolBarCallback->View();
    TRect r(TPoint(0,0),view.Size());

    TInt dx = Min(x , r.iBr.iX - x);
    TInt dy = Min(y , r.iBr.iY - y);
    iAnimDirX = dx >= dy ? 0: (r.iBr.iX - x > x) ? 1 : -1;
    iAnimDirY = dy > dx ? 0: (r.iBr.iY - y > y) ? 1 : -1;

    TInt count = iButtonArray->Count();
    if (count <= 0)
        User::Leave(KErrUnderflow);

    iPosition.iX = iAnimDirX == 0 ? Min(x - iToolBarSize.iWidth/(2*count), r.iBr.iX - iToolBarSize.iWidth)
                   : iAnimDirX == 1 ? - iToolBarSize.iWidth : r.iBr.iX;
    iPosition.iY = iAnimDirY == 0 ? Min(y - iToolBarSize.iHeight/2, r.iBr.iY - iToolBarSize.iHeight)
                   : iAnimDirY == 1 ? - iToolBarSize.iHeight : r.iBr.iY;


    iAnimationTimer = CIdle::NewL( CActive::EPriorityHigh );
    iAnimationTimer->Start(TCallBack(&Animate,this));

    iToolTipTimer = CPeriodic::NewL( CActive::EPriorityLow );
    ConstructSprite();
    UpdateCursorPosition();
    }

// ----------------------------------------------------------------------------
// void CToolBar::InitToolbarL()
// Method to init and layout toolbar buttons
// ----------------------------------------------------------------------------
void CToolBar::InitToolbarL()
    {
    TInt x = KMargin + KShadow;
    TInt y = KMargin;
    TInt h = 0;

    for (TUint i= TBrCtlDefs::ESettingsToolbarButton1Cmd ; i<= TBrCtlDefs::ESettingsToolbarButton7Cmd ; i++ )
        {
        TUint buttonsettingval = iToolBarCallback->GetBrowserSettingL( i );
        TInt buttoncmd = ToolbarCommandDecodeMatrix[buttonsettingval][KToolbarCommandDecodeMatrixCmdVal];
        TInt buttonstring = ToolbarCommandDecodeMatrix[buttonsettingval][KToolbarCommandDecodeMatrixStringId];

        switch (buttoncmd)
            {
            case EToolBarNone:
                {
                break;
                }

            case EToolBarRecenUrls:
                {
                HBufC* visitedToolTip = StringLoader::LoadLC(buttonstring);
                CVisitedButton*visitedButton = CVisitedButton::NewL(*this, buttoncmd,*visitedToolTip);
                CleanupStack::PopAndDestroy();
                visitedButton->SetPosition(TPoint(x,y));
                x += visitedButton->Size().iWidth + KMargin;
                h = Max(h,visitedButton->Size().iHeight);
                iButtonArray->AppendL(visitedButton);
                iButtonConfigArray->AppendL(buttoncmd);
                break;
                }

            case EToolBarListBookmarks:
                {
                HBufC* BookmarksToolTip = StringLoader::LoadLC(buttonstring);
                CBookmarksButton*bookmarksButton = CBookmarksButton::NewL(*this, buttoncmd,*BookmarksToolTip);
                CleanupStack::PopAndDestroy();
                bookmarksButton->SetPosition(TPoint(x,y));
                x += bookmarksButton->Size().iWidth + KMargin;
                h = Max(h,bookmarksButton->Size().iHeight);
                iButtonArray->AppendL(bookmarksButton);
                iButtonConfigArray->AppendL(buttoncmd);
                break;
                }

            case EToolBarSubscribeToFeeds:
                {
                const RPointerArray<TBrCtlSubscribeTo>& feedsArray = iToolBarCallback->SubscribeToMenuItemsL();
                HBufC* feedsToolTip = StringLoader::LoadLC(buttonstring);
                CRssButton* rssButton = CRssButton::NewL(*this, buttoncmd,*feedsToolTip,feedsArray);
                CleanupStack::PopAndDestroy();
                rssButton->SetPosition(TPoint(x,y));
                x += rssButton->Size().iWidth + KMargin;
                h = Max(h,rssButton->Size().iHeight);
                iButtonArray->AppendL(rssButton);
                iButtonConfigArray->AppendL(buttoncmd);
                break;
                }

            case EToolBarMiniatureShow:
            case EToolBarReload:
            case EToolBarFindKeyword:
            case EToolBarSettings:
            case EToolBarGoToWebAddress:
            case EToolBarManageBookmarks:
            case EToolBarSaveAsBookmark:
            case EToolBarPreviousPage:
            case EToolBarVisualHistory:
            case EToolBarGoToHompage:
            case EToolBarZoomIn:
            case EToolBarZoomOut:
            case EToolBarRotateScreen:
            case EToolBarViewImages:
            case EToolBarSavePage:
            case EToolBarSwitchWindow:
            case EToolBarShowKeymap:
            case EToolBarShowHelp:
            case EToolBarFullScreen:
                {
                HBufC* str = StringLoader::LoadLC(buttonstring);
                CToolBarButton* b = CToolBarButton::NewL(*this, buttoncmd, *str);
                CleanupStack::PopAndDestroy();
                b->SetPosition(TPoint(x,y));
                x += b->Size().iWidth + KMargin;
                h = Max(h,b->Size().iHeight);
                iButtonArray->AppendL(b);
                iButtonConfigArray->AppendL(buttoncmd);
                break;
                }

            /* Zoom Mode not currently in use
            case EToolBarZoomMode:
                {
                if ( !iToolBarCallback->wmlMode() )
                    {
                    HBufC* str = StringLoader::LoadLC(buttonstring);
                    CToolBarButton* b = CToolBarButton::NewL(*this, buttoncmd, *str);
                    CleanupStack::PopAndDestroy();
                    b->SetPosition(TPoint(x,y));
                    x += b->Size().iWidth + KMargin;
                    h = Max(h,b->Size().iHeight);
                    iButtonArray->AppendL(b);
                    iButtonConfigArray->AppendL(buttoncmd);
                    }
                break;
                }
            */

            default:
                break;  // shouldn't get here
            }

        } // for loop end

    iToolBarSize = TSize(x + KShadow,h + 2*KMargin + KShadow);
    }

// ----------------------------------------------------------------------------
// CToolBar::DrawToolbarToBitmap()
// Method to draw the empty toolbar to the offscreen bitmap
// ----------------------------------------------------------------------------
void CToolBar::DrawToolbarToBitmap()
    {
    iMaskBitmapGc->SetPenStyle( CGraphicsContext::ESolidPen );
    iMaskBitmapGc->SetPenColor( TRgb( 0, 0, 0 ) );
    iMaskBitmapGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iMaskBitmapGc->SetBrushColor( TRgb( 0, 0, 0 ) );
    iMaskBitmapGc->DrawRect( TRect( TPoint( 0, 0 ), iToolBarSize ) );

    TRect r1(iPosition,iToolBarSize);
    iMaskBitmapGc->SetPenStyle(CGraphicsContext::ESolidPen);
    iMaskBitmapGc->SetBrushStyle(CGraphicsContext::ENullBrush);
    // wider shadow to bottom, no shadow to top
    TInt bw = KShadow;
    for (TInt n=1;n<=2*bw-1;n++)
        {
        TInt grade = 85*n/(2*bw);
        //iMaskBitmapGc->SetPenColor(TRgb(0,0,0,n*n*KMaxBorderAlpha/(2*2*bw*bw)));
        iMaskBitmapGc->SetPenColor(TRgb(255-grade, 255-grade, 255-grade));
        iMaskBitmapGc->DrawRect(r1);
        r1.iTl.iX += n%2;
        r1.iBr.iX -= n%2;
        r1.iBr.iY -= 1;
        }

    iBitmapGc->SetPenStyle( CGraphicsContext::ESolidPen );
    iBitmapGc->SetPenColor( TRgb( 242, 244, 242 ) );
    iBitmapGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iBitmapGc->SetBrushColor( TRgb( 242, 244, 242 ) );
    iBitmapGc->DrawRect( TRect( TPoint( 0, 0 ), iToolBarSize ) );

    TRect r(iPosition,iToolBarSize);
    iBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    iBitmapGc->SetPenStyle(CGraphicsContext::ESolidPen);
    iBitmapGc->SetBrushStyle(CGraphicsContext::ENullBrush);
    // wider shadow to bottom, no shadow to top
    for (TInt n=1;n<=2*bw-1;n++)
        {
        iBitmapGc->SetPenColor(TRgb(0,0,0,n*n*KMaxBorderAlpha/(2*2*bw*bw)));
        iBitmapGc->DrawRect(r);
        r.iTl.iX += n%2;
        r.iBr.iX -= n%2;
        r.iBr.iY -= 1;
        }
    }

// ----------------------------------------------------------------------------
// CToolBar::DrawButtonsToBitmap()
// Method to draw the toolbar buttons to the offscreen bitmap
// ----------------------------------------------------------------------------
void CToolBar::DrawButtonsToBitmap()
    {

    iBitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
    for (TInt i = 0; i < iButtonArray->Count() ;i++)
        {
        CToolBarButton * b = (*iButtonArray)[i];
        TRect bmpPieceRect(b->Position(),b->Size());
        CFbsBitmap& bmp = b->ButtonImage();
        CFbsBitmap& mask = b->ButtonMaskImage();
        if (i == iFocusedButtonIndex)
            {
            iBitmapGc->SetPenStyle( CGraphicsContext::ESolidPen );
            iBitmapGc->SetPenColor( TRgb( 194, 195, 194 ) );
            iBitmapGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
            iBitmapGc->SetBrushColor( TRgb( 194, 195, 194 ) );
            iBitmapGc->DrawRect( bmpPieceRect );
            }
        else
            {
            iBitmapGc->SetPenStyle( CGraphicsContext::ESolidPen );
            iBitmapGc->SetPenColor( TRgb( 242, 244, 242 ) );
            iBitmapGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
            iBitmapGc->SetBrushColor( TRgb( 242, 244, 242 ) );
            iBitmapGc->DrawRect( bmpPieceRect );
            }

        iBitmapGc->BitBltMasked(b->Position(),&bmp, TRect(TPoint(0,0),b->Size()),&mask, ETrue);
        }

    }

// ----------------------------------------------------------------------------
// CToolBar::UpdateCursorPosition()
// Method to update cursor position
// ----------------------------------------------------------------------------
void CToolBar::UpdateCursorPosition()
{
    if (iButtonArray->Count() > iFocusedButtonIndex) {
        CToolBarButton* b = iButtonArray->At(iFocusedButtonIndex);
        if (b) {
            TPoint pt = iPosition + b->Position() + TPoint(b->Size().iWidth/2,b->Size().iHeight/2);
            iCursorPosition = pt;
            SelectableItemHighlighted(CanActivateButton());
        }
    }
}

// ----------------------------------------------------------------------------
// CToolBar::MakeToolTipVisible()
// Method called by the timer to make the tooltip visible
// ----------------------------------------------------------------------------
void CToolBar::MakeToolTipVisible(TBool aVisible)
    {
    iToolTipTimer->Cancel();
    //Show tooltip only if the animation is not running
    if ( !iAnimationTimer->IsActive() )
        {
        CToolBarButton* b = (*iButtonArray)[iFocusedButtonIndex];
        b->ShowToolTip(aVisible);
        }
    }

// ----------------------------------------------------------------------------
// CToolBar::StartToolTipTimer()
// Method to start the tooltip timer
// ----------------------------------------------------------------------------
void CToolBar::StartToolTipTimer()
    {
    iToolTipTimer->Cancel();
    iToolTipTimer->Start( KToolTipDelay, 0, TCallBack( &DoToolTipCb, this ) );
    }

// ----------------------------------------------------------------------------
// CToolBar::ToolBarAnimation()
// Method to do toolbar animation
// ----------------------------------------------------------------------------
TBool CToolBar::ToolBarAnimation()
    {
    TPoint cp = iOrigCursorPos;
    TInt dir = iCanCloseToolBar ? -1 : 1;
    iPosition += TPoint(dir*iAnimDirX*KAnimDeltaX,dir*iAnimDirY*KAnimDeltaY);

    CCoeControl& view = iToolBarCallback->View();

    TBool ret = ETrue;
    if (iCanCloseToolBar)
        {
        TRect r(TPoint(0,0),view.Size());
        TRect tbRect = TRect(iPosition,iToolBarSize);
        ret = r.Intersects(tbRect);
        if (ret)
            {
            iSprite.SetPosition( iPosition );
            }
        else
            {
            if (iActivateButton)
                {
                //Now handle the toolbar activation
                HandleButtonActivation();
                }
            else{
                iSprite.Close();
                TRAP_IGNORE(iToolBarCallback->CloseToolBarL());
                }
            }
        }
    else
        {
        TPoint p = iPosition;
        if (iAnimDirY == 0)
            {
            TInt count = iButtonArray->Count();
            TInt dx = count != 0 ? iToolBarSize.iWidth/(2*count) : 0;
            p.iX = iAnimDirX == 1 ? Min(p.iX,Max(0,cp.iX - dx))
                       : Max(p.iX,Min(cp.iX + dx,view.Size().iWidth - iToolBarSize.iWidth));
            ret = p.iX == iPosition.iX;
            }
        else
            {
            p.iY= iAnimDirY == 1 ? Min(p.iY,Max(0,cp.iY - iToolBarSize.iHeight/2))
                       : Max(p.iY,Min(cp.iY - iToolBarSize.iHeight/2,view.Size().iHeight - iToolBarSize.iHeight));
            ret = p.iY == iPosition.iY;
            }

        if (!ret)
            {
            if (((cp.iX + iToolBarSize.iWidth) > view.Size().iWidth) && 
                (iAnimDirX == 1))
                {
                p.iX = view.Size().iWidth - iToolBarSize.iWidth;
                }
            iPosition = p;
            iSprite.SetPosition( iPosition );
            UpdateCursorPosition();
            CToolBarButton * b = (*iButtonArray)[iFocusedButtonIndex];
            b->SetFocus(ETrue);
            StartToolTipTimer();
            view.DrawNow();
            }
        else
            {
            iSprite.SetPosition( iPosition );
            }
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CToolBar::AnimateToolBarClosing()
// Method to do toolbar animation when closing it
// ----------------------------------------------------------------------------
EXPORT_C void CToolBar::AnimateToolBarClosing(
    TBool aActivateButton )
    {
    iAnimationTimer->Cancel();
    CToolBarButton* b = (*iButtonArray)[iFocusedButtonIndex];
    b->SetFocus(EFalse);
    iCanCloseToolBar = ETrue;
    iActivateButton = aActivateButton;
    iAnimationTimer->Start(TCallBack(&Animate,this));
    }

// ----------------------------------------------------------------------------
// CToolBar::CToolBar()
// Destructor
// ----------------------------------------------------------------------------
CToolBar::~CToolBar()
    {
    if( iToolBarCallback )
        {
        iToolBarCallback->SetAndUpdateCursorPosition(iOrigCursorPos);
        }
    if (iButtonArray != NULL)
        {
        iButtonArray->ResetAndDestroy();
        }
    delete iButtonArray;

    delete iButtonConfigArray;

    delete iBitmapGc;
    delete iBitmapDevice;
    delete iOffScreenBitmap;

    delete iMaskBitmapGc;
    delete iMaskBitmapDevice;
    delete iMaskBitmap;

    if (iAnimationTimer)
        {
        iAnimationTimer->Cancel();
        }
    delete iAnimationTimer;

    if ( iToolTipTimer )
        {
        iToolTipTimer->Cancel();
        }
    delete iToolTipTimer;

    iSprite.Close();
    // remove resource file
    CCoeEnv::Static()->DeleteResourceFile( iResourceFile );

    if (iCba)
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack(iCba->ButtonGroup()->AsControl());
        }
    delete iCba;
    }

void CToolBar::ConstructSprite()
    {
    CCoeControl& view = iToolBarCallback->View();
    iSprite = RWsSprite(view.ControlEnv()->WsSession());
    RWindowTreeNode *window =  (RDrawableWindow* )iToolBarCallback->CCoeControlParent().DrawableWindow();
    iSprite.Construct(*window,iPosition,ESpriteNoChildClip);

    TSpriteMember spriteMem;
    spriteMem.iBitmap = iOffScreenBitmap;
    spriteMem.iMaskBitmap = iMaskBitmap;
    spriteMem.iInvertMask = ETrue;

    iSprite.AppendMember(spriteMem);
    iSprite.Activate();
    }

// ----------------------------------------------------------------------------
// CToolBar::Draw()
// Method to draw the toolbar
// ----------------------------------------------------------------------------
//EXPORT_C void CToolBar::Draw(
  //  CWindowGc& aGc ) const
EXPORT_C void CToolBar::Draw() const
    {
    for (TInt i = 0; i < iButtonArray->Count() ;i++)
        {
        CToolBarButton* b = (*iButtonArray)[i];
        if ( b )
            {
            b->Draw();
            }
        }
    }

// ----------------------------------------------------------------------------
// CToolBar::HandleCursorMovement()
// Method to handle cursor movement
// ----------------------------------------------------------------------------
void CToolBar::HandleCursorMovement(
    TInt aLr,
    TInt aTb )
    {
    TInt count = iButtonArray->Count();
    TInt oldIndex =  iFocusedButtonIndex;
    if ( aTb == 0)
        {
        iFocusedButtonIndex = aLr == 1 ? iFocusedButtonIndex == count - 1 ? 0 : ++iFocusedButtonIndex
                             : iFocusedButtonIndex == 0 ? count - 1 : --iFocusedButtonIndex;
        }

    if ( oldIndex != iFocusedButtonIndex )
       {
       // button hover
        CToolBarButton* oldB = iButtonArray->At( oldIndex );
        CToolBarButton* newB = iButtonArray->At( iFocusedButtonIndex );
        if ( oldB )
            {
            oldB->SetFocus( EFalse );
            }
        if ( newB )
            {
            newB->SetFocus( ETrue );
            }
       UpdateCursorPosition();
       DrawButtonsToBitmap();

       StartToolTipTimer();
       iToolBarCallback->View().DrawNow();
       }
    }

// ----------------------------------------------------------------------------
// CToolBar::CanActivateButton()
// Method to test if a button can be activated
//
// ----------------------------------------------------------------------------
TBool CToolBar::CanActivateButton()
    {
    switch( iButtonConfigArray->At(iFocusedButtonIndex) )
            {
            case EToolBarRecenUrls:
            case EToolBarSubscribeToFeeds:
            case EToolBarListBookmarks:
                {
                return EFalse;
                }

            case EToolBarMiniatureShow:
            case EToolBarReload:
            case EToolBarFindKeyword:
            case EToolBarGoToWebAddress:
            case EToolBarManageBookmarks:
            case EToolBarSaveAsBookmark:
            case EToolBarSettings:
            case EToolBarPreviousPage:
            case EToolBarVisualHistory:
            case EToolBarGoToHompage:
            case EToolBarZoomIn:
            case EToolBarZoomOut:
            // case EToolBarZoomMode:
            case EToolBarRotateScreen:
            case EToolBarViewImages:
            case EToolBarSavePage:
            case EToolBarSwitchWindow:
            case EToolBarShowKeymap:
            case EToolBarShowHelp:
            case EToolBarFullScreen:
                {
                return ETrue;
                }


            default:
                {
                break;
                }
        }
     return EFalse;
    }

// ----------------------------------------------------------------------------
// CToolBar::HandleButtonActivation()
// Method to handle toolbar activation.
// This method contains the behaviour of each button
// ----------------------------------------------------------------------------
void CToolBar::HandleButtonActivation()
    {
    CArrayFixFlat<TPtrC>* attrNames = 0;
    CArrayFixFlat<TPtrC>* attrValues = 0;

    MakeToolTipVisible(EFalse);
    switch( iButtonConfigArray->At(iFocusedButtonIndex) )
            {
            case EToolBarRecenUrls:
                {
                break;
                }

            case EToolBarMiniatureShow:
                {
                TRAP_IGNORE(iToolBarCallback->OpenPageViewL());
                break;
                }

            case EToolBarReload:
                {
                TRAP_IGNORE(iToolBarCallback->HandleHistoryCommandL(TBrCtlDefs::ECommandReload));
                break;
                }

            case EToolBarPreviousPage:
                {
                TRAP_IGNORE(iToolBarCallback->HandleHistoryCommandL(TBrCtlDefs::ECommandOneStepBack));
                break;
                }

            case EToolBarVisualHistory:
                {
                TRAP_IGNORE(iToolBarCallback->HandleHistoryCommandL(TBrCtlDefs::ECommandShowHistory));
                break;
                }

            case EToolBarViewImages:
                {
                TRAP_IGNORE(iToolBarCallback->ViewImagesL());
                TRAP_IGNORE(iToolBarCallback->CloseToolBarL());
                break;
                }

            case EToolBarFindKeyword:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandLaunchFindKeyword,*attrNames,*attrValues);
                break;
                }

            case EToolBarListBookmarks:
                {
                break;
                }

            case EToolBarSubscribeToFeeds:
                {
                break;
                }

            case EToolBarSettings:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandToolbarSettings,*attrNames,*attrValues);
                break;
                }

            case EToolBarShowKeymap:
                {
                // may seem roundabout to go to ui and then back to client but
                // this prevents problem with CBAs and keeps things cleaner
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandToolbarShowKeymap,*attrNames,*attrValues);
                break;
                }

            case EToolBarGoToWebAddress:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandGotoWebAddress,*attrNames,*attrValues);
                break;
                }

            case EToolBarManageBookmarks:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandManageBookmarks,*attrNames,*attrValues);
                break;
                }

            case EToolBarSaveAsBookmark:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandSaveAsBookmark,*attrNames,*attrValues);
                break;
                }

            /* Zoom Mode not currently used
            case EToolBarZoomMode:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandZoomMode,*attrNames,*attrValues);
                break;
                }
            */

            case EToolBarZoomIn:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandZoomIn,*attrNames,*attrValues);
                break;
                }

            case EToolBarZoomOut:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandZoomOut,*attrNames,*attrValues);
                break;
                }

            case EToolBarGoToHompage:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandGoToHompage,*attrNames,*attrValues);
                break;
                }

            case EToolBarRotateScreen:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandRotateScreen,*attrNames,*attrValues);
                break;
                }

            case EToolBarSavePage:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandSavePage,*attrNames,*attrValues);
                break;
                }

            case EToolBarSwitchWindow:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandSwitchWindow,*attrNames,*attrValues);
                break;
                }

            case EToolBarShowHelp:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandShowHelp,*attrNames,*attrValues);
                break;
                }

            case EToolBarFullScreen:
                {
                iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandFullScreen,*attrNames,*attrValues);
                break;
                }

            default:
                {
                break;
                }
        }
    }

// ----------------------------------------------------------------------------
// CToolBar::HandleOfferKeyEventL()
// Method to handle toolbar key events
// This method is the control point for left/right up/down and enter key
// ----------------------------------------------------------------------------
EXPORT_C TKeyResponse CToolBar::HandleOfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
    {

    CToolBarButton* b = iButtonArray->At( iFocusedButtonIndex );

    if ( b && b->HandleOfferKeyEventL( aKeyEvent, aEventCode ) == EKeyWasConsumed )
        {
        return EKeyWasConsumed;
        }

    switch( aEventCode )
        {
        case EEventKeyDown:
            {
            break;
            }
        case EEventKey:
            {
            iLastEventKey = aKeyEvent;
            break;
            }
        case EEventKeyUp:
            {

            switch( iLastEventKey.iCode )
                {
                // The arrow key events:
                case EKeyRightUpArrow:        // Northeast
                case EStdKeyDevice11:         //   : Extra KeyEvent supports diagonal event simulator wedge
                case EKeyRightArrow:          // East
                case EKeyRightDownArrow:      // Southeast
                case EStdKeyDevice12:         //   : Extra KeyEvent supports diagonal event simulator wedge
                    {
                    if ( !iAnimationTimer->IsActive() ) //Do navigation only when the animation timer is not running
                        HandleCursorMovement(+1,0);
                    }
                    break;

                case EKeyLeftUpArrow:         // Northwest
                case EStdKeyDevice10:         //   : Extra KeyEvent supports diagonal event simulator wedge
                case EKeyLeftArrow:           // West
                case EKeyLeftDownArrow:       // Southwest
                case EStdKeyDevice13:         //   : Extra KeyEvent supports diagonal event simulator wedge
                        {
                    if ( !iAnimationTimer->IsActive() )
                        HandleCursorMovement(-1,0);
                    }
                    break;

                // The enter key events. aka activate key
                case EKeyDevice3:
                    {
                    if ( iAnimationTimer->IsActive() )
                        {
                        AnimateToolBarClosing(EFalse);
                        break;
                        }
                    if ( CanActivateButton() )
                        {
                        AnimateToolBarClosing(ETrue);
                        break;
                        }
                    break;
                    }

                default:
                    {
                    break;
                    }

                }
            iLastEventKey.iCode = EKeyNull; // Reset
            }

        default:
            break;
        }

    return EKeyWasConsumed;
    }

// ----------------------------------------------------------------------------
// CToolBar::HandlePointerEventL
// Method to handle toolbar pointer events
// ----------------------------------------------------------------------------
EXPORT_C void CToolBar::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TPointerEvent::TType pEventType = aPointerEvent.iType;
    switch (pEventType)
        {
            case TPointerEvent::EButton1Down:
                {

                TRect tbRect = TRect(iPosition,iToolBarSize);
                if (tbRect.Contains(aPointerEvent.iPosition))
                    {
                    ButtonOnPointerEvent(aPointerEvent);
                    }
                break;
                }

            case TPointerEvent::EButton1Up:
                {

                TRect tbRect = TRect(iPosition,iToolBarSize);
                if (tbRect.Contains(aPointerEvent.iPosition))
                    {
                    ButtonOnPointerEvent(aPointerEvent);
                    HandleButtonActivation();
                    }
                else
                    {
                    CToolBarButton* b = iButtonArray->At(0);
                    CToolBarButton* fb = iButtonArray->At(KButtonCount-1);

                    if (iFocusedButtonIndex == 0)
                        {
                        b = iButtonArray->At(0);
                        }
                    else if (CRssButton* rssb = dynamic_cast<CRssButton*>(fb))
                        {
                        b = iButtonArray->At(KButtonCount-1);
                        }
                    if (b->HandlePointerEventL(aPointerEvent))
                        {
                        TRAP_IGNORE(iToolBarCallback->CloseToolBarL());
                        }
                    }
                break;
                }

            case TPointerEvent::EDrag:
                {
                break;
                }

            case TPointerEvent::EMove:
                {
                break;
                }

            default:
                break;
        }

    }

// ----------------------------------------------------------------------------
// CToolBar::ButtonOnPointerEvent(const TPointerEvent& aPointerEvent)
// Method to activate the toolbar button on pointer events
// ----------------------------------------------------------------------------
void CToolBar::ButtonOnPointerEvent(const TPointerEvent& aPointerEvent)
    {
    CToolBarButton* b = (*iButtonArray)[iFocusedButtonIndex];
    b->SetFocus(EFalse);
    iToolBarCallback->View().DrawNow();

    //find the button to be activated
    for (TInt i = 0; i < iButtonArray->Count() ;i++)
        {
        CToolBarButton* b = (*iButtonArray)[i];
        TRect bTect(iPosition + b->Position(), b->Size());
        if (bTect.Contains(aPointerEvent.iPosition))
            {
            b->SetFocus(ETrue);
            iFocusedButtonIndex = i;
            UpdateCursorPosition();
            MakeToolTipVisible();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCannedImages::GetImage
// Gets the button image
// -----------------------------------------------------------------------------
//
void CToolBar::GetButtonImage(
    TInt aImageId,
    CFbsBitmap*& aImage ,
    CFbsBitmap*& aImageMask)
{
    switch (aImageId)
        {
        case EToolBarRecenUrls:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_urls,
                     KAknsIIDQgnIndiBrowserTbUrls,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarMiniatureShow:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_page,
                     KAknsIIDQgnIndiBrowserTbPage,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarReload:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_reload,
                     KAknsIIDQgnIndiBrowserTbReload,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarFindKeyword:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_find_kw,
                     KAknsIIDQgnIndiBrowserTbFindKw,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarSubscribeToFeeds:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_rss,
                     KAknsIIDQgnIndiBrowserTbRss,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarListBookmarks:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_bm,
                     KAknsIIDQgnIndiBrowserTbBm,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarSettings:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_settings,
                     KAknsIIDQgnIndiBrowserTbSettings,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarGoToWebAddress:
             LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_goto,
                      KAknsIIDQgnIndiBrowserTbGoto,
                      *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarManageBookmarks:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_bm,
                     KAknsIIDQgnIndiBrowserTbBm,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarSaveAsBookmark:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_save_bm,
                     KAknsIIDQgnIndiBrowserTbSaveBm,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarPreviousPage:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_back,
                     KAknsIIDQgnIndiBrowserTbBack,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarVisualHistory:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_view_pages,
                     KAknsIIDQgnIndiBrowserTbViewPages,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarGoToHompage:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_home,
                     KAknsIIDQgnIndiBrowserTbHome,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        /* Zoom Mode not currently used
        case EToolBarZoomMode:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_zoom_in,
                     KAknsIIDQgnIndiBrowserTbZoomIn,
                     *iSvgMbmFile, aImage, aImageMask );
            break;
        */

        case EToolBarZoomIn:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_zoom_in,
                     KAknsIIDQgnIndiBrowserTbZoomIn,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarZoomOut:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_zoom_out,
                     KAknsIIDQgnIndiBrowserTbZoomOut,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarRotateScreen:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_rotate,
                     KAknsIIDQgnIndiBrowserTbRotate,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarViewImages:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_view_images,
                     KAknsIIDQgnIndiBrowserTbViewImages,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarSavePage:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_save_page,
                     KAknsIIDQgnIndiBrowserTbSavePage,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarSwitchWindow:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_switch_win,
                     KAknsIIDQgnIndiBrowserTbSwitchWin,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarShowKeymap:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_key_map,
                     KAknsIIDQgnIndiBrowserTbKeyMap,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarShowHelp:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_browser_tb_help,
                     KAknsIIDQgnIndiBrowserTbHelp,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        case EToolBarFullScreen:
            LoadSvg( EMbmWebkitutilsiconsQgn_indi_button_exit_fs,
                     KAknsIIDQgnIndiBrowserTbFull,
                     *iSvgMbmFile, aImage, aImageMask );
            break;

        default:
            break;

        }   // end of switch
}



// -----------------------------------------------------------------------------
// CToolBar::LoadSvg
// Loads the canned image
// -----------------------------------------------------------------------------
//
void CToolBar::LoadSvg(
    TInt aImageId,
    TAknsItemID aAknsId,
    const TDesC& aFileName,
    CFbsBitmap*& aImage ,
    CFbsBitmap*& aImageMask )
    {
    // This stuff is copied from Kimono

    // Check the device resolution
    CEikonEnv* eikEnv = CEikonEnv::Static();
    CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();

    TUint dpi = screenDev.VerticalTwipsToPixels(KTwipsPerInch);

    // Initialize size to be 12 pixels
    TInt defaultPx = 15;
    TInt px = 15;

    // Since web designers assume a 96 pixel per inch (ppi) display use that as the reference point.
    // Thus the multiplier should be:
    // ppi/96
    // Below is a list of the density of commercially available LCD displays.
    // Density Multiplier:
    // 400 ppi  4.2
    // 210 ppi  2.2
    // 200 ppi  2.1
    // 120 ppi  1.3
    // 100 ppi  1.0
    // 96 ppi   1.0
    if ((200 > dpi) && (dpi >= 120))
        {
        defaultPx = (TInt)((float)defaultPx * 1.7);
        }
    else if ((210 > dpi) && (dpi >= 200))
        {
        defaultPx = (TInt)((float)defaultPx * 2.1);
        }
    else if ((400 > dpi) && (dpi >= 210))
        {
        defaultPx = (TInt)((float)defaultPx * 2.2);
        }
    else if (dpi >= 400)
        {
        defaultPx = (TInt)((float)defaultPx * 4.2);
        }

    // Don't skin icons--at least for now.
    TRAPD(error, AknIconUtils::CreateIconL( aImage,
                                            aImageMask,
                                            aFileName,
                                            aImageId,
                                            aImageId+1));

    if (error!=KErrNone)
        {
        return;
        }


    // adjust the size
    if (dpi < 200)
        {
        px = (TInt)((float)defaultPx * 1.1);
        }
    else
        {
        px = (TInt)((float)defaultPx * 1.3);
        }

    AknIconUtils::SetSize(aImage,TSize(px,px)); //rect.Rect().Size());
    AknIconUtils::SetSize(aImageMask,TSize(px,px)); //rect.Rect().Size());
    }

// -----------------------------------------------------------------------------
// CToolBar::GetCannedImageDirL
// Gets the folder where the canned image is located
// -----------------------------------------------------------------------------
//
HBufC* CToolBar::GetCannedImageDirL( )
    {
    TFileName mbmDrive;
    HBufC* mbmFile = NULL;
    TParse parse;

    // get the drive letter - MBM is on the same drive as this DLL
    Dll::FileName( mbmDrive );
    parse.Set( mbmDrive, NULL, NULL );
    mbmDrive = parse.Drive();

    // allocate memory for MBM file name
    mbmFile = HBufC::NewL( KMaxFileName );

    // assemble the MBM file name
    TPtr ptr( mbmFile->Des() );
    ptr.SetLength( 0 );
    ptr.Append( mbmDrive );
    ptr.Append( KDC_APP_BITMAP_DIR );
    return mbmFile;
    }

//-------------------------------------------------------------------------------
// CToolBar::LoadResourceFileL
// add webkit resource file to the coe environment
//
//-------------------------------------------------------------------------------
void CToolBar::LoadResourceFileL()
    {
    // add webkit resource file to the coe environment
    // load resource file
    TFileName drive;
    TFileName resourceFileName;
    TParse parse;
    Dll::FileName( drive );
    parse.Set( drive, NULL, NULL );

    resourceFileName += parse.Drive();
    resourceFileName += KDC_RESOURCE_FILES_DIR;
    resourceFileName += _L("WebKitUtils.RSC");

    CCoeEnv* coeEnv = CCoeEnv::Static();
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFileName  );
    iResourceFile = coeEnv->AddResourceFileL( resourceFileName );
    }

// ---------------------------------------------------------
// CNTAIDialogWindowControl::ProcessCommandL
// Softkey event handling
// ---------------------------------------------------------
//
void CToolBar::ProcessCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyCancel:
            {
            AnimateToolBarClosing(EFalse);
            break;
            }
        case EAknSoftkeySelect:
            {
            // down and up key events are already handled
            // here we need to fake the EventKey
            TKeyEvent keyEvent;
            keyEvent.iModifiers = 0;
            keyEvent.iRepeats = 0;
            keyEvent.iCode = EKeyDevice3;
            keyEvent.iScanCode = 0;
            HandleOfferKeyEventL(keyEvent, EEventKey);
            break;
            }

        case EAknSoftkeyChange:
            {
            CArrayFixFlat<TPtrC>* attrNames = 0;
            CArrayFixFlat<TPtrC>* attrValues = 0;

            // close toolbar first
               AnimateToolBarClosing(EFalse);
            // Go to Toolbar group Settings directly
            iToolBarCallback->SendCommandsToClient(TBrCtlDefs::EClientCommandToolbarConfigure,*attrNames,*attrValues);
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CToolBar::SetEmphasis
// ---------------------------------------------------------
//
void CToolBar::SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/)
    {
    }

// ---------------------------------------------------------
// CToolBar::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CToolBar::DynInitMenuPaneL( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    }

// ---------------------------------------------------------
// CToolBar::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CToolBar::SelectableItemHighlighted( TBool aSelectable )
    {
    if (aSelectable)
        {
        SetCba(R_TOOLBAR_SELECTABLE_BUTTON);
        }
    else
        {
        SetCba(R_TOOLBAR_NONSELECTABLE_BUTTON);
        }
    }

// ---------------------------------------------------------
// CToolBar::CreateCbaL
// Softkey construction
// ---------------------------------------------------------
//
void CToolBar::CreateCbaL()
    {
    ASSERT(!iCba);
    const TSize screenSize=CCoeEnv::Static()->ScreenDevice()->SizeInPixels();
    iCba = CEikButtonGroupContainer::NewL(CEikButtonGroupContainer::ECba,
                                          CEikButtonGroupContainer::EHorizontal,
                                          this, R_TOOLBAR_SELECTABLE_BUTTON);

    iCba->SetBoundingRect(TRect(screenSize));

    CCoeEnv::Static()->AppUi()->AddToStackL(iCba->ButtonGroup()->AsControl(),
                       ECoeStackPriorityCba,
                       ECoeStackFlagRefusesFocus);
    iCba->DrawableWindow()->SetOrdinalPosition(0);

    ((CEikCba*)iCba->ButtonGroup())->SetSkinBackgroundId(KAknsIIDQsnBgAreaControlIdle);

    iCba->MakeVisible(ETrue);
    }

// ---------------------------------------------------------
// CToolBar::SetCba
// ---------------------------------------------------------
//
void CToolBar::SetCba(TInt aResourceId)
    {
    TRAP_IGNORE(iCba->SetCommandSetL(aResourceId));
    iCba->DrawDeferred();
    }

//  End of File
