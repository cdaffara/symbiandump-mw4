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
* Description:  Handle dialogs needed for browser operation
*
*/

#ifndef BROWSERDIALOGSPROVIDER_H
#define BROWSERDIALOGSPROVIDER_H

#warning The Browser Dialogs Provider API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

//  INCLUDES
// System Includes
#include <BrCtlDialogsProvider.h>
#include <eikenv.h>
#include <coneresloader.h> 
#include <e32std.h>
#include <MMGFetchVerifier.h>

// CONSTANTS

// Tooltip
const TInt KDefaultTooltipDelay = 1;        // time in milliseconds
const TInt KDefaultTooltipDuration = 2000;  // time in milliseconds
const TInt KErrTooManyRedirects  = -20019;

// FORWARD DECLARATIONS
class CBrowserSelectElementDlg;
class MBrowserDialogsProviderObserver;
class CAknInfoPopupNoteController;
class CBrowserUploadProgressNote;
class MMGFetchVerifier;

//=============================================================================
// CLASS DECLARATION: CBrowserDialogsProvider
//=============================================================================
/**
*  Provide dialogs needed for browser operation
*
*  @lib BrowserDialogsProvider.lib
*  @since 2.8
*/
class CBrowserDialogsProvider : public  CBase,
                                public  MBrCtlDialogsProvider,
                                public  MMGFetchVerifier
    {
    // Construction/Destruction
    public:
        
        // EPOC Two stage constructor
        IMPORT_C static CBrowserDialogsProvider* NewL( 
                                MBrowserDialogsProviderObserver* aObserver );

        // Destructor
        virtual ~CBrowserDialogsProvider();

    protected:

        // 1st stage Constructor
        CBrowserDialogsProvider( MBrowserDialogsProviderObserver* aObserver );

        // 2nd stage constructor
        void ConstructL();

    public: // From MBrCtlDialogsProvider
        
        /**
        * Notify the user of an error
        * @since 2.8
        * @param aErrCode The error that occured
        * @return void
        */
        IMPORT_C virtual void DialogNotifyErrorL(TInt aErrCode);

        /**
        * Notify the user of an http error
        * @since 2.8
        * @param aErrCode The error that occured
        * @param aUri The uri of the request that failed
        * @return void
        */
        IMPORT_C virtual void DialogNotifyHttpErrorL( TInt aErrCode,
                                                        const TDesC& aUri );

        /**
        * File selection dialog
        * @since 2.8
        * @param aStartPath The initial displayed directory
        * @param aRootPath The top most directory that the user can go up to
        * @param aSelectedFileName The selected file name. 
        * Returned on cleanup stack. Browser control will free the buffer
        * @return EFalse if the user canceled, ETrue otherwise
        */
        IMPORT_C virtual TBool DialogFileSelectLC(const TDesC& aStartPath,
                                            const TDesC& aRootPath,
                                            HBufC*& aSelectedFileName);

        /**
        * List selection dialog
        * @since 2.8
        * @param aTitle The title, could be empty
        * @param aBrowserSelectOptionType The type of the list box 
        * @param aOptions A list of options to display
        * @return EFalse if the user canceled, ETrue otherwise
        */
        IMPORT_C virtual TBool DialogSelectOptionL(
                            const TDesC& aTitle, 
                            TBrCtlSelectOptionType aBrowserSelectOptionType, 
                            CArrayFix<TBrCtlSelectOptionData>& aOptions );

        /**
        * User Authentication dialog. 
        * User name and password are returned on cleanup stack
        * @since 2.8
        * @param aUrl The url requiring authentication
        * @param aRealm The realm requiring authentication
        * @param aDefaultUser/Name The user name that was used before for this
        * realm and path, if any
        * @param aReturnedUserName The user name entered by the user
        * @param aReturnedPasswd The password entered by the user
        * @param aBasicAuthentication A flag if basic authentication was used
        * @return EFalse if the user canceled, ETrue otherwise
        */
        IMPORT_C virtual TBool DialogUserAuthenticationLC(
                                        const TDesC& aUrl, 
                                        const TDesC& aRealm, 
                                        const TDesC& aDefaultUserName, 
                                        HBufC*& aReturnedUserName, 
                                        HBufC*& aReturnedPasswd,
                                        TBool aBasicAuthentication = EFalse);

        /**
        * Display a note to the user. No softkeys
        * @since 2.8
        * @param aMessage The message to display
        * @return void
        */
        IMPORT_C virtual void DialogNoteL( const TDesC& aMessage ); 
 
        /**
        * Display a note to the user with ok softkey only
        * @since 2.8
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @return void
        */
        IMPORT_C virtual void DialogAlertL( const TDesC& aTitle,
                                                    const TDesC& aMessage );

        /**
        * Display confirmation message to the user
        * @since 2.8
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @param aYesMessage The text to display on left softkey
        * @param aNoMessage The text to display on right softkey
        * @return EFalse if the user canceled, ETrue otherwise
        */
        IMPORT_C virtual TBool DialogConfirmL(  const TDesC& aTitle,
                                                const TDesC& aMessage,
                                                const TDesC& aYesMessage,
                                                const TDesC& aNoMessage);

        /**
        * Display input dialog to the user
        * @since 2.8
        * @param aTitle The title, could be empty
        * @param aMessage The message to display
        * @param aDefaultInput The default input if available
        * @param aReturnedInput The input entered by the user
        * Returned on the cleanup stack, owned by the calling application
        * @return EFalse if the user canceled, ETrue otherwise
        */
        IMPORT_C virtual TBool DialogPromptLC(const TDesC& aTitle,
                                     const TDesC& aMessage,
                                     const TDesC& aDefaultInput,
                                     HBufC*& aReturnedInput);

        /**
        * Display object info and ask confirmation before download
        * @since 2.8
        * @param aBrCtlObjectInfo The object info
        * @return EFalse if the user canceled, ETrue otherwise
        */
        IMPORT_C virtual TBool DialogDownloadObjectL( 
                                        CBrCtlObjectInfo* aBrCtlObjectInfo );

        /**
        * DIsplay the images that appear in the current page
        * @since 2.8
        * @param aPageImages The images that appear in this page
        * @return vois
        */
        IMPORT_C virtual void DialogDisplayPageImagesL( 
                            CArrayFixFlat<TBrCtlImageCarrier>& aPageImages);

        /**
        * Cancel any dialog that is being displayed due to 
        * e.g. Browser exit or page was destroyed
        * @since 2.8
        * @return void
        */
        IMPORT_C virtual void CancelAll();

        
        /**
        * Display search on page dialog
        * @since 3.0
        * @return void
        */
        inline virtual void DialogFindL() {}; 

                            
    public:         // New functions
    
        /**
        * Display a tooltip (info popup note)
        * @since 3.0
        * @return void
        */
        IMPORT_C void ShowTooltipL( const TDesC& aText, 
                                    TInt aDuration = KDefaultTooltipDuration, 
                                    TInt aDelay = KDefaultTooltipDelay );
                                    
        
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
        IMPORT_C void UploadProgressNoteL(  
                                    TInt32 aTotalSize,
                                    TInt32 aChunkSize,
                                    TBool aIsLastChunk,
                                    MBrowserDialogsProviderObserver* aObserver
                                                     );
        // From MBrCtlDialogsProvider
        /**
        * Mime File selection dialog for specific mime type files
        * @since 3.2
        * @param aSelectedFileName The selected file name. 
        * @param aMimeType The accepted mime type
        * Returned on cleanup stack. Browser control will free the buffer
        * @return EFalse if the user canceled, ETrue otherwise
        */
        IMPORT_C virtual TBool DialogMimeFileSelectLC(
                                            HBufC*& aSelectedFileName,
                                            const TDesC& aMimeType);

        /**
        * Display an confirmation query with given dialog resource
        * @since Series 60 2.8
        * @param aPrompt resource if of prompt to display.
        * @param aResId resource id of command set 
        * (aResId = 0 means R_AVKON_SOFTKEYS_YES_NO as per default )
        * @param aAnimation animation overrides default one
        * @return selected softkey id.
        * If you give your own aResId you have to check the return value!
        * In that case return value are commands 
        * as your specified in resource of that CBA.
        */        
        TInt ConfirmQueryDialogL(   const TInt aPromptResourceId, 
                                    TInt aResId = 0,
                                    TInt aAnimation = 0 );

        /**
        * Display an confirmation query with given dialog resource
        * @since Series 60 2.8
        * @param aPrompt Prompt to display.
        * @param aResId resource id of dialog
        * @param aAnimation animation overrides default one
        * @return selected softkey id.
        */      
        TInt ConfirmQueryDialogL(   const TDesC& aPrompt, 
                                    TInt aResId = 0,
                                    TInt aAnimation = 0 );
        
        /**
        * User authentication dialog
        * @since Series 60 2.8
        * @param aUsername entered username
        * @param aPassword entered password
        * @return AVKON error code
        */
        TInt GetUserPasswordL( TDes& aUsername, TDes& aPassword );
        
        inline void SetObserver( 
                            MBrowserDialogsProviderObserver* aDialogsObserver )
            {
            iObserver = aDialogsObserver;
            }

		/**
		* Checks if the drive is ready
		* @param aDrive enumerated drive number
		* @return error code
		*/
		TInt DriveReady( TDriveNumber aDrive );
		
		inline TBool IsDialogLaunched() 
		    {
		    return (iDialogs.Count() > 0);
		    }

    protected:      // New functions

        // Loads the resources for the DLL
        void AssureResourceL();

    private:

        // Removes a pointer to an open dialog from the array
        void RemoveDialogFromArray();

    private: // from MMGFetchVerifier
        TBool VerifySelectionL(const MDesCArray* aSelectedFiles);

    // DATA MEMBERS
    private:

        // For DialogSelectOptionL
        CBrowserSelectElementDlg* iSelectDlg;               // owned
        
        CBrowserUploadProgressNote* iUploadProgressNote;    // owned
        
        RPointerArray<CCoeControl> iDialogs;                // owned

        CCoeEnv& iCoeEnv;   // Reference to CONE environment.

        MBrowserDialogsProviderObserver* iObserver;         // not ownedCBrowserDialogsProvider

        RConeResourceLoader iResourceLoader;
        TBool iResourceOpened;	  			  
        TBool iErrResourceOpened;

        // Reusable tooltip
        CAknInfoPopupNoteController* iTooltip;              // owned

        enum TPanicReason
            {
            EInvalidSizeValue
            };
    };

#endif      // BROWSERDIALOGSPROVIDER_H   
            
// End of File
