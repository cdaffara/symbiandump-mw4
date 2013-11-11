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
* Description:   Symbian specific screen buffer drawer
*
*/



#include "alf/alfsymbiansbdrawer.h"
#include "alf/alfscreenbuffer.h"
#include <uiacceltk/HuiUtil.h>
#include <fbs.h>

struct CAlfSymbianBufferDrawer::TPrivData
    {
    CFbsBitmap* iBitmap;
    CFbsBitmap* iMask;
    };
    
CAlfSymbianBufferDrawer::CAlfSymbianBufferDrawer()
    {    
    }

CAlfSymbianBufferDrawer::~CAlfSymbianBufferDrawer()
    {
    if (iPrivData)
    	{
    	delete iPrivData->iBitmap;
    	delete iPrivData->iMask;
    	delete iPrivData; 	
    	}
    }

CAlfSymbianBufferDrawer* CAlfSymbianBufferDrawer::NewL()
    {
    CAlfSymbianBufferDrawer* self = new (ELeave) CAlfSymbianBufferDrawer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
void CAlfSymbianBufferDrawer::ConstructL()
    {
    iPrivData = new (ELeave) TPrivData;
    Mem::FillZ(iPrivData, sizeof(TPrivData));
    }


EXPORT_C void CAlfSymbianBufferDrawer::ReleaseDrawer()
    {
    delete this;
    }

EXPORT_C void CAlfSymbianBufferDrawer::DrawBuffer(CBitmapContext& aContext, const TPoint& aPos, const TRect& aRect)
    {
    if (iPrivData && iPrivData->iBitmap)
        {
        aContext.BitBlt(aPos,iPrivData->iBitmap, aRect);
        }
    }

EXPORT_C void CAlfSymbianBufferDrawer::DrawBufferMasked(CBitmapContext& aContext, const CFbsBitmap* aMask,const TPoint& aPos, const TRect& aRect)
    {
    if (iPrivData && iPrivData->iBitmap && aMask)
        {
        aContext.BitBltMasked(aPos,iPrivData->iBitmap, aRect,aMask, EFalse);
        }
    else
        {
        DrawBuffer(aContext, aPos, aRect);
        }    
    }
    
EXPORT_C void CAlfSymbianBufferDrawer::GetBufferBitmaps(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask) const
    {
    if (iPrivData)
    	{
    	if (iPrivData->iBitmap)
    		{
    		aBitmap = iPrivData->iBitmap;
    		}
    	if (iPrivData->iMask)
    		{
    		aMask = iPrivData->iMask;
    		}
    	}
    }        
    
void CAlfSymbianBufferDrawer::SetBufferL(TInt aBitmapHandle, TInt aMaskHandle)
    {
    __ASSERT_DEBUG(iPrivData, USER_INVARIANT());
    if (iPrivData && iPrivData->iBitmap)
        {
        delete iPrivData->iBitmap;
        iPrivData->iBitmap = NULL;
        }
    if (iPrivData && iPrivData->iMask)
        {
        delete iPrivData->iMask;
        iPrivData->iMask = NULL;
        }
 
 
    if (aBitmapHandle)
        {
        iPrivData->iBitmap = new (ELeave) CFbsBitmap;
        User::LeaveIfError(iPrivData->iBitmap->Duplicate(aBitmapHandle));
        }
    if (aMaskHandle)
        {
        iPrivData->iMask = new (ELeave) CFbsBitmap;
        User::LeaveIfError(iPrivData->iMask->Duplicate(aMaskHandle));
        }
    }

