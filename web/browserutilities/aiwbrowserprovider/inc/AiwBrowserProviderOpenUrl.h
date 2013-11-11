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
* Description:  header file for implementation of open url functionality
*
*/


#ifndef AIWBROWSERPROVIDEROPENURL_H
#define AIWBROWSERPROVIDEROPENURL_H

// INCLUDES
#include <AiwServiceIfMenu.h>

// CLASS DECLARATION

/**
*  Class which solves the open url functionality.
*
*  @since Series 60 3.1
*/

class CAiwBrowserProviderOpenUrl : public CAiwServiceIfMenu
    {
    public:
        /**
        * Two-phased constructor.
        */           
        static CAiwBrowserProviderOpenUrl* NewL();
        /**
        * Destructor.
        */
        ~CAiwBrowserProviderOpenUrl();
        // New functions
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
	
        /**
        * launches the url
        * @since Series 60 3.1
        * @param url to open
        * @return none
        */        
        void LaunchBrowserL( const TDesC& aUrl );
	
    private: // Implementation
        /**
        * constructor
        * @since Series 60 3.1
        * @param none
        * @return none
        */        
        CAiwBrowserProviderOpenUrl();
        /**
        * constructl 
        * @since Series 60 3.1
        * @param none
        * @return none
        */        
        void ConstructL();

    private: // Data
        TInt iResourceOffset;
        MAiwNotifyCallback* iNotifyCallback;
        const RCriteriaArray* iInterest;
    };

#endif // __CAiwBrowserProvider_H__
