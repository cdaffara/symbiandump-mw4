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
* Description: 
*      Declaration of class CRoapApp.   
*      
*
*/


#ifndef ROAP_APP_H
#define ROAP_APP_H

// INCLUDES

#include <e32base.h>
#include <aknapp.h>

// CLASS DECLARATION

/**
* ROAP Application.
*/
class CRoapApp: public CAknApplication
    {
    
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

#endif /* def ROAP_APP_H */
