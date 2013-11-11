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
*      Declaration of Scheme handler interface implementation for rtsp:// scheme
*      
*
*/


#ifndef RTSP_HANDLER_H
#define RTSP_HANDLER_H

// INCLUDES

#include "BaseHandler.h"
#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>

// FORWARD DECLARATION	
class CAknLaunchAppService;
class CDocumentHandler;
			
// CLASS DECLARATION

/**
* Scheme Handler IF implementation class for rtsp scheme
*/
class CRtspHandler : public CBaseHandler,
                     public MAknServerAppExitObserver
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CRtspHandler* NewL( const TDesC& aUrl );

       /**
        * Destructor.
        */      
        virtual ~CRtspHandler();
        
	private: // Constructors

        /**
        * Constructor.
        */      
		CRtspHandler();

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

		CEikDocument* iDoc; ///< Owned.		
		CAknLaunchAppService* iLaunchAppService;  ///<Owned
		TBool                 iSync;
		CActiveSchedulerWait  iWait;		

		CDocumentHandler*     iDocHandler; ///< Owned.        
	};

#endif /* def RTSP_HANDLER_H */
