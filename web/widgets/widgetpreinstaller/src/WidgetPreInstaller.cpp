/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Pre installs widgets on system startup
*
*/

#include <SWInstUIPluginAPI.h>
#include <centralrepository.h>
#include "WidgetInstaller.h"
#include "SWInstWidgetUid.h"
#include "WidgetPreInstaller.h"
#include "WidgetInstallerInternalCRKeys.h"

//  CONSTANTS
_LIT(KWidgetPreInstallPath, "z:\\data\\WidgetPreInstall\\");
    
// ============================================================================
// CWidgetPreInstaller::ConstructL()
// Constructs and start installs
//
// @since 3.1
// ============================================================================
//
void CWidgetPreInstaller::ConstructL()
    {

    BaseConstructL( ENonStandardResourceFile | ENoScreenFurniture  );
        
    CRepository* cenRep(NULL);
    TBool preInstall(EFalse);
  	
  	TRAPD(cenRepError, cenRep = CRepository::NewL( TUid::Uid( KSWInstWidgetUIUid ) ));
  	if (!cenRepError)
  		{
  		CleanupStack::PushL(cenRep);
  		TInt preInstallVal;
        if (cenRep->Get( KWidgetPreInstall, preInstallVal ) == KErrNone)
            {
            preInstall = (preInstallVal == 1);
            }
  		}

    // Start the installs
    if (preInstall)
        {
        DoInstallL();
        }
    
    if (!cenRepError)
        {
        // reset since we only do this once
        cenRep->Set(KWidgetPreInstall,0);
        CleanupStack::PopAndDestroy(); // cenRep
        }
    
    Exit();
    }

// ============================================================================
// CWidgetPreInstaller::CWidgetPreInstaller()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetPreInstaller::CWidgetPreInstaller()
    {
    }
  
    
// -----------------------------------------------------------------------------
// CWidgetPreInstaller::~CWidgetPreInstaller
// C++ destructor
// 
// -----------------------------------------------------------------------------
//
CWidgetPreInstaller::~CWidgetPreInstaller()
    {
    iFs.Close();
    }

// ============================================================================
// CWidgetPreInstaller::DoInstallL
// Perform installation.
// (other items were commented in a header).
//
// ============================================================================
//
void CWidgetPreInstaller::DoInstallL()
    {
    CDir* dir = NULL;
    User::LeaveIfError( iFs.Connect() );
    
    if ( iFs.GetDir( KWidgetPreInstallPath, KEntryAttMatchMask, EDirsFirst, dir ) == KErrNone )
        {
        CleanupStack::PushL(dir);
        
        CWidgetInstaller* installer = NULL;
        TRAPD( err , installer = CWidgetInstaller::NewL() );
        User::LeaveIfError( err );
        if ( !installer )
            {
            User::Leave( KErrGeneral );
            }
        CleanupStack::PushL(installer);
        
        for (TInt i=0; i<dir->Count(); i++)
            {
            const TEntry& fileEntry = (*dir)[i];
            
            // check for directory entries, i.e. "com.nokia.widget"
            if (fileEntry.IsDir())
                {
                // e.g.  restorePath = "\data\WidgetPreInstall\com.nokia.widget"
                TFileName restorePath( KWidgetPreInstallPath );
                restorePath.Append(fileEntry.iName);

                // installer will move the restorePath to widgetUI private area
                TRAPD( err, installer->InstallL( restorePath ) );
                if ( err )
                    {
                    installer->RunError( err );
                    if ( err == KErrNoMemory || err == KErrDiskFull )
                        {
                        User::Leave( err );
                        }               
                    }
                } // end of if (fileEntry
            else
                {
                // only care about dir
                break;
                }
            }  // end of for (
        
        CleanupStack::PopAndDestroy(2);  // dir, installer

        }  // end of if ( iFs.GetDir
    }

// End of File
