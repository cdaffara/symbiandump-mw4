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
* Description: 
*
*/


#ifndef WIDGEUIDOCUMENT_H_
#define WIDGEUIDOCUMENT_H_

// INCLUDES

#include <AknDoc.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CEikAppUi;

// CLASS DECLARATION

/**
*
*  @lib
*  @since 3.1
*/
class CWidgetUiDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        * @since 3.1
        * @param aApp Pointer to the CEikApplication
        * @return CWidgetUiDocument*
        */
        static CWidgetUiDocument* NewL( CEikApplication& aApp );

        /**
        * ~CWidgetUiDocument
        * Destructor.
        * @since 3.1
        * @param none
        * @return none
        */
        virtual ~CWidgetUiDocument();

    private:

        /**
        * CWidgetUiDocument
        * @since 3.1
        * @param aApp Pointer to the CEikApplication
        * @return none
        */
        CWidgetUiDocument( CEikApplication& aApp );

    private:

        /**
        * CreateAppUiL
        * @since 3.1
        * @param none
        * @return CEikAppUi*
        */
        CEikAppUi* CreateAppUiL();
    };

#endif // WidgetUIDOCUMENT_H

// End of File





