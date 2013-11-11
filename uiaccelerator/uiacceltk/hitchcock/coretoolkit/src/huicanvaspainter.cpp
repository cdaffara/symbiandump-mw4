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
* Description:   Definition of CHuiCanvasAlfPainter.
*
*/


#include "uiacceltk/HuiCanvasVisual.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiCanvasCmdBufferReader.h"
#include "huicanvasgc.h"
#include "HuiRenderSurface.h"
#include "huicanvastexturecache.h"
#include "huicanvasbackground.h"
#include <graphics/wsgraphicscontext.h>
#include <e32cmn.h>
#include <AknLayoutFont.h>
#include "huicanvaspainter.h"



CHuiCanvasPainter::~CHuiCanvasPainter()
  	{
  	iCommandBuffers.Close();
   	delete iPartialCommandBuffer;
   	};

void CHuiCanvasPainter::SetCommandSetL( const TDesC8& aCommands )
    {
    ClearCommandSet();
    AddCommandSetL(aCommands);
	}

void CHuiCanvasPainter::ClearCommandSet()
    {
	TInt count = iCommandBuffers.Count();
    for (TInt i=0; i< count;i++)
        {
        delete iCommandBuffers[i];
        }
    iCommandBuffers.Reset();  
    }

void CHuiCanvasPainter::AddCommandSetL( const TDesC8& aMoreCommands )
    {
	HBufC8* commands = aMoreCommands.AllocL();    
    CleanupStack::PushL(commands);

    CHuiCanvasCommandBuffer* newBuffer = new(ELeave) CHuiCanvasCommandBuffer; 
    CleanupStack::PushL(newBuffer);
    newBuffer->iCommands = commands;
	
	// the orientation will be defined by the commandbuffer
    /*if (iCanvasVisual && iCanvasVisual->Display())
        {
        newBuffer->iOrientation = iCanvasVisual->Display()->Orientation();
        }*/
		
    iCommandBuffers.AppendL( newBuffer);    
    CleanupStack::Pop(2, commands);
    
    RequestContinueRefresh();
    }

void CHuiCanvasPainter::AddPartialCommandSetL( const TDesC8& aMoreCommands, TBool aLastPart )
    {
	if (!iPartialCommandBuffer)
        {
        iPartialCommandBuffer = aMoreCommands.AllocL();
        }
    else 
        {
        iPartialCommandBuffer = 
            iPartialCommandBuffer->ReAllocL( iPartialCommandBuffer->Length() + aMoreCommands.Length() );    
        
        TPtr8 ptr = iPartialCommandBuffer->Des();
        ptr.Append(aMoreCommands);
        }
        
    if (aLastPart && iPartialCommandBuffer)
        {
        AddCommandSetL(*iPartialCommandBuffer);
        delete iPartialCommandBuffer;
        iPartialCommandBuffer = NULL;                    
        }        	
    }
	
// Tags the last buffer for tracking	
#ifdef HUI_DEBUG_TRACK_DRAWING
void CHuiCanvasPainter::SetTrackCommandSet( TFileName& aFileName, TBool aTrack )
    {
    TInt count = iCommandBuffers.Count();
    if ( count == 0 )
        {
        return;
        }
    iCommandBuffers[count-1]->iTrackedBuffer = aTrack;
    
    if ( aFileName.Length() > 0 )
        {
        iCommandBuffers[count-1]->iProcessName = aFileName.Alloc();
        }
    }
#endif

TBool CHuiCanvasPainter::HasCommandBuffers()
    {
	if (iCommandBuffers.Count() == 0 || 
        !iCommandBuffers[0]->iCommands )
    	{
    	return EFalse;		
    	}
    return ETrue;	
    }

void CHuiCanvasPainter::SetVisual(CHuiCanvasVisual* aVisual)
    {
    iCanvasVisual = aVisual;    
    }

CHuiCanvasVisual* CHuiCanvasPainter::Visual()
    {
    return iCanvasVisual;    
    }

TBool CHuiCanvasPainter::BuffersChanged() const
    {    
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        if (iCommandBuffers[cb]->iChanged)
            {
            return ETrue;
            }
        }
    return EFalse;    
    }

void CHuiCanvasPainter::ClearAllBuffersChanged()
    {
    SetAllBuffersChanged(EFalse);
    }

void CHuiCanvasPainter::SetAllBuffersChanged(TBool aChanged)
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        iCommandBuffers[cb]->iChanged = aChanged;
        }
    
    if ( aChanged )
        {
        RequestContinueRefresh();
        }
    }

void CHuiCanvasPainter::RequestContinueRefresh()
    {
    if ( iCanvasVisual && iCanvasVisual->Display() )
        {
        iCanvasVisual->Owner().Env().ContinueRefresh(ETrue);
        }
    }

void CHuiCanvasPainter::SetAllBufferStatusFlags(THuiCanvasBufferStatusFlags aStatusFlags)
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        iCommandBuffers[cb]->SetStatusFlags(aStatusFlags);
        }    
    }

void CHuiCanvasPainter::ClearAllBufferStatusFlags(THuiCanvasBufferStatusFlags aStatusFlags)
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        iCommandBuffers[cb]->ClearStatusFlags(aStatusFlags);
        }        
    }


TBool CHuiCanvasPainter::HasCommandBuffers(THuiCanvasBufferStatusFlags aStatusFlags)
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        if (iCommandBuffers[cb]->iStatusFlags & aStatusFlags)
            {
            return ETrue;
            }
        }    
    return EFalse;
    }

TBool CHuiCanvasPainter::HasCommandBuffersWithoutFlags(THuiCanvasBufferStatusFlags aStatusFlags)
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        if (!(iCommandBuffers[cb]->iStatusFlags & aStatusFlags))
            {
            return ETrue;
            }
        }    
    return EFalse;
    }

TBool CHuiCanvasPainter::HasOldCommandBuffers(TInt aLifeTimeInMs)
    {
    TTime now;
    now.UniversalTime();    
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        TInt lifetimeInMs =  now.MicroSecondsFrom( iCommandBuffers[cb]->iCreationTime ).Int64()/1000;                               
        if (lifetimeInMs > 0 && lifetimeInMs > aLifeTimeInMs)
            {
            return ETrue;
            }
        }    

    return EFalse;    
    }


TRect CHuiCanvasPainter::CommandBufferCoverage(TInt aOrientation)
    {
    TInt bufferCount = iCommandBuffers.Count();
    RRegionBuf<1> tempRegion;
    
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        if (iCommandBuffers[cb]->iOrientation == aOrientation)
            {
            // add update region of command buffer to region
            TRect rect(iCommandBuffers[cb]->iOriginalUpdateRegion.BoundingRect());
            rect.Move(-iCommandBuffers[cb]->iOriginalDisplayRect.Round().iTl);
            
            tempRegion.AddRect(rect);
            tempRegion.Tidy();
            }
        }    
    
    if (iCanvasVisual)
        {
        TRect displayRect(iCanvasVisual->DisplayRect());
        tempRegion.ClipRect(TRect(displayRect.Size()));
        tempRegion.Offset(displayRect.iTl);
        }

    TRect result = tempRegion.BoundingRect();
    tempRegion.Close();
    return result;
    }

void CHuiCanvasPainter::ClearCapturingBufferArea(const TRect& /*aRect*/)
    {
    // Not supported by default
    }  
