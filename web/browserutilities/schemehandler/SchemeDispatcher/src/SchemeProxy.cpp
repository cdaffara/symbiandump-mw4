/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of Scheme handler proxy table
*      
*
*/


//#include "FileHandler.h"
//#include "HttpHandler.h"
#include "RtspHandler.h"
//#include "MailToHandler.h"
//#include "WtaiHandler.h"
//#include "TelHandler.h"
//#include "CtiHandler.h"
#include "MmsHandler.h"
#include <e32std.h>
#include <SchemeDefs.hrh>
#include <implementationproxy.h>

// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
//		IMPLEMENTATION_PROXY_ENTRY( SCHEME_FILE_IMP,	CFileHandler::NewL ),
//		IMPLEMENTATION_PROXY_ENTRY( SCHEME_HTTP_IMP,	CHttpHandler::NewL ),
//		IMPLEMENTATION_PROXY_ENTRY( SCHEME_HTTPS_IMP,   CHttpHandler::NewL ),
		IMPLEMENTATION_PROXY_ENTRY( SCHEME_RTSP_IMP,	CRtspHandler::NewL ),
//		IMPLEMENTATION_PROXY_ENTRY( SCHEME_MAIL_TO_IMP,	CMailToHandler::NewL ),
//		IMPLEMENTATION_PROXY_ENTRY( SCHEME_WTAI_IMP,	CWtaiHandler::NewL ),
//		IMPLEMENTATION_PROXY_ENTRY( SCHEME_TEL_IMP,	CTelHandler::NewL ),
//		IMPLEMENTATION_PROXY_ENTRY( SCHEME_CTI_IMP,	CCtiHandler::NewL ),
		IMPLEMENTATION_PROXY_ENTRY( SCHEME_MMS_IMP,		CMmsHandler::NewL )
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
