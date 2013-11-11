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


#include <apgtask.h>
#include <AknDoc.h>
#include "alf/alfappui.h"
#include "alf/alfappserver.h"
#include <uiacceltk/HuiEnv.h>
#include <aknenv.h>
#include <AknsConstants.h>
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
#include <uiacceltk/HuiRoster.h>

#ifdef ALF_USE_CANVAS
#include <uiacceltk/HuiCanvasVisual.h>
#endif

#include "alflogger.h"
#include "alf/alfappsrvsessionbase.h"
#include "alfsrvresourcemanager.h"
#include "alfsrvsettingshandler.h"
#include <uiacceltk/HuiTextureManager.h>
#include "alfsrvtexturemanager.h"

#include "alfstreamerserver.h"
#include "alfshareddisplaycoecontrol.h"
#include "alfbridge.h"
#include "alfstreamerconsts.h"
#include "alfscreen.h"
#include <akntranseffect.h>

#include "alfwindowmanager.h"
#include "alfwindowstructs.h"
#include <e32property.h>
#include "HuiFxEffect.h"
#include <akntransitionutils.h>
#include <alf/AlfTransEffectPlugin.h>
#include "alfwindowdata.h"
#include "huieffectable.h"
#include <akntranseffect.h>
#include "HuiRenderPlugin.h"
#include "huicanvasgc.h"
#include "huicanvasrenderbuffer.h"
#include "alfeffectutils.h"
#include "alfrenderstageutils.h" // for KAlfPSUidSynchronizer & KAlfPSKeySynchronizer
#include "alfdrawerinternal.h"

#ifdef HUI_DEBUG_TRACK_DRAWING
#include <alfcommanddebug.h>
#endif

#ifdef _ALF_FXLOGGING
#include <alfcommanddebug.h>
#endif

#ifdef SYMBIAN_BUILD_GCE
#include <bautils.h>
#endif

const TInt KVisualTransformationStepRotate    = 0;

const TReal32 KAlfVisualDefaultOpacity = 1.0f;
//const TReal32 KAlfVisualDefaultOpacity = 0.5f;
const TInt KEffectCleanupDelayInMs = 10;

const TInt KPreventingCollisionOfIdsInSynchDrawRequests = 35;
const TInt KRequestFrameSynchForComposition = -1; // bogus

const TInt KAlfFSTaskSwapperAppStart = 1507;
_LIT8(KAlfWindowGroupContainerControlTag, "WGROUP");

#define USE_APPLICATION_ENDFULLSCREEN_TIMEOUT
// This debug option shows window groups in a grid
//#define ALF_DEBUG_VISUALIZE_WINDOWGROUP_ORDER

// Timeout for manual refresh
const TInt KAlfManualRefreshTimeout = 35000;

_LIT_SECURITY_POLICY_PASS(KAlwaysPass);
const TUid KPSSysCategory = { KUidSystemCategoryValue };
const TUint32 KAlfPSKeyHSVisible = 0x2002EA91;

const TInt KFadeAction = 6000;

// IsLayoutSwitchReady heuristic constants:

// Duration to wait that visual display rects are fully inside fullscreen rect. 
// 1 unit = 50ms.
const TInt KAlfCheckVisualDisplayRect = 15;

// Duration to wait that visual has drawing inside screen area.
const TInt KAlfCheckVisualDrawRect = 15;

// Duration to wait that visual has drawing that covers the whole display rect.
const TInt KAlfCheckVisualDrawingComplete = 20;

// Duration to wait that visual has some drawing (not necessarily complete).
const TInt KAlfCheckVisualDrawingNotEmpty = 28;

// Duration to wait that fullscreen is covered by visual drawing.
const TInt KAlfCheckFullscreenCoveredByVisualDrawing = 50;

// Duration to wait if coverage is modified by multiple control groups.
// This may indicate that topmost application hasn't yet had time to update
// its layout properly.
const TInt KAlfCheckCoverageFromMultipleGroups = 20;


NONSHARABLE_CLASS( TAlfBridgeDrawerWrapper ) : public MAlfDrawerScreenInterface
    {
public:
    TAlfBridgeDrawerWrapper(CAlfBridge& aBridge);
    TInt GetSizeAndRotation(TSize& aSize, TInt& aRotation);
    TInt ReadPixels(CFbsBitmap* aBitmap);
private:
    CAlfBridge& iBridge;
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
CAlfBridge* CAlfBridge::NewL( CAlfStreamerBridge** aHost, CHuiEnv* aEnv )
    {
    CAlfBridge* self = new (ELeave) CAlfBridge( aHost );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap)
    {
    aBitmap = iHack;
    aMaskBitmap = iDummyMask;
    };


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
CAlfBridge::CAlfBridge(CAlfStreamerBridge** aHost)
    : iHost(aHost), iCurrentMemoryLevel(EHuiMemoryLevelNormal)
	{
	}
    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
CAlfBridge::~CAlfBridge()
	{
    iTempRegion2.Close();
    
    iAlfNativeClientsWgIds.Close();
	delete iOrphanStorage;
	delete iFadeEffectFile;
	iWindowHashArray.Close();
	for( TInt i = 0; i< iAlfRegisteredEffects.Count(); i++ )
	    {
	    delete iAlfRegisteredEffects[i].iEffectFile;
	    }
	iAlfRegisteredEffects.Close();
    delete iEffectEndTimer;
    iDeadControlGroups.Close();
    iEffectWindowGroups.Close();
    delete iFSFxData;
    delete iFSFXDataPart2;

    if (iActivated)
        {
        iBridgerClient.Close();        
        }

#ifdef HUI_DEBUG_TRACK_DRAWING
    delete iCommandDebug;
#endif
    delete iCursorTimer;
    delete iLayoutSwitchEffectCoordinator;
    delete iManualRefreshTimer;
	}
    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ConstructL(CHuiEnv* aEnv)
    {
    iHuiEnv = aEnv;
    
    iIdForEAlfDSSynchronizeOp = 0;
    iIdForLayoutSwitchFrameSync = iIdForEAlfDSSynchronizeOp - KPreventingCollisionOfIdsInSynchDrawRequests;

#ifdef SYMBIAN_BUILD_GCE
    iPrintFPS = EFalse;
#ifndef __WINSCW__
    _LIT(KRDSupport, "c:\\resource\\errrd" );
    RFs& fs = CHuiStatic::FsSession();
    if (fs.Handle() && BaflUtils::FileExists( fs, KRDSupport ))
        {
        iPrintFPS = ETrue;
        }
#endif
#endif //SYMBIAN_BUILD_GCE
   
    iEffectEndTimer = CAlfEffectEndTimer::NewL( *this );
    iWindowHashArray.ReserveL( 500 );
    iAlfRegisteredEffects.ReserveL(10);
    
#ifdef HUI_DEBUG_TRACK_DRAWING
    iCommandDebug = CAlfCommandDebug::NewL();
#endif
    
    iLayoutSwitchEffectCoordinator = new (ELeave) CAlfLayoutSwitchEffectCoordinator(*this);
    iAlfSecureId = RThread().SecureId();
	
	// Create control for the orphaned windows, which control group id deleted before them
    iOrphanStorage = new (ELeave) CHuiControl(*iHuiEnv);
    iOrphanStorage->ConstructL();
	
    RegisterFadeEffectL();
    
    iManualRefreshTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    RProperty::Define( KPSSysCategory, KAlfPSKeyHSVisible, RProperty::EInt, KAlwaysPass, KAlwaysPass );
    RProperty::Set( KPSSysCategory, KAlfPSKeyHSVisible, 0 );
    
    }

// ---------------------------------------------------------------------------
//  RegisterFadeEffectL
//
//  Note, If theme DOES NOT register its own fade effect, another hardcoded fade 
//  effect will be used instead. For example, if theme effects are turned off, 
//  a fade that does not belong to the theme may be in use.
// ---------------------------------------------------------------------------
void CAlfBridge::RegisterFadeEffectL()
    {
    // TODO: RND, REMOVE MMC DRIVE-F, WHEN NOT REQUIRED FOR TESTING
    _LIT(KDrivePrefence,"FZC"); 
    // Force register fade effect. Try first MMC, then ROM.
    CHuiFxEngine* engine = iHuiEnv->EffectsEngine();
    RFs& fs = CHuiStatic::FsSession();
    if (fs.Handle() && engine)
        {
        _LIT(KFadeEffectPath, ":\\resource\\effects\\fade_effect.fxml");
        TBufC<4> drives(KDrivePrefence);
        HBufC* effectFullName = HBufC::NewLC(KFadeEffectPath().Length() + 1);
        
        for(TInt i=0; i< drives.Length(); i++)
            {
            effectFullName->Des().Copy(drives.Mid(i,1));
            effectFullName->Des().Append(KFadeEffectPath);
    
            if (BaflUtils::FileExists( fs, *effectFullName ))
                {
                DoRegisterEffectL(*effectFullName, KFadeAction);
                break;
                }
            }
        CleanupStack::PopAndDestroy(effectFullName);
        DoSetCachedFadeEffectL();
        }
    }

// ---------------------------------------------------------------------------
// DoSetCachedFadeEffectL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::DoSetCachedFadeEffectL()
    {
    for (TInt i = 0; i < iAlfRegisteredEffects.Count(); i++)
        {
        //KFadeAction = 6000, indicating the action corroposnds to fading
        if (iAlfRegisteredEffects[i].iAction == KFadeAction)
            {
            delete iFadeEffectFile;
            iFadeEffectFile = NULL;
            iFadeEffectFile = iAlfRegisteredEffects[i].iEffectFile->AllocL();
            __ALFLOGSTRING1( ">> Setting fade effect file  %S", iFadeEffectFile );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// AddNewScreenL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::AddNewScreenL(CAlfSharedDisplayCoeControl* aSharedCoeControl)
    {
    TInt screenNumber = iAlfScreens.Count(); // \todo this might not be the same as real WServ screen number
    
    // We do not yet support drawing winGC draw commands to other than primary screen.
    // As fully functional and tested implementation is not there, better to panic here.
    if (screenNumber!=0)
        {
        __ALFLOGSTRING("CAlfBridge::AddNewScreenL. Fatal error! Only screen 0 is supported in Alf server!");
        User::Invariant();
        }
         
    CAlfScreen* screen = new(ELeave) CAlfScreen(); 
    CleanupStack::PushL(screen);
    screen->ConstructL(screenNumber, *this, *iHuiEnv, aSharedCoeControl);
    iAlfScreens.AppendL( screen );
    CleanupStack::Pop(screen);

	ShowControlGroupL(screen->iDisplay->Roster(), *(screen->iFloatingSpriteControlGroup), KHuiRosterShowAtTop, screenNumber); 
	ShowControlGroupL(screen->iDisplay->Roster(), *(screen->iFullscreenEffectControlGroup), KHuiRosterShowAtTop, screenNumber); 
    
	screen->iFloatingSpriteControlGroup->SetAcceptInput(EFalse);
	screen->iFullscreenEffectControlGroup->SetAcceptInput(EFalse);
	
	if ( screen->iFpsControlGroup )
	    {
	    ShowControlGroupL(screen->iDisplay->Roster(), *(screen->iFpsControlGroup), KHuiRosterShowAtTop, screenNumber); 
	    screen->iFpsControlGroup->SetAcceptInput(EFalse);
	    }
    }

//------------------------------------------------------------------------------
// Dumdidumdidum..
//------------------------------------------------------------------------------
void CAlfBridge::AddNewScreenFromWindowL(RWindow* aWindow)
    {
    TInt screenNumber = iAlfScreens.Count(); // \todo this might not be the same as real WServ screen number
         
    CAlfScreen* screen = new(ELeave) CAlfScreen(); 
    CleanupStack::PushL(screen);
    screen->ConstructL(screenNumber, *this, *iHuiEnv, aWindow);
    iAlfScreens.AppendL( screen );
    CleanupStack::Pop(screen);

	ShowControlGroupL(screen->iDisplay->Roster(), *(screen->iFloatingSpriteControlGroup), KHuiRosterShowAtTop, screenNumber); 
	ShowControlGroupL(screen->iDisplay->Roster(), *(screen->iFullscreenEffectControlGroup), KHuiRosterShowAtTop, screenNumber); 
    
    screen->iFloatingSpriteControlGroup->SetAcceptInput(EFalse);
    screen->iFullscreenEffectControlGroup->SetAcceptInput(EFalse);
    
    if ( screen->iFpsControlGroup )
	    {
	    ShowControlGroupL(screen->iDisplay->Roster(), *(screen->iFpsControlGroup), KHuiRosterShowAtTop, screenNumber); 
	    screen->iFpsControlGroup->SetAcceptInput(EFalse);
	    }
    }



// ---------------------------------------------------------------------------
// AddVisual
// ---------------------------------------------------------------------------
// 
void CAlfBridge::AddVisual( 
        TInt aWindowNodeId, 
        TInt aClientSideId, 
        TInt aClientSideGroupId,
        CHuiCanvasVisual* aVisual )
    {
    __ALFFXLOGSTRING4("CAlfBridge::AddVisual id: %d (%d,%d), visual: 0x%x", aWindowNodeId, aClientSideId, aClientSideGroupId, aVisual);
    THashVisualStruct visualStruct( aVisual, aClientSideId, aClientSideGroupId);
    iWindowHashArray.Insert( aWindowNodeId, visualStruct );
    iPreviouslySearchedVisualId = aWindowNodeId;
    iPreviouslySearchedVisual = aVisual;

    AMT_INC_COUNTER( iTotalVisualCount );
    
    AMT_MAP_BRIDGE_ADD_VISUAL();
    }

// ---------------------------------------------------------------------------
// RemoveVisual
// ---------------------------------------------------------------------------
// 
void CAlfBridge::RemoveVisual( TInt aWindowNodeId )
    {
    __ALFFXLOGSTRING1("CAlfBridge::RemoveVisual 0x%x", aWindowNodeId);
    iWindowHashArray.Remove( aWindowNodeId );
    iPreviouslySearchedVisualId = 0;
    
    AMT_DEC_COUNTER( iTotalVisualCount );
    
    AMT_MAP_INC_VALUE( iIntMap, aWindowNodeId, EAlfModuleTestTypeBridgeReleaseWindow );
    AMT_MAP_SET_VALUE( iBoolMap, aWindowNodeId, ETrue, EAlfModuleTestTypeBridgeReleaseWindow );
    }
    
// ---------------------------------------------------------------------------
// FindVisual
// ---------------------------------------------------------------------------
// 
CHuiCanvasVisual* CAlfBridge::FindVisual(TInt aWindowNodeId )
    {
    if ( iPreviouslySearchedVisualId == aWindowNodeId )
        {
        return iPreviouslySearchedVisual;
        }
    
    THashVisualStruct* visualStruct = iWindowHashArray.Find( aWindowNodeId );
    if ( visualStruct )
        {
        iPreviouslySearchedVisualId = aWindowNodeId;
        iPreviouslySearchedVisual = visualStruct->iVisual;
        return iPreviouslySearchedVisual; 
        }
    __ALFLOGSTRING1("CAlfBridge::FindVisual - Visual 0x%x not found", aWindowNodeId);
    return NULL;
    }

// ---------------------------------------------------------------------------
// FindVisualByClientSideId
// ---------------------------------------------------------------------------
// 
CHuiCanvasVisual* CAlfBridge::FindVisualByClientSideIds(
    TUint32 aClientSideId, 
	TUint32 aClientSideGroupId )
    {
    THashMapIter<TUint32, THashVisualStruct> iter(iWindowHashArray);
    THashVisualStruct const * node = 0;
    do
        {
        node = iter.NextValue();
        if (node 
                && (*node).iClientSideId==aClientSideId
                && (*node).iClientSideGroupId==aClientSideGroupId )
            {
            return (*node).iVisual;
            }
        }
    while(node);
	return NULL;
    }
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
CHuiControlGroup* CAlfBridge::FindControlGroup(TInt aWindowGroupNodeId, TInt aScreenNumber )
    {
    for (TInt i=0; i<iAlfScreens[aScreenNumber]->iControlGroups.Count();i++)
        {
        if (iAlfScreens[aScreenNumber]->iControlGroups[i].iWindowGroupNodeId == aWindowGroupNodeId)
            {
            return iAlfScreens[aScreenNumber]->iControlGroups[i].iControlGroup;
            }
        }        
    return NULL;       
    }

// ---------------------------------------------------------------------------
// FindClientWindowGroupId
// ---------------------------------------------------------------------------
//
TInt CAlfBridge::FindClientWindowGroupId( TInt aScreenNumber, CHuiControlGroup& aControlGroup  )
    {
    for (TInt i=0; i<iAlfScreens[aScreenNumber]->iControlGroups.Count();i++)
        {
        if( iAlfScreens[aScreenNumber]->iControlGroups[i].iControlGroup == &aControlGroup )
            {
            return iAlfScreens[aScreenNumber]->iControlGroups[i].iClientWindowGroupId;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// FindWindowGroupNodeId
// ---------------------------------------------------------------------------
//
TInt CAlfBridge::FindWindowGroupNodeId( TInt aScreenNumber, CHuiControlGroup& aControlGroup ) const
    {
    for (TInt i=0; i<iAlfScreens[aScreenNumber]->iControlGroups.Count();i++)
        {
        if( iAlfScreens[aScreenNumber]->iControlGroups[i].iControlGroup == &aControlGroup )
            {
            return iAlfScreens[aScreenNumber]->iControlGroups[i].iWindowGroupNodeId;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CHuiControlGroup* CAlfBridge::FindControlGroupByWindowGroupId( TInt aWindowGroupId, TInt& aScreenNumber, TAlfControlGroupEntry** aAlfGroup )
    {
    TInt firstWin = aScreenNumber;
    TInt lastWin = aScreenNumber;
    if ( aScreenNumber == KErrNotFound )
        {
        firstWin = 0;
        lastWin = iAlfScreens.Count()-1;
        }
    
    for ( TInt j = firstWin; j <= lastWin; j++ )
        {
        for ( TInt i = 0; i < iAlfScreens[j]->iControlGroups.Count(); i++ )
            {
            if ( iAlfScreens[j]->iControlGroups[i].iClientWindowGroupId == aWindowGroupId )
                {
                aScreenNumber = j;
                if ( aAlfGroup != NULL )
                    {
                    *aAlfGroup = &iAlfScreens[j]->iControlGroups[i];
                    }
                return iAlfScreens[j]->iControlGroups[i].iControlGroup;
                }
            }        
        }
    return NULL;       
    }

// ---------------------------------------------------------------------------
// FindControlGroupBySecureId
// ---------------------------------------------------------------------------
//
CHuiControlGroup* CAlfBridge::FindControlGroupBySecureId( TInt aSecureId, TInt aWgId ) const
    {
    if ( aSecureId == 0 )
        {
        return NULL;
        }
    
    for ( TInt i = 0; i < iAlfScreens[0]->iControlGroups.Count(); i++ )
        {
        TAlfControlGroupEntry& groupEntry = iAlfScreens[0]->iControlGroups[i];
        if ( groupEntry.iSecureId == aSecureId )
            {
            if ( aWgId != -1 )
                {
                // match the window group as well
                if ( groupEntry.iClientWindowGroupId == aWgId )
                    {
                    // found match
                    return groupEntry.iControlGroup;
                    }
                // found a matching SID, but the window group ID was incorrect. keep going...
                }
            else
                {
                // done for the day
                return groupEntry.iControlGroup;
                }
            }
        }        
    return NULL;       
    }

// ---------------------------------------------------------------------------
// FindControlGroupByFullScreenToEffect
// ---------------------------------------------------------------------------
//
CHuiControlGroup* CAlfBridge::FindControlGroupByFullScreenToEffect() const
    {
    if ( iFSFxData )
        {
        return FindControlGroupBySecureId( iFSFxData->iToSecureId, iFSFxData->iToWg ); 
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// FindControlGroupByFullScreenFromEffect
// ---------------------------------------------------------------------------
//
CHuiControlGroup* CAlfBridge::FindControlGroupByFullScreenFromEffect() const
    {
    if ( iFSFxData )
        {
        return FindControlGroupBySecureId( iFSFxData->iFromSecureId, iFSFxData->iFromWg );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//

void CAlfBridge::ListFamilyTreeL( RPointerArray<CHuiLayout>& aArray, const CHuiLayout* aLayout )
    {
    if (aLayout->Count())
        {
        aArray.Append(aLayout);
        
        for(TInt i = 0; i< aLayout->Count(); i++)
            {
            ListFamilyTreeL(aArray, (CHuiLayout*)&aLayout->Visual(i));
            }
        }
    else
        {
        aArray.Append(aLayout);
        }
    }

// ---------------------------------------------------------------------------
// ResolveAfterEffectAppearingApplicationL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ResolveAfterEffectAppearingApplicationL(CHuiControlGroup* aGroup)
    {
#ifdef USE_APPLICATION_ENDFULLSCREEN_TIMEOUT
    if (iFSFxData && iFSFxData->iEffectType == CFullScreenEffectState::EExitEffect)
        {
        CHuiControlGroup *exitingGroupInEffect = FindControlGroupByFullScreenToEffect();
        if (exitingGroupInEffect == aGroup)
            {
            CHuiRoster& roster = iAlfScreens[0]->iDisplay->Roster();
            CHuiControlGroup* nextToBecomeVisible = NULL;
            // resolve who is under this application in roster
            for (TInt i = 0 ; i < roster.Count() ; i++)
                {
                if (&roster.ControlGroup(i) == exitingGroupInEffect && i > 1)
                    {
                    __ALFFXLOGSTRING("CAlfBridge::ResolveAfterEffectAppearingApplicationL - Found underneath control group");
                    nextToBecomeVisible = &roster.ControlGroup(i-1);
                    break;
                    }
                }
                // resolve, to which application this maps to
            if (nextToBecomeVisible)
                {
                for(TInt j = 0 ; j < iAlfScreens[0]->iControlGroups.Count() - 1 ; j++)
                    {
                    if (iAlfScreens[0]->iControlGroups[j].iControlGroup == nextToBecomeVisible)
                        {
                        iFSFxData->iFromSecureId = iAlfScreens[0]->iControlGroups[j].iSecureId;
                        iFSFxData->iFromWg = iAlfScreens[0]->iControlGroups[j].iClientWindowGroupId;
                        __ALFFXLOGSTRING2("CAlfBridge::ResolveAfterEffectAppearingApplicationL - Found underneath application uid: 0x%x, window group: %d -  Start track drawing for exit effect.", iFSFxData->iFromSecureId, iFSFxData->iFromWg );
                        break;
                        }
                    }
                }
            }
        }
#endif    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::DeleteControlGroupL(TInt aWindowGroupNodeId, TInt aScreenNumber )
    {
    for (TInt i=0; i<iAlfScreens[aScreenNumber]->iControlGroups.Count();i++)
        {
        if (iAlfScreens[aScreenNumber]->iControlGroups[i].iWindowGroupNodeId == aWindowGroupNodeId)
            {
            if (iAlfScreens[aScreenNumber]->iDisplay)
                {
                RemoveWindowGroupAsAlfApp(iAlfScreens[aScreenNumber]->iControlGroups[i].iClientWindowGroupId);

                CHuiControlGroup* controlGroup = iAlfScreens[aScreenNumber]->iControlGroups[i].iControlGroup;
                CHuiControl& control = controlGroup->Control(0);
                CHuiLayout* layout = (CHuiLayout*)&control.Visual(0);
                ResolveAfterEffectAppearingApplicationL(controlGroup);
                // wserv has notifed that this control group and its layout should be destroyed. However, we might be
                // have effect on the layout itself or layout is being drawn as external content. This indicates that
                // we should not delete the control group at this point, but add it to iDeadControlGroup list, which 
                // is cleared when effect has finished.
                RPointerArray<CHuiLayout> familyTree;
                ListFamilyTreeL(familyTree, layout); // recursively dig the family tree
                TBool anyVisualHasEffect(EFalse);
                TInt familyIndex(0);
                TInt familySize = familyTree.Count();
                TInt activeEffectIndex = KErrNotFound; 
                while(familyIndex < familySize && !anyVisualHasEffect)
                    {
                    anyVisualHasEffect = HasActiveEffect(familyTree[familyIndex++], activeEffectIndex); 
                    
                    }
                
                if (anyVisualHasEffect)
                    {
                    TInt activeEffectHandle = iEffectCleanupStack[activeEffectIndex].iHandle;
                    if (iFSFXDataPart2 && iFSFXDataPart2->iHandle == activeEffectHandle)
                        {
                        __ALFFXLOGSTRING1("CAlfBridge::DeleteControlGroupL - WARNING! Control group with 2 phase effect is being destroyed. Canceling second phase for handle %d", activeEffectHandle);
                        delete iFSFXDataPart2;
                        iFSFXDataPart2 = NULL;
                        }
                    
                    __ALFFXLOGSTRING1("Layout 0x%x has external content", layout);
                    // EHuiVisualFlagShouldDestroy destroy flag should have come for the windows in this layout already
                    layout->SetFlags(EHuiVisualFlagShouldDestroy);
                    // move this layout to effect control group, where it can still be shown. this control group may be deleted.
                    CHuiControl& control = iAlfScreens[aScreenNumber]->iControlGroups[i].iControlGroup->Control(0);
                    CHuiLayout* layout = (CHuiLayout*)&control.Visual(0);
                    
                    for(TInt familyIndex = 0; familyIndex < familyTree.Count();familyIndex++)
                        {
                        control.Remove(familyTree[familyIndex]); // remove ownership from original group
                        }
                        
                    control.Remove(layout); // remove ownership from original group
                    // move visuals to safe place. the original group must be destroyed.
                    __ALFFXLOGSTRING1("MOVING 0x%x to effect group", layout);
                    CHuiControl& effectControlGroup = iAlfScreens[0]->iFullscreenEffectControlGroup->Control(0);
                    CHuiLayout* effectControlGroupLayout = (CHuiLayout*) &effectControlGroup.Visual(0);
                    // Transfers ownership of visuals. 
                    effectControlGroup.AppendL(layout, effectControlGroupLayout); // this will remove it from the previous layout
                    layout->SetOwner(effectControlGroup);
                    
                    for(TInt familyIndex = familyTree.Count() - 1; familyIndex >= 0; --familyIndex)
                       {
                       familyTree[familyIndex]->SetOwner(effectControlGroup);                      
                       
                       if (!HasActiveEffect(familyTree[familyIndex]))
                           {
                           TInt dummy;
                           AddFxItemL(activeEffectHandle, familyTree[familyIndex], NULL, NULL, EFalse, dummy, ETrue, EFalse);
                           }
                       }
                    }
                else
                    {
                    // check once more, that the windows in this group are not having effects. 
                    // the layout is not have effect, but some child window might.
                    // in this case, the child window effects WILL BE REMOVED.
                     for(TInt familyIndex = 0; familyIndex < familyTree.Count();familyIndex++)
                         {
                         CHuiLayout* removedVisual = familyTree[familyIndex];
                         CleanFxVisual(removedVisual);
          
                         if ( removedVisual != layout ) // let control group delete the layout
					         {
					         control.Remove(removedVisual); // remove ownership from the original control (group)
					         __ALFFXLOGSTRING1("CAlfBridge::DeleteControlGroupL - adding to orphonage 0x%x", layout);
					         CleanFxVisual(removedVisual);
					         if ( removedVisual->Layout() == layout ) 
					             {
					             iOrphanStorage->AppendL( removedVisual );
					             } 
					         else 
					             {
                                 removedVisual->SetOwner(*iOrphanStorage);
					             }
                             }
                         }
                    }
                familyTree.Close();
                iAlfScreens[aScreenNumber]->iDisplay->Roster().Hide(*iAlfScreens[aScreenNumber]->iControlGroups[i].iControlGroup);
                __ALFFXLOGSTRING("CAlfBridge::DeleteControlGroupL - Deleting group");
                iAlfScreens[aScreenNumber]->iControlGroups.Remove(i);
                iHuiEnv->DeleteControlGroup(aWindowGroupNodeId);
                __ALFFXLOGSTRING("CAlfBridge::DeleteControlGroupL - Deleting group done");
                }
            
            AMT_DEC_COUNTER( iTotalControlGroupCount );            
            
            break;
            }
        }
    
    AMT_MAP_INC_VALUE( iIntMap, aWindowGroupNodeId,
                       EAlfModuleTestTypeBridgeReleaseWindowGroup );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
CHuiControlGroup& CAlfBridge::CreateControlGroupL(
            TInt aWindowGroupNodeId, 
            TInt aClientWindowGroupId,
            TInt aSecureId,
            TInt aScreenNumber )
    {
    CHuiControlGroup* group = FindControlGroup(aWindowGroupNodeId,aScreenNumber);
    if (!group)
        {
        group = &iHuiEnv->NewControlGroupL(aWindowGroupNodeId); 
        group->SetSecureId(aSecureId);
        CleanupStack::PushL(group);
        CHuiControl*  cntrl = new (ELeave) CHuiControl(*iHuiEnv);
        CleanupStack::PushL(cntrl);
        CHuiLayout* layout = NULL;
        
        cntrl->ConstructL();
        group->AppendL(cntrl);
        CleanupStack::Pop(cntrl);
        cntrl->SetRole(EHuiWindowGroupContainer);

        layout = CHuiCanvasVisual::AddNewL(*cntrl);
        layout->SetTagL(KAlfWindowGroupContainerControlTag);
        layout->ClearFlag(EHuiVisualFlagClipping); // No need to clip, should be faster without clipping 
        // IsVisualOpaque should return true if there is no effect. 
        // So that's why opaque flag is set to this layout.
        layout->SetFlag(EHuiVisualFlagOpaqueHint); 
        
        for( TInt i = 0 ; i < iAlfNativeClientsWgIds.Count() ; i++ )
            {
            if(iAlfNativeClientsWgIds[i] == aClientWindowGroupId)
                {
                group->iAlfApp = ETrue;
                break;
                }
            }
       
        TAlfControlGroupEntry entry;
        entry.iControlGroup = group;
        entry.iWindowGroupNodeId = aWindowGroupNodeId;
        entry.iClientWindowGroupId = aClientWindowGroupId;  
        entry.iSecureId = aSecureId;
        // we have received start effect for this group, but the group did not exist in alf universe at the time. hide the group.
        if (iFSFxData
                && (iFSFxData->State() == EWaitingWindowGroup || iFSFxData->State() == EEndFullScreenReceivedWaitingWindowGroup)
                && iFSFxData->iToSecureId == aSecureId)
            {
            iFSFxData->iToWg = aClientWindowGroupId;
            if (iFSFXDataPart2)
                {
                iFSFXDataPart2->iToWg = aClientWindowGroupId;
                }
            CHuiControlGroup* fromGroup = NULL;
            CHuiLayout* fromLayout = NULL; 
            fromGroup = FindControlGroupByFullScreenFromEffect();
            if (fromGroup)
                {
                CHuiControl& control2 = fromGroup->Control(0);
                fromLayout = (CHuiLayout*)&control2.Visual(0);
                }
            
            
            // First HandleGfxEvent, then clear iWaitingWindowGroup.
            __ALFFXLOGSTRING1("CAlfBridge::CreateControlGroupL - try triggering effect 0x%x", layout);
            TBool failed = HandleGfxEventL( *iFSFxData, layout, fromLayout );
            if ( iFSFxData->State() == EWaitingWindowGroup)
                {
                iFSFxData->SetState(EWaitEndFullScreen);
                }
            else
                {  
				// was CFullScreenEffectState::EEndFullScreenReceivedWaitingWindowGroup           
                iFSFxData->SetState(EEndFullscreenReceived);
                }
            
            if ( failed )
                {
                // Effect failed, reset state
                __ALFFXLOGSTRING1("CAlfBridge::CreateControlGroupL - effect failed on layout 0x%x", layout);
                HandleGfxStopEvent( EFalse ); // destroys iFSFxData and iFSFXDataPart2
                }
            }     
        entry.iScreenNumber = aScreenNumber;
//        entry.iRole = EHuiWindowGroupContainer;

        iAlfScreens[aScreenNumber]->iControlGroups.Append(entry);
        CleanupStack::Pop(group);                

        if (iAlfScreens[aScreenNumber]->iDisplay)
            ShowControlGroupL(iAlfScreens[aScreenNumber]->iDisplay->Roster(), *group, KHuiRosterShowAtTop, aScreenNumber); 
        
        AMT_INC_COUNTER( iTotalControlGroupCount );
        }

    AMT_MAP_BRIDGE_CREATE_CONTROL_GROUP();

    return *group;       
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ShowControlGroupL(CHuiRoster& aRoster, CHuiControlGroup& aGroup, TInt aWhere, TInt aScreenNumber )
    {        
    CAlfScreen* screen = iAlfScreens[aScreenNumber];

#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
        DebugPrintControlGroupOrder(*screen, aRoster, aGroup);
#endif

#ifdef ALF_DEBUG_VISUALIZE_WINDOWGROUP_ORDER
    if (!aGroup.IsTransformed())
        {
        aGroup.EnableTransformationL();
        aGroup.Transformation().Translate(0,0);                    
        aGroup.Transformation().Scale(0.3,0.3);                    
        }
#endif

    TInt last = aRoster.Count() - 1;
    
    
// when an application exits, it must be shown on top util the effect has run its course.
// But this tends to mess up the order of other groups, and application menu softkeys will
// disappear.
    if ( aGroup.Control(0).Visual(0).Effect() )
        {
        if ( aGroup.Control(0).Role() != EHuiWindowGroupContainer )
            {
            // The case where the application control group is deleted by window server
            // has been solved by deleting the tag when window server wants to delete
            // the group. Window server no longer has it, but we keep it alive for a while
            // to show the effect. The group will be deleted when the effect ends.
            aWhere = aRoster.Count() - screen->FixedControlGroupCount();
            }
        }

    if (aGroup.Control(0).Role() == EHuiWindowGroupContainer)      
        {
        // Window group control groups
        ShowWindowGroupControlGroupL(aRoster, aGroup, aWhere, aScreenNumber);
        aGroup.SetAcceptInput(EFalse);
        }
    else if (aGroup.Control(0).Role() == EHuiSessionContainer)      
        {
        // ALF application control groups    
        ShowSessionContainerControlGroupL(aRoster, aGroup, aWhere, aScreenNumber);
        }
    else
        {
        aRoster.ShowL(aGroup, aWhere);
        aGroup.SetAcceptInput(EFalse);
        }                                    

#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER        
        DebugPrintControlGroupOrder(*screen, aRoster, aGroup);
#endif

#ifdef ALF_DEBUG_VISUALIZE_WINDOWGROUP_ORDER
        VisualizeControlGroupOrderL(*screen, aRoster, aGroup);
#endif
    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ShowSessionContainerControlGroupL(CHuiRoster& aRoster, CHuiControlGroup& aGroup, TInt aWhere, TInt aScreenNumber )
    {
    TInt last = aRoster.Count() - 1;

    CAlfScreen* screen = iAlfScreens[aScreenNumber];

    if (aWhere == KHuiRosterShowAtBottom)
        {
        // Most bottom one of alf control groups
        aRoster.ShowL(aGroup, FirstAlfControlGroupIndex(aScreenNumber));    
        }
    else if (aWhere == KHuiRosterShowAtTop)
        {
        // Most top one of alf control groups
        TInt newIndex = LastAlfControlGroupIndex(aScreenNumber);
    
        // check if the aGroup is already in a roster
        for (TInt j=0; j<newIndex;j++)
                { 
                if( &aRoster.ControlGroup(j) == &aGroup )
                    {
                    // adjust position a bit because this is in fact "move" operation
                    // move would move alf event window and place this control group
                    // on top of it if we don't do this adjusment
                    newIndex--;    
                    break;
                    }
                }               
        
        aRoster.ShowL(aGroup, newIndex);    
        }
    else
        {
        TInt index = 0; // Index for ALF group control groups
        TBool added = EFalse;
        TBool move = EFalse; // indicates that controlgroup is already in the roster somewhere below the new index.
        for (TInt i=FirstAlfControlGroupIndex(aScreenNumber); i<last; i++)
            {
            CHuiLayout* hostContainer = aRoster.ControlGroup(i).Control(0).ContainerLayout(NULL);
            if(hostContainer->Flags() & EHuiVisualFlagUnderOpaqueHint)
                {
                // If the visual is inactive, we should skip it so that alf control
                // group ordering would be correct
                continue;
                }
        
            if (index == aWhere)
                {
                if( move )
                    {
                    // adjust the new index because of ShowL call
                    // will first remove the controlgroup from original position
                    index--; 
                    }
                aRoster.ShowL(aGroup, i);        
                added = ETrue;
                break;
                }

            if( &aRoster.ControlGroup(i) == &aGroup )
                {
                move = ETrue;
                }
            
            if (aRoster.ControlGroup(i).Control(0).Role() == EHuiSessionContainer)
                {
                index++;                                                
                }                    
            }
        
        // Too large index was given, just add it to topmost    
        if (!added)
            {
            // Topmost alf group
            ShowSessionContainerControlGroupL( aRoster, aGroup, KHuiRosterShowAtTop, aScreenNumber );
            }        
        }                        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ShowWindowGroupControlGroupL(CHuiRoster& aRoster, CHuiControlGroup& aGroup, TInt aWhere, TInt aScreenNumber )
    {
    CAlfScreen* screen = iAlfScreens[aScreenNumber];

    if (aWhere == KHuiRosterShowAtBottom)
        {
        // Most bottom one of any control groups
        aRoster.ShowL(aGroup, KHuiRosterShowAtBottom);    
        }
    else if (aWhere == KHuiRosterShowAtTop)
        {
        // Topmost
        aRoster.ShowL(aGroup, aRoster.Count() - screen->FixedControlGroupCount());
        }
    else
        {
        TInt index = 0; // Index for Window group control groups
        TBool added = EFalse;
        for (TInt i=0; i<aRoster.Count() - screen->FixedControlGroupCount(); i++)
                   {
                   if (index == aWhere)
                       {
                       TBool lSyncAlfAppAndAlfEventGroup = EFalse;
                       if ( i>0 &&  aRoster.ControlGroup(i).iAlfApp && aRoster.ControlGroup(i-1).ResourceId() == iAlfWindowGroupNodeId  && &aRoster.ControlGroup(i) != &aGroup)
                           {
                           // we still need to check that there are no other alf client window groups on top.
                           // if one native alf application is embedding another native alf application, WServ
                           // seems to update wg chains so that the following check must be done  
                           TInt u = i+1;
                           TBool clienWgFoundOntop =EFalse;
                           for(;u < aRoster.Count() - screen->FixedControlGroupCount(); u++)
                               {
                               if(aRoster.ControlGroup(u).iAlfApp)
                                   {
                                   clienWgFoundOntop = ETrue;
                                   break;
                                   }
                               }
                           
                           if(!clienWgFoundOntop)
                               {
                               lSyncAlfAppAndAlfEventGroup = ETrue;
                               }
                           }
                       aRoster.ShowL(aGroup, i);
                       added = ETrue;
                       TBool lSyncDone(EFalse);
                       if (lSyncAlfAppAndAlfEventGroup && i< (aRoster.Count()-1) )
                       {
                       lSyncDone = ETrue;
                       CHuiControlGroup &lGroup = aRoster.ControlGroup(i+1);
                       TInt clientWindowGroupId = FindClientWindowGroupId( aScreenNumber, lGroup );
                       iAppUi->AdjustWindowGroupPositionL(clientWindowGroupId,CAlfAppServer::EBehindOfParent);
                       }
                       if(!lSyncDone && aGroup.iAlfApp && aRoster.ControlGroup(i-1).ResourceId() != iAlfWindowGroupNodeId )
                           {
                          
                          CHuiControlGroup &lGroup = aRoster.ControlGroup(i);
                          TInt clientWindowGroupId = FindClientWindowGroupId( aScreenNumber, lGroup );
                          iAppUi->AdjustWindowGroupPositionL(clientWindowGroupId,CAlfAppServer::EBehindOfParent);
                           }
                       break;
                       }

            if (aRoster.ControlGroup(i).Control(0).Role() == EHuiWindowGroupContainer &&
                &aRoster.ControlGroup(i) != &aGroup)
                {
                index++;                                                
                }                    
            }
        
        // Too large index was given, just add it to topmost    
        if (!added)
            {
            // Topmost
            for (TInt i=aRoster.Count() - screen->FixedControlGroupCount(); i >= 0; i--)
                {
                if (aRoster.ControlGroup(i).Control(0).Role() == EHuiWindowGroupContainer)
                    {
                    // increasing the count, assuming that WindowControlGroup is not in the Roster
                    i++;
                    aRoster.ShowL(aGroup, i); 
                    break;
                    }                    
                }                                                             
            }
        }                                            
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::SetAlfWindowGroupId(TInt aAlfWindowGroupId)
    {
    iAlfWindowGroupId = aAlfWindowGroupId;
    // check if hithcock window group was already there
    TInt secureId = RThread().SecureId(); 
    if (iAlfScreens.Count())
        {
        for ( TInt i = 0; i < iAlfScreens[0]->iControlGroups.Count(); i++ )
            {
            if ( iAlfScreens[0]->iControlGroups[i].iSecureId == secureId && 
                 iAlfScreens[0]->iControlGroups[i].iClientWindowGroupId != CHuiStatic::RootWin(0)->Identifier())
                {
                iAlfWindowGroupNodeId = iAlfScreens[0]->iControlGroups[i].iWindowGroupNodeId;
                
                CHuiControlGroup* controlGroup = FindControlGroup(iAlfWindowGroupNodeId, 0);
                CHuiControl& control = controlGroup->Control(0);
                CHuiVisual* layout = &control.Visual(0);
                iAlfScreens[0]->iDisplay->Roster().SetAlfEventWindow(layout);
                return;
                }
            }        
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
CHuiDisplay* CAlfBridge::Display(TInt aScreenNum)
    {
    for ( TInt i = 0 ; i < iAlfScreens.Count() ; i++ )
        {
        if ( iAlfScreens[i]->iScreenNum == aScreenNum )
            {
             return iAlfScreens[i]->iDisplay;
            }
        }
    return NULL;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ReorderAlfControlGroupsL( TInt aScreenNumber )
    {
    if (iAlfScreens[aScreenNumber]->iDisplay)
        {
        RPointerArray<CHuiControlGroup> controlGroupOrder;
        CHuiRoster& roster = iAlfScreens[aScreenNumber]->iDisplay->Roster();
        for (TInt j=0; j<roster.Count();j++)
            {
            CHuiControlGroup& controlGroup = roster.ControlGroup(j);
            if (controlGroup.Control(0).Role() == EHuiSessionContainer)
                {
                controlGroupOrder.Append(&controlGroup);
                roster.Hide(controlGroup);
                j--; // roster.Hide does remove controlGroup from the roster 
                }
            }
            
        for (TInt k=0;k<controlGroupOrder.Count();k++)
            {
            ShowControlGroupL(roster, *controlGroupOrder[k], KHuiRosterShowAtTop, aScreenNumber);    
            }                                                        
        
        controlGroupOrder.Close();
        }
    }
        

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TInt CAlfBridge::FirstAlfControlGroupIndex( TInt aScreenNumber )
    {
    if (iAlfScreens[aScreenNumber]->iDisplay)
        {
        CHuiRoster& roster = iAlfScreens[aScreenNumber]->iDisplay->Roster();
        for (TInt j=0; j<roster.Count();j++)
            {
            if( roster.ControlGroup(j).Control(0).Role() == EHuiSessionContainer 
                || roster.ControlGroup(j).ResourceId() == iAlfWindowGroupNodeId )
                {
                return j; // Alf groups positioned just above alf servers window group   
                }
            }                                        
        }
    return 0; // Not found      
    }
        

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TInt CAlfBridge::LastAlfControlGroupIndex( TInt aScreenNumber )
    {
    if (iAlfScreens[aScreenNumber]->iDisplay)
        {                
        CHuiRoster& roster = iAlfScreens[aScreenNumber]->iDisplay->Roster();
        for (TInt j=0; j<roster.Count();j++)
            { // last is always a index of pointer event window position
            if( roster.ControlGroup(j).ResourceId() == iAlfWindowGroupNodeId )
                {
                return j; // Alf groups positioned just above alf servers window group   
                }
            }                
        }
    return 0;    
    }
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TInt CAlfBridge::ResolveScreenNumber( CHuiDisplay& aDisplay )
        {
        for ( TInt i = 0 ; i < iAlfScreens.Count() ; i++ )
            {
            if ( iAlfScreens[i]->iDisplay == &aDisplay )
                {
                return iAlfScreens[i]->iScreenNum;
                }
            }
        return KErrNotFound;
        }
    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::NotifyDisplayRefreshStarted(CHuiDisplay& aDisplay)
    {
    // Look for the correct display
    TInt screenNumber = ResolveScreenNumber(aDisplay);
    CleanFxVisuals();
    if ( screenNumber != KErrNotFound )
        {
        // FPS Counter with hitchcock drawing
#ifdef SYMBIAN_BUILD_GCE
        if(iPrintFPS)
            {
            TReal fps = CHuiStatic::FrameRate();
            if(fps > 0)
                {
                TBuf<8> numBuf;
                numBuf.AppendNum(fps, TRealFormat(5,1));
                TRAP_IGNORE(
                        {
                        iAlfScreens[screenNumber]->iFPSText->SetTextL( numBuf );
                        })
                }
            }
#endif
        if (!iActivated)            
            {
            TInt err = iBridgerClient.Connect();
            iActivated = (err == KErrNone);            
            }
                    
        if (iAlfScreens[screenNumber]->IsVisualTreeVisibilityChanged())
            {
            HandleVisualVisibility( screenNumber );    
            iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(EFalse);
            }
        }
    }

// ---------------------------------------------------------------------------
// Check if there is effect and it's not fade effect.
// ---------------------------------------------------------------------------
// 
static TBool IsNonFadeEffect(CHuiFxEffect* aEffect)
    {
    return aEffect && !(aEffect->EffectFlags() & KHuiFadeEffectFlag);
    }

// ---------------------------------------------------------------------------
// Check if effect has been flagged as opaque
// ---------------------------------------------------------------------------
// 
static TBool IsOpaqueEffect(CHuiFxEffect* aEffect)
    {
    return aEffect && (aEffect->EffectFlags() & KHuiFxOpaqueHint);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TBool CAlfBridge::IsVisualOpaque(CHuiVisual& aVisual)
    {
    // Effect with opaque hint overrides everything else.
    if (IsOpaqueEffect( aVisual.Effect() ))
        {
        return ETrue; // Opaque
        }    

    TBool transparent = EFalse;
    
    // TODO: We should check transformation too and perhaps parent transformations as well ?
    //transparent |= aVisual.IsTransformed(); 

    // if the control has an attached effect which is transformed, it must be considered transparent
    // as it may not cover the full screen
    if ( IsNonFadeEffect( aVisual.Effect() ) )
        {
        return EFalse;
        }
    
    // Note: The long startup effect uses opaque effect to hide the background. 
    if ( aVisual.Layout() 
            && (IsNonFadeEffect( aVisual.Layout()->Effect() ) 
			&& !IsOpaqueEffect(aVisual.Layout()->Effect() ))
            )
        {
        return EFalse;
        }

    if (aVisual.Flags() & EHuiVisualFlagDrawOnlyAsExternalContent)
        {
        return EFalse; // not transparent
        }

    transparent |= (!((aVisual.Flags() & EHuiVisualFlagOpaqueHint) == EHuiVisualFlagOpaqueHint));
    transparent |= (aVisual.iOpacity.Now() < KAlfVisualDefaultOpacity);
    transparent |= (aVisual.iOpacity.Target() < KAlfVisualDefaultOpacity);
    
    return  !transparent;            
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ClipVisualRect(TRect& aRect, const TRect& aClippingRect)
    {    
    if (aRect.Intersects(aClippingRect))
        {
        // Clip to visible area, there are windows larger than screen for some reason.                
        aRect.Intersection(aClippingRect);     
        }
    else
        {
        // no intersection with the clipping rect -> outside of the screen -> not visible
        aRect = TRect(0,0,0,0);
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TBool CAlfBridge::IsRectCoveredByRegion(TRect aRect, TRegion& aCoveringRegion)
    {
    TBool isCovered = EFalse;

    // Zero sized rects are considered as covered (although it is a bit philosphical question)
    if (aRect.Size() == TSize(0,0))
        {
        isCovered = ETrue;    
        }
            
    // First check if it is covered by one of the rects in the covering region...
    if (!isCovered)
        {            
        TPoint topLeft = aRect.iTl;
        TPoint bottomRight = aRect.iBr;
        bottomRight.iX--;
        bottomRight.iY--;

        for (TInt i=0; i < aCoveringRegion.Count(); i++)
            {
            if (aCoveringRegion[i].Contains(topLeft) && 
                aCoveringRegion[i].Contains(bottomRight))
                {
                isCovered = ETrue;
                }                                    
            }
        }        

    // ...it may still cover it with a combination of several rects
    if (!isCovered)
        {            
        iTempVisualRegion.Clear();
        iTempIntersectingRegion.Clear();

        iTempVisualRegion.AddRect(aRect);
        
        iTempIntersectingRegion.Intersection(aCoveringRegion, iTempVisualRegion);
        iTempIntersectingRegion.Tidy();
        
        if (iTempIntersectingRegion.Count() == 1)
            {
            if (iTempIntersectingRegion[0] == aRect)
                {
                isCovered = ETrue;    
                }
            }        
        }
    
    return isCovered;
    }



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleVisualVisibility( TInt aScreenNumber )
    {
    if (!iAlfScreens.Count())
        return;
            
    iTempRegion.Clear();
    iTempRegion2.Clear();
    
    CAlfScreen* screen = iAlfScreens[aScreenNumber];
    TRect fullscreen = TRect(TPoint(0,0), screen->Size());
    TBool checkFadeEffectInScreen = EFalse;
    
    
    // Prepare SW render target (if needed)
    if (iSwRenderingEnabled)
        {
        TBool modified = PrepareSwRenderingTarget( screen );
        
        if ( modified )
            {
            // To avoid debug panic, we need to reset foreground bitmap.
            TRAP_IGNORE( screen->iDisplay->SetForegroundBitmapL( 
                screen->iSwRenderingTarget ) );
            }
        }
    
    TBool fullscreenCovered = EFalse;
    CAlfAppSrvSessionBase* topMostAlfSrvSession = NULL;
    
    iBgSurfaceFound = EFalse;
    iHomeScreenWallpaperWindowFound = EFalse;
    iHomeScreenVisible = EFalse;
   
    // Check if effect group has an effect with opaque hint.
    CHuiControlGroup& fxcontrolgroup = *(iAlfScreens[aScreenNumber]->iFullscreenEffectControlGroup);
    CHuiControl& fxcontrol = fxcontrolgroup.Control(0);
    CHuiCanvasVisual* fxlayout = (CHuiCanvasVisual*)&fxcontrol.Visual(0);
    CHuiVisual* fxExternalContent = fxlayout->ExternalContent();
    
    if (fxlayout && IsOpaqueEffect(fxlayout->Effect()))
        {
        fullscreenCovered = ETrue;    
        }
    else if (fxExternalContent && IsOpaqueEffect(fxExternalContent->Effect()))
        {
        fullscreenCovered = ETrue;
        }    
    
	
	TBool alfWindowGroupFoundVisible = EFalse;
	TBool alfClientWindowGroupVisible = EFalse;
	
    AMT_SET_VALUE( iVisibleVisualCount, 0 );
	
    // skip the topmost (effect) layer, start from floating sprite group
    for (TInt j=screen->iDisplay->Roster().Count() - screen->FixedControlGroupCount(); j>=0; j--)
        {                
#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
        activevisualcount = 0;
        passivevisualcount = 0;
#endif

        CHuiControlGroup& controlgroup = iAlfScreens[aScreenNumber]->iDisplay->Roster().ControlGroup(j);
        CHuiControl& control = controlgroup.Control(0);

        if (control.Role() == EHuiFpsIndicatorContainer)
            {
            // FPS container doesn't contain canvas visuals
            continue;
            }

        CHuiCanvasVisual* layout = (CHuiCanvasVisual*)&control.Visual(0);
#ifdef HUI_DEBUG_TRACK_DRAWING	
        if ( layout->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleVisualVisibility: tracked visual 0x%x"), canvasVisual);
            }
#endif            
            
        
        // For optimization reasons, check if all visuals below in Z-order are covered    
        if (!fullscreenCovered)
            {
            fullscreenCovered = IsRectCoveredByRegion(fullscreen, iTempRegion);            
#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
            if (fullscreenCovered)
                __ALFLOGSTRING("Full screen covered!");                    
#endif
            }
        
        
        // Dont mess with alf control group visuals, alf session handling does it for us
        if (control.Role() == EHuiSessionContainer)
            {
            CHuiLayout* hostContainer = control.ContainerLayout( NULL );
            TInt flags = hostContainer->Flags();   
            CAlfAppSrvSessionBase* activeSession = NULL;
            if (iAppUi)
                {
                activeSession = iAppUi->ActiveSession();
                }
            // !fullscreenCovered need to be checked because for fullscreen
            // alf applications alf event window group could be on top of alf client windowgroup
            if ( (alfClientWindowGroupVisible || !fullscreenCovered)  && activeSession)
                {
                // clear inactive flag if client has not made this controlgroup hidden
                if(!(flags&EHuiVisualFlagUnderOpaqueHint))
                    {
                    if(iAppUi)
                        {
                        topMostAlfSrvSession = iAppUi->SrvSessionForControlGroup(controlgroup);
                        
                        TInt clientWindowGroupId = topMostAlfSrvSession->ClientWindowGroup();
                        __ALFLOGSTRING1("Alf Application: clientWindowGroupId %d", clientWindowGroupId);
                        // checking the case if there are multiple alf application openend.
                        if (clientWindowGroupId == iAppUi->GetLastActiveClient())
                            {
                            __ALFLOGSTRING1("Alf Application: GetLastActiveClient %d", iAppUi->GetLastActiveClient());
                            alfWindowGroupFoundVisible = ETrue;
                            hostContainer->ClearFlags(EHuiVisualFlagInactive);
                            // just add the rect to covered region because alf draws solid background
                            iTempRegion.AddRect(fullscreen);
                            iTempRegion.Tidy();
                            fullscreenCovered = ETrue;
                            alfClientWindowGroupVisible = EFalse;  // change flag so that we don't go in this branch again
                            }
                        }
                    }
                else // else put as inactive
                    {
                    hostContainer->SetFlags(EHuiVisualFlagInactive);
                    }
                }
            else if(!alfWindowGroupFoundVisible || flags&EHuiVisualFlagUnderOpaqueHint)
                {
                // this alf session control group should not be visible anymore because avkon application is on top
                // -> change to inactive
                hostContainer->SetFlags(EHuiVisualFlagInactive);
                }
            else if(!(flags&EHuiVisualFlagUnderOpaqueHint)) // alf app covers full screen
                {
                // check if current controlgroup belongs to same srvsession as the topmost visible alf session container
                CAlfAppSrvSessionBase* alfsrvsession = NULL;
                if(iAppUi)
                    {
                    alfsrvsession = iAppUi->SrvSessionForControlGroup(controlgroup);
                    }
                if(topMostAlfSrvSession!=NULL && topMostAlfSrvSession == alfsrvsession)
                    {
                    hostContainer->ClearFlags(EHuiVisualFlagInactive);
                    }
                else
                    {
                    hostContainer->SetFlags(EHuiVisualFlagInactive);
                    }
                }
            continue;
            }



        
        if (!fullscreenCovered)
            {
            fullscreenCovered = screen->iDisplay->Roster().IsVisibleContentFrozen();
#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
            if (fullscreenCovered)
                __ALFLOGSTRING("Full screen covered because of frozen roster content");                    
#endif
            }
        
        TBool subTreeCovered = EFalse;
        TBool hasLayers = EFalse;
        
        TBool hasActiveVisualsInVisualTree(EFalse);
        
        // native alf application assumes that it should cover whole screen with alfcontent
        // it makes alfeventwindowgroup as inactive and fading is not done on alf content
        // this call is exculsive for alfeventwindowgroup 
        if (alfClientWindowGroupVisible && (controlgroup.ResourceId() == iAlfWindowGroupNodeId))
            {
            hasActiveVisualsInVisualTree = 
               HandleLayoutVisualVisibility( layout, controlgroup, control, 
                   hasActiveVisualsInVisualTree, fullscreen, screen, 
                   subTreeCovered, hasLayers, IsVisualOpaque(*layout),alfClientWindowGroupVisible );
            }
        else
            {
            hasActiveVisualsInVisualTree = 
               HandleLayoutVisualVisibility( layout, controlgroup, control, 
                   fullscreenCovered, fullscreen, screen, 
                   subTreeCovered, hasLayers, IsVisualOpaque(*layout) );
            }
        

        TBool hasFadeEffectsInVisualTree = (layout->CanvasFlags() & EHuiCanvasFlagExternalFadeExistsInsideVisualTree);        

        // if native alf app is found visible and the client it self is not covering whole screen, we can assume that alf content
        // fills the rest. 
        // this is for embedded native alf application cases. Otherwise chained window groups tend to
        // flicker from time to time
        // also if alfcontent is not hided below avkon content the fading effect would reveal alfconent
        if(controlgroup.iAlfApp)
            {
            fullscreenCovered = IsRectCoveredByRegion(fullscreen, iTempRegion); 
            if(!fullscreenCovered )
                {
                iTempRegion.AddRect(fullscreen);
                iTempRegion.Tidy();
                fullscreenCovered = ETrue;
                alfClientWindowGroupVisible = ETrue;
                }
            }
        
        // If root visuals effect is marked as opaque, then add whole screen area as covered.
        if (!fullscreenCovered)
            {
            fullscreenCovered = IsOpaqueEffect(layout->Effect());
            }                    
        
        if ( hasLayers && iActivated )
            {
            HandleLayerVisibility( layout, controlgroup, control, hasActiveVisualsInVisualTree );
            }
        
        TBool isLayoutActive = !(layout->Flags() & EHuiVisualFlagInactive);        
        
        // we need to set /clear inactive flags before we evaluate the fade effect,
        // otherwise the fade effect will be removed without a reason in
        // some use cases
        if (isLayoutActive && !hasActiveVisualsInVisualTree && !IsOpaqueEffect(layout->Effect()))
            {
            // Setting also the root visual (layout) as inactive, if it had none
			// active children. This is because otherwise the Inactive checks won't
			// work correctly within RosterImpl ScanDirty & ClearChanged phases.
			// If root visual is having an opaque effect, it must remain active
            layout->SetFlag(EHuiVisualFlagInactive);
            }
        else if(!isLayoutActive && (hasActiveVisualsInVisualTree || IsOpaqueEffect(layout->Effect())))
            {
            layout->ClearFlag(EHuiVisualFlagInactive);
            layout->SetPos(fullscreen.iTl);
            layout->SetSize(fullscreen.Size());
            }
 
            
		// If we layout is active setup the fade effects. Also if it is inactive, but has been
		// flagged as containing fade effect, then run the setup as well so that effects which
		// are no more needed get removed.
        if (hasActiveVisualsInVisualTree || (!hasActiveVisualsInVisualTree && hasFadeEffectsInVisualTree))
            {
            checkFadeEffectInScreen = ETrue;
            
            // Prepare fade effects to whole visual tree  below layout
            PrepareFadeEffects( *layout );
    
            // Load needed fade effects (or remove them)
            TBool visualTreeHasFadeEffects = LoadFadeEffectsL( *layout );
            
            // Set flag so that next time we can optimize
            if (visualTreeHasFadeEffects)
                {
                layout->SetCanvasFlags(EHuiCanvasFlagExternalFadeExistsInsideVisualTree);
                }
            else
                {
                layout->ClearCanvasFlags(EHuiCanvasFlagExternalFadeExistsInsideVisualTree);            
                }
            }
               
#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
        __ALFLOGSTRING1(">>>> HandleVisualVisibility: Control group index: %d", j );
        __ALFLOGSTRING1(">>>> HandleVisualVisibility: Active visuals : %d", activevisualcount );
        __ALFLOGSTRING1(">>>> HandleVisualVisibility: Passive visuals: %d", passivevisualcount );
#endif
            
        }
        
    if (iActivated && !iLayoutSwitchInProgress)
        {
        if (iBgSurfaceFound || iHomeScreenWallpaperWindowFound)
            {
            // only bg surface should trigger lowmem
            if (!iInLowMemMode && iBgSurfaceFound)
                {
                SetLowMemory(ETrue);
                iInLowMemMode = ETrue;
                }
            else if (!iBgSurfaceFound && iInLowMemMode)
                {
                SetLowMemory(EFalse);
                iInLowMemMode = EFalse;                
                }

            // if the background animation is not hidden yet, hide it now
            if (!iBgAnimHidden)
                {
                // Notify Bg anim
                iBridgerClient.SendBlind(EAlfQueueRequestBGSessions, TIpcArgs(KAlfCompositionTargetHidden));
                iBgAnimHidden = ETrue;
                }
            }
        else if (iInLowMemMode || iBgAnimHidden)
            {
            // if we did not found bg surface, but are in low memory mode
            // exit it now...
            if (!iBgSurfaceFound && iInLowMemMode)
                {
                SetLowMemory(EFalse);
                iInLowMemMode = EFalse;                
                }

            // if the bg animation is hidden and we did not found a background surface
            // or the homescreen window containing wallpaper, show the background animation
            // again
            if (iBgAnimHidden && !iBgSurfaceFound && !iHomeScreenWallpaperWindowFound)
                {
                // Notify Bg anim
                iBridgerClient.SendBlind(EAlfQueueRequestBGSessions, TIpcArgs(KAlfCompositionTargetVisible));
                iBgAnimHidden = EFalse;
                }
            }
        }
        
    if (iHomeScreenPSValue != iHomeScreenVisible)
        {
        TInt value = iHomeScreenVisible;
        if ((iHomeScreenVisible && iHomeScreenWallpaperWindowFound) || iBgAnimHidden)
            {
            value = EFalse;
            }
        if (value != iHomeScreenPSValue)
            {
            RProperty::Set( KPSSysCategory, KAlfPSKeyHSVisible, value );
            iHomeScreenPSValue = value;
            }
        }
    if ( iSwRenderingEnabled )
        {
        screen->iDisplay->SetForegroundTextureOptions( alfWindowGroupFoundVisible || alfClientWindowGroupVisible );
        }
   
    // Finally, if there are fadeeffects applied to windowgroups, make sure first one does not
    // blend itself, but other windowgroups do blend. Otherwise windowgrouops above others
    // would clear the screen areas where they do not really draw.
    if (checkFadeEffectInScreen)
        {
        TBool firstFadedWindowGroupFound = EFalse;
        for (TInt j=0; j<screen->iDisplay->Roster().Count() - screen->FixedControlGroupCount(); j++) // skip the topmost (effect) layer 
            {                
            CHuiControlGroup& controlgroup = iAlfScreens[aScreenNumber]->iDisplay->Roster().ControlGroup(j);
            CHuiControl& control = controlgroup.Control(0);
            CHuiVisual* layout = &control.Visual(0);
            if (layout->Effect() && (layout->Effect()->EffectFlags() & KHuiFadeEffectFlag))
                {
                if (firstFadedWindowGroupFound)
                    {
                    TInt flags = layout->Effect()->EffectFlags();
                    flags |= KHuiFxAlwaysBlend; // Workaround for opaque layout canvasvisual.
                    layout->Effect()->SetEffectFlags(flags);
                    }
                
                if ((controlgroup.ResourceId() == iAlfWindowGroupNodeId))
                    {
					// no special actions needed currently for alf content.
                    }                
                firstFadedWindowGroupFound = ETrue;
                }
            }
        }
    }

TBool CAlfBridge::HandleLayoutVisualVisibility(
        CHuiLayout* aLayout, 
        CHuiControlGroup& aControlGroup,
        CHuiControl& aControl,
        TBool& aFullscreenCovered, 
        TRect& aFullscreen,
        CAlfScreen* aScreen,
        TBool& aSubtreeVisible, 
        TBool& aHasVisualsWithLayers,
        TBool aChildCanBeOpaque,
        TBool aOnlyForEmbeddedAlfApp)
    {
    TBool visualTreeActive = EFalse;
    TRect visualDisplayRect;
    TBool visualRectIsCovered = EFalse;
    TBool visualIsOpaque = EFalse;
    TBool visualIsActive = EFalse;
    CHuiCanvasVisual* canvasVisual = NULL;

    for (TInt i=aLayout->Count()-1; i >= 0; i--)
        {
        visualDisplayRect = TRect(0,0,0,0);
        visualRectIsCovered = EFalse;
        visualIsOpaque = EFalse;
        visualIsActive = EFalse;
        TBool visualSubtreeVisible = EFalse;
        
        // Check first if visual itself is hidden or does hide other visuals below 
        // in z-order. If it does not hide, then we do not add its displayrect to covering 
        // region.
        canvasVisual = (CHuiCanvasVisual*)(&aLayout->Visual(i));
        
        // Child can be considered to be opaque only if all parents are opaque and
        // visual itself is opaque.
        // For example, parent can have opacity < 1 and that affects children as well.
        // As another example, parent can have scaling transformation.
        visualIsOpaque = aChildCanBeOpaque && IsVisualOpaque(*canvasVisual);
        
        if (!canvasVisual->LayerExtent().IsEmpty())
            {
            aHasVisualsWithLayers = ETrue;
            }        

        if (canvasVisual->Count())
            {
            visualTreeActive |= HandleLayoutVisualVisibility( canvasVisual, aControlGroup, aControl, 
                aFullscreenCovered, aFullscreen, aScreen, 
                visualSubtreeVisible, aHasVisualsWithLayers, visualIsOpaque );
            }
    #ifdef HUI_DEBUG_TRACK_DRAWING  
        if ( canvasVisual->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleVisualVisibility: tracked visual 0x%x"), canvasVisual);
            }
    #endif                
        
        if (visualSubtreeVisible)
            {
            aSubtreeVisible = ETrue;
            }
            
        // Optimization, it is faster to check fullscreen than read visuals displayrect
        if (aFullscreenCovered)
            {
            visualRectIsCovered = ETrue;    
            }
        else
            {
            // Check where visual is            
            visualDisplayRect = canvasVisual->DisplayRect();
    
            // Make sure we clip visual rect to visible screen area
            ClipVisualRect(visualDisplayRect, aLayout->DisplayRect());
            ClipVisualRect(visualDisplayRect, aFullscreen);
            
            
            // Check if this visual is covered by other opaque visuals which rects are in "covered" region
            // it should not check for alf event window group, when we have embedded alf application,
            // because we have assumed that alf app will have full screen covered
            if(!aOnlyForEmbeddedAlfApp)
                {
                visualRectIsCovered = IsRectCoveredByRegion(visualDisplayRect, iTempRegion);
                }
                                
            }
    
    /*            if ( layout->Effect() || canvasVisual->Effect() )
            {
            visualRectIsCovered = EFalse;
            }
    */
        TBool wasInactive = canvasVisual->Flags() & 
            ( EHuiVisualFlagInactive | EHuiVisualFlagUnderOpaqueHint ); 
        
        if (visualRectIsCovered)
            {                                        
            // We clear texture cache here to avoid running out of texture memory  
            if (!(canvasVisual->Flags() & EHuiVisualFlagAlwaysDraw)) // The window has been hidden. However it has exit effect and it must stay active until effect has stopped
                {
                if (visualSubtreeVisible)
                    {
                    canvasVisual->SetFlag(EHuiVisualFlagUnderOpaqueHint);
                    canvasVisual->ClearFlags(EHuiVisualFlagInactive); 
                    }
                else
                    {
                    canvasVisual->SetFlag(EHuiVisualFlagInactive);
                    canvasVisual->ClearFlags(EHuiVisualFlagUnderOpaqueHint); 
                    }
                
                canvasVisual->ClearCache();
                // For SW rendering, disable capturing buffer
                canvasVisual->SetCapturingBufferL(NULL);
                }
            else
                { // this should be drawn, but inactivate when effect is done
                canvasVisual->ClearFlags(
                    EHuiVisualFlagInactive | EHuiVisualFlagUnderOpaqueHint |
                    EHuiVisualFlagShouldBeInactive | EHuiVisualFlagShouldBeUnderOpaqueHint );
                if (visualSubtreeVisible)
                    {
                    canvasVisual->SetFlag(EHuiVisualFlagShouldBeUnderOpaqueHint);
                    }
                else
                    {
                    canvasVisual->SetFlag(EHuiVisualFlagShouldBeInactive);
                    }
                canvasVisual->PrepareCache();
                // For SW rendering, set capturing buffer if it exists)
                canvasVisual->SetCapturingBufferL(aScreen->iSwRenderingTarget);                

                if (wasInactive)
                    {
                    canvasVisual->SetChanged();
                    }
                }
                
            if (!wasInactive)
                {
                aScreen->iDisplay->SetDirty();
                }                

            #ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
                    passivevisualcount++;
            #endif
            }
        else
            {
			aSubtreeVisible = ETrue;
            canvasVisual->ClearFlags( 
                EHuiVisualFlagInactive | EHuiVisualFlagShouldBeInactive |
                EHuiVisualFlagUnderOpaqueHint | EHuiVisualFlagShouldBeUnderOpaqueHint );
            canvasVisual->PrepareCache();
            canvasVisual->SetCapturingBufferL(aScreen->iSwRenderingTarget);                

            // We found an _ACTIVE_ window in homescreens controlgroup
            if (aControlGroup.SecureId() == 0x102750F0)
                {
                iHomeScreenVisible = ETrue;
                // which is fully opaque, and visible ...
                if ( (canvasVisual->iOpacity.Now() > 0.001) && canvasVisual->Flags() & EHuiVisualFlagOpaqueHint)
                    {
                    // ... and it's a fullscreen one, and does not have a transparent clear in it
                    if (canvasVisual->DisplayRect().Round() == Display(0)->VisibleArea() &&
                        !canvasVisual->HasTransParentClear())
                        {
                        // we can "safely" asseme that the window is the
                        // one used to draw the user defined wallpaper
                        // ----> stop background animation as it's not visible
                        iHomeScreenWallpaperWindowFound = ETrue;
                        }
                    }
                }
            
            // we've found non-inactive window which has background surface
            // attached..
            if (canvasVisual->IsBackgroundDrawingEnabled() &&
                canvasVisual->LayerExtent() != TRect() &&
                !canvasVisual->LayerUsesAlphaFlag() )
                {
                // if paintedareacount is exactly one or two, it means that the window
                // has background surface but no drawing commands
                TInt areaCount = canvasVisual->PaintedAreaCount();
                // special handling for camera...
                TBool incamera = aControlGroup.SecureId() == 0x101f857a;
                if (areaCount == 1 || areaCount == 2 || incamera) 
                    {
                    TBool onlyFullScreenAreas = ETrue;
                    for (TInt count = 0; count < areaCount; count++)
                        {
                        TRect area = canvasVisual->PaintedArea(count).iPaintedRect.Round();
                        if (area != Display(0)->VisibleArea())
                            {
                            onlyFullScreenAreas = EFalse;
                            }
                        }
                    // if we found a fullscreen surface with no other drawing commands
                    // we can safely assume that it's about the only thing to be visible
                    // and we can release memory occupied by other parts of the system
                    // 
                    // NOTE: this mechanism keeps the system in low mem state
                    // if the surface is visible, meaning that for example
                    // opening an options menu does not instantly trigger normal
                    // memory state. We want to do it like this as otherwise
                    // we would be triggering for example background animation
                    // on / off quite rapidly........
                    if ( onlyFullScreenAreas || incamera)
                        {
                        // Final test. Surface must not be ALF surface, but some other surface.                        
                        CHuiControlGroup* alfControlGroup = FindControlGroupBySecureId( iAlfSecureId );
                        if (alfControlGroup != &aControlGroup)
                            {                        
                            iBgSurfaceFound = ETrue;
                            }
                        }
                    }
                }

            if (wasInactive)
                {
                canvasVisual->SetChanged();
                }

            #ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
            activevisualcount++;
            #endif
            }


        
        // Finally check the area that this visual covers and add it to covered region
        visualIsActive = !(canvasVisual->Flags() & EHuiVisualFlagInactive);
    
        // Sprites and effects as we consider them always as transparent and also
        // if controlgroup is transformed somehow    
        
        if (aControl.Role() == EHuiFullScreenEffectContainer 
            || aControl.Role() == EHuiWindowFloatingSpriteContainer ||
            aControlGroup.IsTransformed())
            {
            visualIsOpaque = EFalse;    
            }
    
        if (visualIsActive && visualIsOpaque && !visualRectIsCovered)
            { 
    #ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
            if (canvasVisual->PaintedAreaCount())
                __ALFLOGSTRING2("Visual has painted areas: displayrect: iTl: %i, %i", visualDisplayRect.iTl.iX, visualDisplayRect.iTl.iY);                    
                __ALFLOGSTRING2("Visual has painted areas: displayrect: iBr: %i, %i", visualDisplayRect.iBr.iX,visualDisplayRect.iBr.iY);
                #endif
#ifndef ALF_OLD_VISIBILITY
            // New visibility system takes only window shape into account.
            if ( canvasVisual->CanvasFlags() & EHuiCanvasFlagIncludeToVisibilityCalculation )
                {
                if ( !canvasVisual->HasCustomShape() )
                    {
                    TRect displayRect(canvasVisual->DisplayRect());
                    ClipVisualRect(displayRect, aLayout->DisplayRect());
                    ClipVisualRect(displayRect, aFullscreen);

#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
                    __ALFLOGSTRING2("Covered rect: iTl: %i, %i", displayRect.iTl.iX, displayRect.iTl.iY);                    
                    __ALFLOGSTRING2("Covered rect: iBr: %i, %i", displayRect.iBr.iX, displayRect.iBr.iY);
#endif

                    iTempRegion.AddRect(displayRect);
                    iTempRegion.Tidy();
                    }
                else
                    {
                    TRect displayRect = canvasVisual->DisplayRect();
                    const TRegion& region = canvasVisual->ShapeRegion();
                    TPoint delta = -canvasVisual->ShapeOrigin() + displayRect.iTl;
                
                    for (TInt i = 0; i < region.Count(); ++i)
                        {
                        TRect coveredRect(region[i]);
                        coveredRect.Move(delta);
                        ClipVisualRect(coveredRect, aFullscreen);

#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
                        __ALFLOGSTRING2("Covered rect: iTl: %i, %i", coveredRect.iTl.iX, coveredRect.iTl.iY);                    
                        __ALFLOGSTRING2("Covered rect: iBr: %i, %i", coveredRect.iBr.iX, coveredRect.iBr.iY);
#endif
                    
                        iTempRegion.AddRect(coveredRect);
                        iTempRegion.Tidy();
                        }
                    }
                }
#else
            for (TInt k=0; k < canvasVisual->PaintedAreaCount(); k++)
                {                                
                THuiCanvasPaintedArea paintArea = canvasVisual->PaintedArea(k);
                TRect coveredRect = paintArea.iPaintedRect;
                                
                // Clip to visible area, there are windows larger than screen for some reason.                
                ClipVisualRect(coveredRect, aFullscreen);
                
                // Only add to covering region if the painted area is defined as opaque
                if (paintArea.iPaintType == EHuiCanvasPaintTypeOpaque)
                    {
    #ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
                    __ALFLOGSTRING2("Covered rect: iTl: %i, %i", coveredRect.iTl.iX, coveredRect.iTl.iY);                    
                    __ALFLOGSTRING2("Covered rect: iBr: %i, %i", coveredRect.iBr.iX,coveredRect.iBr.iY);
    #endif
                    iTempRegion.AddRect(coveredRect);
                    iTempRegion.Tidy();                                    
                    }
                }
            
            // If effect is marked as opaque, then add whole visual area as covered.
            if (IsOpaqueEffect(canvasVisual->Effect()))
                {
                iTempRegion.AddRect(visualDisplayRect);
                iTempRegion.Tidy();
                }
#endif				
            }                                        
        visualTreeActive |= visualIsActive;
        
        // iVisibleVisualCount is cleared in HandleVisualVisibility()
        AMT_INC_COUNTER_IF( visualIsActive && visualIsOpaque && !visualRectIsCovered, iVisibleVisualCount );
        
        AMT_MAP_SET_VALUE_IF(
                visualIsActive && visualIsOpaque && !visualRectIsCovered && (canvasVisual->CanvasFlags() & EHuiCanvasFlagIncludeToVisibilityCalculation),
                iBoolMap,
                AMT_MAP_CPTR_TO_KEY_CAST( canvasVisual ),
                ETrue,
                EAlfModuleTestTypeBridgeVisualVisibility);
        AMT_MAP_SET_VALUE_IF(
                !visualIsActive || !visualIsOpaque || visualRectIsCovered || !(canvasVisual->CanvasFlags() & EHuiCanvasFlagIncludeToVisibilityCalculation),
                iBoolMap,
                AMT_MAP_CPTR_TO_KEY_CAST( canvasVisual ),
                EFalse,
                EAlfModuleTestTypeBridgeVisualVisibility);
        } // for loop end : children checking loop
     
    return visualTreeActive;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleLayerVisibility(
        CHuiLayout* aLayout,
        CHuiControlGroup& aControlGroup,
        CHuiControl& aControl,
        TBool aVisible )
    {
    CHuiCanvasVisual* canvasVisual = NULL;

    for (TInt i = aLayout->Count()-1; i >= 0; i--)
        {
        canvasVisual = (CHuiCanvasVisual*)(&aLayout->Visual(i));
        
        if (canvasVisual->Count())
            {
            HandleLayerVisibility( 
                canvasVisual, aControlGroup, 
                aControl, aVisible );
            }            

        if (!canvasVisual->LayerExtent().IsEmpty())
            {
            // Determine if surface is visible.
            TBool surfaceWasVisible = ( canvasVisual->CanvasFlags() & EHuiCanvasFlagSurfaceVisible );
            TBool surfaceWasInvisible = ( canvasVisual->CanvasFlags() & EHuiCanvasFlagSurfaceInvisible );
            __ALFLOGSTRING2("CAlfBridge::HandleLayerVisibility aVisible %d surfaceWasVisible %d", aVisible, surfaceWasVisible);

            TBool forcedUpdate = !surfaceWasVisible && !surfaceWasInvisible;

#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS                    
            if ( !aVisible && canvasVisual->LayerUsesAlphaFlag() )
                {
                __ALFLOGSTRING( "CAlfBridge::HandleLayerVisibility LayerUsesAlphaFlag setting visible" ); 
                aVisible = ETrue;
                }
#endif // SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS                    
    
            if ( !aVisible && ( forcedUpdate || surfaceWasVisible ) )
                {
                // notify surface observer that it has been hidden
                __ALFLOGSTRING("CAlfBridge::HideTarget >>");
                TLex8 lex(canvasVisual->Tag());
                TInt windowId(0);
                if (lex.Val(windowId) == KErrNone)
                    {
                    __ALFLOGSTRING1("CAlfBridge::HideTarget: WindowId: %d", windowId);
                    canvasVisual->SetCanvasFlags( EHuiCanvasFlagSurfaceInvisible );
                    canvasVisual->ClearCanvasFlags( EHuiCanvasFlagSurfaceVisible );
                    
                    iBridgerClient.SendBlind(EAlfPostDataToCompositionTarget, TIpcArgs(KAlfCompositionTargetHidden, windowId));                    
                    iBridgerClient.SendBlind(EAlfPostDataToCompositionClient, TIpcArgs(KAlfCompositionTargetHidden, windowId));
                    }
                __ALFLOGSTRING("CAlfBridge::HideTarget <<");
                }
            else if ( aVisible && ( forcedUpdate || surfaceWasInvisible ) )
                {
                // notify surface observer that it has been hidden
                __ALFLOGSTRING("CAlfBridge::ShowTarget >>");
                TLex8 lex(canvasVisual->Tag());
                TInt windowId(0);
                if (lex.Val(windowId) == KErrNone)
                    {
                    __ALFLOGSTRING1("CAlfBridge::ShowTarget: WindowId: %d", windowId);
                    canvasVisual->ClearCanvasFlags( EHuiCanvasFlagSurfaceInvisible );
                    canvasVisual->SetCanvasFlags( EHuiCanvasFlagSurfaceVisible );
                    
                    iBridgerClient.SendBlind(EAlfPostDataToCompositionTarget, TIpcArgs(KAlfCompositionTargetVisible, windowId));                    
                    iBridgerClient.SendBlind(EAlfPostDataToCompositionClient, TIpcArgs(KAlfCompositionTargetVisible, windowId));
                    }
                __ALFLOGSTRING("CAlfBridge::ShowTarget <<");
                }
            else
                {
                // No change - ignore.
                }
            }
        } // for loop end : children checking loop
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ClearCanvasVisualCommandSets(TBool aInactiveOnly)
    {
    if (!iAlfScreens.Count())
        return;
            
    CAlfScreen* screen = iAlfScreens[0]; // TODO  
    for (TInt j=screen->iDisplay->Roster().Count() - screen->FixedControlGroupCount(); j>=0; j--) // skip the topmost (effect) layer 
        {                
        CHuiControlGroup& controlgroup = iAlfScreens[0]->iDisplay->Roster().ControlGroup(j);
        CHuiControl& control = controlgroup.Control(0);

        if ( control.Role() == EHuiSessionContainer ||
             control.Role() == EHuiFpsIndicatorContainer )
            {
            continue;
            }
        
        CHuiVisual* layout = &control.Visual(0);
        for (TInt i=layout->Count()-1; i >= 0; i--)
            {
            CHuiCanvasVisual* canvasVisual = (CHuiCanvasVisual*)(&layout->Visual(i));
            ClearCanvasVisualCommandSetsRecursive(canvasVisual, aInactiveOnly);            
            }
        }
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::ClearCanvasVisualCommandSetsRecursive(CHuiCanvasVisual* aVisual, TBool aInactiveOnly)
    {
    if (!aVisual)
        {
        return;
        }

    if (!aInactiveOnly)
        {
        aVisual->ClearCommandSet();
        }
    else if (aInactiveOnly && (aVisual->Flags() & EHuiVisualFlagInactive))
        {
        aVisual->ClearCommandSet();                        
        }
    else
        {
        // dont clear
        }
        
    for (TInt i=aVisual->Count()-1; i >= 0; --i)
        {
        CHuiCanvasVisual* canvasVisual = (CHuiCanvasVisual*)(&aVisual->Visual(i));
        ClearCanvasVisualCommandSetsRecursive(canvasVisual, aInactiveOnly);            
        }
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::EnableVisualDefaultTransformationsL(CHuiVisual& aVisual)
    {
    aVisual.EnableTransformationL();
    if (!aVisual.Transformation().Count())
        {
        aVisual.Transformation().Rotate(0);
        aVisual.Transformation().Scale(1,1);
        aVisual.Transformation().Translate(0,0);                                                
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleCallback(TInt aStatus)
    {
    TInt err = KErrNone;
    if (aStatus >= 0)
        {
        TRAP(err, DoDispatchL(aStatus))
        }            
    
    if (aStatus < 0 || err)
        {
        __ALFLOGSTRING2("CAlfBridge::HandleCallback status: %d error: %d",aStatus,err);
        
        CEikonEnv* eikenv = CEikonEnv::Static();
        if ( eikenv )
        		{
        		eikenv->HandleError(err?err:aStatus);
            }
        }
    }
        

// ---------------------------------------------------------------------------
// DoDispatchL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::DoDispatchL(TInt aStatus)
    {
    if (*iHost)
        {
        TAlfBridgerData data = (*iHost)->GetData(aStatus);
        // dispatch
        __ALFLOGSTRING1( "CAlfBridge::DoDispatchL: %d",data.iOp );

        switch (data.iOp)
            {
            case EAlfDSSynchronize:
                {
                iIdForEAlfDSSynchronizeOp = data.iInt1;
                if ( iHuiEnv )
                    {
                    iHuiEnv->Synchronize( iIdForEAlfDSSynchronizeOp, this );
                    }
                }
                break;
                
            case EAlfDSCreateNewDisplay:
                {
                AddNewScreenL(NULL);
                break;
                }
            case EAlfDSDestroyDisplay:
                {
                HandleDestroyDisplay( data.iInt1 );
                break;
                }
            case EAlfDSNewWindow:
                {
                HandleNewWindowL( data );
                break;
                }
            case EAlfDSDestroyWindow:
            	{
                HandleDestroyWindowL( data );
            	break;
                }
            case EAlfDSSetWindowPos:
                {
                HandleSetWindowPosL( data );
                break;
                }
            case EAlfDSSetWindowSize:
                {
                HandleSetWindowSizeL( data );
                break;
                }
            case EAlfDSSetWindowRotation:
                {
                HandleSetWindowRotationL( data );
                break;
                }
            case EAlfDSSetWindowOpacity:
                {
                HandleSetWindowOpacityL( data );
                break;
                }
            case EAlfDSSetTransparencyAlphaChannel:
                {
                HandleSetTransparencyAlphaChannelL( data );
                break;
                }
            case EAlfDSIncludeToVisibilityCalculation:
                {
                HandleIncludeToVisibilityCalculationL( data );
                break;
                }
            case EAlfDSSetWindowArea:
                {
                HandleSetWindowAreaL( data );
                break;
                }
            case EAlfDSRefreshCallback:
                {
                iHuiEnv->ContinueRefresh();
                break;
                }
            case EAlfDSReorder:
                {
                HandleReorderWindowL( data );
                break;
                }                    
            case EAlfDSPostCanvasBuffer:
            	{
                HandlePostCanvasBufferL( data );
            	break;
            	}
            case EAlfDSSetWindowActive:
                {
                HandleSetWindowActiveL( data );
                break;
                }
            case EAlfDSSetWindowFlag:
            case EAlfDSClearWindowFlag:
                {
                HandleSetWindowFlagL(data, data.iOp);
                break;
                }
            case EAlfDSSetSurfaceExtent:
                {
                HandleSetSurfaceExtentL( data );
                break;
                }
            case EAlfDsLayerUsesAlphaFlagChanged:
                { 
                HandleLayerUsesAlphaFlagChanged( data );
                break;
                }
            case EAlfDSGetAlfNativeWindowData:
                {
//                HandleGetNativeWindowDataL( data );
                break;
                }
            case EAlfDSGroupChained:
            	{
            	__ALFLOGSTRING("CAlfBridge::DoDispatchL, EAlfDSGroupChained");    
            	// TODO, link groups 
            	break;
            	}
            case EAlfDSGroupChainBroken:
            	{
            	__ALFLOGSTRING("CAlfBridge::DoDispatchL, EAlfDSGroupChainBroken");    
            	// TODO, break link
            	break;
            	}
            case EAlfDSMoveWindowToNewGroup:
            	{
            	HandleMoveWindowToNewGroupL( data );
            	break; 
            	}
            case EAlfEffectFx:
                {
                HandleGfxEffectsL( data );
                break;
                }
            case EAlfStopEffectFx:
                {
                HandleGfxStopEffectsL( data );
                break;
                }
            case EAlfStopControlEffectFx:
                {
                HandleGfxStopControlEffectsL( data );
                break;
                }
            case EAlfControlEffectFx:
                {
                HandleGfxControlEffectsL( data );
                break;
                }
            // TODO: implement these    
            case EAlfRegisterEffectFx:
                {
                HandleRegisterEffectL( data );
                break;
                }
            case EAlfUnregisterEffectFx:
                {
                for (TInt i = 0; i < iAlfRegisteredEffects.Count(); i++)
                    {
                    if (iAlfRegisteredEffects[i].iAction == data.iInt1)
                        {
                        TRegisteredEffectsStruct removed =
                                iAlfRegisteredEffects[i];
                        iAlfRegisteredEffects.Remove(i);
                        CHuiFxEngine* engine = NULL;
                        engine = iHuiEnv->EffectsEngine();
                        if (engine)
                            {
                            engine->UnRegisterEffectL(*removed.iEffectFile);
                            }
                        delete removed.iEffectFile;
						// if this happened to be the fade effect, the cached fade effect filename should
						// be cleared
                        if ( data.iInt1 == KFadeAction)
                            {
                            RegisterFadeEffectL();
                            }
                        break;
                        }
                    }
                break;
                }
            case EAlfUnregisterAllFx:
                {
                while (iAlfRegisteredEffects.Count())
                    {
                    TRegisteredEffectsStruct removed =
                            iAlfRegisteredEffects[0];
                    iAlfRegisteredEffects.Remove(0);
                    CHuiFxEngine* engine = NULL;
                    engine = iHuiEnv->EffectsEngine();
                    if (engine)
                        {
                        engine->UnRegisterEffectL(*removed.iEffectFile);
                        }

                    delete removed.iEffectFile;
                    }
                // fade file might have changed. This will be resolved when next fade effect request arrives.
                RegisterFadeEffectL();
                break;
                }
            case EAlfDSSetCursorData:
                {
                HandleSetCursorDataL( data );
                break;
                }
            case EAlfDSSetFadeEffect:
                {
                HandleSetFadeEffectL( data );
                break;
                }
            case EAlfDSSetNodeTracking:
                {
                HandleSetNodeTracking( data );
                break;
                }
           case EAlfBridgeSetScreenRotation:
                {
                //short cut..  
                __ALFLOGSTRING1("CAlfBridge::DoDispatchL: EAlfBridgeSetScreenRotation: %d",data.iInt1);  
                CHuiGc::TOrientation huiOrientation(CHuiGc::EOrientationNormal);
	            switch(data.iInt1)
	                {
	            case 1: // 90    
                    huiOrientation = CHuiGc::EOrientationCCW90;
                    break;
	            case 2: // 180    
                    huiOrientation = CHuiGc::EOrientation180;
                    break;
	            case 3: // 270    
                    huiOrientation = CHuiGc::EOrientationCW90;
                    break;
                default:
                    break;
                    }
	            if (iAlfScreens[0]->iDisplay->Orientation() != huiOrientation)
	                {
                    HandleSetLayoutSwitchEffectL();
	                iAlfScreens[0]->iDisplay->SetOrientation(huiOrientation);
	                UpdateRootVisualsToFullscreen();
	                if (iAppUi)
	                	{
	                	iAppUi->AdjustWindowGroupPositionL(0,CAlfAppServer::EAlfWindowSize); // hackish, but one way to enforce alf window resizing
	                	}
	                __ALFLOGSTRING1("AlfScreens[0]->iDisplay->SetOrientation: %d",huiOrientation);  	                
	                }
	            
                if (!iLayoutSwitchInProgress) // LayoutSwitchStart wasn't called, so inform that all is fine.
                    {
                    if (iActivated)
                        {
                        iBridgerClient.SendBlind(KAlfCompositionLayoutSwitchComplete, TIpcArgs());
                        }            
                    }
                break;         
                }
           case EAlfEffectFxBeginSyncronizedGroup:
        	   {
        	   CHuiFxEngine* engine = iHuiEnv->EffectsEngine();
        	   if (engine)
        		   {
				   engine->BeginGroupEffect(data.iInt1);
        		   }
        	   break;
        	   }
           case EAlfEffectFxEndSyncronizedGroup:
        	   {
        	   TInt groupId = data.iInt1;
        	   //TBool forced = data.iInt2;
        	   __ALFLOGSTRING1("CAlfBridge::DoDispatchL - Ending for group %d requested.", 
        			   groupId ); 
        	   CHuiFxEngine* engine = iHuiEnv->EffectsEngine();
        	   if (engine)
        		   {
				   engine->StartGroupEffect(groupId);
        		   }
        	   iHuiEnv->ContinueRefresh();
        	   break;
        	   }
           case EAlfDSSetDistractionWindow:
               {
               HandleSetDistractionWindowL( data );
               break;
               }
           case KUnInitialized:
               {
               __ALFLOGSTRING1("CAlfBridge::DoDispatchL: Received KUnInitialized: %d - CRITICAL ERROR!",data.iOp);
               USER_INVARIANT();
               }
            default:
              {
              // Should not happen
              __ALFLOGSTRING1("CAlfBridge::DoDispatchL: Received Unknown op: %d",data.iOp);
              }
            }
        
        AsynchRefresh();
        }
   }

// ---------------------------------------------------------------------------
// HandleDestroyDisplay
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleDestroyDisplay( TInt aScreenNumber )
    {
    iAlfScreens[aScreenNumber]->iDisplay->iRefreshObservers.Remove( *this );
    // TODO: Following delete iAlfScreens[aScreenNumber] crashes when display is trying to delete roster. 
    // RosterEntry is trying to refer to display, which is appareantly alrady destroyed. The roster should have no iRootVisual when display is removed. 
    
    iAlfScreens[aScreenNumber]->iDisplay->Roster().Hide(*(iAlfScreens[aScreenNumber]->iFloatingSpriteControlGroup)); 
    iAlfScreens[aScreenNumber]->iDisplay->Roster().Hide(*(iAlfScreens[aScreenNumber]->iFullscreenEffectControlGroup));
    if ( iAlfScreens[aScreenNumber]->iFpsControlGroup )
        {
        iAlfScreens[aScreenNumber]->iDisplay->Roster().Hide(*(iAlfScreens[aScreenNumber]->iFpsControlGroup));
        }
    delete iAlfScreens[aScreenNumber];
    iAlfScreens.Remove( aScreenNumber );
    }

// ---------------------------------------------------------------------------
// HandleNewWindowL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleNewWindowL( TAlfBridgerData& aData )
	{
	TInt windowGroupNodeId = aData.iInt1;
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber; 
	__ALFLOGSTRING1("CAlfBridge::HandleNewWindowL: new window %d!",windowNodeId);    
	                
	switch (windowAttributes->iWindowNodeType)
		{
		case EAlfWinTreeNodeGroup:
			{
			CHuiControlGroup& controlGroup = CreateControlGroupL(
			        windowGroupNodeId, 
			        windowAttributes->iClientHandle, 
			        windowAttributes->iSecureId,
			        screenNumber);                            

			// We just received notification for our own window group creation, store its node id for doing
			// faster lookups later...
			if (windowAttributes->iClientHandle == iAlfWindowGroupId)
				{
				iAlfWindowGroupNodeId = windowGroupNodeId;    
				}
			break;    
			}
		case EAlfWinTreeNodeClient:
		case EAlfWinTreeNodeRoot: // TODO: Root window receives drawing commands too
		case EAlfWinTreeNodeAnim:
		case EAlfWinTreeNodeSprite:
		case EAlfWinTreeNodeTextCursor:
			{
			CHuiControlGroup* controlGroup = FindControlGroup(windowGroupNodeId,screenNumber);
			// Floating sprites only require own group. Normal sprites should behave as normal visuals.
			if (!controlGroup && windowAttributes->iWindowNodeType == EAlfWinTreeNodeSprite )
				{                            
				controlGroup = iAlfScreens[screenNumber]->iFloatingSpriteControlGroup;
				}

			if (controlGroup)
				{
				CHuiControl& control = controlGroup->Control(0);
				CHuiLayout* layout = (CHuiLayout*)&control.Visual(0);
				CHuiCanvasVisual* viz = NULL;
				CHuiCanvasVisual* parentViz = NULL;             
				parentViz = (CHuiCanvasVisual*)FindVisual(windowAttributes->iParentNodeId);             
				if (parentViz)
				    {
                    viz = CHuiCanvasVisual::AddNewL(control, parentViz);
				    }
				else
				    {
                    viz = CHuiCanvasVisual::AddNewL(control, layout);
				    }

				__ALFFXLOGSTRING3("CAlfBridge::HandleNewWindowL visual: 0x%x, id 0x%x, Owner group Uid: 0x%x", viz, windowNodeId, viz->Owner().ControlGroup()->SecureId());
				AddVisual( 
				    windowNodeId, 
					windowAttributes->iClientHandle, 
					windowAttributes->iClientGroupHandle, 
					viz );

                // check , if a window is having already defined effect.
                if (iControlEffectData 
                        && iControlEffectData->iClientHandle == windowAttributes->iClientHandle
                        && iControlEffectData->iClientGroupHandle == windowAttributes->iClientGroupHandle)
                    {
                    HandleGfxControlEventL(*iControlEffectData, viz);
                    delete iControlEffectData;
                    iControlEffectData = NULL;
                    }
				viz->SetCommandType( CHuiCanvasVisual::ECommandBufferSgc );
				viz->SetFlags(EHuiVisualFlagManualLayout | EHuiVisualFlagInactive);
#ifdef HUI_DEBUG_TRACK_DRAWING
				if ( windowAttributes->iTrackWindow )
				    {
				    viz->SetTracking( windowAttributes->iTrackWindow );
				    }
#endif
				// draw sprites and anims the last inside their windowgroup
				/*if (windowAttributes->iWindowNodeType == EAlfWinTreeNodeSprite || windowAttributes->iWindowNodeType == EAlfWinTreeNodeAnim )
					{
					viz->SetFlag(EHuiVisualFlagDrawAfterOthers);
					}
*/
				// Store identifier / tag to get handle of this visual later
				TBuf8<16> buf;
				buf.AppendNum(windowNodeId);
				viz->SetTagL(buf);                    
				viz->SetPos(windowAttributes->iPosition, windowAttributes->iTransitionTime);
				viz->SetSize(windowAttributes->iSize, windowAttributes->iTransitionTime);

				// Keep window invisible until activated, for now we use opacity for that.
				viz->iOpacity.Set(0, 0);
				
				// Set up parent link for clipping calculations
	            if (parentViz)
	                {
	                viz->SetCanvasFlags(EHuiCanvasFlagEnableCanvasClipping);
	                viz->SetParentCanvas(parentViz);	                
	                }
				}
			else
				{
				__ALFLOGSTRING1("CAlfBridge::HandleNewWindowL: Control group not found for visual 0x%x!",windowNodeId);    
				}    

			break; 
			}
		default:
			{
			__ALFLOGSTRING("CAlfBridge::HandleNewWindowL: Unknown window node type received !");
			}
		}
	iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);
	}

// ---------------------------------------------------------------------------
// HandleDestroyWindow
// ---------------------------------------------------------------------------
// 
void CAlfBridge::DestroyWindow(CHuiVisual* aVisual, TBool aUseForce)
    {
    TInt index;
    TBool visualParticipatingInEffect = HasActiveEffect(aVisual, index);
    
    TBool effectIsExitEffect(ETrue);
    if (index != KErrNotFound)
        {
        effectIsExitEffect = iEffectCleanupStack[index].iHideWhenFinished;
        }
    // effects inside application may need to destroy window to get the new content visible. 
    // Even when there is effect on the layout, destruction of a child window must be allowed.
    if ( !aUseForce && visualParticipatingInEffect && effectIsExitEffect )
        {
        // this visual will be destroyed on the effect call back.
        __ALFFXLOGSTRING1("CAlfBridge::DestroyWindow - not destroying 0x%x", aVisual);
        aVisual->SetFlag(EHuiVisualFlagShouldDestroy);
        // TODO: Revise
        // move to effect controlgroup
        //CHuiControl& effectControlGroup = iAlfScreens[0]->iFullscreenEffectControlGroup->Control(0);
        //CHuiLayout* effectControlGroupLayout = (CHuiLayout*) &effectControlGroup.Visual(0);
        // Transfers ownership of visuals. 
        //effectControlGroup.AppendL(aVisual, effectControlGroupLayout); // this will remove it from the previous layout
        //aVisual->SetOwner(effectControlGroup);
        return;
        }
    __ALFFXLOGSTRING1("CAlfBridge::DestroyWindow 0x%x", aVisual);
    RemoveImageBrushL(*aVisual);
    CHuiLayout* layout = aVisual->Layout();
    if (layout)
        layout->Remove(aVisual);
    aVisual->Owner().Remove(aVisual);
    RPointerArray<CHuiLayout> familyTree;
    ListFamilyTreeL(familyTree, (CHuiLayout*)aVisual); // recursively dig the family tree
    for (TInt familyIndex = 1; familyIndex < familyTree.Count();familyIndex++)
        {
        CHuiLayout* lVisual = familyTree[familyIndex];
        lVisual->Owner().Remove(lVisual);
        iOrphanStorage->AppendL( lVisual );
        CleanFxVisual(lVisual);
        
        __ALFLOGSTRING1("CAlfBridge::DestroyWindow - orphons: %d", iOrphanStorage->VisualCount());
        }
    familyTree.Close();
         
    __ALFLOGSTRING1("CAlfBridge::HandleDestroyWindow - destroying visual 0x%x", aVisual);
    // check if visual is having an effect at the moment. This could occur, if options menu is exited (with effect) and then 
    // application is exited. EHuiVisualFlagDrawOnlyAsExternalContent is indication that
    // there might be effect on this visual. It is not guaranteen.
    
    if (!aUseForce)
        {
        // we can remove from iEffectCleanupStack only when this method was called from HandleDestroyWindow. Otherwise
        // messing iEffectCleanupStack is likely to cause forever loop in some CleanFxVisuals method
        for (TInt i = 0; i < iEffectCleanupStack.Count(); i++)
            {
            TEffectCleanupStruct& effectItem = iEffectCleanupStack[i];
            if (aVisual == effectItem.iEffectedVisual)
                {
                iEffectCleanupStack.Remove(i);
                break;
                }
            }
        }
    delete aVisual;
    }
// ---------------------------------------------------------------------------
// HandleDestroyWindowL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleDestroyWindowL( TAlfBridgerData& aData )
	{
	TInt windowGroupNodeId = aData.iInt1;
	TInt windowNodeId = aData.iInt2;

	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber;

	switch (windowAttributes->iWindowNodeType)
		{
		case EAlfWinTreeNodeGroup:
			{
			HandleVisualVisibility(0);
			DeleteControlGroupL(windowGroupNodeId, screenNumber);                            
			break;    
			}
		case EAlfWinTreeNodeClient:
		case EAlfWinTreeNodeRoot:
		case EAlfWinTreeNodeAnim:
		case EAlfWinTreeNodeSprite:                            
		case EAlfWinTreeNodeFloatingSprite:  
		case EAlfWinTreeNodeTextCursor:    
			{
            // close cursor timer
            if (windowAttributes->iWindowNodeType == EAlfWinTreeNodeTextCursor)
                {
	            iCursorTimer->Cancel();
	            }

			//just remove the visual
			CHuiCanvasVisual* viz;
			
			viz = (CHuiCanvasVisual*)FindVisual(windowNodeId);
			CHuiControlGroup* controlGroup = FindControlGroup( windowGroupNodeId, screenNumber );
		     // Sprite is in its own group, and can be deleted normally.
			if ( !controlGroup  && windowAttributes->iWindowNodeType != EAlfWinTreeNodeSprite )
			    {
			    __ALFLOGSTRING("CAlfBridge::HandleDestroyWindowL: group containing this visual has been destroyed.");
			    // the group containing this visual has been destroyed. 
                if ( viz ) 
				    {
				    DestroyWindow(viz);
					}
			    }
			else
			    {
			    // = (CHuiCanvasVisual*)de(windowNodeId, windowGroupNodeId,screenNumber);
			    if (viz)
			        {
#ifdef HUI_DEBUG_TRACK_DRAWING
        if ( viz->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleDestroyWindowL - Tracked visual"));
            }
#endif
		            DestroyWindow(viz);
			        }
			    else
			        {
			        __ALFLOGSTRING("CAlfBridge::HandleDestroyWindowL: visual not found!");
			        }
			    }
			RemoveVisual( windowNodeId );
			break;
			}                                                     
		default:
			{
			__ALFLOGSTRING("CAlfBridge::HandleDestroyWindowL: Unknown window node type received !");
			break;
			}
		}
	// TODO: Toolkit does not support recycling (of visuals),
	// this is not in line with Nokia environmental policy...
	iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);
	}

// ---------------------------------------------------------------------------
// HandleSetWindowPosL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetWindowPosL( TAlfBridgerData& aData )
	{
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber;
	// fetch visual
	CHuiVisual* viz = FindVisual(windowNodeId);
	if (viz)
		{
#ifdef HUI_DEBUG_TRACK_DRAWING
		if ( viz->Tracking() )
		    {
		    RDebug::Print(_L("CAlfBridge::HandleSetWindowPosL - Tracked visual"));
		    }
#endif

		viz->SetPos(windowAttributes->iPosition, windowAttributes->iTransitionTime);
		  iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue); // TODO: Check if really changed   
		}
	else
		{
		__ALFLOGSTRING("CAlfBridge::HandleSetWindowPosL, EAlfDSSetWindowPos: Visual not found!");    
		}   

    AMT_INC_COUNTER_IF( viz, iVisualPositionChangedCount );
    AMT_SET_VALUE_IF( viz, iLatestVisualExtentRect, TRect( windowAttributes->iPosition, windowAttributes->iSize ) );
    AMT_MAP_SET_VALUE_IF( viz, iPositionMap, windowNodeId,
                          windowAttributes->iPosition, 
                          EAlfModuleTestTypeBridgeChangeWindowPosition );    
	}

// ---------------------------------------------------------------------------
// HandleSetWindowSizeL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetWindowSizeL( TAlfBridgerData& aData )
	{
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber;
	// fetch visual
	CHuiVisual* viz = FindVisual(windowNodeId);
	if (viz)
		{
#ifdef HUI_DEBUG_TRACK_DRAWING
        if ( viz->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleSetWindowSizeL - Tracked visual"));
            }
#endif
		viz->SetSize(windowAttributes->iSize, windowAttributes->iTransitionTime);
		}
	else
		{
		__ALFLOGSTRING("CAlfBridge::HandleSetWindowSizeL, EAlfDSSetWindowSize: Visual not found!");    
		}   
    iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);
    
    AMT_INC_COUNTER_IF(viz, iVisualSizeChangedCount );
    AMT_SET_VALUE_IF(viz, iLatestVisualExtentRect, TRect( windowAttributes->iPosition, windowAttributes->iSize ) );
    
    AMT_MAP_SET_VALUE_IF( viz, iSizeMap, windowNodeId, 
                          windowAttributes->iSize, 
                          EAlfModuleTestTypeBridgeChangeWindowSize );
	}

// ---------------------------------------------------------------------------
// HandleSetWindowRotationL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetWindowRotationL( TAlfBridgerData& aData )
	{
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
	// fetch visual
	CHuiVisual* viz = FindVisual(windowNodeId);
	if (viz)
		{
#ifdef HUI_DEBUG_TRACK_DRAWING
        if ( viz->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleSetWindowRotationL - Tracked visual"));
            }
#endif
		EnableVisualDefaultTransformationsL(*viz);
		viz->Transformation()[KVisualTransformationStepRotate].iParams[0].Set(windowAttributes->iRotation, 
				windowAttributes->iTransitionTime);                                                        
		}
	else
		{
		__ALFLOGSTRING("CAlfBridge::HandleSetWindowRotationL, EAlfDSSetWindowRotation: Visual not found!");                    
		}    
	}

// ---------------------------------------------------------------------------
// HandleSetWindowOpacityL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetWindowOpacityL( TAlfBridgerData& aData )
	{
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber;
	// fetch visual
	CHuiVisual* viz = FindVisual(windowNodeId);
	if (viz)
		{
#ifdef HUI_DEBUG_TRACK_DRAWING
        if ( viz->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleSetWindowOpacityL - Tracked visual"));
            }
#endif
		viz->iOpacity.Set(windowAttributes->iOpacity, windowAttributes->iTransitionTime);
		  iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);  // TODO: Check if really changed  
		}
	else
		{
		__ALFLOGSTRING("CAlfBridge::HandleSetWindowOpacityL, EAlfDSSetWindowOpacity: Visual not found!");                                                
		}  
	}

// ---------------------------------------------------------------------------
// HandleSetTransparencyAlphaChannel
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetTransparencyAlphaChannelL( TAlfBridgerData& aData )
    {
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber;
	// fetch visual
	CHuiVisual* viz = FindVisual(windowNodeId);
	if (viz)
		{
#ifdef HUI_DEBUG_TRACK_DRAWING
        if ( viz->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleSetTransparencyAlphaChannel - Tracked visual"));
            }
#endif
		if ( windowAttributes->iActive )
		    {
		    viz->ClearFlag( EHuiVisualFlagOpaqueHint );
		    }
		else
		    {
		    viz->SetFlag( EHuiVisualFlagOpaqueHint );
		    }
		
	    iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);  // TODO: Check if really changed  
		}
	else
		{
		__ALFLOGSTRING("CAlfBridge::HandleSetTransparencyAlphaChannel, EAlfDSSetTransparencyAlphaChannel: Visual not found!");                                                
		}          
    }

// ---------------------------------------------------------------------------
// HandleSetWindowAreaL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleIncludeToVisibilityCalculationL( TAlfBridgerData& aData )
    {
    TInt windowNodeId = aData.iInt2;
    TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
    TInt screenNumber = windowAttributes->iScreenNumber;
    // fetch visual
    CHuiVisual* viz = FindVisual(windowNodeId);
    if (viz)
        {
#ifdef HUI_DEBUG_TRACK_DRAWING
        if ( viz->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleIncludeToVisibilityCalculationL - Tracked visual"));
            }
#endif
        if ( windowAttributes->iWindowNodeType == EAlfWinTreeNodeClient )
            {
            CHuiCanvasVisual* canvasVisual = static_cast<CHuiCanvasVisual*>(viz);
            
            if ( windowAttributes->iActive )
                {
                canvasVisual->SetCanvasFlags( EHuiCanvasFlagIncludeToVisibilityCalculation );
                }
            else
                {
                canvasVisual->ClearCanvasFlags( EHuiCanvasFlagIncludeToVisibilityCalculation );
                }
            }
        
        iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);  // TODO: Check if really changed  
        }
    else
        {
        __ALFLOGSTRING("CAlfBridge::HandleIncludeToVisibilityCalculationL, EAlfDSIncludeToVisibilityCalculation: Visual not found!");                                                
        }              
    }

// ---------------------------------------------------------------------------
// HandleSetWindowAreaL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetWindowAreaL( TAlfBridgerData& aData )
	{
	TInt windowNodeId = aData.iInt2;
	TAny* ptr = (TAny*)(*iHost)->GetVarDataL( (TInt)aData.iPtr );
	
	TInt screenNumber = 0;
	TPoint origin;
	TInt count = 0;
	RRegion region;
	CleanupClosePushL( region );
	
	    {
	    RMemReadStream str(ptr, 4*sizeof(TInt32));	
	    screenNumber = str.ReadInt32L(); 
    	origin.iX = str.ReadInt32L();
	    origin.iY = str.ReadInt32L();
	    count = str.ReadInt32L();
        str.Release();
        }

    if ( count > 0 )        
        {
        RMemReadStream str(((TUint8*)ptr) + 4*sizeof(TInt32), 4*sizeof(TInt32)*count);
        TRect rect;
        for (TInt i = 0; i < count; ++i)
            {
            rect.iTl.iX = str.ReadInt32L();
            rect.iTl.iY = str.ReadInt32L();
            rect.iBr.iX = str.ReadInt32L();
            rect.iBr.iY = str.ReadInt32L();
            region.AddRect(rect);
            }
        str.Release();
        }
    
	
	// fetch visual
	CHuiVisual* viz = FindVisual(windowNodeId);
	if (viz)
		{
#ifdef HUI_DEBUG_TRACK_DRAWING
        if ( viz->Tracking() )
            {
            RDebug::Print(_L("CAlfBridge::HandleSetWindowAreaL - Tracked visual"));
            }
#endif
		
		
		CHuiCanvasVisual* canvas = static_cast<CHuiCanvasVisual*>(viz);
		canvas->SetShape( origin, region );
		
		iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);
		}
	else
		{
		__ALFLOGSTRING("CAlfBridge::HandleSetWindowAreaL, EAlfDSSetWindowArea: Visual not found!");                                                
		}  
		
    CleanupStack::PopAndDestroy();
	}

// ---------------------------------------------------------------------------
// HandleReorderWindowL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleReorderWindowL( TAlfBridgerData& aData )
	{
	TInt windowGroupNodeId = aData.iInt1;
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber;
	TInt ordinalPosition = windowAttributes->iOrdinalPosition;

	switch (windowAttributes->iWindowNodeType)
		{
		case EAlfWinTreeNodeGroup:
			{        

			// HACK !!!!                                                
			#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
			if (windowGroupNodeId == iAlfWindowGroupNodeId)
				{
				__ALFLOGSTRING1("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: Alf window group going to: %d ",windowAttributes->iOrdinalPosition);    
				}
			else
				{
				__ALFLOGSTRING1("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: WS window group going to: %d ",windowAttributes->iOrdinalPosition);                                        
				}    
			#endif

			CHuiControlGroup* controlGroup = FindControlGroup(windowGroupNodeId,screenNumber);
			
            if (!controlGroup)
                {
                User::Leave(KErrNotFound);
                }
            ResolveAfterEffectAppearingApplicationL(controlGroup); // does nothing, if effect is not active on this control group
			// Window server nodes are in inverted Z-order, we switch it here.
			iAlfScreens[screenNumber]->iDisplay->Roster().Hide(*controlGroup);                            
			TInt wsWindowGroupCount = 0;
			for (TInt i=0; i<iAlfScreens[screenNumber]->iDisplay->Roster().Count();i++)
				{
                if (iAlfScreens[screenNumber]->iDisplay->Roster().ControlGroup(i).Control(0).Role()==EHuiWindowGroupContainer)
					{
					wsWindowGroupCount++;    
					}
				}
			ordinalPosition = wsWindowGroupCount - windowAttributes->iOrdinalPosition;
			
			#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
			__ALFLOGSTRING1("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: wsWindowGroupCount: %d ", wsWindowGroupCount);                                        
			__ALFLOGSTRING1("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: Wimpautettu: %d ", ordinalPosition);                                        
			//RDebug::Print(_L("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: Last total pos: %d "), iDisplay->Roster().Count() - 1);                                        
			#endif
			
			if (ordinalPosition < -1)
				{
				ordinalPosition = -1; // show at top ?    
				}
			
			if (iAlfScreens[screenNumber]->iDisplay && controlGroup)
				{
				ShowControlGroupL(iAlfScreens[screenNumber]->iDisplay->Roster(), *controlGroup, ordinalPosition, screenNumber); 
				}
			else
				{
				__ALFLOGSTRING2("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: Control group not found! Screen: %d, Id: %d ", screenNumber, windowNodeId );        
				}
			
			ReorderAlfControlGroupsL(screenNumber);                                        
			break;    
			}
		case EAlfWinTreeNodeClient:
		case EAlfWinTreeNodeRoot:
		case EAlfWinTreeNodeAnim:
		case EAlfWinTreeNodeSprite:                            
		case EAlfWinTreeNodeFloatingSprite:
		case EAlfWinTreeNodeTextCursor:
			{
			
			// fetch visual
			CHuiLayout* layout = NULL;
			CHuiVisual* viz = FindVisual(windowNodeId);
			if (viz)
				{
#ifdef HUI_DEBUG_TRACK_DRAWING
                                if ( viz->Tracking() )
                                    {
                                    __ALFLOGSTRING("CAlfBridge::HandleReorderWindowL - Tracked visual");
                                    }
#endif
				layout = viz->Layout();
				__ALFLOGSTRING3("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: Id: %d, position %d, layout count %d", windowNodeId, windowAttributes->iOrdinalPosition, layout->Count() );
				TInt pos = windowAttributes->iOrdinalPosition;
				TInt layoutCount = layout->Count();
				
				if (windowAttributes->iOrdinalPosition > layoutCount - 1 )
					{
					__ALFLOGSTRING("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: Illegal visual index !");                                            
					pos = layoutCount - 1; // FixMe !!!
					}

				pos = layoutCount - pos - 1;    
				layout->Reorder(*viz, pos);
				}
			else
				{
				__ALFLOGSTRING2("CAlfBridge::HandleReorderWindowL, EAlfDSReorder: Visual not found! Screen: %d, Id: %d ", screenNumber, windowNodeId );    
				}                       
			break;
			}
		default:
			{
			__ALFLOGSTRING("CAlfBridge::HandleReorderWindowL: Unknown window node type received !");    
			break;
			}
		}
  
	iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue); // TODO: Check if really changed

	AMT_MAP_BRIDGE_SET_ORDINAL_POSITION();
	}

// ---------------------------------------------------------------------------
// HandlePostCanvasBufferL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandlePostCanvasBufferL( TAlfBridgerData& aData )
	{
	TInt windowGroupNodeId = aData.iInt1;
	TInt windowNodeId = aData.iInt2;
	TAlfWindowCommandBufferAttributes* bufferAttributes = (TAlfWindowCommandBufferAttributes*)(*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = bufferAttributes->iScreenNumber;

	CHuiCanvasVisual* viz = (CHuiCanvasVisual*)FindVisual(windowNodeId);
	
	// For now we omit drawing commands to ALF window group because in some configurations it causes issues
	// TODO: Possible viz->Flags() & EHuiVisualFlagDrawAf.. is not needed anymore. To be checked. Currently this enables floating sprites on the "root visual"
	if (viz && iAlfWindowGroupNodeId != windowGroupNodeId
	        ||  ( viz && viz->Flags() & EHuiVisualFlagDrawAfterOthers )/* THIS LINE IS HACK FOR DRAWING FLOATING SPRITES */ )
		{
#ifdef HUI_DEBUG_TRACK_DRAWING
        TBool tracking( EFalse );
	CHuiControlGroup* controlGroup = FindControlGroup( windowGroupNodeId, screenNumber );
	CAlfScreen* screen = iAlfScreens[screenNumber];
	
	//TInt clientWindowGroupId = FindClientWindowGroupId( screenNumber, *controlGroup );
	// NOTE, THE FOLLOWING WServClientFileName MAY CAUSE DEADLOCK. 
	// THIS DOES NOT HAPPEN IF HUI_DEBUG_TRACK_DRAWING IS NOT ENABLED (DEFAULT)
	TFileName processName;/* = iCommandDebug->WServClientFileName( clientWindowGroupId, CHuiStatic::WsSession() );
	
	if ( viz->Tracking() || 
	        ( processName.Length() > 0 && iCommandDebug->TrackProcess( (HBufC16*)processName.Ptr() ) ) )
            {
            tracking = ETrue; 
            }*/
#endif
		// Now when we receive actual drawing commands, we can set the window as opaque (unless the flags say it isn't opaque).		
	    if (bufferAttributes->iWindowNodeFlags & EAlfWinNodeFlagOpaque)
			{
			viz->SetFlag(EHuiVisualFlagOpaqueHint); 
			}
	    
	    if (!viz->HasCommandBuffers(EFalse))
	        {
            // For performance resons, only set visual tree changed if this 
            // was the first buffer for the window. 
	        iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);
	        }

		TPtrC8 commands((TUint8*)bufferAttributes->iBuffer, bufferAttributes->iBufferLength);
		if ( bufferAttributes->iEmptyThisBuffer )
			{
			viz->ClearCommandSet();
			}
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
		if (!(viz->Flags() & EHuiVisualFlagInactive))
		    {
            TInt secureId = viz->Owner().ControlGroup()->SecureId();
            TInt64 temp(KErrNotFound);
            TBool effects(EFalse);
            AMT_GET_TIME(temp, secureId, 0, effects);
            if (temp != KErrNotFound && effects == EFalse)
                {
                TTime time;
                time.UniversalTime();
                TTime startTime(temp);
                    
                TInt64 reactionTime = time.MicroSecondsFrom(startTime).Int64();
                    
                RDebug::Printf("CAlfBridge::HandlePostCanvasBufferL - Reaction time \t0x%x\t%f", secureId, (TReal)reactionTime / 1000000.0f );
                AMT_RESET_TIME(secureId);
                }
		    }
#endif
		
		// If tracking has been enabled for this CHuiCanvasVisual object, the buffers will be marked for tracking aswell in CHuiCanvasVisual
		switch( bufferAttributes->iPartStatus )
			{
			case TAlfWindowCommandBufferAttributes::EPartComplete:
				{
				viz->AddCommandSetL( commands );
#ifdef HUI_DEBUG_TRACK_DRAWING
				viz->SetTrackCommandSet( processName, tracking );
#endif
				break;
				}
			case TAlfWindowCommandBufferAttributes::ENotComplete: // ENotComplete part should be eventually followed by ELastPart
				{
				viz->AddPartialCommandSetL( commands, EFalse );
				break;
				}
			case TAlfWindowCommandBufferAttributes::ELastPart:
				{
				viz->AddPartialCommandSetL( commands, ETrue );
#ifdef HUI_DEBUG_TRACK_DRAWING
                viz->SetTrackCommandSet( processName, tracking );
#endif
				break;
				}
			default:
			    __ALFLOGSTRING("CAlfBridge::HandlePostCanvasBufferL, EAlfDSPostCanvasBuffer: Unknown iPartStatus !!!!");                                                         	
				USER_INVARIANT();
				break;
			}
		}
	else
		{
		__ALFLOGSTRING3("CAlfBridge::HandlePostCanvasBufferL, EAlfDSPostCanvasBuffer: Visual not found! Screen: %d, Id: %d, GroupId: %d ", screenNumber, windowNodeId, windowGroupNodeId );                                 
		}    
#ifdef	USE_APPLICATION_ENDFULLSCREEN_TIMEOUT	
	    if (iFSFxData
	            && iFSFxData->iEffectType != CFullScreenEffectState::ENotDefinedEffect 
	            && ((iFSFxData->State() == EWaitEndFullScreen && iFSFxData->iTwoPhaseEffect <= CFullScreenEffectState::ESecondPartActive)))
	        {
	        CHuiControlGroup *to_group = NULL;
            if (iFSFxData->iEffectType == CFullScreenEffectState::EExitEffect)
                {
                to_group = FindControlGroupByFullScreenFromEffect();
                }
            else
                {
                to_group = FindControlGroupByFullScreenToEffect();
                }
            
	    
	        // 1. if we get drawing commands after BeginFullScreen, we need to generate EndFullScreen call.
	        // 2. only the first drawing commands will trigger EndFullScreen
	        // (this mechanism is here because we want to remove delay from fullscreen effects - and EndFullScreens are coming from application too late)
	        
	        if (viz && viz->Owner().ControlGroup() == to_group)
	            {
                GfxTriggerEffectWhenFullScreenDrawn(to_group);
	            }
	        }
#endif
	}

// ---------------------------------------------------------------------------
// HandleGfxEndFullScreenTimeout
// ---------------------------------------------------------------------------
// 
void CAlfBridge::GfxTriggerEndFullScreen(CFullScreenEffectState* aFullScreenEffectData)
    {
    if (iFSFxData->State() == EFinalEffectActive)
        {
        return;
        }
    CHuiControlGroup *to_group = FindControlGroupByFullScreenToEffect();
    CHuiControlGroup *from_group = FindControlGroupByFullScreenFromEffect();
    CHuiLayout *to_layout = NULL;
    CHuiLayout *from_layout = NULL;
    if (!to_group) // group has been destroyed and moved to effectControlGroup for waiting the application exit effect EndFullScreen trigger 
         {
        to_layout = FindLayoutByEffectHandle(aFullScreenEffectData->iHandle);
         }
     else
         {
         CHuiControl& control = to_group->Control(0);
         to_layout = (CHuiLayout*)&control.Visual(0);
         }
        
    if (from_group)
        {
        CHuiControl& control = from_group->Control(0);
        from_layout = (CHuiLayout*)&control.Visual(0);
        }
    aFullScreenEffectData->iOperation = MAlfGfxEffectPlugin::EEndFullscreen;
    if (to_layout)
        {
        // from layout may be undefined
        __ALFFXLOGSTRING1("CAlfBridge::GfxTriggerEndFullScreen : Enough app drawing. Trigger EndFullScreen for layout 0x%x", to_layout);    
        HandleGfxEventL( *aFullScreenEffectData, to_layout, from_layout );
        aFullScreenEffectData->SetState(EFinalEffectActive);
        }

    iAlfScreens[0]->SetVisualTreeVisibilityChanged(ETrue);
    }

// ---------------------------------------------------------------------------
// SetWindowActiveL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::SetWindowActiveL(CHuiVisual* aVisual, TBool aActive)
    {
    TInt effectIndex;
    if (!HasActiveEffect(aVisual, effectIndex))
        {
        // Does not have effect
        if (aActive)
            {
            aVisual->ClearFlags(EHuiVisualFlagShouldBeHidden | EHuiVisualFlagShouldBeShown);
            aVisual->iOpacity.Set(KAlfVisualDefaultOpacity);
            }
        else
            {
            aVisual->ClearFlags(EHuiVisualFlagShouldBeShown | EHuiVisualFlagShouldBeHidden);
            aVisual->iOpacity.Set(0.0f);
            }
        }
    else
        {
        __ALFFXLOGSTRING2("CAlfBridge::SetWindowActiveL 0x%x has active effect. New state: %d", aVisual, aActive);
        // Has effect
		// these flags are put to action in CleanFxItem
        if (aActive)
            {
			// this prevents windows appearing before their "effected" time
            if (!iEffectCleanupStack[effectIndex].iHideWhenFinished)
                {
                // this is appear effect. Lets show it. if effect would be stopped
                // by some other effect, then EHuiVisualFlagShouldBeShown assures the correct state
                // after cleanup.
                aVisual->iOpacity.Set(KAlfVisualDefaultOpacity);
                aVisual->SetFlag(EHuiVisualFlagShouldBeShown);
                }
            else
                {
                aVisual->SetFlag(EHuiVisualFlagShouldBeShown);
                }
            aVisual->ClearFlag(EHuiVisualFlagShouldBeHidden);
            }
        else
            {
			// this prevents windows disappearing before their effect has finished, unless it makes no difference. 
            // For instance, if the visual is being shown by a screenshot in some other visual, then having this visual around 
            // will only break the "live view"
            if (iEffectCleanupStack[effectIndex].iCanDestroyOrHideImmediately && !aVisual->Effect())
                {
                aVisual->ClearFlags(EHuiVisualFlagShouldBeShown);
                aVisual->SetFlag(EHuiVisualFlagShouldBeHidden);
                //if effect would be stopped by some other effect, then EHuiVisualFlagShouldBeShown flag
                // assures the correct state after cleanup.
                aVisual->iOpacity.Set(0.0f);    
                }
            else
                {
                aVisual->SetFlag(EHuiVisualFlagShouldBeHidden);
                aVisual->ClearFlag(EHuiVisualFlagShouldBeShown);
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleSetWindowActiveL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetWindowActiveL( TAlfBridgerData& aData )
	{
	TInt windowGroupNodeId = aData.iInt1;
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*)(*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber;

    if (windowAttributes->iWindowNodeType == EAlfWinTreeNodeTextCursor && iCursorTimer)
	    {
		if (!windowAttributes->iActive)
		    {
		    iCursorTimer->Cancel();
		    }
        else
            {
            SetCursorTimerL(); // continue with set interwal
            }   
		}


	switch (windowAttributes->iWindowNodeType)
		{
		case EAlfWinTreeNodeGroup:
			{
			CHuiControlGroup* controlGroup = FindControlGroup(windowGroupNodeId,screenNumber);
			if (controlGroup)
				{
				CHuiControl& control = controlGroup->Control(0);
				CHuiLayout* layout = (CHuiLayout*)&control.Visual(0);
                // Uses opacity for now
                SetWindowActiveL(layout, windowAttributes->iActive);
                }
            else
                {
				__ALFLOGSTRING("CAlfBridge::HandleSetWindowActiveL, EAlfDSSetWindowActive: EAlfWinTreeNodeGroup not found!");         
				}    
			break;    
			}
		case EAlfWinTreeNodeClient:
		case EAlfWinTreeNodeRoot:
		case EAlfWinTreeNodeAnim:
		case EAlfWinTreeNodeSprite:                            
		case EAlfWinTreeNodeFloatingSprite:
		case EAlfWinTreeNodeTextCursor:
			{                                                        
			CHuiVisual* viz = FindVisual(windowNodeId);
			if (viz)
				{
#ifdef HUI_DEBUG_TRACK_DRAWING
                if ( viz->Tracking() )
                   {
                   RDebug::Print(_L("CAlfBridge::HandleSetWindowActiveL - Tracked visual"));
                   }
#endif
				// Uses opacity for now                            
                SetWindowActiveL(viz, windowAttributes->iActive);                
                }
			else
				{
				__ALFLOGSTRING("CAlfBridge::HandleSetWindowActiveL, EAlfDSSetWindowActive: Visual not found!");     
				}    
			break;
			}
		default:
			{
			__ALFLOGSTRING("CAlfBridge::HandleSetWindowActiveL, EAlfDSSetWindowActive: Unknown iWindowNodeType!");                                                         	
			USER_INVARIANT();
			break;                       	        
			}
		}
    iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue);	// TODO: Check if really changed                       

    AMT_INC_COUNTER( iTotalVisualFlagChangedCount );
    AMT_INC_COUNTER_IF( windowAttributes->iActive, iActiveVisualCount );
    AMT_INC_COUNTER_IF( !windowAttributes->iActive, iPassiveVisualCount );

    AMT_MAP_SET_VALUE( iBoolMap, windowNodeId, windowAttributes->iActive,
                       EAlfModuleTestTypeBridgeChangeWindowVisibility );
    AMT_MAP_SET_VALUE( iBoolMap, windowNodeId, windowAttributes->iActive,
                       EAlfModuleTestTypeBridgeActiveWindow );
	}


// ---------------------------------------------------------------------------
// HandleSetWindowFlagL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetWindowFlagL( TAlfBridgerData& aData, TInt aOp )
    {
    TInt windowGroupNodeId = aData.iInt1;
    TInt windowNodeId = aData.iInt2;
    TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*)(*iHost)->GetVarDataL( (TInt)aData.iPtr );
    TInt screenNumber = windowAttributes->iScreenNumber;
    TInt windowFlag = windowAttributes->iActive;
    THuiVisualFlags visualFlag = (THuiVisualFlags)0;
    switch(windowFlag)
        {
        case TAlfWindowData::EShouldInactivate:
            {
            visualFlag = EHuiVisualFlagShouldBeHidden;
            break;
            }
        case TAlfWindowData::EShouldDestroy:
            {
            visualFlag = EHuiVisualFlagShouldDestroy;
            break;
            }
        default:;
        }
    
    switch (windowAttributes->iWindowNodeType)
        {
        case EAlfWinTreeNodeGroup:
            {
            CHuiControlGroup* controlGroup = FindControlGroup(windowGroupNodeId,screenNumber);
            if (controlGroup)
                {
                CHuiControl& control = controlGroup->Control(0);
                CHuiLayout* layout = (CHuiLayout*)&control.Visual(0);
                // Uses opacity for now                            
                if (aOp == EAlfDSSetWindowFlag)
                    {
                    layout->SetFlag(visualFlag);
                    }
                else
                    {
                    layout->ClearFlag(visualFlag);
                    } 
                }
            else
                {
                __ALFLOGSTRING("CAlfBridge::HandleSetWindowActiveL, EAlfDSSetWindowActive: EAlfWinTreeNodeGroup not found!");         
                }    
            break;    
            }
        case EAlfWinTreeNodeClient:
        case EAlfWinTreeNodeRoot:
        case EAlfWinTreeNodeAnim:
        case EAlfWinTreeNodeSprite:                            
        case EAlfWinTreeNodeFloatingSprite:
        case EAlfWinTreeNodeTextCursor:
            {                                                        
            CHuiVisual* viz = FindVisual(windowNodeId);
            if (viz)
                {
                if (aOp == EAlfDSSetWindowFlag)
                    {
                    viz->SetFlag(visualFlag);
                    if (visualFlag == EHuiVisualFlagShouldDestroy )
                        {
                        if (windowAttributes->iWindowNodeType == EAlfWinTreeNodeTextCursor && iCursorTimer)
                            {
                            iCursorTimer->Cancel();
                            }        
                        RemoveVisual(windowNodeId); // Only the effect will have pointer to this visual from here on
                        }
                    }
                else
                    {
                    viz->ClearFlag(visualFlag);
                    }
                }
            else
                {
                __ALFLOGSTRING("CAlfBridge::HandleSetWindowFlagL, HandleSetWindowFlagL: Visual not found!");     
                }    
            break;
            }
        default:
            {
            __ALFLOGSTRING("CAlfBridge::HandleSetWindowFlagL, HandleSetWindowFlagL: Unknown iWindowNodeType!");                                                          
            USER_INVARIANT();
            break;                                  
            }
        }
    iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue); // TODO: Check if really changed                       
    }

// ---------------------------------------------------------------------------
// InsertImageBrushL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::InsertImageBrushL(CHuiVisual& aVisual, TInt aBitmapHandle)
    {
    // NOTE: We wssume that image brush is always the only brush
    CHuiTexture* texture = CHuiTexture::NewL();
    CleanupStack::PushL(texture);
    THuiImage image(*texture);	        				        				    
    CHuiImageBrush* imageBrush = CHuiImageBrush::NewL(image);				            				        
    imageBrush->SetLayer(EHuiBrushLayerForeground);
    CleanupStack::Pop(texture);

    aVisual.EnableBrushesL(ETrue);
    aVisual.Brushes()->InsertL(0, imageBrush, EHuiHasOwnership);    				        				            
    
    UpdateImageBrushL(aVisual, aBitmapHandle);
    }

// ---------------------------------------------------------------------------
// UpdateImageBrushL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::UpdateImageBrushL(CHuiVisual& aVisual, TInt aBitmapHandle)
    {
    // NOTE: We wssume that image brush is always the only brush with EImageBrush type
    const TInt KAlfCanvasVisualImageBrushIndex = 0;    

	if (aVisual.Brushes() && aVisual.Brushes()->BrushWithTypeCount(CHuiBrush::EImageBrush))
	    {    				            
        CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
        CleanupStack::PushL(bitmap);
        bitmap->Duplicate(aBitmapHandle);

	    CHuiImageBrush* imageBrush = (CHuiImageBrush*) aVisual.Brushes()->BrushWithTypeAt(CHuiBrush::EImageBrush, KAlfCanvasVisualImageBrushIndex);    	    	    
	    CHuiTexture* texture = (CHuiTexture*) &imageBrush->Image().Texture();            
        
        // Use of direct upload here would give much better performance, but is
        // it too risky ? 
        texture->UploadL(*bitmap, NULL);
        imageBrush->SetChanged();

        CleanupStack::PopAndDestroy(bitmap);    				                
	    }
    }

// ---------------------------------------------------------------------------
// RemoveImageBrushL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::RemoveImageBrushL(CHuiVisual& aVisual)
    {
    // NOTE: We wssume that image brush is always the only brush with EImageBrush type
    const TInt KAlfCanvasVisualImageBrushIndex = 0;
    
    if (aVisual.Brushes() && aVisual.Brushes()->BrushWithTypeCount(CHuiBrush::EImageBrush))
        {
	    CHuiImageBrush* imageBrush = (CHuiImageBrush*) aVisual.Brushes()->BrushWithTypeAt(CHuiBrush::EImageBrush, KAlfCanvasVisualImageBrushIndex);    
        CHuiTexture* texture = (CHuiTexture*) &imageBrush->Image().Texture();
	    THuiImage image;
        imageBrush->SetImage(image);
        delete texture;
        }
    
    if (aVisual.Brushes())
        {
        aVisual.EnableBrushesL(EFalse);				                
        }            
    }


// ---------------------------------------------------------------------------
// HandleSetSurfaceExtentL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetSurfaceExtentL( TAlfBridgerData& aData )
	{
	TInt windowGroupNodeId = aData.iInt1;
	TInt windowNodeId = aData.iInt2;
	TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*)(*iHost)->GetVarDataL( (TInt)aData.iPtr );
	TInt screenNumber = windowAttributes->iScreenNumber;
	// fetch visual
	CHuiCanvasVisual* viz = FindVisual(windowNodeId);

    // We should not do this for alf window, but renderstage does not know if it is
    // handling alf window or other window, so we ignore the request here if needed.
    // TODO: When alf apps work in multiple display, check here for every displays alf groups...
	if (viz && iAlfWindowGroupNodeId != windowGroupNodeId)
		{
        viz->SetLayerExtent(windowAttributes->iSurfaceExtent);
		// Set surface extend area to be cleared with fully transparent color
		// Note: does not support extend that is bigger that the visual
		// if surface extent is (0,0,0,0) then the background item array is reseted 
		// and surface extent will not be cleared.	
		if (windowAttributes->iSurfaceExtent != TRect())
			{
			// Set surface extend area to be cleared with fully transparent color
			// Note: does not support extend that is bigger that the visual
			TRgb fullyTransparentColor(255,0,0,0); // Red but fully transparent. Red color component has been set only for debug purposes, it is not visible.
			viz->SetClearBackground(CHuiDisplay::EClearWithColor);
			viz->SetBackgroundColor(fullyTransparentColor);
			}
		else
			{
            viz->SetClearBackground(CHuiDisplay::EClearNone);

            // Force update
            viz->ClearCanvasFlags( EHuiCanvasFlagSurfaceInvisible );
            viz->ClearCanvasFlags( EHuiCanvasFlagSurfaceVisible );
			}		
		}
	else
		{
		if (iAlfWindowGroupNodeId != windowGroupNodeId)
		    __ALFLOGSTRING("CAlfBridge::HandleSetSurfaceExtentL, EAlfDSSetSurfaceExtent: Visual not found!");    
		}                            
    iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue); // TODO: Check if really changed   
	}

// ---------------------------------------------------------------------------
// HandleLayerUsesAlphaFlagChanged
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleLayerUsesAlphaFlagChanged( TAlfBridgerData& aData )
    {
    TInt windowGroupNodeId = aData.iInt1;
    TInt windowNodeId = aData.iInt2;
    TAlfWindowAttributes* windowAttributes = (TAlfWindowAttributes*)(*iHost)->GetVarDataL( (TInt)aData.iPtr );
    TInt screenNumber = windowAttributes->iScreenNumber;
    
    // fetch visual
    CHuiCanvasVisual* viz = (CHuiCanvasVisual*)FindVisual(windowNodeId);

    // We should not do this for alf window, but renderstage does not know if it is
    // handling alf window or other window, so we ignore the request here if needed.
    // TODO: When alf apps work in multiple display, check here for every displays alf groups...
    if (viz && iAlfWindowGroupNodeId != windowGroupNodeId)
        {
        viz->SetLayerUsesAlphaFlag(windowAttributes->iLayerUsesAlphaFlagEnabled);
        }
    else
        {
        if (iAlfWindowGroupNodeId != windowGroupNodeId)
            __ALFLOGSTRING("CAlfBridge::HandleLayerUsesAlphaFlagChanged: Visual not found!");    
        }                            
    iAlfScreens[screenNumber]->SetVisualTreeVisibilityChanged(ETrue); // TODO: Check if really changed   
    }


// ---------------------------------------------------------------------------
// HandleGetNativeWindowDataL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleGetNativeWindowDataL( TAlfBridgerData& aData )
	{
	// Get the data from original Render Stage message
	RMessage2* rsMessage = (RMessage2*)aData.iInt1;
	TAlfNativeWindowData winData;
	TPckg<TAlfNativeWindowData> winDataPckg(winData);
	rsMessage->Read(0, winDataPckg);

	if(winData.iScreenNumber == 0)
		{
		// primary screen
		CHuiDisplay& disp = iHuiEnv->PrimaryDisplay();
		if (disp.NativeWindow())
			{
			winData.iAlfWindowHandle = disp.NativeWindow()->ClientHandle();                    
			winData.iAlfWindowGrpId = disp.NativeWindow()->WindowGroupId();	
			}
		}
	else
		{
		// other than primary screen 
		for (TInt i = 1; i < iAlfScreens.Count(); i++)
			{
			if (iAlfScreens[i]->iScreenNum == winData.iScreenNumber)
				{
				RDrawableWindow* win = iAlfScreens[i]->iCoeControl->DrawableWindow();
				winData.iAlfWindowHandle = win->ClientHandle();
				winData.iAlfWindowGrpId = win->WindowGroupId();
				}
			}
		}

	// write result	
	rsMessage->Write(0, winDataPckg);

	if (winData.iAlfWindowHandle == 0)
		{
		__ALFLOGSTRING("CAlfBridge::HandleGetNativeWindowDataL, EAlfDSGetAlfNativeWindowData. Error: window data not found!");    
		}
	}


void CAlfBridge::PrepareFadeEffects( CHuiCanvasVisual& aVisual )
    {
    TBool faded = aVisual.CanvasFlags() & EHuiCanvasFlagExternalFade;     
    if (faded)
        {
        if (IsFadedByParent( aVisual ))
            {            
            // Faded by parent
            aVisual.SetCanvasFlags(EHuiCanvasFlagExternalFadeByParent);

            // Faded by parent, include to parent effect
            aVisual.ClearCanvasFlags(EHuiCanvasFlagExcludeFromParentEffect);            

            // Does not fade own children because parent already does that.
            aVisual.ClearCanvasFlags(EHuiCanvasFlagExternalFadeToChildren);
            }
        else
            {
            //  Not faded by parent 
            aVisual.ClearCanvasFlags(EHuiCanvasFlagExternalFadeByParent);

            // Not faded by parent, exclude from parent effect
            aVisual.SetCanvasFlags(EHuiCanvasFlagExcludeFromParentEffect);                

            // Does (or does not) fade own children if it can. 
            if (CanFadeChildren(aVisual))
                {
                aVisual.SetCanvasFlags(EHuiCanvasFlagExternalFadeToChildren);
                }
            else
                {
                aVisual.ClearCanvasFlags(EHuiCanvasFlagExternalFadeToChildren);
                }
            }
        }
    else
        {
        // Not faded 
        aVisual.ClearCanvasFlags(EHuiCanvasFlagExternalFadeByParent);

        if (IsNearestParentEffectFade( aVisual ))
            {
            // Exclude from parent effect to avoid getting faded by parent.
            aVisual.SetCanvasFlags(EHuiCanvasFlagExcludeFromParentEffect);
            }
        else
            {
            // By default included into parent effects.  
            aVisual.ClearCanvasFlags(EHuiCanvasFlagExcludeFromParentEffect);
            }
        
        // Does not fade own children because it is not faded itself. 
        aVisual.ClearCanvasFlags(EHuiCanvasFlagExternalFadeToChildren);
        }

    // Handle children
    TInt count = aVisual.Count();
    for (TInt i=0; i<count; i++)
        {
        CHuiCanvasVisual* child = (CHuiCanvasVisual*) (&aVisual.Visual(i));                        
        PrepareFadeEffects(*child);        
        }    
    }

TBool CAlfBridge::LoadFadeEffectsL( CHuiCanvasVisual& aVisual )
    {
    TBool fadeEffectLoadedInsideTree = EFalse;
    
    // Load fade effect (or remove existing)
    fadeEffectLoadedInsideTree |= SetupFadeEffectL(aVisual);
    
    // Handle children
    TInt count = aVisual.Count();
    for (TInt i=0; i<count; i++)
        {
        CHuiCanvasVisual* child = (CHuiCanvasVisual*) (&aVisual.Visual(i));                        
        fadeEffectLoadedInsideTree |= LoadFadeEffectsL(*child);        
        }
    
    return fadeEffectLoadedInsideTree;
    }


TBool CAlfBridge::CanFadeChildren( CHuiCanvasVisual& aParent )
    {
    // NOTE! This function is not yet implemented optimally !!!! 
    //       Below is some initial description how it could behave, but even that has some downsides. 
    //       Ultimate solution which would implement legacy symbian fading functionality and
    //       on the other hand fully effectable solution needs to be developed at some point.
    //        
    //
    // This function shold determine wheter we can fade whole visual tree starting from aParent with just
    // one fade effect. Even if visual tree has non-fading windows, it is possible to use tree fading
    // in some cases where children can be excluded from parent visual effects using EHuiCanvasFlagExcludeFromParentEffect
    // flag. However it changes drawing order of the childern (visuals with EHuiCanvasFlagExcludeFromParentEffect
    // are drawn after other children) so it cannot be used in those cases without side-effect.
    //    
    //
    //  V1 = Visual 1, parent of Visual 2 etc. Index number refers to order how visuals are drawn.
    // (f) = Visal is faded 
    //
    // Example scenario 1) CanFadeChildren(V1) (YES) 
    //                     CanFadeChildren(V2) (YES) (Can fade because faded child is drawn first, but no need because parent can fade as well)
    //
    //       V4     V3(f)
    //         \    /
    //          \  /
    //   V5(f)   V2(f)
    //     \    /
    //      \  /    
    //       V1(f)
    //
    // Example Scenario 2) CanFadeChildren(V1) (NO) (Cant fade because nonfading grandchild draw before faded)
    //                     CanFadeChildren(V2) (NO) (Can't fade children because non-faded child is drawn first)
    //               
    //       V4(f)   V3                     
    //         \    /                       
    //          \  /
    //   V5(f)   V2(f)
    //     \    /
    //      \  /    
    //       V1(f)
    //
    // Example scenario 3) CanFadeChildren(V1) (YES) (Can fade because faded child (V2) is drawn first)
    //                     CanFadeChildren(V2) (YES) (Can fade because all children are faded, but no need because parent can fade as well)
    //               
    //       V4(f)   V3(f)
    //         \    /
    //          \  /
    //   V5     V2(f)
    //     \    /
    //      \  /    
    //       V1(f)
    //
    // Scenario 4) CanFadeChildren(V1) (NO) (Can't fade because non-faded child is drawn first)
    //             CanFadeChildren(V2) (NO) (It would be YES, but V2 it self is not faded -> NO)
    //               
    //       V4(f)   V3(f)
    //         \    /
    //          \  /
    //   V5(f)   V2
    //     \    /
    //      \  /    
    //       V1(f)
    //
    // Scenario 5) CanFadeChildren(V1) (NO) (Can't fade because non-faded child is drawn first)
    //             CanFadeChildren(V2) (NO) (Can't fade children because it does not have children)
    //             CanFadeChildren(V3) (YES) (All children are faded)
    //               
    //       V4(f)   V4(f)
    //         \    /
    //          \  /
    //           V3(f)  V2
    //            \    /
    //             \  /    
    //             V1(f)
    //
    //
    // TODO: Real implementation for algorithm which takes into account the order of non-fading and
    //       fading children. Current implentation has side-effect where drawing order of non-fading
    //       children may get altered. But on the other hand without visual treefader (i.e each visual 
    //       faded with separate effects) there still exists a potential problem with faded transparent 
    //       windows above faded windows (double fade)!
    //
    
    // For now this function just checks if there are children to be faded... 
    return HasActiveFadedChildren( aParent ); 
    }
    

TInt CAlfBridge::RecursiveChildCount( CHuiCanvasVisual& aParent, TInt aCanvasFlags )
    {
    TInt childCount = 0;
    TInt count = aParent.Count();
    for (TInt i=0; i<count; i++)
        {
        CHuiCanvasVisual* child = (CHuiCanvasVisual*) (&aParent.Visual(i));        
        if (aCanvasFlags)
            {
            if (child->CanvasFlags() & aCanvasFlags)
                {
                childCount++;
                }
            }
        else
            {            
            childCount++;
            }
        childCount += RecursiveChildCount( *child, aCanvasFlags );        
        }
    return childCount;
    }

TBool CAlfBridge::IsNearestParentEffectFade( CHuiCanvasVisual& aVisual )
    {
    TBool nearestParentEffectFade = EFalse;
    CHuiCanvasVisual* parent = (CHuiCanvasVisual*) (aVisual.Layout()); 
    while (parent)
        {
        if (parent->Effect())
            {
            nearestParentEffectFade |= (parent->Effect() && (parent->Effect()->EffectFlags() & KHuiFadeEffectFlag));    
            return nearestParentEffectFade;            
            }        
        parent = (CHuiCanvasVisual*) (parent->Layout());
        }
    return EFalse;
    }


TBool CAlfBridge::IsFadedByParent( CHuiCanvasVisual& aVisual )
    {
    CHuiCanvasVisual* parent = (CHuiCanvasVisual*) (aVisual.Layout()); 
    while (parent)
        {
        TInt flags = parent->CanvasFlags();
        if ((flags & EHuiCanvasFlagExternalFade) && (flags & EHuiCanvasFlagExternalFadeToChildren))
            {
            return ETrue;
            }        
        parent = (CHuiCanvasVisual*) (parent->Layout());
        }
    return EFalse;
    }

TBool CAlfBridge::HasActivePaintedAreas( CHuiCanvasVisual& aVisual, TBool aIncludeChildren )
    {
    TBool visualHasPaintedAreas = aVisual.PaintedAreaCount() > 0;         
    if (!visualHasPaintedAreas && aIncludeChildren)
        {
        TInt count = aVisual.Count();        
        for (TInt i=0; i<count; i++)
            {
            CHuiCanvasVisual* child = (CHuiCanvasVisual*) (&aVisual.Visual(i)); 
            TBool childIsActive = !(child->Flags() & EHuiVisualFlagInactive);            
            if (childIsActive && child->PaintedAreaCount())
                {
                visualHasPaintedAreas = ETrue;
                break;
                }        
            }    
        }
    return visualHasPaintedAreas;
    }

TBool CAlfBridge::HasActiveFadedChildren( CHuiCanvasVisual& aVisual )
    {
    TBool has = EFalse;
    has |= IsAlfOriginatedWindow(aVisual);
    
    TInt count = aVisual.Count();        
    for (TInt i=0; i<count; i++)
        {
        CHuiCanvasVisual* child = (CHuiCanvasVisual*) (&aVisual.Visual(i)); 
        TBool active = !(child->Flags() & EHuiVisualFlagInactive);            
        TBool faded = child->CanvasFlags() & EHuiCanvasFlagExternalFade; 
        
        if (active && faded && (child->PaintedAreaCount() || IsAlfOriginatedWindow(*child)))
            {
            has = ETrue;
            break;
            }        
        }    
    return has;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAlfBridge::UpdateRootVisualsToFullscreen()
    {
    if (!iAlfScreens.Count())
        {
        return;    
        }
    
    // update all the layouts even inactive ones.
    CAlfScreen* screen = iAlfScreens[0];
    TRect fullscreen = TRect(TPoint(0,0), screen->Size());
    for (TInt j=screen->iDisplay->Roster().Count() - screen->FixedControlGroupCount(); j>=0; j--)
        {                
        CHuiControlGroup& controlgroup = screen->iDisplay->Roster().ControlGroup(j);
        CHuiControl& control = controlgroup.Control(0);

        if( control.Role() == EHuiWindowGroupContainer)
            {
            // Only update layout which are made to correspond window groups. 
            // Layouts that fullscreen effects are applied to
            CHuiCanvasVisual* layout = (CHuiCanvasVisual*)&control.Visual(0);
            layout->SetPos(fullscreen.iTl);
            layout->SetSize(fullscreen.Size());
            }
        }
    }



// ---------------------------------------------------------------------------
// SetupFadeEffectL
//
// NOTE: This method gets called very often, so we should not do anything time
//       consuming here (unless we really need to).
//
// ---------------------------------------------------------------------------
// 
TBool CAlfBridge::SetupFadeEffectL( CHuiCanvasVisual& aVisual )
    {
    TBool didFadeEffectLoad = EFalse;
    
    TBool faded = (aVisual.CanvasFlags() & EHuiCanvasFlagExternalFade);

    // Just in case, check if we are alrady faded to avoid unnecassary effect loadings.
    TBool alreadyFaded = aVisual.Effect() && (aVisual.Effect()->EffectFlags() & KHuiFadeEffectFlag);
        
    TBool fadesChildren = EFalse; 
    
    if (faded)
        {                
        fadesChildren = (aVisual.CanvasFlags() & EHuiCanvasFlagExternalFadeToChildren);
        
        // Inactive visuals are not faded.
        TBool visualIsActive = !(aVisual.Flags() & EHuiVisualFlagInactive);
        
        // Avoid interrupting other effects. 
        TBool otherEffectActive = aVisual.Effect() && !(aVisual.Effect()->EffectFlags() & KHuiFadeEffectFlag);

        // Check if we really need to fade. Note the order of if-conditions, fastest checks first to optimize performance.
        if (visualIsActive && !alreadyFaded && !otherEffectActive && !IsFadedByParent(aVisual) && (HasActivePaintedAreas(aVisual, fadesChildren) || IsAlfOriginatedWindow(aVisual)))
        	{        
        	CHuiFxEffect* effect = NULL;
        	CHuiFxEngine* engine = iHuiEnv->EffectsEngine();

            if (engine && iFadeEffectFile )
                {
                 __ALFLOGSTRING1( ">> Using cached fade effect file name: %S", iFadeEffectFile );
                TInt err = 0;
                // CHuiFxParser sets the effect to visual if loading succeeds.
                TInt effectFlags = 0;
                effectFlags |= KHuiFadeEffectFlag;
                effectFlags |= KHuiFxEffectDisableMarginsFlag; // Margins cannot safely be used in fade usecase, artefacts may appear depending on window location in window tree        			                    

                // For top level canvas visuals (window group visuals) disable background for 
                // performance reasons (in case canvas visual is not opaque).
                if (!aVisual.Layout())
                    {
                    effectFlags |= KHuiFxDisableBackground;
                    }

                if (fadesChildren)
                    {
                    /*
                     TInt children = RecursiveChildCount( aVisual, 0 );
                     TInt faded = RecursiveChildCount( aVisual, EHuiCanvasFlagExternalFade );
                     TInt fadedbyparent = RecursiveChildCount( aVisual, EHuiCanvasFlagExternalFadeByParent );
                     TInt excludedfromparenteffect = RecursiveChildCount( aVisual, EHuiCanvasFlagExcludeFromParentEffect );
                     RDebug::Print(_L("CAlfBridge::SetupFadeEffectL - faded parent has %i children"), children);
                     RDebug::Print(_L("CAlfBridge::SetupFadeEffectL - faded parent has %i faded children"), faded);
                     RDebug::Print(_L("CAlfBridge::SetupFadeEffectL - faded parent fades %i children"), fadedbyparent);
                     RDebug::Print(_L("CAlfBridge::SetupFadeEffectL - faded parent has %i children which are excluded from parent fx"), excludedfromparenteffect);
                     */
                    }
                else
                    {
                    // All children in the tree are not faded, do not apply effect on children.
                    effectFlags |= KHuiFxEffectExcludeChildrenFlag;
                    //RDebug::Print(_L("CAlfBridge::SetupFadeEffectL - no children faded by parent"));
                    }
                __ALFFXLOGSTRING1("CAlfBridge::SetupFadeEffectL - insert fade 0x%x", &aVisual);
                TRAP( err, engine->LoadEffectL(*iFadeEffectFile, effect, aVisual.Effectable(), NULL, NULL, 0, effectFlags ) );
                didFadeEffectLoad = ETrue;
        		}
        	}
        }
    else
        {
        // remove only an old fade effect, leave any others intact
        if (alreadyFaded)
            {
            aVisual.SetEffect( NULL );
            __ALFFXLOGSTRING1("CAlfBridge::SetupFadeEffectL - remove fade: 0x%x", &aVisual);
            alreadyFaded = EFalse;
            }
        }
    
    // If we did load and fade children we must re-prepare the flags again
    // Maybe there is more elegant way to avoid this. 
    if (didFadeEffectLoad && fadesChildren)
        {
        PrepareFadeEffects( aVisual );
        }
    
    return (didFadeEffectLoad || alreadyFaded);
    }


// ---------------------------------------------------------------------------
// HandleMoveWindowToNewGroupL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleMoveWindowToNewGroupL( TAlfBridgerData& aData )
    {
    TInt windowNodeId = aData.iInt1;
    TInt screenNumber = aData.iInt2;
    TInt newWindowGroupNodeId = (TInt)aData.iPtr;
   
    CHuiCanvasVisual* viz = (CHuiCanvasVisual*)FindVisual(windowNodeId);
    
#ifdef _ALF_LOGGING
    CHuiControlGroup* oldControlGroup = viz->Owner().ControlGroup();
    TInt oldWindowGroupId = 0; 
    if ( oldControlGroup  )
        {
        oldWindowGroupId = FindWindowGroupNodeId(screenNumber, *oldControlGroup );
        }   
    __ALFLOGSTRING3("CAlfBridge::HandleMoveWindowToNewGroupL moving nodeId 0x%x from group 0x%x to 0x%x ",windowNodeId, oldWindowGroupId, newWindowGroupNodeId );
#endif
    
    if (viz)
        {
        CHuiLayout* oldLayout = viz->Layout();
        TBool isRootOfTheMovingTree = EFalse; // if true, we need to update the parent layout 
        
        if ( viz->Owner().ControlGroup() && 
                (&viz->Owner().ControlGroup()->Control(0).Visual(0) == oldLayout) )
            {
            // Symbian OS documentation: "This function allows a window with a window group as its 
            // immediate parent to be moved from one window group to another one."
            //
            // This implementation is assuming that only a window which is an immediate child of a window group
            // can be moved to new group. Moving an immediate child (=root of the moving tree) 
            // causes the child windows to move as well.
            isRootOfTheMovingTree = ETrue;
            }

        // #1 remove visual from old group
        if ( isRootOfTheMovingTree && oldLayout )
            {
            // remove also from the old parent layout - new layout is coming from the new group
            oldLayout->Remove(viz);
            }
        // else: keep the parent layout - assume that is moving as well.  
        
        // remove from the old control
        viz->Owner().Remove(viz);
        
        // #2 insert visual to the beginning of the new group
        CHuiControlGroup* newControlGroup = FindControlGroup(newWindowGroupNodeId,screenNumber);
        if (newControlGroup)
            {
            CHuiControl& newControl = newControlGroup->Control(0);
            if ( isRootOfTheMovingTree )
                {
                // update the parent layout to be the default layout of the window group
                CHuiLayout* newLayout = (CHuiLayout*)&newControl.Visual(0);
                newControl.AppendL( viz, newLayout );
                }
            else
                {
                // just update the owner. Assume that the parent RWindowTreeNode/CAlfNode/CHuiLayout is moving as well.
                newControl.AppendL( viz );
                }
            }
        else 
            {
            __ALFLOGSTRING("CAlfBridge::HandleMoveWindowToNewGroupL cannot find new group! orphaning the visual");
            
            CleanFxVisual(viz);
             iOrphanStorage->AppendL( viz );
            }
        }
    else
        {
        __ALFLOGSTRING("CAlfBridge::HandleMoveWindowToNewGroupL cannot find moving visual!");
        }
    }

// ---------------------------------------------------------------------------
// HandleSetFadeEffect
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetFadeEffectL( TAlfBridgerData& aData )
    {
    TInt windowNodeId = aData.iInt1;  
    TBool faded = aData.iInt2;
    MWsWindowTreeNode::TType type = (MWsWindowTreeNode::TType)(TInt)aData.iPtr;
    CHuiVisual* viz = NULL;
    if (type != MWsWindowTreeNode::EWinTreeNodeGroup)
        {
        viz = (CHuiVisual*)FindVisual(windowNodeId);
        }
    else
        {
        CHuiControlGroup* cg = FindControlGroup(windowNodeId, 0); // TODO: Screen            
        if (cg) 
            {
            CHuiControl& control = cg->Control(0);
            viz = &control.Visual(0);
            }        
        }
    
    if ( viz )
        {
        CHuiCanvasVisual* canvasviz = (CHuiCanvasVisual*)viz;        
        if (faded)
            {
            canvasviz->SetCanvasFlags(EHuiCanvasFlagExternalFade);
            }
        else
            {
            canvasviz->ClearCanvasFlags(EHuiCanvasFlagExternalFade);
            }        
        iAlfScreens[0]->SetVisualTreeVisibilityChanged(ETrue); // TODO: Screen 
        }
    }

void CAlfBridge::HandleSetLayoutSwitchEffectL()
    {
    ClearCanvasVisualCommandSets(EFalse); 
    iLayoutSwitchEffectCoordinator->BeginLayoutSwitch();
    }

TInt Blink(TAny* aVisual)
    {
    CHuiVisual* visual = (CHuiVisual*)aVisual;
    if (visual->iOpacity.Now() > 0.01 ) // kind of an epsilon
        {
        visual->iOpacity.Set(0);
        }
    else
        {
        visual->iOpacity.Set(1);
        }

    return ETrue; // call me again
    }                          
    
void CAlfBridge::SetCursorTimerL(TUint aTime, CHuiVisual* aCursor)
    {
    if (aTime)
        {
        iCursorInterval = aTime;
        }
        
    if (aCursor)    
        {
        iCursorVisual = aCursor;
        }   
         
    if (iCursorInterval && iCursorVisual)        
        {
        if (iCursorTimer)
            {
            iCursorTimer->Cancel();
            }
    
        if (!iCursorTimer)
            {
            iCursorTimer = CPeriodic::NewL(CActive::EPriorityHigh);
            }
    
        iCursorTimer->Start(iCursorInterval, iCursorInterval, TCallBack(Blink, aCursor));                
        }
    }        
    
// ---------------------------------------------------------------------------
// HandleSetCursorDataL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::HandleSetCursorDataL( TAlfBridgerData& aData )
    {
    TInt windowNodeId = aData.iInt2;
    
    TAlfCursorDataBufferAttributes* windowAttributes = (TAlfCursorDataBufferAttributes*) (*iHost)->GetVarDataL( (TInt)aData.iPtr );
    CHuiVisual* viz = (CHuiVisual*)FindVisual(windowNodeId);
    if (viz)
        {
        viz->SetFlags(windowAttributes->iFlags);
        SetCursorTimerL(windowAttributes->iFlashInterval, viz);
        }
    else
        {
        __ALFLOGSTRING1("CAlfBridge::HandleSetCursorDataL - WARNING! Cursor node 0x%x not found!", windowNodeId);
        }
    
    AMT_INC_COUNTER_IF(viz, iTotalVisualAttributeChangedCount);
    
    AMT_MAP_BRIDGE_TEXT_CURSOR_CHANGE();    
    }

// ---------------------------------------------------------------------------
// HandleSetNodeTracking
// ---------------------------------------------------------------------------
// 
#ifdef HUI_DEBUG_TRACK_DRAWING
void CAlfBridge::HandleSetNodeTracking( TAlfBridgerData& aData )
    {
    TInt windowNodeId = aData.iInt1;
    CHuiCanvasVisual* viz = (CHuiCanvasVisual*)FindVisual(windowNodeId);
    if ( aData.iInt2 )
        {
        viz->SetTracking( aData.iInt2 );
        }
#else
void CAlfBridge::HandleSetNodeTracking( TAlfBridgerData& )
    {
#endif
    }
	
void RecursiveStoreRenderBufferL(CHuiVisual *aVisual)
    {
    CHuiCanvasVisual *canvasvisual = dynamic_cast<CHuiCanvasVisual*>(aVisual);
    if (canvasvisual && canvasvisual->Layout()) 
        {
        canvasvisual->StoreRenderBufferL();
        }

    TInt c = aVisual->Count();
    for(int i=0;i<c;i++)
        {
        CHuiVisual &vis = aVisual->Visual(i);
        RecursiveStoreRenderBufferL(&vis);
        }
    }

void StoreRenderBufferStartL(CHuiLayout *aLayout)
    {
    // assumes that the layout takes full screen area.
    CHuiRenderPlugin& renderplugin = CHuiStatic::Renderer();
    CHuiCanvasGc *canvasGc = renderplugin.CreateCanvasGcL();
    CleanupStack::PushL(canvasGc);
    
    CHuiCanvasRenderBuffer *renderBuffer;
    renderBuffer = canvasGc->CreateRenderBufferL(TSize(0,0));
    CleanupStack::PushL(renderBuffer);
    renderBuffer->InitializeL(CHuiStatic::Env().Display(0).VisibleArea().Size());
    renderBuffer->Copy(TPoint(0,0));    
    CleanupStack::Pop(renderBuffer);
    aLayout->SetStoredRenderBuffer(renderBuffer); // moves ownership

    CleanupStack::PopAndDestroy();
    }
TBool NeedsStoredBuffers(CHuiFxEngine *aEngine, const TDesC &aFileName)
    {
    return aEngine->FxmlUsesInput1(aFileName);
    }
TBool FxmlHasOpaqueHint(CHuiFxEngine *aEngine, const TDesC &aFileName)
    {
    return aEngine->FxmlUsesOpaqueHint(aFileName);
    }

// ---------------------------------------------------------------------------
// StoreLayoutIfRequiredByEffectL
//
// Certains effects require screenshot to be taken when BeginFullScreen event
// is triggered. This method will take a screenshot (of the whole screen), if 
// there is enough memory available.
// ---------------------------------------------------------------------------
//
TBool CAlfBridge::StoreLayoutIfRequiredByEffectL(CHuiLayout* aLayout, CFullScreenEffectState& aEvent, TBool& aNeededStoredLayout)
    {
    aNeededStoredLayout = NeedsStoredBuffers(iHuiEnv->EffectsEngine(), *aEvent.iEffectName);
    __ALFFXLOGSTRING2("CAlfBridge::StoreLayoutIfRequiredByEffectL - aHandle: %d, Take screenhot: %d", aEvent.iHandle, aNeededStoredLayout);
    if (aNeededStoredLayout)
        {
        TRAPD(err,StoreRenderBufferStartL(aLayout));
        __ALFFXLOGSTRING1("CAlfBridge::StoreLayoutIfRequiredByEffectL - Screenshot result: KErrNone == %d", err);
        if (err == KErrNone)
            {
            aLayout->SetFreezeState(ETrue);
            aEvent.iCanDestroyOrHideImmediately = ETrue; // if children of this layout are hidden or destroyed during effect, that can happen immediately
            return ETrue; // storing of content requesteds, could store the content => success
            }
        else
            {
            return EFalse; // could not store the content => failed
            }
        }
    else
        {
        __ALFFXLOGSTRING("CAlfBridge::StoreLayoutIfRequiredByEffectL - Storing not required");
        return ETrue; // no storing requred, no screenshot required => success
        }
    }

TBool CAlfBridge::HandleGfxEventL(CFullScreenEffectState& aEvent, CHuiLayout* aToLayout, CHuiLayout *aFromLayout)
    {
    __ALFFXLOGSTRING3("CAlfBridge::HandleGfxEventL - To SecureUid: 0x%x, From SecureUid: 0x%x, effect %S", aEvent.iToSecureId, aEvent.iFromSecureId, aEvent.iEffectName);
    __ALFFXLOGSTRING4("CAlfBridge::HandleGfxEventL - To layout: 0x%x, From layout: 0x%x, Effect handle: %d, Action: %d", aToLayout, aFromLayout, aEvent.iHandle, aEvent.iAction);
    
    TInt err = KErrNone;
    TBool failed = EFalse;
    
    // Check if there is an effects engine in HuiEnv
    CHuiFxEffect* effect = NULL;
    // We need to pass the filenamerta from the transeffect plugin that handles the controls
    // that define which effect is used for the selected skin or the selected application
    // For full screen effects the skin should determine the effect, or there should be
    // some default effects.
    
    CHuiFxEngine* engine = NULL;
    // engine is not owned by us, it is a member of HuiEnv
    engine = iHuiEnv->EffectsEngine();

    if ( engine )
        { 
        switch(aEvent.iOperation)
            {
            case MAlfGfxEffectPlugin::EBeginFullscreen:
                {
                aToLayout->SetStoredRenderBufferModificationsEnabled(ETrue);
                TBool neededStoredBuffers;
                TBool success = StoreLayoutIfRequiredByEffectL(aToLayout, aEvent, neededStoredBuffers);
				
				// if screenshot was requested and it was possible to take one, then add this layout
				// to effectcleanup stack
                if (neededStoredBuffers)
                    {
                    TInt dummy;
					if (!HasActiveEffect(aToLayout, dummy))
						{
						__ALFFXLOGSTRING2("CAlfBridge::HandleGfxEventL - Add layout 0x%x with handle %d to cleanupstack", aToLayout, aEvent.iHandle);
                    	AddFxItemL(aEvent.iHandle, aToLayout, NULL, NULL, EFalse, dummy, ETrue, EFalse);
						}
                    }
				// Screenshot was requested, but it could not be taken. Lets cancel the effect.	
				if (!success)
					{
					return ETrue; // this will cause effect cleanup
					}	
					      

                switch(aEvent.iAction)
                    {
                    case AknTransEffect::EApplicationActivate:
                    case AknTransEffect::EApplicationStart:
                    case AknTransEffect::EApplicationStartRect:
                    case AknTransEffect::EApplicationStartSwitch:
                    case AknTransEffect::EApplicationStartSwitchRect:
                    case KAlfFSTaskSwapperAppStart:
                        {
                        aToLayout->iOpacity.Set(0.0f);    // these are meant for applications that are not yet ready to be drawn, but possible already on the foreground
                        FreezeLayoutUntilEffectDestroyedL(aFromLayout, aEvent.iHandle);
                        aEvent.iEffectType = CFullScreenEffectState::EStartEffect;
                        if (iFSFXDataPart2)
                            {
                            iFSFXDataPart2->iEffectType = CFullScreenEffectState::EStartEffect; 
                            }
                                
                        aEvent.iCanDestroyOrHideImmediately = ETrue; // enable hiding of windows during application start/activate effects
                        break;
                        }
                    case AknTransEffect::EApplicationExit:
                        {
                        aEvent.iEffectType = CFullScreenEffectState::EExitEffect;
                        // The effect should start when the new view is ready,
                        // but we have no signal telling us that, so we just have to do our best

                        // Effect end observer is given to engine in LoadEffectL
                        // It will be set to effect there and called when the effect ends or is deleted

                        // The layout should be visible at this time. if not, then this is assumed
                        // as effect to an background application and ignored.
						
                        if (aToLayout->Effect() 
							&& !(aToLayout->Effect()->EffectFlags() & KHuiFadeEffectFlag ))                         
							{
							// If the exit effect took a screesnhot (above), then the layout is freezed to show the screenshot. 
							// Layout may not be unfrozen before EndFullScreen, or flicker will occur.
                            // effect on a layout must be an application start effect. 
                            // External content visual is not used for that.
                            __ALFFXLOGSTRING2("CAlfBridge::HandleGfxEventL - Found effect on layout 0x%x. Removing effect 0x%x", aToLayout, aToLayout->Effect());
                            aToLayout->SetEffect(NULL);
                            aToLayout->SetFreezeState(EFalse); // Only if a screenshot was taken earlier then freeze state would be ETrue
                            }
							
                        __ALFFXLOGSTRING2("CAlfBridge::HandleGfxEventL - Layout 0x%x is active:  %d", aToLayout, !TBool(aToLayout->Flags() & EHuiVisualFlagInactive) );
                        if (!(aToLayout->Flags() & EHuiVisualFlagInactive)) 
                            {
                            TInt index;
                            if (aToLayout->StoredRenderBuffer() 
                                    && HasActiveEffect(aToLayout, index) 
                                    && index != KErrNotFound)
                                {
	                            // a screenshot has been saved into the layout, and the layout was added to cleanupstack
    	                        // for cleaning. SetupEffectLayoutContainerL will add the same layout 
        	                    // to the cleanupstack, but double cleaning is not a good idea. Thus it is removed here. 
                                __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEventL - Removing the layout 0x%x with screenshot from iEffectCleanupStack", aToLayout);
                                iEffectCleanupStack.Remove(index);
                                }
                            // this will tag the visual, that they cannot be hidden by HandleVisualVisibility
                            // Initialize layout for the exit effect  
                            iLayoutInitializedForExitEffect = SetupEffectLayoutContainerL(aEvent.iHandle, aToLayout, ETrue, ETrue, aEvent.iCanDestroyOrHideImmediately);
                            __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEventL - EBeginFullscreen - iLayoutInitializedForExitEffect: %d", iLayoutInitializedForExitEffect);
                            aEvent.iSetupDone = iLayoutInitializedForExitEffect; 
                            }
                        else
                            {
                            iLayoutInitializedForExitEffect = EFalse;
                            aEvent.iSetupDone = EFalse;
                            failed = ETrue;
                            }
                        if (iFSFXDataPart2)
                            {
                            iFSFXDataPart2->iSetupDone = aEvent.iSetupDone;
                            }
                         return failed;
                         }
                    default:
                        break;
                    }
                break;
                }
            case MAlfGfxEffectPlugin::EEndFullscreen:
                {
                TBool layoutEffectable(EFalse);
                if (aEvent.iAction != 5000)
                    {
                    if ( aEvent.iAction != AknTransEffect::EApplicationExit && aToLayout->Flags() & EHuiVisualFlagInactive)
                        {
                        aToLayout->iOpacity.Set(1.0f);
                        }
                        
                    if (aToLayout)
                        {
                        // if this was timeout appstart effect, we need to take screenshot at this point. 
                        // It was not taken in the beginfullscreen
                        
                        if (aEvent.iLongAppStartTimeout)
                            {
                            __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEventL  - taking screenshot for the timeout appstart effect. Handle: %d",  aEvent.iHandle );
                            aToLayout->SetStoredRenderBufferModificationsEnabled(ETrue);
                            TBool neededStoredBuffers(EFalse); // dummy 
                            StoreLayoutIfRequiredByEffectL(aToLayout, aEvent, neededStoredBuffers);
                            }
                        
                        aToLayout->iOpacity.Set(1.0f);
                        __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEventL - loading effect, handle %d", aEvent.iHandle );
                        if (aEvent.iAction == AknTransEffect::EApplicationExit)
                            {
                            // Exit effect was initialized earlier with EBeginFullscreen event
						    layoutEffectable = aEvent.iSetupDone;
						    __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEventL - EEndFullScreen: iSetupDone: %d", layoutEffectable);
                            }
                        else
                        	{
                            // add visuals to visual cleanupstack
                            aToLayout->SetStoredRenderBufferModificationsEnabled(EFalse);
                            TInt index;
                            if (aToLayout->StoredRenderBuffer() 
                                    && HasActiveEffect(aToLayout, index) 
                                    && index != KErrNotFound
                                    && aEvent.iTwoPhaseEffect != CFullScreenEffectState::ESecondPartActive // effects second phase
                                    )
                                {
	                            // a screenshot has been saved into the layout, and the layout was added to cleanupstack
    	                        // for cleaning. SetupEffectLayoutContainerL will add the same layout 
        	                    // to the cleanupstack, but double cleaning is not a good idea. Thus it is removed here. 
                                __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEventL - Removing the layout 0x%x with screenshot from iEffectCleanupStack", aToLayout);
                                iEffectCleanupStack.Remove(index);
                                }
                            if (aEvent.iTwoPhaseEffect == CFullScreenEffectState::ESecondPartActive || aEvent.iSetupDone)
                                {
                                // two phase effect, setup has been made with the first part of the effect. 
                                // Only changing the effect is required.
                                layoutEffectable = ETrue; 
                                }
                            else
                                {
                                layoutEffectable = SetupEffectLayoutContainerL(aEvent.iHandle,aToLayout, ETrue, EFalse, aEvent.iCanDestroyOrHideImmediately);
                                }
                            aEvent.iSetupDone = layoutEffectable; 
                            aToLayout->SetStoredRenderBufferModificationsEnabled(ETrue);
                            }

                        if (layoutEffectable)
                            {
                            TInt effectFlags = 0;
                            TInt activeEffectGroup = engine->ActiveGroupEffect();
                            if (activeEffectGroup != KErrNotFound)
                                {
                                engine->AddEffectToGroup(activeEffectGroup);
                                }
                            __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEventL - Active effect group: %d", activeEffectGroup);
                            effectFlags = KHuiFxDelayRunUntilFirstFrameHasBeenDrawn;
                            if (FxmlHasOpaqueHint(iHuiEnv->EffectsEngine(), *aEvent.iEffectName) ||
                                    aEvent.iTwoPhaseEffect == CFullScreenEffectState::ESecondPartActive)
                                {
                                effectFlags |= KHuiFxOpaqueHint;
                                }
                            if (iFSFXDataPart2)
                                {
                                iFSFXDataPart2->iSetupDone = ETrue;
                                }
                            
                            if (aEvent.iRect != TRect())
                                {
                                TRAP( err, engine->LoadGroupEffectL( *aEvent.iEffectName, effect, aToLayout->Effectable(), activeEffectGroup, &aEvent.iRect, this, aEvent.iHandle, effectFlags ));
                                }
                            else
                                {
                                TRAP( err, engine->LoadGroupEffectL( *aEvent.iEffectName, effect, aToLayout->Effectable(), activeEffectGroup, NULL, this, aEvent.iHandle, effectFlags ) );
                                }
                            effect = NULL;
                            // only use the effect if the effect file was correctly parsed
                            aToLayout->SetFreezeState(EFalse);
                                                            
                            if (err != KErrNone)
                                {
                                // visuals added to "active effect visual" stack, but they wont be used, because effect loading has failed. cleanup.
                                CleanFxVisual(NULL, aEvent.iHandle);
                                aToLayout->SetEffect(NULL);
                                failed = ETrue;
                                }
                            else
                                {
                                switch(aEvent.iTwoPhaseEffect)
                                    {
                                    case CFullScreenEffectState::EFirstPartActive:
                                        aEvent.iTwoPhaseEffect = CFullScreenEffectState::EFirstPartRunning;
                                        break;
                                    case CFullScreenEffectState::ESecondPartActive:
                                        aEvent.iTwoPhaseEffect = CFullScreenEffectState::ESecondPartRunning;
                                        break;
                                    }
                                }
                            }
                        else
                            {
                            failed = ETrue;                            
                            __ALFFXLOGSTRING2("CAlfBridge::HandleGfxEventL - Effect layout initialization failed. Skipping effect handle: %d, Visual: 0x%x", aEvent.iHandle, aToLayout);
                            }
                        }
                    }
                break;
                }
            default:
                {
                // just in case...
                break;
                }
            }
        }
    
    iAlfScreens[0]->SetVisualTreeVisibilityChanged(ETrue);
    return failed;
    }

void CAlfBridge::FreezeLayoutUntilEffectDestroyedL(CHuiLayout* aLayout, TInt aHandle)
    {
    if (aLayout)
        {
		TRAPD(err, StoreRenderBufferStartL(aLayout));
		__ALFFXLOGSTRING3("CAlfBridge::FreezeLayoutUntilEffectDestroyed - Storing SecureId x%x, StoreRenderBufferStartL call returned: %d for layout 0x%x", aLayout->Owner().ControlGroup()->SecureId(), err, aLayout);
		if (err == KErrNone)
			{
            // Freeze only, if buffer was reserved succesfully 
            aLayout->SetFreezeState(ETrue);
        	TBool itemsDestroyed;
    	    AddFxItemL(aHandle, aLayout, NULL, NULL, EFalse, itemsDestroyed, ETrue, EFalse);
	        iFSFxData->iAppStartScreenshotItemHandle = aHandle;
			}
        }
    }

void CAlfBridge::HandleGfxStopEvent( TBool aClientRequest )
    {
    __ALFFXLOGSTRING("CAlfBridge::HandleGfxStopEvent BEGIN");
    if (!iFSFxData)
        {
        __ALFFXLOGSTRING("CAlfBridge::HandleGfxStopEvent - END: none");
        CleanAllFxVisuals();
        delete iControlEffectData;
        iControlEffectData = NULL;
        return;
        }
    
    CFullScreenEffectState* fxData = iFSFxData;
    iFSFxData = NULL;
    CleanupStack::PushL( fxData );
    
     __ALFFXLOGSTRING2("CAlfBridge::HandleGfxStopEvent - handle: %d, toAppUid: 0x%x", fxData->iHandle, fxData->iToAppId);
    
    // clean effects with this handle       
    CleanFxVisual(NULL, fxData->iHandle);
    // this was abort, so we might have received earlier event hiding this window. Here we'll bring it 
    // back
    if (fxData->iToAppId && fxData->iToAppId != KErrNotFound)
        {
        RemoveEffectFromApp(fxData->iToSecureId, fxData->iToWg );
        RemoveEffectFromApp(fxData->iFromSecureId, fxData->iFromWg );
        }
    // abort ALL other possible control effects
    CleanAllFxVisuals();
    
    delete iControlEffectData;
    iControlEffectData = NULL;
    
    iHuiEnv->ContinueRefresh();
    
    if ( !aClientRequest )
        {
        // Aborted errornously, signal client side
        RProperty::Set( KPSAlfDomain, KAlfTransitionStatus, fxData->iCompletionHandle );
        }
    
    CleanupStack::PopAndDestroy( fxData );
    delete iFSFXDataPart2;
    iFSFXDataPart2 = NULL;
    __ALFFXLOGSTRING("CAlfBridge::HandleGfxStopEvent - END");
    }

void CAlfBridge::RemoveEffectFromApp(TInt aSecureId, TInt aWgId )
    {
    if(aSecureId == 0)
        {
        return;
        }
    CHuiControlGroup* group = NULL;
    if ( aWgId > 0 )
        {
        // check with the specific window gruop
        group = FindControlGroupBySecureId(aSecureId, aWgId);
        }
    else 
        {
        // any window gruop will do
        group = FindControlGroupBySecureId(aSecureId);
        }
    
    if (group)
        {
        CHuiControl& control = group->Control(0);
        CHuiLayout* layout = (CHuiLayout*)&control.Visual(0);
    
        TInt handle = FindEffectHandle(layout);
        if (handle != KErrNotFound)
            {
            __ALFFXLOGSTRING2("CAlfBridge::HandleGfxStopEvent - layout visual: 0x%x ; handle: %d ", layout, handle);
            CleanFxVisual(NULL, handle);
            }
        layout->iOpacity.Set(1.0f); 
        }
    }

// ---------------------------------------------------------------------------
// HandleGfxEffectsL
// ---------------------------------------------------------------------------
//

void CAlfBridge::HandleGfxEffectsL( TAlfBridgerData data )
    {
    // This method is called when "Begin fullscreen" and "End fullscreen" are
    // invoked. Data in "Begin fullscreen" is used, "End fullscreen" and
    // "Abort fullscreen" (HandleStopGfxEffectsL) are just signals.
    // As a general limitation, there can be exactly one fullscreen effect
    // ongoing at once.
    
    TInt action = data.iInt1;
    TInt length = data.iInt2;
    __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEffectsL - BEGIN (bridge data %d)", (TInt)data.iPtr);
    void* bridgeBuffer = (void*) (*iHost)->GetEffectsDataL( (TInt)data.iPtr );    
    RMemReadStream stream( bridgeBuffer, length );
    
    // operation tells if this is begin full screen or end full screen
    TInt operation = stream.ReadInt32L();
    
    // If we are in low memory state, we don't want to do any effects
    if(iHuiEnv->MemoryLevel() < EHuiMemoryLevelReduced)
        {
        __ALFFXLOGSTRING("CAlfBridge::HandleGfxEffectsL - Returning because memory level below NORMAL");
		stream.Release();
        return;
        }
     
    if ( operation == MAlfGfxEffectPlugin::EBeginFullscreen )
        {
        if ( !iFSFxData || !iFSFxData->iSetupDone )
            {
            // No fullsceen effect ongoing or fullscreen effect hasn't yet been set up, 
            // so we can initialize iFSFxData from stream.
            
            __ALFFXLOGSTRING("HandleGfxEffectsL - fresh start");
            
            CFullScreenEffectState* fxData = new (ELeave) CFullScreenEffectState;
            CleanupStack::PushL( fxData );
            fxData->ConstructL( action, stream );
            CleanupStack::Pop( fxData );
        	
			// the effect handle of freezed layout (iAppStartScreenshotItemHandle) must match the actual
			// effect handle that is run. Only then frozen application can be freed at the end of the effect
			if (iFSFxData)
			{
            	for (TInt i = 0; i < iEffectCleanupStack.Count(); i++)
            	{
               	 TEffectCleanupStruct& effectItem = iEffectCleanupStack[i];
               	 if (iFSFxData->iAppStartScreenshotItemHandle == effectItem.iHandle)
               	     {
               	     effectItem.iHandle = fxData->iHandle;
               	     }
               	}
			}
            
            if (iFSFxData && iFSFxData->iToAppId != fxData->iToAppId)
                {
				__ALFFXLOGSTRING("CAlfBridge::HandleGfxEffectsL - WARNING - Another fullscreen effect to different app. Deleting the previous ongoing effect"); 
                RemoveEffectFromApp(iFSFxData->iToSecureId, iFSFxData->iToWg);// Fullscreen effect for another app has arrived, and the previous has not finished and abort effect was not called.
                RemoveEffectFromApp(iFSFxData->iFromSecureId, iFSFxData->iFromWg);
                __ALFFXLOGSTRING2("CAlfBridge::HandleGfxEffectsL - Effect request has changed from appUid 0x%x to 0x%x. Cancel previous effect.", iFSFxData->iToAppId, fxData->iToAppId);
                RemoveEffectFromApp(iFSFxData->iToAppId);
                // Fullscreen effect for another 
                }

               delete iFSFxData;
               delete iFSFXDataPart2;
               iFSFXDataPart2 = NULL;
            iFSFxData = fxData;
            iFSFxData->iAppStartScreenshotItemHandle = fxData->iHandle;
            
            stream.Release();
            }
        else
            {
            // Fullscreen effect is ongoing. We could either use new data
            // and abort the ongoing effect, or just continue ongoing.
            // For now, we continue ongoing.
            
            __ALFFXLOGSTRING("HandleGfxEffectsL - END: ongoing");
            
            // Client expects completion to be signalled with this handle.
            iFSFxData->iCompletionHandle = stream.ReadInt32L();
            if (iFSFXDataPart2)
                {
                // Two phase effect path has been chosen. First part is now ongoing, and second part will be 
                // triggered after that. Client has now notified, that its screen is ready. We may trigger the 
                // second effect as soon as the first part finishes.
                __ALFFXLOGSTRING("CAlfBridge::HandleGfxEffectsL - Trigger second part. when 1st part done.");
                iFSFXDataPart2->iCompletionHandle = iFSFxData->iCompletionHandle;
                }
            
            stream.Release();           
            return ; 
            }
        }
    else // MAlfGfxEffectPlugin::EEndFullscreen
        {
        // End fullscreen signal received. We proceed only if there is really
        // effect ongoing and "end fullscreen" hasn't already been processed.
        
        if ( !iFSFxData 
                || iFSFxData->State() == EFinalEffectActive
                || iFSFxData->State() == EEndFullscreenReceived)
            {
            if (iFSFxData)
            __ALFFXLOGSTRING1("CAlfBridge::HandleGfxEffectsL - END: fx ready, state %d ", iFSFxData->State());
            stream.Release();
            return;
            }
        CFullScreenEffectState* fxData = new (ELeave) CFullScreenEffectState;
        CleanupStack::PushL( fxData );
        fxData->ConstructL( KErrUnknown, stream );
        
        TBool timeout = fxData->iTimeout;
        // could the application be ready?
        TBool applicationExists = FindControlGroupByFullScreenToEffect() != NULL ? ETrue : EFalse;

        // It may be, that application exited already and it was moved already to effect group
        if (iFSFxData->iEffectType == CFullScreenEffectState::EExitEffect && !applicationExists )
            {
            applicationExists = FindLayoutByEffectHandle(fxData->iHandle) != NULL ? ETrue : EFalse;
            }
                
        TBool longAppStartTimeout = fxData->iLongAppStartTimeout; 
        
        if (iFSFxData &&
                !iFSFXDataPart2 && // second part has not been requested yet
                longAppStartTimeout && // this is indeed timeout for app start effect
                iFSFxData->iTwoPhaseEffect == CFullScreenEffectState::EOnlyOnePart && // second part has not been activated. note, that iFSFXDataPart2 would be NULL in this case 
                (iFSFxData->State() == EWaitEndFullScreen 
                 || iFSFxData->State() == EWaitingWindowGroup))
            {
            // Load the whole structure. If timeout was triggered then request effect name to be
            // updated.
            UpdateSecondaryEffectL(*fxData);
            iFSFxData->iLongAppStartTimeout = fxData->iLongAppStartTimeout;
            iFSFxData->iTwoPhaseEffect = CFullScreenEffectState::EFirstPartActive;
            iFSFxData->iOperation = MAlfGfxEffectPlugin::EEndFullscreen;
            }

        __ALFFXLOGSTRING4("HandleGfxEffectsL - iTwoPhaseEffect %d, Setup done: %d, Timeout: %d, long app start timeout: %d", iFSFxData->iTwoPhaseEffect,iFSFxData->iSetupDone, timeout,longAppStartTimeout);
        if (!fxData->iLongAppStartTimeout) // endfullscreen originating from the application
            {
            if (iFSFxData->State() == EWaitingWindowGroup)
                {
                iFSFxData->SetState(EEndFullScreenReceivedWaitingWindowGroup);
                }
            else
                {
                iFSFxData->SetState(EEndFullscreenReceived);
                }
            
            if (iFSFXDataPart2)
                {
                iFSFXDataPart2->SetState(EEndFullscreenReceived);
                }
            
            }
        CleanupStack::PopAndDestroy( fxData );
        fxData = NULL;
        
        __ALFFXLOGSTRING4("CAlfBridge::HandleGfxEffectsL - Regular timeout: %d, Slow start timeout: %d, Effect setup: %d, State: %d",
                timeout,
                longAppStartTimeout,
                iFSFxData->iSetupDone,
                iFSFxData->State());

        stream.Release();// We should respect normal timeouts.
        if (iFSFxData->iTwoPhaseEffect > CFullScreenEffectState::EOnlyOnePart
                && iFSFxData->iSetupDone && timeout && longAppStartTimeout) // timeout = endfullscreen was requested or it was normal endfullscreen timeout, longAppStartTime = 
            {
            // If this is two phased effect, it can be started only by application request of endfullscreen (!timeout)
            // or screen drawn (triggered from 
            __ALFFXLOGSTRING("HandleGfxEffectsL - Two phase effect. Waiting second part to start. Skip this event.");
            return;
            }
        
        // If client triggered EndFullScreen has been received, and still we haven't seen sign of the application, we know
        // things have gone wrong and will not go right. Clean up the mess.
        if (iFSFxData->State() == EEndFullscreenReceived && !applicationExists && !timeout)
            {
            HandleGfxStopEvent(ETrue);
            return;
            }
	    // If a long two part app start effect is ongoing or about to start, we can skip directly to the 
		// second part when EndFullScreen request arrives.
        if (iFSFXDataPart2 && iFSFxData 
				&& applicationExists
                && iFSFXDataPart2->State() == EEndFullscreenReceived 
                && iFSFxData->iHandle == iFSFXDataPart2->iHandle )
            {
            AlfGfxEffectEndCallBack(iFSFxData->iHandle);
            return;
            }
        }

    CFullScreenEffectState* fxData = iFSFxData;
    fxData->iOperation = operation;

    __ALFFXLOGSTRING3("CAlfBridge::HandleGfxEffectsL - Operation: %d, handle: %d, type: %d", operation, fxData->iHandle, fxData->iType);
    __ALFFXLOGSTRING2("CAlfBridge::HandleGfxEffectsL - towg: %d, fromwg: %d", fxData->iToWg, fxData->iFromWg);

    CHuiControlGroup* toGroup = NULL;
    
    TBool failed = EFalse;
    if (fxData->iToAppId && fxData->iToAppId != KErrNotFound)
        {        
        CHuiLayout* toLayout = NULL; 
        toGroup = FindControlGroupByFullScreenToEffect();

        CHuiControlGroup* fromGroup = NULL;
        CHuiLayout* fromLayout = NULL; 
        fromGroup = FindControlGroupByFullScreenFromEffect();

        if (!toGroup) // group has been destroyed and moved to effectControlGroup for waiting the application exit effect EndFullScreen trigger 
            {
            toLayout = FindLayoutByEffectHandle(fxData->iHandle);
            }
        else
            {
            CHuiControl& control = toGroup->Control(0);
            toLayout = (CHuiLayout*)&control.Visual(0);
            }
        if (fromGroup)
            {
            CHuiControl& control2 = fromGroup->Control(0);
            fromLayout = (CHuiLayout*)&control2.Visual(0);
            }
        
        if (toLayout)
            {
            failed = HandleGfxEventL(*fxData, toLayout, fromLayout);
            // We found layout, and this was beginfullscreen event. Wait for endfullscreen to be triggered
            if (fxData->iOperation == MAlfGfxEffectPlugin::EBeginFullscreen)
                {
                iFSFxData->SetState(EWaitEndFullScreen);
                }
            }
        else
            {
            __ALFFXLOGSTRING("HandleGfxEffectsL - waiting window group");            
            fxData->SetState(EWaitingWindowGroup);
            }
        }

    if ( failed )
        {
        // Effect failed, reset state
        HandleGfxStopEvent( EFalse ); // destroys iFSFxData and iFSFXDataPart2
        }
    else
        {
        // it might be that the application is already having drawing for the whole screen. 
		// If so, then trigger EndFullScreen immediately.
        if (fxData && 
                toGroup 
                && fxData->iOperation == MAlfGfxEffectPlugin::EBeginFullscreen 
                && fxData->iEffectType == CFullScreenEffectState::EStartEffect)
            {
            GfxTriggerEffectWhenFullScreenDrawn(toGroup);
            }
        }
    
    for (TInt i = 0; i < iAlfScreens.Count(); i++)
        {
        iAlfScreens[i]->iDisplay->SetDirty();
        }
    iAlfScreens[0]->iVisualTreeVisibilityChanged = ETrue;
    iHuiEnv->ContinueRefresh();

    __ALFFXLOGSTRING("CAlfBridge::HandleGfxEffectsL - END");
    }

TBool CAlfBridge::UpdateSecondaryEffectL(const CFullScreenEffectState& aState)
    {
    
    __ALFFXLOGSTRING1("CAlfBridge::UpdateSecondaryEffectL - appstart timeout triggered: %d", aState.iLongAppStartTimeout);
    if (aState.iLongAppStartTimeout) // only application start effects can trigger this timeout 
        {
        // clone the primary startup effect for later usage
        delete iFSFXDataPart2;
        iFSFXDataPart2 = new (ELeave) CFullScreenEffectState;
        iFSFXDataPart2->ConstructL(*iFSFxData);
        iFSFXDataPart2->iLongAppStartTimeout = EFalse;
        
        delete iFSFxData->iEffectName;
        iFSFxData->iEffectName = NULL;
        iFSFxData->iEffectName = aState.iEffectName->AllocL();
        }
    __ALFFXLOGSTRING("CAlfBridge::UpdateSecondaryEffectL <<" );
    return EFalse;
    }    

TBool CAlfBridge::GfxTriggerEffectWhenFullScreenDrawn(CHuiControlGroup* aToGroup)
    {
    if (!aToGroup || (aToGroup && aToGroup->Count() == 0))
        {
        return EFalse;
        }
    iTempRegion.Clear();
    
    CAlfScreen* screen = iAlfScreens[0];
    TRect fullscreen = TRect(TPoint(0,0), screen->Size());
    TBool fullscreenCovered(EFalse);
    TInt dummy = 0;
    
    CHuiControl& control = aToGroup->Control(0);
    CHuiLayout* toLayout = control.VisualCount() > 0 ? (CHuiLayout*)&control.Visual(0) : NULL;
    if (!toLayout)
        {
        // no point starting effect, if the layout is still inactive.
        return EFalse;
        }
    TBool opaque = ETrue; // in app start case the layout is propably not opaque. Fake it.
    IsFullScreenDrawnRecursive(toLayout, *aToGroup, control, fullscreenCovered, fullscreen, screen, dummy, opaque, iAlfScreens[0]->iDisplay->Orientation());
    fullscreenCovered = IsRectCoveredByRegion(fullscreen, iTempRegion);
    // alf content check is relavant only for alf apps
    if (!fullscreenCovered && aToGroup->iAlfApp) 
        {
        TInt clientGroupId = FindClientWindowGroupId(0, *aToGroup);
        if (clientGroupId != KErrNotFound)
            {
            fullscreenCovered = HasActiveAlfContent(clientGroupId);
            }
        }
    
    __ALFFXLOGSTRING2("CAlfBridge::GfxTriggerEffectWhenFullScreenDrawn - SecureId: 0x%x, Covered: %d", aToGroup->SecureId(), fullscreenCovered);
    if (fullscreenCovered)
        {
        if (iFSFxData->iTwoPhaseEffect == CFullScreenEffectState::EFirstPartRunning)
            {
            // effect has two parts, and the first one is ongoing. We'll skip to the second part. 
            delete iFSFxData;
            iFSFxData = iFSFXDataPart2;
            iFSFXDataPart2 = NULL;
            iFSFxData->iTwoPhaseEffect = CFullScreenEffectState::ESecondPartActive;
             // Client notified of the EndFullScreen during the effect. Trigger the end part immediately.
             __ALFFXLOGSTRING("CAlfBridge::AlfGfxEffectEndCallBack - End was received. Trigger second part.");
             GfxTriggerEndFullScreen(iFSFxData);
             iFSFxData->SetState(EFinalEffectActive);
            }
        else
            {
            GfxTriggerEndFullScreen(iFSFxData); // trigger effect
            }
        return ETrue;
        }
    return EFalse;
    }

// TODO: add effect type to effect struct. remove all fullscreen effects. currently some might hang around
void CAlfBridge::HandleGfxStopEffectsL(TAlfBridgerData data)
    {
    // Fullscreen effect is aborted. Clear the state. 
    
    //TInt action = data.iInt1;
    TInt length = data.iInt2;
    __ALFFXLOGSTRING1("CAlfBridge::HandleGfxStopEffectsL - Reading bridge data %d", (TInt)data.iPtr);
    void* bridgeBuffer = (void*) (*iHost)->GetEffectsDataL( (TInt)data.iPtr );    
    RMemReadStream stream( bridgeBuffer, length );
    
    TInt operation = stream.ReadInt32L();
    TInt handle = stream.ReadInt32L();
    TInt type = stream.ReadInt32L();
    TInt wg1 = stream.ReadInt32L();
    TInt wg2 = stream.ReadInt32L();
    TInt toAppUi = stream.ReadInt32L();
    stream.Release();

    // operation tells if this is abort full screen or something else
    // When this function is used to abort control transitions, all data is 0s
    if ( iFSFxData )
        {
        __ALFFXLOGSTRING("CAlfBridge::HandleGfxStopEffectsL - kill fullscreen");        
        HandleGfxStopEvent( ETrue );
        }
    
    if ( !toAppUi )
        {
        __ALFFXLOGSTRING("CAlfBridge::HandleGfxStopEffectsL - kill all");
        CleanAllFxVisuals();
        iHuiEnv->ContinueRefresh();
        }
    __ALFFXLOGSTRING("CAlfBridge::HandleGfxStopEffectsL - end");
    }

void CAlfBridge::HandleGfxStopControlEffectsL(TAlfBridgerData data)
    {
    TInt length = data.iInt2;
    __ALFFXLOGSTRING1("CAlfBridge::HandleGfxStopControlEffectsL - Reading bridge data %d", (TInt)data.iPtr);
    void* bridgeBuffer = (void*) (*iHost)->GetEffectsDataL( (TInt)data.iPtr );    
    RMemReadStream stream( bridgeBuffer, length );
    
    TInt operation = stream.ReadInt32L();
    TInt handle = stream.ReadInt32L();
    stream.Release();

    delete iControlEffectData;
    iControlEffectData = NULL;
        
    if (handle != KErrNotFound)
        {
        __ALFFXLOGSTRING1("CAlfBridge::HandleGfxStopControlEffectsL - Remove effect with handle: %d", handle);
        CleanFxVisual(NULL, handle);
        iHuiEnv->ContinueRefresh();
        }
    __ALFFXLOGSTRING("CAlfBridge::HandleGfxStopControlEffectsL - end");
    }

void CAlfBridge::HandleSetDistractionWindowL( TAlfBridgerData data )
    {
    TInt windowGroupId = data.iInt1;
    TInt windowClientHandle = data.iInt2;
    TBool setDistractionWindow = (TBool)data.iPtr;
        
    CControlEffectState* fxData = new (ELeave) CControlEffectState;
    CleanupStack::PushL(fxData);
    fxData->ConstructL(windowClientHandle, windowGroupId, setDistractionWindow);

    __ALFFXLOGSTRING2("CAlfBridge::HandleSetDistractionWindowL client handle: %d, group: %d",  windowClientHandle, windowGroupId );
    CHuiCanvasVisual* visual = FindVisualByClientSideIds(windowClientHandle, windowGroupId);
    
    iControlEffectData = fxData;
    CleanupStack::Pop(fxData);
    
    if (visual)
        {
        HandleGfxControlEventL(*iControlEffectData, visual);
        }
    else
        {
        __ALFFXLOGSTRING2("CAlfBridge::HandleSetDistractionWindowL - Control not found. iClientHandle %d, iClientGroupHandle %d. Cache request.", 
                fxData->iClientHandle, 
                fxData->iClientGroupHandle);
        return;
        }
    delete iControlEffectData;
    iControlEffectData = NULL;
    }
void CAlfBridge::HandleRegisterEffectL( TAlfBridgerData data )
    {     
    TInt action = data.iInt1;
    TInt length = data.iInt2;    
    
    void* bridgeBuffer = (void*) (*iHost)->GetEffectsDataL( (TInt)data.iPtr );    
    RMemReadStream stream( bridgeBuffer, length );
  
   
    HBufC* effectDirectory = HBufC::NewL( stream, 256 );
    CleanupStack::PushL( effectDirectory );
    HBufC* effectFile = HBufC::NewL( stream, 256 );
    CleanupStack::PushL( effectFile );
    HBufC* effectFullName = HBufC::NewL( effectDirectory->Des().Length() + effectFile->Des().Length() );
    CleanupStack::PushL( effectFullName );
    effectFullName->Des().Copy( *effectDirectory );
    effectFullName->Des().Append( *effectFile );  
    
    stream.Release();
    __ALFFXLOGSTRING2("CAlfBridge::HandleRegisterEffectL - dir: %S, file: %S", effectDirectory, effectFile);
    DoRegisterEffectL(*effectFullName, action);
    
    if (action == KFadeAction)
        {
        DoSetCachedFadeEffectL();
        }
    
    CleanupStack::PopAndDestroy(effectFullName);
    CleanupStack::PopAndDestroy(effectFile);
    CleanupStack::PopAndDestroy(effectDirectory);
    }

void CAlfBridge::DoRegisterEffectL(const TDesC& aFilename, TInt aAction)
    {
    TRegisteredEffectsStruct newEntry;
    newEntry.iAction = aAction;
    newEntry.iEffectFile = HBufC::NewL(aFilename.Length());
    newEntry.iEffectFile->Des().Append(aFilename);
    iAlfRegisteredEffects.AppendL(newEntry);

    CHuiFxEngine* engine = NULL;
    engine = iHuiEnv->EffectsEngine();
    if (engine)
        {
        engine->RegisterEffectL(aFilename);
        }
    }

CHuiCanvasVisual* CAlfBridge::AddFxItemL(
        TInt aEffectHandle, 
        CHuiVisual* aSourceVisual, 
        CHuiLayout* aTargetLayout, 
        CHuiControl* aEffectControl, 
        TBool aInsertTemporaryVisual,
        TInt& aItemDestroyed,
        TBool aIsFullScreenEffect,
        TBool aIsExitEffect,
        TBool aCanDestroyOrHideImmediately 
        )
    {
    CHuiCanvasVisual* temporaryPresenterVisual = NULL;
    TInt enableEffect = ETrue;
    
    if (aSourceVisual->iOpacity.Now() >= 0.01 && !(aSourceVisual->Flags() & EHuiVisualFlagShouldBeHidden))
        {
        aSourceVisual->SetFlag(EHuiVisualFlagShouldBeShown);
        }
    
    aSourceVisual->ClearFlags(EHuiVisualFlagShouldBeInactive | EHuiVisualFlagShouldBeUnderOpaqueHint);
    if (aIsExitEffect) 
        {
        aSourceVisual->SetFlag(EHuiVisualFlagAlwaysDraw);   // forces drawing during exit effect. handlevisualvisibility will set the EShouldbeInactive flag, if visual is determined inactive while this flag is set 
        }
    
    if (HasActiveEffect(aSourceVisual))
        {
        // this visual is having effect and we are closing down the layout this visual inhabits.
        // Propably this is the case, when application is closed while the options menu is 
        // still having effect animation.
    
        // removes the effect, and the visual (if EShouldDestroy flag enabled). Removes from effect
        // cleanup stack iEffectCleanupStack
        enableEffect = aSourceVisual->Flags() & EHuiVisualFlagShouldDestroy ? 0 : 1;
        __ALFFXLOGSTRING2("CAlfBridge::SetupEffectLayoutContainerL - visual 0x%x is having effect. EShouldBeDestroyed flag state %d", &aSourceVisual, enableEffect );
        CleanFxVisual(aSourceVisual);
        if (!enableEffect)
            {
            aItemDestroyed++;
            }
        }
    if ( enableEffect )
        {
    // RDebug::Print(_L("CAlfBridge::SetupEffectLayoutContainerL - binding visual 0x%x (source)--> 0x%x (presenter) layout 0x%x (presenter layout)"), &sourceVisual, temporaryPresenterVisual, temporaryPresenterLayout);
        if (aInsertTemporaryVisual)
            {
            temporaryPresenterVisual = CHuiCanvasVisual::AddNewL(*aEffectControl, aTargetLayout);
        
            temporaryPresenterVisual->SetExternalContentL(aSourceVisual);
            temporaryPresenterVisual->SetSize(THuiRealSize(640, 640));
            temporaryPresenterVisual->SetPos(aSourceVisual->Pos().Target());
            aSourceVisual->SetFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
            temporaryPresenterVisual->SetChanged();
            TEffectCleanupStruct item = TEffectCleanupStruct(
                    aEffectHandle,
                    aSourceVisual, 
                    temporaryPresenterVisual,
                    ETrue,
                    aIsExitEffect,
                    aCanDestroyOrHideImmediately,
                    aIsFullScreenEffect);
            iEffectCleanupStack.AppendL(item);
            }
        else
            {
            // this will be a child of another visual, and draw by its parent external content visual
            // aSourceVisual->SetFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
            TEffectCleanupStruct item = TEffectCleanupStruct(aEffectHandle,
                        aSourceVisual, NULL, EFalse,
                        aIsExitEffect,
                        aCanDestroyOrHideImmediately,
                        aIsFullScreenEffect);
            iEffectCleanupStack.AppendL(item);
            }
     }
    return temporaryPresenterVisual;
    }

void CAlfBridge::AddToEffectLayoutContainerL(
        TInt aEffectHandle,
        CHuiLayout* aSourceLayout, 
        CHuiLayout* aTargetLayout, 
        CHuiControl* aEffectControl,
        TInt& aItemsDestroyed,
        TBool aIsFullScreenEffect,
        TBool aAddLayout,
        TBool aIsExitEffect,
        TBool aCanDestroyOrHideImmediately)
    {
    __ALFFXLOGSTRING2("CAlfBridge::AddToEffectLayoutContainerL 0x%x is having %d children", aSourceLayout, aSourceLayout->Count());
    if (aAddLayout)
        {
        AddFxItemL(aEffectHandle, aSourceLayout, aTargetLayout, aEffectControl, EFalse, aItemsDestroyed, aIsFullScreenEffect, aIsExitEffect, aCanDestroyOrHideImmediately);
        }
        
    for (TInt i = 0; i < aSourceLayout->Count(); i++)
           {
           CHuiCanvasVisual& sourceVisual = static_cast<CHuiCanvasVisual&> (aSourceLayout->Visual(i));
           
           if (sourceVisual.Count())
               {
               CHuiLayout& layout = static_cast<CHuiLayout&> (aSourceLayout->Visual(i));
               AddToEffectLayoutContainerL(aEffectHandle, &layout, NULL, aEffectControl, aItemsDestroyed, aIsFullScreenEffect,  EFalse, aIsExitEffect, aCanDestroyOrHideImmediately );
               }
           TInt oldItemsDestroyed = aItemsDestroyed;
           AddFxItemL(aEffectHandle, &sourceVisual, aTargetLayout, aEffectControl, EFalse, aItemsDestroyed, aIsFullScreenEffect, aIsExitEffect, aCanDestroyOrHideImmediately);
           if (oldItemsDestroyed != aItemsDestroyed)
               {
               // Visual was destroyed. If so, then we must adjust index.
               --i;
               }
           }
    __ALFFXLOGSTRING2("CAlfBridge::AddToEffectLayoutContainerL 0x%x end of children", aSourceLayout, aSourceLayout->Count());
    }

TBool CAlfBridge::SetupEffectLayoutContainerL(TInt aHandle,CHuiLayout* aSourceLayout, TBool aIsFullScreenEffect, TBool aIsExitEffect, TBool aCanDestroyOrHideImmediately)
    {
    __ALFFXLOGSTRING4("CAlfBridge::SetupEffectLayoutContainerL - aHandle: %d, aSourceLayout: 0x%x, aIsExitEffect: %d, CanDstrOrHideImd.:%d >>", 
            aHandle, 
            aSourceLayout, 
            aIsExitEffect,
            aCanDestroyOrHideImmediately );
    
    if (aSourceLayout->Flags() & EHuiVisualFlagDrawOnlyAsExternalContent)
        {
        // the requested visual is already having an effect. 
        __ALFFXLOGSTRING("CAlfBridge::SetupEffectLayoutContainerL - WE ARE ALREADY HAVING EFFECT! WHAT TO DO??");
        // layout is having already effect. this is tricky. This might be place to panic. Resolve later. 
        }
    // duplicate the layout to effect window group
    CHuiControl& effectControlGroup =  iAlfScreens[0]->iFullscreenEffectControlGroup->Control(0);
    CHuiLayout& effectControlGroupLayout = (CHuiLayout&) effectControlGroup.Visual(0);

    // CHuiCanvasVisual* temporaryPresenterLayout = CHuiCanvasVisual::AddNewL( effectControlGroup, &effectControlGroupLayout);
    // create presenter visual and set bindings
    TInt itemsDestroyed(0);
    AddToEffectLayoutContainerL(aHandle, aSourceLayout, NULL, &effectControlGroup, itemsDestroyed, aIsFullScreenEffect, EFalse, aIsExitEffect, aCanDestroyOrHideImmediately);

    if (aIsExitEffect)
        {
        CHuiCanvasVisual* temporaryPresenterVisual = AddFxItemL(aHandle, aSourceLayout, &effectControlGroupLayout, &effectControlGroup, ETrue, itemsDestroyed, aIsFullScreenEffect, aIsExitEffect, EFalse);
        aSourceLayout->SetFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
        __ALFFXLOGSTRING3("CAlfBridge::SetupEffectLayoutContainerL - adding handle: %d, 0x%x (source layout)-> 0x%x (presenter layout) to iEffectCleanupStack", aHandle, aSourceLayout, temporaryPresenterVisual);
        }
    else
        {
        AddFxItemL(aHandle, aSourceLayout, NULL, &effectControlGroup, EFalse, itemsDestroyed, aIsFullScreenEffect, EFalse, EFalse);
        __ALFFXLOGSTRING2("CAlfBridge::SetupEffectLayoutContainerL - adding handle: %d, 0x%x (source layout), NO presenter layout) to iEffectCleanupStack", aHandle, aSourceLayout);
        }
    iAlfScreens[0]->iVisualTreeVisibilityChanged = ETrue;
    iAlfScreens[0]->iDisplay->SetDirty();
    if (itemsDestroyed)
        {
        __ALFFXLOGSTRING3("CAlfBridge::SetupEffectLayoutContainerL - trying to put effect on destroyed visuals. Skipping effect visual: 0x%x, handle %d, %d items destroyed", aSourceLayout, aHandle, itemsDestroyed);
        }
    __ALFFXLOGSTRING("CAlfBridge::SetupEffectLayoutContainerL");
    return ETrue; // TBool(!itemsDestroyed);
    }

void CAlfBridge::HandleGfxControlEffectsL( TAlfBridgerData data )
    {
    TInt action = data.iInt1;
    TInt length = data.iInt2;
    __ALFFXLOGSTRING1("HandleGfxControlEffectsL - Reading bridge data %d", (TInt)data.iPtr);
    void* bridgeBuffer = (void*) (*iHost)->GetEffectsDataL((TInt) data.iPtr);
    RMemReadStream stream(bridgeBuffer, length);
    if (iControlEffectData)
        {
        delete iControlEffectData;
        iControlEffectData = NULL;
        }
    
    // If we are in low memory state, we don't want to do any effects
    if(iHuiEnv->MemoryLevel() < EHuiMemoryLevelReduced)
        {
        __ALFFXLOGSTRING("CAlfBridge::HandleGfxControlEffectsL - Returning because memory level below NORMAL");
        stream.Release();
        return;
        }
    
    CControlEffectState* fxData = new (ELeave) CControlEffectState;
    CleanupStack::PushL(fxData);
    fxData->ConstructL(action, stream);
    iControlEffectData = fxData;
    CleanupStack::Pop(fxData);

    // We now have all the data sent to us.
    stream.Release();

    // We need to pass the filename from the transeffect plugin that handles the controls
    // that define which effect is used for the selected skin or the selected application
    // For full screen effects the skin should determine the effect, or there should be
    // some default effects.

    CHuiCanvasVisual* visual = FindVisualByClientSideIds(fxData->iClientHandle, fxData->iClientGroupHandle);

    if (visual)
        {
        HandleGfxControlEventL(*iControlEffectData, visual);
        }
    else
        {
        __ALFFXLOGSTRING2("CAlfBridge::HandleGfxControlEffectsL - Control not found. iClientHandle %d, iClientGroupHandle %d", 
                iControlEffectData->iClientHandle, 
                iControlEffectData->iClientGroupHandle);
        return;
        }
    iAlfScreens[0]->iDisplay->SetDirty();
    iHuiEnv->ContinueRefresh();

    delete iControlEffectData;
    iControlEffectData = NULL;

    __ALFFXLOGSTRING("HandleGfxControlEffectsL - end");
    }

void CAlfBridge::HandleGfxControlEventL(CControlEffectState& aEvent,
        CHuiCanvasVisual* aCanvasVisual)
    {
	
    if (aEvent.iSetDistractionWindow) 
        {
		// marks the background clear window that comes as courtesy of the framework.
		__ALFFXLOGSTRING2("CAlfBridge::HandleGfxControlEventL - setting distraction window 0x%x to state %d", aCanvasVisual, aEvent.iSetDistractionWindow);
        switch(aEvent.iSetDistractionWindow)
            {
            case CControlEffectState::ESetDistractionWindow:
                {
                aCanvasVisual->SetCanvasFlags(EHuiCanvasFlagDistractionWindow);
                break;
                }
            case CControlEffectState::ERemoveDistractionWindow:
                {
                aCanvasVisual->ClearCanvasFlags(EHuiCanvasFlagDistractionWindow);
                break;
                }
            }
        return;
        }
		
    if ( aCanvasVisual && aCanvasVisual->Owner().ControlGroup() && aCanvasVisual->Owner().ControlGroup()->SecureId() == 0x101f857A ) // Camera id
        {
        __ALFFXLOGSTRING("CAlfBridge::HandleGfxControlEventL - Returning because inside camera app");
        return;
        }
    TInt err = KErrNone;
    // Check if there is an effects engine in HuiEnv
    CHuiFxEffect* effect = NULL;
    CHuiFxEngine* engine = NULL;
    // engine is not owned by us, it is a member of HuiEnv
    engine = iHuiEnv->EffectsEngine();
    if (engine && aCanvasVisual)
        {
        if (NeedsStoredBuffers(engine, *aEvent.iEffectName))
            {
            RecursiveStoreRenderBufferL(aCanvasVisual);
            }

        // clear out old effect if this visual has one
        __ALFFXLOGSTRING2("HandleGfxControlEffectsL - loading effect %S on visual 0x%x", aEvent.iEffectName, aCanvasVisual );
        __ALFFXLOGSTRING2("HandleGfxControlEffectsL - loading control effect, handle %d, Action: %d", aEvent.iHandle, aEvent.iAction );
        if (aCanvasVisual->Effect())
            {
            __ALFFXLOGSTRING1("HandleGfxControlEffectsL - 0x%x has active effect. Requesting removal.", aCanvasVisual);
            // another effect coming to already effected visual. E.g. close options menu, while opening effect still ongoing
            CleanFxVisual(aCanvasVisual);
            }
        TBool layoutEffectable(EFalse);
        if (aEvent.iAction == KGfxControlDisappearAction)
            {
            if (aCanvasVisual->Flags() & EHuiVisualFlagInactive)
                {
                __ALFFXLOGSTRING1("HandleGfxControlEffectsL - 0x%x is inactive. Skipping effect.", aCanvasVisual);
                return;
                }
            layoutEffectable = SetupEffectLayoutContainerL(aEvent.iHandle, aCanvasVisual, EFalse, ETrue, EFalse);
            }
        else
            {
            layoutEffectable = SetupEffectLayoutContainerL(aEvent.iHandle, aCanvasVisual, EFalse, EFalse, EFalse);
            }
        if (layoutEffectable)
            {
            TInt effectFlags = 0;
            
            // effect will start delayed anyway when it is syncronized. this flag would break syncronization between effects.
            effectFlags = KHuiFxDelayRunUntilFirstFrameHasBeenDrawn; 
            if (FxmlHasOpaqueHint(iHuiEnv->EffectsEngine(), *aEvent.iEffectName))
                {
                effectFlags |= KHuiFxOpaqueHint;
                }

            TInt activeEffectGroup = engine->ActiveGroupEffect();
            if (activeEffectGroup != KErrNotFound)
                {
                engine->AddEffectToGroup(activeEffectGroup);
                }
            TRAP( err, engine->LoadGroupEffectL(*aEvent.iEffectName, effect, aCanvasVisual->Effectable(), activeEffectGroup, NULL, this, aEvent.iHandle, effectFlags ) );
            }
        else
            {
            err = KErrNotFound;
            }
        //            RDebug::Print(_L("HandleGfxControlEffectsL - loading effect returned %d"), err );
        // only use the effect if the effect file was correctly parsed
        aCanvasVisual->SetFreezeState(EFalse);
                  
        if (err == KErrNone)
            {
#ifdef HUI_DEBUG_TRACK_DRAWING  
            visual->SetTracking(ETrue);
#endif                
            // HuiVisual takes ownership of the effect, but we save the value for reference
            // add new visual to iFullscreenEffectControlGroup. This enables multiple effects in the group, and their 
            // destruction in any order
            }
        else
            {
            // visuals added to "active effect visual" stack, but they wont be used, because effect loading has failed. cleanup.
            CleanFxVisual(NULL, aEvent.iHandle);
            }
        effect = NULL; // HuiVisual has taken ownership
        }
    }

TBool CAlfBridge::CleanFxItem(TEffectCleanupStruct& aEffectItem)
    {
    CHuiVisual* sourceViz = aEffectItem.iEffectedVisual;
    CHuiCanvasVisual* sourceViz2 = dynamic_cast<CHuiCanvasVisual*> (aEffectItem.iEffectedVisual);
    // wserv has already hidden this window. Effect has finished, so lets do what 
    // was asked. Also hide, if this is supposed to be destroyed by now.
    sourceViz->SetFreezeState(EFalse);    
    sourceViz->SetStoredRenderBuffer(NULL); // free render buffer from visual
    if (sourceViz2 && sourceViz2->Layout()) 
        {
        sourceViz2->FreeRenderBuffer();
        }
    sourceViz->ClearFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
    TBool hideVisual = EFalse;
    TBool showVisual = ETrue;
    // Effect request flow:
    // 1. Effect request
	// 2. If component is found, effect started immediately.
	// 	   If component is not found, effect request is cached and applied when window is created. 
	//	     NOTE: Only one effect request can be cached at time. Possible TODO, if this causes
	//       issues.
    // 3. Possible change of component visibility (hide / show)
    // 4. At the end of the effect the requested visiblity is applied to the visual (hidden / shown)
    
    // Exceptions 1: If it was disappear effect (aEffectItem.iHideWhenFinished is ETrue), the 
    //              component is hidden. This is to guaranteen that e.g. options menu would not popup 
    //              after disappear effect, if visual destruction / hiding request did not come in time.
    //
    // Exception 2: If disapper effect was requested, and component was requested to be hidden, 
    //              and then component was requested to be visible DURING the effect, the 
    //              component will be shown at the end of the effect. Use case: the same component shows 
    //              two sequentive notes and each note dismissal has disappear effect and no appear
	//              effect between.
    if(!aEffectItem.iIsFullScreenEffect)
        {
        hideVisual = (sourceViz->Flags() & EHuiVisualFlagShouldBeHidden) || (aEffectItem.iHideWhenFinished && !(sourceViz->Flags() & EHuiVisualFlagShouldBeShown));
        showVisual = (sourceViz->Flags() & EHuiVisualFlagShouldBeShown) && (!aEffectItem.iHideWhenFinished || !hideVisual);
        }
    else
        {
        hideVisual = (sourceViz->Flags() & EHuiVisualFlagShouldBeHidden);
        showVisual = (sourceViz->Flags() & EHuiVisualFlagShouldBeShown);
        }
    __ALFFXLOGSTRING4("CAlfBridge::RemoveTemporaryPresenterItem - iIsFullScreenEffect: %d, iHideWhenFinished: %d, Hide: %d, Show: %d", aEffectItem.iIsFullScreenEffect, aEffectItem.iHideWhenFinished, hideVisual, showVisual);
    TBool destroyVisual = sourceViz->Flags() & EHuiVisualFlagShouldDestroy;
    TBool shouldInactivate = sourceViz->Flags() & EHuiVisualFlagShouldBeInactive;
    TBool shouldBeUnderOpaqueHint = sourceViz->Flags() & EHuiVisualFlagShouldBeUnderOpaqueHint;
    __ALFFXLOGSTRING3("CAlfBridge::RemoveTemporaryPresenterItem - cleaning handle: %d, 0x%x -> 0x%x", aEffectItem.iHandle, aEffectItem.iEffectedVisual, aEffectItem.iTemporaryPresenterVisual);
    __ALFFXLOGSTRING3("CAlfBridge::RemoveTemporaryPresenterItem - hide %d, destroy %d, should inactivate: %d", TBool(hideVisual), TBool(destroyVisual), TBool(shouldInactivate));
    
    sourceViz->ClearFlag(EHuiVisualFlagAlwaysDraw);
    if (hideVisual)
        {
        // this was exit effect, and window was not really destroyed.
        sourceViz->iOpacity.Set(0.0f);
        sourceViz->ClearFlag(EHuiVisualFlagShouldBeHidden); // it is now hidden
        }
	
    if (showVisual)
        {
        sourceViz->iOpacity.Set(KAlfVisualDefaultOpacity);
        sourceViz->ClearFlag(EHuiVisualFlagShouldBeShown); // it is now shown
        }
    
    if (destroyVisual)
    	{
    	sourceViz->iOpacity.Set(0.0f);
    	}
    if (shouldInactivate)
        {
        sourceViz->SetFlag(EHuiVisualFlagInactive);
        
        if (sourceViz2)
            {
            sourceViz2->ClearCache();
            }
        sourceViz->ClearFlags(EHuiVisualFlagShouldBeInactive);
        }
    if (shouldBeUnderOpaqueHint)
        {
        sourceViz->SetFlag(EHuiVisualFlagUnderOpaqueHint);
        if (sourceViz2)
             {
             sourceViz2->ClearCache();
             }
        sourceViz->ClearFlags(EHuiVisualFlagShouldBeUnderOpaqueHint);
        }
    if (sourceViz->Effect())
        {
        sourceViz->SetEffect(NULL);
        }

        // first delete the temporary presenter visual, because this will unbind the external content
    if (aEffectItem.iTemporaryPresenterVisual)
        {
        CHuiVisual* viz = aEffectItem.iTemporaryPresenterVisual;
        
        if (viz)
            {
            if (viz->Effect())
                {
                // temporary presenter visual should not have effect, because the effect is 
                // applied to the original visual. However, if somebody misuses this API, it
                // is good to cleap up the effect here.
                viz->SetEffect(NULL);
                }
            if (viz->Layout())
                {
                viz->Layout()->Remove(viz);
                }
            viz->Owner().Remove(viz);
            delete viz;
            aEffectItem.iTemporaryPresenterVisual = NULL;
            }
        }
    // although layout does not support EHuiVisualFlagDrawOnlyAsExternalContent flag, it is being used to indicate a layout that has effect. flag must be cleared, that the layout can be drawn normally after the effect. 
    if (aEffectItem.iIsLayout)
    	{
    	aEffectItem.iEffectedVisual->ClearFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
    	// sourceViz->iOpacity.Set(1.0f);
    	}
    
    if (destroyVisual)
        {
        // window has been destroyed during effect. Kill it once and for all
        // DeleteControlGroupL and DestroyWindow can do their duty, when this flag is cleared
        sourceViz->ClearFlags(EHuiVisualFlagShouldDestroy | EHuiVisualFlagDrawOnlyAsExternalContent); 
        if (aEffectItem.iIsLayout)
            {
            if (sourceViz->Layout())
                {
                sourceViz->Layout()->Remove(sourceViz);
                }
            sourceViz->Owner().Remove(sourceViz);
             
            delete sourceViz;
            __ALFFXLOGSTRING1("CAlfBridge::RemoveTemporaryPresenterItem - Destroying sourceViz: 0x%x", sourceViz);
            aEffectItem.iEffectedVisual = NULL;
            }
        else
            {
            DestroyWindow(sourceViz, ETrue);
            }
        }
    return TBool(destroyVisual);
    }

// CleanFxVisual removes the temporary presenter visual, and unbinds it from the source. 
TBool CAlfBridge::CleanFxVisual(CHuiVisual* aVisual,
        TInt aHandle)
    {
    __ALFFXLOGSTRING2("CAlfBridge::RemoveTemporaryPresenterVisual - cleaning handle: %d, 0x%x", aHandle, aVisual);
    TInt visualsRemoved = 0;
    for (TInt i = 0; i < iEffectCleanupStack.Count(); i++)
        {
        TEffectCleanupStruct& effectItem = iEffectCleanupStack[i];
        if (aVisual == effectItem.iEffectedVisual || 
		    aHandle == effectItem.iHandle)
            {
            if (CleanFxItem(effectItem))
                {
                visualsRemoved++;
                }
            // must remove from finished effects to keep it in sync
            for(TInt k = 0; k < iFinishedCleanupStackEffects.Count(); k++)
                {
                if (iFinishedCleanupStackEffects[k] == effectItem.iHandle)
                    {
                    iFinishedCleanupStackEffects.Remove(k);
                    k--;
                    }
                }
            iEffectCleanupStack.Remove(i);
            i--;
            }
        }
    return TBool(visualsRemoved);
    }
	
CHuiLayout* CAlfBridge::FindTemporaryPresenterLayout(CHuiVisual* aVisual)
    {
    for (TInt i = 0; i < iEffectCleanupStack.Count(); i++)
        {
        if (aVisual == iEffectCleanupStack[i].iEffectedVisual)
            {
            ASSERT(iEffectCleanupStack[i].iIsLayout);
            return (CHuiLayout*)iEffectCleanupStack[i].iTemporaryPresenterVisual;
            }
        }
    return NULL;
    }

TBool CAlfBridge::HasActiveEffect(CHuiVisual* aVisual)
    {
    TInt dummy;
    return HasActiveEffect(aVisual, dummy);
    }

TBool CAlfBridge::HasActiveEffect(CHuiVisual* aVisual, TInt& aIndex)
    {
    aIndex = KErrNotFound;
    for (TInt i = 0; i < iEffectCleanupStack.Count(); i++)
        {
        if (aVisual == iEffectCleanupStack[i].iEffectedVisual)
            {
            aIndex = i;
            return ETrue;
            }
        }
    return EFalse;
    }

TInt CAlfBridge::FindEffectHandle(CHuiVisual* aVisual)
    {
    for (TInt i = 0; i < iEffectCleanupStack.Count(); i++)
        {
        if (aVisual == iEffectCleanupStack[i].iEffectedVisual)
            {
            return iEffectCleanupStack[i].iHandle;
            }
        }
    __ALFFXLOGSTRING1("CAlfBridge::FindEffectHandle - Could not find handle for 0x%x", aVisual);
    return KErrNotFound;
    }

CHuiLayout* CAlfBridge::FindLayoutByEffectHandle(TInt aHandle)
    {
    for (TInt i = 0; i < iEffectCleanupStack.Count(); i++)
        {
        if (aHandle == iEffectCleanupStack[i].iHandle && iEffectCleanupStack[i].iIsLayout)
            {
            return (CHuiLayout*)iEffectCleanupStack[i].iEffectedVisual;
            }
        }
    return NULL;
    }


TBool CAlfBridge::CleanFxVisuals()
    {
    if (!iFinishedCleanupStackEffects.Count())
        {
        return 0;
        }
    TInt itemsRemoved(0);
    CHuiControl& effectControlGroup =  iAlfScreens[0]->iFullscreenEffectControlGroup->Control(0);
    CHuiLayout* effectControlGroupLayout = (CHuiLayout*) &effectControlGroup.Visual(0);
    __ALFFXLOGSTRING1("CAlfBridge::RemoveTemporaryPresenterVisuals - BEGIN - effects in layout %d", effectControlGroupLayout->Count());

    while(iFinishedCleanupStackEffects.Count())
        {
        for (TInt i = 0; i < iEffectCleanupStack.Count() ; i++)
            {
            __ALFFXLOGSTRING2("CAlfBridge::RemoveTemporaryPresenterVisuals - looping, finished count: %d, index in active effecs: %d", iFinishedCleanupStackEffects.Count(), i );
            TEffectCleanupStruct& effectItem = iEffectCleanupStack[i];
             
            if (iFinishedCleanupStackEffects[0] == effectItem.iHandle)
                {
                if (CleanFxItem(effectItem))
                    {
                    itemsRemoved++;
                    }
                if (iEffectCleanupStack.Count() > i)
                    {
                    iEffectCleanupStack.Remove(i);
                    break;
                    }
                }
            }
        iFinishedCleanupStackEffects.Remove(0);
       }
    
    if (iFinishedCleanupStackEffects.Count())
        {
        __ALFFXLOGSTRING1("CAlfBridge::RemoveTemporaryPresenterVisuals - There are still %d finished effects, that could not be found. They have been cleaned already. Reseting the array", iFinishedCleanupStackEffects.Count());
        iFinishedCleanupStackEffects.Reset();
        }
    
    iAlfScreens[0]->iVisualTreeVisibilityChanged = ETrue; // TODO: Check if really changed   
    iAlfScreens[0]->iDisplay->SetDirty();
	__ALFFXLOGSTRING1("CAlfBridge::RemoveTemporaryPresenterVisuals - END - effects in layout %d", effectControlGroupLayout->Count());
    return itemsRemoved;
    }

void CAlfBridge::CleanAllFxVisuals()
    {
    CHuiControl& effectControlGroup =  iAlfScreens[0]->iFullscreenEffectControlGroup->Control(0);
    CHuiLayout* effectControlGroupLayout = (CHuiLayout*) &effectControlGroup.Visual(0);
    __ALFFXLOGSTRING1("CAlfBridge::RemoveAllTemporaryPresenterVisuals - BEGIN - effects in layout %d", effectControlGroupLayout->Count());

    while(iEffectCleanupStack.Count())
        {
        TEffectCleanupStruct& effectItem = iEffectCleanupStack[0];
        CleanFxItem(effectItem);
        iEffectCleanupStack.Remove(0);
        }
    
    while(iFinishedCleanupStackEffects.Count())
        {
        iFinishedCleanupStackEffects.Remove(0);
        }    
    }

void CAlfBridge::AlfGfxEffectEndCallBack(TInt aHandle)
    {
    // We need a delay to prevent the refresh from being messed up
    // We try a short delay before clearing everything up.
    __ALFFXLOGSTRING1("AlfGfxEffectEndCallBack, handle %d", aHandle );
    //    RDebug::Print(_L("AlfGfxEffectEndCallBack, append handle %d"), aHandle );
    // If the handle cannot be appended, the function returns an error.
    // However, if the memory is low, there is nothing we can do about it.
    // We try to handle the transition finish anyway.
    // If there are already other handles in the queue, the handler will not
    // be called for this handle, but all handles that have been left hanging
    // around will be cleared when HandleGfxStopEffectsL is called

    // iFinishedCleanupStackEffects.Append(aHandle);
    
	// check if the first part of application start effect finished
    if (iFSFxData 
            && iFSFxData->iHandle == aHandle
            && iFSFxData->iLongAppStartTimeout && iFSFXDataPart2)
        {
        delete iFSFxData;
        iFSFxData = iFSFXDataPart2;
        iFSFXDataPart2 = NULL;
        iFSFxData->iTwoPhaseEffect = CFullScreenEffectState::ESecondPartActive;
        if (iFSFxData->State() == EEndFullscreenReceived)
            {
            // Client notified of the EndFullScreen during the effect. Trigger the end part immediately.
            __ALFFXLOGSTRING("CAlfBridge::AlfGfxEffectEndCallBack - End was received. Trigger second part.");
            GfxTriggerEndFullScreen(iFSFxData);
            iFSFxData->SetState(EFinalEffectActive);
			// skip cleanup. It will be run, when this effect finishes
            return;
            }
        else
            {
            iFSFxData->SetState(EWaitEndFullScreen);
            }
        
        iFSFxData->iEffectType = CFullScreenEffectState::EStartEffect;
        
        CHuiControlGroup* toGroup = FindControlGroupByFullScreenToEffect();
        GfxTriggerEffectWhenFullScreenDrawn(toGroup);
        return;
        }
    
    if (!iEffectEndTimer->IsActive())
        {
        TRAP_IGNORE(iEffectEndTimer->AddFinishedHandleL(aHandle));
        iEffectEndTimer->Start(KEffectCleanupDelayInMs * 1000);
        }
    else
        {
        TRAP_IGNORE(iEffectEndTimer->AddFinishedHandleL(aHandle));
        }

    // We should do visual visibility scan after effect is ended
    iAlfScreens[0]->SetVisualTreeVisibilityChanged(ETrue);
    }

void CAlfBridge::TransitionFinishedHandlerL(TInt aHandle)
    {
    for (TInt i=0;i<iEffectCleanupStack.Count();i++)
    	{
    	if(aHandle==iEffectCleanupStack[i].iHandle)
    		{
    		// effect might have been cleaned even without this callback. e.g. closing of applicaion while option menu disappear 
    		// effect is running, would cause it. There is no need to request cleaning again.
            __ALFFXLOGSTRING2("CAlfBridge::TransitionFinishedHandlerL - Request cleaning for handle %d on visual 0x%x", aHandle, iEffectCleanupStack[i].iEffectedVisual);
    		iFinishedCleanupStackEffects.Append(aHandle);
    		}
    	}
    // If the finish request comes from the plugin, the plugin is not interested
    // in getting a notification of the transition end.
    __ALFFXLOGSTRING1("Gfx transition finished, handle %d", aHandle );
    if ( aHandle == KErrNotFound )
        {
        __ALFFXLOGSTRING("invalid handle");
        }
    // alftranstion plugin may cancel end timers connected to this effect
    TBool fsEffectDone = iFSFxData && aHandle == iFSFxData->iHandle;
    if ( fsEffectDone )
        {
        aHandle = iFSFxData->iCompletionHandle;
        }
    RProperty::Set( KPSAlfDomain, KAlfTransitionStatus, aHandle );

    // Effect finished
    if ( fsEffectDone )
        {
        delete iFSFxData;
        iFSFxData = NULL;
        
        delete iFSFXDataPart2;
        iFSFXDataPart2 = NULL;
        }
    }
    

void CAlfBridge::PrintAllocMem()
    {
#ifdef _ALF_LOGGING
    TInt allocCells = 0;
    TInt allocSpace = 0;
    TInt freeSpace = 0;
    TInt largestFreeBlock = 0;
    allocCells = User::AllocSize( allocSpace );
    freeSpace = User::Available( largestFreeBlock );
    __ALFLOGSTRING2("CAlfBridge::PrintAllocMem - alloc cells %d, alloc space %d", allocCells, allocSpace );
    __ALFLOGSTRING2("CAlfBridge::PrintAllocMem - free space %d, largest free block %d", freeSpace, largestFreeBlock );
#endif
    }
      
// ---------------------------------------------------------------------------
// DebugPrintControlGroupOrder
//
// Note, that HUI_DEBUG_TRACK_DRAWING must be enabled if you want to see the 
// real process names
// ---------------------------------------------------------------------------
// 

#ifdef ALF_DEBUG_PRINT_WINDOWGROUP_ORDER
void CAlfBridge::DebugPrintControlGroupOrder(CAlfScreen& aScreen, CHuiRoster& aRoster, CHuiControlGroup& aGroup)
    {
    __ALFLOGSTRING(">> ---AFTER-----");    
    for (TInt i=0; i<aRoster.Count();i++)
        {
        CHuiControlGroup& indexedGroup = aRoster.ControlGroup(i);
        
        if (&indexedGroup == &aGroup)
            {
            __ALFLOGSTRING("<<");                                        
            }
#ifdef  HUI_DEBUG_TRACK_DRAWING
        TInt clientWindowGroupId = FindClientWindowGroupId( aScreen.iScreenNum, indexedGroup );

        TFileName processName = iCommandDebug->WServClientFileName( clientWindowGroupId, CHuiStatic::WsSession() );
#endif         
        if (indexedGroup.Control(0).Role() == EHuiWindowGroupContainer)
            {
            if (aRoster.ControlGroup(i).ResourceId() == iAlfWindowGroupNodeId)
                {
#ifdef  HUI_DEBUG_TRACK_DRAWING
                __ALFLOGSTRING2(">> %d WINDOW GROUP (ALF), %d", i, clientWindowGroupId);                                    
#else
                __ALFLOGSTRING1(">> %d WINDOW GROUP (ALF)", i);                                                    
#endif         
                }
            else
                {
#ifdef  HUI_DEBUG_TRACK_DRAWING
                __ALFLOGSTRING3(">> %d WINDOW GROUP %S, %d", i, &processName, clientWindowGroupId);
#else
                __ALFLOGSTRING1(">> %d WINDOW GROUP", i);            
#endif
                }                    
            }
        else if (indexedGroup.Control(0).Role() == EHuiSessionContainer)
            {
#ifdef  HUI_DEBUG_TRACK_DRAWING
            __ALFLOGSTRING2(">> %d ALF GROUP, %d", i, clientWindowGroupId);                        
#else
            __ALFLOGSTRING1(">> %d ALF GROUP", i);                                    
#endif
            }
        else if (indexedGroup.Control(0).Role() == EHuiWindowFloatingSpriteContainer)
            {
#ifdef  HUI_DEBUG_TRACK_DRAWING			
            __ALFLOGSTRING3(">> %d FLOATING SPRITE GROUP %S, %d", i, &processName, clientWindowGroupId);                                            
#else
            __ALFLOGSTRING1(">> %d FLOATING SPRITE GROUP", i);                                            
#endif
            }
        else if (indexedGroup.Control(0).Role() == EHuiFullScreenEffectContainer)
            {
#ifdef  HUI_DEBUG_TRACK_DRAWING
            __ALFLOGSTRING3(">> %d EFFECT GROUP %S, %d", i, &processName, clientWindowGroupId );                                                                
#else
            __ALFLOGSTRING1(">> %d EFFECT GROUP", i);                                                                
#endif
            }
        else if (indexedGroup.Control(0).Role() == EHuiFpsIndicatorContainer)
            {
#ifdef  HUI_DEBUG_TRACK_DRAWING
            __ALFLOGSTRING3(">> %d FPS GROUP %S, %d", i, &processName, clientWindowGroupId );                                                                
#else
            __ALFLOGSTRING1(">> %d FPS GROUP", i);                                                                
#endif
            }
        else 
            {
#ifdef  HUI_DEBUG_TRACK_DRAWING
            __ALFLOGSTRING3(">> %d UNKNOWN GROUP ?!!? %S, %d", i, &processName, clientWindowGroupId );
#else
            __ALFLOGSTRING1(">> %d UNKNOWN GROUP ?!!?", i);                                                                            
#endif
            }                                                

        if (&indexedGroup == &aGroup)
            {
            __ALFLOGSTRING("<<");                                        
            }            
        }
    
    // Print the group that has keyboard focus
    TInt focusWindowGroupId = CHuiStatic::WsSession().GetFocusWindowGroup();
    TFileName processName;
    TThreadId threadId;
    TInt error = CHuiStatic::WsSession().GetWindowGroupClientThreadId( focusWindowGroupId, threadId );
    RThread thread;
    TInt err = thread.Open( threadId );
    if( !err )
        {
        RProcess process;
        err = thread.Process( process );
        if( !err )
            {
            processName = process.FileName();
            __ALFLOGSTRING2( ">> Keyboard focus group: %S, %d", &processName, focusWindowGroupId );
            }
        process.Close();
        thread.Close();
        }      
    __ALFLOGSTRING(">> ---------------");                
    }
#else
void CAlfBridge::DebugPrintControlGroupOrder(CAlfScreen& /*aScreen*/, CHuiRoster& /*aRoster*/, CHuiControlGroup& /*aGroup*/)
    {
    }
#endif        

#ifdef ALF_DEBUG_VISUALIZE_WINDOWGROUP_ORDER
void CAlfBridge::VisualizeControlGroupOrderL(CAlfScreen& aScreen, CHuiRoster& aRoster, CHuiControlGroup& /*aGroup*/)
    {       
    TInt nbrofcolums = 3;
    TInt nbrofrows = 3;
    TInt gridIndex = 0;
    TBool skipEmpty = EFalse;
    TInt skipCount = aScreen.FixedControlGroupCount() + 1;
    if (aRoster.Count() > skipCount)
        {
        for (TInt i=aRoster.Count() - skipCount; i>0; i--)
            {
            CHuiControl& control = aRoster.ControlGroup(i).Control(0);
            if (skipEmpty)
                {                        
                TBool empty = ETrue;
                CHuiLayout* layout = (CHuiLayout*)&control.Visual(0);
                for (TInt j=layout->Count()-1; j >= 0; j--)
                    {
                    CHuiCanvasVisual* canvasVisual = (CHuiCanvasVisual*) &layout->Visual(j);
                    if (canvasVisual && canvasVisual->DisplayRect().Size().Round() != TSize(0,0) && canvasVisual->PaintedAreaCount())
                        {
                        empty = EFalse;        
                        break;
                        }                        
                    }
                
                if (empty && skipEmpty)
                    {
                    continue;   
                    }                    
                }
            
            TSize screensize = HuiUtil::ScreenSize();
            CHuiTransformation::TTransform& step = aRoster.ControlGroup(i).Transformation().Step(0);
            step.iParams[EHuiTransformParamTranslateX].Set((gridIndex % nbrofcolums) * (screensize.iWidth/nbrofcolums), 500);
            step.iParams[EHuiTransformParamTranslateY].Set((gridIndex / nbrofrows) * (screensize.iHeight/nbrofrows), 500);
            gridIndex++;
            }                        
        }
    iAlfScreens[0]->iDisplay->SetClearBackgroundL(CHuiDisplay::EClearWithSkinBackground);            
    }
#else
void CAlfBridge::VisualizeControlGroupOrderL(CAlfScreen& /*aScreen*/, CHuiRoster& /*aRoster*/, CHuiControlGroup& /*aGroup*/)
    {       
    }
#endif    

// ---------------------------------------------------------------------------
// PostQTCommandBufferL
// ---------------------------------------------------------------------------
// 
void CAlfBridge::PostQTCommandBufferL( TAlfQtCommandBufferParams params )
    {       
    CHuiCanvasVisual* huiVisual = NULL;       
/*    if ((*iHost))
        {  
        if( (*iHost)->StreamerServer() )
            { 
            if ((*iHost)->StreamerServer()->WindowMgr())
                {
                RArray<TAlfWServInfo> winInfoList;
                
                (*iHost)->StreamerServer()->WindowMgr()->GetWinInfoListL( params.iWsWgId, 
                        params.iWsClientId, winInfoList );
                    
                for ( TInt i= 0; i<winInfoList.Count(); i++ )
                    {
                    if( winInfoList[i].iClientSideId.iWindowIdentifer == params.iWsClientId )
                        {
						 huiVisual = FindVisual( winInfoList[i].iRefId.iWindowIdentifer );
						 break;
	                  }    
                    }
				 winInfoList.Close();                                
                }
            }
        }
*/    
    if ( huiVisual )
       {
       TPtrC8 commands( (TUint8 *)params.iPtr, params.iLength );
       TAlfCommandBufferStatus bufferStatus =  params.iBufferStatus;
       huiVisual->SetCommandType( 1 ); // set to ECommandBufferWs should it be some QT specific?
       if ( bufferStatus == EComplete )
           {         
           huiVisual->ClearCommandSet();
           huiVisual->AddCommandSetL(commands);
           }
       else if ( bufferStatus == EPartial )
           {
           huiVisual->AddPartialCommandSetL( commands, EFalse );
           }
       else if ( bufferStatus == ELastPart )
           {
           huiVisual->AddPartialCommandSetL( commands, ETrue );
           }
       }    
    }
    

// ---------------------------------------------------------------------------
// SetClientWindowForDrawingL
//
// Note: One external content visual can exist in one canvas visual at the 
// time. When external content visual is deleted, in destructor, it removes 
// pointers to it from all canvas visuals. So canvasVisual->ExternalContent() 
// always returns a valid pointer, or NULL. The parameter aRootVisual can 
// be NULL, which means that the external content is removed from the canvas 
// visual
// ---------------------------------------------------------------------------
// 
void CAlfBridge::SetClientWindowForDrawingL(TInt /*aWindowGroupId*/, TInt /*aClientWindowHandle*/, 
	CHuiVisual* /*aExternalContentVisual*/)
	{    	
  /*  RArray<TAlfWServInfo> winInfoList;
    CleanupClosePushL(winInfoList);

	// Find canvas visual for the RWindow
	
	(*iHost)->StreamerServer()->WindowMgr()->GetWinInfoListL(aWindowGroupId, aClientWindowHandle, winInfoList );
	if(winInfoList.Count() != 1)
		{
		User::Leave(KErrArgument);
		}

	CHuiCanvasVisual* canvasVisual = (CHuiCanvasVisual*)FindVisual(winInfoList[0].iRefId.iWindowIdentifer);
	if (!canvasVisual)
		{
		User::Leave(KErrNotFound);
		}
			
	// Current external content 
	CHuiVisual* currentContent = canvasVisual->ExternalContent();
	
	if (!aExternalContentVisual)
		{	
		// Do not draw external content any more. 
			
		// Disable external content for the canvas visual	
		canvasVisual->SetExternalContentL(NULL);
		// For the current content, enable drawing it in the original visual tree. 
		if (currentContent)
			{
			currentContent->ClearFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
			}
		}
	else if (!(aExternalContentVisual->Flags() & EHuiVisualFlagDrawOnlyAsExternalContent))
		{
		// The visual is not yet used by other canvas visual
		
		// Enable external content for the canvas visual	
		canvasVisual->SetExternalContentL(aExternalContentVisual);
		// For the current content, enable drawing it in the original visual tree. 
		if (currentContent)
			{
			currentContent->ClearFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
			}
		// Disable normal drawing and start using as external content	
		aExternalContentVisual->SetFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
		}   
	else
		{
		// aExternalContentVisual is already used as an external content for some visual
		User::Leave(KErrAlreadyExists);
		}
		
	CleanupStack::PopAndDestroy(); // winInfoList
*/	}

void CAlfBridge::SetVisualTreeVisibilityChanged(TBool aChanged)
    {
    for (TInt i=0; i<iAlfScreens.Count();i++)
        {
        iAlfScreens[i]->iVisualTreeVisibilityChanged = aChanged;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::EnableSwRenderingL(TBool aEnable)
    {    
    if (aEnable == iSwRenderingEnabled)
        {
        return;
        }
    
    iSwRenderingEnabled = aEnable;    
    
    AMT_MAP_SET_VALUE(
            iBoolMap,
            AMT_MAP_SW_ENABLED_HANDLE,
            iSwRenderingEnabled,
            EAlfModuleTestTypeBridgeGoom );
    
    for (TInt i=0; i<iAlfScreens.Count();i++)
        {
        PrepareSwRenderingTarget(iAlfScreens[i]);        
     
        if (iSwRenderingEnabled)
            {
            CHuiTexture* texture = CHuiTexture::NewL();
            CleanupStack::PushL(texture);
            iAlfScreens[i]->iDisplay->SetForegroundBitmapL( iAlfScreens[i]->iSwRenderingTarget );
            CleanupStack::Pop(texture);
            iAlfScreens[i]->iDisplay->SetForegroundTexture(texture);
#ifdef ALF_DRAW_FRAME_BEFORE_END_CALLBACK 
            iHuiEnv->SetRefreshMode(EHuiRefreshModeManual);
#endif // #ifdef ALF_DRAW_FRAME_BEFORE_END_CALLBACK 
            }
        else
            {
            iAlfScreens[i]->iDisplay->SetForegroundBitmapL(NULL);            
            iAlfScreens[i]->iDisplay->SetForegroundTexture(NULL);
#ifdef ALF_DRAW_FRAME_BEFORE_END_CALLBACK 
            iHuiEnv->SetRefreshMode(EHuiRefreshModeAutomatic);
#endif // #ifdef ALF_DRAW_FRAME_BEFORE_END_CALLBACK 
            }
            
        // SetCapturingBufferL is called from HandleVisualVisibility.
        iAlfScreens[i]->SetVisualTreeVisibilityChanged(ETrue);
        }

    if (!iSwRenderingEnabled && iManualRefreshTimer)
        {
        iManualRefreshTimer->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TBool CAlfBridge::PrepareSwRenderingTarget( CAlfScreen* aScreen )
    {
    TBool modified = EFalse;
    
    // Check ultra low memory mode here
    if (iHuiEnv->MemoryLevel() <= EHuiMemoryLevelLowest)
        {        
        if (!aScreen->iSwRenderingTarget)
            {
            // screen owns this bitmap
            aScreen->iSwRenderingTarget = new CFbsBitmap;
            if (aScreen->iSwRenderingTarget)
                {
                aScreen->iSwRenderingTarget->Create(aScreen->Size(), EColor16MAP); 
                modified = ETrue;
                }
            }
        
        if (aScreen->iSwRenderingTarget)
            {
            if (aScreen->iSwRenderingTarget->SizeInPixels() != aScreen->Size())
                {
                aScreen->iSwRenderingTarget->Resize(aScreen->Size());
                modified = ETrue;
                }        
            // For tests
            //TUint32* data = aScreen->iSwRenderingTarget->DataAddress();
            }
        
        if (modified && aScreen->iSwRenderingTarget)
            {
            InitializeSwRenderingTarget(aScreen->iSwRenderingTarget);
            }
        }
    else
        {
        delete aScreen->iSwRenderingTarget;
        aScreen->iSwRenderingTarget = NULL;
        modified = ETrue;
        }
        
    return modified;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::InitializeSwRenderingTarget(CFbsBitmap* aBitmap)
    {
    __ALFLOGSTRING("CAlfBridge::InitializeSwRenderingTarget");
    TAlfBridgeDrawerWrapper wrapper(*this);
    AlfDrawerInternal::CopyScreenToBitmap(&wrapper, aBitmap, 
        TRect(aBitmap->SizeInPixels()));
    __ALFLOGSTRING("CAlfBridge::InitializeSwRenderingTarget ok");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::SetMemoryLevel(THuiMemoryLevel aMemoryLevel)
    {
    iCurrentMemoryLevel = aMemoryLevel;
    DoUpdateMemoryLevel();    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::SetLowMemory(TBool aEnabled)
    {
    iLowMemoryMode = aEnabled;
    
    AMT_MAP_SET_VALUE(
            iBoolMap,
            AMT_MAP_LOW_MEMORY_MODE_HANDLE,
            iLowMemoryMode,
            EAlfModuleTestTypeBridgeGoom );
    
    DoUpdateMemoryLevel();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TInt CAlfBridge::ForceSwRendering(TBool aEnabled)
    {
    iForcedSwRendering = aEnabled;
    
    AMT_MAP_SET_VALUE(
            iBoolMap,
            AMT_MAP_FORCE_SW_HANDLE,
            iForcedSwRendering,
            EAlfModuleTestTypeBridgeGoom );
    
    DoUpdateMemoryLevel();
    return KErrNone;        
    }

// ---------------------------------------------------------------------------
// This decides memory level for core toolkit. This should the only place
// where core toolkit memory level is modified.
// ---------------------------------------------------------------------------
// 
void CAlfBridge::DoUpdateMemoryLevel()
    {
    THuiMemoryLevel memoryLevel = iCurrentMemoryLevel;
    if ( iLowMemoryMode && ( memoryLevel > EHuiMemoryLevelReduced ) )
        {
        memoryLevel = EHuiMemoryLevelReduced;
        }
    if ( iForcedSwRendering && ( memoryLevel > EHuiMemoryLevelLowest ) )
        {
        // For now, we use memory level to force SW rendering
        // to be used.
        memoryLevel = EHuiMemoryLevelLowest;
        }
    
    if ( memoryLevel != iHuiEnv->MemoryLevel() )
        {    

        __ALFLOGSTRING1("CAlfBridge::DoUpdateMemoryLevel -> %d", memoryLevel);
        TBool nowGoodMemory = !(memoryLevel < EHuiMemoryLevelReduced);
        TBool wasGoodMemory = !(iHuiEnv->MemoryLevel() < EHuiMemoryLevelReduced);
        
        if (iActivated)
            {
            if (nowGoodMemory && !wasGoodMemory)
                {
                iBridgerClient.SendBlind(KAlfCompositionGoodOnGraphicsMemory, TIpcArgs());
                }
            else if (!nowGoodMemory && wasGoodMemory)
                {
                iBridgerClient.SendBlind(KAlfCompositionLowOnGraphicsMemory, TIpcArgs());
                }
            else
                {
                // No change.
                }
            }
        
        if ( memoryLevel < EHuiMemoryLevelReduced)
            {
            OnLowMemoryModifyAllEffects();
            }
            
        iHuiEnv->NotifyMemoryLevel(memoryLevel);
        if (memoryLevel <= EHuiMemoryLevelLowest)
            {
            TRAP_IGNORE(EnableSwRenderingL(ETrue));
            }
        else
            {
            TRAP_IGNORE(EnableSwRenderingL(EFalse));        
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::OnLowMemoryModifyAllEffects()
    {
    __ALFLOGSTRING("CAlfBridge::OnLowMemoryModifyAllEffects");
    iLayoutSwitchEffectCoordinator->LowMemoryEvent();
    HandleGfxStopEvent( EFalse );
    
	__ALFLOGSTRING("CAlfBridge::OnLowMemoryModifyAllEffects done");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TInt CAlfBridge::GetSizeAndRotation(TSize& aSize, TInt& aRotation)
    {
    if ( !iAlfScreens.Count() )
        {
        return KErrNotReady;
        }

    switch (iAlfScreens[0]->iDisplay->Orientation())
        {
    case CHuiGc::EOrientationCCW90:
        aRotation = 1;
        break;
    case CHuiGc::EOrientation180:
        aRotation = 2;
        break;
    case CHuiGc::EOrientationCW90:
        aRotation = 3;
        break;
    case CHuiGc::EOrientationNormal:
    default:
        aRotation = 0;
        break;
        }    

    aSize = iAlfScreens[0]->iDisplay->Size();

    __ALFLOGSTRING3("CAlfBridge::GetSizeAndRotation %d x %d, rot %d", aSize.iWidth, aSize.iHeight, aRotation);
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TInt CAlfBridge::ReadPixels(CFbsBitmap* aBitmap)
    {
    if ( !iAlfScreens.Count() )
        {
        return KErrNotReady;
        }
    __ALFLOGSTRING("CAlfBridge::ReadPixels");  
    TRAPD(err, iAlfScreens[0]->iDisplay->CopyScreenToBitmapL(aBitmap));
    __ALFLOGSTRING1("CAlfBridge::ReadPixels ret %d", err);    
    return err;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CAlfBridge::Synchronized(TInt aId)
    {
    if (aId == KRequestFrameSynchForComposition)
        {
        return;    
        }    
    else if (aId == iIdForLayoutSwitchFrameSync)
        {
        // send notification to alfstreamer server about
        if (iActivated)
            {
            iBridgerClient.SendBlind(KAlfCompositionLayoutSwitchComplete, TIpcArgs());
            }        
        }
    else
        {
        // Use P&S for now.
        RProperty::Set( KAlfPSUidSynchronizer, KAlfPSKeySynchronizer, aId );
        }
    }

void CAlfBridge::SetWindowGroupAsAlfApp(TInt aId)
    {
   TBool lBreak = EFalse;
    
    for ( TInt j = 0; j < iAlfScreens.Count(); j++ )
            {
            for ( TInt i = 0; i < iAlfScreens[j]->iControlGroups.Count(); i++ )
                {
                if ( iAlfScreens[j]->iControlGroups[i].iClientWindowGroupId == aId )
                    {
                    iAlfScreens[j]->iControlGroups[i].iControlGroup->iAlfApp = ETrue;
                    lBreak = ETrue;
                    break;
                    }
                }   
            if ( lBreak )
               break;
            }
    TBool alreadyExists = EFalse;
    for( TInt i = 0 ; i < iAlfNativeClientsWgIds.Count() ; i++ )
        {
        if(iAlfNativeClientsWgIds[i] == aId)
            {
            alreadyExists = ETrue;
            }
        }
    if(!alreadyExists)
        {
        iAlfNativeClientsWgIds.Append(aId);
        }
    }

void CAlfBridge::RemoveWindowGroupAsAlfApp( TInt aId )
    {
    for ( TInt j = 0; j < iAlfScreens.Count(); j++ )
            {
            for ( TInt i = 0; i < iAlfScreens[j]->iControlGroups.Count(); i++ )
                {
                if ( iAlfScreens[j]->iControlGroups[i].iClientWindowGroupId == aId )
                    {
                    iAlfScreens[j]->iControlGroups[i].iControlGroup->iAlfApp = EFalse;
                    }
                }   
            }
    for( TInt i = 0 ; i < iAlfNativeClientsWgIds.Count() ; i++ )
        {
        if(iAlfNativeClientsWgIds[i] == aId)
            {
            iAlfNativeClientsWgIds.Remove(i);
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TBool CAlfBridge::IsAlfOriginatedWindow(CHuiCanvasVisual& aVisual)
    {
    return (aVisual.Owner().ControlGroup()->ResourceId() == iAlfWindowGroupNodeId);
    }


// ---------------------------------------------------------------------------
// Checks if layout switch is ready. The aDuration parameter is the duration
// since layout switch started. So this allows heuristics to loosen as there
// may be applications that do not behave as expected. aDuration is meant
// to be given in units of 50ms.
// ---------------------------------------------------------------------------
// 
TBool CAlfBridge::IsLayoutSwitchReady( TInt aDuration )
    {
    TBool visualTreeReady = ETrue;

    if (!iAlfScreens.Count())
        return EFalse;
   
    iTempRegion.Clear();
    iTempRegion2.Clear();
    
    CAlfScreen* screen = iAlfScreens[0];
    TRect fullscreen = TRect(TPoint(0,0), screen->Size());
    TInt orientation = screen->iDisplay->Orientation();

    __ALFLOGSTRING2("CAlfBridge::IsLayoutSwitchReady begin o:%d d:%d", orientation, aDuration);
    
    TBool fullscreenCovered = EFalse;
	TBool alfClientWindowGroupVisible = EFalse;
	
	TInt coverageModifiedGroupCount = 0;
		
    // Skip the topmost (effect) layer, start from floating sprite group
    for (TInt j=screen->iDisplay->Roster().Count() - screen->FixedControlGroupCount(); j>=0; j--)
        {                
        CHuiControlGroup& controlgroup = screen->iDisplay->Roster().ControlGroup(j);
        CHuiControl& control = controlgroup.Control(0);

        if (control.Role() == EHuiFpsIndicatorContainer)
            {
            // FPS container doesn't contain canvas visuals
            continue;
            }

        if (controlgroup.ResourceId() == iAlfWindowGroupNodeId)
            {
            // ALF window doesn't have drawing and it's meant to be completely semitransparent,
            // so skip.
            continue;
            }

        CHuiCanvasVisual* layout = (CHuiCanvasVisual*)&control.Visual(0);
        
        // For optimization reasons, check if all visuals below in Z-order are covered    
        if (!fullscreenCovered)
            {
            fullscreenCovered = IsRectCoveredByRegion(fullscreen, iTempRegion);            
            }
    
        if (fullscreenCovered)
            {
            break;
            }    
        
        // ALF applications are handled by checking if their visuals cover the whole screen.
        if (control.Role() == EHuiSessionContainer)
            {
            CHuiLayout* hostContainer = control.ContainerLayout( NULL );
            TInt flags = hostContainer->Flags();            
            if (alfClientWindowGroupVisible)
                {
                // check visual sizes if client has not made this control group hidden
                if(!(flags&EHuiVisualFlagUnderOpaqueHint))
                    {
                    if(iAppUi)
                        {
                        CAlfAppSrvSessionBase* alfSrvSession = iAppUi->SrvSessionForControlGroup(controlgroup);
                        
                        TInt clientWindowGroupId = alfSrvSession->ClientWindowGroup();
                        // checking the case if there are multiple alf application openend.
                        if (clientWindowGroupId == iAppUi->GetLastActiveClient())
                            {
                            if(IsFullScreenDrawnRecursiveAlfContent(hostContainer, fullscreen) )
                                {
                                return ETrue;
                                }
                            }
                        }
                    }
                }
            continue;
            }
        
        TBool hasActiveVisualsInVisualTree(EFalse);
        TBool coverageRegionModified(EFalse);
        
        if (alfClientWindowGroupVisible && (controlgroup.ResourceId() == iAlfWindowGroupNodeId))
            {
            visualTreeReady =
               IsLayoutSwitchReadyRecursive( layout, controlgroup, control, 
                   hasActiveVisualsInVisualTree, fullscreen, screen, 
                   IsVisualOpaque(*layout), alfClientWindowGroupVisible, 
                   orientation, aDuration, coverageRegionModified );            
            }
        else
            {
            visualTreeReady =
               IsLayoutSwitchReadyRecursive( layout, controlgroup, control, 
                   fullscreenCovered, fullscreen, screen, 
                   IsVisualOpaque(*layout), EFalse, orientation, 
                   aDuration, coverageRegionModified );            
            }

        if (!visualTreeReady)
            {
            // Problem found in IsLayoutSwitchReadyRecursive
            visualTreeReady = EFalse;
            break;
            }

        if (coverageRegionModified)
            {
            coverageModifiedGroupCount++;
            }

        // Heuristic: If coverage region is modified by multiple apps, then it's possible that
        // foreground application hasn't had time to update its layout. So give little time to do that.
        if (aDuration <= KAlfCheckCoverageFromMultipleGroups && coverageRegionModified > 1)
            {
            __ALFLOGSTRING("CAlfBridge::IsLayoutSwitchReady not ready - multiple groups");
            visualTreeReady = EFalse;
            break;
            }

        if( controlgroup.iAlfApp && layout->iOpacity.Now() > 0.0f )
            {
            alfClientWindowGroupVisible = ETrue;
            }
        }

    __ALFLOGSTRING2("CAlfBridge::IsLayoutSwitchReady done %d (groups %d)", visualTreeReady, coverageModifiedGroupCount);
    iTempRegion2.Clear();
    
    // Heuristic: Check that visual drawing covers the whole screen.
    if ( visualTreeReady && ( aDuration <= KAlfCheckFullscreenCoveredByVisualDrawing ) )
        {
        if ( !IsRectCoveredByRegion( fullscreen, iTempRegion ) )
            {
            __ALFLOGSTRING("CAlfBridge::IsLayoutSwitchReady not ready - screen not covered");
            visualTreeReady = EFalse;
            }
        }
    
    return visualTreeReady;
    }


// ---------------------------------------------------------------------------
// Checks if rect is inside fullscreen area.
// ---------------------------------------------------------------------------
// 
static TBool CheckVisualRect(const TRect aRect, const TRect& aFullScreen)
    {
    if (aRect.IsEmpty())
        {
        return EFalse;
        }
         
    if (!aFullScreen.Contains(aRect.iTl))
        {
        return EFalse;
        }

    TPoint br = aRect.iBr;
    br.iX--;
    br.iY--;
    if (!aFullScreen.Contains(br))
        {
        return EFalse;
        }
    
    return ETrue;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
TBool CAlfBridge::IsLayoutSwitchReadyRecursive(
        CHuiLayout* aLayout,
        CHuiControlGroup& aControlGroup,
        CHuiControl& aControl,
        TBool& aFullscreenCovered, 
        const TRect& aFullscreen,
        CAlfScreen* aScreen,
        TBool aChildCanBeOpaque, 
        TBool aOnlyForEmbeddedAlfApp,
        TInt aOrientation,
        TInt aDuration,
        TBool& aCoverageRegionModified )
    {
    TBool visualTreeReady = ETrue;
    TRect visualDisplayRect;
    TBool visualRectIsCovered = EFalse;
    TBool visualIsOpaque = EFalse;
    TBool visualIsActive = EFalse;
    CHuiCanvasVisual* canvasVisual = NULL;

    for (TInt i = aLayout->Count() - 1; i >= 0; i--)
        {
        visualDisplayRect = TRect(0,0,0,0);
        visualRectIsCovered = EFalse;
        visualIsOpaque = EFalse;
        visualIsActive = EFalse;
        
        canvasVisual = (CHuiCanvasVisual*)(&aLayout->Visual(i));

        // Distraction windows are completely ignored.
        TBool distractionWindow = (canvasVisual->CanvasFlags() & EHuiCanvasFlagDistractionWindow);
        if ( distractionWindow )
            {
            continue;
            }

        visualIsOpaque = aChildCanBeOpaque && IsVisualOpaque(*canvasVisual);
        TBool visualIsVisible = (canvasVisual->iOpacity.Now() > 0.01f);       
        
        if (canvasVisual->Count() && visualIsVisible)
            {
            TBool ready = IsLayoutSwitchReadyRecursive( canvasVisual, aControlGroup, aControl, 
                aFullscreenCovered, aFullscreen, aScreen, 
                visualIsOpaque, EFalse, aOrientation, 
                aDuration, aCoverageRegionModified );
                
            if (!ready)
                {
                visualTreeReady = EFalse;
                break;
                }
            }

        // Calculate visual display rect
        visualDisplayRect = canvasVisual->DisplayRect();
    
        ClipVisualRect(visualDisplayRect, aLayout->DisplayRect());
        ClipVisualRect(visualDisplayRect, aFullscreen);
        
        // Check if covered.
        if (aFullscreenCovered)
            {
            visualRectIsCovered = ETrue;    
            }
        else
            {            
            // Check if this visual is covered by other opaque visuals which rects are in "covered" region
            // it should not check for alf event window group, when we have embedded alf application,
            // because we have assumed that alf app will have full screen covered
            if(!aOnlyForEmbeddedAlfApp)
                {
                visualRectIsCovered = IsRectCoveredByRegion(visualDisplayRect, iTempRegion);
                }                                
            }

        // Heuristic: Check that visual display rect is ok (not empty, inside screen)
        if ( aDuration <= KAlfCheckVisualDisplayRect && !visualRectIsCovered && 
             !CheckVisualRect(visualDisplayRect, aFullscreen) )
            {
            __ALFLOGSTRING("CAlfBridge::IsLayoutSwitchReady not ready - display rect not ok");
            visualTreeReady = EFalse;
            break;
            }
    
	    // Determine if visual is active from coverage point of view.
        visualIsActive = EFalse;
        if (visualRectIsCovered)
            {                                        
            if (!(canvasVisual->Flags() & EHuiVisualFlagAlwaysDraw)) 
                {
                visualIsActive = EFalse;                
                }
            else
                { 
                visualIsActive = ETrue;
                }                
            }
        else
            {
			visualIsActive = ETrue;
            }
        
        // Sprites and effects as we consider them always as transparent and also
        // if controlgroup is transformed somehow    
        if (aControl.Role() == EHuiFullScreenEffectContainer || 
            aControl.Role() == EHuiWindowFloatingSpriteContainer ||
            aControlGroup.IsTransformed())
            {
            visualIsOpaque = EFalse;    
            }
           
        // Check that command buffers are ok.
        if ( visualIsActive )
            {
            TRect displayRect = visualDisplayRect;
            visualDisplayRect = canvasVisual->CommandBufferCoverage(aOrientation);

            // Heuristic: Check that visual drawing isn't completely empty.
            if ( aDuration <= KAlfCheckVisualDrawingNotEmpty && !visualRectIsCovered &&
                 visualIsVisible && visualDisplayRect.IsEmpty() )
                {
                __ALFLOGSTRING("CAlfBridge::IsLayoutSwitchReady not ready - no drawing");
                visualTreeReady = EFalse;
                break;
                }
            
            // Heuristic: Check that visual drawing isn't outside the screen.
            if ( aDuration <= KAlfCheckVisualDrawRect && !visualRectIsCovered &&
                 visualIsVisible && !CheckVisualRect(visualDisplayRect, aFullscreen) )
                {
                __ALFLOGSTRING("CAlfBridge::IsLayoutSwitchReady not ready - bad drawing");
                visualTreeReady = EFalse;
                break;
                }

            // Heuristic: Check that visual drawing is complete.
            if ( aDuration <= KAlfCheckVisualDrawingComplete && !visualRectIsCovered &&
                 visualIsVisible && displayRect != visualDisplayRect )
                {
                __ALFLOGSTRING("CAlfBridge::IsLayoutSwitchReady not ready - incomplete drawing");
                visualTreeReady = EFalse;
                break;
                }

            // Make sure we clip visual rect to visible screen area
            ClipVisualRect(visualDisplayRect, aFullscreen);
            
            // Heuristic: Union of opaque visual drawing regions should cover the whole screen.
            // Note - coverage is checked at the end of IsLayoutSwitchReady.
            if (visualIsOpaque && visualIsVisible && !visualRectIsCovered)
                {
                iTempRegion2.AddRect( visualDisplayRect );
                iTempRegion2.Tidy();
                }
            }
			
        if (visualIsActive && visualIsOpaque && !visualRectIsCovered)
            { 
            // New visibility system takes only window shape into account.
            if ( canvasVisual->CanvasFlags() & EHuiCanvasFlagIncludeToVisibilityCalculation )
                {
                if ( !canvasVisual->HasCustomShape() )
                    {
                    aCoverageRegionModified = ETrue;
                    
                    TRect displayRect(canvasVisual->DisplayRect());
                    ClipVisualRect(displayRect, aLayout->DisplayRect());
                    ClipVisualRect(displayRect, aFullscreen);

                    iTempRegion.AddRect(displayRect);
                    iTempRegion.Tidy();
                    }
                else
                    {
                    aCoverageRegionModified = ETrue;
                    
                    TRect displayRect = canvasVisual->DisplayRect();
                    const TRegion& region = canvasVisual->ShapeRegion();
                    TPoint delta = -canvasVisual->ShapeOrigin() + displayRect.iTl;
                
                    for (TInt i = 0; i < region.Count(); ++i)
                        {
                        TRect coveredRect(region[i]);
                        coveredRect.Move(delta);
                        ClipVisualRect(coveredRect, aFullscreen);

                        iTempRegion.AddRect(coveredRect);
                        }
                    iTempRegion.Tidy();
                    }
                }
            }        
        } // for loop end : children checking loop
     
    return visualTreeReady;
    }

TBool CAlfBridge::IsFullScreenDrawnRecursiveAlfContent(CHuiVisual* aVisual, TRect& aFullScreen)
    {
    if(aVisual)
        {
        TInt flags = aVisual->Flags();
        if(!(flags && EHuiVisualFlagInactive) && aVisual->iOpacity.Now() > 0 ) // don't take inactive or invisible visual into account
            {
            TPoint point = aVisual->Pos().Now();
            TPoint size = aVisual->Size().Now();
            TRect visualRect(point, TSize(size.iX, size.iY));
            iTempRegion.AddRect(visualRect);
            iTempRegion.Tidy();

            if( IsRectCoveredByRegion(aFullScreen, iTempRegion) )
                {
                return ETrue;
                }
            }
        TInt count = aVisual->Count();
        if(count)
            {
            for(TInt i = 0 ; i < count ; i++)
                {
                if( IsFullScreenDrawnRecursiveAlfContent( &aVisual->Visual(i), aFullScreen ) )
                    {
                    return ETrue;
                    }
                }
            }
        }
    return EFalse;
    }


TBool CAlfBridge::IsFullScreenDrawnRecursive(
        CHuiLayout* aLayout, 
        CHuiControlGroup& aControlGroup,
        CHuiControl& aControl,
        TBool& aFullscreenCovered, 
        TRect& aFullscreen,
        CAlfScreen* aScreen,
        TBool& aSubtreeVisible, 
        TBool aChildCanBeOpaque,
        TInt aOrientation)
    {
    TBool visualTreeActive = EFalse;
    TRect visualDisplayRect;
    TBool visualRectIsCovered = EFalse;
    TBool visualIsOpaque = EFalse;
    TBool visualIsActive = EFalse;
    CHuiCanvasVisual* canvasVisual = NULL;

    for (TInt i=aLayout->Count()-1; i >= 0; i--)
        {
        visualDisplayRect = TRect(0,0,0,0);
        visualRectIsCovered = EFalse;
        visualIsOpaque = EFalse;
        visualIsActive = EFalse;
        TBool visualSubtreeVisible = EFalse;
        
        // Check first if visual itself is hidden or does hide other visuals below 
        // in z-order. If it does not hide, then we do not add its displayrect to covering 
        // region.
        canvasVisual = (CHuiCanvasVisual*)(&aLayout->Visual(i));
        
        // Child can be considered to be opaque only if all parents are opaque and
        // visual itself is opaque.
        // For example, parent can have opacity < 1 and that affects children as well.
        // As another example, parent can have scaling transformation.
        visualIsOpaque = aChildCanBeOpaque && IsVisualOpaque(*canvasVisual);
        
        if (canvasVisual->Count())
            {
            visualTreeActive |= IsFullScreenDrawnRecursive( canvasVisual, aControlGroup, aControl,aFullscreenCovered, aFullscreen, aScreen,visualSubtreeVisible, visualIsOpaque, aOrientation );
            }
           
        
        if (visualSubtreeVisible)
            {
            aSubtreeVisible = ETrue;
            }
            
        // Optimization, it is faster to check fullscreen than read visuals displayrect
        if (aFullscreenCovered)
            {
            visualRectIsCovered = ETrue;    
            }
        else
            {
            // add the rect only if the window contains drawing for the new orientation
            
            visualDisplayRect = canvasVisual->CommandBufferCoverage(aOrientation);
            
            // Make sure we clip visual rect to visible screen area
            ClipVisualRect(visualDisplayRect, aFullscreen);
            
            // Check if this visual is covered by other opaque visuals which rects are in "covered" region           
            visualRectIsCovered = IsRectCoveredByRegion(visualDisplayRect, iTempRegion);
           
            }

        
        visualIsActive = canvasVisual->iOpacity.Now() > 0.01; // in fact this does not mean that visual should be drawn, but this atleast prevents disabled "on top" windows to be considered as visible.
        
        // Sprites and effects as we consider them always as transparent and also
        // if controlgroup is transformed somehow    
        if (aControl.Role() == EHuiFullScreenEffectContainer 
            || aControl.Role() == EHuiWindowFloatingSpriteContainer ||
            aControlGroup.IsTransformed())
            {
            visualIsOpaque = EFalse;    
            }
    
        if (visualIsActive && visualIsOpaque && !visualRectIsCovered 
                && !(canvasVisual->CanvasFlags() & EHuiCanvasFlagDistractionWindow) // exclude the akn clearer window from heuristics 
                )
            { 
            // New visibility system takes only window shape into account.
            if ( canvasVisual->CanvasFlags() & EHuiCanvasFlagIncludeToVisibilityCalculation )
                {                    
				iTempRegion.AddRect(visualDisplayRect);
                iTempRegion.Tidy();
                __ALFFXLOGSTRING4("CAlfBridge::IsFullScreenDrawnRecursive - Added covered area (%d,%d)-(%d,%d)", visualDisplayRect.iTl.iX, visualDisplayRect.iTl.iY , visualDisplayRect.iBr.iX, visualDisplayRect.iBr.iY );
                }
            }                                   
        visualTreeActive |= visualIsActive;
        
        // iVisibleVisualCount is cleared in HandleVisualVisibility()
        AMT_INC_COUNTER_IF( visualIsActive && !visualRectIsCovered , iVisibleVisualCount );
        } // for loop end : children checking loop
    return visualTreeActive;
    }


void CAlfBridge::LayoutSwitchStart()
    {
    // mark all the application surfaces hidden
    if (!iAlfScreens.Count())
        {
        return;    
        }
    iLayoutSwitchInProgress = ETrue;
    CAlfScreen* screen = iAlfScreens[0];
    // skip the topmost (effect) layer, start from floating sprite group
    for (TInt j=screen->iDisplay->Roster().Count() - screen->FixedControlGroupCount(); j>=0; j--)
        {                
        CHuiControlGroup& controlgroup = screen->iDisplay->Roster().ControlGroup(j);
        CHuiControl& control = controlgroup.Control(0);

        if (control.Role() == EHuiFpsIndicatorContainer || control.Role() == EHuiSessionContainer)
            {
            // FPS container doesn't contain canvas visuals and alfsession containers cannot have surfaces
            continue;
            }
        
        CHuiCanvasVisual* layout = (CHuiCanvasVisual*)&control.Visual(0);
        MarkAllLayersHiddenRecursive(layout);
        }
    
    }

void CAlfBridge::MarkAllLayersHiddenRecursive(CHuiLayout* aLayout)
    {
    CHuiCanvasVisual* canvasVisual = NULL;

    for (TInt i = aLayout->Count()-1; i >= 0; i--)
        {
        canvasVisual = (CHuiCanvasVisual*)(&aLayout->Visual(i));
        if(canvasVisual)
            {
            if (canvasVisual->Count())
                {
                MarkAllLayersHiddenRecursive(canvasVisual);
                }            
    
            if (!canvasVisual->LayerExtent().IsEmpty())
                {
                // Force update
                canvasVisual->ClearCanvasFlags( EHuiCanvasFlagSurfaceInvisible );
                canvasVisual->ClearCanvasFlags( EHuiCanvasFlagSurfaceVisible );
                }
            }
        }
    }

void CAlfBridge::LayoutSwitchComplete()
    {
    // send notification to alfstreamer server
    /*if (iActivated)
        {
        iBridgerClient.SendBlind(KAlfCompositionLayoutSwitchComplete, TIpcArgs());
        }
        */
    // request syncronized frame
    iLayoutSwitchInProgress = EFalse;
    if ( iHuiEnv )
        {
        iIdForLayoutSwitchFrameSync = iIdForEAlfDSSynchronizeOp - KPreventingCollisionOfIdsInSynchDrawRequests;
        iHuiEnv->Synchronize( iIdForLayoutSwitchFrameSync, this );
        AsynchRefresh(); // In SW mode, device is in manual refresh mode - enforce refresh in order to get synchronize done.
        }
    }

TAlfBridgeDrawerWrapper::TAlfBridgeDrawerWrapper(CAlfBridge& aBridge)
    : iBridge( aBridge )
    {
    }
    
TInt TAlfBridgeDrawerWrapper::GetSizeAndRotation(TSize& aSize, TInt& aRotation)
    {
    return iBridge.GetSizeAndRotation(aSize, aRotation);
    }
    
TInt TAlfBridgeDrawerWrapper::ReadPixels(CFbsBitmap* aBitmap)
    {
    return iBridge.ReadPixels(aBitmap);
    }

RAlfBridgerClient* CAlfBridge::BridgerClient()
    {
    if (iActivated)
        {
        return &iBridgerClient;
        }
    return 0;      
    }

void CAlfBridge::RefreshNow(TBool aSyncWait)
    {
    if (!iHuiEnv)
        {
        return;
        }

    if (iSwRenderingEnabled && iManualRefreshTimer)
        {
        iManualRefreshTimer->Cancel();
        }

    if (aSyncWait)
        {
        iHuiEnv->Synchronize( KRequestFrameSynchForComposition, this );
        }
        
    iHuiEnv->RefreshCallBack((TAny*)iHuiEnv);
    }

static TInt DoAlfBridgeRefreshNow(TAny* aAny)
    {
    CAlfBridge* bridge = static_cast<CAlfBridge*>(aAny);
    bridge->RefreshNow(EFalse);
    return KErrNone;
    }

void CAlfBridge::AsynchRefresh()
    {
    if (iSwRenderingEnabled && iManualRefreshTimer && !iManualRefreshTimer->IsActive())
        {
        iManualRefreshTimer->Start(KAlfManualRefreshTimeout, KAlfManualRefreshTimeout, TCallBack(DoAlfBridgeRefreshNow, this));
        }
    }

TBool CAlfBridge::HasActiveAlfContent( TInt aClientWgId )
    {
    TBool hasAlfContent = EFalse;
    if(iAppUi)
        {
        CAlfAppSrvSessionBase* activeAlfSession = iAppUi->ActiveSession();
        CAlfScreen* screen = iAlfScreens[0];
        for(TInt i = 0 ; i < screen->iDisplay->Roster().Count() - screen->FixedControlGroupCount() ; i++)
            {
            CHuiControlGroup& controlgroup = screen->iDisplay->Roster().ControlGroup(i);
            CHuiControl& control = controlgroup.Control(0);
            if (control.Role() == EHuiSessionContainer)
                {
                CAlfAppSrvSessionBase* alfSrvSession = iAppUi->SrvSessionForControlGroup(controlgroup);
                if(alfSrvSession && 
                   (alfSrvSession->ClientWindowGroup() == aClientWgId))
                    {
                    hasAlfContent = ETrue;
                    break;
                    }
                }
            }
        }
    return hasAlfContent;
    }

// end of file
