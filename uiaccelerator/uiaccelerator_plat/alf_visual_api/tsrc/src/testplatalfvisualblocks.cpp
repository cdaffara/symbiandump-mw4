/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  test function implement of CTestPlatAlfVisual
*
*/



// [INCLUDE FILES]
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>

#include "testplatalfvisual.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestPlatAlfVisual::RunMethodL( CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        //FOR TEST alfanchorlayout.h        
        ENTRY( "TestAnchorLayoutAddNewL", CTestPlatAlfVisual::TestAnchorLayoutAddNewL ),
        ENTRY( "TestAnchorLayoutConstructorL", CTestPlatAlfVisual::TestAnchorLayoutConstructorL ),
        ENTRY( "TestAnchorLayoutConstructL", CTestPlatAlfVisual::TestAnchorLayoutConstructL ),
        ENTRY( "TestAnchorLayoutResetL", CTestPlatAlfVisual::TestAnchorLayoutResetL ),
        ENTRY( "TestAnchorLayoutSetAnchorL", CTestPlatAlfVisual::TestAnchorLayoutSetAnchorL ),
        ENTRY( "TestAnchorLayoutSetRelativeAnchorRectL", CTestPlatAlfVisual::TestAnchorLayoutSetRelativeAnchorRectL ),
        ENTRY( "TestAnchorLayoutRemoveAnchorL", CTestPlatAlfVisual::TestAnchorLayoutRemoveAnchorL ),
        ENTRY( "TestAnchorLayoutAttachEdgeL", CTestPlatAlfVisual::TestAnchorLayoutAttachEdgeL ),
        ENTRY( "TestAnchorLayoutAttachCornerL", CTestPlatAlfVisual::TestAnchorLayoutAttachCornerL ),
        ENTRY( "TestAnchorLayoutAttachBoxL", CTestPlatAlfVisual::TestAnchorLayoutAttachBoxL ),
        ENTRY( "TestAnchorLayoutDetachL", CTestPlatAlfVisual::TestAnchorLayoutDetachL ),
        ENTRY( "TestAnchorLayoutDetachTypeL", CTestPlatAlfVisual::TestAnchorLayoutDetachTypeL ),
        ENTRY( "TestAnchorLayoutRemoveAndDestroyAllDL", CTestPlatAlfVisual::TestAnchorLayoutRemoveAndDestroyAllDL ),
        ENTRY( "TestAnchorLayoutUpdateChildrenLayoutL", CTestPlatAlfVisual::TestAnchorLayoutUpdateChildrenLayoutL ),
        ENTRY( "TestAnchorLayoutFindTagL", CTestPlatAlfVisual::TestAnchorLayoutFindTagL ),
        ENTRY( "TestAnchorLayoutBaseUnitL", CTestPlatAlfVisual::TestAnchorLayoutBaseUnitL ),
        ENTRY( "TestAnchorLayoutDoRemoveAndDestroyAllDL", CTestPlatAlfVisual::TestAnchorLayoutDoRemoveAndDestroyAllDL ),
        ENTRY( "TestAnchorLayoutPropertyOwnerExtensionL", CTestPlatAlfVisual::TestAnchorLayoutPropertyOwnerExtensionL ),
        
        //FOR TEST alfvisual.h
        ENTRY( "TestCAlfVCAlfVisualL", CTestPlatAlfVisual::TestCAlfVCAlfVisualL ),
        ENTRY( "TestCAlfVConstructL", CTestPlatAlfVisual::TestCAlfVConstructL ),
        ENTRY( "TestCAlfVRemoveAndDestroyAllDL", CTestPlatAlfVisual::TestCAlfVRemoveAndDestroyAllDL ),
        ENTRY( "TestCAlfVIdentifierL", CTestPlatAlfVisual::TestCAlfVIdentifierL ),
        ENTRY( "TestCAlfVSetFlagL", CTestPlatAlfVisual::TestCAlfVSetFlagL ),
        ENTRY( "TestCAlfVSetFlagsL", CTestPlatAlfVisual::TestCAlfVSetFlagsL ),
        ENTRY( "TestCAlfVClearFlagL", CTestPlatAlfVisual::TestCAlfVClearFlagL ),
        ENTRY( "TestCAlfVClearFlagsL", CTestPlatAlfVisual::TestCAlfVClearFlagsL ),
        ENTRY( "TestCAlfVFlagsL", CTestPlatAlfVisual::TestCAlfVFlagsL ),
        ENTRY( "TestCAlfVLayoutL", CTestPlatAlfVisual::TestCAlfVLayoutL ),
        ENTRY( "TestCAlfVPosL", CTestPlatAlfVisual::TestCAlfVPosL ),
        ENTRY( "TestCAlfVSetPosL", CTestPlatAlfVisual::TestCAlfVSetPosL ),
        ENTRY( "TestCAlfVSizeL", CTestPlatAlfVisual::TestCAlfVSizeL ),
        ENTRY( "TestCAlfVSetSizeL", CTestPlatAlfVisual::TestCAlfVSetSizeL ),
        ENTRY( "TestCAlfVOpacityL", CTestPlatAlfVisual::TestCAlfVOpacityL ),
        ENTRY( "TestCAlfVSetOpacityL", CTestPlatAlfVisual::TestCAlfVSetOpacityL ),
        ENTRY( "TestCAlfVEnvL", CTestPlatAlfVisual::TestCAlfVEnvL ),
        ENTRY( "TestCAlfVDisplayL", CTestPlatAlfVisual::TestCAlfVDisplayL ),
        ENTRY( "TestCAlfVTagL", CTestPlatAlfVisual::TestCAlfVTagL ),
        ENTRY( "TestCAlfVUpdateChildrenLayoutL", CTestPlatAlfVisual::TestCAlfVUpdateChildrenLayoutL ),
        ENTRY( "TestCAlfVDisplayRectTargetL", CTestPlatAlfVisual::TestCAlfVDisplayRectTargetL ),
        ENTRY( "TestCAlfVLocalToDisplayL", CTestPlatAlfVisual::TestCAlfVLocalToDisplayL ),
        ENTRY( "TestCAlfVLocalToDisplayWithRealPointL", CTestPlatAlfVisual::TestCAlfVLocalToDisplayWithRealPointL ),
        ENTRY( "TestCAlfVDisplayToLocalL", CTestPlatAlfVisual::TestCAlfVDisplayToLocalL ),
        ENTRY( "TestCAlfVDisplayToLocalWithRealPointL", CTestPlatAlfVisual::TestCAlfVDisplayToLocalWithRealPointL ),
        ENTRY( "TestCAlfVEnableBrushesL", CTestPlatAlfVisual::TestCAlfVEnableBrushesL ),
        ENTRY( "TestCAlfVBrushesL", CTestPlatAlfVisual::TestCAlfVBrushesL ),
        ENTRY( "TestCAlfVMoveToFrontL", CTestPlatAlfVisual::TestCAlfVMoveToFrontL ),
        ENTRY( "TestCAlfVEnableTransformationL", CTestPlatAlfVisual::TestCAlfVEnableTransformationL ),
        ENTRY( "TestCAlfVTransformationL", CTestPlatAlfVisual::TestCAlfVTransformationL ),
        ENTRY( "TestCAlfVSetPaddingL", CTestPlatAlfVisual::TestCAlfVSetPaddingL ),
        ENTRY( "TestCAlfVSetPaddingWithPointL", CTestPlatAlfVisual::TestCAlfVSetPaddingWithPointL ),
        ENTRY( "TestCAlfVSetPaddingWithMetricL", CTestPlatAlfVisual::TestCAlfVSetPaddingWithMetricL ),
        ENTRY( "TestCAlfVPaddingInPixelsL", CTestPlatAlfVisual::TestCAlfVPaddingInPixelsL ),
        ENTRY( "TestCAlfVSetTagL", CTestPlatAlfVisual::TestCAlfVSetTagL ),
        ENTRY( "TestCAlfVIsTaggedL", CTestPlatAlfVisual::TestCAlfVIsTaggedL ),
        ENTRY( "TestCAlfVFindTagL", CTestPlatAlfVisual::TestCAlfVFindTagL ),
        ENTRY( "TestCAlfVConstructWithMoreParamsL", CTestPlatAlfVisual::TestCAlfVConstructWithMoreParamsL ),
        ENTRY( "TestCAlfVCommsL", CTestPlatAlfVisual::TestCAlfVCommsL ),
        ENTRY( "TestCAlfVDepthOffsetL", CTestPlatAlfVisual::TestCAlfVDepthOffsetL ),
        ENTRY( "TestCAlfVSetDepthOffsetL", CTestPlatAlfVisual::TestCAlfVSetDepthOffsetL ),
        ENTRY( "TestCAlfVOwnerL", CTestPlatAlfVisual::TestCAlfVOwnerL ),
        ENTRY( "TestCAlfVSetOwnerL", CTestPlatAlfVisual::TestCAlfVSetOwnerL ),
        ENTRY( "TestCAlfVSetOriginL", CTestPlatAlfVisual::TestCAlfVSetOriginL ),
        ENTRY( "TestCAlfVHorizontalOriginL", CTestPlatAlfVisual::TestCAlfVHorizontalOriginL ),
        ENTRY( "TestCAlfVVerticalOriginL", CTestPlatAlfVisual::TestCAlfVVerticalOriginL ),
        ENTRY( "TestCAlfVAlignByOriginL", CTestPlatAlfVisual::TestCAlfVAlignByOriginL ),
        ENTRY( "TestCAlfVSetMinSizeL", CTestPlatAlfVisual::TestCAlfVSetMinSizeL ),
        ENTRY( "TestCAlfVSetMinSizeWithRealSizeL", CTestPlatAlfVisual::TestCAlfVSetMinSizeWithRealSizeL ),
        ENTRY( "TestCAlfVMinSizeL", CTestPlatAlfVisual::TestCAlfVMinSizeL ),
        ENTRY( "TestCAlfVMinSizeInBaseUnitsL", CTestPlatAlfVisual::TestCAlfVMinSizeInBaseUnitsL ),
        ENTRY( "TestCAlfVSetMaxSizeL", CTestPlatAlfVisual::TestCAlfVSetMaxSizeL ),
        ENTRY( "TestCAlfVSetMaxSizeWithRealSizeL", CTestPlatAlfVisual::TestCAlfVSetMaxSizeWithRealSizeL ),
        ENTRY( "TestCAlfVMaxSizeL", CTestPlatAlfVisual::TestCAlfVMaxSizeL ),
        ENTRY( "TestCAlfVMaxSizeInBaseUnitsL", CTestPlatAlfVisual::TestCAlfVMaxSizeInBaseUnitsL ),
        ENTRY( "TestCAlfVSetRectL", CTestPlatAlfVisual::TestCAlfVSetRectL ),
        ENTRY( "TestCAlfVMoveL", CTestPlatAlfVisual::TestCAlfVMoveL ),
        ENTRY( "TestCAlfVMoveWithRealPointL", CTestPlatAlfVisual::TestCAlfVMoveWithRealPointL ),
        ENTRY( "TestCAlfVMimicL", CTestPlatAlfVisual::TestCAlfVMimicL ),
        ENTRY( "TestCAlfVSetCenteredPosAndSizeL", CTestPlatAlfVisual::TestCAlfVSetCenteredPosAndSizeL ),
        ENTRY( "TestCAlfVSetCenteredPosAndSizeWithRealL", CTestPlatAlfVisual::TestCAlfVSetCenteredPosAndSizeWithRealL ),
        ENTRY( "TestCAlfVDisplayRectL", CTestPlatAlfVisual::TestCAlfVDisplayRectL ),
        ENTRY( "TestCAlfVEffectiveOpacityL", CTestPlatAlfVisual::TestCAlfVEffectiveOpacityL ),
        ENTRY( "TestCAlfVSetClippingL", CTestPlatAlfVisual::TestCAlfVSetClippingL ),
        ENTRY( "TestCAlfVClippingL", CTestPlatAlfVisual::TestCAlfVClippingL ),
        ENTRY( "TestCAlfVPaddingL", CTestPlatAlfVisual::TestCAlfVPaddingL ),
        ENTRY( "TestCAlfVPaddingAsMetricL", CTestPlatAlfVisual::TestCAlfVPaddingAsMetricL ),
        ENTRY( "TestCAlfVHorizontalPaddingL", CTestPlatAlfVisual::TestCAlfVHorizontalPaddingL ),
        ENTRY( "TestCAlfVVerticalPaddingL", CTestPlatAlfVisual::TestCAlfVVerticalPaddingL ),
        ENTRY( "TestCAlfVSetPosWithTimedPointL", CTestPlatAlfVisual::TestCAlfVSetPosWithTimedPointL ),
        ENTRY( "TestCAlfVSetSizeWithTimedPointL", CTestPlatAlfVisual::TestCAlfVSetSizeWithTimedPointL ),
        ENTRY( "TestCAlfVCopyValuesL", CTestPlatAlfVisual::TestCAlfVCopyValuesL ),
        ENTRY( "TestCAlfVPauseL", CTestPlatAlfVisual::TestCAlfVPauseL ),
        ENTRY( "TestCAlfVResumeL", CTestPlatAlfVisual::TestCAlfVResumeL ),
        ENTRY( "TestCAlfVDoRemoveAndDestroyAllDL", CTestPlatAlfVisual::TestCAlfVDoRemoveAndDestroyAllDL ),
        ENTRY( "TestCAlfVSetTactileFeedbackL", CTestPlatAlfVisual::TestCAlfVSetTactileFeedbackL ),
        ENTRY( "TestCAlfVHasTactileFeedbackL", CTestPlatAlfVisual::TestCAlfVHasTactileFeedbackL ),
        ENTRY( "TestCAlfVRemoveTactileFeedbackL", CTestPlatAlfVisual::TestCAlfVRemoveTactileFeedbackL ),
        ENTRY( "TestCAlfVEnableDropShadowL", CTestPlatAlfVisual::TestCAlfVEnableDropShadowL ),
        ENTRY( "TestCAlfVDropShadowHandlerL", CTestPlatAlfVisual::TestCAlfVDropShadowHandlerL ),
        ENTRY( "TestCAlfVPropertyOwnerExtensionL", CTestPlatAlfVisual::TestCAlfVPropertyOwnerExtensionL ),
        
        //FOR TEST alfvisualfactory.h
        ENTRY( "TestAlfVFNewLayoutL", CTestPlatAlfVisual::TestAlfVFNewLayoutL ),
        ENTRY( "TestAlfVFNewLayoutLCL", CTestPlatAlfVisual::TestAlfVFNewLayoutLCL ),
        ENTRY( "TestAlfVFNewVisualL", CTestPlatAlfVisual::TestAlfVFNewVisualL ),
        ENTRY( "TestAlfVFNewVisualLCL", CTestPlatAlfVisual::TestAlfVFNewVisualLCL ),
        
        //FOR TEST alfcontrol.h
        ENTRY( "TestAlfCtrlConstructorL", CTestPlatAlfVisual::TestAlfCtrlConstructorL ),
        ENTRY( "TestAlfCtrlConstructL", CTestPlatAlfVisual::TestAlfCtrlConstructL ),
        ENTRY( "TestAlfCtrlIdL", CTestPlatAlfVisual::TestAlfCtrlIdL ),
        ENTRY( "TestAlfCtrlGetCtrlMember", CTestPlatAlfVisual::TestAlfCtrlGetCtrlMember ),
        ENTRY( "TestAlfCtrlProcessVisualL", CTestPlatAlfVisual::TestAlfCtrlProcessVisualL ),
        ENTRY( "TestAlfCtrlProcessConnectL", CTestPlatAlfVisual::TestAlfCtrlProcessConnectL ),
        ENTRY( "TestAlfCtrlProcessRole", CTestPlatAlfVisual::TestAlfCtrlProcessRole ),
        ENTRY( "TestAlfCtrlProcessHostL", CTestPlatAlfVisual::TestAlfCtrlProcessHostL ),
        ENTRY( "TestAlfCtrlHostDisplayL", CTestPlatAlfVisual::TestAlfCtrlHostDisplayL ),
        ENTRY( "TestAlfCtrlDisplayPos", CTestPlatAlfVisual::TestAlfCtrlDisplayPos ),
        ENTRY( "TestAlfCtrlEventL", CTestPlatAlfVisual::TestAlfCtrlEventL ),
        ENTRY( "TestAlfCtrlProcessFocus", CTestPlatAlfVisual::TestAlfCtrlProcessFocus ),
        ENTRY( "TestAlfCtrlLayoutL", CTestPlatAlfVisual::TestAlfCtrlLayoutL ),
        ENTRY( "TestAlfCtrlIdentifier", CTestPlatAlfVisual::TestAlfCtrlIdentifier ),
        ENTRY( "TestAlfCtrlBounds", CTestPlatAlfVisual::TestAlfCtrlBounds ),
        ENTRY( "TestAlfCtrlShowL", CTestPlatAlfVisual::TestAlfCtrlShowL ),
        ENTRY( "TestAlfCtrlAddAndRemoveVisualL", CTestPlatAlfVisual::TestAlfCtrlAddAndRemoveVisualL ),
        ENTRY( "TestAlfCtrlNotifyControlVisibilityL", CTestPlatAlfVisual::TestAlfCtrlNotifyControlVisibilityL ),
        ENTRY( "TestAlfCtrlAddAndRemoveConnectionL", CTestPlatAlfVisual::TestAlfCtrlAddAndRemoveConnectionL ),
        ENTRY( "TestAlfCtrlHostChangingL", CTestPlatAlfVisual::TestAlfCtrlHostChangingL ),
        ENTRY( "TestAlfCtrlPropertyOwnerExtensionL", CTestPlatAlfVisual::TestAlfCtrlPropertyOwnerExtensionL ),
        
        //FOR TEST alfcontrolgroup.h
        ENTRY( "TestAlfCtrlGroupResourceId", CTestPlatAlfVisual::TestAlfCtrlGroupResourceId ),
        ENTRY( "TestAlfCtrlGroupProcessCtrlL", CTestPlatAlfVisual::TestAlfCtrlGroupProcessCtrlL ),
        ENTRY( "TestAlfCtrlGroupProcessInput", CTestPlatAlfVisual::TestAlfCtrlGroupProcessInput ),
        ENTRY( "TestAlfCtrlGroupProcessTransformL", CTestPlatAlfVisual::TestAlfCtrlGroupProcessTransformL ),
        
        //FOR TEST alfcurvepath.h
        ENTRY( "TestAlfCurvePathNewL", CTestPlatAlfVisual::TestAlfCurvePathNewL ),
        ENTRY( "TestAlfCurvePathLoopL", CTestPlatAlfVisual::TestAlfCurvePathLoopL ),
        ENTRY( "TestAlfCurvePathOffsetL", CTestPlatAlfVisual::TestAlfCurvePathOffsetL ),
        ENTRY( "TestAlfCurvePathProcessPointsL", CTestPlatAlfVisual::TestAlfCurvePathProcessPointsL ),
        ENTRY( "TestAlfCurvePathResetL", CTestPlatAlfVisual::TestAlfCurvePathResetL ),
        ENTRY( "TestAlfCurvePathMappingFunctionIdentifierL", CTestPlatAlfVisual::TestAlfCurvePathMappingFunctionIdentifierL ),
        
        //FOR TEST alfcurvepathlayout.h
        ENTRY( "TestAlfCurvePathLayoutAddNewL", CTestPlatAlfVisual::TestAlfCurvePathLayoutAddNewL ),
        ENTRY( "TestAlfCurvePathLayoutFullConstructL", CTestPlatAlfVisual::TestAlfCurvePathLayoutFullConstructL ),
        ENTRY( "TestAlfCurvePathLayoutForLayoutL", CTestPlatAlfVisual::TestAlfCurvePathLayoutForLayoutL ),
        ENTRY( "TestAlfCurvePathLayoutForVisualL", CTestPlatAlfVisual::TestAlfCurvePathLayoutForVisualL ),
        ENTRY( "TestAlfCurvePathLayoutForMapL", CTestPlatAlfVisual::TestAlfCurvePathLayoutForMapL ),
        ENTRY( "TestAlfCurvePathLayoutProtectedFuncL", CTestPlatAlfVisual::TestAlfCurvePathLayoutProtectedFuncL ),
        
        //FOR TEST alfdecklayout.h
        ENTRY( "TestAlfDeckLayoutAddNewL", CTestPlatAlfVisual::TestAlfDeckLayoutAddNewL ),
        ENTRY( "TestAlfDeckLayoutFullConstructL", CTestPlatAlfVisual::TestAlfDeckLayoutFullConstructL ),
        ENTRY( "TestAlfDeckLayoutForLayoutL", CTestPlatAlfVisual::TestAlfDeckLayoutForLayoutL ),
        ENTRY( "TestAlfDeckLayoutForVisualL", CTestPlatAlfVisual::TestAlfDeckLayoutForVisualL ),
        ENTRY( "TestAlfDeckLayoutProtectedFuncL", CTestPlatAlfVisual::TestAlfDeckLayoutProtectedFuncL ),
        
        //FOR TEST alfdisplay.h
        ENTRY( "TestAlfDisplayForBackgroundL", CTestPlatAlfVisual::TestAlfDisplayForBackgroundL ),
        ENTRY( "TestAlfDisplayForRoster", CTestPlatAlfVisual::TestAlfDisplayForRoster ),
        ENTRY( "TestAlfDisplayForVisibleArea", CTestPlatAlfVisual::TestAlfDisplayForVisibleArea ),
        ENTRY( "TestAlfDisplayForSetAttributesL", CTestPlatAlfVisual::TestAlfDisplayForSetAttributesL ),
        ENTRY( "TestAlfDisplayGetAttributes", CTestPlatAlfVisual::TestAlfDisplayGetAttributes ),
        ENTRY( "TestAlfDisplayFading", CTestPlatAlfVisual::TestAlfDisplayFading ),
        ENTRY( "TestAlfDisplayHandlePointerEventL", CTestPlatAlfVisual::TestAlfDisplayHandlePointerEventL ),
        
        //FOR TEST alfdisplaybackgrounditem.h
        ENTRY( "TestAlfDisplayBgItemConstructor", CTestPlatAlfVisual::TestAlfDisplayBgItemConstructor ),
        ENTRY( "TestAlfDisplayBgItemSetAttributes", CTestPlatAlfVisual::TestAlfDisplayBgItemSetAttributes ),
        
        //FOR TEST alfdropshadow.h
        ENTRY( "TestAlfDropShadowSetAttributesL", CTestPlatAlfVisual::TestAlfDropShadowSetAttributesL ),
        
        //FOR TEST alfdropshadowbrush.h
        ENTRY( "TestAlfDropShadowBrushNewFuncsL", CTestPlatAlfVisual::TestAlfDropShadowBrushNewFuncsL ),
        ENTRY( "TestAlfDropShadowBrushSoftnessL", CTestPlatAlfVisual::TestAlfDropShadowBrushSoftnessL ),
        ENTRY( "TestAlfDropShadowBrushDepthL", CTestPlatAlfVisual::TestAlfDropShadowBrushDepthL ),
        ENTRY( "TestAlfDropShadowBrushSetColorL", CTestPlatAlfVisual::TestAlfDropShadowBrushSetColorL ),
        
        //FOR TEST alfenv.h
        ENTRY( "TestAlfEnvNewFuncsL", CTestPlatAlfVisual::TestAlfEnvNewFuncsL ),
        ENTRY( "TestAlfEnvClient", CTestPlatAlfVisual::TestAlfEnvClient ),
        ENTRY( "TestAlfEnvNewDisplayL", CTestPlatAlfVisual::TestAlfEnvNewDisplayL ),
        ENTRY( "TestAlfEnvCtrlGroupL", CTestPlatAlfVisual::TestAlfEnvCtrlGroupL ),
        ENTRY( "TestAlfEnvProcessFresh", CTestPlatAlfVisual::TestAlfEnvProcessFresh ),
        ENTRY( "TestAlfEnvProcessTextureManagerL", CTestPlatAlfVisual::TestAlfEnvProcessTextureManagerL ),
        ENTRY( "TestAlfEnvObserverL", CTestPlatAlfVisual::TestAlfEnvObserverL ),
        ENTRY( "TestAlfEnvProcessCmd", CTestPlatAlfVisual::TestAlfEnvProcessCmd ),
        ENTRY( "TestAlfEnvKeyAndPointerEventL", CTestPlatAlfVisual::TestAlfEnvKeyAndPointerEventL ),
        ENTRY( "TestAlfEnvNotifyL", CTestPlatAlfVisual::TestAlfEnvNotifyL ),
        ENTRY( "TestAlfEnvProcessWidgetL", CTestPlatAlfVisual::TestAlfEnvProcessWidgetL ),
        ENTRY( "TestAlfEnvGetAttributes", CTestPlatAlfVisual::TestAlfEnvGetAttributes ),
        ENTRY( "TestAlfEnvFindDisplayIndex", CTestPlatAlfVisual::TestAlfEnvFindDisplayIndex ),
        ENTRY( "TestAlfEnvFindControl", CTestPlatAlfVisual::TestAlfEnvFindControl ),
        ENTRY( "TestAlfEnvAddExtension", CTestPlatAlfVisual::TestAlfEnvAddExtension ),
        ENTRY( "TestAlfEnvSetAndReSetL", CTestPlatAlfVisual::TestAlfEnvSetAndReSetL ),
        ENTRY( "TestAlfEnvStatic", CTestPlatAlfVisual::TestAlfEnvStatic ),
        
        //FOR TEST alfevent.h
        ENTRY( "TestAlfEventConstructor", CTestPlatAlfVisual::TestAlfEventConstructor ),
        ENTRY( "TestAlfEventParameters", CTestPlatAlfVisual::TestAlfEventParameters ),
        ENTRY( "TestAlfEventCustomData", CTestPlatAlfVisual::TestAlfEventCustomData ),
        ENTRY( "TestAlfEventDisplayAndVisual", CTestPlatAlfVisual::TestAlfEventDisplayAndVisual ),
        
        //FOR TEST alfflowlayout.h
        ENTRY( "TestAlfFlowLayoutAddNewL", CTestPlatAlfVisual::TestAlfFlowLayoutAddNewL ),
        ENTRY( "TestAlfFlowLayoutFullConstructL", CTestPlatAlfVisual::TestAlfFlowLayoutFullConstructL ),
        ENTRY( "TestAlfFlowLayoutForLayoutL", CTestPlatAlfVisual::TestAlfFlowLayoutForLayoutL ),
        ENTRY( "TestAlfFlowLayoutForVisualL", CTestPlatAlfVisual::TestAlfFlowLayoutForVisualL ),
        ENTRY( "TestAlfFlowLayoutProtectedFuncL", CTestPlatAlfVisual::TestAlfFlowLayoutProtectedFuncL ),
        ENTRY( "TestAlfFlowLayoutSetParametersL", CTestPlatAlfVisual::TestAlfFlowLayoutSetParametersL ),
        
        //FOR TEST alfframebrush.h
        ENTRY( "TestAlfFrameBrushNewL", CTestPlatAlfVisual::TestAlfFrameBrushNewL ),
        ENTRY( "TestAlfFrameBrushSetParaL", CTestPlatAlfVisual::TestAlfFrameBrushSetParaL ),
        
        //FOR TEST alfgencomponent.h
        ENTRY( "TestAlfGenComponentNewL", CTestPlatAlfVisual::TestAlfGenComponentNewL ),
        ENTRY( "TestAlfGenComponentProcessL", CTestPlatAlfVisual::TestAlfGenComponentProcessL ),
        
        //FOR TEST alfgradientbrush.h
        ENTRY( "TestAlfGradientBrushNewL", CTestPlatAlfVisual::TestAlfGradientBrushNewL ),
        ENTRY( "TestAlfGradientBrushSetL", CTestPlatAlfVisual::TestAlfGradientBrushSetL ),
        ENTRY( "TestAlfGradientBrushGetL", CTestPlatAlfVisual::TestAlfGradientBrushGetL ),
        
        //FOR TEST alfgridlayout.h
        ENTRY( "TestAlfGridLayoutAddNewL", CTestPlatAlfVisual::TestAlfGridLayoutAddNewL ),
        ENTRY( "TestAlfGridLayoutFullConstructL", CTestPlatAlfVisual::TestAlfGridLayoutFullConstructL ),
        ENTRY( "TestAlfGridLayoutForLayoutL", CTestPlatAlfVisual::TestAlfGridLayoutForLayoutL ),
        ENTRY( "TestAlfGridLayoutForVisualL", CTestPlatAlfVisual::TestAlfGridLayoutForVisualL ),
        ENTRY( "TestAlfGridLayoutProtectedFuncL", CTestPlatAlfVisual::TestAlfGridLayoutProtectedFuncL ),
        ENTRY( "TestAlfGridLayoutSetParametersL", CTestPlatAlfVisual::TestAlfGridLayoutSetParametersL ),
        ENTRY( "TestAlfGridLayoutGetParaL", CTestPlatAlfVisual::TestAlfGridLayoutGetParaL ),
        
        //FOR TEST alfimage.h
        ENTRY( "TestAlfImageConstructorL", CTestPlatAlfVisual::TestAlfImageConstructorL ),
        ENTRY( "TestAlfImageSetAttributesL", CTestPlatAlfVisual::TestAlfImageSetAttributesL ),
        ENTRY( "TestAlfImageGetAttributesL", CTestPlatAlfVisual::TestAlfImageGetAttributesL ),
        
        //FOR TEST alfimagebrush.h
        ENTRY( "TestAlfImageBrushNewL", CTestPlatAlfVisual::TestAlfImageBrushNewL ),
        ENTRY( "TestAlfImageBrushSetAndGetParaL", CTestPlatAlfVisual::TestAlfImageBrushSetAndGetParaL ),
        
        //FOR TEST alfimageloaderutil.h
        ENTRY( "TestAlfImgLoadUtilConstructorL", CTestPlatAlfVisual::TestAlfImgLoadUtilConstructorL ),
        ENTRY( "TestAlfImgLoadUtilCreateImageLoaderL", CTestPlatAlfVisual::TestAlfImgLoadUtilCreateImageLoaderL ),
        ENTRY( "TestAlfImgLoadUtilSetL", CTestPlatAlfVisual::TestAlfImgLoadUtilSetL ),
        ENTRY( "TestAlfAutoSizeImgLoadUtilSetL", CTestPlatAlfVisual::TestAlfAutoSizeImgLoadUtilSetL ),
        
        //FOR TEST alfimagevisual.h
        ENTRY( "TestAlfImgVisualFullConstructorL", CTestPlatAlfVisual::TestAlfImgVisualFullConstructorL ),
        ENTRY( "TestAlfImgVisualAddNewL", CTestPlatAlfVisual::TestAlfImgVisualAddNewL ),
        ENTRY( "TestAlfImgVisualSetAndGetL", CTestPlatAlfVisual::TestAlfImgVisualSetAndGetL ),
        ENTRY( "TestAlfImgVisualForVisualL", CTestPlatAlfVisual::TestAlfImgVisualForVisualL ),
        ENTRY( "TestAlfImgVisualProtectedFuncL", CTestPlatAlfVisual::TestAlfImgVisualProtectedFuncL ),
        
        //FOR TEST alflayout.h
        ENTRY( "TestAlfLayoutFullConstructorL", CTestPlatAlfVisual::TestAlfLayoutFullConstructorL ),
        ENTRY( "TestAlfLayoutAddNewL", CTestPlatAlfVisual::TestAlfLayoutAddNewL ),
        ENTRY( "TestAlfLayoutSetAndGetL", CTestPlatAlfVisual::TestAlfLayoutSetAndGetL ),
        ENTRY( "TestAlfLayoutForVisualL", CTestPlatAlfVisual::TestAlfLayoutForVisualL ),
        ENTRY( "TestAlfLayoutBaseUnitL", CTestPlatAlfVisual::TestAlfLayoutBaseUnitL ),
        ENTRY( "TestAlfLayoutProtectedFuncL", CTestPlatAlfVisual::TestAlfLayoutProtectedFuncL ),
        
        //FOR TEST alflinevisual.h
        ENTRY( "TestAlfLineVisualFullConstructorL", CTestPlatAlfVisual::TestAlfLineVisualFullConstructorL ),
        ENTRY( "TestAlfLineVisualAddNewL", CTestPlatAlfVisual::TestAlfLineVisualAddNewL ),
        ENTRY( "TestAlfLineVisualSetAndGetL", CTestPlatAlfVisual::TestAlfLineVisualSetAndGetL ),
        ENTRY( "TestAlfLineVisualForVisualL", CTestPlatAlfVisual::TestAlfLineVisualForVisualL ),
        ENTRY( "TestAlfLineVisualProtectedFuncL", CTestPlatAlfVisual::TestAlfLineVisualProtectedFuncL ),
        
        //FOR TEST alfm3gmesh.h
        ENTRY( "TestAlfM3GMeshLoadSceneL", CTestPlatAlfVisual::TestAlfM3GMeshLoadSceneL ),
        
        //FOR TEST alfmappingfunctions.h
        ENTRY( "TestAlfConstantMapFuncL", CTestPlatAlfVisual::TestAlfConstantMapFuncL ),
        ENTRY( "TestAlfLinearMapFuncL", CTestPlatAlfVisual::TestAlfLinearMapFuncL ),
        ENTRY( "TestAlfSineMapFuncL", CTestPlatAlfVisual::TestAlfSineMapFuncL ),
        ENTRY( "TestAlfCosineMapFuncL", CTestPlatAlfVisual::TestAlfCosineMapFuncL ),
        ENTRY( "TestAlfAverageMapFuncL", CTestPlatAlfVisual::TestAlfAverageMapFuncL ),
        ENTRY( "TestAlfTableMapFuncL", CTestPlatAlfVisual::TestAlfTableMapFuncL ),
        
        //FOR TEST alfmaterial.h
        ENTRY( "TestAlfMaterialConstructor", CTestPlatAlfVisual::TestAlfMaterialConstructor ),
        
        //FOR TEST alfmesh.h
        ENTRY( "TestAlfMeshSetAndGetL", CTestPlatAlfVisual::TestAlfMeshSetAndGetL ),
        
        //FOR TEST alfmeshvisual.h
        ENTRY( "TestAlfMeshVisualFullConstructorL", CTestPlatAlfVisual::TestAlfMeshVisualFullConstructorL ),
        ENTRY( "TestAlfMeshVisualAddNewL", CTestPlatAlfVisual::TestAlfMeshVisualAddNewL ),
        ENTRY( "TestAlfMeshVisualSetAndGetL", CTestPlatAlfVisual::TestAlfMeshVisualSetAndGetL ),
        ENTRY( "TestAlfMeshVisualForVisualL", CTestPlatAlfVisual::TestAlfMeshVisualForVisualL ),
        ENTRY( "TestAlfMeshVisualProtectedFuncL", CTestPlatAlfVisual::TestAlfMeshVisualProtectedFuncL ),
        
        //FOR TEST alfbatchbuffer.h
        ENTRY( "TestBBSetAutoFlushModeL", CTestPlatAlfVisual::TestBBSetAutoFlushMode ),
        ENTRY( "TestBBAutoFlushModeL", CTestPlatAlfVisual::TestBBAutoFlushMode ),
        ENTRY( "TestBBFlushBatchBufferL", CTestPlatAlfVisual::TestBBFlushBatchBufferL ),
        ENTRY( "TestBBSetMaxBatchBufferSizeL", CTestPlatAlfVisual::TestBBSetMaxBatchBufferSize ),
        ENTRY( "TestBBGetBatchBufferInfoL", CTestPlatAlfVisual::TestBBGetBatchBufferInfoL ),
        
        //FOR TEST alfborderbrush.h
        ENTRY( "TestBorderBrushNewL", CTestPlatAlfVisual::TestBorderBrushNewL ),
        ENTRY( "TestBorderBrushNewLCL", CTestPlatAlfVisual::TestBorderBrushNewLCL ),
        ENTRY( "TestBorderBrushOffsetNewL", CTestPlatAlfVisual::TestBorderBrushOffsetNewL ),
        ENTRY( "TestBorderBrushOffsetNewLCL", CTestPlatAlfVisual::TestBorderBrushOffsetNewLCL ),
        ENTRY( "TestBorderBrushSetImageL", CTestPlatAlfVisual::TestBorderBrushSetImageL ),
        ENTRY( "TestBorderBrushImageL", CTestPlatAlfVisual::TestBorderBrushImageL ),
        ENTRY( "TestBorderBrushSetColorL", CTestPlatAlfVisual::TestBorderBrushSetColorL ),
        ENTRY( "TestBorderBrushThicknessL", CTestPlatAlfVisual::TestBorderBrushThicknessL ),
        ENTRY( "TestBorderBrushSetThicknessL", CTestPlatAlfVisual::TestBorderBrushSetThicknessL ),
        ENTRY( "TestBorderBrushSetThicknessMetricL", CTestPlatAlfVisual::TestBorderBrushSetThicknessMetricL ),
        ENTRY( "TestBorderBrushEdgeOffsetL", CTestPlatAlfVisual::TestBorderBrushEdgeOffsetL ),
        ENTRY( "TestBorderBrushSetEdgeOffsetL", CTestPlatAlfVisual::TestBorderBrushSetEdgeOffsetL ),
        ENTRY( "TestBorderBrushSetEdgeOffsetMetricL", CTestPlatAlfVisual::TestBorderBrushSetEdgeOffsetMetricL ),
        ENTRY( "TestBorderBrushImageOffsetL", CTestPlatAlfVisual::TestBorderBrushImageOffsetL ),
        ENTRY( "TestBorderBrushSetImageOffsetL", CTestPlatAlfVisual::TestBorderBrushSetImageOffsetL ),
        ENTRY( "TestBorderBrushSetImageOffsetMetricL", CTestPlatAlfVisual::TestBorderBrushSetImageOffsetMetricL ),
        
        //FOR TEST alfviewportlayout.h
        ENTRY( "TestVpLayoutAddNewL", CTestPlatAlfVisual::TestVpLayoutAddNewL ),
        ENTRY( "TestVpLayoutConstructorL", CTestPlatAlfVisual::TestVpLayoutConstructorL ),
        ENTRY( "TestVpLayoutConstructL", CTestPlatAlfVisual::TestVpLayoutConstructL ),
        ENTRY( "TestVpLayoutSetVirtualSizeL", CTestPlatAlfVisual::TestVpLayoutSetVirtualSizeL ),
        ENTRY( "TestVpLayoutSetViewportSizeL", CTestPlatAlfVisual::TestVpLayoutSetViewportSizeL ),
        ENTRY( "TestVpLayoutSetViewportPosL", CTestPlatAlfVisual::TestVpLayoutSetViewportPosL ),
        ENTRY( "TestVpLayoutRemoveAndDestroyAllDL", CTestPlatAlfVisual::TestVpLayoutRemoveAndDestroyAllDL ),
        ENTRY( "TestVpLayoutUpdateChildrenLayoutL", CTestPlatAlfVisual::TestVpLayoutUpdateChildrenLayoutL ),
        ENTRY( "TestVpLayoutFindTagL", CTestPlatAlfVisual::TestVpLayoutFindTagL ),
        ENTRY( "TestVpLayoutBaseUnitL", CTestPlatAlfVisual::TestVpLayoutBaseUnitL ),
        ENTRY( "TestVpLayoutDoRemoveAndDestroyAllDL", CTestPlatAlfVisual::TestVpLayoutDoRemoveAndDestroyAllDL ),
        ENTRY( "TestVpLayoutPropertyOwnerExtensionL", CTestPlatAlfVisual::TestVpLayoutPropertyOwnerExtensionL ),
        
        //FOR TEST alfutil.h
        ENTRY( "TestUtilPower2", CTestPlatAlfVisual::TestUtilPower2 ),
        ENTRY( "TestUtilPower2RoundDown", CTestPlatAlfVisual::TestUtilPower2RoundDown ),
        ENTRY( "TestUtilInterpolate", CTestPlatAlfVisual::TestUtilInterpolate ),
        ENTRY( "TestUtilWrapValue", CTestPlatAlfVisual::TestUtilWrapValue ),
        ENTRY( "TestUtilRandomInt", CTestPlatAlfVisual::TestUtilRandomInt ),
        ENTRY( "TestUtilRandomReal", CTestPlatAlfVisual::TestUtilRandomReal ),
        ENTRY( "TestUtilQuickLengthWithPoint", CTestPlatAlfVisual::TestUtilQuickLengthWithPoint ),
        ENTRY( "TestUtilQuickLength", CTestPlatAlfVisual::TestUtilQuickLength ),
        ENTRY( "TestUtilQuickNormalize", CTestPlatAlfVisual::TestUtilQuickNormalize ),
        ENTRY( "TestUtilQuickNormalizeWithVector", CTestPlatAlfVisual::TestUtilQuickNormalizeWithVector ),
        ENTRY( "TestUtilCrossProduct", CTestPlatAlfVisual::TestUtilCrossProduct ),
        ENTRY( "TestUtilNormalFromPoints", CTestPlatAlfVisual::TestUtilNormalFromPoints ),
        ENTRY( "TestUtilShadowMatrix", CTestPlatAlfVisual::TestUtilShadowMatrix ),
        ENTRY( "TestUtilFreeMemory", CTestPlatAlfVisual::TestUtilFreeMemory ),
        ENTRY( "TestUtilScreenSize", CTestPlatAlfVisual::TestUtilScreenSize ),
        ENTRY( "TestUtilNearestFontInTwipsL", CTestPlatAlfVisual::TestUtilNearestFontInTwipsL ),
        ENTRY( "TestUtilReleaseFont", CTestPlatAlfVisual::TestUtilReleaseFont ),
        ENTRY( "TestUtilLengthUnit", CTestPlatAlfVisual::TestUtilLengthUnit ),
        ENTRY( "TestUtilColorLightness", CTestPlatAlfVisual::TestUtilColorLightness ),
        ENTRY( "TestUtilScaleFbsBitmapL", CTestPlatAlfVisual::TestUtilScaleFbsBitmapL ),
        ENTRY( "TestUtilCombineMaskFbsBitmapL", CTestPlatAlfVisual::TestUtilCombineMaskFbsBitmapL ),
        ENTRY( "TestUtilCropFbsBitmapL", CTestPlatAlfVisual::TestUtilCropFbsBitmapL ),
        ENTRY( "TestUtilScaleImageL", CTestPlatAlfVisual::TestUtilScaleImageL ),
        ENTRY( "TestUtilCropImageL", CTestPlatAlfVisual::TestUtilCropImageL ),
        ENTRY( "TestUtilTagMatches", CTestPlatAlfVisual::TestUtilTagMatches ),
        ENTRY( "TestUtilConvertBitmapToDisplayModeLCL", CTestPlatAlfVisual::TestUtilConvertBitmapToDisplayModeLCL ),
        ENTRY( "TestUtilThemeItemIdL", CTestPlatAlfVisual::TestUtilThemeItemIdL ),
        
        //FOR TEST alftransformation.h
        ENTRY( "TestTransNewL", CTestPlatAlfVisual::TestTransNewL ),
        ENTRY( "TestTransNewLCL", CTestPlatAlfVisual::TestTransNewLCL ),
        ENTRY( "TestTransCountL", CTestPlatAlfVisual::TestTransCountL ),
        ENTRY( "TestTransNonIdentityCountL", CTestPlatAlfVisual::TestTransNonIdentityCountL ),
        ENTRY( "TestTransStepL", CTestPlatAlfVisual::TestTransStepL ),
        ENTRY( "TestTransReplaceStepL", CTestPlatAlfVisual::TestTransReplaceStepL ),
        ENTRY( "TestTransLoadIdentityL", CTestPlatAlfVisual::TestTransLoadIdentityL ),
        ENTRY( "TestTransTranslateL", CTestPlatAlfVisual::TestTransTranslateL ),
        ENTRY( "TestTransScaleL", CTestPlatAlfVisual::TestTransScaleL ),
        ENTRY( "TestTransRotateL", CTestPlatAlfVisual::TestTransRotateL ),
        
        //FOR TEST alftimedvalue.h
        ENTRY( "TestTimedValueConstructor", CTestPlatAlfVisual::TestTimedValueConstructor ),
        ENTRY( "TestTimedValueValueNow", CTestPlatAlfVisual::TestTimedValueValueNow ),
        ENTRY( "TestTimedValueTargetAndSpeed", CTestPlatAlfVisual::TestTimedValueTargetAndSpeed ),
        ENTRY( "TestTimedValueStyle", CTestPlatAlfVisual::TestTimedValueStyle ),
        ENTRY( "TestTimedValueMappingFunctionIdentifier", CTestPlatAlfVisual::TestTimedValueMappingFunctionIdentifier ),
        ENTRY( "TestTimedValueFlags", CTestPlatAlfVisual::TestTimedValueFlags ),
        ENTRY( "TestRealPointMethods", CTestPlatAlfVisual::TestRealPointMethods ),
        ENTRY( "TestRealSizeMethods", CTestPlatAlfVisual::TestRealSizeMethods ),
        ENTRY( "TestRealRectMethods", CTestPlatAlfVisual::TestRealRectMethods ),
        ENTRY( "TestTimedPointConstructor", CTestPlatAlfVisual::TestTimedPointConstructor ),
        ENTRY( "TestTimedPointSetStyle", CTestPlatAlfVisual::TestTimedPointSetStyle ),
        ENTRY( "TestTimedPointSetMapFuncIdentifier", CTestPlatAlfVisual::TestTimedPointSetMapFuncIdentifier ),
        ENTRY( "TestTimedPointTargetMethods", CTestPlatAlfVisual::TestTimedPointTargetMethods ),
        ENTRY( "TestTimedPointValueNowMethods", CTestPlatAlfVisual::TestTimedPointValueNowMethods ),
        
        //FOR TEST alftextvisual.h
        ENTRY( "TestTxtVisualAddNewL", CTestPlatAlfVisual::TestTxtVisualAddNewL ),
        ENTRY( "TestTxtVisualConstructorL", CTestPlatAlfVisual::TestTxtVisualConstructorL ),
        ENTRY( "TestTxtVisualConstructL", CTestPlatAlfVisual::TestTxtVisualConstructL ),
        ENTRY( "TestTxtVisualTxtMethodsL", CTestPlatAlfVisual::TestTxtVisualTxtMethodsL ),
        ENTRY( "TestTxtVisualStyleMethodsL", CTestPlatAlfVisual::TestTxtVisualStyleMethodsL ),
        ENTRY( "TestTxtVisualSetAlignL", CTestPlatAlfVisual::TestTxtVisualSetAlignL ),
        ENTRY( "TestTxtVisualSetLineSpacingL", CTestPlatAlfVisual::TestTxtVisualSetLineSpacingL ),
        ENTRY( "TestTxtVisualExtentsMethodsL", CTestPlatAlfVisual::TestTxtVisualExtentsMethodsL ),
        ENTRY( "TestTxtVisualMaxLineCountMethodsL", CTestPlatAlfVisual::TestTxtVisualMaxLineCountMethodsL ),
        ENTRY( "TestTxtVisualWrappingMethodsL", CTestPlatAlfVisual::TestTxtVisualWrappingMethodsL ),
        ENTRY( "TestTxtVisualBackgroundTypeL", CTestPlatAlfVisual::TestTxtVisualBackgroundTypeL ),
        ENTRY( "TestTxtVisualShadowOpacityMethodsL", CTestPlatAlfVisual::TestTxtVisualShadowOpacityMethodsL ),
        ENTRY( "TestTxtVisualColorMethodsL", CTestPlatAlfVisual::TestTxtVisualColorMethodsL ),
        ENTRY( "TestTxtVisualEnableShadowL", CTestPlatAlfVisual::TestTxtVisualEnableShadowL ),
        ENTRY( "TestTxtVisualOffsetMethodsL", CTestPlatAlfVisual::TestTxtVisualOffsetMethodsL ),
        ENTRY( "TestTxtVisualRemoveAndDestroyAllDL", CTestPlatAlfVisual::TestTxtVisualRemoveAndDestroyAllDL ),
        ENTRY( "TestTxtVisualUpdateChildrenLayoutL", CTestPlatAlfVisual::TestTxtVisualUpdateChildrenLayoutL ),
        ENTRY( "TestTxtVisualFindTagL", CTestPlatAlfVisual::TestTxtVisualFindTagL ),
        ENTRY( "TestTxtVisualSetHighlightRangeL", CTestPlatAlfVisual::TestTxtVisualSetHighlightRangeL ),
        ENTRY( "TestTxtVisualProtectedMethodsL", CTestPlatAlfVisual::TestTxtVisualProtectedMethodsL ),
        
        //FOR TEST alftextureprocessor.h
        ENTRY( "TestTxtProcessorBlurL", CTestPlatAlfVisual::TestTxtProcessorBlurL ),
        
        //FOR TEST alftexturegroup.h
        ENTRY( "TestTxtGroupNewL", CTestPlatAlfVisual::TestTxtGroupNewL ),
        ENTRY( "TestTxtGroupNewLCL", CTestPlatAlfVisual::TestTxtGroupNewLCL ),
        ENTRY( "TestTxtGroupTextureMethodsL", CTestPlatAlfVisual::TestTxtGroupTextureMethodsL ),
        ENTRY( "TestTxtGroupIsLoadingCompletedL", CTestPlatAlfVisual::TestTxtGroupIsLoadingCompletedL ),
        ENTRY( "TestTxtGroupObserverMethodsL", CTestPlatAlfVisual::TestTxtGroupObserverMethodsL ),
        
        //FOR TEST alftexturemanager.h
        ENTRY( "TestTxtMgrEnv", CTestPlatAlfVisual::TestTxtMgrEnv ),
        ENTRY( "TestTxtMgrTextureL", CTestPlatAlfVisual::TestTxtMgrTextureL ),
        ENTRY( "TestTxtMgrImagePathL", CTestPlatAlfVisual::TestTxtMgrImagePathL ),
        ENTRY( "TestTxtMgrBlankTexture", CTestPlatAlfVisual::TestTxtMgrBlankTexture ),
        ENTRY( "TestTxtMgrLoadTextureL", CTestPlatAlfVisual::TestTxtMgrLoadTextureL ),
        ENTRY( "TestTxtMgrCreateTextureL", CTestPlatAlfVisual::TestTxtMgrCreateTextureL ),
        ENTRY( "TestTxtMgrUnloadTextureL", CTestPlatAlfVisual::TestTxtMgrUnloadTextureL ),
        ENTRY( "TestTxtMgrUpdateTextureL", CTestPlatAlfVisual::TestTxtMgrUpdateTextureL ),
        ENTRY( "TestTxtMgrDefineFileNameL", CTestPlatAlfVisual::TestTxtMgrDefineFileNameL ),
        ENTRY( "TestTxtMgrPrependImagePath", CTestPlatAlfVisual::TestTxtMgrPrependImagePath ),
        ENTRY( "TestTxtMgrObserverMethodsL", CTestPlatAlfVisual::TestTxtMgrObserverMethodsL ),
        ENTRY( "TestTxtMgrProcessor", CTestPlatAlfVisual::TestTxtMgrProcessor ),
        ENTRY( "TestTxtMgrSetAutomaticTextureIdRange", CTestPlatAlfVisual::TestTxtMgrSetAutomaticTextureIdRange ),
        ENTRY( "TestTxtMgrIsLoadedL", CTestPlatAlfVisual::TestTxtMgrIsLoadedL ),
        ENTRY( "TestTxtMgrTextureIdL", CTestPlatAlfVisual::TestTxtMgrTextureIdL ),
        
        //FOR TEST alfstatic.h
        ENTRY( "TestStaticEnv", CTestPlatAlfVisual::TestStaticEnv ),
        
        //FOR TEST alfsymbiansbdrawer.h
        ENTRY( "TestBufDrawerDrawBufferL", CTestPlatAlfVisual::TestBufDrawerDrawBufferL ),
        ENTRY( "TestBufDrawerGetBufferBitmapsL", CTestPlatAlfVisual::TestBufDrawerGetBufferBitmapsL ),
        ENTRY( "TestBufDrawerReleaseDrawerL", CTestPlatAlfVisual::TestBufDrawerReleaseDrawerL ),
        
        //FOR TEST alftextstylemanager.h
        ENTRY( "TestStyleMgrTextStyleMethodsL", CTestPlatAlfVisual::TestStyleMgrTextStyleMethodsL ),
        
        //FOR TEST alftextstyle.h
        ENTRY( "TestStyleParentIdL", CTestPlatAlfVisual::TestStyleParentIdL ),
        ENTRY( "TestStyleIdL", CTestPlatAlfVisual::TestStyleIdL ),
        ENTRY( "TestStyleTextColorL", CTestPlatAlfVisual::TestStyleTextColorL ),
        ENTRY( "TestStyleTextSizeL", CTestPlatAlfVisual::TestStyleTextSizeL ),
        ENTRY( "TestStyleBoldL", CTestPlatAlfVisual::TestStyleBoldL ),
        ENTRY( "TestStyleItalicL", CTestPlatAlfVisual::TestStyleItalicL ),
        ENTRY( "TestStyleUnderlineL", CTestPlatAlfVisual::TestStyleUnderlineL ),
        ENTRY( "TestStyleStrikeThroughL", CTestPlatAlfVisual::TestStyleStrikeThroughL ),
        ENTRY( "TestStyleTypefaceNameL", CTestPlatAlfVisual::TestStyleTypefaceNameL ),
        ENTRY( "TestStyleSetTextPaneHeightInPixelsL", CTestPlatAlfVisual::TestStyleSetTextPaneHeightInPixelsL ),
        
        //FOR TEST alftexture.h
        ENTRY( "TestTextureGetMethods", CTestPlatAlfVisual::TestTextureGetMethods ),
        ENTRY( "TestTextureAnimation", CTestPlatAlfVisual::TestTextureAnimation ),
        ENTRY( "TestTextureSetMethodsL", CTestPlatAlfVisual::TestTextureSetMethodsL ),
        
        //FOR TEST alftextureautosizeparams.h
        ENTRY( "TestTxtAutoSizeParamsConstructor", CTestPlatAlfVisual::TestTxtAutoSizeParamsConstructor ),
        ENTRY( "TestTxtAutoSizeParamsGetMethods", CTestPlatAlfVisual::TestTxtAutoSizeParamsGetMethods ),
        ENTRY( "TestTxtAutoSizeParamsSetMethods", CTestPlatAlfVisual::TestTxtAutoSizeParamsSetMethods ),
        
        //FOR TEST alfshadowborderbrush.h
        ENTRY( "TestShadowBBNewL", CTestPlatAlfVisual::TestShadowBBNewL ),
        ENTRY( "TestShadowBBNewLCL", CTestPlatAlfVisual::TestShadowBBNewLCL ),
        ENTRY( "TestShadowBBWidthMethodsL", CTestPlatAlfVisual::TestShadowBBWidthMethodsL ),
        
        //FOR TEST alfpropertyowner.h
        ENTRY( "TestPropOwnerConstructorL", CTestPlatAlfVisual::TestPropOwnerConstructorL ),
        ENTRY( "TestPropOwnerPropertyRemoveL", CTestPlatAlfVisual::TestPropOwnerPropertyRemoveL ),
        ENTRY( "TestPropOwnerPropertyClearL", CTestPlatAlfVisual::TestPropOwnerPropertyClearL ),
        ENTRY( "TestPropOwnerPropertyStringL", CTestPlatAlfVisual::TestPropOwnerPropertyStringL ),
        ENTRY( "TestPropOwnerCopyPropertyL", CTestPlatAlfVisual::TestPropOwnerCopyPropertyL ),
        ENTRY( "TestPropOwnerPropertyOwnerExtensionL", CTestPlatAlfVisual::TestPropOwnerPropertyOwnerExtensionL ),
        
        //FOR TEST alfroster.h
        ENTRY( "TestRosterCtrlGroupMethodsL", CTestPlatAlfVisual::TestRosterCtrlGroupMethodsL ),
        ENTRY( "TestRosterCtrlMethodsL", CTestPlatAlfVisual::TestRosterCtrlMethodsL ),
        ENTRY( "TestRosterVisualMethodsL", CTestPlatAlfVisual::TestRosterVisualMethodsL ),
        ENTRY( "TestRosterPointerEventObserverMethodsL", CTestPlatAlfVisual::TestRosterPointerEventObserverMethodsL ),
        ENTRY( "TestRosterSetPointerDragThresholdL", CTestPlatAlfVisual::TestRosterSetPointerDragThresholdL ),
        ENTRY( "TestRosterDisableLongTapEventsWhenDraggingL", CTestPlatAlfVisual::TestRosterDisableLongTapEventsWhenDraggingL ),
        ENTRY( "TestRosterCtrlGroupOrderChangedObserverL", CTestPlatAlfVisual::TestRosterCtrlGroupOrderChangedObserverL ),
        
        //FOR TEST alfscreenbuffer.h
        ENTRY( "TestScrBufNewLMethodsL", CTestPlatAlfVisual::TestScrBufNewLMethodsL ),
        ENTRY( "TestScrBufGetMethodsL", CTestPlatAlfVisual::TestScrBufGetMethodsL ),
        ENTRY( "TestScrBufRequestMethodsL", CTestPlatAlfVisual::TestScrBufRequestMethodsL ),
        ENTRY( "TestScrBufObserverMethodsL", CTestPlatAlfVisual::TestScrBufObserverMethodsL ),
        
        //FOR TEST alfscrollerevent.h
        ENTRY( "TestScrollEventMethods", CTestPlatAlfVisual::TestScrollEventMethods ),
        
        //FOR TEST alfmetric.h
        ENTRY( "TestMetricMethods", CTestPlatAlfVisual::TestMetricMethods ),
        ENTRY( "TestXYMetricMethods", CTestPlatAlfVisual::TestXYMetricMethods ),
        ENTRY( "TestBoxMetricMethods", CTestPlatAlfVisual::TestBoxMetricMethods ),
        
        //FOR TEST alfproceduralmesh.h
        ENTRY( "TestProMeshMakeMethodsL", CTestPlatAlfVisual::TestProMeshMakeMethodsL ),
        ENTRY( "TestProMeshMaterialMethodsL", CTestPlatAlfVisual::TestProMeshMaterialMethodsL ),
        
        //FOR TEST alfcommand.h
        ENTRY( "TestCmdMethods", CTestPlatAlfVisual::TestCmdMethods ),
        ENTRY( "TestActCmdMethodsL", CTestPlatAlfVisual::TestActCmdMethodsL ),
        ENTRY( "TestObjCmdMethodsL", CTestPlatAlfVisual::TestObjCmdMethodsL ),
        ENTRY( "TestGroupCmdMethodsL", CTestPlatAlfVisual::TestGroupCmdMethodsL ),
        ENTRY( "TestCtrlCmdMethodsL", CTestPlatAlfVisual::TestCtrlCmdMethodsL ),
        ENTRY( "TestVisualCmdMethodsL", CTestPlatAlfVisual::TestVisualCmdMethodsL ),
        ENTRY( "TestImageCmdMethodsL", CTestPlatAlfVisual::TestImageCmdMethodsL ),
        ENTRY( "TestTextCmdMethodsL", CTestPlatAlfVisual::TestTextCmdMethodsL ),
        ENTRY( "TestValueCmdMethodsL", CTestPlatAlfVisual::TestValueCmdMethodsL ),
        ENTRY( "TestPointCmdMethodsL", CTestPlatAlfVisual::TestPointCmdMethodsL ),
        ENTRY( "TestTransCmdMethodsL", CTestPlatAlfVisual::TestTransCmdMethodsL ),
        ENTRY( "TestMarkerCmdMethodsL", CTestPlatAlfVisual::TestMarkerCmdMethodsL ),
        ENTRY( "TestCustomEventCmdMethodsL", CTestPlatAlfVisual::TestCustomEventCmdMethodsL ),
        
        //FOR TEST alfbrush.h
        ENTRY( "TestAlfBrushOpacityL", CTestPlatAlfVisual::TestAlfBrushOpacityL ),
        ENTRY( "TestAlfBrushSetOpacityL", CTestPlatAlfVisual::TestAlfBrushSetOpacityL ),
        ENTRY( "TestAlfBrushSetLayerL", CTestPlatAlfVisual::TestAlfBrushSetLayerL ),
        ENTRY( "TestAlfBrushSetClipToVisualL", CTestPlatAlfVisual::TestAlfBrushSetClipToVisualL ),
        ENTRY( "TestAlfBrushClipToVisualL", CTestPlatAlfVisual::TestAlfBrushClipToVisualL ),
        ENTRY( "TestAlfBrushIdentifierL", CTestPlatAlfVisual::TestAlfBrushIdentifierL ),
        ENTRY( "TestAlfBrushCommsL", CTestPlatAlfVisual::TestAlfBrushCommsL ),
        ENTRY( "TestAlfBrushConstructLWithParamL", CTestPlatAlfVisual::TestAlfBrushConstructLWithParamL ),
        ENTRY( "TestAlfBrushConstructL", CTestPlatAlfVisual::TestAlfBrushConstructL ),
        ENTRY( "TestAlfBrushPropertyOwnerExtL", CTestPlatAlfVisual::TestAlfBrushPropertyOwnerExtL ),
        
        //FOR TEST alfbrusharray.h
        ENTRY( "TestBrushArrayResetL", CTestPlatAlfVisual::TestBrushArrayResetL ),
        ENTRY( "TestBrushArrayAppendL", CTestPlatAlfVisual::TestBrushArrayAppendL ),
        ENTRY( "TestBrushArrayInsertL", CTestPlatAlfVisual::TestBrushArrayInsertL ),
        ENTRY( "TestBrushArrayRemoveL", CTestPlatAlfVisual::TestBrushArrayRemoveL ),
        ENTRY( "TestBrushArrayCountL", CTestPlatAlfVisual::TestBrushArrayCountL ),
        ENTRY( "TestBrushArrayOperatorL", CTestPlatAlfVisual::TestBrushArrayOperatorL ),
        ENTRY( "TestBrushArrayAtL", CTestPlatAlfVisual::TestBrushArrayAtL ),

        // [test cases entries]

        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


//  [End of File]
