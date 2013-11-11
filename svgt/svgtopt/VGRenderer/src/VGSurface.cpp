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
* Description:  CVGSurface source file
 *
*/

#include "VGSurface.h"

CVGSurface::CVGSurface()
    {
    }

CVGSurface::~CVGSurface()
    {
    delete this->iImpl;
    }

CVGSurface* CVGSurface::NewLC(MVGSurfaceImpl *aVGSurfaceImpl)
    {
    CVGSurface* self = new (ELeave)CVGSurface();
    CleanupStack::PushL(self);
    self->ConstructL(aVGSurfaceImpl);
    return self;
    }

CVGSurface* CVGSurface::NewL(MVGSurfaceImpl *aVGSurfaceImpl)
    {
    CVGSurface* self=CVGSurface::NewLC(aVGSurfaceImpl);
    CleanupStack::Pop(self);
    return self;
    }

void CVGSurface::ConstructL(MVGSurfaceImpl *aVGSurfaceImpl)
    {
    this->iImpl = aVGSurfaceImpl;
    }
