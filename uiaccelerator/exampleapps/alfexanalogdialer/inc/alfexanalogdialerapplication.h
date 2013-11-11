/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Application class for Analog Dialer example application, which
*                implements rotating dial pad.
*
*/



#ifndef ANALOGDIALER_APPLICATION_H
#define ANALOGDIALER_APPLICATION_H

#include <aknapp.h>

/**
 *  Application class for Analog Dialer
 *
 *  @since S60 5.0
 */
class CAlfExAnalogDialerApplication : public CAknApplication
    {

public:


    /* Methods. */

// from base class CAknApplication

    /**
     * From CAknApplication
     * Returns the application UID
     *
     * @result the UID of this Application/Dll
     */
    TUid AppDllUid() const;

// from base class ?base_class2



protected:


// from base class CAknApplication

    /**
     * From CAknApplication
     * Create a CApaDocument object and return a pointer to it
     *
     * @result a pointer to the created document
     */
    CApaDocument* CreateDocumentL();

    };

#endif // ANALOGDIALER_APPLICATION_H
