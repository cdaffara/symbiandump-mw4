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



#include "HuiFxFilterLayer.h"
#include "HuiFxFilter.h"

CHuiFxFilterLayer* CHuiFxFilterLayer::NewL(CHuiFxFilter* aFilter)
    {
    CHuiFxFilterLayer* e = new (ELeave) CHuiFxFilterLayer();
    CleanupStack::PushL(e);
    e->ConstructL(aFilter);
    CleanupStack::Pop(e);
    return e;
    }

CHuiFxFilterLayer::CHuiFxFilterLayer()
    {
#ifdef HUIFX_TRACE        
    RDebug::Print(_L("CHuiFxFilterLayer::CHuiFxFilterLayer - 0x%x "), this);
#endif
    }

CHuiFxFilterLayer::~CHuiFxFilterLayer()
    {
    delete iFilter;
    iFilter = NULL;
#ifdef HUIFX_TRACE 
    RDebug::Print(_L("CHuiFxFilterLayer::~CHuiFxFilterLayer - 0x%x "), this);
#endif
    }
EXPORT_C CHuiFxFilterLayer *CHuiFxFilterLayer::CloneL() const
{
    CHuiFxFilter *filter = iFilter->CloneL();
    CHuiFxFilterLayer *layer = CHuiFxFilterLayer::NewL(filter);
    layer->CHuiFxLayer::CopyFromL(this);
    return layer;
}
EXPORT_C void CHuiFxFilterLayer::SetExtRect( TRect * /*aExtRect*/ )
    {
    // iFilter does not need these
    }
EXPORT_C void CHuiFxFilterLayer::SetVisual( CHuiVisual * /*aVisual*/ )
    {
    // iFilter does not need these
    }
EXPORT_C void CHuiFxFilterLayer::SetVisual( MHuiEffectable * /*aVisual*/ )
    {
    // iFilter does not need these
    }


EXPORT_C void CHuiFxFilterLayer::ConstructL(CHuiFxFilter* aFilter)
    {
    CHuiFxLayer::ConstructL( ELayerTypeFilter );
    iFilter = aFilter;
    }

EXPORT_C TBool CHuiFxFilterLayer::PrepareDrawL(CHuiFxEngine& aEngine)
    {
    return iFilter->PrepareDrawL(aEngine);
    }


EXPORT_C TBool CHuiFxFilterLayer::Changed() const
    {
#ifdef HUIFX_TRACE 
    RDebug::Print(_L("CHuiFxFilterLayer::Changed - 0x%x %d"), this,iFilter->Changed() );
#endif
    return iFilter->Changed();
    }

EXPORT_C void CHuiFxFilterLayer::AdvanceTime(TReal32 aElapsedTime)
    {
    iFilter->AdvanceTime(aElapsedTime);
    }

EXPORT_C void CHuiFxFilterLayer::Draw(CHuiFxEngine& aEngine, CHuiGc& aGc, CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource, TBool aHasSurface)
    {
#ifdef HUIFX_TRACE 
	RDebug::Print(_L("CHuiFxFilterLayer::Draw - 0x%x "), this);
#endif
// TODO: fast path
    CHuiFxRenderbuffer* backBuffer = aEngine.AcquireRenderbuffer(TargetRect().Size());

    if (!backBuffer)
        {
        return;
        }

    // Render the filter
    TRect targetRect(TPoint(0, 0), TargetRect().Size());
    TInt alpha = 0xff; // TODO

    iFilter->Draw(aEngine, aGc, *backBuffer, aSource, targetRect, SourceRect(), aHasSurface);

    // Composite the result
    TRect compositionSourceRect(targetRect);
    TRect compositionTargetRect(TargetRect());
    
    compositionTargetRect.Move(-aTarget.Position());
    
    aEngine.Composite(aTarget, *backBuffer, compositionTargetRect, compositionSourceRect, BlendingMode(), alpha);
    aEngine.ReleaseRenderbuffer(backBuffer);
    }

EXPORT_C TBool CHuiFxFilterLayer::Margin(TMargins &m)
   {
   TMargins s = iFilter->Margin2();
   m.iTop = s.iTop;
   m.iBottom = s.iBottom;
   m.iLeft = s.iLeft;
   m.iRight = s.iRight;
   return ETrue;
   }

EXPORT_C CHuiFxFilter& CHuiFxFilterLayer::Filter() const
    {
    return *iFilter;
    }

TBool CHuiFxFilterLayer::IsFiltered() const
    {
    return ETrue;
    }
