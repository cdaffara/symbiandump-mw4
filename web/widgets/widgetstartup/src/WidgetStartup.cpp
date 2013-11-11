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
* Description:  Launches startup widget applications
*
*/

#include <apacmdln.h>           // For CApaCommandLine
#include <apgcli.h>             // For RApaLsSession
#include "WidgetStartup.h"

//  CONSTANTS
_LIT(KPreInstallerExe,"WidgetPreInstaller.exe");

// ============================================================================
// CWidgetStartup::ConstructL()
// Constructs and start installs
//
// @since 3.1
// ============================================================================
//
void CWidgetStartup::ConstructL()
    {    
    BaseConstructL( ENonStandardResourceFile | ENoScreenFurniture  );
    
    DoPreInstallL();
    
    Exit();
    }

// ============================================================================
// CWidgetStartup::CWidgetStartup()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetStartup::CWidgetStartup()
    {
    }
  
    
// -----------------------------------------------------------------------------
// CWidgetStartup::~CWidgetStartup
// C++ destructor
// 
// -----------------------------------------------------------------------------
//
CWidgetStartup::~CWidgetStartup()
    {
    }

// ---------------------------------------------------------------------------
// Execute startup functionality.
// ---------------------------------------------------------------------------
//
void CWidgetStartup::DoPreInstallL()
    {
    RApaLsSession appArcSession;
    CleanupClosePushL(appArcSession);
    CApaCommandLine* cmdLine=CApaCommandLine::NewLC();
    cmdLine->SetCommandL(EApaCommandBackground); 
    cmdLine->SetExecutableNameL( KPreInstallerExe );
    
    appArcSession.StartApp( *cmdLine ); 
    CleanupStack::PopAndDestroy(2); // cmdLine, appArcSession
    }
// End of File
