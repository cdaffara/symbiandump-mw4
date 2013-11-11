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
* Description:  NVG Decoder source file
*
*/

 

#include "NVGTLVIcon.h"
#include "NVGIconData.h"
#include "NVGUtil.h"
#include "TLVRenderer.h" 
#include "TLVIconCreator.h"
#include "TLVIconRenderer.h"
#include "OpenVGHandleStore.h"

CNVGTLVIcon::CNVGTLVIcon()
    : iNVGIconData(0),
      iVGImageBinder(0)
    {
    }

CNVGTLVIcon::~CNVGTLVIcon()
    {
    delete iNVGIconData;
    delete iOpenVGHandles;
    }

CNVGTLVIcon * CNVGTLVIcon::NewL()
    {
    CNVGTLVIcon* self    = CNVGTLVIcon::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
CNVGTLVIcon * CNVGTLVIcon::NewLC()
    {
    CNVGTLVIcon* self    = new (ELeave) CNVGTLVIcon;
    CleanupStack::PushL(self);

    self->ConstructL();
    return self;
    }

void CNVGTLVIcon::ConstructL()
    {
    iNVGIconData = CNVGIconData::NewL();
    iOpenVGHandles = COpenVGHandleStore::NewL();
    }

TInt CNVGTLVIcon::SetPreserveAspectRatio(TInt aPreserveAspectSetting, 
        TInt aSmilFitSetting)
    {
    (void)aPreserveAspectSetting;
    (void)aSmilFitSetting;
    
    return KErrNotSupported;
    }

TInt CNVGTLVIcon::Rotate(TReal32 aAngle, TReal32 aX, TReal32 aY) __SOFTFP
    {
    (void)aAngle;
    (void)aX;
    (void)aY;
    
    return KErrNotSupported;
    }

TInt CNVGTLVIcon::DirectDrawL(const TDesC8& aBuf, const TSize& aTargetSize)
    {
    TInt error      = KErrNone;
    
    CTLVRenderer * iconRenderer = CTLVRenderer::NewLC(aBuf, aTargetSize.iWidth, aTargetSize.iHeight);
    
    iconRenderer->SetVGImageBinder(iVGImageBinder);
    
    iconRenderer->ExecuteL();
    
    CleanupStack::PopAndDestroy(iconRenderer);
    
    return error;
    }

TInt CNVGTLVIcon::CreateL(const TDesC8& aBuf, const TSize& aTargetSize)
    {    
    TInt error = KErrNone;

    CTLVIconCreator * iconCreater = CTLVIconCreator::NewLC(aBuf, aTargetSize.iWidth, aTargetSize.iHeight, this);
    
    iconCreater->ExecuteL();
    
    CleanupStack::PopAndDestroy(iconCreater);
    return error;
    }

TInt CNVGTLVIcon::Draw(const TSize aSize, CNvgEngine * aNVGEngine)
    {
    TInt error = KErrNone;
    iNVGEngine = aNVGEngine;
    UpdateClientMatrices();
    
    TRAP(error, DoDrawL(aSize));
    RestoreClientMatrices();
    return error;
    }
TInt CNVGTLVIcon::DoDrawL(const TSize aSize)
    {    
    TPtr8 bufPtr = ((HBufC8 *)(iNVGIconData->GetNVGData()))->Des();
    
    CTLVRenderer * iconRenderer = CTLVIconRenderer::NewLC(bufPtr, aSize.iWidth, aSize.iHeight);
    
    iconRenderer->SetVGImageBinder(iVGImageBinder);
    
#ifndef __MIRROR_
    vgScale(1.0f, -1.0f);
    vgTranslate(0, (VGfloat)(-aSize.iHeight));
#endif
    iconRenderer->ExecuteL();
    
    CleanupStack::PopAndDestroy(iconRenderer);
    return KErrNone;
    }

void CNVGTLVIcon::AddDrawPathCommandL(VGPath aPath, VGPaintMode aPaintMode)
    {
    iOpenVGHandles->AddPathDHL(aPath);
    iNVGIconData->EncodeInt8L(EPath);
    iNVGIconData->EncodeInt32L(aPath);
    iNVGIconData->EncodeInt32L(aPaintMode);
    }

void CNVGTLVIcon::AddCommandL(const TUint8 * aCommandBuffer, TInt aCommandBufferLength)
    {
    iNVGIconData->EncodeDataL(aCommandBuffer, aCommandBufferLength);
    }

void CNVGTLVIcon::AddCommandL(TInt8 aCommandType, const TUint8 * aCommandBuffer, TInt aCommandBufferLength)
    {
    iNVGIconData->EncodeInt8L(aCommandType);
    iNVGIconData->EncodeDataL(aCommandBuffer, aCommandBufferLength);
    }
void CNVGTLVIcon::UpdateClientMatrices()
    {    
    iMatrixMode = vgGeti(VG_MATRIX_MODE);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgGetMatrix(iPathMatrix);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgGetMatrix(iImageMatrix);
    vgSeti(VG_MATRIX_MODE, iMatrixMode);
    }

void CNVGTLVIcon::RestoreClientMatrices()
    {
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgLoadMatrix(iPathMatrix);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgLoadMatrix(iImageMatrix);    
    vgSeti(VG_MATRIX_MODE, iMatrixMode);
    }   
