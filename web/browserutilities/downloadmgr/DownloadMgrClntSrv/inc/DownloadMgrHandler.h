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
*     This file contains the definition of the  receive handler of Download Mgr Server.
*
*/



#ifndef __DOWNLOADMGR_RECEIVEHANDLER_H__
#define __DOWNLOADMGR_RECEIVEHANDLER_H__

#include "DownloadMgrClient.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class RHttpDownload;
class RHttpDownloadMgr;
class MHttpDownloadMgrObserver;

// CLASS DECLARATION

/**
*  Handles the responses of the asyncron services.
*  It can be created by a RHttpDownloadMgr session or 
*  RHttpDownload subsession.
*
*  @lib -
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( CDownloadMgrHandler ) : public CActive
	{
    public:

        /**
        * C++ default constructor.
        */
        CDownloadMgrHandler( MHttpDownloadMgrObserver* aHandler,
                             RHttpDownload* aSubSession );

        /**
        * C++ default constructor.
        */
        CDownloadMgrHandler( RHttpDownloadMgr* aSession );

        /**
        * Destructor.
        */
        ~CDownloadMgrHandler();

    public:

        /**
        * Starts handle request completion event.
        * @param -
        * @return None.
        */
        void StartL();

        /**
        *  Handler type
        *
        *  @lib -
        *  @since Series 60 2.8
        */
        enum THandlerType
            {
            EHttpDownloadMgr,
            EHttpDownload
            };

    private: // New functions
    
        void AdjustPriority( THttpDownloadEvent& aCurrentDlEvent );
        
    private: // From CActive

        /**
        * Handles an active object’s request completion event.
        * @param -
        * @return None.
        */
        void RunL();

        /**
        * Implements cancellation of an outstanding request.
        * @param -
        * @return None.
        */
        void DoCancel();

    private:
        // Reference the owner
        RHttpDownload*            iDownloadSubSession;  ///< NOT Owned.
        RHttpDownloadMgr*         iDownloadSession;     ///< NOT Owned.
        // Points to the observer
        MHttpDownloadMgrObserver* iHandler;  ///< NOT Owned.
        // Possible event parameters.
        TPckgBuf<TInt32>           iDownloadState;
        TPckgBuf<TInt32>           iProgressState;
        TPckgBuf<TInt>             iHandle;		// Contains masked values of 'handle' and 'media index'
        										// Must be unmasked before use.
        HBufC8*                   iPackedEventAttribs; ///<Owned  Buffer for event attributes
        TPtr8                     iPtr;                ///<Client/Server framework needs this pointer to be saved        
        // Indicates that the corresponding download subsession is closed
        TBool*                    iClosed;
        // Indicates the owner of the handler
        THandlerType              iHandlerType;
        // Storage for the 'previous' download event. Used for priority adjustments.
        THttpDownloadEvent        iPrevDlEvent;
	};


#endif /* __DOWNLOADMGR_RECEIVEHANDLER_H__ */

