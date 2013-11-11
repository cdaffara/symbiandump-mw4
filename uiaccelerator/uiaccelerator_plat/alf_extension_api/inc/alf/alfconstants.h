/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Constant definitions
*
*/



#ifndef ALFCONSTANTS_H
#define ALFCONSTANTS_H

#include <alf/alfanchorlayout.h> 
#include <alf/alfgridlayout.h> 
#include <alf/alftimedvalue.h>
#include <gdi.h>
#include <w32std.h>
#include <AknsItemID.h>
#include <AknFontSpecification.h> 
#include <alf/alfmaterial.h>
#include <alf/alfclientwindow.h>

/**
 * Service UID which the Alfred Server implements.
 */
const TUid KAlfServiceUid = {0x10282847};

/**
 * HuiEnv action command UID to notify that a new TVOut display has been created.
 */
const TUid KHuiActionNewTVOutDisplayUid = {0x2000B5E6};

/**
 * IPC enums
 *
 * When adding new values, please obey the blocks and add new oppcodes at the end of suitable block 
 * So that existing enumerations do not change.
 *
 * When implementing app specifc extension, define your opcodes so that they wont clash with system
 * enumerations i.e. chooose value above EAlfLastReservedOpCmd
 */
enum TAlfredServerIPC
    {
    // Env & basic session managenemt 
    EAlfEnvSetRefreshMode,
    EAlfEnvSetMaxFrameRate,
    EAlfEnvContinueRefresh,
    EAlfEnvPauseRefresh,
    EAlfEnvRefreshCallBack,
    EAlfEnvSetIdleThreshold,
    EAlfEnvRenderer,
    EAlfGetPointerEvent,
    EAlfCancelPtrEvents,
    ESetFullScreenDrawing,
    EAlfGetSystemEvent,
    EAlfCancelSystemEvents,
    EAlfNotifyAppVisibility,
    EAlfDisplaySubSessionOpen,
    EAlfControlGroupSubSessionOpen,
    EAlfTransformationSubSessionOpen,
    EAlfCreateSubSession,
    EAlfCloseSubSession,
    EAlfDoSubSessionCmd,
    EAlfDoAsyncSubSessionCmd,
    EAlfSubSCancelAsynchRequest,
    EAlfSetWgParent,
    EAlfSBufAddObserver, 
    EAlfSBufRemoveObserver, 
    EAlfSBufRequestEvent, 
    EAlfSBufRequestNextBuffer, 
    EAlfDisplaySubSessionOpen2, 
    EAlfDoSubSessionBatchCmd,
    EAlfConfigureBatchCmd,

    EAlfNotifyTextureInfo,
    EAlfCancelNotifyTextureInfo,
    EAlfSBufRequestBufferDraw, 

    // Control
    EAlfCntrlCreate = 150,
    
    // Visual
    EAlfVisualCreate = 200,
    EAlfTextVisualCreate,
    EAlfVisualRemoveAndDestroyAll,
    EAlfVisualUpdateChildrenLayout,
    EAlfTextVisualSetText,
    EAlfTextVisualSetStyle,
    EAlfTextVisualSetTextStyle,
    EAlfTextVisualSetEffects,
    EAlfTextVisualSetAlign,
    EAlfTextVisualSetLineSpacing,
    EAlfTextVisualTextExtents,
    EAlfTextVisualSubstringExtents,
    EAlfTextVisualStyle,
    EAlfTextVisualSetMaxLineCount,
    EAlfTextVisualMaxLineCount,
    EAlfTextVisualWrapping,
    EAlfTextVisualSetWrapping,
    EAlfTextVisualShadowOpacity,  // deprecated
    EAlfTextVisualSetShadowOpacity, // deprecated    
    EAlfTextVisualSetColor,
    EAlfTextVisualEnableShadow,  // deprecated       
    EAlfVisualSetOpacity,
    EAlfVisualGetOpacity,
    EAlfVisualLocalToDisplay,
    EAlfVisualDisplayToLocal,
    EAlfVisualDisplayRectTarget,
    EAlfVisualPos,
    EAlfVisualSetPos,
    EAlfVisualMoveToFront,
    EAlfVisualSize,
    EAlfVisualSetSize,
    EAlfVisualBrushArrayReset,
    EAlfVisualBrushArrayAppend,
    EAlfVisualBrushArrayInsert,
    EAlfVisualBrushArrayRemove,
    EAlfVisualSetFlag,
    EAlfVisualSetFlags,
    EAlfVisualClearFlag,
    EAlfVisualClearFlags,
    EAlfVisualFlags,
    EAlfVisualSetPaddingInt,
    EAlfVisualSetPaddingPoint,
    EAlfVisualSetPaddingBox,
    EAlfVisualPaddingInPixels,
    EAlfVisualEnableTransformation,
    EAlfVisualDepthOffset,
    EAlfVisualSetDepthOffset,
    EAlfVisualSetMinSize,
    EAlfVisualSetMaxSize,
    EAlfVisualDisplayRect,
    EAlfVisualEffectiveOpacity,
    EAlfVisualSetPosTimed,
    EAlfVisualSetSizeTimed,
    EAlfVisualLayoutUpdatedNotification,
    EAlfVisualLayoutUpdatedNotificationCancel,
    EAlfVisualCopyValuesFromOtherVisual,
    EAlfVisualSetMinSizeInBaseUnits,
    EAlfVisualGetMinSize,
    EAlfVisualGetMinSizeInBaseUnits,
    EAlfVisualSetMaxSizeInBaseUnits,
    EAlfVisualGetMaxSize,
    EAlfVisualGetMaxSizeInBaseUnits,
    EAlfVisualMove,
    EAlfVisualSetCenteredPosAndSize,
    EAlfVisualSetTactileFeedback,
    EAlfVisualHasTactileFeedback,
    EAlfVisualRemoveTactileFeedback,
    EAlfVisualPauseProperties,
    EAlfVisualResumeProperties,
    EAlfVisualEnableDropShadow,
    
    EAlfTextVisualSetOffset,         
    EAlfTextVisualOffset,         
    
	EAlfTextVisualSetHighlightRange,             
	
    // hip hop, slightly wrong context for these two but moving would break bc
    EAlfVisualSetEffect,
    EAlfVisualSetGroupEffect,
    EAlfVisualRemoveEffect,
    
    EAlfTextVisualSetRasterizedMesh,
    
    EAlfQtCommandBuffer = 450,
    EAlfDirectClientFPSCounterOn,
    EAlfDirectClientFPSCounterOff,
    
    // LCT Text Visual
    EAlfLCTTextVisualCreate = 500,
    EAlfLCTTextVisualSetTextPane,

    // Line visual
    EAlfLineVisualCreate = 600,
    EAlfLineVisualSetPath,
    EAlfLineVisualSetImage,
    EAlfLineVisualSetAlphaFunction,
    EAlfLineVisualSetWidthFunction,
    EAlfLineVisualGetThickness,
    EAlfLineVisualSetThickness,
    EAlfLineVisualGetShadowThickness,
    EAlfLineVisualSetShadowThickness,
    EAlfLineVisualGetStartPos,
    EAlfLineVisualSetStartPos,
    EAlfLineVisualGetEndPos,
    EAlfLineVisualSetEndPos,
    EAlfLineVisualSetColor,

    // Image visual
    EAlfImageVisualCreate = 700,
    EAlfImageVisualSetImage,
    EAlfImageVisualSetScaleMode,
    EAlfImageVisualSetSecondaryImage,
    EAlfImageVisualSetSecondaryAlpha,
    EAlfImageVisualSecondaryAlpha,
    EAlfImageVisualSetColor,
    EAlfImageVisualSetColorMode,
    EAlfImageVisualSetStretch,
    EAlfImageVisualSetStretchMode,
    EAlfImageVisualSetDropShadow, // deprecated
    EAlfImageVisualSetTurnAngle,
    EAlfImageVisualTurnAngle,
    EAlfImageVisualSetScale,
    EAlfImageVisualScale,
    EAlfImageVisualSetOffset,
    EAlfImageVisualOffset,
    EAlfImageVisualSetDropShadowMetric, // deprecated
    
    // Mesh visual
    EAlfMeshVisualCreate = 800,
    EAlfMeshVisualCreateMesh,
    EAlfMeshVisualSetImage,
    EAlfMeshVisualSetSecondaryImage,
    EAlfMeshVisualSetSpecularImage,
    EAlfMeshVisualSecondaryAlpha,
    EAlfMeshVisualSetSecondaryAlpha,
    EAlfMeshVisualYawAngle,
    EAlfMeshVisualSetYawAngle,
    EAlfMeshVisualPitchAngle,
    EAlfMeshVisualSetPitchAngle,
    EAlfMeshVisualScale,
    EAlfMeshVisualSetScale,
    EAlfMeshReset,
    EAlfMeshMakeCube,
    EAlfMeshMakeSphere,
    EAlfMeshMakeTorus,
    EAlfMeshStretchUniformly,
    EAlfMeshScaleUniformly,
    EAlfMeshAnimationControllerCount,
    EAlfMeshSetAnimationPosition,
    EAlfMeshStartAnimationController,
    EAlfMeshStopAnimationController,
    EAlfMeshMaterial,
    EAlfMeshSetMaterial,
    EAlfMeshLoadM3GScene,

    // canvas visual
    EAlfCanvasVisualCreate  = 900,
    EAlfCanvasVisualSendbuffer,

    // Layout
    EAlfLayoutCreate = 1000,
    EAlfLayoutAppendVisual,
    EAlfLayoutRemoveVisual, 
    EAlfLayoutVisualFromIndex,
    EAlfLayoutEnableScrolling,
    EAlfLayoutScrollOffset,
    EAlfLayoutSetScrollOffset,
    EAlfLayoutChildOrdinal,
    EAlfLayoutChildPos,
    EAlfLayoutChildSize,
    EAlfLayoutReorderVisual,
    EAlfLayoutSetTransitionTime,
    EAlfLayoutSetInnerPaddingPoint,
    EAlfLayoutSetInnerPaddingMetric,
    EAlfLayoutInnerPaddingInBaseUnits,
    EAlfLayoutVirtualSize,
    EAlfLayoutSetBaseUnit,
    EAlfLayoutSetBaseUnitXY,
    EAlfLayoutBaseUnit,
    EAlfLayoutSetLayoutModeFlags,
    EAlfLayoutLayoutModeFlags,
    EAlfLayoutClearLayoutModeFlags,
    
    // Anchor layout
    EAlfAnchorLayoutCreate = 1100,
    EAlfAnchorLayoutReset,
    EAlfAnchorLayoutSetAnchor,
    EAlfAnchorLayoutSetRelativeAnchorRect,
    EAlfAnchorLayoutRemoveAnchor,
    EAlfAnchorLayoutAttachEdge,
    EAlfAnchorLayoutAttachCorner,
    EAlfAnchorLayoutAttachBox,
    EAlfAnchorLayoutDetachAll,
    EAlfAnchorLayoutDetach,

    // LCT Anchor Layout
    EAlfLCTAnchorLayoutCreate = 1200,
    EAlfLCTAnchorLayoutSetAnchors,

    // Grid layout
    EAlfGridLayoutCreate = 1300,
    EAlfGridLayoutSetColumns,
    EAlfGridLayoutSetRows,
    EAlfGridLayoutSetColumnWeights,
    EAlfGridLayoutSetRowsWeights,
    EAlfGridLayoutSetExpanding,
    EAlfGridLayoutOrdinalToBlock,
    EAlfGridLayoutColumnCount,
    EAlfGridLayoutRowCount,
    EAlfGridLayoutFillWeights,
    EAlfGridLayoutAppendWeight,
    EAlfGridLayoutInsertWeight,
    EAlfGridLayoutReplaceWeight,
    EAlfGridLayoutRemoveWeight,
    EAlfGridLayoutWeight,
    EAlfGridLayoutCount,
    EAlfGridLayoutSetLayoutModeFlags,
    EAlfGridLayoutLayoutModeFlags,
    EAlfGridLayoutClearLayoutModeFlags,
    
    // LCT Grid Layout
    EAlfLCTGridLayoutCreate = 1400,
    EAlfLCTGridLayoutSetColsAndRows,
    
    // Deck layout
    EAlfDeckLayoutCreate = 1500,
    
    // Flow layout
    EAlfFlowLayoutCreate = 1600,
    EAlfFlowLayoutSetFlowDirection,
    EAlfFlowLayoutSetCentering,
    EAlfFlowLayoutSetMode,
    
    // Curve path layout
    EAlfCurvePathLayoutCreate = 1700,
    EAlfCurvePathLayoutMapValue,
    
    // Viewport layout
    EAlfViewportLayoutCreate = 1800,
    EAlfViewportLayoutSetVirtualSize,
    EAlfViewportLayoutSetViewportSize,
    EAlfViewportLayoutSetViewportPos,
    
    // Roster
    EAlfRosterShow = 2000,
    EAlfRosterHide,
    EAlfRosterShowVisual,
    EAlfRosterHideVisual,
    EAlfRosterMoveVisualToFront,
    EAlfRosterSetPointerEventFlags,
    EAlfRosterAddPointerEventObserver,
    EAlfRosterRemovePointerEventObserver,
	EAlfRosterSetPointerDragTreshold,
	EAlfRosterDisableLongTapEventsWhenDragging,
	
    // Texture 
    EAlfTextureCreateAnimated = 2100,
    EAlfTextureStopAnimation,
    EAlfTextureStartAnimation,
    EAlfTextureCreate,
    EAlfTextureLoad,
    EAlfTextureUnload,
    EAlfTextureBlur,
    EAlfTextureHasContent,
    EAlfTextureDelete,
    EAlfTextureRelease,
    EAlfTextureRestore,
    EAlfTextureNotifySkinChanged,
    EAlfTextureUpdateOwnerId,
    EAlfTextureSetAutoSizeParams,
        
    // Display subsession functions
    EAlfDisplaySetClearBackground =2200,
    EAlfDisplaySetVisibleArea,
    EAlfDisplayForceSetVisibleArea,
    EAlfDisplayVisibleArea,
    EAlfDisplaySubSessionClose,
    EAlfDisplaySetDirty,
    EAlfDisplaySetQuality,
    EAlfDisplayQuality,
    EAlfDisplaySetUseDepth,
    EAlfDisplaySetUsage,
    EAlfDisplaySetBackgroundItems,
    EAlfDisplayHandlePointerEvent,
    EAlfDisplaySuppressAutomaticFading,
    EAlfDisplaySetClientWindowForDrawing,
    
    // Control group subsession functions
    EAlfControlGroupAppend = 2300,
    EAlfControlGroupRemove,
    EAlfControlGroupEnableTransformation,
    EAlfControlGroupSubSessionClose,
    
    // Brush
    EAlfBrushSetOpacity = 2500,
    EAlfBrushGetOpacity,
    EAlfBrushSetLayer,
    EAlfBorderBrushCreate,
    EAlfBorderBrushSetImage,
    EAlfBorderBrushSetColor,
    EAlfBorderBrushSetThickness,
    EAlfBorderBrushGetThickness,
    EAlfBorderBrushSetEdgeOffset,
    EAlfBorderBrushGetEdgeOffset,
    EAlfBorderBrushSetImageOffset,
    EAlfBorderBrushGetImageOffset,
    EAlfBorderBrushCreateMetric,
    EAlfBorderBrushSetThicknessMetric,
    EAlfBorderBrushSetEdgeOffsetMetric,
    EAlfBorderBrushSetImageOffsetMetric,
    EAlfBrushSetClipToVisual,
    EAlfBrushClipToVisual,
    
    // image brush
    EAlfImageBrushCreate = 2600,
    EAlfImageBrushSetBorders,
    EAlfImageBrushSetImage,
    EAlfImageBrushSetScaleMode,
    EAlfImageBrushSetScale,
    EAlfImageBrushScale,
    EAlfImageBrushSetOffset,
    EAlfImageBrushOffset,    
    
    // shadow border brush
    EAlfShadowBorderBrushCreate = 2700,
    EAlfShadowBorderBrushGetWidth,
    EAlfShadowBorderBrushSetWidth,
    EAlfShadowBorderBrushCreateMetric,
    EAlfShadowBorderBrushGetWidthMetric,
    EAlfShadowBorderBrushSetWidthMetric,
    
    // drop shadow brush
    EAlfDropShadowBrushCreate = 2800,
    EAlfDropShadowBrushGetSoftness,
    EAlfDropShadowBrushSetSoftness,
    EAlfDropShadowBrushGetDepth,
    EAlfDropShadowBrushSetDepth,
    EAlfDropShadowBrushCreateMetric,
    EAlfDropShadowBrushSetDepthMetric,
    EAlfDropShadowBrushSetSoftnessMetric,
    EAlfDropShadowBrushSetColor,

    
    // gradient brush
    EAlfGradientBrushCreate = 2900,
    EAlfGradientBrushSetDirection,
    EAlfGradientBrushSetColor,
    EAlfGradientBrushSetImage,
    EAlfGradientBrushColor,
    EAlfGradientBrushOpacity,
    EAlfGradientBrushAppendColor,
    
    // Frame brush
    EAlfFrameBrushCreate = 3000,
    EAlfFrameBrushSetFrameRects,
    EAlfFrameBrushCreateMetric,
    EAlfFrameBrushSetEdgeOffsetMetric,
    EAlfFrameBrushCreateImage,
    EAlfFrameBrushSetImage,
    EAlfFrameBrushSetFramePartImage,
    
    // Transformation subsession functions
    EAlfTransformationLoadIdentity = 3300,
    EAlfTransformationTranslate,
    EAlfTransformationRotate,
    EAlfTransformationSubSessionClose,
    EAlfTransformationCount,
    EAlfTransformationNonIdentityCount,
    EAlfTransformationStep,
    EAlfTransformationReplaceStep,
    EAlfTransformationRotate3D,
    EAlfTransformationTranslate2D,
    EAlfTransformationScale,
       
    // mapping functions
    EAlfMappingFunctionAverageCreate = 3500,
    EAlfMappingFunctionMapValue,
    EAlfAverageMappingFunctionSetFunction1,
    EAlfAverageMappingFunctionSetFunction2,
    EAlfAverageMappingFunctionSetFunctions,
    EAlfAverageMappingFunctionSetWeight,
    EAlfMappingFunctionConstantCreate,
    EAlfConstantMappingFunctionSetValue,
    EAlfMappingFunctionLinearCreate,
    EAlfLinearMappingFunctionSetFactor,
    EAlfLinearMappingFunctionSetOffset,
    EAlfMappingFunctionSineCreate,
    EAlfSineMappingFunctionSetFactor,
    EAlfSineMappingFunctionSetOffset,
    EAlfMappingFunctionCosineCreate,
    EAlfCosineMappingFunctionSetFactor,
    EAlfCosineMappingFunctionSetOffset,
    EAlfMappingFunctionTableCreate, 
    EAlfMappingFunctionTableSetValues, 
    
    // curve path
    EAlfCurvePathCreate = 3700,
    EAlfCurvePathReset,
    EAlfCurvePathEnableLoop,
    EAlfCurvePathLoop,
    EAlfCurvePathSetOrigin,
    EAlfCurvePathAppendLine,
    EAlfCurvePathAppendArc,
    EAlfCurvePathLength,
    EAlfCurvePathEvaluate,
    EAlfCurvePathOffset,
    EAlfCurvePathSetOffset,
    EAlfCurvePathMapValue,
    EAlfCurvePathAppendLine2,
    EAlfCurvePathAppendArc2,

    // text styles
    EAlfPlatformTextStyleCreate = 3900,
    EAlfPreconfiguredTextStyleCreate,
    EAlfPlatformTextStyleCopy,
    EAlfTextStyleSetParent,
    EAlfTextStyleTextColor,
    EAlfTextStyleSizeInTwips,
    EAlfTextStyleSizeInPixels,
    EAlfTextStyleIsBold,
    EAlfTextStyleIsItalic,
    EAlfTextStyleIsUnderline,
    EAlfTextStyleIsStrikeThrough,
    EAlfTextStyleSetTextColor,
    EAlfTextStyleSetTextSizeInTwips,
    EAlfTextStyleSetTextSizeInPixels,
    EAlfTextStyleSetBold,
    EAlfTextStyleSetItalic,
    EAlfTextStyleSetUnderline,
    EAlfTextStyleSetStrikeThrough,
    EAlfTextStyleGetTypeface,
    EAlfTextStyleSetTextPaneHeightInPixels,
    EAlfTextStyleServerHandle,

    // Layout metrics
    EAlfLayoutMetricsTextStyleData = 4100,
    
    // Drop shadow handler
    EAlfDropShadowSetOffsetInPolar = 4200,
    EAlfDropShadowSetOffsetInXY,
    EAlfDropShadowSetColorRgb,
    EAlfDropShadowSetColorSkin,
    EAlfDropShadowSetOpacity,
    EAlfDropShadowSetBlurRadius,
    EAlfDropShadowSetScale,

    EAlfEnableLowMemoryState = 4300,
    EAlfForceSwRendering,
    EAlfBlankScreen,
    EAlfGetSizeAndRotation,
    EAlfReadPixels,
    
    EAlfLastReservedOpCmd = 10000
    };
    
struct TAlfLayoutSetAnchorParams
    {
    TAlfAnchor iAnchor;
    TInt iOrdinal;
    TAlfAnchorOrigin iHorizOrigin;
    TAlfAnchorOrigin iVertOrigin;
    TAlfAnchorMetric iHorizMetric;
    TAlfAnchorMetric iVertMetric;
    TAlfTimedPoint iOffset;
    };
    
struct TAlfLayoutSetRelativeAnchorRectParams
    {
    TInt iOrdinal;
    TAlfAnchorOrigin iTlHorizOrigin;
    TAlfAnchorOrigin iTlVertOrigin;
    TAlfRealPoint iTopLeftOffset;
    TAlfAnchorOrigin iBrHorizOrigin;
    TAlfAnchorOrigin iBrVertOrigin;
    TAlfRealPoint iBottomRightOffset;
    };

struct TAlfLayoutAttachEdgeParams
    {
    TInt iOrdinal;
    TAlfAnchorType iType;
    TAlfMetric iOffset;
    TAlfAnchorAttachmentOrigin iAttachmentOrigin;
    TInt iAttachmentOrdinal;
    };

struct TAlfLayoutAttachCornerParams
    {
    TInt iOrdinal;
    TAlfAnchorType iType;
    TAlfXYMetric iOffset;
    TAlfAnchorAttachmentOrigin iAttachmentOrigin;
    TInt iAttachmentOrdinal;
    };

struct TAlfLayoutAttachBoxParams
    {
    TInt iOrdinal;
    TAlfBoxMetric iOffset;
    TAlfAnchorAttachmentOrigin iAttachmentOrigin;
    TInt iAttachmentOrdinal;
    };

struct TAlfLCTAnchorSetAnchorsParams
    {
    TInt iOrdinal;
    TInt iApiId;
    TInt iComponentId;
    TInt iOptionIndex;
    TInt iDrawingOrderIndex;
    TInt iVarietyIndex;
    TInt iColumn;
    TInt iRow;
    TAlfTimedPoint iOffset;
    };

struct TAlfLCTGridSetColsAndRowsParams
    {
    TInt iOrdinal;
    TInt iApiId;
    TInt iComponentId;
    TInt iOptionIndex;
    TInt iDrawingOrderIndex;
    TInt iVarietyIndex;
    TInt iColumn;
    TInt iRow;
    TAlfTimedPoint iOffset;
    };

struct TAlfCreateBorderBrushParams
    {
    TInt iThicknessWidth; 
    TInt iThicknessHeight;
    TInt iEdgeOffsetX;
    TInt iEdgeOffsetY;
    };

struct TAlfCreateBorderBrushMetricParams
    {
    TAlfXYMetric iThickness; 
    TAlfXYMetric iEdgeOffset;
    };

struct TAlfFrameBrushParams
    {
    TAknsItemID iFrameIID;
    TRect iInnerRect;
    TRect iOuterRect;
    TInt iEdgeOffsetX;
    TInt iEdgeOffsetY;
    };

struct TAlfFrameBrushMetricParams
    {
    TAknsItemID iFrameIID;
    TRect iInnerRect;
    TRect iOuterRect;
    TAlfXYMetric iEdgeOffset;
    };

struct TAlfImageBrushParams
    {
    TInt iTextureHandle;
    TAlfTimedPoint iTl;
    TAlfTimedPoint iBr;
    TInt iLeftBorderWidth;
    TInt iRightBorderWidth;
    TInt iTopBorderWidth;
    TInt iBottomBorderWidth;         
    };
    
struct TAlfGradientBrushSetColorParams
    {
    TRgb iColor;
    TReal32 iOpacity;
    };
    
struct TAlfGradientBrushAppendColorParams
    {
    TReal32 iPosition;
    TRgb iColor;
    TReal32 iOpacity;
    };

struct TAlfCreateTextureAnimatedParams
    {
    TInt iId; 
    TInt iBitmapHandle;
    TInt iMaskBitmapHandle;
    TInt iFlags;
    TInt iManagerId;
    TFileName iFilename;
    };

struct TAlfCreateTextureParams
    {
    TInt iId; 
    TInt iBitmapHandle;
    TInt iMaskBitmapHandle;
    TInt iFlags;
    TInt iManagerId;
    };

struct TAlfLoadTextureParams
    {
    TInt iId; 
    TInt iBitmapHandle;
    TInt iMaskBitmapHandle;
    TInt iFlags;
    TInt iManagerId;
    };
    
struct TAlfBlurTextureParams
    {
    TInt iServerSideSrcHandle;
    TInt iServerSideDstHandle;
    TInt iManagerId;
    TSize iPreferredSize;     
    TInt iFilterSize;
    TInt iFlag;        
    };
    
struct TAlfImageParams
    {
    TAlfImageParams():iTextureHandle(0){}
    TAlfImageParams(const TAlfImage& aImage)
        : iTextureHandle( aImage.HasTexture() ? aImage.Texture().ServerHandle() : 0 ),
          iTl(aImage.TimedTopLeft()),
          iBr(aImage.TimedBottomRight()) {}
    TInt iTextureHandle;
    TAlfTimedPoint iTl;
    TAlfTimedPoint iBr;
    };
    
struct TAlfMeshLoadM3GSceneParams
    {
    TFileName iFilename;
    };    

struct TAlfMeshAnimationPositionParams
	{
	TInt iControllerId;
	TReal32 iTarget;
	TInt iTime;
	};
    
struct TAlfMaterialParams
    {
    TAlfMaterialPreset iPreset;
    TRgb iColor;
    TAlfImageParams iTextureImage;
    TAlfImageParams iSpecularImage;
    TAlfTimedValue iSpecular;
    };

struct TAlfPointerEventDragTreshold
	{
	TInt iControlHandle;
	TAlfXYMetric iXYMetric;
	TInt iDisplayHandle;
	};
    
/**
 * @deprecated 
 */
struct TAlfCurvePathLineParams
    {
    TPoint iStart;
    TPoint iEnd;
    TReal32 iLength;
    };
    
struct TAlfCurvePathLineRealParams
    {
    TAlfRealPoint iStart;
    TAlfRealPoint iEnd;
    TReal32 iLength;
    };
    
/**
 * @deprecated 
 */
struct TAlfCurvePathArcParams
    {
    TPoint iOrigin;
    TSize iSize;
    TReal32 iStartAngle;
    TReal32 iEndAngle;
    TReal32 iLength;
    };

struct TAlfCurvePathArcRealParams
    {
    TAlfRealPoint iOrigin;
    TAlfRealSize iSize;
    TReal32 iStartAngle;
    TReal32 iEndAngle;
    TReal32 iLength;
    };


struct TAlfTransformationRotateParams
    {
    TAlfTimedValue iAngle;
    TReal32 iAxisX;
    TReal32 iAxisY;
    TReal32 iAxisZ;
    };

struct TAlfTransformationTimedParams
    {
    TAlfTimedValue iX;
    TAlfTimedValue iY;
    TAlfTimedValue iZ;
    };

const TInt KAlfTableMappingNumberOfMappedValues = 100;
struct TAlfTableMappingFunctionParams
    {
    TInt iComponent;
    TReal32 iValues[KAlfTableMappingNumberOfMappedValues];
    TReal32 iMappedValues[KAlfTableMappingNumberOfMappedValues];
    };

struct TAlfTextVisualFontColorParams
    {
    TRgb iColor; // If iId is KAknsIIDNone only then iColor is valid     
    TAknsItemID iId; 
    TInt iIndex;     
    };
    
struct TAlfTextVisualSetHighlightRangeParams
    {
    TInt    iStart;
    TInt 	iEnd;
    TRgb	iHighlightColor;
	TRgb	iHighlightTextColor;    
    };    

struct TAlfLCTTextVisualSetTextPaneParams
    {
    TInt iApiId;
    TInt iComponentId;
    TInt iOptionIndex;
    TInt iDrawingOrderIndex;
    TInt iVarietyIndex;
    TInt iColumn;
    TInt iRow;
    };

struct TAlfDisplayCreateParams
    {
    TInt iWindowGroupId;
    TInt iDisplayType;
    };

struct TAlfDisplayCreateParams2
    {
    TInt iWindowGroupId;
    TInt iDisplayType;
    TUid iBufferUid;
    };

struct TAlfGridLayoutFillWeightsParams
    {
    TAlfGridDimension iDim;
    TInt iCount;
    TAlfMetric iWeight;
    };

struct TAlfGridLayoutAppendWeightParams
    {
    TAlfGridDimension iDim;
    TAlfMetric iWeight;
    };
    
struct TAlfGridLayoutInsertWeightParams
    {
    TAlfGridDimension iDim;
    TAlfMetric iWeight;
    TInt iPos;
    };
    
struct TAlfGridLayoutReplaceWeightParams
    {
    TAlfGridDimension iDim;
    TAlfMetric iWeight;
    TInt iPos;
    };
    
struct TAlfGridLayoutRemoveWeightParams
    {
    TAlfGridDimension iDim;
    TInt iPos;
    };
    
struct TAlfGridLayoutWeightParams
    {
    TAlfGridDimension iDim;
    TInt iPos;
    };
    
    
// todo: should rather use streaming
struct TInt2
    {
    TInt2(TInt a1, TInt a2):iInt1(a1),iInt2(a2){}
    TInt iInt1;
    TInt iInt2;
    };
    
struct TInt3
    {
    TInt3(TInt a1, TInt a2, TInt a3):iInt1(a1),iInt2(a2),iInt3(a3){}
    TInt iInt1;
    TInt iInt2;
    TInt iInt3;
    };

struct TReal2
    {
    TReal2(TReal32 a1, TReal32 a2):iReal1(a1),iReal2(a2){}
    TReal32 iReal1;
    TReal32 iReal2;
    };

struct TIntTReal
    {
    TIntTReal(TInt a1, TReal32 a2):iInt(a1),iReal(a2){}
    TInt iInt;
    TReal32 iReal;    
    };

struct TAlfImplementationInformation
    {
    TAlfImplementationInformation(TInt aId = 0, TInt aImplUid = 0, TVersion aVersion = TVersion(0,0,0))
        :iImplementationUid(aImplUid),iImplementationId(aId), iVersion(aVersion){}
    TInt iImplementationUid;
    TInt iImplementationId;
    TVersion iVersion;
    };

struct TAlfVisualPos
    {
    TAlfRealPoint iPoint;
    TInt iTime;
    };

struct TAlfVisualSize
    {
    TAlfRealSize iSize;
    TInt iTime;
    };

struct TAlfVisualPosSize
    {
    TAlfRealPoint iPoint;
    TAlfRealSize iSize;
    TInt iTime;
    };

struct TAlfPosInt
    {
    TPoint iPoint;
    TInt iInt;    
    };

struct TAlfSizeInt
    {
    TSize iSize;
    TInt iInt;
    };

const TInt KAlfTouchEventControlCount = 10;

struct TAlfTouchEvent
    {
    TAdvancedPointerEvent iEvent;
    TInt iControls[KAlfTouchEventControlCount]; // @todo, is it ok to make assumptions, about max amount of controls in client  
    TInt iVisuals[KAlfTouchEventControlCount]; // @todo, same as above
    };
  
struct TAlfBackBufferUpdate
    {
    TUid iCommandUid;
    TUid iBackBufferUid;
    };
    
enum TAlfScreenBufferEventId
	{
	EAlfScreenBufferBufferComplete = 0,
	EAlfScreenBufferEvent = 1,
	};
	
struct TAlfScreenBufferEvent
    {
    // in
    TUid iBufferUid;
    // out
    TInt iEventId;
    TRect iDisplayRect;
    TRect iDirtyRect;
    };
    
struct TAlfScreenBufferBitmaps
    {
    // in
    TUid iBufferUid;
    TInt iPriority;
    TInt iFlags;
    // out
    TInt iBitmapHandle;
    TInt iMaskHandle;
    };

struct TAlfTextureInfoEvent
    {
    TInt iTextureId;
    TSize iTextureSize;
    };
    
struct TAlfFrameBrushImageParams
    {
    TAlfImageParams iImage;
    TAlfXYMetric iEdgeOffset;
    };

struct TAlfFrameBrushFramePartImageParams
    {
    TAlfImageParams iImage;
    TInt iFramePart;
    };
    
struct TAlfDropShadowOffsetPolarParams
    {
    TReal32 iAngle;
    TAlfMetric iDistance;
    TInt iTransitionTime;
    };
    
struct TAlfDropShadowOffsetXYParams
    {
    TAlfTimedPoint iOffset;
    TInt iOffsetUnit;
    };
    
struct TAlfDropShadowColorRgbParams
    {
    TRgb iColor;
    TInt iTransitionTime;
    };
    
struct TAlfDropShadowColorSkinParams
    {
    TAknsItemID iID;
    TInt iIndex;
    TInt iTransitionTime;
    };
struct TAlfQtCommandBufferParams
    {
    TInt iWsClientId;
    TInt iWsWgId;
    TInt iVisualHandle; // return param
    TInt iLength;
    TAlfCommandBufferStatus iBufferStatus;
    TAny* iPtr;    
    };
struct TAlfVisualEffectParams
    {
    TFileName   iFileName;
    TInt 		iGroupHandle;
    TInt        iHandle;
    };
/**
 * Use the following functions to serialize the RArray<T> objects so
 * that they can be sent to server. 
 *
 * Client side should use ExternalizeLC function and the server side
 * the InternalizeL function. 
 */   
const TUint KAlfArrayBufferSeparatorChar = ' ';
    
template <class T>
HBufC8* ExternalizeLC( const RArray<T>& aArray )
    {
    const TInt itemCount = aArray.Count();
    if ( !itemCount)
        {
        return HBufC8::NewLC(0);
        }
    
    const T* firstItem = &aArray[0];
    
    TPtrC8 arrayPtr( (TUint8*)firstItem, itemCount*sizeof(T) );
    
    TBuf8<10> itemCountBuf;
    itemCountBuf.AppendNum( itemCount );
    
    TChar separator( KAlfArrayBufferSeparatorChar );
    
    HBufC8* buffer = HBufC8::NewLC( itemCountBuf.Length() + 1 + arrayPtr.Length() );
    TPtr8 ptr = buffer->Des();
    
    ptr.Append( itemCountBuf );
    ptr.Append( separator );
    ptr.Append( arrayPtr );
    
    return buffer;
    }
   
template <class T> 
void InternalizeL( const TDesC8& aDes, RArray<T>& aArray )
    {
    aArray.Reset();
    if ( !aDes.Length() )
        {
        return;
        }
        
    const TInt separatorIndex = aDes.Locate( TChar( KAlfArrayBufferSeparatorChar ) );
    TInt itemCount = 0;
    TLex8 lexer( aDes.Left( separatorIndex ) );
    User::LeaveIfError( lexer.Val( itemCount ) );
    
    TPtrC8 buffer( &aDes[separatorIndex+1], itemCount*sizeof(T) );
    
    for ( TInt i = 0 ; i < itemCount ;  i++ )
        {
        T* itemPtr = (T*)&buffer[i*sizeof(T)];
        T item = *itemPtr;
        User::LeaveIfError( aArray.Append( item ) );
        }
    }

#endif // ALFCONSTANTS_H
