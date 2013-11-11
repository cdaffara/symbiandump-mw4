/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Supports common base functionality for DMgr user interfaces
*
*/



// INCLUDE FILES
#include    "CDownloadMgrUiBase.h"
#include    "UiLibLogger.h"
#include    <e32std.h>
#include    <e32def.h>
#include    <coemain.h>
#include    <data_caging_path_literals.hrh>
#include    <f32file.h>

// CONSTANTS
/// DLL rsource file name with path
_LIT( KUiLibResourceFileName, "\\resource\\DownloadMgrUiLib.rsc" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDownloadMgrUiBase::CDownloadMgrUiBase
// -----------------------------------------------------------------------------
//
CDownloadMgrUiBase::CDownloadMgrUiBase( CDownloadMgrUiLibRegistry& aRegistryModel )
:   iCoeEnv( *CCoeEnv::Static() ), 
    iRegistryModel( aRegistryModel ),
    iResourceFileName( KNullDesC ), 
    iResourceLoader( iCoeEnv ), 
    iResourceOpened( EFalse ),
    iExtension( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiBase::BaseConstructL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiBase::BaseConstructL()
    {
    TParse* fileParser = new (ELeave) TParse;
    CleanupStack::PushL( fileParser );
    
    Dll::FileName (iResourceFileName);
    
    fileParser->Set(KUiLibResourceFileName, &iResourceFileName, NULL);
    iResourceFileName = fileParser->FullName(); 
    iResourceLoader.OpenL( iResourceFileName );
    iResourceOpened = ETrue;
    
    CleanupStack::PopAndDestroy( fileParser ); // fileParser
    }

// Destructor
CDownloadMgrUiBase::~CDownloadMgrUiBase()
    {
    if ( iResourceOpened )
        {
        iResourceLoader.Close();
        iResourceOpened = EFalse;
        }
    //iExtension is not used yet.
    //delete iExtension;
    iExtension = NULL;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiBase::ResourceFileName
// -----------------------------------------------------------------------------
//
TPtrC CDownloadMgrUiBase::ResourceFileName() const
    {
    return (iResourceFileName);
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiBase::SetServerAppExitObserver
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiBase::SetServerAppExitObserver( MAknServerAppExitObserver* aObserver )
    {
    CLOG_WRITE_FORMAT("aObserver: 0x%x",aObserver);
    iServerAppExitObserver = aObserver;
    }

// End of file.
