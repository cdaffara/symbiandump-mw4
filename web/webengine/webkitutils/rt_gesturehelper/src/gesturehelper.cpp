/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture helper interface
*
*/


// class include
#include "rt_gesturehelper.h"

// local includes
#include "gesturehelperimpl.h"

using namespace RT_GestureHelper;

// ----------------------------------------------------------------------------
// Two-phase constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CGestureHelper* CGestureHelper::NewL( MGestureObserver& aObserver )
    {
    CGestureHelper* self = new ( ELeave ) CGestureHelper;
    CleanupStack::PushL( self );
    self->iImpl = CGestureHelperImpl::NewL( aObserver );    
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
EXPORT_C CGestureHelper::~CGestureHelper()
    {
    delete iImpl;
    }

// ----------------------------------------------------------------------------
// SetHoldingEnabled
// ----------------------------------------------------------------------------
//
EXPORT_C void CGestureHelper::SetHoldingEnabled( TBool aEnabled )
    {
    iImpl->SetHoldingEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsHoldingEnabled
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CGestureHelper::IsHoldingEnabled() const
    {
    return iImpl->IsHoldingEnabled();
    }

// ----------------------------------------------------------------------------
// SetDoubleTapEnabled
// ----------------------------------------------------------------------------
//
EXPORT_C void CGestureHelper::SetDoubleTapEnabled( TBool aEnabled )
    {
    iImpl->SetDoubleTapEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsDoubleTapEnabled
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CGestureHelper::IsDoubleTapEnabled() const
    {
    return iImpl->IsDoubleTapEnabled();
    }
    



// ----------------------------------------------------------------------------
// HandlePointerEventL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CGestureHelper::HandlePointerEventL( const TPointerEvent& aEvent )
    {
    return iImpl->HandlePointerEventL( aEvent );
    }





// ----------------------------------------------------------------------------
// Cancel
// ----------------------------------------------------------------------------
//
EXPORT_C void CGestureHelper::Cancel()
    {
    iImpl->Reset();
    }

