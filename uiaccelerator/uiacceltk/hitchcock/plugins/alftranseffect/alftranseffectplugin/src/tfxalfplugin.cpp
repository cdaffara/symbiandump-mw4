/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   implementation of the control drawer handling data for control
*                transitions.
*
*/



#include "tfxalfplugin.h"
#include "../../servercore/inc/alfwindow.h"
#include "../../servercore/inc/alfwindowbuffer.h"
#include "../../servercore/inc/alfwindowmanager.h"

CAlfWindowFxPlugin* CTfxAlfPlugin::CreateL(TInt aAlfBufferFormat)
	{
	return new (ELeave) CTfxAlfPlugin();
	}
	
CTfxAlfPlugin::CTfxAlfPlugin() : iWinId(0,0)
	{
	
	}
	
CTfxAlfPlugin::~CTfxAlfPlugin()
	{
	
	}
	
void CTfxAlfPlugin::PrepareFrameL(TUint aEstimatedFrameInterval)
	{
	TInt a = 1;
	
	/*
	TUint8* ptr = (TUint8*)iAlfBuffer->BufferPtr();
	TInt stride = iAlfBuffer->Stride();
	TSize size  = iAlfBuffer->Size();
	
	for(TInt y = size.iHeight; y > 0; y--) 
		{
		TUint32* scanline = (TUint32*)ptr;
		for(TInt x = size.iWidth; x > 0; x--)
			{
			*(scanline++) = x&(y+iFrames)&0xffff00ff;
			}
		ptr += stride;
		}
	TRegionFix<1> reg(TRect(TPoint(0,0),size));
	iAlfWindow->ContentReady(iAlfBuffer,reg);
	
	if(0 == --iFrames) 
		{
		//WindowManager()->Release(iWinId);
		WindowManager()->DestroyWindow(*iAlfWindow);
		}
	*/	
	}
	
void CTfxAlfPlugin::WindowUpdated(TInt aWindowId, TInt aEventType)
	{
	if(aWindowId == iWinId.iWindowIdentifer) 
		{
		// ...
		
		}
	}
	
/** Synch message from client */    
void CTfxAlfPlugin::HandleMessageL(const TDesC8& aMessage, TPtr8& aResponse)
	{
	}

/** Asynch message from client */    
void CTfxAlfPlugin::HandleMessageL(const TDesC8& aMessage, const RMessage2& aClientMessage)
	{
	
	}

/** Cancel asynch message(s) from client */    
void CTfxAlfPlugin::CancelMessage(TInt aMessageId)
	{
	
	}
	
