/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "config.h"
#include "WebCoreGraphicsContext.h"

WebCoreGraphicsContext* WebCoreGraphicsContext::NewL(CFbsBitmapDevice *aBitmapDevice, CFbsBitmap* aBitmap, MScrollView* aView)
    {
    WebCoreGraphicsContext *self = new (ELeave) WebCoreGraphicsContext(aBitmap, aView);
    CleanupStack::PushL(self);
    self->ConstructL(aBitmapDevice);
    CleanupStack::Pop();
    return self;
    }

WebCoreGraphicsContext* WebCoreGraphicsContext::NewL(CFbsBitGc& aGc, CFbsBitmap* aBitmap, MScrollView* aView)
    {
    WebCoreGraphicsContext *self = new (ELeave) WebCoreGraphicsContext(aBitmap, aView);
    CleanupStack::PushL(self);
    self->ConstructL(aGc);
    CleanupStack::Pop();
    return self;
    }


void WebCoreGraphicsContext::ConstructL(CFbsBitmapDevice *aBitmapDevice)
    {
    User::LeaveIfError(aBitmapDevice->CreateContext(iBitmapContext));
    iGcOwned = ETrue;
    }

void WebCoreGraphicsContext::ConstructL(CFbsBitGc& aGc)
    {
    iBitmapContext = &aGc;
    iGcOwned = EFalse;
    }

WebCoreGraphicsContext::WebCoreGraphicsContext(CFbsBitmap* aBitmap, MScrollView* aView)
    {
    iScrollView = aView;
    iOffscreenBitmap = aBitmap;
    }

WebCoreGraphicsContext::~WebCoreGraphicsContext()
    {
    if (iGcOwned)
        delete iBitmapContext;
    }


