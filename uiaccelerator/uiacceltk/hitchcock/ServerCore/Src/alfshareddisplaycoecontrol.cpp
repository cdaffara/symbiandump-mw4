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
* Description:   AppUi class
*
*/



#include "alfshareddisplaycoecontrol.h"
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiEvent.h>

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
// 
CAlfSharedDisplayCoeControl::CAlfSharedDisplayCoeControl(){}

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
// 
CAlfSharedDisplayCoeControl::~CAlfSharedDisplayCoeControl()  { iDisplays.Close(); }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
// 
void CAlfSharedDisplayCoeControl::ConstructL()
    {
    CreateWindowL();
    SetExtentToWholeScreen();

    // Since display creation went ok, we know that this rectangle size can be handled.
    iPrevRect = Rect();

    EnableDragEvents();
    Window().EnableAdvancedPointers();
    ActivateL();

    // Set pointer grab, so we will get pointer up events also from other´
    // window groups
    DrawableWindow()->SetPointerGrab(ETrue);
    }

// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------
// 
void CAlfSharedDisplayCoeControl::Draw(const TRect& /*aRect*/) const
{
for ( TInt i = 0 ; i <  iDisplays.Count() ; i++ )
    {
    iDisplays[i]->SetDirty();
    }
}

// ---------------------------------------------------------------------------
// SizeChanged
// ---------------------------------------------------------------------------
// 
void CAlfSharedDisplayCoeControl::SizeChanged()
    {
    TSize size = Rect().Size();

    for ( TInt i = 0 ; i <  iDisplays.Count() ; i++ )
        {
        // Change the size of the display.
        TRAPD(err, iDisplays[i]->SetSizeL(size));
        if(err != KErrNone)
            {
            // Switching to new size failed. Maintain old size.
            size = iPrevRect.Size();
            }
        else
            {
            iPrevRect = Rect();
            }

        // Redefine the portion of the drawing surface that is used by 
        // the display.
        iDisplays[i]->SetVisibleArea(TRect(TPoint(0, 0), size));
        }
    }

// ---------------------------------------------------------------------------
// HandlePointerEventL
// ---------------------------------------------------------------------------
// 
void CAlfSharedDisplayCoeControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    // passing the event to hui
    THuiEvent event(iDisplays[0], aPointerEvent);
    // Send events received here only to the primary display    
    if (iDisplays[0])
        {
        iDisplays[0]->Roster().HandleEventL(event);        
        }    
    }

// end of file
