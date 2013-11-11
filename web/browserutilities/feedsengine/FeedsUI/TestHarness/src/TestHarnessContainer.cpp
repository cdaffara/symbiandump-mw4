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
* Description:  A view to change settings.
*
*/


#include "TestHarnessContainer.h"
#include "TestHarnessView.h"
#include "LeakTracker.h"


// -----------------------------------------------------------------------------
// CFeedContainer::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestHarnessContainer* CTestHarnessContainer::NewL(CTestHarnessView& aView, const TRect& aRect)
    {
    CTestHarnessContainer* self = new (ELeave) CTestHarnessContainer(aView);
    
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CTestHarnessContainer::CTestHarnessContainer
//
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CTestHarnessContainer::CTestHarnessContainer(CTestHarnessView& aView):
        iView(aView)
	{
    LEAK_TRACKER_INCREMENT(CLeakTracker::ETestHarnessContainer);
	}


// -----------------------------------------------------------------------------
// CFeedContainer::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestHarnessContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    SetRect(aRect);
    ActivateL();
    }


// -----------------------------------------------------------------------------
// CTestHarnessContainer::~CTestHarnessContainer
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CTestHarnessContainer::~CTestHarnessContainer()
    {
    LEAK_TRACKER_DECREMENT(CLeakTracker::ETestHarnessContainer);
    }


// -----------------------------------------------------------------------------
// CFeedContainer::OfferKeyEventL
//
// Handles key event.
// -----------------------------------------------------------------------------
//
TKeyResponse CTestHarnessContainer::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,
        TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }


// -----------------------------------------------------------------------------
// CTestHarnessContainer::SizeChanged
//
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CTestHarnessContainer::SizeChanged()
    {
    }

// -----------------------------------------------------------------------------
// CFolderContainer::CountComponentControls
//
// Returns number of components.
// -----------------------------------------------------------------------------
//
TInt CTestHarnessContainer::CountComponentControls() const
    {
    return 0;
    }


// -----------------------------------------------------------------------------
// CFolderContainer::ComponentControl
//
// Returns pointer to particular component.
// -----------------------------------------------------------------------------
//
CCoeControl* CTestHarnessContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }


// ---------------------------------------------------------
// CTestHarnessContainer::Draw
//
// CBrCtlSampleAppContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CTestHarnessContainer::Draw(const TRect& aRect) const
    {    
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

