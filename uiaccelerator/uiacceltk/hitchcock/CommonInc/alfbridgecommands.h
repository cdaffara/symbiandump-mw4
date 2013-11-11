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
/*
 * alfbridgecommands.h
 *
 *  Created on: May 6, 2009
 *      Author: teroviit
 */

#ifndef ALFBRIDGECOMMANDS_H_
#define ALFBRIDGECOMMANDS_H_

enum TAlfDecoderServerBindings
    {
    EAlfDSNewWindow = 100,
    EAlfDSDestroyWindow,    
    EAlfDSSetWindowPos,
    EAlfDSSetWindowSize,
    EAlfDSSetWindowRotation,
    EAlfDSSetWindowOpacity,
    EAlfDSRefreshCallback,
    EAlfDsUpdateTexture,
    EAlfDSReorder, 
    EAlfDSSetWindowActive,

    // 110
    EAlfDSSetWindowFlag,
    EAlfDSClearWindowFlag,
    EAlfDSPostCanvasBuffer,
    EAlfDSSetCursorData,
    EAlfDSSetSurfaceExtent,
    EAlfDsLayerUsesAlphaFlagChanged,
    EAlfDSGroupChained,
    EAlfDSGroupChainBroken,
    EAlfDSMoveWindowToNewGroup,
    EAlfDSCreateNewDisplay,

    // 120
    EAlfDSDestroyDisplay,
    EAlfDSRequestCommandsStart,
    EAlfRequestWrap,
    EAlfDSSetNodeTracking,
    EAlfRequestCommitBatch,
    EAlfRequestCommandReadNotification,
    EAlfDSRequestCommandsEnd,    
    
    EAlfDSGetCommandsStart, // Put all getters between EAlfDSGetCommandsStart and EAlfDSGetCommandsEnd
    EAlfDSGetAlfNativeWindowData,
    EAlfDSGetCommandsEnd,   
    
    // 130
    EAlfEffectFx,
    EAlfStopEffectFx,
    EAlfStopControlEffectFx,
    EAlfControlEffectFx,
    EAlfRegisterEffectFx,
    EAlfUnregisterEffectFx,
    EAlfUnregisterAllFx,
    EAlfEffectFxBeginSyncronizedGroup,
    EAlfEffectFxEndSyncronizedGroup,
    EAlfDSSetCapturingBitmap,
    EAlfDSSetCoveringBitmap,
    
    //140
    EAlfDSSetFadeEffect,
    EAlfReleaseTemporaryChunk,
    EAlfBridgeSetScreenRotation,
    EAlfDSSynchronize,
    EAlfDSSetWindowArea,
    EAlfDSSetTransparencyAlphaChannel,
    EAlfDSIncludeToVisibilityCalculation,
    EAlfDSSetDistractionWindow,

    KUnInitialized // This must be kept as last command!
    };

#endif /* ALFBRIDGECOMMANDS_H_ */
