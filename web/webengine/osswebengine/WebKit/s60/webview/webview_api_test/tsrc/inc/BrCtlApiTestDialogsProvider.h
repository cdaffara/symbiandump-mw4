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
* Description:  Forwards Dialog requests to the BrowserDialogsProvider
*
*/



#ifndef BROWSERDIALOGSPROVIDERPROXY_H
#define BROWSERDIALOGSPROVIDERPROXY_H

//  INCLUDES

// User includes

// System Includes
#include <BrCtlDialogsProvider.h>
//#include <BrowserDialogsProviderProxy.h>
#include <e32std.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CBrCtlDialogsProvider;
class CBrowserDialogsProviderBlocker;
class CBrowserWindowFocusNotifier;
class CBrowserWindow;
class MBrowserDialogsProviderObserver;

// CLASS DECLARATION

/**
*  Provide dialogs needed for browser operation
*
*  @since 3.0
*/
class CBrowserDialogsProviderProxy :    public  CBase,
                                        public  MBrCtlDialogsProvider
    {
    // Construction/Destruction
    public:
                       
        // Two stage constructor
        static CBrowserDialogsProviderProxy* NewL( );

        // Destructor
        virtual ~CBrowserDialogsProviderProxy();

    protected:

        // 1st stage Constructor
        CBrowserDialogsProviderProxy( );
        

        // 2nd stage constructor
        void ConstructL();

    public: // From  MBrCtlDialogsProvider
        
        /**
        * Notify the user of an error
        * @since 3.0
        * @param aErrCode The error that occured
        * @return void
        */
        virtual void DialogNotifyErrorL( TInt aErrCode );

        /**
        * Notify the user of an http error
        * @since 3.0
        * @param aErrCode The error that occured
        * @param aUri The uri of the request that failed
        * @return void
        */
        virtual void DialogNotifyHttpErrorL( TInt aErrCode,
                                                        const TDesC& aUri );

        /**
        * File selection dialog
        * @since 3.0
        * @param aStartPath The initial displayed directory
        * @param aRootPath The top most directory that the user can go up to
        * @param aSelectedFileName The selected file name. 
        * Returned on cleanup stack. Browser control will free the buffer
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogFileSelectLC( const TDesC& aStartPath,
                                            const TDesC& aRootPath,
                                            HBufC*& aSelectedFileName);

        /**
        * List selection dialog
        * @since 3.0
        * @param aTitle The title, could be empty
        * @param aBrowserSelectOptionType The type of the list box 
        * @param aOptions A list of options to display
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogSelectOptionL(
                            const TDesC& aTitle, 
                            TBrCtlSelectOptionType aBrowserSelectOptionType, 
                            CArrayFix<TBrCtlSelectOptionData>& aOptions );

        /**
        * User Authentication dialog. 
        * User name and password are returned on cleanup stack
        * @since 3.0
        * @param aUrl The url requiring authentication
        * @param aRealm The realm requiring authentication
        * @param aDefaultUser/Name The user name that was used before for this
        * realm and path, if any
        * @param aReturnedUserName The user name entered by the user
        * @param aReturnedPasswd The password entered by the user
        * @param aBasicAuthentication A flag if basic authentication was used
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogUserAuthenticationLC(
                                        const TDesC& aUrl, 
                                        const TDesC& aRealm, 
                                        const TDesC& aDefaultUserName, 
                                        HBufC*& aReturnedUserName, 
                                        HBufC*& aReturnedPasswd,
                                        TBool aBasicAuthentication = EFalse);

        /**
        * Display a note to the user. No softkeys
        * @since 3.0
        * @param aMessage The message to display
        * @return void
        */
        virtual void DialogNoteL( const TDesC& aMessage ); 
 
        /**
        * Display a note to the user with ok softkey only
        * @since 3.0
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @return void
        */
        virtual void DialogAlertL( const TDesC& aTitle,
                                    const TDesC& aMessage );

        /**
        * Display confirmation message to the user
        * @since 3.0
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @param aYesMessage The text to display on left softkey
        * @param aNoMessage The text to display on right softkey
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogConfirmL(  const TDesC& aTitle,
                                        const TDesC& aMessage,
                                        const TDesC& aYesMessage,
                                        const TDesC& aNoMessage);

        /**
        * Display input dialog to the user
        * @since 3.0
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @param aDefaultInput The default input if available
        * @param aReturnedInput The input entered by the user
        * Returned on the cleanup stack, owned by the calling application
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogPromptLC(const TDesC& aTitle,
                                     const TDesC& aMessage,
                                     const TDesC& aDefaultInput,
                                     HBufC*& aReturnedInput);

        /**
        * Display object info and ask confirmation before download
        * @since 3.0
        * @param aBrCtlObjectInfo The object info
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogDownloadObjectL( 
                                        CBrCtlObjectInfo* aBrCtlObjectInfo );

        /**
        * DIsplay the images that appear in the current page
        * @since 3.0
        * @param aPageImages The images that appear in this page
        * @return vois
        */
        virtual void DialogDisplayPageImagesL( 
                            CArrayFixFlat<TBrCtlImageCarrier>& aPageImages);

        /**
        * Cancel any dialog that is being displayed due to 
        * e.g. Browser exit or page was destroyed
        * @since 3.0
        * @return void
        */
        virtual void CancelAll();
        
        /**
        * Display search on page dialog
        * @since 3.0
        * @return void
        */
        virtual void DialogFindL() { /* TODO: implement this body!!! */ } ;
        
        /**
        * Mime File selection dialog for specific mime type files
        * @since 3.2
        * @param aSelectedFileName The selected file name. 
        * @param aMimeType The accepted mime type
        * Returned on cleanup stack. Browser control will free the buffer
        * @return EFalse if the user canceled, ETrue otherwise
        */
        virtual TBool DialogMimeFileSelectLC( HBufC*& aSelectedFileName,
									  const TDesC& aMimeType );
        
    
    public:     // New functions
    
        /**
        * Display a tooltip (info popup note)
        * @since 3.0
        * @return void
        */                                   
        void ShowTooltipL( const TDesC& aText, TInt aDuration, TInt aDelay );
        
        /**
        * Display a dialog which tracks upload progress
        * @since 3.0    
        * @param aTotalSize     the total size of a file being uploaded
        * @param aChunkSize     the size of the current chunk being uploaded
        * @param aIsLastChunk   boolean value indicating whether the current 
        * @param aObserver      an observer for the upload progress dialog
        * chunk is the last one in the entire download
        * @return void
        */
        void UploadProgressNoteL( 
                            TInt32 aTotalSize, 
                            TInt32 aChunkSize,
                            TBool aIsLastChunk,
                            MBrowserDialogsProviderObserver* aObserver );
    
        /**
        * Callback from Window, indicating that the window is now active
        * @since 3.0
        */
        void WindowActivated();

    // DATA MEMBERS
    private:

        TBool iCancelWaitingDialogs;    // ETrue means that waiting dialogs should not be displayed when flushed
        
    };

#endif      // BROWSERDIALOGSPROVIDERPROXY_H   
            
// End of File
