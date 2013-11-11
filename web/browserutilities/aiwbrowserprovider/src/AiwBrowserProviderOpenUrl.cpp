/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Providing open url functionality
*
*/


// INCLUDE FILES

#include "AiwBrowserProviderOpenUrl.h"

#include <AiwMenu.h>
#include <coemain.h>
#include <barsread.h>
#include <aknnotedialog.h>
#include <StringLoader.h>
#include <AiwBrowserProvider.rsg>
#include <data_caging_path_literals.hrh> 
#include <aknnotewrappers.h> 
#include <aknnotedialog.h>
#include <apgcli.h>
#include <FavouritesLimits.h>
#include "AiwBrowserCommands.hrh"
#include "logger.h"

// LOCAL CONSTANTS AND MACROS
_LIT( KResourceFileName, "AiwBrowserProvider.rsc");

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewL 
// Constructor of CAiwBrowserProviderOpenUrl
// Returns:
// 
// -----------------------------------------------------------------------------
//
CAiwBrowserProviderOpenUrl* CAiwBrowserProviderOpenUrl::NewL()
    {
	CAiwBrowserProviderOpenUrl* self = new(ELeave) CAiwBrowserProviderOpenUrl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CAiwBrowserProviderOpenUrl
// Constructor of CAiwBrowserProviderOpenUrl
// Returns:
// 
// -----------------------------------------------------------------------------
//
CAiwBrowserProviderOpenUrl::CAiwBrowserProviderOpenUrl()
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// Constructor of CAiwBrowserProviderOpenUrl
// Returns:
// 
// -----------------------------------------------------------------------------
//
void CAiwBrowserProviderOpenUrl::ConstructL()
    {
    TFileName fName(  KNullDesC );
    iResourceOffset = -1;
    fName.Append( _L("Z:") );
    fName.Append( KDC_RESOURCE_FILES_DIR );
    fName.Append( KResourceFileName );
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL( fName );
    LOG_WRITE("AIWBP: OpenUrl Resource file added.");    
    }

// -----------------------------------------------------------------------------
// ~CAiwBrowserProviderOpenUrl
// Destructor of CAiwBrowserProviderOpenUrl
// Returns:
// 
// -----------------------------------------------------------------------------
//
CAiwBrowserProviderOpenUrl::~CAiwBrowserProviderOpenUrl()
    {
    if( iResourceOffset != -1 )
        {
    	CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        iResourceOffset = -1;
        }
    }

// -----------------------------------------------------------------------------
// InitialiseL 
// aiw initialization
// Returns: none
//                    
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderOpenUrl::InitialiseL(MAiwNotifyCallback& aFrameworkCallback, 
                                       const RCriteriaArray& aInterest)
    {
    iNotifyCallback = &aFrameworkCallback;
    iInterest = &aInterest;
    }

// -----------------------------------------------------------------------------
// HandleServiceCmdL 
// 
// Returns: 
//         
//         
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderOpenUrl::HandleServiceCmdL
        (const TInt& /*aCmdId*/, const CAiwGenericParamList& /*aInParamList*/,
         CAiwGenericParamList& /*aOutParamList*/, TUint /*aCmdOptions*/,
         const MAiwNotifyCallback* /*aCallback*/)
    {
    }

// -----------------------------------------------------------------------------
// InitializeMenuPaneL 
// initialization of menu pane 
// Returns: none
//          
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderOpenUrl::InitializeMenuPaneL
        (CAiwMenuPane& aMenuPane, TInt aIndex,
         TInt /*aCascadeId*/, const CAiwGenericParamList& /*aInParamList*/)
    {
    TResourceReader reader;    
    CCoeEnv::Static()->CreateResourceReaderLC
        (reader, R_AIW_BROWSER_PROVIDER_OPEN_URL_MENU);
    aMenuPane.AddMenuItemsL(reader, EBrowserAiwCmdOpenUrl, aIndex);//open url
    CleanupStack::PopAndDestroy(); // reader
    LOG_WRITE("AIWBP: OpenUrl menupane initialized.");
    }

// -----------------------------------------------------------------------------
// HandleMenuCmdL
// handles menu commands
// Returns: none
//          
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderOpenUrl::HandleMenuCmdL
        (TInt aMenuCmdId, const CAiwGenericParamList& aInParamList,
         CAiwGenericParamList& /*aOutParamList*/, TUint /*aCmdOptions*/,
         const MAiwNotifyCallback* /*aCallback*/)
    {
    if ( aMenuCmdId == EBrowserAiwCmdOpenUrl )
        {
        TInt index = 0;
        const TAiwGenericParam* genericParam = NULL;
        TPtrC paramtext;
        index = 0;
        genericParam = aInParamList.FindFirst( index, 
                                    EGenericParamURL, 
                                    EVariantTypeDesC);
        LOG_WRITE("AIWBP: LaunchBrowser called with url:");
        LOG_WRITE_TDES( genericParam->Value().AsDes() );
        if( index == KErrNotFound )
            {
            LOG_WRITE("AIWBP: Open Url Parameter not found!" );
            return;
            }
	    LaunchBrowserL( genericParam->Value().AsDes() );
        }
    }

// -----------------------------------------------------------------------------
// LaunchBrowser 
// Launches a browser with an url
// Returns: none  
//          
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderOpenUrl::LaunchBrowserL( const TDesC& aUrl )
{
const TInt KBrowserUid = 0x10008D39;
HBufC* param = HBufC::NewLC( KFavouritesMaxUrl );
param->Des().Copy( _L( "4 " ) );
param->Des().Append( aUrl );
TUid id( TUid::Uid( KBrowserUid ) );
TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
TApaTask task = taskList.FindApp( id );

LOG_WRITE("AIWBP: Browser launched with url:" );
LOG_WRITE_TDES( param->Des() ); 

if ( task.Exists() )
    {
    HBufC8* param8 = HBufC8::NewLC( param->Length() );
    param8->Des().Append( *param );
    task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
    CleanupStack::PopAndDestroy();
    }
else 
    {
    RApaLsSession appArcSession;
    User::LeaveIfError( appArcSession.Connect() ); // connect to AppArc server
    CleanupClosePushL( appArcSession );
    TThreadId id;
    User::LeaveIfError
        ( appArcSession.StartDocument( *param, TUid::Uid( KBrowserUid ), id ) );
    CleanupStack::PopAndDestroy( &appArcSession );
    }

CleanupStack::PopAndDestroy(); // param
}
