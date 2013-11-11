/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  header file for implementation of save url functionality
*
*/


#ifndef AIWBROWSERPROVIDERSAVEURL_H
#define AIWBROWSERPROVIDERSAVEURL_H

// INCLUDES
#include <AiwServiceIfMenu.h>

// CLASS DECLARATION

/**
*  Class which solves the save url functionality.
*
*  @since Series 60 3.1
*/

class CAiwBrowserProviderSaveUrl : public CAiwServiceIfMenu
    {
    public:
        /**
        * Two-phased constructor.
        */            
        static CAiwBrowserProviderSaveUrl* NewL();
        /**
        * Destructor.
        */
        ~CAiwBrowserProviderSaveUrl();
        // New functions
    private: // From CAiwServiceIfMenu
        /**
        * Input prompt dialog used by Search and Home page features library
        * @since Series 60 3.1
        * @param aMsg prompt message
        * @param aResp entered text
        * @param aMaxLength maximum length allowed in editor
        * @param aLowerCase use lower case in editor or not
        * @return AVKON error code
        */
		TInt DialogPromptReqL( const TDesC& aMsg, 
                                        TDes* aResp, 
                                        TBool aLowerCase, 
                                        TInt aMaxLength = 0 );
        /**
        * Display an information note with no buttons.
        * @since Series 60 3.1
        * @param aDialogResourceId Resource id of note to display.
        * Available notes: R_WMLBROWSER_OK_NOTE (check mark animation);
        * R_WMLBROWSER_INFO_NOTE (info mark animation).
        * @param aPromptResourceId Resource id of the prompt.
        */
        static void InfoNoteL
					( TInt aDialogResourceId, const TInt aPromptResourceId );
		
        /**
        * Display an error note with buttons Ok - <empty>.
        * @since Series 60 3.1
        * @param aPromptResourceId Resource id of the prompt (not the
        * dialog!).
        */
        static void ErrorNoteL( const TInt aPromptResourceId );

    private: // From CAiwServiceIfMenu
        /**
        * init
        * @since Series 60 3.1
        * @param all defined in aiw framework
        * @return none 
        */        
        void InitialiseL(MAiwNotifyCallback& aFrameworkCallback, 
                         const RCriteriaArray& aInterest);
        
        /**
        * Handles service commands
        * @since Series 60 3.1
        * @param all defined in aiw framework
        * @return none
        */        
        void HandleServiceCmdL(const TInt& aCmdId,
		                       const CAiwGenericParamList& aInParamList,
		                       CAiwGenericParamList& aOutParamList,
		                       TUint aCmdOptions = 0,
		                       const MAiwNotifyCallback* aCallback = NULL);
        /**
        * initializes menupane objects
        * @since Series 60 3.1
        * @param menupane
        * @param index
        * @param cascade id
        * @param parameter list
        * @return none
        */        
        void InitializeMenuPaneL(CAiwMenuPane& aMenuPane,
		                         TInt aIndex,
		                         TInt aCascadeId,
		                         const CAiwGenericParamList& aInParamList);
        /**
        * Handles menu commands
        * @since Series 60 3.1
        * @param menu command id 
        * @param in parameter list
        * @param out parameter list
        * @param command options
        * @param callback
        * @return none
        */            
	    void HandleMenuCmdL(TInt aMenuCmdId,
		                    const CAiwGenericParamList& aInParamList,
		                    CAiwGenericParamList& aOutParamList,
		                    TUint aCmdOptions = 0,
		                    const MAiwNotifyCallback* aCallback = NULL);

    private: // Implementation
        /**
        * constructor
        * @since Series 60 3.1
        * @param none
        * @return none
        */         
        CAiwBrowserProviderSaveUrl();
        /**
        * constructl 
        * @since Series 60 3.1
        * @param none
        * @return none
        */                
        void ConstructL();
        /**
        * Saves url
        * @since Series 60 3.1
        * @param url to save
        * @param name of the bookmark
        * @param username
        * @param password
        * @param access point id
        * @return none
        */        
    	void SaveUrlL(  const TDesC& aUrl, 
			const TDesC& aName,
			const TDesC& aUserName, 
			const TDesC& aPassword, 
			const TUint32 aApId );

    private: // Data
        TInt iResourceOffset;
        MAiwNotifyCallback* iNotifyCallback;
        const RCriteriaArray* iInterest;
    };

#endif // __CAiwBrowserProviderSaveUrl_H__
