/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <AknUtils.h>
#include <uiacceltk/HuiStatic.h>

#include "huiskinbackroundlayout.h"

static inline void GetScalableViaPane( TRect& aRect, const TRect& aParentRect,
    const TAknWindowComponentLayout& aIntermediateLayout,
    const TAknWindowComponentLayout& aFinalLayout )
    {
    TAknLayoutRect layout;
    layout.LayoutRect( aParentRect,
        TAknWindowComponentLayout::Compose(
        aIntermediateLayout, aFinalLayout).LayoutLine() );
    aRect = layout.Rect();
    }


TAknsItemID GetParent( const TAknsItemID& aIID )
    {
    if( aIID.iMajor == EAknsMajorSkin )
        {
        switch( aIID.iMinor )
            {
            case EAknsMinorQsnBgScreen:
                return KAknsIIDNone;

            case EAknsMinorQsnBgScreenIdle:
            case EAknsMinorQsnBgScreenMp:
                return KAknsIIDQsnBgScreen;
            case EAknsMinorQsnBgAreaMain:
            case EAknsMinorQsnBgAreaMainAppsGrid:
            case EAknsMinorQsnBgAreaMainListGene:
            case EAknsMinorQsnBgAreaMainListSet:
            case EAknsMinorQsnBgAreaMainMessage:
                return KAknsIIDQsnBgScreen;

            case EAknsMinorQsnBgAreaMainHigh:
            case EAknsMinorQsnBgAreaMainCalc:
            case EAknsMinorQsnBgAreaMainPinb:
            case EAknsMinorQsnBgAreaMainQdial:
                return KAknsIIDQsnBgScreen;

            case EAknsMinorQsnBgAreaMainMp:
                return KAknsIIDQsnBgScreenMp;

            case EAknsMinorQsnBgAreaMainFlat:
            case EAknsMinorQsnBgAreaMainSmall:
                return KAknsIIDQsnBgScreen;
#ifdef RD_FULLSCREEN_WALLPAPER
            case EAknsMinorWallpaper:
                return KAknsIIDQsnBgScreenIdle;
#else
            case EAknsMinorWallpaper:
                return KAknsIIDQsnBgAreaMainIdle;
#endif //RD_FULLSCREEN_WALLPAPER

            case EAknsMinorQsnBgAreaMainIdle:
            case EAknsMinorQsnBgAreaMainIdleDimmed:
#ifdef RD_FULLSCREEN_WALLPAPER
                return KAknsIIDWallpaper;
#else
                return KAknsIIDQsnBgScreenIdle;
#endif // RD_FULLSCREEN_WALLPAPER

            case EAknsMinorQsnBgAreaStatus:
                return KAknsIIDQsnBgScreen;

            case EAknsMinorQsnBgAreaStatusIdle:
                return KAknsIIDQsnBgScreenIdle;

            case EAknsMinorQsnBgAreaStaconRt:
            case EAknsMinorQsnBgAreaStaconLt:
            case EAknsMinorQsnBgAreaStaconRb:
            case EAknsMinorQsnBgAreaStaconLb:
                return KAknsIIDQsnBgScreen;

            case EAknsMinorQsnBgAreaStaconRtIdle:
            case EAknsMinorQsnBgAreaStaconLtIdle:
            case EAknsMinorQsnBgAreaStaconRbIdle:
            case EAknsMinorQsnBgAreaStaconLbIdle:
                return KAknsIIDQsnBgScreenIdle;

            case EAknsMinorQsnBgAreaControl:
            case EAknsMinorQsnBgAreaControlPopup:
                return KAknsIIDQsnBgScreen;

            case EAknsMinorQsnBgAreaControlMp:
                return KAknsIIDQsnBgScreenMp;

            case EAknsMinorQsnBgAreaControlIdle:
                return KAknsIIDQsnBgScreenIdle;

            case EAknsMinorQsnBgColumn0:
            case EAknsMinorQsnBgColumnA:
            case EAknsMinorQsnBgColumnAB:
            case EAknsMinorQsnBgColumnC0:
            case EAknsMinorQsnBgColumnCA:
            case EAknsMinorQsnBgColumnCAB:
                return KAknsIIDQsnBgAreaMainListGene;

            case EAknsMinorQsnBgSliceList0:
            case EAknsMinorQsnBgSliceListA:
            case EAknsMinorQsnBgSliceListAB:
                return KAknsIIDQsnBgAreaMainListGene;

            // INSERTION POINT for layout parent relations
            }
        }
    else if( aIID.iMajor == EAknsMajorAvkon )
        {
        switch( aIID.iMinor )
            {
            case EAknsMinorQsnBgNavipaneSolid:
            case EAknsMinorQsnBgNavipaneWipe:
                return KAknsIIDQsnBgAreaStatus;

            case EAknsMinorQsnBgNavipaneSolidIdle:
            case EAknsMinorQsnBgNavipaneWipeIdle:
                return KAknsIIDQsnBgAreaStatusIdle;
            }
        }

    return KAknsIIDDefault;
    }
    
TBool GetItemLayout( const TAknsItemID& aIID,
    const TRect& aParentRect, TRect& aRect  )
    {
    if (!CCoeEnv::Static())
        { //ToDO
        aRect = aParentRect;
        return ETrue;
        }
    //---------------------------------------
    const AknLayoutScalable_Avkon::CInstance& iAknLayoutScalable_Avkon = AknLayoutScalable_Avkon::CustomisationInstance();
    TAknWindowLineLayout screenLayout = AknLayout::screen();
    TRect iScreenRect = screenLayout.Rect();
    //---------------------------------------
    TRect normParentRect( TPoint(0,0), aParentRect.Size() );


    TAknLayoutRect layRect;

    if( aIID.iMajor == EAknsMajorSkin ) {
        switch( aIID.iMinor )
            {
            case EAknsMinorQsnBgScreen:
#ifdef RD_FULLSCREEN_WALLPAPER
            case EAknsMinorWallpaper:
#endif //RD_FULLSCREEN_WALLPAPER
                {
                aRect = iScreenRect;
                return ETrue;
                }
            case EAknsMinorQsnBgScreenIdle:
            case EAknsMinorQsnBgScreenMp:
                {
                aRect = normParentRect;
                return ETrue;
                }

            case EAknsMinorQsnBgAreaMainAppsGrid:
                {
/*                if (iGridMode == -1)
                    {
                    // if gridmode is -1, it has not beed read
                    // so read it here
                    TRAP_IGNORE( iGridMode = GetGridModeL() );
                    }
                if (!Layout_Meta_Data::IsLandscapeOrientation() && iGridMode == 1) //3x4 mode
                    {
                    layRect.LayoutRect( normParentRect,
                        iAknLayoutScalable_Avkon.main_pane(7));
                    aRect = layRect.Rect();
                    return ETrue;
                    }
                else // normal 3x3 mode
                    {
                    return AknLayoutUtils::LayoutMetricsRect(
                        AknLayoutUtils::EMainPane, aRect );
                    }*/
                }

            case EAknsMinorQsnBgAreaMain:
            case EAknsMinorQsnBgAreaMainListGene:
            case EAknsMinorQsnBgAreaMainListSet:
            case EAknsMinorQsnBgAreaMainMessage:
                layRect.LayoutRect( normParentRect,
                AknLayout::main_pane( normParentRect, 0, 1, 1 ) );
                aRect = layRect.Rect();
                return ETrue;

            case EAknsMinorQsnBgAreaMainFlat:
                {
                layRect.LayoutRect( normParentRect,
                     iAknLayoutScalable_Avkon.main_pane(7));
                    aRect = layRect.Rect();
                    return ETrue;
                }
            case EAknsMinorQsnBgAreaMainSmall:
                   {
                   layRect.LayoutRect( normParentRect,
                    iAknLayoutScalable_Avkon.main_pane(6));
                    aRect = layRect.Rect();
                    return ETrue;
                  }

            case EAknsMinorQsnBgAreaMainHigh:
            case EAknsMinorQsnBgAreaMainCalc:
            case EAknsMinorQsnBgAreaMainPinb:
            case EAknsMinorQsnBgAreaMainQdial:
            case EAknsMinorQsnBgAreaMainMp:
                {
                layRect.LayoutRect( normParentRect,
                    AknLayout::main_pane( normParentRect, 0, 0, 1 ) );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgAreaMainIdle:
            case EAknsMinorQsnBgAreaMainIdleDimmed:
                {
                layRect.LayoutRect( normParentRect,
                    AknLayout::main_pane( normParentRect, 0, 1, 1 ) );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgAreaStatus:
                {
                GetScalableViaPane( aRect, normParentRect,
                    iAknLayoutScalable_Avkon.area_top_pane(0),
                    iAknLayoutScalable_Avkon.status_pane(0) );

                return ETrue;
                }

            case EAknsMinorQsnBgAreaStatusIdle:
                {
                GetScalableViaPane( aRect, normParentRect,
                    iAknLayoutScalable_Avkon.area_top_pane(0),
                    iAknLayoutScalable_Avkon.status_pane(0) );

                return ETrue;
                }

            case EAknsMinorQsnBgAreaStaconRt:
            case EAknsMinorQsnBgAreaStaconLt:
            case EAknsMinorQsnBgAreaStaconRtIdle:
            case EAknsMinorQsnBgAreaStaconLtIdle:
                {
                GetScalableViaPane( aRect, normParentRect,
                    iAknLayoutScalable_Avkon.area_top_pane(2),
                    iAknLayoutScalable_Avkon.stacon_top_pane() );
                return ETrue;
                }

            case EAknsMinorQsnBgAreaStaconRb:
            case EAknsMinorQsnBgAreaStaconLb:
            case EAknsMinorQsnBgAreaStaconRbIdle:
            case EAknsMinorQsnBgAreaStaconLbIdle:
                {
                GetScalableViaPane( aRect, normParentRect,
                    iAknLayoutScalable_Avkon.area_bottom_pane(2),
                    iAknLayoutScalable_Avkon.stacon_bottom_pane() );
                return ETrue;
                }

            case EAknsMinorQsnBgAreaControl:
            case EAknsMinorQsnBgAreaControlPopup:
            case EAknsMinorQsnBgAreaControlIdle:
            case EAknsMinorQsnBgAreaControlMp:
                {
                layRect.LayoutRect( normParentRect,
                    AknLayout::control_pane( normParentRect) );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgColumn0:
                {
                layRect.LayoutRect( normParentRect,
                    SkinLayout::Column_background_and_list_slice_skin_placing_Line_1() );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgColumnA:
                {
                layRect.LayoutRect( normParentRect,
                    SkinLayout::Column_background_and_list_slice_skin_placing_Line_2() );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgColumnAB:
                {
                layRect.LayoutRect( normParentRect,
                    SkinLayout::Column_background_and_list_slice_skin_placing_Line_3() );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgColumnC0:
                {
                layRect.LayoutRect( normParentRect,
                    SkinLayout::Column_background_and_list_slice_skin_placing_Line_4() );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgColumnCA:
                {
                layRect.LayoutRect( normParentRect,
                    SkinLayout::Column_background_and_list_slice_skin_placing_Line_5() );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgColumnCAB:
                {
                layRect.LayoutRect( normParentRect,
                    SkinLayout::Column_background_and_list_slice_skin_placing_Line_6() );
                aRect = layRect.Rect();
                return ETrue;
                }

            case EAknsMinorQsnBgSliceList0:
            case EAknsMinorQsnBgSliceListA:
            case EAknsMinorQsnBgSliceListAB:
                {
                layRect.LayoutRect( normParentRect,
                    SkinLayout::Column_background_and_list_slice_skin_placing_Line_7() );
                aRect = layRect.Rect();
                return ETrue;
                }
#ifndef RD_FULLSCREEN_WALLPAPER
            case EAknsMinorWallpaper:
                {
                aRect = normParentRect;
                return ETrue;
                }
#endif //RD_FULLSCREEN_WALLPAPER

            // INSERTION POINT for layout information retrieval
            }
        }
    else if( aIID.iMajor == EAknsMajorAvkon )
        {
        switch( aIID.iMinor )
            {
            case EAknsMinorQsnBgNavipaneSolid:
                {
                layRect.LayoutRect( normParentRect,
                    iAknLayoutScalable_Avkon.status_pane_g1(0) );
                aRect = layRect.Rect();
                // Artificial correction
                aRect.iTl.iX = 0;
                aRect.SetWidth( aParentRect.Width() );
                return ETrue;
                }

            case EAknsMinorQsnBgNavipaneWipe:
                {
                layRect.LayoutRect( normParentRect,
                    iAknLayoutScalable_Avkon.status_pane_g1(0) );
                aRect = layRect.Rect();
                // Artificial correction
                aRect.iTl.iX = 0;
                aRect.SetWidth( aParentRect.Width() );
                return ETrue;
                }

            case EAknsMinorQsnBgNavipaneSolidIdle:
                {
                layRect.LayoutRect( normParentRect,
                    iAknLayoutScalable_Avkon.status_pane_g1(0) );
                aRect = layRect.Rect();
                // Artificial correction
                aRect.iTl.iX = 0;
                aRect.SetWidth( aParentRect.Width() );
                return ETrue;
                }

            case EAknsMinorQsnBgNavipaneWipeIdle: 
                {
                layRect.LayoutRect( normParentRect,
                    iAknLayoutScalable_Avkon.status_pane_g1(0) );
                aRect = layRect.Rect();
                // Artificial correction
                aRect.iTl.iX = 0;
                aRect.SetWidth( aParentRect.Width() );
                return ETrue;
                }
            }
        }

    return EFalse;
    }

EXPORT_C void GetRectForItem(const TAknsItemID& aID, TRect& aParentRect, TRect& aOutRect)
    {
    TAknsItemID parent = GetParent(aID);
    TRect parentRect;
    TRect itemRect;

    if (parent != KAknsIIDDefault)//KAknsIIDNone) // go recursively down to the root element
        {
        GetRectForItem(parent, aParentRect, aOutRect);
        }
    else // root element
        {
        CHuiStatic::LayoutMetricsRect(AknLayoutUtils::EScreen,  aParentRect);
        }
    GetItemLayout( aID,aParentRect, aOutRect );
    aParentRect = aOutRect;
    }
