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
* Description:   
*
*/



#ifndef HUIWSCANVASCOMMANDS_H_
#define HUIWSCANVASCOMMANDS_H_


enum TSgcCanvasCommands
     {
     EAlfSendSyncDataBlock = 1,
     EAlfDrawCommandsStart,
     EAlfCommandEndMarker,
     EAlfSendEndMarker,

     EAlfBitBlt,
     EAlfBitBltRect,
     EAlfBitBltMasked,
     EAlfBitBltMaskedPoint,
     EAlfCombinedBitBlitMasked,
     
     //10
     EAlfResetClippingRegion,
     EAlfClear,
     EAlfClearRect,
     EAlfResetBrushPattern,
     EAlfResetFont,
     EAlfDrawArc,
     EAlfDrawPie,
     EAlfDrawBitmap1,
     EAlfDrawBitmap2,
     EAlfDrawBitmap3,

     //20
     EAlfDrawBitmapMasked,
     EAlfDrawRoundRect,
     EAlfDrawPolyLine,
     EAlfDrawPolyLineNoEndPoint,
     EAlfDrawPolygon,
     EAlfDrawEllipse,
     EAlfDrawLine,
     EAlfDrawLineTo,
     EAlfDrawLineBy,
     EAlfDrawRect,

     //30
     EAlfDrawText1,
     EAlfDrawText2,
     EAlfDrawText3,
     EAlfDrawText4,
     EAlfDrawText5,
     EAlfDrawTextVertical1,
     EAlfDrawTextVertical2,
     EAlfDrawTextVertical3,
     EAlfDrawTextVertical4,
     EAlfDrawTextVertical5,

     //40
     EAlfMoveTo,
     EAlfMoveBy,
     EAlfPlot,
     EAlfReset,
     EAlfSetBrushColor,
     EAlfSetBrushOrigin,
     EAlfSetBrushStyle,
     EAlfSetClippingRegion,
     EAlfSetDrawMode,
     EAlfSetOrigin,
     
     //50
     EAlfSetPenColor,
     EAlfSetPenStyle,
     EAlfSetPenSize,
     EAlfSetTextShadowColor,
     EAlfSetCharJustification,
     EAlfSetWordJustification,
     EAlfSetUnderlineStyle,
     EAlfSetStrikethroughStyle,
     EAlfSetBrushPattern,
     EAlfSetBrushPattern2,

     //60
     EAlfSetFont,
     EAlfCopyRect,
     EAlfUpdateJustification,
     EAlfUpdateJustificationVertical,
     EAlfSetFontNoDuplicate,
     EAlfHasBrushPattern,
     EAlfHasFont,
     EAlfBrushColor,
     EAlfPenColor,
     EAlfTextShadowColor,
 
     //70
      EAlfCopySettings,
     EAlfSetClippingRect,
     EAlfCancelClippingRect,
     EAlfSetFaded,
     EAlfSetFadingParameters,
     EAlfFadeArea,
     EAlfMapColors,
     EAlfSetUserDisplayMode,
     EAlfUseFont,
     EAlfSetUpdateRegion,

     // 80
     EAlfSetShapeRegion,
     EAlfContentTag,
     EAlfFrameFlags,
     EAlfFrameOrientation,    
     EAlfPacketReady,
     EAlfPacketNotReady,
     EAlfPacketPadding,
     EAlfDrawCommandsEnd,
     EAlfSetWindowId,
     EAlfWindowInactivateWindow,
     
     // 90
     EAlfWrap,
     EAlfJumpToAnotherChunk,
	 EAlfDestroyChunk,
     EAlfSetExtent,
     EAlfNodeCreated,
     EAlfNodeReleased,
     EAlfNodeActivated,
     EAlfNodeExtentChanged,
     EAlfNodeSiblingOrderChanged,
     EAlfNodeFlagChanged,
     
     // 100
     EAlfNodeAttributeChanged,
     EAlfNodeFadeCountChanged,
     EAlfNodeTransparentRegionChanged,
     EAlfNodeLayerAdded,
     EAlfNodeMovedToWindowGroup,
     EAlfNodeWindowGroupChained,
     EAlfNodeWindowGroupChainBrokenAfter,
     EAlfCommandNotInitialized,
     EAlfCommitBatch,
     EAlfFrameContainsUnsupportedCommands,
     
     // 110     
     EAlfDebugTrackNode,
     EAlfNodeLayerExtentChanged,
     EAlfNodeLayerUsesAlphaFlagChanged,
     EAlfNodeFadeAllChildren,
     EAlfCommandIndexArray,
     EAlfCommandIndexArrayHeader,
     EAlfNodeSetWindowArea,    
     
     // This must be kept as last command!
     EAlfLastCommand
     };

enum TAlfSendBufferFrameFlags
        {
        EAlfTransparentContent = 0x01,
        EAlfTransparentContentFlush = 0x02
        };

// drawing commands for the window should be ignored 
#define KWindowIsDSAHost 2

#endif /* HUIWSCANVASCOMMANDS_H_ */
