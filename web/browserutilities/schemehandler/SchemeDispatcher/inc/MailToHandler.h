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
*      Declaration of Scheme handler interface implementation for mailto:// scheme
*      
*
*/


#ifndef MAIL_TO_HANDLER_H
#define MAIL_TO_HANDLER_H

// INCLUDES

#include "BaseHandler.h"
#include "BrowserTelService.h"
#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>

//#include <bamdesca.h>

// FORWARD DECLARATION

// CLASS DECLARATION


/**
* Scheme Handler IF implementation class for mailto scheme
*/
class CMailToHandler : public CBaseHandler, public MBrowserTelServiceObserver
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CMailToHandler* NewL( const TDesC& aUrl );

       /**
        * Destructor.
        */      
        virtual ~CMailToHandler();
        
	private: // Constructors

        enum TSchemeMailToFields          ///<Possible MailTo fields
            {
            ESchemeMailTo,                ///< MailTo.
            ESchemeSubject,               ///< Subject.
            ESchemeMsgBody,               ///< Message Body.
            ESchemeCc,                    ///< CC.
            ESchemeTo,                    ///< TO.
            ESchemeBcc,                   ///< Bcc.
			ESchemeNoMore
            };

        /**
        * Constructor.
        */      
		CMailToHandler();

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
		* Get the appropriate filed from the url
		* @param aHeader the name of the field
		*/
		TPtrC GetField(const TDesC& aHeader);

		/**
		* Check if the field is in the url
		* @param aHeader the name of the field
		* @return TBool
		*/
		TBool IsHeader(const TDesC& aHeader);

		/**
		* Starting position of the specific field
		* @param aHeader the name of the field
		* @return TInt Starting position of the specific field
		*/
        TInt FieldStart(const TDesC& aHeader);

		/**
		* Ending position of the specific field
		* @param aHeader the name of the field
		* @return TInt Ending position of the specific field
		*/
		TInt FieldEnd( const TDesC& aHeader );

		/**
		* Get the enumerated position of the specific field
		* @param aHeader the name of the field
		* @return TSchemeMailToFields
		*/
		TSchemeMailToFields GetHeaderPos( const TDesC& aHeader );

		/**
		* Get the next enumerated position of the specific field
		* @param aPos enumerated position of the field
		*/
		//TSchemeMailToFields CMailToHandler::GetNextField(TSchemeMailToFields aPos);
        TInt GetNextField( TInt aStart );

		/**
        * Notification of the state change.
        * @param aEvent The new state.
		* @return TSchemeMailToFields
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

		/**
        * Returns the minimum value of the arguments.
        * @param.
        * @return
        */
        TInt Minimum( TInt aPos1, TInt aPos2, TInt aPos3, TInt aPos4, TInt aPos5 );

        /**
        * Changes ?to= hedear into comma in case of multiple recipients
        * @param aRecipients - email addresses.
        * @return parsed descriptor
        */
        HBufC* ChangeSeparationLC( const TDesC& aRecipients );

	private: // Data
		CBrowserTelService* iTelService; ///< Owned.
	};

#endif /* def MAIL_TO_HANDLER_H */
