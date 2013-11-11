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
* Description:  Declaration of class CCodViewerApp.   
*
*/


#ifndef COD_VIEWER_APP_H
#define COD_VIEWER_APP_H

// INCLUDES

#include <e32base.h>
#include <aknapp.h>

// CLASS DECLARATION

/**
* COD Handler Application.
*/
NONSHARABLE_CLASS( CCodViewerApp ): public CAknApplication
    {
    public:     // Constructors and destructor

        /**
        * Constructor.
        */      
        IMPORT_C CCodViewerApp();

    private:    // from CAknApplication (CApaApplication)

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

#endif /* def COD_VIEWER_APP_H */
