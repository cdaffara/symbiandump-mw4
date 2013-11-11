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
* Description:  Declaration of class CSchemeDocument
*
*/


#ifndef SCHEME_DOCUMENT_H
#define SCHEME_DOCUMENT_H

// INCLUDES

#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>
#include <AknServerApp.h>

// FORWARD DECLARATIONS

class  CEikAppUi;
class  CSchemeHandler;
class  CSchemeAppUi;

// CLASS DECLARATION
/**
* Shutdown class forScheme App.
*/
class CSchemeShutdown : public CActive
    {
    public:

        /**
        * C++ default constructor.
        */
        CSchemeShutdown( CSchemeAppUi* iAppUi );

        /**
        * Destructor.
        */
        ~CSchemeShutdown();

    public:

        /**
        * Starts handle request completion event.
        * @param -
        * @return None.
        */
        void Start();

    private: // From CActive

        /**
        * Handles an active object’s request completion event.
        * @param -
        * @return None.
        */
        void RunL();

        /**
        * Implements cancellation of an outstanding request.
        * @param -
        * @return None.
        */
        void DoCancel();

    private:

        CSchemeAppUi* iAppUi;
    };

/**
* Document class forScheme App.
*/
class CSchemeDocument : public CEikDocument,
                        public MAknServerAppExitObserver
    {
    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aApp Application.
        * @return The created document.
        */
        static CSchemeDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CSchemeDocument();

    private:     // Constructors

        /**
        * Constructor.
        * @param aApp Application.
        */
        CSchemeDocument( CEikApplication& aApp );

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    private:     // Functions from base classes

        /**
        * Create Application Ui.
        * @return Pointer to App Ui.
        */
        CEikAppUi* CreateAppUiL();

        /**
        * Open a file.
        * @param aDoOpen Don't know what it is.
        * @param aFilename File name.
        * @param aFs File Server Session to be used.
        * @return NULL (DD file is not EPOC-style store based).
        *//*
        CFileStore* OpenFileL
            (
            TBool aDoOpen,
            const TDesC& aFilename,
            RFs& aFs
            );*/

		/**
		 *
		 * Implements the required behaviour when the editing of an embedded
		 * document completes.
		 *
		 * @param     
		 *            Indicates the state of the document.
		 */
        void HandleServerAppExit( TInt aReason );

    private: // Data members

		//CSchemeHandler*  iSchemeHandler; ///< Owned.
		CSchemeAppUi*    iSchemeAppUi;
        CSchemeShutdown* iShutdown;
    };

#endif // SCHEME_DOCUMENT_H

// End of File

