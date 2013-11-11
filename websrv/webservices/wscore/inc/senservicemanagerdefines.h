/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/











#ifndef SEN_SERVICE_MANAGER_DEFS_H
#define SEN_SERVICE_MANAGER_DEFS_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT(KSenUnderline, "_");
_LIT8(KSenAttrInternalSC, "_internal_");
_LIT8( KSenAttrDisableDispatcherSC, "_disable_dispatcher_");

#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    //server name
    #if defined (EKA2)
    _LIT(KSenServiceManager,                "!Sen");
    #else
    _LIT(KSenServiceManager,                "Sen");
    #endif
    //server file name 
    _LIT(KSenServiceManagerFileName,        "Sen");
    _LIT(KSenServiceManagerPanic,           "Sen");
    _LIT(KSenServiceManagerSemaphoreName,   "SenSemaphore");
#else //RD_SEN_COMPILE_SIS_PACKAGE_FILES is defined:
    //server name
    #if defined (EKA2)
    _LIT(KSenServiceManager,                "!wscore");
    #else
    _LIT(KSenServiceManager,                "wscore");
    #endif
    //server file name 
    _LIT(KSenServiceManagerFileName,        "wscore");
    _LIT(KSenServiceManagerPanic,           "wscore");
    _LIT(KSenServiceManagerSemaphoreName,   "WsSemaphore");
#endif //RD_SEN_COMPILE_SIS_PACKAGE_FILES

// Maximum delta time after which any XML DB item is considered old, 
//and will be discarded at parsing phase, instead of deserialization:
const TUint32 KMaxTicks	= 86400*14*1000; // = 1209600000 ticks = 2 weeks
                                         // 1 tick = 1 millisec
                                         // 86400 sec = 1 day
                                         // 86400*1000 millisec = 1 day
                                         // 86400*14*1000 = 1209600000 millisec = 2 weeks




// Maximum number of async message slots to reserve for client server session.
const TUint KAsyncMessageSlots = 10;

// Default number of message slots to reserve for client server session.
const TUint KDefaultMessageSlots = KAsyncMessageSlots+1;

// Maximum number of server message slots avilable 
const TUint KMaxServMessageSlots = 10; 
// SenServiceManager UID
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    const TUid KServerUid3 = {0x101F96F4};
#else
    const TUid KServerUid3 = {0x101F972C}; //SIS package UID
#endif

// The server version. A version must be specified when
// creating a session with the server
const TUint KWsfServMajorVersionNumber=0;
const TUint KWsfServMinorVersionNumber=1;
const TUint KWsfServBuildVersionNumber=1;

// Max number of attempts that each client (SC, HC, SM) should 
// perform prior giving up client-server connection initialization:
const TInt KSenMaxClientConnectionOpeningAttempts = 5;

//Introduced a PropertKeyBasenumber to avoid any key collisions with
//the client applications further.
const TInt KSenRPropertyKeyBaseNumber = 0x6A954E5;

// DATA TYPES

// SenServiceManager panic codes
enum TWsfServPanic
    {
    EBadRequest = 1,
    EBadDescriptor,
    ESrvCreateServer,
    EMainSchedulerError,
    ECreateTrapCleanup,
    ESrvSessCreateTimer,
    EReqAlreadyPending
    };

// opcodes used in message passing between client and server
enum TWsfServRqst
    {
    ESenServInitialize,
    ESenServTransaction,
    ESenServInstallFramework,
    ESenServAssociateService,
    ESenServDissociateService,
    ESenServRegisterIdentityProvider,
    ESenServUnregisterIdentityProvider,
    ESenServRegisterServiceDescription,
    ESenServUnregisterServiceDescription,
    ESenServGetServiceDescriptionsByPattern,
    ESenServGetServiceDescriptionsByUri,
    ESenServIsReady,
    ESenServHasFacet,
    ESenServServerMessagesOnOff,
    ESenServGetLengthOfServiceDescriptionByUri,
    ESenServGetLengthOfServiceDescriptionByPattern,
    ESenServRequestServiceDescription,
    ESenServReceiveServiceDescription,
    ESenServSendMsgAndGetTxnId,
    ESenServSendSoapMsgAndGetTxnId,
    ESenServStartTransaction,
    ESenServTransactionCompleted,
    ESenServCancelSession,
    ESenServSetTransportProperties,
    ESenServTransportProperties,
    ESenServEstablishHostletConnection,
    ESenServAwaitHostletRequest,
    ESenServProvideHostletResponse,
    ESenServMoveFile,
    ESenServSendFileHandle,
    ESenServMoveChunk,
    ESenServGetGredentials,
    ESenServAddCredential,
    ESenServRemoveCredential,
    ESenServCancelRequest,
    ESenServConnectionID,
    ESenServConnectionIdentityProvider,
    ESenObserveTransfer,
    ESenObserveAuthCallback,
    ESenTransferProgress,
    ESenServSearchIdentityProvider,
    ESenTrafficDetails,
    ESenObserveCoBranding,
    ESenServGetIdentityProviders
#ifdef __ENABLE_ALR__
    ,ESenServRegisterMobiltyObserver,
    ESenServMigrateToPreffredCarrier,
    ESenServNewCarrierAccepted
#endif
    };

enum TSenClServRqst
    {
    ESenCliServInitialize,
    ESenCliServAuthInitialize,
    ESenCliServTransferProgress,
    ESenCliServReAuthNeeded,
    ESenCliServCoBrandingInitialize,
    ESenCliServGoNoGo
#ifdef __ENABLE_ALR__
    ,ESenCliServMobilityInitialize,
    ESenCliServPrefferedCarrierAvailable,
    ESenCliServNewCarrierActive,
    ESenCliServMobilityError    
#endif
    };

struct TTransferProgress
    {
    TInt    iTxnId;
    TBool   iIncoming;
    TInt    iProgress;
    TBool   iSoap;
    };


NONSHARABLE_CLASS( TFileOutgoingTransferProgressBase )
    {
    public: // Data:
        TInt    iTxnId;
        TBool   iIsIncoming;
        TInt    iProgress;
        TInt    iCid;
        TInt    iCid2;
        TBool   iHasCidPostfix;
    };

// Operation codes used by server to indicate
// which asynchronous service has completed
enum TWsfServRqstOpCode
    {
    ESenServInitialized             = 1,
    ESenServRequestResponsed        = 2,
    ESenServRequestCancelled        = 4,
    ESenReAuthAndResendNeeded       = 5,
    ESenResendNeeded                = 6, 
    ESenHostletRequestPending       = 7,
    ESenOnServiceComplete           = 8,
    ESenReAuthNeeded                = 9,
    ESenInternalError               = 5005
    };

#endif //SEN_SERVICE_MANAGER_DEFS_H

// End of File

