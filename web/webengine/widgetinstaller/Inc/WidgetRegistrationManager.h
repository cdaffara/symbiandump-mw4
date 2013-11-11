/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is defines CWidgetRegistrationManager which handles registration of
*                widgets
*
*/


#ifndef WIDGETREGISTRATIONMANAGER_H
#define WIDGETREGISTRATIONMANAGER_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS

/**
* This class handles registration and deregistration during widget
* installation.
*
* @lib WidgetInstallerUI
* @since 3.1
*/
NONSHARABLE_CLASS(CWidgetRegistrationManager): public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWidgetRegistrationManager* NewL( RFs& aFs );

        /**
        * Destructor.
        */
        ~CWidgetRegistrationManager();

    public: // new functions

        /**
        * Register installed widget as non native app
        * @since 3.1
        * @param aWidgetEntry: A widget entry to be registered
        * return void
        */
        void RegisterWidgetL( const TDesC& aMainHTML,
                              const TDesC& aBundleDisplayName,
                              const TDesC& aIconPath,
                              const TDesC& aDriveName,
                              const TUid& aUid );

        /**
        * deregister installed widget as non native app
        * @since 3.1
        * @param aUid - the UID of widget to be unisntalled
        * @return void
        */
        void DeregisterWidgetL( const TUid& aUid );


    protected:  // constructors

        /**
        * C++ Constructor.
        */
        CWidgetRegistrationManager( RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:

       RFs 				iFs; //Not owned
    };

#endif
