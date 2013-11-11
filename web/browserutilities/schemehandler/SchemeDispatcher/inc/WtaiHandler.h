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
*      Declaration of Scheme handler interface implementation for wtai:// scheme
*      
*
*/


#ifndef WTAI_HANDLER_H
#define WTAI_HANDLER_H

// INCLUDES

#include "BaseHandler.h"
#include "BrowserTelService.h"
#include <e32base.h>

// FORWARD DECLARATION

// CLASS DECLARATION

/**
* Scheme Handler IF implementation class for wtai scheme
*/
class CWtaiHandler : public CBaseHandler, public MBrowserTelServiceObserver
    {
    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CWtaiHandler* NewL( const TDesC& aUrl );

       /**
        * Destructor.
        */      
        virtual ~CWtaiHandler();

	private: // Constructors

        /**
        * Constructor.
        */      
		CWtaiHandler();

        /**
        * Second phase constructor. Leaves on failure.
		* @param - aUrl
        */      
		void ConstructL( const TDesC& aUrl );

    private: // Functions from base classes

		/**
		* Url Handler with embedding
		* @param -
		*/
		void HandleUrlEmbeddedL();

		/**
		* Url Handler without embedding
		* @param -
		*/
		void HandleUrlStandaloneL();
        
		/**
		* Get library function from wtai sheme
		* @param -
		* @return TPtrC library function
		*/

		TPtrC GetWtaiLibraryFunctionL();

		/**
		* Get the specific parameter of wtai
		* @param - aPos the position of the required parameter
		* @return TPtrC
		*/

		TPtrC GetParameterL(TInt aPos);

		/**
		* Get the number of parameter of wtai sheme
		* @param -
		* @return TInt
		*/
		TInt GetParameterCountL();

        /**
		* Verify the parameters of wtai sheme
        * Leaves if it is not a valid wtai library or function
		* @param -
		* @return TInt - first occurance of colon position
		*/
        TInt VerifyWtaiSchemeL( TPtrC path );

		/**
        * Notification of the state change.
        * @param aEvent The new state.
        */
		void BrowserTelServiceEvent(TBrowserTelServiceState aEvent);

		/**
        * Notification of the error that occurred.
        * @param aError The error.
        */
		void BrowserTelServiceError(TBrowserTelServiceError aError);

		/**
        * Notification when the phonebook dialog has been exited.
        * @param.
        */
		void NotifyClient();

	private: // Data

        enum TSchemeWtaiParams       //< Positions of the possible wtai parameters
            {
            ESchemeWtaiNumber = 1,   ///< Phone number
            ESchemeWtaiName = 2,     ///< Name
            ESchemeWtaiEmail = 3     ///< Email address
            };

		CBrowserTelService* iTelService; ///< Owned.
	};

#endif /* def WTAI_HANDLER_H */
