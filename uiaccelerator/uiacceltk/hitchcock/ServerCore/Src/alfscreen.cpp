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
* Description:   Screen class
*
*/



#include "alfscreen.h"

#include <aknenv.h>
#include <AknsConstants.h>
#include <s32mem.h>

#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiControlGroup.h>
#include <uiacceltk/HuiEvent.h>
#include <uiacceltk/HuiDeckLayout.h>
#include <uiacceltk/HuiImageVisual.h>
#include <uiacceltk/HuiTransformation.h>
#include <uiacceltk/HuiBorderBrush.h>
#include <uiacceltk/HuiUtil.h>
#include <uiacceltk/HuiTextVisual.h>
#include <uiacceltk/HuiGradientBrush.h>
#include <uiacceltk/HuiImageBrush.h>
#include <uiacceltk/HuiCanvasVisual.h>

#ifdef SYMBIAN_BUILD_GCE
#include <bautils.h>
#endif

//_LIT8(KAlfSessionContainerControlTag, "Session container");
_LIT8(KAlfWindowFloatingSpriteContainerControlTag, "FSPRITE");
_LIT8(KAlfFullScreenEffectContainerControlTag, "EFFECT");
//_LIT8(KAlfWindowGroupTemporaryRemoveControlTag, "WGTEMP");

const TInt KAlfNumberOfFixedControlGroups = 2;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
CAlfScreen::CAlfScreen() 
	{
	};
	

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfScreen::ConstructL(TInt aScreenNum, MHuiDisplayRefreshObserver& aObserver, CHuiEnv& aHuiEnv, CAlfSharedDisplayCoeControl* aSharedCoeControl)
	{
	iScreenNum = aScreenNum;
	
	CAlfSharedDisplayCoeControl* coeControl = aSharedCoeControl;
	if (!coeControl)
		{
		// Create own coe control
		
		// \todo: This works only if the screen is created to screen 0. Do *not* work
		//        for external screens.
		iCoeControl = new (ELeave) CAlfSharedDisplayCoeControl();
		iCoeControl->ConstructL();
		iCoeControl->DrawableWindow()->EnableVisibilityChangeEvents();
		iCoeControl->DrawableWindow()->SetPosition(TPoint(0, 0)); 
		
		// This is not the main ALF native control, put it back
		iCoeControl->DrawableWindow()->SetOrdinalPosition(-2000);
		
		coeControl = iCoeControl;
		}
	
	// Resolve screen buffer uid
	// \todo currently only screen 0 and 1 are supported 
	TUid screenBufferUid;
	switch(iScreenNum)
		{
		case 0: 
			screenBufferUid = KHuiUidBackBufferScreen0;	
			break;
		case 1: 
		default: 
			screenBufferUid = KHuiUidBackBufferScreen1;	
			break;		
		}
	
	// create display
	iDisplay = &aHuiEnv.NewDisplayL(coeControl->Rect(), coeControl, 0, NULL,CHuiDisplay::EDisplayNormal, screenBufferUid);
	CommonConstructL(aObserver, aHuiEnv);
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfScreen::ConstructL(TInt aScreenNum, MHuiDisplayRefreshObserver& aObserver, CHuiEnv& aHuiEnv, RWindow* aWindow)
	{
	iScreenNum = aScreenNum;
		
	TUid screenBufferUid;
	switch(iScreenNum)
		{
		case 0: 
			screenBufferUid = KHuiUidBackBufferScreen0;	
			break;
		case 1: 
		default: 
			screenBufferUid = KHuiUidBackBufferScreen1;	
			break;		
		}
	
	// create display
	iDisplay = &aHuiEnv.NewDisplayL(aWindow, 0, NULL,CHuiDisplay::EDisplayNormal, screenBufferUid);
	CommonConstructL(aObserver, aHuiEnv);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfScreen::CommonConstructL( MHuiDisplayRefreshObserver& aObserver, CHuiEnv& aHuiEnv)
    {	
	
	iDisplay->iRefreshObservers.AppendL(aObserver);	
	
	// Create special control group for floating sprites
	iFloatingSpriteControlGroup = &aHuiEnv.NewControlGroupL(0xDEADBEEF + iScreenNum); // TODO
	CHuiControl* cntrl = new (ELeave) CHuiControl(aHuiEnv);
	CleanupStack::PushL(cntrl); 
	cntrl->ConstructL();
	cntrl->SetRole(EHuiWindowFloatingSpriteContainer);
	iFloatingSpriteControlGroup->AppendL(cntrl);
	CleanupStack::Pop(cntrl);
	CHuiCanvasVisual* layout = CHuiCanvasVisual::AddNewL(*cntrl);            
	layout->SetTagL(KAlfWindowFloatingSpriteContainerControlTag);	

	// Create special control group for full screen effects
	iFullscreenEffectControlGroup = &aHuiEnv.NewControlGroupL(0xDEADBEEF + 10 + iScreenNum ); // TODO
	CHuiControl* effectCntrl = new (ELeave) CHuiControl(aHuiEnv);		    
	CleanupStack::PushL(effectCntrl); 
	effectCntrl->ConstructL();
	effectCntrl->SetRole(EHuiFullScreenEffectContainer);
	iFullscreenEffectControlGroup->AppendL(effectCntrl);
	CleanupStack::Pop(effectCntrl);
	CHuiCanvasVisual* effectLayout = CHuiCanvasVisual::AddNewL(*effectCntrl);            
	effectLayout->SetTagL(KAlfFullScreenEffectContainerControlTag);

#ifdef SYMBIAN_BUILD_GCE
#ifndef __WINSCW__
    _LIT(KRDSupport, "c:\\resource\\errrd" );
    if (CHuiStatic::FsSession().Handle() && BaflUtils::FileExists( CHuiStatic::FsSession(), KRDSupport ))
        {
        iFpsControlGroup = &aHuiEnv.NewControlGroupL(0xDEADBEEF + 20 + iScreenNum); // TODO
    	
    	CHuiControl* fpsCntrl = new (ELeave) CHuiControl(aHuiEnv);	    
	    CleanupStack::PushL(fpsCntrl);
	    fpsCntrl->ConstructL();
	    fpsCntrl->SetRole(EHuiFpsIndicatorContainer);
	    iFpsControlGroup->AppendL(fpsCntrl);
	    CleanupStack::Pop(fpsCntrl);
        
        // FPS Counter with hitchcock drawing
        iFPSText = CHuiTextVisual::AddNewL(*fpsCntrl, NULL);
        iFPSText->SetColor(KRgbBlue);
        iFPSText->SetFlag(EHuiVisualFlagManualLayout);
        iFPSText->SetPos(THuiRealPoint(300,20)); // could be adjusted
        iFPSText->SetSize(THuiRealSize(50,50));
        }
#endif
#endif
	}
	

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TSize CAlfScreen::Size() const
    {
    return iDisplay->VisibleArea().Size();    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
CAlfScreen::~CAlfScreen() 
	{
	// this deletes also control and layout
	delete iFullscreenEffectControlGroup; 
	
	// this deletes also control and layout
	delete iFloatingSpriteControlGroup;
	
	delete iFpsControlGroup;
	
	delete iDisplay;
	iControlGroups.Close();
	
	delete iCoeControl;
	
	delete iSwRenderingTarget;
	};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfScreen::SetVisualTreeVisibilityChanged(TBool aChanged)
    {
    if (aChanged && iDisplay)
        {
        iDisplay->SetDirty();
        }
    iVisualTreeVisibilityChanged = aChanged;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TBool CAlfScreen::IsVisualTreeVisibilityChanged()
    {
    return iVisualTreeVisibilityChanged;    
    }
     
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TInt CAlfScreen::FixedControlGroupCount() const
    {
    return !iFpsControlGroup ? 
        KAlfNumberOfFixedControlGroups :
        KAlfNumberOfFixedControlGroups + 1;
    }
