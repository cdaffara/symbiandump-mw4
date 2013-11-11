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
* Description:  Feeds application instance.
*
*/


#ifndef FEEDS_APPLICATION_H
#define FEEDS_APPLICATION_H


// INCLUDES
#include <aknapp.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Feeds application instance.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CFeedsApplication: public CAknApplication
    {
    public:  // Constructor and destructor
        /**
        * Constructor
        */        
        CFeedsApplication();
        
        /**
        * Destructor.
        */        
        virtual ~CFeedsApplication();


    public:  // From CAknApplication
        /**
        * Returns application's UID.
        *
        * @since 3.0
        * @return the UID.
        */
        TUid AppDllUid() const;



    protected: // from CAknApplication
        /**
        * Creates CFeedsDocument document object.
        *
        * @since 3.0
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif      // FEEDS_APPLICATION_H
            
// End of File
