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
* Description:  Base class for client side request handling.
*
*/


#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32base.h>
#include <e32std.h>

#include <ConnectionObserver.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RFeedsInterface;
class CRequestHandler;

/**
*  The CRequestHandler observer interface.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class MRequestHandlerObserver : public MConnectionObserver
	{
    public:  // New Methods
        /**
        * Requests the observer to send a command to the server.
        *
        * @since 3.1
        * @param aFunction The function number identifying the request. 
        * @param aArgs A set of up to 4 arguments and their types to be passed to the server.
        * @param aStatus The request status object used to contain the completion status of the request.
        * @return void.
        */
        virtual void SendAsyncCommand(TInt aFunction, const TIpcArgs& aArgs, 
                TRequestStatus& aStatus) const = 0;
                
        /**
        * Notifies the observer that the request is completed.
        *
        * @since 3.1
        * @param aRequestHandler The request handler.
        * @param aStatus The result code of the request.
        * @return void.
        */
        virtual void RequestHandlerCompleted(CRequestHandler& aRequestHandler,
                TInt aStatus) = 0;

        /**
        *
        */
        virtual void CancelAllCommandsL() = 0;
	};


/**
*  Base class for client side request handling.
*
*  \b Library: FeedsEngine.lib
*
*  @since CCB (7.1)
*/
class CRequestHandler: public CBase
	{
public:  // Data types
    enum TRequestHandlerType
        {
        ENone = 0,
        // folder related
        EFetchRootFolderItem,
        EWatchForChanges,
        EAddFolderItem,
        EDeleteFolderItem,
        EChangeFolderItem,
        EMoveFolderItem,
        EMoveFolderItemTo,
        EUpdateFolderItem,
        EImportOPML,
        EExportOPML,
        // feed related
        EFetchFeed,
        EUpdateItemStatus,
        // general client related
        EChangeSettings,
        EFetchSettings,
        EWatchForSettingChanges,
        ECancelAll,
        EDisconnectManualUpdateConnection,
        EPrintDBTables
        };

public:

        /**
         * C++ default constructor.
         */
        CRequestHandler();

        /**
        * Destructor
        */
        virtual ~CRequestHandler();

    };

#endif  // REQUEST_HANDLER_H
// End of File
