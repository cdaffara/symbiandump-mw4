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
* Description:  Declaration of class CSchemeApp
*
*/


#ifndef SCHEME_APP_H
#define SCHEME_APP_H

// INCLUDES

#include <e32base.h>
#include <eikapp.h>
#include <SchemeDefs.hrh>

// CONSTANTS

// UID of the application.
LOCAL_D const TUid KSchemeApp = { SCHEME_APP_UID };

// CLASS DECLARATION

/**
* Application for Scheme App
*/
class CSchemeApp : public CEikApplication
    {
    private:    // from CEikApplication (CApaApplication)

        /**
        * Create document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * Return application's UID.
        * @return Application UID.
        */
        TUid AppDllUid() const;

    };

#endif // SCHEME_APP_H

// End of File

