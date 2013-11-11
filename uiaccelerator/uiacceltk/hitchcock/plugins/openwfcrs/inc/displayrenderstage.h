// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __DISPLAYRENDERSTAGE_H__
#define __DISPLAYRENDERSTAGE_H__

#include <graphics/surfaceupdateclient.h> 
#include <graphics/wsrenderstage.h>
#include <graphics/wsscreendevice.h>
#include <graphics/wsdisplaycontrol.h>
#include <graphics/wsdisplaymapping.h>
#include <graphics/wsdisplaypolicy.h>
#include <graphics/wsscene.h>
#include <graphics/wselement.h>
#include <alf/alfcompositionclient.h>
#include "alfstreamerconsts.h"
#include "alfrenderstageutils.h"

class RSgImageCollection;
class CDisplayPolicy;
class CRsDisplayChangeNotifier;
class CRsConfigChangeNotifier;
class COpenWfcWrapper;
class CScreenCaptureUtil;
class CElementWrapper;
    


/**
This is the implementation of CWsRenderStage which is created by a CDisplayRenderStageFactory
*/
class CDisplayRenderStage : 
        public CWsRenderStage,
        public MWsScreenDevice,
        public MWsDisplayControl,
        public MWsDisplayMapping,
        public MWsDisplayPolicy,
        public MWsScene,
        public MWsWindowTreeObserver,
        public MAlfCompositionController
    {
friend class COpenWfcWrapper;

public:
    static CDisplayRenderStage* NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber);
    ~CDisplayRenderStage();

public: //from CWsRenderStage
    TAny* ResolveObjectInterface(TUint aTypeId);
    void Begin(const TRegion* aRegion);
    void End(TRequestStatus* aFinished);    

public: //from MWsScreenDevice
    TInt ScreenNumber() const;
    TDisplayMode DisplayMode() const;
    TSize SizeInPixels() const;
    TSize SizeInTwips() const;
    TRect PointerRect() const;
    void GetPixel(TRgb& aColor, const TPoint& aPixel) const;
    void GetScanLine(TDes8& aScanLine, const TPoint& aStartPixel, TInt aPixelLength, TDisplayMode aDispMode) const;
    TBool RectCompare(const TRect& aRect1, const TRect& aRect2) const;
    void CopyScreenToBitmapL(CFbsBitmap* aBitmap, const TRect &aRect) const;
    void ClearDisplay(TRgb aBackColor);
    
public: //from MWsScene
    TInt SetSceneRotation(const MWsScene::TSceneRotation aSceneRotation);
    MWsScene::TSceneRotation SceneRotation() const;
    MWsElement* CreateSceneElementL();
    void DestroySceneElement(MWsElement* aElement);
    TInt InsertSceneElement(MWsElement* aInsertElement, MWsElement* aSubordinateElement);
    TInt RemoveSceneElement(MWsElement* aRemoveElement);
    void ComposePendingScene(TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted);
    TInt RegisterSurface(const TSurfaceId& aSurface);
    TInt UnregisterSurface(const TSurfaceId& aSurface);
    
public: // from MWsDisplayControl/MDisplayControlBase
    void NotifyOnDisplayChange(TRequestStatus& aStatus);
    void NotifyOnDisplayChangeCancel();
    void NotifyOnConfigChange(TRequestStatus& aStatus);
    void NotifyOnConfigChangeCancel();
    TInt NumberOfResolutions() const;
    TInt GetResolutions(RArray<TResolution>& aResolutions) const;
    void GetConfiguration(TDisplayConfiguration& aConfig) const;
    TInt SetConfigurationAndBuffer(const TDisplayConfiguration& aConfig,const TSize* aUiSize);
    TInt SetConfiguration(const TDisplayConfiguration& aConfig);
    TInt PreferredDisplayVersion() const;
    
public: //from MWsDisplayMapping/MDisplayMapping
    virtual TInt MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const;
    void SetSizeModeExtent(TRect& aExtent, TBitFlags32 aContext);
    
public://from MWsDisplayPolicy
    virtual void NewAppModesAvailable(); 
    TInt GetSizeModeConfiguration(TInt aScreenSizeMode, TDisplayConfiguration& aConfig, TRect& aSizeModePosition) const;
    TInt SuitableAppMode(TDisplayStatus aSituation);
    void SetLastAppMode(TInt aMode);
    
public: 
    
    void NodeCreated(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode* aParent) { if (iWindowTreeObserver) iWindowTreeObserver->NodeCreated(aWindowTreeNode, aParent);}
    void NodeReleased(const MWsWindowTreeNode& aWindowTreeNode) { if (iWindowTreeObserver) iWindowTreeObserver->NodeReleased(aWindowTreeNode);}
    void NodeActivated(const MWsWindowTreeNode& aWindowTreeNode) { if (iWindowTreeObserver) iWindowTreeObserver->NodeActivated(aWindowTreeNode);}
    void NodeExtentChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRect& aRect) { if (iWindowTreeObserver) iWindowTreeObserver->NodeExtentChanged(aWindowTreeNode, aRect);}
    void SiblingOrderChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aNewPos) { if (iWindowTreeObserver) iWindowTreeObserver->SiblingOrderChanged(aWindowTreeNode, aNewPos);}
    void FlagChanged(const MWsWindowTreeNode& aWindowTreeNode, TFlags aFlag, TBool aNewValue) { if (iWindowTreeObserver) iWindowTreeObserver->FlagChanged(aWindowTreeNode,aFlag, aNewValue);}
    void AttributeChanged(const MWsWindowTreeNode& aWindowTreeNode, TAttributes aAttribute) { if (iWindowTreeObserver) iWindowTreeObserver->AttributeChanged(aWindowTreeNode, aAttribute);}
    void FadeCountChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFadeCount) { if (iWindowTreeObserver) iWindowTreeObserver->FadeCountChanged(aWindowTreeNode, aFadeCount);}
    void TransparentRegionChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aNewTransparentRegion, const TRegion* aNewOpaqueRegion) { if (iWindowTreeObserver) iWindowTreeObserver->TransparentRegionChanged(aWindowTreeNode, aNewTransparentRegion, aNewOpaqueRegion);}
    void ElementAdded(const MWsWindowTreeNode& aWindowTreeNode, const MWsElement& aElement);
    void MovedToWindowGroup(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode& aNewWindowGroupNode) { if (iWindowTreeObserver) iWindowTreeObserver->MovedToWindowGroup(aWindowTreeNode, aNewWindowGroupNode);}
    void WindowGroupChained(const MWsWindowTreeNode& aParent, const MWsWindowTreeNode& aChild) { if (iWindowTreeObserver) iWindowTreeObserver->WindowGroupChained(aParent, aChild);}
    void WindowGroupChainBrokenAfter(const MWsWindowTreeNode& aWindowGroupNode) { if (iWindowTreeObserver) iWindowTreeObserver->WindowGroupChainBrokenAfter(aWindowGroupNode);}
    void FadeAllChildren(const MWsWindowTreeNode& aWindowTreeNode, TBool aFaded) { if (iWindowTreeObserver) iWindowTreeObserver->FadeAllChildren(aWindowTreeNode, aFaded);}
            
public: //MAlfCompositionController
    TInt EnableAlpha(TBool aEnable = ETrue);
    TInt SetOpacity(TReal32 aOpacity) __SOFTFP;
    TInt SetRotation(TInt aRotationInDegrees);
    TInt SetExtent(const TRect& aRect, TInt aScreen);
    void SetIsBackgroundAnim(TBool ETrue);    
    TInt SetSourceRect(const TRect& aRect);   
    TInt CreateTargetL(TInt aTarget, TInt aClientHandle, TInt aGrpHandle);    
    void SetTargetL(TInt aTarget);    
    void SetExtentSurfaceId(const TSurfaceId& aSurfaceId);
    TInt SetOrder(TInt aTarget, TInt aTop, TBool aAbove, TBool aCombineTargets);
    void DeleteTarget(TInt aTarget);
    void LowOnGraphicsMemory();
    void GraphicsMemoryGood();
    void AlfBridgeCallback(TInt aEvent, TAny* aData);
    
public:     
    
private:
    CDisplayRenderStage(TInt aScreenNumber);
    void ConstructL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber);
    void ElementUseAlphaFlagChangedL(MWsElement& aElement, TBool aEnabled);
    MWsElement* FindElement(TInt aWindowGroupId, TInt aWindowHandle);
    CElementWrapper* FindElementTarget(TInt aTarget);
    void AlfWindowListenerCallback( TAlfNativeWindowData* aData );
    void SetTarget(MWsElement* aElement);
private:
    TInt iScreenNumber;
    TTimeStamp iTimeStamp;
    RSurfaceUpdateSession iSurfaceUpdateSession;
    const TRegion* iUpdateRegion;
    //CDirectGdiDriver* iDGdiDriver;
    //CRenderTarget* iRenderTarget;
    COpenWfcWrapper* iOpenWfcWrapper;
    MWsDisplayControl*  iOpenWfcWrapperDisplayControl;
        
    CDisplayPolicy* iDisplayPolicy;
    TBool iDisplayPolicyEnabled;
    
    //required for display and config change notification
    CRsDisplayChangeNotifier* iDispNotifier;
    CRsConfigChangeNotifier* iConfigNotifier;
    
    TBool iRunningOnLowMem;
    MAlfBridge* iAlfBridge;
    MWsElement* iAlfElement;
    MWsElement* iUiElement;
    CElementWrapper* iTargetElement;
    MAlfCompositionAgnosticWindowTreeObserver* iWindowTreeObserver;

    struct TElementData
        {
        MWsElement* iElement;
        TSurfaceId iSurfaceId;
        TUint32 iWindowGrpId; 
        TUint32 iWindowHandle; 
        TUint32 iWindowTreeNodeId; 
        
        TElementData(MWsElement* aElement, TSurfaceId aSurfaceId, TInt aWindowGrpId, TUint32 aWindowHandle, TUint32 aWindowTreeNodeId)
            {
            iElement = aElement;
            iSurfaceId = aSurfaceId;
            iWindowGrpId = aWindowGrpId;
            iWindowHandle = aWindowHandle;
            iWindowTreeNodeId = aWindowTreeNodeId;
            }
        };
    
    RArray<TElementData> iElementArray;
    
    struct SElementTarget
      { 
      TInt iTarget;
      CElementWrapper* iElement;
      };
      
    TSurfaceId iExtentSurfaceId;
      
    RArray<SElementTarget> iElementTargets;
public:
    mutable CScreenCaptureUtil* iScreenCaptureUtil;
    };

#endif //__DISPLAYRENDERSTAGE_H__
