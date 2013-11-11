/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef __HTTPDOWNLOAD_H__
#define __HTTPDOWNLOAD_H__

#include <BrCtlSpecialLoadObserver.h>
#include "HttpUiCallbacks.h"
#include "HttpCallbacks.h"
#include <DownloadMgrClient.h>
#include <CDownloadMgrUiLibRegistry.h>
#include <BrCtlDownloadObserver.h>

class HttpDlConnection;
class RHTTPTransaction;
class CHttpLoaderEventToUiListener;
class HttpSessionManager;

class HttpDownload : public MHttpDownloadMgrObserver, public MHttpDownloadMgrNextUriObserver
{
public:
    HttpDownload(HttpSessionManager* sessionManager);
    virtual ~HttpDownload();
    void continueDownloadL(RHTTPTransaction* connTransaction, HttpDlConnection* dlConnection);

    /**
    * Create a connection in DMgr with the given name.
    */
    void connect( TName& aName );

    /**
    * Close Download Manager connection
    */
    void disconnect();

    /* This method displays download list
    */
    void ShowDownloadsL();

   /*
    * This method calls to download manager in order to display query "Cancel
    * outstanding downloads?" and cancel if it requested by user
    * return ETrue if it is OK to exit; EFalse - if it is not OK to exit
    */
    bool okToExit();

   /*
    * This method calls to Download Manager to prepare to exit.
    */
    void prepareToExit( long appUid, long viewId, long customMessageId );

   /*
    * This method calls to Download Manager to HandleDownloadCommandL.
    */
    void HandleDownloadCommandL(unsigned int transId, TBrCtlDefs::TBrCtlDownloadCmd command);

    /**
    * Enables cookie usage in DMgr.
    * enable true if cookies are enabled
    */
    void enableCookies(int enable );

    /**
    */
    void updateDownloadsOpenEnabled();

    // from MHttpDownloadMgrObserver
    void HandleDMgrEventL( RHttpDownload& download, THttpDownloadEvent dvent );

    //from MHttpDownloadMgrNextUriObserver
    void NextUriL( RHttpDownload& download, const TDesC8& uri );

   /*
    * Returns a number of outstanding downloads which not handled by
    * the media
    */
    unsigned int numOfDownloads();

private:
   /**
    * Initialize Download Manager
    */
    void initDownloadMgrL();

   /**
    * Initialize Download Manager Ui Lib
    */  
    void initDownloadMgrUiLibL();

    /**
    * Finds connection associated with the download id
    * dlId unique download id
    */
    HttpDlConnection* findDownloadConnection(long dlId, int& position);

    /**
    * Handle the EHttpDlCompleted DM event.
    * download The download with which the event occured.
    */
    void handleDMgrCompletedEventL(RHttpDownload& download, TInt event );

    void handleDMgrEventDlCompletedL(RHttpDownload& download);
    void handleDMgrEventDlFailedL(RHttpDownload& download);
    void handleDMgrEventDlDeletingL(RHttpDownload& download);
    void handleDMgrEventDlPausableL(RHttpDownload& download);
    void handleDMgrEventDlNonPausableL(RHttpDownload& download);
    void handleDMgrEventDlPausedL(RHttpDownload& download);
    void handleDMgrEventProgConnectionNeededL(RHttpDownload& download);
    void handleDMgrEventProgResponseBodyReceivedL(RHttpDownload& download);
    void handleDMgrEventProgResponseHeaderReceivedL(RHttpDownload& download);

    void CreateDownloadArraysLC(RHttpDownload& download, RArray<TUint>*& typeArray, CDesCArrayFlat*& desArray );

private:
    HttpSessionManager* m_sessionManager;
    bool m_dMgrReady;
    int m_launchViewer;
    Vector<HttpDlConnection*> m_pendingHttpDownloadRequests;
    RHttpDownloadMgr m_downloadMgr;
    CDownloadMgrUiLibRegistry* m_dMgrUiReg;
    int m_suppressDloadConfirmation;
	MBrCtlDownloadObserver* m_downloadObserver;
};
#endif // __HTTPDOWNLOAD_H__
// end of file
