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


#ifndef WIDGETPREINSTALLERDOCUMENT_H
#define WIDGETPREINSTALLERDOCUMENT_H

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
class CWidgetPreInstallerDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        * @since 3.1
        * @param aApp Pointer to the CEikApplication
        * @return CWidgetPreInstallerDocument*
        */
        static CWidgetPreInstallerDocument* NewL( CEikApplication& aApp );

        /**
        * ~CWidgetPreInstallerDocument
        * Destructor.
        * @since 3.1
        * @param none
        * @return none
        */
        virtual ~CWidgetPreInstallerDocument();

    private:

        /**
        * CWidgetPreInstallerDocument
        * @since 3.1
        * @param aApp Pointer to the CEikApplication
        * @return none
        */
        CWidgetPreInstallerDocument( CEikApplication& aApp );

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

#endif // WIDGETPREINSTALLERDOCUMENT_H

// End of File

