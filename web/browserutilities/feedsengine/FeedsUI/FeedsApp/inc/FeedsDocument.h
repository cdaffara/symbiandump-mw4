/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Feeds application's document.
*
*/

#ifndef FEEDS_DOCUMENT_H
#define FEEDS_DOCUMENT_H


// INCLUDES
#include <GenericParamConsumer.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFeedsAppUi;
class CEikApplication;

// CLASS DECLARATION


/**
*  Feeds application's document.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CFeedsDocument: public CAiwGenericParamConsumer
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * Construct a CFeedsDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer to the created object.
        *
        * @since ??
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CFeedsDocument.
        */
        static CFeedsDocument* NewL(CEikApplication& aApp);

        /**
        * Two-phased constructor.
        * Construct a CFeedsDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer to the created object.
        *
        * @since ??
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CFeedsDocument.
        */
        static CFeedsDocument* NewLC(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CFeedsDocument();


    public: // From CAknDocument
        /**
        * Creates a CFeedsAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        *
        * @since ??
        * @return Pointer to created instance of AppUi.
        */
        virtual CEikAppUi* CreateAppUiL();

        /**
        * Opens a given file from the app-framework.
        *
        * @since ??
        * @param aFileStore ?
        * @param aFile ?
        * @return void.
        */
        virtual void OpenFileL(CFileStore*& aFileStore, RFile& aFile);
        

    private:
        /**
        * C++ default constructor.
        */
        void ConstructL();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        CFeedsDocument(CEikApplication& aApp);

#if 0
        /**
        * Subscribes to a feed, provided via OpenFileL (from an external app).
        *
        * @since 3.0
        * @param aUrl The feed's url.
        * @param aContentType The feed's content-type.
        * @param aCharSet The feed's char-set.
        * @param aFeedBuffer A buffer containing the downloaded feed.
        * @return void.
        */
        void SubscribeToL(const TDesC& aUrl, const TDesC& aContentType, 
                const TDesC& aCharSet, HBufC8& aFeedBuffer);

        /**
        * Subscribes to a feed.
        *
        * @since 3.0
        * @param aUrl The feed's name.
        * @param aUrl The feed's url.
        * @return void.
        */
        void SubscribeToL(const TDesC& aName, const TDesC& aUrl);
#endif
    };

#endif      // FEEDS_DOCUMENT_H
            
// End of File
