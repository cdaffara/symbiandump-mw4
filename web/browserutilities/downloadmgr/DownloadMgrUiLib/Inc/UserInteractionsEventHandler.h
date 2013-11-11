/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  User Interactions event handler
*
*/



#ifndef USERINTERACTIONSEVENTHANDLER_H
#define USERINTERACTIONSEVENTHANDLER_H

//  INCLUDES
#include "AsyncEventHandlerBase.h"
#include <badesca.h>

// CONSTANTS
LOCAL_D const TInt KMaxDownloadSizeTextLength = 64;
LOCAL_D const TInt KMaxLength = 1024;

// FORWARD DECLARATIONS
class CUserInteractionsUtils;
class CAknPopupList;
class RApaLsSession;

// CLASS DECLARATION

/**
*  User Interactions event handler
*/
NONSHARABLE_CLASS( CUserInteractionsEventHandler ) : public CAsyncEventHandlerBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CUserInteractionsEventHandler( RHttpDownload& aDownload, 
                                       THttpDownloadEvent aEvent, 
                                       MDownloadMgrUiLibRegModel& aRegistryModel, 
                                       CAsyncEventHandlerArray& aOwner,
                                       CUserInteractionsUtils& aUiUtils );

        /**
        * Destructor.
        */
        virtual ~CUserInteractionsEventHandler();

    public: // New functions

        /**
        * Check the free disk space.
        * @return ETrue - the indicate dsize will be below critical level.
        */
#ifdef RD_MULTIPLE_DRIVE
        TBool DiskSpaceBelowCriticalLevelL( RHttpDownload& aDownload, 
                                            TInt aSize ) const;
#else
        TBool DiskSpaceBelowCriticalLevelL( TInt aSize ) const;
#endif

        TBool DownloadConfirmationShown() const
            { return iDownloadConfirmationShown; };

    protected: // New functions 

        /**
        * Show confiration (with wait dialogs).
        * The confirmation may not be shown in some cases!
        * @param aDownload The actual download
        * @param aCbaResource Softkeys resource ID (eg. R_AVKON_SOFTKEYS_OK_CANCEL)
        * @param aForce Set it to ETrue if it must be shown in any case.
        * @return Confirmation accepted? True/False.
        */
        TBool ShowDownloadConfirmationL( RHttpDownload& aDownload, 
                                         TInt aCbaResource, 
                                         TBool aForce );
                                         
        /**
        * Show confiration (with wait dialogs).
        * @param aDownload The actual download
        * @param aCbaResource Softkeys resource ID (eg. R_AVKON_SOFTKEYS_OK_CANCEL)
        * @return Confirmation accepted? True/False.
        */
        TBool ShowDownloadConfirmationL( RHttpDownload& aDownload, 
                                         TInt aCbaResource );
                                         
        /**
        * Construct the handler application's name..
        * @param aAppUid UID of the application.
        * @param aApaLs APPARC session.
        * @return The allocated heap buffer.
        */
        HBufC* ConstructHandlerAppNameLC( const TUid& aAppUid, RApaLsSession& aApaLs );

        /**
        * Format list box string for Download confirmation.
        * @param aFirst First line
        * @param aSecond Second line of list item
        * @return The allocated heap buffer.
        */
        HBufC* FormatListBoxItemLC( const TDesC& aFirst, const TDesC& aSecond );

        /**
        * Handle download in EHttpDlInprogress state. It operates on iDownload.
        * @param aThisDeleted Indicator. Output parameter.
        */
        void HandleInProgressStateL( TBool& aThisDeleted );
        
        /**
        * Handle download in EHttpDlPaused state. It operates on iDownload.
        * @param aThisDeleted Indicator. Output parameter.
        */
        void HandlePausedStateL( TBool& aThisDeleted );
        
        /**
        * Handle download in EHttpDlCompleted state. It operates on iDownload.
        */
        void HandleCompletedStateL();
        
        /**
        * Handle download in EHttpDlFailed state. It operates on iDownload.
        */
        void HandleFailedStateL();
        
        /**
        * Show info note and delete the download..
        */
        void HandleUnrecoverableFailL();
        
    protected: // Functions from CActive

	    virtual void DoCancel();
	    virtual void RunL();
	    virtual TInt RunError( TInt aError );

    protected: // Data

        CUserInteractionsUtils& iUiUtils; ///< Reference to the UI utils.
        /**
        * Temp storage. Should be long enough to hold R_DMUL_DOWNLOADCONF_UNKNOWN 
        * localized string or the content size (max. 7 characters).
        */
        TBuf<KMaxDownloadSizeTextLength> iSizeInfoRes;

        /// Indicator if the download confirmation is being shown.
        TBool iDownloadConfirmationShown;
        
        /// Popup. Not owned.
        CAknPopupList* iPopupList;
        TBool* iDownloadConfirmationDeletedPtr;
        TBool* iDeletedPtr;
    };

#endif /* USERINTERACTIONSEVENTHANDLER_H */
