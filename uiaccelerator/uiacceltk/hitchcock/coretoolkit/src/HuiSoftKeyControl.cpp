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
* Description:   Implementation of a soft key control class. CHuiSoftKeyControl 
*                is a control that displays the soft key labels and handles 
*                soft key input events.
*
*/



#include "uiacceltk/HuiSoftKeyControl.h"  // Class definition
#include "uiacceltk/HuiTextVisual.h"
#include "uiacceltk/HuiEvent.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiCommand.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiAnchorLayout.h"
#include "uiacceltk/HuiBrushes.h"

const TReal32 KLabelHeightFactor = 0.1f; // Soft key label height relative to display height.
const TReal32 KLabelWidthFactor = 0.25f; // Soft key label width relative to display width.
const TUint KDefaultExitTransitionTime = 500; // Default exit animation transition time.
const TUint KDefaultEnterTransitionTime = 500; // Default exit animation transition time.

EXPORT_C CHuiSoftKeyControl* CHuiSoftKeyControl::NewL(CHuiEnv& aEnv)
    {
    CHuiSoftKeyControl* self = NewLC(aEnv);
    CleanupStack::Pop(self);
    return self;
    }


EXPORT_C CHuiSoftKeyControl* CHuiSoftKeyControl::NewLC(CHuiEnv& aEnv)
    {
    CHuiSoftKeyControl* self = new (ELeave) CHuiSoftKeyControl(aEnv);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiSoftKeyControl::CHuiSoftKeyControl(CHuiEnv& aEnv)
        : CHuiControl(aEnv), iCurrentLocation(EHuiSkinLocationNone),
        iExitTransitionTime(KDefaultExitTransitionTime),
        iEnterTransitionTime(KDefaultEnterTransitionTime)
    {
    }


void CHuiSoftKeyControl::ConstructL()
    {
    // Append anchor layout to be used to align the label visuals relatively to display
    iLayout = (CHuiAnchorLayout*)(AppendLayoutL(EHuiLayoutTypeAnchor));

    // Create left soft key label visual and align it using anchor layout
    iLeftLabel = CHuiTextVisual::AddNewL(*this, iLayout);
    iLeftLabel->SetAlign(EHuiAlignHLeft, EHuiAlignVBottom);
    iLeftLabel->SetStyle(EHuiTextStyleSoftkey, EHuiBackgroundTypeDark);
    iLeftLabel->SetPadding(TPoint(2, 3));

    // Create right soft key label visual and align it using anchor layout
    iRightLabel = CHuiTextVisual::AddNewL(*this, iLayout);
    iRightLabel->SetAlign(EHuiAlignHRight, EHuiAlignVBottom);
    iRightLabel->SetStyle(EHuiTextStyleSoftkey, EHuiBackgroundTypeDark);
    iRightLabel->SetPadding(TPoint(2, 3));    

    //iRightLabel->SetFlag(EHuiVisualFlagLayoutUpdateNotification);
    iLayout->SetFlag(EHuiVisualFlagLayoutUpdateNotification);
    }


EXPORT_C void CHuiSoftKeyControl::SetBackgroundType(THuiBackgroundType aBackgroundType)
    {
    iLeftLabel->SetStyle(EHuiTextStyleSoftkey, aBackgroundType);
    iRightLabel->SetStyle(EHuiTextStyleSoftkey, aBackgroundType);
    }


CHuiSoftKeyControl::~CHuiSoftKeyControl()
    {
    delete iLeftText;
    delete iRightText;
    
    iRightLabel = NULL;
    iRightLabelImageBrush = NULL;
    iLayout = NULL;
    }



EXPORT_C void CHuiSoftKeyControl::SetLabelL(THuiSoftKeyPosition aKeyPos, const TDesC& aText, TBool aAnimateControl)
    {
    // Animate softkey label change, if a valid softkey location is set.
    if(iCurrentLocation != EHuiSkinLocationNone)
        {
        if(aKeyPos == EHuiSoftKeyLower || aKeyPos == EHuiSoftKeyLeft)
            {
            // Exit view, animate only the changed softkey.
            ExitView(ESoftkeyLabelLeft | (aAnimateControl ? ESoftkeyLabelRight : 0));
            // Cache label text
            delete iLeftText;
            iLeftText = NULL;
            iLeftText = aText.AllocL();
            }
        else
            {
            // Exit view, animate only the changed softkey.
            ExitView(ESoftkeyLabelRight | (aAnimateControl ? ESoftkeyLabelLeft : 0));
            // Cache label text
            delete iRightText;
            iRightText = NULL;
            iRightText = aText.AllocL();
            }

        // Update label(s) after softkey has exited the view.
        if(aAnimateControl)
            {
            Env().Send(THuiCustomEventCommand(ECustomEventUpdateLabels, this), iExitTransitionTime);
            }
        else
            {
            Env().Send(THuiCustomEventCommand(ECustomEventUpdateLabel, this), iExitTransitionTime);
            }
        }
    else
        {
        /** @todo  Display orientation affects how the key positions are
                   interpreted? */

        if(aKeyPos == EHuiSoftKeyLower || aKeyPos == EHuiSoftKeyLeft)
            {
            iLeftLabel->SetTextL(aText);
            }
        else
            {
            iRightLabel->SetTextL(aText);
            }
        }                
    }

/**
 * Sets background image to the softkeys.
 */
EXPORT_C void CHuiSoftKeyControl::SetBackgroundL(THuiImage image, TInt aLeftBorderWidth, TInt aRightBorderWidth)
    {
    // Create image brush for left softkey (if needed)
    if (iLeftLabelImageBrush==NULL)
        {
        iLeftLabel->EnableBrushesL();
        iLeftLabelImageBrush = CHuiImageBrush::NewL(image);
        iLeftLabelImageBrush->SetBorders(aLeftBorderWidth, aRightBorderWidth, 0, 0);
        iLeftLabel->Brushes()->AppendL(iLeftLabelImageBrush, EHuiHasOwnership);
        }
    else
        {
        // Set background image for the softkey
        iLeftLabelImageBrush->SetImage(image);
        iLeftLabelImageBrush->SetBorders(aLeftBorderWidth, aRightBorderWidth, 0, 0);
        }

    // Create image brush for right softkey (if needed)
    if (iRightLabelImageBrush==NULL)
        {
        iRightLabel->EnableBrushesL();
        iRightLabelImageBrush = CHuiImageBrush::NewL(image);
        iRightLabelImageBrush->SetBorders(aLeftBorderWidth, aRightBorderWidth, 0, 0);
        iRightLabel->Brushes()->AppendL(iRightLabelImageBrush, EHuiHasOwnership);
        }
    else
        {
        // Set background image for the softkey
        iRightLabelImageBrush->SetImage(image);
        iRightLabelImageBrush->SetBorders(aLeftBorderWidth, aRightBorderWidth, 0, 0);
        }

    }


EXPORT_C void CHuiSoftKeyControl::SetAnimationTimes( TUint aEnterTransition, TUint aExitTransition )
    {
    iEnterTransitionTime = aEnterTransition;
    iExitTransitionTime = aExitTransition;
    }


EXPORT_C void CHuiSoftKeyControl::EnterView(TUint aLabelMask)
    {
    // Show control
    TRAP_IGNORE(ShowL( *Display() ))

    // Set anchor points to target
    SetAnchorPoints(iCurrentLocation, ETrue, aLabelMask);

    // Do the transition
    iLayout->UpdateChildrenLayout(iEnterTransitionTime);
    }


EXPORT_C void CHuiSoftKeyControl::ExitView(TUint aLabelMask)
    {
    ExitView(iCurrentLocation, aLabelMask);
    }


TBool CHuiSoftKeyControl::OfferEventL(const THuiEvent& aEvent)
    {
    if(aEvent.IsCustomEvent())
        {
        if(aEvent.iParam == ECustomEventEnterView)
            {
            // Hide soft keys first
            SetAnchorPoints(iCurrentLocation, EFalse);
            iLayout->UpdateChildrenLayout(0);

            // Enter soft keys to view
            EnterView();
            return ETrue;
            }
        if(aEvent.iParam == ECustomEventUpdateLabels || aEvent.iParam == ECustomEventUpdateLabel)
            {
            if(iLeftText)
                {
                // Define label update mask
                TUint labelMask = ESoftkeyLabelLeft | ((aEvent.iParam == ECustomEventUpdateLabels) ? ESoftkeyLabelRight : 0);

                // Hide soft key(s) first
                SetAnchorPoints(iCurrentLocation, EFalse, labelMask);
                iLayout->UpdateChildrenLayout(0);

                // Update label text
                iLeftLabel->SetTextL(*iLeftText);
                delete iLeftText;
                iLeftText = 0;

                // Enter soft key(s) to view
                EnterView(labelMask);
                }
            if(iRightText)
                {
                // Define label update mask
                TUint labelMask = ESoftkeyLabelRight | ((aEvent.iParam == ECustomEventUpdateLabels) ? ESoftkeyLabelLeft : 0);

                // Hide soft key(s) first
                SetAnchorPoints(iCurrentLocation, EFalse, labelMask);
                iLayout->UpdateChildrenLayout(0);

                // Update label text
                iRightLabel->SetTextL(*iRightText);
                delete iRightText;
                iRightText = 0;

                // Enter soft key(s) to view
                EnterView(labelMask);
                }

            return ETrue;
            }
        }

    return EFalse;
    }


void CHuiSoftKeyControl::VisualLayoutUpdated(CHuiVisual& aVisual)
    {
    if(&aVisual == iLayout)
        {
        // Get current soft keys location
        THuiSkinLocation location = Env().Skin().Location(EHuiSkinElementSoftKeys);

        if(location != iCurrentLocation)
            {
            // Get the previous location
            THuiSkinLocation prevLocation = iCurrentLocation;

            // Set the current location
            iCurrentLocation = location;

            // Enter from new location after exit transition is completed.
            Env().Send(THuiCustomEventCommand(ECustomEventEnterView, this), ExitView(prevLocation));
            }
        }
    }


void CHuiSoftKeyControl::SetAnchorPoints(THuiSkinLocation aSkinLocation, TBool aShown, TUint aLabelMask)
    {
    // Recalculate visual rectangles depending on the skin location.
    switch(aSkinLocation)
        {
        case EHuiSkinLocationTop:
            HUI_DEBUG(_L("CHuiSoftKeyControl::ResetLabelRectangles() - EHuiSkinLocationTop not yet supported."));
            HUI_PANIC(THuiPanic::ENotImplemented)
            break;

        case EHuiSkinLocationRight:
            {
            // Determine soft key label x-offset from the right border of the layout.
            TReal32 leftX = aShown ? -KLabelWidthFactor : 0.0f;
            TReal32 rightX = aShown ? 0.0 : KLabelWidthFactor;

            if(aLabelMask & ESoftkeyLabelLeft)
                {
                // Define label text alignment
                iLeftLabel->SetAlign(EHuiAlignHRight, EHuiAlignVBottom);

                // Anchor top-left corner of the left soft key label (ordinal 0)
                iLayout->SetAnchor(EHuiAnchorTopLeft, 0,
                    EHuiAnchorOriginRight, EHuiAnchorOriginVCenter,
                    EHuiAnchorMetricRelativeToSize, EHuiAnchorMetricRelativeToSize,
                    THuiTimedPoint(leftX, 0.f));

                // Anchor bottom-right corner of the left soft key label (ordinal 0)
                iLayout->SetAnchor(EHuiAnchorBottomRight, 0,
                    EHuiAnchorOriginRight, EHuiAnchorOriginBottom,
                    EHuiAnchorMetricRelativeToSize, EHuiAnchorMetricRelativeToSize,
                    THuiTimedPoint(rightX, 0.f));
                }
            if(aLabelMask & ESoftkeyLabelRight)
                {
                // Define label text alignment
                iRightLabel->SetAlign(EHuiAlignHRight, EHuiAlignVTop);

                // Anchor top-left corner of the right soft key label (ordinal 1)
                iLayout->SetAnchor(EHuiAnchorTopLeft, 1,
                    EHuiAnchorOriginRight, EHuiAnchorOriginTop,
                    EHuiAnchorMetricRelativeToSize, EHuiAnchorMetricRelativeToSize,
                    THuiTimedPoint(leftX, 0.f));

                // Anchor bottom-right corner of the right soft key label (ordinal 1)
                iLayout->SetAnchor(EHuiAnchorBottomRight, 1,
                    EHuiAnchorOriginRight, EHuiAnchorOriginVCenter,
                    EHuiAnchorMetricRelativeToSize, EHuiAnchorMetricRelativeToSize,
                    THuiTimedPoint(rightX, 0.f));
                }
            
            break;
            }
        case EHuiSkinLocationBottom:
            {
            // Determine soft key label y-offset from the bottom of the layout.
            TReal32 topY = aShown ? -KLabelHeightFactor : 0.0f;
            TReal32 bottomY = aShown ? 0.0 : KLabelHeightFactor;

            if(aLabelMask & ESoftkeyLabelLeft)
                {
                // Define label text alignment
                iLeftLabel->SetAlign(EHuiAlignHLeft, EHuiAlignVBottom);
                
                // Retrieve text label visual width.
                /// @todo There should be a way to transform this width to relative coordinates
                /// otherwise we can't make sure that the text label is wide enough.
                TSize leftSize = iLeftLabel->TextExtents();

                // Anchor top-left corner of the left soft key label (ordinal 0)
                iLayout->SetAnchor(EHuiAnchorTopLeft, 0,
                    EHuiAnchorOriginLeft, EHuiAnchorOriginBottom,
                    EHuiAnchorMetricRelativeToSize, EHuiAnchorMetricRelativeToSize,
                    THuiTimedPoint(0.f, topY));

                // Anchor bottom-right corner of the left soft key label (ordinal 0)
                iLayout->SetAnchor(EHuiAnchorBottomRight, 0,
                    EHuiAnchorOriginLeft, EHuiAnchorOriginBottom,
                    EHuiAnchorMetricAbsolute, EHuiAnchorMetricRelativeToSize,
                    THuiTimedPoint(leftSize.iWidth, bottomY));
                }

            if(aLabelMask & ESoftkeyLabelRight)
                {
                // Define label text alignment
                iRightLabel->SetAlign(EHuiAlignHRight, EHuiAlignVBottom);

                // Retrieve text label visual width.
                /// @todo There should be a way to transform this width to relative coordinates
                /// otherwise we can't make sure that the text label is wide enough.
                TSize rightSize = iRightLabel->TextExtents();

                // Anchor top-left corner of the right soft key label (ordinal 1)
                iLayout->SetAnchor(EHuiAnchorTopLeft, 1,
                    EHuiAnchorOriginRight, EHuiAnchorOriginBottom,
                    EHuiAnchorMetricAbsolute, EHuiAnchorMetricRelativeToSize,
                    THuiTimedPoint(-rightSize.iWidth, topY));

                // Anchor bottom-right corner of the right soft key label (ordinal 1)
                iLayout->SetAnchor(EHuiAnchorBottomRight, 1,
                    EHuiAnchorOriginRight, EHuiAnchorOriginBottom,
                    EHuiAnchorMetricRelativeToSize, EHuiAnchorMetricRelativeToSize,
                    THuiTimedPoint(0.f, bottomY));
                }

            break;
            }
        case EHuiSkinLocationLeft:
            HUI_DEBUG(_L("CHuiSoftKeyControl::ResetLabelRectangles() - EHuiSkinLocationLeft not yet supported."));
            HUI_PANIC(THuiPanic::ENotImplemented)
            break;

        default:
            HUI_DEBUG(_L("CHuiSoftKeyControl::ResetLabelRectangles() - Unknown soft key draw location."));
            HUI_PANIC(THuiPanic::ENotImplemented)
            break;
        }
    }


TUint CHuiSoftKeyControl::ExitView(THuiSkinLocation aTarget, TUint aLabelMask)
    {
    // Bail out if target location is none.
    if(aTarget == EHuiSkinLocationNone)
        {
        return 0;
        }

    // Set anchor points to target
    SetAnchorPoints(aTarget, EFalse, aLabelMask);

    // Do the transition
    iLayout->UpdateChildrenLayout(iExitTransitionTime);

    return iExitTransitionTime;
    }
