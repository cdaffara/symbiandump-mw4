/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class definition file for an Application class
*
*/


#ifndef WIDGETSTARTUPAPPLICATION_H
#define WIDGETSTARTUPAPPLICATION_H

// INCLUDES


#include <aknapp.h>  	// CEikApplication

// DATA TYPES

const TUid KUidWidgetStartupApp = {0x200100B6};

// FORWARD DECLARATIONS

// CLASS DEFINITIONS

/**
*  Defines the application's UID and manufactures a new,
*  blank document.
*
*  @lib WidgetStartup.exe
*  @since Series 60 3.0
*/
class CWidgetStartupApplication : public CAknApplication
    {
	// Methods from base classes

    private: 	// CEikApplication
        /**
        * This function is inherited from class CApaApplication. This function is called by
        * the UI framework at application start-up. It creates an instance of the document class.
        * @param None
        * @return pointer to a CApaDocument object
        */
        CApaDocument* CreateDocumentL();

        /**
        * The function is called by the UI framework to ask for the	application's UID.
        * @param None
        * @return the UID of the application
        */
        TUid AppDllUid() const;
    };

#endif		// WIDGETSTARTUPAPPLICATION_H

// End of File
