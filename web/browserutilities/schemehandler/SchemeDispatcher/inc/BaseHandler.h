/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class CBaseHandler.   
*      
*
*/


#ifndef BASE_HANDLER_H
#define BASE_HANDLER_H

// INCLUDES

#include "SchemeHandler.h"
#include <e32base.h>

// FORWARD DECLARATION
#include <AknServerApp.h>
// CONSTS
/// UID of Scheme App.
LOCAL_D const TUid KUidSchemeApp = { SCHEME_APP_UID };

// CLASS DECLARATION

/**
* Scheme Handler IF definition class
*/
class CBaseHandler : public CSchemeHandler
    {

    public:     // New functions

		/**
		* Url Handler with embedding
		* @param -
		*/
		virtual void HandleUrlEmbeddedL() = 0;

		/**
		* Url Handler without embedding
		* @param -
		*/
		virtual void HandleUrlStandaloneL() = 0;

		/**
		* Observer
		* @param - aSchemeDoc
		*/
        void Observer( MAknServerAppExitObserver* aSchemeDoc );

	protected:

        /**
        * Constructor.
        */      
		CBaseHandler();

       /**
        * Destructor.
        */      
        virtual ~CBaseHandler();

        /**
        * Second phase constructor. Leaves on failure.
		* BaseConstructL must be called from inherited ConstructL
		* preferably before do anything
		* @param - aUrl
        */      
		void BaseConstructL( const TDesC& aUrl );

	protected: // New functions

		/**
        * Remove scheme from the begining of Url.
        * @param aScheme - sheme pattern
        * @return TPtrC
        */    
		TPtrC RemoveSchemeFromUrlL( const TDesC& aScheme );

        /**
        * Handle the possible errors given by
        * subsystems
        * @param aErrorCode - error code
        * @return -
        */
        void ErrorHandlerL( TInt aErrorCode );

        /**
        * Handle the possible errors given by
        * subsystems
        * @param -
        * @return the value of the shared data (boolean)
        */
        TBool ReadSdConfirmDtmfValueL();


        /**
        * Launch SchemeApp passing Generic Params List
        * @param -
        * @return -
        */
        void LaunchSchemeAppWithCommandLineL();

	protected: // Data

        MAknServerAppExitObserver* iSchemeDoc; ///< Used.

    protected: // Data

		// Results of parsing
		HBufC* iParsedUrl; ///< Owned.
	};

#endif /* def BASE_HANDLER_H */