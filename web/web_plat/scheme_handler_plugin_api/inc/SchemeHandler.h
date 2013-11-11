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
*      Declaration of class CSchemeHandler.   
*      
*
*/


#ifndef SCHEME_HANDLER_H
#define SCHEME_HANDLER_H

#warning The Scheme Handler API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES

#include <e32base.h>
#include <AiwGenericParam.h>

// FORWARD DECLARATION

class MAknServerAppExitObserver;

// CONSTS

// CLASS DECLARATION

/**
* Scheme Handler IF definition class
*/
class CSchemeHandler : public CBase
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param aUrl Url
        * @return The created object.
        */      
        inline static CSchemeHandler* NewL( const TDesC& aUrl );
        
       /**
        * Destructor.
        */      
        inline virtual ~CSchemeHandler();

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
        virtual void Observer( MAknServerAppExitObserver* aSchemeDoc ) = 0;		

		/**
		* Set Generic Parameters
		* @param - aParamList
		*/
        inline void SetParameterList(CAiwGenericParamList* aParamList); // Takes ownership

		//reserved functions
		virtual inline TInt Reserved_1( TAny* aAny );
		virtual inline TInt Reserved_2( TAny* aAny );

	private: // Data

		/// Required attribute for the framework
		/// (An identifier used during destruction)
		TUid iDtor_ID_Key;

    protected:

        CAiwGenericParamList* iParamList; ///< OWNS
	};

#include <SchemeHandler.inl>

#endif /* def SCHEME_HANDLER_H */
