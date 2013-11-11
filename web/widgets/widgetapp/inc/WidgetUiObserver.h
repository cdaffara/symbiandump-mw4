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
* Description: 
*
*/


#ifndef WIDGETUIOBSERVER_H_
#define WIDGETUIOBSERVER_H_

//  INCLUDES
#include <BrCtlLayoutObserver.h>
#include <BrCtlSoftkeysObserver.h>
#include <BrCtlSpecialLoadObserver.h>
#include <BrCtlDownloadObserver.h>
#include <BrCtlWindowObserver.h>
#include <brctlinterface.h>
#include <DocumentHandler.h>
#include <flogger.h>

// FORWARD DECLARATIONS
class CActiveApDb;
class CWidgetUiWindow;

// CLASS DECLARATION

/**
*
*  @lib WidgetUI.exe
*  @since 3.1
*/
class CWidgetUiObserver : public CBase,
                       public MBrCtlLayoutObserver,     // not needed
                       public MBrCtlSoftkeysObserver,   // needed
                       public MBrCtlLoadEventObserver,  // needed
                       public MBrCtlLinkResolver,       // needed
                       public MBrCtlDownloadObserver,   // not needed
                       public MBrCtlWindowObserver      // needed
    {

    public:  // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        * @since 3.1
        * @param none
        * @return CWidgetUiObserver*
        */
        static CWidgetUiObserver* NewL( CWidgetUiWindow& aWindow );

        /**
        * ~CWidgetUiObserver
        * Destructor
        * @since 3.1
        * @param none
        * @return none
        */
        ~CWidgetUiObserver();


        /**
        * UpdateBrowserVScrollBarL
        * Update the position of vertical scrollbar.
        * @since 3.1
        * @param aDocumentHeight The total height of the markup page
        * @param aDisplayHeight The height of the display
        * @param aDisplayPosY The current Y position
        * @return void
        */
        void UpdateBrowserVScrollBarL( TInt aDocumentHeight,
                                       TInt aDisplayHeight,
                                       TInt aDisplayPosY );

        /**
        * UpdateBrowserHScrollBarL
        * Update the position of horizontal scrollbar.
        * @since 3.1
        * @param aDocumentWidth The total width of the markup page
        * @param aDisplayWidth The width of the display
        * @param aDisplayPosX The current X position
        * @return void
        */
        void UpdateBrowserHScrollBarL( TInt aDocumentWidth,
                                       TInt aDisplayWidth,
                                       TInt aDisplayPosX );

        /**
        * NotifyLayoutChange
        * Inform the layout of the page: right to left or left to right. Useful when the application draws the scrollbar itself.
        * @since 3.1
        * @param aNewLayout RTL or LTR
        * @return void
        */
        void NotifyLayoutChange( TBrCtlLayout aNewLayout );

        /**
        * UpdateTitleL
        * Update the title of the page in history view
        * @since 3.1
        * @param aTitle Title of the page
        * @return void
        */
        void UpdateTitleL( const TDesC& aTitle );

        /**
        * UpdateSoftkeyL
        * WidgetUi Control requests to update a softkey
        * @since 3.1
        * @param aKeySoftkey Update the left softkey or the right softkey
        * @param aLabel The label associated with the softkey update
        * @param aCommandId The command to use if the softkey is selected by the user
        * @param aBrCtlSoftkeyChangeReason The reason for the softkey change
        * @return void
        */
        void UpdateSoftkeyL( TBrCtlKeySoftkey aKeySoftkey,
                             const TDesC& aLabel,
                             TUint32 aCommandId,
                             TBrCtlSoftkeyChangeReason aBrCtlSoftkeyChangeReason);

        /**
        * HandleBrowserLoadEventL
        * A load events notification
        * @since 3.1
        * @param aLoadEvent The load event
        * @param aSize Size depends on the event
        * @param aTransactionId The transaction id of the transaction that had this event
        * @return void
        */
        void HandleBrowserLoadEventL( TBrCtlDefs::TBrCtlLoadEvent aLoadEvent,
                                      TUint aSize, TUint16 aTransactionId );

        /**
        * ResolveEmbeddedLinkL
        * WidgetUi plug-in calls this method when embedded link is found. Used with ECapabilityClientResolveEmbeddedURL
        * @since 3.1
        * @param aEmbeddedUrl The url of the embedded content
        * @param aCurrentUrl The url of the current page
        * @param aLoadContentType The expected type of the embedded content
        * @param aEmbeddedLinkContent a callback interface to return the embedded content
        * @return EFalse, WidgetUI will resolve the link
        */
        TBool ResolveEmbeddedLinkL( const TDesC& aEmbeddedUrl,
                                    const TDesC& aCurrentUrl,
                                    TBrCtlLoadContentType aLoadContentType,
                                    MBrCtlLinkContent& aEmbeddedLinkContent );

        /**
        * ResolveLinkL
        * WidgetUi plug-in calls this method when the user requests to load content via selecting a link, or any other way. Used with ECapabilityClientNotifyURL
        * @since 3.1
        * @param aUrl The requested url
        * @param aCurrentUrl The url of the current page
        * @param aBrCtlLinkContent a callback interface to return the embedded content
        * @return EFalse, WidgetUi will resolve the link
        */
        TBool ResolveLinkL( const TDesC& aUrl, const TDesC& aCurrentUrl,
                            MBrCtlLinkContent& aBrCtlLinkContent );

        /**
        * CancelAll
        * Cancel all outstanding resolving operations
        * @since 3.1
        * @param none
        * @return void
        */
        void CancelAll() ;

        /**
        * NewDownloadL
        * Inform the host application that a new download has started using the Download Manager
        * @since 3.1
        * @param aTransactionID The ID of the transaction, it is unique as long as the transaction is on-going
        * @param aFileName The name of file in which the content is stored/
        * @param aContentType The content type of the downloaded content
        * @param aUrl The Url of the request to be done in the new window
        * @return ETrue if the file is handled progressively, EFalse otherwise
        */
        TBool NewDownloadL( TUint aTransactionID,
                            const TDesC& aFileName,
                            const TDesC& aContentType,
                            const TDesC& aUrl );

        /**
        * ResumeDownloadL
        * Inform the host application that a new download has started using the Download Manager
        * @since 3.1
        * @param aTransactionID The ID of the transaction, it is unique as long as the transaction is on-going
        * @param aFileName The name of file in which the content is stored/
        * @param aContentType The content type of the downloaded content
        * @param aUrl The Url of the request to be done in the new window
        * @return Return ETrue if the file is handled progressively, EFalse otherwise
        */
        virtual void ResumeDownloadL( TUint /*aTransactionID*/,
                                      TUint /*aLength*/,
                                      const TDesC& /*aFileName*/,
                                      const TDesC& /*aContentType*/,
                                      const TDesC& /*aUrl*/ ) { }
        /**
        * HandleDownloadEventL
        * Inform the host application of a progress event related to a download
        * @since 3.1
        * @param aTransactionID The ID of the transaction, it is unique as long as the transaction is on-going
        * @param aDownloadEvent The event to be reported
        * @param aValue The value depends on the event
        * @return void
        */
        void HandleDownloadEventL( TUint aTransactionID,
                                   TBrCtlDownloadEvent aDownloadEvent,
                                   TUint aValue );

    public: // From MBrCtlWindowObserver
    
        
        CBrCtlInterface* OpenWindowL(TDesC& aUrl, TDesC* aTargetName, TBool aUserInitiated, 
             TAny* aReserved);

        /**
        * Find a window by target name
        * @since 3.0
        * @param aTargetName name of the window to find
        * @return Return Value is the browser control associated with the window name
        */
        CBrCtlInterface* FindWindowL( const TDesC& aTargetName ) const;             

        /**
        * Handle window events such as close/focus etc
        * @since 3.0
        * @param aTargetName name of the window to send the event to
        * @param aCommand Command to pass to the window
        * @return void
        */
        void HandleWindowCommandL( const TDesC& aTargetName, TBrCtlWindowCommand aCommand );
        
    private:

        /**
        * CWidgetUiObserver
        * Construtor
        * @since 3.1
        * @param aWindowManager
        * @return none
        */
        CWidgetUiObserver( CWidgetUiWindow& aWindow );

        /**
        * ConstructL
        * EPOC default constructor.
        * By default Symbian 2nd phase constructor is private.
        * @since 3.1
        * @param none
        * @return void
        */
        void ConstructL();

        /**
        * IsFileScheme
        * Find if the scheme is file://
        * @since 3.1
        * @param aFilename
        * @return TBool
        */
        TBool IsFileScheme( const TDesC& aFileName );

        /**
        * ReadFileL
        * Read the file
        * @since 3.1
        * @param aFileName
        * @return HBufC*
        */
        HBufC8* ReadFileL( const TDesC& aFileName);

        /**
        * RecognizeLC
        * Rcognize the mime type.
        * @since 3.1
        * @param aFileName
        * @param aData
        * @return HBufC*
        */
        HBufC* RecognizeLC( const TDesC& aFileName, const TDesC8& aData );

        /**
        * TranslateURLToFilenameL
        * Translate the file name from a URL to a valid file name in the system..
        * @since 3.1
        * @param aFileName
        * @return TBool
        */
        TBool TranslateURLToFilenameL( const TDesC& aFileName, const TDesC& aLanguageDir  );

    private:   // data

        CWidgetUiWindow* iWindow; // not owned, not responsible for deleting
#ifdef _DEBUG
        RFileLogger             iFileLogger;
        TBool                   iCanLog;
#endif
        // The loaded file's name. This is different from the URL. It does not contain the scheme.
        HBufC*                  iFileName;

        CDocumentHandler*       iHandler;
        TBool                   iConnected;
        TFileName               iAppPrivatePath;
        HBufC*                  iSoftkeyExitStr;
        HBufC*                  iLeftSoftkeyStr;
        //Active Object to collect bitmap
        CIdle* iCollectBitmapTimer;
        RFs iFs;
    };

#endif      // WIDGEUIOBSERVER_H

// End of File
