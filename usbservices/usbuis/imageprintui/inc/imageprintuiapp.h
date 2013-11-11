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
* Description:  Header file for imageprintuiapp 
*
*/


#ifndef CIMAGEPRINTUIAPP_H
#define CIMAGEPRINTUIAPP_H

//  EXTERNAL INCLUDES
#include <e32base.h>
#include <aknapp.h>

//  CONSTANTS
/**
* UID of the application
*/
const TUid KUidImagePrint = { 0x1020E470 };
const TUid KUidUSB = { 0x102068E2 }; 

//  CLASS DEFINITION
/**
 * S60 Application class
 */
class CImagePrintUiApp : public CAknApplication
	{

private:	// Constructors and destructors

    /**
     * From CAknApplication, creates CImagePrintUiDocument document
     * object.
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();

private:	// Methods derived from CApaApplication

    /**
     * From CAknApplication, returns application's UID.
     * @return The value of KUidImagePrint.
     */
    TUid AppDllUid() const;

    };

#endif // IMAGEPRINTUIAPP_H
