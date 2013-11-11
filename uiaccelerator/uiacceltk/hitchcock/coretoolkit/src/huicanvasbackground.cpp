/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   
*
*/



#include "huicanvasbackground.h"
#include "huicanvasgc.h"
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiS60Skin.h>
#include <uiacceltk/HuiCanvasVisual.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiStatic.h>
#include <uiacceltk/HuiEnv.h>

#include <e32cmn.h>


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CHuiCanvasBackground::CHuiCanvasBackground()
    {	    
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHuiCanvasBackground::~CHuiCanvasBackground()
    {
    iBackgroundItems.Close();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHuiCanvasBackground* CHuiCanvasBackground::NewL()
    {
    CHuiCanvasBackground* self = CHuiCanvasBackground::NewLC();
    CleanupStack::Pop(self);
    return self;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHuiCanvasBackground* CHuiCanvasBackground::NewLC()
    {
    CHuiCanvasBackground* self = new (ELeave) CHuiCanvasBackground();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::SetVisual(CHuiCanvasVisual* aVisual)
    {
    iCanvasVisual = aVisual;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::SetClearBackground(CHuiDisplay::TClearMode aClearBackground)
    {
    iClearBackground = aClearBackground;    
    iBackgroundItems.Reset();
    if(aClearBackground == CHuiDisplay::EClearWithSkinBackground)
        {
        TRAP_IGNORE(CHuiStatic::Env().Skin().TextureL(EHuiSkinBackgroundTexture));
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHuiCanvasBackground::ClearBackground()
    {
    return iClearBackground;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

TInt CHuiCanvasBackground::BackgroundItemsCount()
    {
    return iBackgroundItems.Count();        
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::SetBackgroundColor(const TRgb& aBackgroundColor)
    {
    iBackgroundColor = aBackgroundColor;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TRgb CHuiCanvasBackground::BackgroundColor() const
    {
    return iBackgroundColor;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::SetBackgroundItems(const RArray<THuiDisplayBackgroundItem>& aItems)
    {
    iClearBackground = CHuiDisplay::EClearNone;
    iBackgroundItems.Reset();
    for(TInt i = 0; i < aItems.Count(); ++i)
        {
        iBackgroundItems.Append(aItems[i]);
        }   
    if (!CHuiStatic::Env().Renderer() == EHuiRendererBitgdi)
        {
        CHuiS60Skin* s60skin = static_cast<CHuiS60Skin*>(&CHuiStatic::Env().Skin());
        TRAP_IGNORE(s60skin->UpdateBackgroundsL(iBackgroundItems));
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::GetBackgroundItems(RArray<THuiDisplayBackgroundItem>& aItems)
    {
    aItems.Reset();
    for(TInt i = 0; i < iBackgroundItems.Count(); ++i)
        {
        aItems.Append(aItems[i]);
        }   
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect)
    {
	iCanvasVisual->CanvasGc().SetGc(aGc);
	    
    // Clear background for the dirty area
    if (iBackgroundItems.Count() != 0)
        {
        ClearWithBackgroundItems(aDisplayRect);    
        }
    else
        {
        switch (iClearBackground)
            {
            case CHuiDisplay::EClearWithColor:
                {
                ClearWithColor(aGc, aDisplayRect);                            
                break;    
                }
            case CHuiDisplay::EClearWithSkinBackground:
                {
                ClearWithSkinBackground(aDisplayRect);                                                    
                break;    
                }
            case CHuiDisplay::EClearNone:
            default:
                {
                // Don't do anything
                break;    
                }                                                
            }                                    
        }                                                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::ClearWithBackgroundItems(const TRect& aDisplayRect)
    {    
    if (!iCanvasVisual)
        {
        return;    
        }

    CHuiCanvasGc& canvasGc = iCanvasVisual->CanvasGc();
    canvasGc.ClearWithBackgroundItems(aDisplayRect, iBackgroundItems);    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::ClearWithSkinBackground(const TRect& aDisplayRect)
    {    
    if (!iCanvasVisual)
        {
        return;    
        }

    CHuiCanvasGc& canvasGc = iCanvasVisual->CanvasGc();
    canvasGc.ClearWithSkinBackground(aDisplayRect);    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasBackground::ClearWithColor(CHuiGc& /*aGc*/, const TRect& aDisplayRect)
    {    
    if (!iCanvasVisual)
        {
        return;    
        }

    CHuiCanvasGc& canvasGc = iCanvasVisual->CanvasGc();
            
    canvasGc.SetPenColor(iBackgroundColor);
    canvasGc.SetOpacity(TReal32(iBackgroundColor.Alpha()) / 255.f);    
    canvasGc.Clear(aDisplayRect);    
    }
    
    
