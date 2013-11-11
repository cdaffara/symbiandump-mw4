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


#ifndef WIDGETSTARTUPDOCUMENT_H
#define WIDGETSTARTUPDOCUMENT_H

// INCLUDES

#include <akndoc.h>

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
class CWidgetStartupDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        * @since 3.1
        * @param aApp Pointer to the CEikApplication
        * @return CWidgetStartupDocument*
        */
        static CWidgetStartupDocument* NewL( CEikApplication& aApp );

        /**
        * ~CWidgetStartupDocument
        * Destructor.
        * @since 3.1
        * @param none
        * @return none
        */
        virtual ~CWidgetStartupDocument();

    private:

        /**
        * CWidgetStartupDocument
        * @since 3.1
        * @param aApp Pointer to the CEikApplication
        * @return none
        */
        CWidgetStartupDocument( CEikApplication& aApp );

        /**
        * ConstructL
        * @since 3.1
        * @param none
        * @return void
        */
        void ConstructL();

    private:

        /**
        * CreateAppUiL
        * @since 3.1
        * @param none
        * @return CEikAppUi*
        */
        CEikAppUi* CreateAppUiL();
    };

#endif // WIDGETSTARTUPDOCUMENT_H

// End of File

