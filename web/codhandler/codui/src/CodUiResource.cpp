/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCodUiResource.   
*
*/


// INCLUDE FILES

#include <eikenv.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

#include "CodUiResource.h"
#include "CodLogger.h"

// CONSTANTS

/// CodUi resource file name.
_LIT( KCodUiResourceFile, "CodUi.rsc" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodUiResource::NewL
// ---------------------------------------------------------
//
CCodUiResource* CCodUiResource::NewL( CEikonEnv& aEikEnv )
    {
    CCodUiResource* ui = new ( ELeave ) CCodUiResource( aEikEnv );
    CleanupStack::PushL( ui );
    ui->ConstructL();
    CleanupStack::Pop( ui ); 
    return ui;    
    }

// ---------------------------------------------------------
// CCodUiResource::~CCodUiResource
// ---------------------------------------------------------
//
CCodUiResource::~CCodUiResource()
    {    
    CLOG(( 2, _L("*** -> CCodUiResource::~CCodUiResource") ));
    if ( iResourceOffset )
        {
        iEikEnv->DeleteResourceFile( iResourceOffset );
        }
    CLOG(( 2, _L("*** <- CCodUiResource::~CCodUiResource") ));
    }

// ---------------------------------------------------------
// CCodUiResource::CCodUiResource
// ---------------------------------------------------------
//
CCodUiResource::CCodUiResource( CEikonEnv& aEikEnv )
: iEikEnv( &aEikEnv )
    {
    CLOG(( 2, _L("*** CCodUiResource::CCodUiResource") ));
    }

// ---------------------------------------------------------
// CCodUiResource::ConstructL
// ---------------------------------------------------------
//
void CCodUiResource::ConstructL()
    {
    TFileName resourceFile;
    resourceFile.Append( TDriveUnit( EDriveZ ).Name() );
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KCodUiResourceFile );
    BaflUtils::NearestLanguageFile( iEikEnv->FsSession(), resourceFile );
    iResourceOffset = iEikEnv->AddResourceFileL( resourceFile );
    }
