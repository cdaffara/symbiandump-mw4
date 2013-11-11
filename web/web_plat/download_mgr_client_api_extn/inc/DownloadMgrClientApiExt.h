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



#ifndef __RHTTPDOWNLOADMGRAPIEXT_H__
#define __RHTTPDOWNLOADMGRAPIEXT_H__

#warning The Download Manager API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

#include "DownloadMgrClient.h"


/**
 *  API Extensions for RHttpDownloadMgr
 *
 *  This class was added to provide support for ProgressiveDownload
 *
 *  @lib DownloadMgr.lib
 *  @since S60 3.2
 */
class RHttpDownloadMgrApiExt : public RHttpDownloadMgr
    {

    public:
        /**
        * C++ default constructor.
        */
        IMPORT_C RHttpDownloadMgrApiExt();

    public:    // New functions

        /**
        * Attach to an existing download.
        * @param aDlId Download ID.
        * @return RHttpDownload&
        */
        IMPORT_C RHttpDownload& AttachToDownloadL( const TInt32 aDlId );

    };



#endif // __RHTTPDOWNLOADMGRAPIEXT_H__
