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
* Description:  CSWOpenVGRendererImpl source file
 *
*/

#include "SWOpenVGRendererImpl.h"
#include "SWVGSurfaceImpl.h"
#include "swopenvgtargetsurfaceimpl.h"

#include <VG/vgu.h>

const TUint CSWOpenVGRendererImpl::KMAJOR_VERSION = 1;
const TUint CSWOpenVGRendererImpl::KMINOR_VERSION = 0;
const TUint CSWOpenVGRendererImpl::KBUILD_VERSION = 1;

EXPORT_C MVGRendererImpl* CreateVGRendererImplL()
    {
    return CSWOpenVGRendererImpl::NewL();
    }


CSWOpenVGRendererImpl* CSWOpenVGRendererImpl::NewLC()
    {
    CSWOpenVGRendererImpl* self = new (ELeave) CSWOpenVGRendererImpl;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CSWOpenVGRendererImpl* CSWOpenVGRendererImpl::NewL()
    {
    CSWOpenVGRendererImpl* self = CSWOpenVGRendererImpl::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CSWOpenVGRendererImpl::CSWOpenVGRendererImpl()
    {
    }

void CSWOpenVGRendererImpl::ConstructL()
    {
    }

CSWOpenVGRendererImpl::~CSWOpenVGRendererImpl()
    {
    }

TVersion CSWOpenVGRendererImpl::Version() const
    {
    return TVersion(KMAJOR_VERSION, KMINOR_VERSION, KBUILD_VERSION);
    }

TVersion CSWOpenVGRendererImpl::GetVersion() const
    {
    return TVersion(KMAJOR_VERSION, KMINOR_VERSION, KBUILD_VERSION);
    }

const TDesC CSWOpenVGRendererImpl::GetName() const
    {
    _LIT(KSWVGRenderer, "SWVGRenderer");
    return KSWVGRenderer; 
    }

void CSWOpenVGRendererImpl::ToggleReset()
    {
    }

MVGSurfaceImpl* CSWOpenVGRendererImpl::CreateVGSurfaceL(TInt aOption)
    {
    if(aOption)
        return CSWVGSurfaceImpl::NewL();
    else
        return CSWVGTargetSurfaceImpl::NewL(); //M2G: Enable rendering on target buffer.
    }
