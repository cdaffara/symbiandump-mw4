/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*      Entry point and main function for Bookmark Import utility application.
*      
*
*/


// INCLUDE FILES

#include <bacline.h>
#include "BookmarkFileImporter.h"

// ================= LOCAL FUNCTIONS =======================

/**
* Main function. Process command line arguments and import bookmark data.
*/
LOCAL_D void MainL()
    {
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    if( args->Count() != 2 )
        {
        // Expecting command line 'Appname infile'.
        User::Leave( KErrArgument );
        }
    CBookmarkFileImporter* importer = CBookmarkFileImporter::NewL();
    CleanupStack::PushL( importer );
    importer->ImportL(  args->Arg( 1 ) );
    CleanupStack::PopAndDestroy( 2 );   // importer, args
    }

/**
* Entry point to the application.
* @return Error code.
*/
GLDEF_C TInt E32Main()
    {
    TInt err = KErrNone;
    CTrapCleanup* trapCleanup = CTrapCleanup::New();
    if ( trapCleanup )
        {
        TRAP( err, MainL() );
        delete trapCleanup;
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;
    }
