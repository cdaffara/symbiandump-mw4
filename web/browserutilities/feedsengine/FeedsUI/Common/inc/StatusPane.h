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
* Description:  Status-pane related methods
*
*/


#ifndef STATUS_PANE_H
#define STATUS_PANE_H


// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class StatusPane
    {
    public:
        /**
        * Sets the title.
        *
        * @since 3.0
        * @param aStringId The resource id of the new title..
        * @return void.
        */
        static void SetTitleL(TInt aStringId);

        /**
        * Sets the title.
        *
        * @since 3.0
        * @param aString The value of the new title.
        * @return void.
        */
        static void SetTitleL(const TDesC& aString);

    };

#endif      // STATUS_PANE_H
            
// End of File
