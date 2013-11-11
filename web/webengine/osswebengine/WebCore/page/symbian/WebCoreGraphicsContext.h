/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __WEBCOREGC
#define __WEBCOREGC

#include "bitstd.h"
#include "bitdev.h"

#include "WebCoreWidget.h"

struct TWebCoreSavedContext
    {
    TRect iClippingRect;
    TPoint iOrigin;
    TBool iHasClipping;
    MScrollView* iScrollView;
    };

//Wrapper class for CFbsBitGc
class WebCoreGraphicsContext : public CBase
{

public:
    static WebCoreGraphicsContext* NewL(CFbsBitmapDevice* aBitmapDevice, CFbsBitmap* aBitmap, MScrollView* aView);
    static WebCoreGraphicsContext* NewL(CFbsBitGc& aGc, CFbsBitmap* aBitmap, MScrollView* aView);

    virtual ~WebCoreGraphicsContext();

    CFbsBitmap* contentBuffer() const
        {
        return iOffscreenBitmap;
        }

    CFbsBitGc& gc() const
        {
        return *iBitmapContext;
        }

    MScrollView& view() const
        {
        return *iScrollView;
        }

    void resized()
        {
        iBitmapContext->Resized();
        }

    void setClippingRect(const TRect& aClip)
        {
        iHasClipping = ETrue;
        iClippingRect = aClip;
        iBitmapContext->SetClippingRect(iClippingRect);
        }

    TRect clippingRect() const
        {
        return iClippingRect;
        }

    TBool hasClipping() const
        {
        return iHasClipping;
        }

    void cancelClipping()
        {
        iHasClipping = EFalse;
        iBitmapContext->CancelClipping();
        }

    void setOrigin(TPoint aOrigin)
        {
        iOrigin = aOrigin;
        iBitmapContext->SetOrigin(iOrigin);
        }

    TPoint origin() const
        {
        return iOrigin;
        }

    TWebCoreSavedContext save()
        {
        TWebCoreSavedContext save;
        save.iClippingRect = iClippingRect;
        save.iHasClipping = iHasClipping;
        save.iOrigin = iOrigin;
        save.iScrollView = iScrollView;
        return save;
        }

    void restore(const TWebCoreSavedContext& aSave)
        {
        iBitmapContext->SetOrigin(aSave.iOrigin);
        iOrigin = aSave.iOrigin;

        if (!aSave.iHasClipping && iHasClipping)
            {
            iBitmapContext->CancelClipping();
            }
        else
        {
            iBitmapContext->SetClippingRect(aSave.iClippingRect);
            }

        iClippingRect = aSave.iClippingRect;
        iHasClipping = aSave.iHasClipping;
        }

    void setView(MScrollView& aView)
        {
        iScrollView = &aView;
        }

    protected:

        WebCoreGraphicsContext(CFbsBitmap* aBitmap, MScrollView* aView);

        void ConstructL(CFbsBitmapDevice *aBitmapDevice);
        void ConstructL(CFbsBitGc& aGc);

    private:
        CFbsBitGc* iBitmapContext;
        CFbsBitmap* iOffscreenBitmap;
        MScrollView* iScrollView;
        TBool iGcOwned;
        TRect iClippingRect;
        TPoint iOrigin;
        TBool iHasClipping;
};

#endif
