/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "minimap.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPageScaler:CPageScaler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPageScaler::CPageScaler()
    {
    }

// -----------------------------------------------------------------------------
// CPageScaler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPageScaler::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CPageScaler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C
CPageScaler* CPageScaler::NewL(MPageScalerCallback& aCallback, TDisplayMode aMode, TBool aLowQuality )
    {
    // construct a CMinimap (super class) instead
    CPageScaler* mmap = CMinimap::NewL(aCallback, aMode, aLowQuality );
    return mmap;
    }


// Destructor
CPageScaler::~CPageScaler()
    {
    }

// Stubs
void CPageScaler::SetRelativePosition(TInt /*aXDist*/, TInt /*aYDist*/, TPosition /*aPos*/)
    {
    }

void CPageScaler::SetVisibleUntil(TTimeIntervalMicroSeconds32 /*aUntil*/)
    {
    }

TRect CPageScaler::Rect() const
    {
    return TRect();
    }

TRect CPageScaler::IndicatorRect() const
    {
    return TRect(0,0,0,0);
    }

void CPageScaler::DocumentStarted()
    {
    }

void CPageScaler::DocumentChangedL()
    {
    }

void CPageScaler::DocumentViewportMoved()
    {
    }

void CPageScaler::DocumentCompleted()
    {
    }

void CPageScaler::Draw( CWindowGc& /*aGc*/, const TRect& /*aRect*/ ) const
    {
    }

void CPageScaler::SetTransparency(TInt /*aPercent*/)
    {
    }

void CPageScaler::SetVisible(TBool /*aVisible*/)
    {
    }

TBool CPageScaler::Visible() const
    {
    return EFalse;
    }

void CPageScaler::SetZoomOutLevel(TInt /*aPercent*/)
    {
    }

TInt CPageScaler::ZoomOutLevel() const
    {
    return 0;
    }

void CPageScaler::SetMaxSize(TSize /*aSize*/)
    {
    }

TSize CPageScaler::MaxSize() const
    {
    return TSize();
    }

TInt CPageScaler::Transparency() const
    {
    return 0;
    }

CFbsBitmap* CPageScaler::ScaledPage() const
    {
    return NULL;
    }

TRect CPageScaler::ContainerRect() const
   {
   return TRect();
   }


void CPageScaler::SetContainerRect(const TRect& )
   {
   }


TBool CPageScaler::FullScreenMode() const
   {
   return EFalse;
   }


void CPageScaler::SetFullScreenMode(TBool)
   {
   }

EXPORT_C TBool CPageScaler::HasOverlaySupport()
    {
    return ETrue;
    }


//  End of File
