/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the public messages sent between the feeds server and
*                its clients.
*
*/


#ifndef FEEDS_SERVER_MSG_H
#define FEEDS_SERVER_MSG_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32std.h>

// CONSTANTS
_LIT(KFeedsServerName, "FeedsServer");

const TUint KFeedsServerMajorVersionNumber = 1;
const TUint KFeedsServerMinorVersionNumber = 0;
const TUint KFeedsServerBuildVersionNumber = 0;

const TInt KFeedsServerUid = {0x1020728E};

const TInt KMaxFeedsServerMessage = 20480;

const TInt KFeedsServerPackedRequestArg = 0;
const TInt KFeedsServerPackedResponseTypeArg = 1;
const TInt KFeedsServerPackedResponseArg = 2;
// when transferring file handle
const TInt KFeedsServerPackedRFsSlot = 1;
const TInt KFeedsServerPackedRFileSlot = 2;

const TInt KFeedsServerGetFeedUrlArg = 2;
const TInt KFeedsServerGetRootFolderListIdArg = 2;


// MACROS

// DATA TYPES
enum TFeedsServerRequest
    {
    // RFeedsServer
    EFeedsServerGetSettings,
    EFeedsServerChangeSettings,
    EFeedsServerCancelAll,

    // RFolderItem
    EFeedsServerOpenFolderSession,
    EFeedsServerCloseFolderSession,
    EFeedsServerGetRootFolder,
    EFeedsServerAddFolderItem,
    EFeedsServerChangeFolderItem,
    EFeedsServerDeleteFolderItems,
    EFeedsServerMoveFolderItems,
    EFeedsServerMoveFolderItemsTo,
    EFeedsServerUpdateFolderItems,
    EFeedsServerWatchFolderList,
    EFeedsServerImportOPML,
    EFeedsServerExportOPML,
    EFeedsServerCompleteWatchFolderList,

    // Feed
    EFeedsServerOpenFeedSession,
    EFeedsServerCloseFeedSession,
    EFeedsServerGetFeed,
    EFeedsServerChangeFeedItemStatus,

    // FeedsSession
    EFeedsServerSetConnection,
    EFeedsServerDisconnectManualUpdateConnection,
    EFeedsServerWatchSettings,
    EFeedsServerCompleteWatchSettings,

    // Debug
    EFeedsServerPrintDBTables
    };

enum TFeedsServerResponseType
    {
    EFeedsServerInitPayload = 0,
    EFeedsServerTokensPayload = 1,
    EFeedsServerStringTablePayload = 2,
    EFeedsServerPayloadDone = 3,
    EFeedsServerConnectionNeeded = 4,
    EFeedsServerManualUpdateDone = 5
    };
    
enum TFeedsServerLeave
    {
	ESomeError
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif      // FEEDS_SERVER_MSG_H
            
// End of File
