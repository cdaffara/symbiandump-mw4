/*
* Copyright (c) 2006, 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WIDGETUIAPP_H_
#define WIDGETUIAPP_H_

// INCLUDES

#include <aknapp.h>

// CONSTANTS

const TUid KUidWidgetUi = { 0x10282822 };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CWidgetUiApp
*
*  @lib WidgetUI.exe
*  @since 3.1
*/
class CWidgetUiApp : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * OpenIniFileLC
        * @since 3.1
        * @param aFs file system reference
        * @return CDictionaryStore*
        */
        CDictionaryStore* OpenIniFileLC( RFs& aFs ) const;

        /**
        * ProcessCommandParametersL
        * @since 3.1
        * @param aCommand
        * @return TBool
        */
        TBool ProcessCommandParametersL( TApaCommand aCommand, 
                                         TFileName& /*aDocumentName*/,
                                         const TDesC8& /*aTail*/ );

        /**
        * ProcessCommandParametersL
        * @since 3.1
        * @param aDocumentName
        * @return none
        */
        void ProcessCommandParametersL( TFileName& aDocumentName );

    private:

        /**
        * CreateDocumentL
        * @since 3.1
        * @param none
        * @return CApaDocument*
        */
        CApaDocument* CreateDocumentL();

        /**
        * AppDllUid
        * @since 3.1
        * @param none
        * @return TUid
        */
        TUid AppDllUid() const;
    };

#endif // WIDGETUIAPP_H

// End of File





