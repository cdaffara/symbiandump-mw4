/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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



#include "HuiFxVg10FilterBase.h"
#include "uiacceltk/HuiUtil.h"


//_LIT(KLitPanicMessage, "HuiFx");


TBool CHuiFxVg10FilterBase::Draw(CHuiFxEngine& aEngine, CHuiGc& /* aGc */, CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource,
                                            const TRect& aTargetRect, const TRect& aSourceRect, TBool aHasSurface)
    {
    aTarget.BindAsTexture(ERenderbufferUsageWriteOnly);
    aSource.BindAsTexture(ERenderbufferUsageReadOnly);
    VGImage srcImage  = (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aSource))->AcquireSubImage(aSourceRect);
    VGImage destImage = (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aTarget))->AcquireSubImage(aTargetRect);
    
    // take opacity into account
    const VGfloat opacity = clamp(iOpacity, 0.0f, 1.0f);

    if(opacity > EPSILON)
        {
        DrawEffect(aEngine, destImage, srcImage, aSourceRect.Width(), aSourceRect.Height(), aHasSurface);
        }
    else
        {
//        VGint width = vgGetParameteri(srcImage, VG_IMAGE_WIDTH);
//        VGint height = vgGetParameteri(srcImage, VG_IMAGE_HEIGHT);
        VGint width = aSourceRect.Width();
        VGint height = aSourceRect.Height();
        vgCopyImage(destImage, 0, 0, srcImage, 0, 0, width, height, VG_FALSE);
        }

    HUIFX_VG_INVARIANT();    
    (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aSource))->ReleaseSubImage(srcImage);
    (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aTarget))->ReleaseSubImage(destImage);
    aTarget.UnbindAsTexture();
    aSource.UnbindAsTexture();
    
    return wasEnoughMemory();
    }

// any unexpected error results in panic
TBool CHuiFxVg10FilterBase::wasEnoughMemory()
    {
#ifndef _DEBUG
	// vgGetError() -call is REALLY SLOW on HW, remove for now at least,
	// since nobody is really even checking this return value nor acting on it..
    TBool success = ETrue;
#else
    TBool success = EFalse;
    const VGErrorCode vgError = vgGetError();
    
    switch(vgError)
        {
        case VG_NO_ERROR : 
            success = ETrue;
            break;
            
        case VG_OUT_OF_MEMORY_ERROR : 
            success = EFalse;
            break;
            
        default :
            success = EFalse;
            // should print an error instead of panicking
            HUI_DEBUG1(_L("CHuiFxVg10FilterBase::wasEnoughMemory() vgError -- %04x"), vgError);
//            User::Panic(KLitPanicMessage, vgError);
        }
#endif
    return success;
    }
