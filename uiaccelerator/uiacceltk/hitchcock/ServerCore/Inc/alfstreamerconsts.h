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
* Description:   IPC command ids for server and client 
*
*/



#ifndef __ALFSTREAMERCONSTS_H__
#define __ALFSTREAMERCONSTS_H__

#include "alfstreameruids.h"

enum 
    {
    EAlfDecodSLoadPlugin,
    EAlfDecodSUnloadPlugin,
    EAlfDecodSSendSynch,
    EAlfDecodSSendAsynch,
    EAlfDecodSCancelAsynch,
    EAlfDecodSPrepareFrame,
    EAlfBridgerAsyncronousData,
    EAlfBridgerSendChunk,
    EAlfBridgerSendSyncData,
    EAlfBridgerRequestDataBlock,
    EDsNotifyNativeWindowData,
    EAlfBridgerBlindSend,
    EAlfSetScreenRotation, 
    EAlfGetNativeWindowHandles,
    EAlfSynchronize,
    EAlfPostDataToCompositionClient,
    EAlfPostDataToCompositionTarget,
    EAlfGetListOfWGsHavingInactiveSurfaces, 
    EAlfQueueRequestBGSessions,
    EAlfGetNumberOfActiveEffects,
    EAlfRequestSignal,
    EAlfCompleteSignal,
    EAlfSetDistractionWindow,
    EAlfVolunteerForGoomTarget,
    EAlfExcludeFromGoomTargets

    };

enum TAlfCompOps{
    KAlfCompOpCreateSource = 15000,   
    KAlfCompOpCreateToken,
    KAlfCompOpBindSourceToToken,
    KAlfCompOpEnableAlpha,
    KAlfCompOpSetOpacity,
    KAlfCompOpSetRotation, 
    KAlfCompOpSetZOrder,
    KAlfCompOpSetExtent,
    KAlfCompOpEnableKb,
    KAlfCompOpRequestEvent,
    KAlfCompOpCancelEventRequest,
    KAlfComOpSetBackgroundAnim,
    KAlfCompOpSessionClosed,
    KAlfCompOpSetSRect,
    
    KAlfCompositionFrameReady = 16000,
    KAlfCompositionLowOnGraphicsMemory,
    KAlfCompositionTargetHidden,
    KAlfCompositionTargetCreated,
    KAlfCompositionWServReady,
    KAlfCompositionWServScreenNumber,
    KAlfCompositionSourceScreenNumber,
    KAlfCompositionGoodOnGraphicsMemory,
    KAlfCompositionTargetVisible,
    KAlfCompositionTargetHiddenBGAnim,
    KAlfCompositionTargetVisibleBGAnim,
    
    KAlfCompositionLayoutSwitchComplete
    
    };

struct TAlfNativeWindowData
    {
    TInt  iScreenNumber;
    TInt  iAlfWindowGrpId;
    TUint iAlfWindowHandle;
    };

/* bitfield*/
enum TAlfSignalFlags
    {
    EAlfSignalEffectStarted = 0x1,
    EAlfSignalEffectComplete = 0x2,     
    };
    
enum TAlfWindowGroupListType
    {    
    EAlfInactiveWgs = 0,
    EAlfAllWgsWithSurface,
    EAlfVolunteersForCommonGood
    };
     
#endif
