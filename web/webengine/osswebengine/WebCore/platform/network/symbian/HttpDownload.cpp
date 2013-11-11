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

#include "HttpDownload.h"
#include <CDownloadMgrUiUserInteractions.h>
#include <CDownloadMgrUiDownloadsList.h>
#include "HttpDlConnection.h"
#include "HttpDefs.h"
#include "HttpSessionManager.h"
#include "BrCtl.h"
#include "StaticObjectsContainer.h"

// CONSTANTS
const unsigned long KMinDataSizeToSend = (1024*5);
#define PREPARE_DOWNLOAD_MANAGER  if(!m_dMgrReady) { initDownloadMgrL(); m_dMgrReady = true; }

using namespace WebCore;

HttpDownload::HttpDownload(HttpSessionManager* sessionManager)
{
    m_sessionManager = sessionManager;
    m_dMgrReady = false;
    m_dMgrUiReg = NULL;
	m_downloadObserver = StaticObjectsContainer::instance()->brctl()->brCtlDownloadObserver();
    TRAP_IGNORE(PREPARE_DOWNLOAD_MANAGER)
}

HttpDownload::~HttpDownload()
{
    delete m_dMgrUiReg;
    if( m_dMgrReady ){
        m_downloadMgr.Close();
        m_dMgrReady = false;
        }
}

void HttpDownload::initDownloadMgrL()
{
    if( m_dMgrReady ) return;
    // get capatabilities
    unsigned int capability = StaticObjectsContainer::instance()->capabilities();
    // initialize m_suppressDloadConfirmation
    if (capability & TBrCtlDefs::ECapabilityUseDlMgr){
        m_suppressDloadConfirmation = true;
        if (capability & TBrCtlDefs::ECapabilityConfirmDownloads){
            m_suppressDloadConfirmation = false;
        }
    }
    // initialize m_launchViewer
    m_launchViewer = false;
    if (capability & TBrCtlDefs::ECapabilityLaunchViewer){
        m_launchViewer = true;
    }
    int master = !m_sessionManager->m_embedded;
    RProcess myProcess;
    
    TRAPD( errMngr, m_downloadMgr.ConnectL( myProcess.Identity(), *this, master ));
    if ( errMngr == KErrAlreadyExists && master ) { 
        m_downloadMgr.ConnectL( myProcess.Identity(), *this, EFalse );
    }
    
    if( !master ){
        // Browser is embedded. Downloads must be paused when the user closes
        // the embedded Browser (that is always started from the Launcher), and
        // CDownloadMgrUiUserInteractions::OkToExitL() must not be called.
        // This behaviour can be indicated to DMgr before hand:
        User::LeaveIfError( m_downloadMgr.SetIntAttribute( EDlMgrExitAction, EExitPause ) );
    }
    // Cookie usage setting
    m_downloadMgr.SetBoolAttribute( EDlMgrEnableCookies, m_sessionManager->m_cookiesEnabled );
    // Auto-connection is disabled
    m_downloadMgr.SetBoolAttribute( EDlMgrAutoConnect, EFalse );

    #ifdef __BROWSER_SDK
    m_downloadMgr.SetDefaultIntAttribute( EDlAttrAction, m_launchViewer ? ELaunch : EDoNothing );
    #else
    if ( !m_launchViewer ){
        m_downloadMgr.SetDefaultIntAttribute( EDlAttrAction, EDoNothing );
    }
    else if ( m_sessionManager->m_autoOpenDownloads) {
        m_downloadMgr.SetDefaultIntAttribute( EDlAttrAction, EMove + EPdLaunch);
    }
    else {
        m_downloadMgr.SetDefaultIntAttribute( EDlAttrAction, EMove);         
    }
    #endif
    m_downloadMgr.SetNextUriObserver((MHttpDownloadMgrNextUriObserver*) this );
    initDownloadMgrUiLibL();

    // go through the download list to define outstanding downloads.
    // If it exist send the ResumeDownloadL() to the host appl
    CDownloadMgrUiUserInteractions* dloadMgrUiUserInteractions = &m_dMgrUiReg->UserInteractions();
    TRAP_IGNORE (dloadMgrUiUserInteractions->SetBoolAttributeL( CDownloadMgrUiUserInteractions::EAttrSuppressDownloadConfirmation,
                                                                m_suppressDloadConfirmation ));
    RHttpDownload* dl = NULL;
    const CDownloadArray& dls = m_downloadMgr.CurrentDownloads();
    int downloadCnt = dls.Count();
    for( int i = 0; i < downloadCnt; ++i )
    {
        dl = dls.At(i); // current download
        if(dl == NULL){
            User::Leave(KErrGeneral);
        }
        // Download Manager Transaction ID
        long transID = 0;
        dl->GetIntAttribute(EDlAttrId, transID);

        // Download Manager total Length of the download
        long dlLength = 0;
        dl->GetIntAttribute(EDlAttrLength, dlLength);

        // local filename
        HBufC16* fileName = HBufC16::NewLC( KMaxPath );
        TPtr fileNameDes( fileName->Des() );
        dl->GetStringAttribute( EDlAttrDestFilename, fileNameDes );
        // received content type
        HBufC* contentType = HBufC::NewLC( KMaxContentTypeLength );
        TPtr contentTypeBuf( contentType->Des() );
        dl->GetStringAttribute( EDlAttrContentType, contentTypeBuf );

        //request url
        HBufC* url = HBufC::NewLC( KMaxUrlLength );
        TPtr urlBuf( url->Des() );
        dl->GetStringAttribute( EDlAttrCurrentUrl, urlBuf );

        // notify Observer of resuming download
        m_downloadObserver->ResumeDownloadL( transID, dlLength, fileNameDes,
                                                contentTypeBuf, urlBuf);
        CleanupStack::PopAndDestroy(3);
    }
    m_dMgrReady = true;
}

void HttpDownload::continueDownloadL(RHTTPTransaction* connTransaction, HttpDlConnection* dlConnection)
{
    // initialize download manager if it has not been initialized
    PREPARE_DOWNLOAD_MANAGER
    // Query HTTP transaction
    int transaction( REINTERPRET_CAST( int, connTransaction ) );
    int isNewDl = 1;
    long id;
    RHttpDownload& dl = m_downloadMgr.CreateClientSideDownloadL(transaction, isNewDl );
    dl.GetIntAttribute( EDlAttrId,id );
    dlConnection->setDownloadId( id );
    User::LeaveIfError( dl.Start() );    
}

void HttpDownload::initDownloadMgrUiLibL()
{
    m_dMgrUiReg = CDownloadMgrUiLibRegistry::NewL( m_downloadMgr );
    m_dMgrUiReg->RegisterDownloadsListL();
    m_dMgrUiReg->RegisterUserInteractionsL();
}

//HandleDMgrEventL methodes
void HttpDownload::handleDMgrEventDlCompletedL(RHttpDownload& download)
{
    // Download Manager User Data
    long userData;
    // Download Manager Transaction ID
    long transID( 0 );
    download.GetIntAttribute( EDlAttrId, transID );

    // Download Manager total Length of the download
    long dlLength( 0 );
    download.GetIntAttribute( EDlAttrLength, dlLength );

    download.GetIntAttribute(EDlAttrUserData, userData);
    // check if the NewDownloadL() callback has been sent,
    // if not send it before EDownloadEventCompleted will be send
    if (!userData){
        // local filename
        HBufC16* fileName = HBufC16::NewLC( KMaxPath );
        TPtr fileNameDes( fileName->Des() );
        download.GetStringAttribute( EDlAttrDestFilename, fileNameDes );

        // received content type
        HBufC* contentType = HBufC::NewLC( KMaxContentTypeLength );
        TPtr contentTypeBuf( contentType->Des() );
        download.GetStringAttribute( EDlAttrContentType, contentTypeBuf );

        //request url
        HBufC* url = HBufC::NewLC( KMaxUrlLength );
        TPtr urlBuf( url->Des() );
        download.GetStringAttribute( EDlAttrCurrentUrl, urlBuf );
        // Return ETrue if the file is handled progressively, false otherwise
        m_downloadObserver->NewDownloadL( transID, fileNameDes, contentTypeBuf, urlBuf );
        // notify Observer of started download
        m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventStarted, dlLength);
        CleanupStack::PopAndDestroy(3);

        // Download Manager User Data
        userData = 1;
        download.SetIntAttribute(EDlAttrUserData, userData);
        }
    // notify Observer of completed download
    m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventCompleted, dlLength );
    handleDMgrCompletedEventL( download, EHttpDlCompleted );
}

void HttpDownload::handleDMgrEventDlFailedL(RHttpDownload& download)
{
    // Download Manager Transaction ID
    long transID( 0 );
    download.GetIntAttribute( EDlAttrId, transID );

    // notify Observer of failed download
    m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventError, 0 );
    if ( numOfDownloads() == 0 ){
        m_downloadMgr.SetIntAttribute( EDlMgrHasActiveDownloads, 0 );
        }
}

void HttpDownload::handleDMgrEventDlDeletingL(RHttpDownload& download)
{
    // Download Manager Transaction ID
    long transID( 0 );
    download.GetIntAttribute(EDlAttrId, transID);

    // Download Manager total Length of the download
    long dlLength( 0 );
    download.GetIntAttribute(EDlAttrLength, dlLength);

    // notify Observer of paused download
    m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventCanceled, dlLength);
    unsigned int noActiveDownloads = numOfDownloads() - 1;
    if ( noActiveDownloads == 0 ){
        m_downloadMgr.SetIntAttribute( EDlMgrHasActiveDownloads, 0 );
    }
}

void HttpDownload::handleDMgrEventDlPausableL(RHttpDownload& download)
{
    // Download Manager Transaction ID
    long transID( 0 );
    download.GetIntAttribute(EDlAttrId, transID);
    // Download Manager total Length of the download
    long dlLength( 0 );
    download.GetIntAttribute(EDlAttrLength, dlLength);
    // notify Observer of paused download
    m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventPausable, ETrue);
}

void HttpDownload::handleDMgrEventDlNonPausableL(RHttpDownload& download)
{
    // Download Manager Transaction ID
    long transID( 0 );
    download.GetIntAttribute(EDlAttrId, transID);
    // Download Manager total Length of the download
    long dlLength( 0 );
    download.GetIntAttribute(EDlAttrLength, dlLength);
    // notify Observer of paused download
    m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventPausable, EFalse);
}

void HttpDownload::handleDMgrEventDlPausedL(RHttpDownload& download)
{
    // Download Manager Transaction ID
    long transID( 0 );
    download.GetIntAttribute(EDlAttrId, transID);
    // Download Manager total Length of the download
    long dlLength( 0 );
    download.GetIntAttribute(EDlAttrLength, dlLength);
    // notify Observer of paused download
    m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventPaused, dlLength);
}

void HttpDownload::handleDMgrEventProgConnectionNeededL(RHttpDownload& download)
{
    int connected( 0 );
    if( !m_downloadMgr.GetBoolAttribute( EDlMgrConnected,connected ) ){
        if( !connected ){
            // Connection is needed in DMgr.
            // Now call ConnectionManager to ask if user wants to create a new one.
            // After that pass the newly created connection's name to DMgr, that
            // will automatically attaches to the connection.
            int connectionPtr( 0 );
            int sockSvrHandle;
            int newConn;
            int err;
            TApBearerType bearerType;
            err = m_sessionManager->m_uiCallbacks.CreateConnection( &connectionPtr, &sockSvrHandle, &newConn, &bearerType );
            if( err == KErrNone ){
                TName name;
                RConnection* connPtr = REINTERPRET_CAST( RConnection*, connectionPtr );
                connPtr->Name( name );
                connect( name );
            }
            else{
                // User doesn't want to make a connection, or connection failed.
                download.Pause();
            }
        }
    }
}

void HttpDownload::handleDMgrEventProgResponseBodyReceivedL(RHttpDownload& download)
{
    // Download Manager Transaction ID
    long transID( 0 );
    long userData( 0 );
    download.GetIntAttribute( EDlAttrId, transID );
    download.GetIntAttribute( EDlAttrUserData, userData );
    // Download Manager already downloaded content size
    long downloadedSize( 0 );
    download.GetIntAttribute( EDlAttrDownloadedSize, downloadedSize );
    if( userData ) {// already sent out EDownloadEventStarted event
        // notify Observer of download in progress
        m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventProgress, downloadedSize );
    }
    else {// did not sent EDownloadEventStarted yet and NewDownloadL.
        // don't call NewDownloadL() until more than 5KB of data available
        if (downloadedSize > KMinDataSizeToSend) {
            // Download Manager total Length of the download
            long dlLength( 0 );
            download.GetIntAttribute( EDlAttrLength, dlLength );
            // local filename
            HBufC16* fileName = HBufC16::NewLC( KMaxPath );
            TPtr fileNameDes( fileName->Des() );
            download.GetStringAttribute( EDlAttrDestFilename, fileNameDes );
            // received content type
            HBufC* contentType = HBufC::NewLC( KMaxContentTypeLength );
            TPtr contentTypeBuf( contentType->Des() );
            download.GetStringAttribute( EDlAttrContentType, contentTypeBuf );
            //request url
            HBufC* url = HBufC::NewLC( KMaxUrlLength );
            TPtr urlBuf( url->Des() );
            download.GetStringAttribute( EDlAttrCurrentUrl, urlBuf );
            // Return ETrue if the file is handled progressively, EFalse otherwise
            int progDl = m_downloadObserver->NewDownloadL( transID, fileNameDes, contentTypeBuf, urlBuf );
            int isProgDl = 0;
            download.GetBoolAttribute( EDlAttrProgressive, isProgDl );
            if ( !isProgDl ){
                download.SetBoolAttribute( EDlAttrProgressive, (long)progDl );
            }   
            // notify Observer of started download
            m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventStarted, dlLength );
            CleanupStack::PopAndDestroy( 3 );
            // Download Manager User Data
            userData = 1;
            download.SetIntAttribute( EDlAttrUserData, userData );
        }
    }
}

void HttpDownload::handleDMgrEventProgResponseHeaderReceivedL(RHttpDownload& download)
{
    long value(0);
    download.GetIntAttribute(EDlAttrStatusCode, value);

    if (value == EPartialContent){
        // Download Manager Transaction ID
        long transID( 0 );
        download.GetIntAttribute(EDlAttrId, transID);

        // Download Manager total Length of the download
        long dlLength( 0 );
        download.GetIntAttribute(EDlAttrLength, dlLength);

        // notify Observer of resuming download
        m_downloadObserver->HandleDownloadEventL( transID, EDownloadEventResumed, dlLength);
    }
    if ( numOfDownloads() ){
        m_downloadMgr.SetIntAttribute( EDlMgrHasActiveDownloads, 1 );
    }
}

void HttpDownload::HandleDMgrEventL(RHttpDownload& download, THttpDownloadEvent event )
{
    PREPARE_DOWNLOAD_MANAGER
    // Download Manager User Data
    long userData;
    switch (event.iDownloadState)
        {
        case EHttpDlCompleted:
            {
            handleDMgrEventDlCompletedL(download);
            break;
            }

        case EHttpDlFailed:
            {
            handleDMgrEventDlFailedL(download);
            break;
            }

        case EHttpDlDeleting:
            {
            handleDMgrEventDlDeletingL(download);
            break;
            }

        case EHttpDlPausable:
            {
            handleDMgrEventDlPausableL(download);
            break;
            }

        case EHttpDlNonPausable:
            {
            handleDMgrEventDlNonPausableL(download);
            break;
            }

        case EHttpDlPaused:
            {
            handleDMgrEventDlPausedL(download);
            break;
            }

        case EHttpDlInprogress:
            {
            switch (event.iProgressState )
                {
                case EHttpProgConnectionNeeded:
                    {
                    handleDMgrEventProgConnectionNeededL(download);
                    break;
                    }
                case EHttpProgResponseBodyReceived:
                    {
                    handleDMgrEventProgResponseBodyReceivedL(download);
                    break;
                    }
               // when download is resumed send ResumeDownloadL() to the host
                case EHttpProgResponseHeaderReceived:
                    {
                    handleDMgrEventProgResponseHeaderReceivedL(download);
                    break;
                    }
                default:
                    {
                    break;
                    }
                } //switch (aEvent.iProgressState)
            break;
            }
        default:
            {
            break;
            }
        } // switch (aEvent.iDownloadState)
}

unsigned HttpDownload::numOfDownloads()
{
    // this feature is not supported. should be fixed on
    // the download manager side.
    // waste of memory to init download manager just for
    // the sake of number of downloads.
    if( !m_dMgrReady ){
        return 0;
    }
    long noMediaDownloads( 0 );
    m_downloadMgr.GetIntAttribute( EDlMgrNoMediaDls, noMediaDownloads );
    // Return value is ignored as it is a non leaving function!
    // If it returns an error, then 'noMediaDownloads' is considered 0.
    return( m_dMgrUiReg->DownloadCount() - noMediaDownloads );
}

HttpDlConnection* HttpDownload::findDownloadConnection (long dlId, int& position)
{
    // check if this connection is in the list
    HttpDlConnection* dlConnection = NULL;
    int index = 0;
    int size = m_sessionManager->m_pendingHttpDownloadRequests.size();
    while (index < size){
        dlConnection = m_sessionManager->m_pendingHttpDownloadRequests[index];
        if (dlConnection->downloadId() == dlId){
            position = index;
            break;
        }
        index++;
    }
    return dlConnection;
}

void HttpDownload::CreateDownloadArraysLC(RHttpDownload& download, RArray<TUint>*& typeArray, CDesCArrayFlat*& desArray )
{
    PREPARE_DOWNLOAD_MANAGER
    // these arrays are to be freed by the download manager
    typeArray = new (ELeave) RArray<TUint>(4);
    CleanupStack::PushL( typeArray );
    desArray = new (ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL( desArray );
    // request url
    HBufC* url = HBufC::NewLC( KMaxUrlLength );
    TPtr urlBuf( url->Des() );
    download.GetStringAttribute( EDlAttrCurrentUrl, urlBuf );

    typeArray->AppendL( EParamRequestUrl );
    desArray->AppendL( urlBuf );
    CleanupStack::PopAndDestroy( url );
    // total content length
    long contentLength;
    download.GetIntAttribute( EDlAttrLength, contentLength );
    HBufC* lengthBuf = HBufC::NewLC( 10 );  // it's enough for a TInt32
    lengthBuf->Des().Format( _L("%d"), contentLength );

    typeArray->AppendL( EParamTotalContentLength );
    desArray->AppendL( *lengthBuf );
    CleanupStack::PopAndDestroy( lengthBuf );
    // received content type
    HBufC* contentType = HBufC::NewLC( KMaxContentTypeLength );
    TPtr contentTypeBuf( contentType->Des() );
    download.GetStringAttribute( EDlAttrContentType, contentTypeBuf );
    typeArray->AppendL( EParamReceivedContentType );
    desArray->AppendL( contentTypeBuf );
    CleanupStack::PopAndDestroy( contentType );
    // referer header
    HBufC16* referer = HBufC16::NewLC( KMaxUrlLength );
    TPtr refererDes( referer->Des() );
    download.GetStringAttribute( EDlAttrRequestReferer, refererDes );
    typeArray->AppendL( EParamRefererHeader );
    desArray->AppendL( refererDes );
    CleanupStack::PopAndDestroy( referer );
    // charset
    HBufC16* charSet = HBufC16::NewLC( KMaxDefAttrLength );
    TPtr charsetDes( charSet->Des() );
    download.GetStringAttribute( EDlAttrRequestReferer, charsetDes );
    typeArray->AppendL( EParamCharset );
    desArray->AppendL( charsetDes );
    CleanupStack::PopAndDestroy( charSet );
    // local filename
    HBufC16* fileName = HBufC16::NewLC( KMaxPath );
    TPtr fileNameDes( fileName->Des() );
    download.GetStringAttribute( EDlAttrDestFilename, fileNameDes );
    typeArray->AppendL( EParamLocalFileName );
    desArray->AppendL( fileNameDes );
    CleanupStack::PopAndDestroy( fileName );
    // Download Manager Transaction ID
    long transID( 0 );
    download.GetIntAttribute(EDlAttrId, transID);
    HBufC* tranIdBuf = HBufC::NewLC( 10 );  // it's enough for a TInt32
    tranIdBuf->Des().Format( _L("%d"), transID);
    typeArray->AppendL( EParamTransactionId );
    desArray->AppendL( *tranIdBuf );
    CleanupStack::PopAndDestroy( tranIdBuf );
}

void HttpDownload::handleDMgrCompletedEventL(RHttpDownload& download, TInt event)
{
    PREPARE_DOWNLOAD_MANAGER

    if( !m_launchViewer && event == EHttpDlCompleted ){
        // create arrays
        RArray<TUint>* typeArray = NULL;
        CDesCArrayFlat* desArray = NULL;
        CreateDownloadArraysLC( download, typeArray, desArray );
        // use download manager
        MBrCtlSpecialLoadObserver* loadObserver = StaticObjectsContainer::instance()->brctl()->brCtlSpecialLoadObserver();
        bool downloadHandled = (loadObserver && loadObserver->HandleDownloadL(typeArray, desArray));
        if ( !downloadHandled ){
            m_dMgrUiReg->UserInteractions().HandleDownloadL(download);
        }
        // cleanup arrays
        if( desArray ){
            desArray->Reset();
            CleanupStack::PopAndDestroy(); // desArray
        }
        if( typeArray ){
            typeArray->Close();
            CleanupStack::PopAndDestroy(); // typeArray
        }
    }

    long dlId( KErrNotFound );
    download.GetIntAttribute( EDlAttrId, dlId );
    int position = 0;
    if( dlId != KErrNotFound ){
        HttpDlConnection* dlConnection = findDownloadConnection(dlId, position);
		if (dlConnection) {
			// check if we need to keep pipelining off
			long noActiveDownloads = 0;
			m_downloadMgr.GetIntAttribute( EDlMgrNumInprogressDownloads, noActiveDownloads );
			if( noActiveDownloads == 0 ){
				// set pipelining
				m_sessionManager->m_httpPipelining = true;
			}
			// remove connection from the download queue
            m_sessionManager->removeDlRequest(dlConnection);
			delete dlConnection;
			dlConnection = NULL;
		}
    }
    if ( numOfDownloads() == 0 ){
        m_downloadMgr.SetIntAttribute( EDlMgrHasActiveDownloads, 0 );
    }
}

void HttpDownload::connect(TName& name )
{
    TRAP_IGNORE(PREPARE_DOWNLOAD_MANAGER)
    int err( KErrNone );
    if( !name.Length() ){
        int conn( 0 );
        int dummy;
        int newConn;
        int err;
        TApBearerType bearerType;
        err = m_sessionManager->m_uiCallbacks.CreateConnection( &conn, &dummy, &newConn, &bearerType );
        if( err == KErrNone ){
            RConnection* connPtr = REINTERPRET_CAST( RConnection*, conn );
            connPtr->Name( name );
        }
    }
    if( err == KErrNone ){
        m_downloadMgr.SetStringAttribute( EDlMgrConnectionName, name );
    }
}

void HttpDownload::disconnect()
{
    if( !m_dMgrReady ) return;
    m_downloadMgr.Disconnect();
}

void HttpDownload::ShowDownloadsL()
{
    // this feature is not supported. should be fixed on
    // the download manager side.
    // waste of memory to init download manager just for
    // the sake of number of downloads.
    if( !m_dMgrReady ){
        return;
    }
    CDownloadMgrUiDownloadsList& dloadlist = m_dMgrUiReg->DownloadsList();
    if( !dloadlist.IsVisible() ){
        dloadlist.DisplayDownloadsListL();
    }
}

bool HttpDownload::okToExit()
{
    // don't need to go futher if download manger is not up
    if( !m_dMgrReady ) return ETrue;
    bool dmgrexit( false );
    CDownloadMgrUiUserInteractions& dloadMgrUiUserInteractions = m_dMgrUiReg->UserInteractions();
    TRAP_IGNORE( dmgrexit = dloadMgrUiUserInteractions.OkToExitL() );
    return dmgrexit;
}

void HttpDownload::prepareToExit(const long appUid, const long viewId, const long customMessageId )
{
    // don't need to go futher if download manger is not up
    if( !m_dMgrReady ) return;
    m_dMgrUiReg->UserInteractions().PrepareToExit( appUid, viewId, customMessageId );
}

void HttpDownload::HandleDownloadCommandL(unsigned int transId, TBrCtlDefs::TBrCtlDownloadCmd command)
{
    PREPARE_DOWNLOAD_MANAGER

    RHttpDownload* dl = NULL;
    const CDownloadArray& dls = m_downloadMgr.CurrentDownloads();
    int downloadCnt = dls.Count();
    long dlTransID(0);
    if ( downloadCnt > 0){
        for( int i = 0; i < downloadCnt; ++i )
        {
        dl = dls.At(i); // current download
        if(dl == NULL){
            User::Leave(KErrGeneral);
        }
        dl->GetIntAttribute(EDlAttrId, dlTransID);
        if ( dlTransID == transId){
            break;
            }
        }
        switch (command)
        {
            case TBrCtlDefs::EDownloadCmdPause:
                {
                    dl->Pause();
                    break;
                }
            case TBrCtlDefs::EDownloadCmdResume:
                {
                    dl->Start();
                    break;
                }
            case TBrCtlDefs::EDownloadCmdCancel:
                {
                    dl->Delete();
                    break;
                }
            case TBrCtlDefs::EDownloadCmdMarkAsProgressive:
                {
                    dl->SetBoolAttribute(EDlAttrProgressive, ETrue);
                    break;
                }
            case TBrCtlDefs::EDownloadCmdMarkAsNotProgressive:
                {
                    dl->SetBoolAttribute(EDlAttrProgressive, EFalse);
                    break;
                }
            default:
                break;
        } //swith command
    }
}

void HttpDownload::enableCookies(int enable )
{
    //this macro can leave
    TRAP_IGNORE(PREPARE_DOWNLOAD_MANAGER)
    m_downloadMgr.SetBoolAttribute( EDlMgrEnableCookies, enable );
}

void HttpDownload::NextUriL( RHttpDownload& /*aDownload*/, const TDesC8& uri )
{
    m_sessionManager->m_uiCallbacks.NextUriL(uri, StaticObjectsContainer::instance()->brctl());
}

void HttpDownload::updateDownloadsOpenEnabled()
{
    TRAP_IGNORE(PREPARE_DOWNLOAD_MANAGER)
#ifdef __BROWSER_SDK
    m_downloadMgr.SetDefaultIntAttribute( EDlAttrAction, m_launchViewer ? ELaunch : EDoNothing );
#else
    if (m_sessionManager->m_autoOpenDownloads){
        m_downloadMgr.SetDefaultIntAttribute( EDlAttrAction, EMove + EPdLaunch);     
        }
    else {
        m_downloadMgr.SetDefaultIntAttribute( EDlAttrAction, EMove);         
        }
#endif
}
// end of file
