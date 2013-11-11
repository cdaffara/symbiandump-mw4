/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class CCodDocument.
*
*/


#ifndef COD_DOCUMENT_H
#define COD_DOCUMENT_H

// INCLUDES

#include <e32base.h>
#include <GenericParamConsumer.h>

// FORWARD DECLARATIONS

class CEikAppUi;
class CCodEngBase;
class CRequestCompleteCallback;
class CCodAppUi;

// CLASS DECLARATION

/**
* Dummy document class COD Viewer application (application is not file-based).
*/
NONSHARABLE_CLASS( CCodDocument ): public CAiwGenericParamConsumer
    {

    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aApp Application.
        * @param aCod COD or DD?
        * @return The created document.
        */
        static CCodDocument* NewL( CEikApplication& aApp, TBool aCod );

        /**
        * Destructor.
        */
        virtual ~CCodDocument();

    public:     // Member access

        /**
        * Access the model.
        * @return The model.
        */
        CCodEngBase& Model();

    public:     // from CEikDocument

        /**
        * Create Application Ui.
        * @return App Ui.
        */
        CEikAppUi* CreateAppUiL();

        /**
        * Open a file.
        * @param aDoOpen Don't know what it is.
        * @param aFilename File name.
        * @param aFs File Server Session to be used.
        * @return NULL (COD file is not EPOC-style store based).
        */
        CFileStore* OpenFileL
            (
            TBool aDoOpen,
            const TDesC& aFilename,
            RFs& aFs
            );

        /**
        * Open a file.
        * @param aFs File Server Session to be used.
        * @param aFile File.
        * @return NULL (COD file is not EPOC-style store based).
        */
        CFileStore* OpenFileL( RFs& aFs, RFile& aFile );

        /**
        * Open a file.
        * @param aFile File.
        * @param aFileStore Return value placeholder.
        * Returns NULL (COD file is not EPOC-style store based).
        */
        void OpenFileL( CFileStore*& aFileStore, RFile& aFile );
        
    private:    // Constructors and destructor

        /**
        * Constructor.
        * @param aCod COD or DD?
        * @param aApp Application.
        */
        CCodDocument( CEikApplication& aApp, TBool aCod );

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    private:    // new methods

        /**
        * Get AppUi upcasted to real type.
        * @return App UI.
        */
        inline CCodAppUi* CodAppUi();

        /**
        * Handle file opening completion (display descriptor or exit).
        */
        void OpenFileDone();

        /**
        * Handle file opening completion (static wrapper for TCallBack use).
        * @param aPtr Pointer passed to the callback (this object as TAny*).
        * @return EFalse (as TInt).
        */
        static TInt StaticOpenFileDone( TAny* aPtr );

    private:    // data

        /// COD or DD?
        TBool iCod;
        /// Model (data & semantics). Owned.
        CCodEngBase* iModel;
        /// Callback object. Owned.
        CRequestCompleteCallback* iCallback;
    };

#endif /* def COD_DOCUMENT_H */
