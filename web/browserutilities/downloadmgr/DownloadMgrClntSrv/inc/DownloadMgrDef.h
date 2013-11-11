/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     This file contains definitions used by Download Mgr Server.
*
*/


#ifndef __DOWNLOADMGR_DEF_H__
#define __DOWNLOADMGR_DEF_H__

#include <e32base.h>

//the server version. A version must be specifyed when creating a session with the server
const TUint KDownloadMgrMajorVersionNumber=0;
const TUint KDownloadMgrMinorVersionNumber=1;
const TUint KDownloadMgrBuildVersionNumber=1;

const TUint KDefaultHeapSize = 0x10000;
const TUint KDefaultMaxHeapSize = 0x200000;

const TInt KDownloadMgrShutdownInterval = 10000000;

// Literals
_LIT( KDownloadMgrServerName, "DownloadMgrServer" );
_LIT( KDownloadMgrMainThreadName, "!DownloadMgrServer" );
_LIT( KDownloadMgrServerSemaphore, "DownloadMgrStartupSemaphore" );
_LIT( KDownloadMgrServerExe, "DownloadMgrServer.exe" );
#if defined(__WINS__)
	_LIT( KDownloadMgrServerDllName, "DownloadMgr.dll");
#endif

// Server UID
const TUid KUidDownloadMgrSrvExe = { 0x10008D60 };
const TUid KUidDownloadMgrClntSrv = { 0x10008D5F };

//opcodes used in message passing between client and server
enum TDownloadMgrRqst
	{
    EHttpDownloadMgrInitialize,                   // = 0
    EHttpDownloadMgrCreateDownload,               // = 1
    EHttpDownloadMgrUnused,                       // = 2
    EHttpDownloadMgrAttach,                       // = 3
    EHttpDownloadMgrPauseAll,                     // = 4
    EHttpDownloadMgrStartAllL,                    // = 5
    EHttpDownloadMgrResetAll,                     // = 6
    EHttpDownloadMgrDeleteAll,                    // = 7
    EHttpDownloadMgrDisconnect,                   // = 8

    EHttpDownloadMgrGetIntAttribute,              // = 9
    EHttpDownloadMgrGetBoolAttribute,             // =10
    EHttpDownloadMgrGetStringAttribute,           // =11
    EHttpDownloadMgrGetString8Attribute,          // =12

    EHttpDownloadMgrSetIntAttribute,              // =13
    EHttpDownloadMgrSetBoolAttribute,             // =14
    EHttpDownloadMgrSetStringAttribute,           // =15
    EHttpDownloadMgrSetString8Attribute,          // =16
    EHttpDownloadMgrSessionEventSubscription,     // =17
    EHttpDownloadMgrSessionEventCancel,           // =18

    EHttpDownloadAttach,                          // =19
    EHttpDownloadStart,                           // =20
    EHttpDownloadPause,                           // =21
    EHttpDownloadReset,                           // =22
    EHttpDownloadDelete,                          // =23
    EHttpDownloadMove,                            // =24
    EHttpDownloadClose,                           // =25
    EHttpDownloadEventSubscription,               // =26
    EHttpDownloadEventCancel,                     // =27

    EHttpDownloadGetIntAttribute,                 // =28
    EHttpDownloadGetBoolAttribute,                // =29
    EHttpDownloadGetStringAttribute,              // =30
    EHttpDownloadGetString8Attribute,             // =31

    EHttpDownloadSetIntAttribute,                 // =32
    EHttpDownloadSetBoolAttribute,                // =33
    EHttpDownloadSetStringAttribute,              // =34
    EHttpDownloadSetString8Attribute,             // =35
    EHttpDownloadBufferAttributes,                // =36
    
    EHttpDownloadGetRFile,                        // =37
    EHttpDownloadSetRFile,                        // =38
    EHttpDownloadSetOnError,                      // =39
    EHttpDownloadAttachToDownload,                // =40

    // Define your item here! EHttpDownload...    // =n
	EHttpDownloadCloseCompleted,				  // =41	
    
    EHttpDownloadDataAttribute,                   // =42
    EHttpDownloadTrackAttribute,				  // =43

    /* 
    * This should be the last item and the opcode must be incremented as well.
    * It is also important to make the changes on KDMgrServerPolicyRanges
    * in DownloadMgrServer.cpp according to this!!!
    */
    EHttpDownloadMgrNotSupported                  // =44 (n+1)
	};

#endif /* __DOWNLOADMGR_DEF_H__ */

