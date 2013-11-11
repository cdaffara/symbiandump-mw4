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
* Description:  Handles the viewing of a single frame. If the page is not frame
*               enabled, this class is used as the single view.  If frame
*               enabled, there is one instance of this class for each frame.
*
*/

#include "../../bidi.h"
#include <fbs.h>
#include <coemain.h>
#include "WebSprite.h"


/*
 * CWebSprite class serves as a replacement for Symbian RWsSprite 
 * in the cases when transparent window is possible (platform supports it).
 * It's a window owning CCoeControl which window gets highest priority and
 * smallest ordinal number. That allows to CWebSprite be on top of all windows
 * in the same window group. The movement is restricted by boundaries of the
 * parent CCoeControl window or Root Window if parent is NULL.
 * Only one bitmap is supported, so animation is not possible.
 * If platform doesn't support window transparency CWebSprite acts as wrapper
 * around RWsSprite.
 */

// ======================================================================
// CWebSprite::NewL
// ======================================================================
CWebSprite* CWebSprite::NewL(TPoint& aPos, CFbsBitmap* aBitmap, 
                             CFbsBitmap* aMask, TBool aInvertMask)
    {
    CWebSprite* self = new(ELeave)CWebSprite(NULL, aBitmap, aMask, aInvertMask);
    CleanupStack::PushL(self);
    self->ConstructL(aPos);
    CleanupStack::Pop();
    return self;
    }

// ======================================================================
// CWebSprite::NewL
// ======================================================================
CWebSprite* CWebSprite::NewL(CCoeControl* aParent, TPoint& aPos, 
                             CFbsBitmap* aBitmap, CFbsBitmap* aMask, 
			     TBool aInvertMask)
    {
    CWebSprite* self = new(ELeave)CWebSprite(aParent, aBitmap, aMask, aInvertMask);
    CleanupStack::PushL(self);
    self->ConstructL(aParent, aPos);
    CleanupStack::Pop();
    return self;
    }

// ======================================================================
// CWebSprite::BaseConstructL
// ======================================================================
void CWebSprite::BaseConstructL(TPoint& aPos, CFbsBitmap* aBitmap, 
                                CFbsBitmap* aMask, TBool aInvertMask)
    {
    iBitmap = aBitmap;
    iMask = aMask;
    iInvertMask = aInvertMask;
    iParent = NULL;
    ConstructL(aPos);
    }

// ======================================================================
// CWebSprite::BaseConstructL
// ======================================================================
void CWebSprite::BaseConstructL(CCoeControl* aParent, TPoint& aPos, 
                                CFbsBitmap* aBitmap, CFbsBitmap* aMask, 
				TBool aInvertMask)
    {
    iBitmap = aBitmap;
    iMask = aMask;
    iInvertMask = aInvertMask;
    iParent = aParent;
    ConstructL(aParent, aPos);
    }

// ======================================================================
// CWebSprite::CWebSprite()
// ======================================================================
CWebSprite::CWebSprite()
    {
    }

// ======================================================================
// CWebSprite::CWebSprite()
// ======================================================================
CWebSprite::CWebSprite(CCoeControl* aParent, CFbsBitmap* aBitmap, 
                       CFbsBitmap* aMask, TBool aInvertMask) : 
                       iParent(aParent), iBitmap(aBitmap), iMask(aMask), 
		       iInvertMask(aInvertMask), iWin(NULL)
    {    
    }

// ======================================================================
// CWebSprite::ConstructL
// ======================================================================
void CWebSprite::ConstructL(TPoint& aPos)
    {
    ConstructL(NULL, aPos);
    }

// ======================================================================
// CWebSprite::ConstructL
// ======================================================================
void CWebSprite::ConstructL(CCoeControl* aParent, TPoint& aPos)
    {
    TInt err = KErrNone;
    if (aParent)
        {
        CreateWindowL(aParent);
        }
    else
        {
        CreateWindowL();
        }
    
    iWin = &(Window());
    iWin->SetRequiredDisplayMode(EColor16MA);
    err = iWin->SetTransparencyAlphaChannel();
    if (err != KErrNone)
        {
        // Transparency is not possible - close window and use sprites
        CloseWindow();
        iWin = NULL;
        }
    
    if (iWin)
        {
        iWin->SetOrdinalPosition(0, KPasswordWindowGroupPriority - 1);   
        iWin->SetBackgroundColor(TRgb(0,0));    
        if (iBitmap) 
            {
            init(aPos, iBitmap->SizeInPixels());
            ActivateL();
            }
        else
            {
            SetPosition(aPos);
            Hide();
            }
        }
    else
        {
        MakeVisible(EFalse);
        CreateSprite(aPos);
        }
    }

// ======================================================================
// CWebSprite::CreateSprite
// ======================================================================
void CWebSprite::CreateSprite(TPoint& aPos)
    {
    /*
     * We should come here only if iWin is NULL since it will mean that 
     * transparent windowis not possible and we are using sprite
     */
    RWindowTreeNode* window = iParent ? static_cast<RWindowTreeNode*>(iParent->DrawableWindow()) :
                                        static_cast<RWindowTreeNode*>(&(ControlEnv()->RootWin()));
    iSprite = RWsSprite(ControlEnv()->WsSession());
    iSprite.Construct(*window, aPos, ESpriteNoChildClip);

    TSpriteMember spriteMem;
    spriteMem.iBitmap = iBitmap;
    spriteMem.iMaskBitmap = iMask;
    spriteMem.iInvertMask = iInvertMask;

    iSprite.AppendMember(spriteMem);
    iSprite.Activate();
    }

// ======================================================================
// CWebSprite::UpdateSprite
// ======================================================================
void CWebSprite::UpdateSprite()
    {
    /*
     * We should come here only if iWin is NULL since it will mean that 
     * transparent windowis not possible and we are using sprite
     */
    TSpriteMember spriteMem;
    spriteMem.iBitmap = iBitmap;
    spriteMem.iMaskBitmap = iMask;
    spriteMem.iInvertMask = iInvertMask;

    iSprite.UpdateMember(0, spriteMem);
    }

// ======================================================================
// CWebSprite::init
// ======================================================================
void CWebSprite::init(TPoint& aPos, TSize aSize)
    {
    if (iWin)
        { // we have a window - let's set position and size
        SetPosition(aPos);
        SetSizeWithoutNotification(aSize);
        }
    }

// ======================================================================
// CWebSprite::Draw
// ======================================================================
void CWebSprite::Draw(const TRect& aRect) const
    {
    if (IsActivated() && iBitmap)
        {
        CWindowGc& gc = SystemGc();       
        gc.Clear(aRect);   
        Window().SetOrdinalPosition(0, KPasswordWindowGroupPriority - 1); 
        TPoint pt = TPoint(0,0);
        TRect rect = TRect(pt, iBitmap->SizeInPixels());
        gc.BitBltMasked(pt, iBitmap, rect, iMask, iInvertMask);
        }
    }

// ======================================================================
// CWebSprite::SetBitmap
// ======================================================================
void CWebSprite::SetBitmap(CFbsBitmap* aBitmap, CFbsBitmap* aMask, TBool aInvertMask)
    {
    iBitmap = aBitmap;
    iMask = aMask;   
    iInvertMask = aInvertMask;
    
    if(iWin && aBitmap)
        {
        TPoint pos = Position();
        init(pos, aBitmap->SizeInPixels());
        }
    }

// ======================================================================
// CWebSprite::Update
// ======================================================================
void CWebSprite::Update(const TPoint& aPos, CFbsBitmap* aBitmap, 
                        CFbsBitmap* aMask)
    {
    SetPos(aPos);
    Update(aBitmap, aMask);
    }

// ======================================================================
// CWebSprite::Update
// ======================================================================
void CWebSprite::Update(CFbsBitmap* aBitmap, CFbsBitmap* aMask)
    {
    TInt err = KErrNone;
    if(iWin && aBitmap) // if we have a window and bitmap check if we need 
        {               // to finish initialization
        if (!IsActivated() || !iBitmap)         // if control wasn't activated 
            {                                   // or bitmap was reset last time to NULL,
            TPoint pos = Position();            // because somebody wanted to reuse this control, 
            init(pos, aBitmap->SizeInPixels()); // we will try to initialize the control.
            TRAP(err, ActivateL());
            }
        }

    if (err == KErrNone)
        {
        iBitmap = aBitmap;
        iMask = aMask;   
        }
    else
        {
        iBitmap = NULL;
        iMask = NULL;    
        }

    if (!iWin)
        {
        UpdateSprite();
        }
    else if (iBitmap)
        {
        Show();
        DrawNow();
        }
    }

// ======================================================================
// CWebSprite::Update
// ======================================================================
void CWebSprite::Update(CFbsBitmap* aBitmap, CFbsBitmap* aMask, 
                        TBool aInvertMask)
    {
    iInvertMask = aInvertMask;
    Update(aBitmap, aMask);
    }

// ======================================================================
// CWebSprite::Show
// ======================================================================
void CWebSprite::Show()
    {
    if (iWin)
        {
        MakeVisible(ETrue);
        }
    else
        {
        UpdateSprite();
        }
    }

// ======================================================================
// CWebSprite::Hide
// ======================================================================
void CWebSprite::Hide()
    {
    if (iWin)
        {
        MakeVisible(EFalse);
        }
    else
        {
        Update(NULL, NULL);
        }
    }

// ======================================================================
// CWebSprite::~CWebSprite
// ======================================================================
CWebSprite::~CWebSprite()
    {
    if (!iWin)
        {
        iSprite.Close();
        }
    }

// ======================================================================
// CWebSprite::SetPos
// ======================================================================
void CWebSprite::SetPos(const TPoint& aPos)
    {
    if (!iWin)
        {
        iSprite.SetPosition(aPos);
        }
    else
        {
        TPoint pos = Position();
        if (pos != aPos) 
            {
            CCoeControl::SetPosition(aPos);
            }
        }
    }


// ======================================================================
// CWebSprite::IsShown
// ======================================================================
TBool CWebSprite::IsShown()
    {
    // if it's a window owning CoeControl just return IsVisible()
    // otherwise check if bitmap != NULL and if it's not then
    // sprite is shown.
    return (iWin && IsVisible()) || iBitmap != NULL;
    }
