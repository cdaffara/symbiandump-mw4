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
* Description:  CHWOpenVGRendererImpl source file
 *
*/

#include "HWOpenVGRendererImpl.h"
#include "HWVGSurfaceImpl.h"

#include <vg/vgu.h>

const TUint CHWOpenVGRendererImpl::KMAJOR_VERSION           = 1;
const TUint CHWOpenVGRendererImpl::KMINOR_VERSION           = 0;
const TUint CHWOpenVGRendererImpl::KBUILD_VERSION           = 1;

EXPORT_C MVGRendererImpl* CreateVGRendererImplL()
    {
    return CHWOpenVGRendererImpl::NewL();
    }


CHWOpenVGRendererImpl::CHWOpenVGRendererImpl()
    {
    }

CHWOpenVGRendererImpl::~CHWOpenVGRendererImpl()
    {
    }

CHWOpenVGRendererImpl* CHWOpenVGRendererImpl::NewLC()
    {
    CHWOpenVGRendererImpl* self = new (ELeave) CHWOpenVGRendererImpl;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CHWOpenVGRendererImpl* CHWOpenVGRendererImpl::NewL()
    {
    CHWOpenVGRendererImpl* self = CHWOpenVGRendererImpl::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

inline void CHWOpenVGRendererImpl::ConstructL()
    {
    }


TVersion CHWOpenVGRendererImpl::Version() const
    {
    return TVersion(KMAJOR_VERSION, KMINOR_VERSION, KBUILD_VERSION);
    }

TVersion CHWOpenVGRendererImpl::GetVersion() const
    {
    return TVersion(KMAJOR_VERSION, KMINOR_VERSION, KBUILD_VERSION);
    }

const TDesC CHWOpenVGRendererImpl::GetName() const
    {
    _LIT(KHWVGRenderer, "HWVGRenderer");
    return KHWVGRenderer; 
    }

void CHWOpenVGRendererImpl::ToggleReset()
    {
    }


MVGSurfaceImpl* CHWOpenVGRendererImpl::CreateVGSurfaceL(TInt aOption)
    {
    return CHWVGSurfaceImpl::NewL(aOption);
    }
