/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the CHuiDisplayCoeControl class. 
*                CHuiDisplayCoeControl is a CCoeControl that has its own 
*                CHuiDisplay instance attached to the control's RWindow.
*
*/



#include "uiacceltk/HuiDisplayCoeControl.h"  // Class definition
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiEvent.h"


EXPORT_C CHuiDisplayCoeControl* 
CHuiDisplayCoeControl::NewL(CHuiEnv& aEnv, const TRect& aRect, TInt aFlags)
    {
    CHuiDisplayCoeControl* self = CHuiDisplayCoeControl::NewLC(aEnv, aRect, aFlags);
    CleanupStack::Pop(self);
    return self;
    }


EXPORT_C CHuiDisplayCoeControl* 
CHuiDisplayCoeControl::NewLC(CHuiEnv& aEnv, const TRect& aRect, TInt aFlags)
    {
    CHuiDisplayCoeControl* self = new (ELeave) CHuiDisplayCoeControl(aEnv);
    CleanupStack::PushL(self);
    self->ConstructL(aRect, aFlags);
    return self;
    }


EXPORT_C CHuiDisplayCoeControl::CHuiDisplayCoeControl(CHuiEnv& aEnv)
        : iEnv(aEnv)
    {
    }


EXPORT_C void CHuiDisplayCoeControl::ConstructL(const TRect& aRect, TInt aFlags)
    {
    // The display has its own window.
    CreateWindowL();
#ifndef SYMBIAN_BUILD_GCE
    Window().EnableOSB(EFalse);
#endif
    // Set the size of the control.
    SetRect(aRect);

    if(aFlags & CHuiEnv::ENewDisplayFullScreen)
        {
        SetExtentToWholeScreen();
        }

    // Create a new CHuiDisplay for our window. We don't get ownership of 
    // the display instance.
    iDisplay = &iEnv.NewDisplayL(Rect(), this, aFlags);

    // Since display creation went ok, we know that this rectangle size can be handled.
    iPrevRect = aRect;

    // The control is now ready to be drawn.
    ActivateL();
    
    EnableDragEvents();
    }


EXPORT_C CHuiDisplayCoeControl::~CHuiDisplayCoeControl()
    {	    	
    // if the environment has been destroyed, the display is
    // already been deleted
    if (CHuiEnv::Static())
        {
        // The display is discarded.
        delete iDisplay;
        }
    }


EXPORT_C CHuiEnv& CHuiDisplayCoeControl::Env()
    {
    return iEnv;
    }
    

EXPORT_C CHuiDisplay& CHuiDisplayCoeControl::Display() 
    {
    return *iDisplay;
    }


EXPORT_C void CHuiDisplayCoeControl::SizeChanged()
    {
    if(iDisplay)
        {     
        // Change the size of the display.
        TSize size = Rect().Size();
        TRAPD(err, iDisplay->SetSizeL(size));
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
        iDisplay->SetVisibleArea(TRect(TPoint(0, 0), size));
        }
    }


EXPORT_C TInt CHuiDisplayCoeControl::CountComponentControls() const
    {
    return 0;
    }


EXPORT_C CCoeControl* CHuiDisplayCoeControl::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }


EXPORT_C void CHuiDisplayCoeControl::Draw(const TRect& /*aRect*/) const
    {
    // This is called when the application framework wants us to redraw
    // a certain portion of the screen.

    if(iDisplay)
        {
        /// @todo  Use aRect as the dirty area to redraw.
        
        // Make sure refresh is going to happen.           
        iDisplay->SetDirty();               
        }
    }


EXPORT_C void CHuiDisplayCoeControl::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    }


EXPORT_C void CHuiDisplayCoeControl::HandleControlEventL(CCoeControl* /*aControl*/,
                                                         TCoeEvent /*aEventType*/)
    {
    /** @todo What to do here? */
    }


EXPORT_C TKeyResponse CHuiDisplayCoeControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                                   TEventCode aType)
    {
    THuiEvent event(iDisplay, aKeyEvent, aType);

    // Notify the environemnt.    
    iEnv.NotifyInputReceivedL(event);
    
    if(iDisplay->Roster().HandleEventL(event))
        {
        return EKeyWasConsumed;
        }
    return EKeyWasNotConsumed;
    }


EXPORT_C void CHuiDisplayCoeControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    THuiEvent event(iDisplay, aPointerEvent);

    // Notify the environment.    
    iEnv.NotifyInputReceivedL(event);

    iDisplay->Roster().HandleEventL(event);
    }

EXPORT_C void CHuiDisplayCoeControl::DisplayCoeCntrlExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParams*/)
    {
    }

EXPORT_C void CHuiDisplayCoeControl::MakeVisible(TBool aVisible)
    {
    CCoeControl::MakeVisible(aVisible);
    }
    
EXPORT_C void CHuiDisplayCoeControl::SetDimmed(TBool aDimmed)
    {
    CCoeControl::SetDimmed(aDimmed);
    }

EXPORT_C void CHuiDisplayCoeControl::ConstructFromResourceL(TResourceReader& aReader)
    {
    CCoeControl::ConstructFromResourceL(aReader);
    }
    
EXPORT_C void CHuiDisplayCoeControl::ActivateL()
    {
    Window().EnableAdvancedPointers();
    CCoeControl::ActivateL();
    }
    
EXPORT_C void CHuiDisplayCoeControl::PrepareForFocusLossL()
    {
    CCoeControl::PrepareForFocusLossL();
    }
    
EXPORT_C void CHuiDisplayCoeControl::PrepareForFocusGainL()
    {
    CCoeControl::PrepareForFocusGainL();
    }
    
EXPORT_C TCoeInputCapabilities CHuiDisplayCoeControl::InputCapabilities() const
    {
    return CCoeControl::InputCapabilities();
    }
    
EXPORT_C void CHuiDisplayCoeControl::FocusChanged(TDrawNow aDrawNow)
    {
    CCoeControl::FocusChanged(aDrawNow);
    }
    
EXPORT_C void CHuiDisplayCoeControl::PositionChanged()
    {
    CCoeControl::PositionChanged();
    }

EXPORT_C void CHuiDisplayCoeControl::SetContainerWindowL(const CCoeControl& aContainer)
    {
    CCoeControl::SetContainerWindowL(aContainer);
    }
    
