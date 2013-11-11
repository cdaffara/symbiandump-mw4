/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Application definition
*
*/



#ifndef C_ALFAPPLICATION_H
#define C_ALFAPPLICATION_H

#include <aknapp.h>

/**
 *  Application class for Alfred app server.
 *
 *  @since S60 v3.2
 */
class CAlfApplication : public CAknApplication
    {
public:

// from base class CAknApplication

    /**
     * From CAknApplication.
     * Called by the framework when new server is launched.
     *
     * @param aAppServer Pointer to the server class which this 
     *                   implementation allocates and returns the
     *                   ownership.
     */
    void NewAppServerL(CApaAppServer*& aAppServer);
    
    /**
     * From CAknApplication.
     * Called by the system when a document is created.
     *
     * @return New document instance. Ownership transferred.
     */
    CApaDocument* CreateDocumentL(); 
    
    /**
     * From CAknApplication.
     * Returns the component UID.
     *
     * @return UID3 of the app server.
     */
    TUid AppDllUid() const;

    };

#endif // C_ALFAPPLICATION_H
