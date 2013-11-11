/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handle scrollbar and tab events
*
*/



#ifndef BRCTLBCTESTOBSERVER_H
#define BRCTLBCTESTOBSERVER_H

//  INCLUDES
#include <BrCtlLayoutObserver.h>
#include <BrCtlSoftkeysObserver.h>
#include <BrCtlSpecialLoadObserver.h>
#include <BrCtlDownloadObserver.h>
#include <BrCtlInterface.h>
#include <DocumentHandler.h>    
#include <flogger.h>

class CWebViewApiTestObserver : 
    public CBase, 
    public MBrCtlLayoutObserver,
    public MBrCtlSoftkeysObserver, 
    public MBrCtlSpecialLoadObserver,
    public MBrCtlLoadEventObserver, 
    public MBrCtlLinkResolver,
    public MBrCtlDownloadObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWebViewApiTestObserver* NewL();

        /**
        * Destructor.
        */
        ~CWebViewApiTestObserver();

      
        /**
        * Update the position of vertical scrollbar.
        * @since 2.8
        * @param aDocumentHeight The total height of the markup page
        * @param aDisplayHeight The height of the display
        * @param aDisplayPosY The current Y position
        * @return void
        */
        void UpdateBrowserVScrollBarL(TInt /*aDocumentHeight*/, 
                                              TInt /*aDisplayHeight*/,
                                              TInt /*aDisplayPosY*/ ) ;

        /**
        * Update the position of horizontal scrollbar.
        * @since 2.8
        * @param aDocumentWidth The total width of the markup page
        * @param aDisplayWidth The width of the display
        * @param aDisplayPosX The current X position
        * @return void
        */
        void UpdateBrowserHScrollBarL(TInt /*aDocumentWidth*/, 
                                              TInt /*aDisplayWidth*/,
                                              TInt /*aDisplayPosX*/ ) ;

        /**
        * Inform the layout of the page: right to left or left to right. Useful when the application draws the scrollbar itself.
        * @since 2.8
        * @param aNewLayout RTL or LTR
        * @return void
        */
        void NotifyLayoutChange( TBrCtlLayout /*aNewLayout*/ ) ;
       
        /**
        * Update the title of the page in history view
        * @since 3.0
        * @param aTitle Title of the page
        * @return void
        */
        void UpdateTitleL( const TDesC& aTitle );

        /**
        * Browser Control requests to update a softkey
        * @since 2.8
        * @param aKeySoftkey Update the left softkey or the right softkey
        * @param aLabel The label associated with the softkey update
        * @param aCommandId The command to use if the softkey is selected by the user
        * @param aBrCtlSoftkeyChangeReason The reason for the softkey change
        * @return void
        */
        void UpdateSoftkeyL(TBrCtlKeySoftkey /*aKeySoftkey*/,
                                    const TDesC& /*aLabel*/,
                                    TUint32 /*aCommandId*/,
                                    TBrCtlSoftkeyChangeReason /*aBrCtlSoftkeyChangeReason*/) ;

        /**
        * Request to create a network connection.
        * @since 2.8
        * @param aConnectionPtr A pointer to the new connection. If NULL, the proxy filter will automatically create a network connection
        * @param aSockSvrHandle A handle to the socket server.
        * @param aNewConn A flag if a new connection was created. If the connection is not new, proxy filter optimization will not read the proxy again from CommsBd
        * @param aBearerType The bearer type of the new connection
        * @return void
        */
        void NetworkConnectionNeededL(TInt* /*aConnectionPtr*/,
                                              TInt* /*aSockSvrHandle*/,
                                              TBool* /*aNewConn*/,
                                              TApBearerType* /*aBearerType*/) ;

        /**
        * Request the host applicaion to handle non-http request.
        * @since 2.8
        * @param aUrl The non-http(s) or file URL
        * @param aParamList Parameters to pass to the host application. Contain referer header. It could be NULL
        * @return ETrue is handled by the host application. EFlase if not
        */
        TBool HandleRequestL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray);

        /**
        * Request the host applicaion to handle downloads
        * @since 2.8
        * @param aTypeArray array of download parameter types
        * @param aDesArray array of values associated with the types in the type array
        * @return ETrue is handled by the host application. EFlase if not
        */
        TBool HandleDownloadL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray);

        /**
        * A load events notification
        * @since 2.8
        * @param aLoadEvent The load event   
        * @param aSize Size depends on the event
        * @param aTransactionId The transaction id of the transaction that had this event
        * @return void
        */
        void HandleBrowserLoadEventL(TBrCtlDefs::TBrCtlLoadEvent aLoadEvent, TUint aSize, TUint16 aTransactionId) ;

    	/**
    	* Browser plug-in calls this method when embedded link is found. Used with ECapabilityClientResolveEmbeddedURL
        * @since 2.8
    	* @param aEmbeddedUrl The url of the embedded content
    	* @param aCurrentUrl The url of the current page
    	* @param aLoadContentType The expected type of the embedded content
    	* @param aEmbeddedLinkContent a callback interface to return the embedded content
    	* @return EFalse, browser will resolve the link
    	*/
        TBool ResolveEmbeddedLinkL(const TDesC& aEmbeddedUrl,
    		                               const TDesC& aCurrentUrl,
        	                               TBrCtlLoadContentType aLoadContentType, 
        	                               MBrCtlLinkContent& aEmbeddedLinkContent) ; 
		
    	/**
    	* Browser plug-in calls this method when the user requests to load content via selecting a link, or any other way. Used with ECapabilityClientNotifyURL 
        * @since 2.8
    	* @param aUrl The requested url
    	* @param aCurrentUrl The url of the current page
    	* @param aBrCtlLinkContent a callback interface to return the embedded content
    	* @return EFalse, browser will resolve the link
    	*/
        TBool ResolveLinkL(const TDesC& aUrl, const TDesC& aCurrentUrl,
                                   MBrCtlLinkContent& aBrCtlLinkContent) ;

		/**
		* Cancel all outstanding resolving operations
        * @since 2.8
    	* @return void
    	*/
        void CancelAll() ;

        /**
        * Inform the host application that a new download has started using the Download Manager
        * @since 3.0
        * @param aTransactionID The ID of the transaction, it is unique as long as the transaction is on-going
        * @param aFileName The name of file in which the content is stored/
        * @param aContentType The content type of the downloaded content
        * @param aUrl The Url of the request to be done in the new window
        * @return Return ETrue if the file is handled progressively, EFalse otherwise
        */
        TBool NewDownloadL(TUint aTransactionID,
                           const TDesC& aFileName,
                           const TDesC& aContentType,
                           const TDesC& aUrl);


        /**
        * Inform the host application that a new download has started using the Download Manager
        * @since 3.0
        * @param aTransactionID The ID of the transaction, it is unique as long as the transaction is on-going
        * @param aFileName The name of file in which the content is stored/
        * @param aContentType The content type of the downloaded content
        * @param aUrl The Url of the request to be done in the new window
        * @return Return ETrue if the file is handled progressively, EFalse otherwise
        */
        void ResumeDownloadL(TUint aTransactionID,
                                   TUint aLength,
                                   const TDesC& aFileName,
                                   const TDesC& aContentType,
                                   const TDesC& aUrl);

        /**
        * Inform the host application of a progress event related to a download
        * @since 3.0
        * @param aTransactionID The ID of the transaction, it is unique as long as the transaction is on-going
        * @param aDownloadEvent The event to be reported
        * @param aValue The value depends on the event
        * @return void
        */
        void HandleDownloadEventL(TUint aTransactionID, 
                                  TBrCtlDownloadEvent aDownloadEvent,
                                  TUint aValue);             

    private:
	    /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

	    /**
        * Find if the scheme if file://
        */
        TBool IsFileScheme(const TDesC& aFileName);

	    /**
        * Read the file.
        */
        HBufC8* ReadFileLC(const TDesC& aFileName);

	    /**
        * Rcognize the mime type.
        */
        HBufC* RecognizeLC(const TDesC& aFileName, const TDesC8& aData);

	    /**
        * Translate the file name from a URL to a valid file name in the system..
        */
        TBool GetFileNameL(const TDesC& aFileName);

    private:   // data

        // The loaded file's name. This is different from the URL. It does not contain the scheme.
        HBufC* iFileName;
        CDocumentHandler* iHandler;
    };

#endif      // BRCTLBCTESTOBSERVER_H
            
// End of File

