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
* Description:   TFX server definitions. Contains various names and numbers
*                used in the server.
*
*/


#ifndef _SERVERDEF_H_
#define _SERVERDEF_H_

#include <e32base.h>

_LIT(KTfxServerName,"TfxServer");
_LIT(KTfxServerImageName, "TfxServer");
_LIT(KAnyName, "*");

/** transition server version numbers */
const TInt KTfxServerMajorVN = 0;
const TInt KTfxServerMinorVN = 6;
const TInt KTfxServerBuildVN = 0;

/** transition server uid */
const TUid KTfxServerUid = { 0x10281F7D };

/** RProperty value for server status */
const TUint KTfxServerStatus = 0x10281F7D; 

/** transition server stack size */
const TInt KTfxServerStackSize = KDefaultStackSize;
/** transition server minimum heap size */
const TInt KTfxServerHeapMinSize = 64*1024; //64KB
/** transition server maximum heap size */
const TInt KTfxServerHeapMaxSize = 4*1024*1024; //4MB

/** The time between start of frames the engine will try to keep, in ms. (1000/fps) */
const TInt KDefaultWantedTime = 67;
/** The minimum time between end of a frame to start of next frame allowed, in ms
    increase this to give more time to other threads.
    A high number will decrease framerate on complex scenes. */
const TInt KDefaultMinTime = 22;

/* 
    default values for cache size
    see TCacheSizeProperties for comments
*/
const TInt KDefaultMaxBytesOngoing = 1024*1024; // 1MB
const TInt KDefaultMaxBytesBetween = 0; //

/*
    The maximum cache size between effects
    if the phone is low on ram
*/
const TInt KLowMemoryMaxBytesBetween = 0; 

/** priority of repository handler. Has to be higher then message loop to ensure repository changes are heeded */
const TInt KTfxServerRepositoryHandler = 110; 
/** priority of CServer2 object. only repository handler is higher. */
const TInt KTfxServerMessageLoopPrio = 100;
/** priority of frame timer. needs to be kept high, but lower then message loop */
const TInt KTfxServerFrameTimerPrio = 80;
/** priority of the requesthandler. needs to be higher then the frame timer, but lower then message loop */
const TInt KTfxServerRequestHandlerPrio = 90;

/** microseconds to wait after InitControltransition before assuming client has failed */
const TInt KStartControlTransitionTimeout = 2000000;  //2s

/** Window Server Secure ID */
const TInt KWServSID = 268450592;

/**  Nokia VID: = VID_DEFAULT */
const TInt KNokiaVID  = 0x101FB657;

/** Ranges for TFX messages */
const TInt KTfxRanges[] =
    {
    0, //range is 0-10 inclusive
    10, //range is 11-254 inclusive
    255 // message 255 (shutdown)
    };

/** Element Indexes for PolicyElement */  
const TUint8 KTfxElementsIndex[] =
    {
    0,                             
    CPolicyServer::EAlwaysPass,
    1
    };

/** Policy Elements */
const CPolicyServer::TPolicyElement KTfxElements[] =
    {
    {_INIT_SECURITY_POLICY_S0(KWServSID), CPolicyServer::EFailClient},
    {_INIT_SECURITY_POLICY_V0(KNokiaVID), CPolicyServer::EFailClient}
    };

/** Policy definition */    
const CPolicyServer::TPolicy KTfxPolicy =
    {
    CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
    sizeof(KTfxRanges) / sizeof(TInt),                   
    KTfxRanges,
    KTfxElementsIndex,
    KTfxElements,
    };

/** opcodes used in message passing between client and server */
enum TTfxServerServRqst
    {
//Wserv only:
	ETfxServerOpRegisterControlDrawer, //0
	ETfxServerOpDeregisterControlDrawer,
//Wserv only:
	ETfxServerOpStartTransition,
	ETfxServerOpFullscreenBegin,
	ETfxServerOpFullscreenEnd,         
//Wserv only:
	ETfxServerOpDSABegin,              //5
	ETfxServerOpDSAEnd,
//Wserv only:
	ETfxServerOpFailedControlTransition,
//Wserv only:
	ETfxServerOpReqUnredirEvent,
	ETfxServerOpCancelUnredirEvent,
//Any thread:
	ETfxServerOpSendStartTick,         //10
	ETfxServerOpRequestFinishedEvent,
//Any thread:
	ETfxServerOpFullscreenAbort,
	ETfxServerOpControlPolicyUpdate,
	ETfxServerOpBeginGroup,
	ETfxServerOpEndGroup,			   //15
	ETfxServerOpAddFullscreenKML,      
	ETfxServerOpRemoveFullscreenKML,
	ETfxServerOpControlPolicyRemove,
	ETfxServerOpControlPolicyGet,      //20
	ETfxServerOpControlPolicyRequest,  
	ETfxServerOpBlockFullscreenUid,
	ETfxServerOpRegisterControlKml,
	ETfxServerOpRemoveControlKml,
	ETfxServerOpControlCancelPolicyUpdate, //25
//Any thread:
	ETfxServerOpSetWantedTime,
	ETfxServerOpSetMinTime,
	ETfxServerVerifyConnection,
	ETfxServerOpAddFullscreenKMLEx,    
//Any thread:
	ETfxServerOpDumpClients,              //30
//Any thread:
    ETfxServerOpHandover,
    ETfxServerOpUpdateLiveBuffer,
    ETfxServerOpControlTransitionAbort,
    ETfxServerOpRemoveAllKml,
    ETfxServerOpFreeRam,
    ETfxServerOpMemoryGood,            //36
    ETfxServerOpRegisterControlKmlEx,
    ETfxServerOpControlSetFrameTime,
    ETfxServerOpControlSetMinFrameTime,
// ListBox opcodes
    ETfxServerOpListBoxStart = 100,
    ETfxServerOpListBoxRegister,
    ETfxServerOpListBoxMakeMove,
    ETfxServerOpListBoxGetObsoleteItems,
    ETfxServerOpListBoxRequestFrame,
    ETfxServerOpListBoxGetDirtyRect,
    ETfxServerOpListBoxInvalidate,
    ETfxServerOpListBoxSetFrameTime,
    ETfxServerOpListBoxSetMinFrameTime,
    ETfxServerOpListBoxDeregister,
    ETfxServerOpListBoxVerifyKML,
    ETfxServerOpListBoxSetBorderDims,
    ETfxServerOpListBoxRegisterKML,
    ETfxServerOpListBoxUnregisterKML,
    ETfxServerOpListBoxInPosition,
    ETfxServerOpListBoxSetDrawing,
#ifdef RD_UI_TRANSITION_EFFECTS_TOUCH_P2
    ETfxServerOpListBoxLongTapStart,
    ETfxServerOpListBoxLongTapEnd,
#endif

    ETfxServerOpListBoxEnd,
 // Shutdown
    ETfxServerOpShutdown = 255
    };

enum TTfxServPanic
    {
    EAlreadyRegistered,
    ENotRegistered
    };

/**
* This is a utility class that retrieves the name and version number 
* of the tfxserver. 
*/
class KSCliDefinitions
    {
public:
    static const TDesC&			ServerAndThreadName();
    static const TDesC&						ServerImageName();
    static TUidType							ServerUidType();
    static TVersion							Version();
};

/**
 * Holds animation and cache priority information of a fullscreen transition
 */
class TFullscreenInfo
	{
public:
	TFullscreenInfo();
	TFullscreenInfo(TInt aActionId, TInt aCachePriority, TInt aWantedTime, TInt aMinTime);
	TInt iActionId;
	TInt iCachePriority;
	TInt iWantedTime;
	TInt iMinTime;
	};

/**
 * Holds cache size properties
 */
class TCacheSizeProperties
	{
public:
/* 
	Maximum amount of bytes that may be cached while a transition
	is ongoing. If this value is less than the total size of the 
	decoded images in the effect the images has to be decoded every 
	frame which will have a huge impact on performance.
	Default is KDefaultMaxBytesOngoing
*/
    TInt32 iMaxBytesOngoing; 
/*
    Maximum amount of bytes that may be cached when a transition
    is not ongoing.
    Default is KDefaultMaxBytesBetween.
*/
    TInt32 iMaxBytesBetween;
    };

struct TParticipantInfo
  {
  TPoint iTl;
  TInt iBitmapHandle;
  TUint32 iId;
  };
  
struct TWantedTimes
{
	TInt32 iWantedTime  : 16;
	TInt32 iMinTime 	: 16;
};

union TWantedTimesPacked
{
	struct TWantedTimes times;
	TInt32 packed;
};

const TUint KMaxDirtyRegions = 5;

/**
 * The time between start of frames the engine will try to keep, 
 * in microseconds.
 */
const TInt KDefaultListBoxWantedTime = 33;

/** 
 * The minimum time between end of a frame to start of next frame allowed, 
 * in microseconds.
 * Increase this to give more time to other threads.
 * A high number will decrease framerate on complex scenes.
 */
const TInt KDefaultListBoxMinTime = 10;

_LIT( KTfxListBoxMutexName, "TfxListBox%d" );

#endif
