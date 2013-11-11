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


#ifndef CTI_HANDLER_H
#define CTI_HANDLER_H

// INCLUDES

#include "BaseHandler.h"
#include "BrowserTelService.h"
#include <e32base.h>

// FORWARD DECLARATION

// CLASS DECLARATION

/**
* Scheme Handler IF implementation class for wtai scheme
*/
class CCtiHandler : public CBaseHandler, public MBrowserTelServiceObserver
    {
    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CCtiHandler* NewL( const TDesC& aUrl );

       /**
        * Destructor.
        */      
        virtual ~CCtiHandler();

	private: // Constructors

        /**
        * Constructor.
        */      
		CCtiHandler();

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
        * Notification of the state change.
        * @param aEvent The new state.
        */
		void BrowserTelServiceEvent(TBrowserTelServiceState aEvent);

		/**
        * Notification of the error that occurred.
        * @param aError The error.
        */
		void BrowserTelServiceError(TBrowserTelServiceError aError);
        
        void NotifyClient();

	private: // Data

		CBrowserTelService* iTelService; ///< Owned.
	};

#endif /* def CTI_HANDLER_H */
