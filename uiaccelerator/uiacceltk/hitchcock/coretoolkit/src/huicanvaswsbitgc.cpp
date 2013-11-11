/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiCanvasWsBitGc.
*
*/



#include "huicanvaswsbitgc.h"
#include "uiacceltk/HuiCanvasVisual.h"
#include <graphics/wsgraphicscontext.h>
#include <e32cmn.h>
#include <AknLayoutFont.h>
#include <gdi.h>



CHuiCanvasWsBitGc* CHuiCanvasWsBitGc::NewL()
	{
	CHuiCanvasWsBitGc* self    = new ( ELeave ) CHuiCanvasWsBitGc;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


CHuiCanvasWsBitGc::CHuiCanvasWsBitGc()
    {
    
    }

void CHuiCanvasWsBitGc::ConstructL()
	{
	CHuiCanvasWsSwGc::ConstructL();
	}

CHuiCanvasWsBitGc::~CHuiCanvasWsBitGc()
    {
    }
    

void CHuiCanvasWsBitGc::BeginActionL(TInt aAction, TRect /*aDisplayRect*/, const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion)
	{
	iAction = aAction;
	iVisual = &aUser;

    if (!iVisual)
        {
        return;    
        }        

    // Update area is in screen coordinates so we must convert it 
    iDefaultClipRegion.Clear();
    for (TInt i=0; i < aUpdateRegion.Count(); i++)
        {
        iDefaultClipRegion.AddRect(AdjustCoordinates(aUpdateRegion[i]));   
        }
    
    TSize size = RoundedDisplayRect().Size();	

    // We create buffer only in drawing phase to reduce memory consumption.
    TBool recreateBackbuffer = 
        ( iAction == EDrawBuffer ) && 
        ( !iBitGc || ( iBitBitmap && iBitBitmap->SizeInPixels() != size ) );
    
    // If this is first draw or size has changed, re-create backbuffer
	if (recreateBackbuffer)
		{				    	
		iBitmapInitialized = EFalse;
		
		if (!iBitBitmap)
            {
            iBitBitmap = new (ELeave) CFbsBitmap;
            iBitBitmap->Create(size, SelectDisplayMode());  
            }
        else
            {
            iBitBitmap->Resize(size);    
            }    
	    
        if (!iBitDevice)
            {
            iBitDevice = CFbsBitmapDevice::NewL(iBitBitmap);        
            }
        else
            {
            iBitDevice->Resize(size);    
            }    

	    delete iBitGc;
	    iBitGc = NULL;
	    User::LeaveIfError(iBitDevice->CreateContext(iBitGc));
        
        TRgb clearColor = KRgbWhite;
	    clearColor.SetAlpha(0x0);
        
	    iBitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        iBitGc->SetPenColor(clearColor);
        iBitGc->SetBrushColor(clearColor);
        iBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
  	    iBitGc->Clear();
  	    iBitGc->Reset();    	  
  	    
  	    iIsReallyDirty = EFalse;
		}
	
    if (iBitGc)
        {
        iBitGc->Reset();
        }
    
    iBitmapPrepared = aIsCachePrepared && iBitmapInitialized;
	}

// Note that for perfomance reasons we keep iBitBitmap alive in this method   
void CHuiCanvasWsBitGc::EndActionL(const TRegion& aUpdateRegion, TBool aUpdateDisplay)
	{
    if (!iBitBitmap || !iVisual)
        {
        return;    
        }        

    // Update internal state - drawing finished
    if ( iAction == EDrawBuffer )
        {
        iBitmapInitialized = ETrue;
        iBitmapPrepared = ETrue;
        }

    // If capturing is used, then copy content from bitmap.
    if (iCapturingBitGc && aUpdateDisplay && iIsReallyDirty && iAction == EDrawBuffer)
        {
        iCapturingBitGc->Reset();
        
        // blend only transparent windows
        if (iVisual->Flags() & EHuiVisualFlagOpaqueHint)
            {
            iCapturingBitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
            }
        else
            {
            iCapturingBitGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
            }
        
        iCapturingBitGc->SetClippingRegion(aUpdateRegion);
        iCapturingBitGc->SetClippingRect(CHuiEnv::Static()->Display(0).CurrentDirtyRect());
        
        TSize visualSize = iVisual->DisplayRect().Size().Round();        
        if (iCapturingBitBitmap->SizeInPixels() != visualSize)
            {       
            iCapturingBitGc->BitBlt(iVisual->DisplayRect().iTl.Round(), iBitBitmap);
            }
        else
            {
            // Old capturing functionality, althoug not really used we support that as well
            iCapturingBitGc->BitBlt(TPoint(0,0), iBitBitmap);
            }
        
        iCapturingBitGc->CancelClippingRect();
        iCapturingBitGc->CancelClippingRegion();
        }

    // Content is not drawn anywhere (except to to capturing bitgc)
	}

	
THuiCanvasWsGcType CHuiCanvasWsBitGc::Type() const
    {
    return EHuiCanvasWsBit;
    }


void CHuiCanvasWsBitGc::ClearCache()
    {
    // ClearCache is called when we can release,
    // e.g. when window is hidden by another opaque window.
    // So now we must release bitmaps in order to free
    // memory.
    iBitmapPrepared = EFalse; 
    iBitmapInitialized = EFalse;

    delete iBitGc;
    iBitGc = NULL;
    
    delete iBitDevice;
    iBitDevice = NULL;
    
    delete iBitBitmap;
    iBitBitmap = NULL;
    }


TBool CHuiCanvasWsBitGc::IsRenderBufferEnabled() const
    {
    return ETrue;
    }

void CHuiCanvasWsBitGc::ClearCapturingBufferArea(const TRect& aRect)
    {
    
    }
