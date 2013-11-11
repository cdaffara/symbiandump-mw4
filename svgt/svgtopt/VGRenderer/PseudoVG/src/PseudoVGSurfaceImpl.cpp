/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CPseudoVGSurfaceImpl source file
 *
*/

#include "PseudoVGSurfaceImpl.h"
#include "PseodoVGRendererImpl.h"
#include "fbs.h"

CPseudoVGSurfaceImpl::CPseudoVGSurfaceImpl()
    {
    }

CPseudoVGSurfaceImpl::~CPseudoVGSurfaceImpl()
    {
    }

CPseudoVGSurfaceImpl* CPseudoVGSurfaceImpl::NewLC(CPseodoVGRendererImpl * aVGRendererImpl)
    {
    CPseudoVGSurfaceImpl* self = new (ELeave)CPseudoVGSurfaceImpl();
    CleanupStack::PushL(self);
    self->ConstructL(aVGRendererImpl);
    return self;
    }

CPseudoVGSurfaceImpl* CPseudoVGSurfaceImpl::NewL(CPseodoVGRendererImpl * aVGRendererImpl)
    {
    CPseudoVGSurfaceImpl* self = CPseudoVGSurfaceImpl::NewLC(aVGRendererImpl);
    CleanupStack::Pop();
    return self;
    }

inline void CPseudoVGSurfaceImpl::ConstructL(CPseodoVGRendererImpl * aVGRendererImpl)
    {
    iPseodoVGRendererImpl = aVGRendererImpl;
    }

TInt CPseudoVGSurfaceImpl::InitializeSurface(TSize aSize, TInt /*aColorSpace*/)
    {
    iSize = aSize;
    iPseodoVGRendererImpl->EmptyEncodedData();
    return KErrNone;
    }

void CPseudoVGSurfaceImpl::SetConfiguration(TInt aOption, const TAny* aValue)
    {
    if (aOption == BTIMAP_HEADER)
        {
        iPseodoVGRendererImpl->SetCommonHeader(*(const TDesC8*)aValue);
        }
    }

TInt CPseudoVGSurfaceImpl::CreateSurface(TInt /*aDisplayMode*/, RWindow */*aSurface*/, CFbsBitmap */*aBitmap*/)
    {
    return KErrNone;
    }

TInt CPseudoVGSurfaceImpl::ResizeSurface(TSize aSize)
    {
    iSize = aSize;
    return KErrNone;
    }

TInt CPseudoVGSurfaceImpl::CopyBitmap(TInt aDisplayMode, TInt aMaskMode,CFbsBitmap *aBitmap, CFbsBitmap *aMask, TSize /*aSize*/)
    {
    const TPtr8 vgStream = iPseodoVGRendererImpl->GetEncodedData();
    aBitmap->CreateExtendedBitmap(iSize, (TDisplayMode)aDisplayMode, KUidNvgProprietaryFormat, (TAny*)&(vgStream[0]), vgStream.Length());
    if (aMask)
        {
        iPseodoVGRendererImpl->SetMaskFlag();
        const TPtr8 vgStream = iPseodoVGRendererImpl->GetEncodedData();
        aMask->CreateExtendedBitmap(iSize, (TDisplayMode)aMaskMode, KUidNvgProprietaryFormat, (TAny*)&(vgStream[0]), vgStream.Length() );
        }
        return KErrNone;
    }

TInt CPseudoVGSurfaceImpl::PrepareToBindClientBuffer()
    {
    return iPseodoVGRendererImpl->AddCommand((TInt)EvgPrepareToBindImage, 0, 0);;
    }

TInt CPseudoVGSurfaceImpl::BindClientBuffer(TInt buffer)
    {
    return iPseodoVGRendererImpl->AddCommand((TInt)EvgBindImage, (TUint8 *)&buffer, sizeof(buffer)); 
    }

TInt CPseudoVGSurfaceImpl::UnBindClientBuffer()
    {
    return iPseodoVGRendererImpl->AddCommand((TInt)EvgUnBindImage, 0, 0); 
    }

void CPseudoVGSurfaceImpl::TerminateSurface()
    {
    }

