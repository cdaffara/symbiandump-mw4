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
*      Declaration of Scheme handler interface implementation for http:// scheme
*      
*
*/


#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

// INCLUDES

#include "BaseHandler.h"
#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>
#include <BrowserLauncher.h>
// FORWARD DECLARATION

// CLASS DECLARATION

/**
* Scheme Handler IF implementation class for http scheme
*/
class CHttpHandler : public CBaseHandler,
                     public MAknServerAppExitObserver
    {
    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CHttpHandler* NewL( const TDesC& aUrl );

       /**
        * Destructor.
        */      
        virtual ~CHttpHandler();
        
	private: // Constructors

        /**
        * Constructor.
        */      
		CHttpHandler();

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
		 *
		 * Implements the required behaviour when the editing of an embedded
		 * document completes.
		 *
		 * @param     
		 *            Indicates the state of the document.
		 */
        void HandleServerAppExit(TInt aReason);

    private: // Data

        CEikDocument*     iDoc;        ///< Owned.
        CBrowserLauncher* iLauncher;   ///< Owned.         
	};

#endif /* def HTTP_HANDLER_H */
