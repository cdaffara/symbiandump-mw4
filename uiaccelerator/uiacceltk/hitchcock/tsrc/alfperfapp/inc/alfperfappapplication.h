/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  alfperfapp application class definition.
*
*/


#ifndef C_ALFPERFAPPAPPLICATION_H
#define C_ALFPERFAPPAPPLICATION_H

#include <aknapp.h>
#include "alfperfapp.hrh"

/**
 * UID of this application.
 */
const TUid KAlfPerfAppUid = { KAlfPerfAppUidValue };

/**
 * CAlfPerfAppApplication application class. 
 */
class CAlfPerfAppApplication : public CAknApplication 
    {
private: // from CApaApplication

    // From base class CAknApplication 

    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;

    };

#endif // C_ALFPERFAPPAPPLICATION_H
            
// End of File
