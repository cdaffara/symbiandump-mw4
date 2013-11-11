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
* Description:  API extensions for RHttpDownloadMgr
*
*/


#include "DownloadMgrClientApiExt.h"
#include "DownloadMgrLogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// RHttpDownloadMgrApiExt::AttachToDownloadL
// ---------------------------------------------------------
//
EXPORT_C RHttpDownload& RHttpDownloadMgrApiExt::AttachToDownloadL( const TInt32 aDlId )
    {
    CLOG_ENTERFN( "RHttpDownloadMgr::CreateDownloadL" )
    
    if( 0 >= aDlId )
        {
        User::Leave( KErrBadHandle );
        }

    RHttpDownload* download = new (ELeave) RHttpDownload ( (RHttpDownloadMgr*)this );
    CleanupStack::PushL( download );
    CleanupClosePushL( *download );

    download->AttachToDownloadL( aDlId );
    
    RHttpDownload& item = AddToArrayL( download );

    CleanupStack::Pop( 2 );  // download->Close, download
   
    return item;    
    }

EXPORT_C RHttpDownloadMgrApiExt::RHttpDownloadMgrApiExt()
	{
    // nothing to do
	}
