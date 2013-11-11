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
* Description:  SVG Implementation source file
 *
*/


#include <e32base.h>

#include "SVGPreserveAspectRatioImpl.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPreserveAspectRatioImpl* CSvgPreserveAspectRatioImpl::NewL()
    {
    CSvgPreserveAspectRatioImpl* self = new ( ELeave )
                                        CSvgPreserveAspectRatioImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPreserveAspectRatioImpl* CSvgPreserveAspectRatioImpl::NewLC()
    {
    CSvgPreserveAspectRatioImpl* self = new ( ELeave )
                                        CSvgPreserveAspectRatioImpl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPreserveAspectRatioImpl::~CSvgPreserveAspectRatioImpl()
    {
    }

// *******************************************************
// From SVG DOM

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TSvgPreserveAspectAlignType CSvgPreserveAspectRatioImpl::GetAlign()
    {
    return iAlign;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TSvgMeetOrSliceType CSvgPreserveAspectRatioImpl::GetMeetOrSlice()
    {
    return iMeetOrSlice;
    }

// *******************************************************
// SVG Implementation

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPreserveAspectRatioImpl::SetAlign( TSvgPreserveAspectAlignType aType )
    {
    iAlign = aType;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPreserveAspectRatioImpl::SetMeetOrSlice( TSvgMeetOrSliceType aType )
    {
    iMeetOrSlice = aType;
    }

// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPreserveAspectRatioImpl::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPreserveAspectRatioImpl::CSvgPreserveAspectRatioImpl() : iMeetOrSlice( ESvgMeetOrSlice_Meet ),
                                                             iAlign( ESvgPreserveAspectRatio_XmidYmid )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPreserveAspectRatioImpl* CSvgPreserveAspectRatioImpl::CloneL()
    {
    CSvgPreserveAspectRatioImpl* retValue = CSvgPreserveAspectRatioImpl::NewL();
    retValue->iMeetOrSlice = iMeetOrSlice;
    retValue->iAlign = iAlign;
    return retValue;
    }
