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
* Description:  
*
*/

#ifndef __HTTPDLCONNECTION_H__
#define __HTTPDLCONNECTION_H__

#include <e32base.h>
#include <http/rhttptransaction.h>
#include <http/mhttptransactioncallback.h>

class HttpSessionManager;

class HttpDlConnection : public MHTTPTransactionCallback
{
public:
    /**
    */
    HttpDlConnection();

    /**
    */
    static HttpDlConnection* dlConnectionFromTransaction(RHTTPTransaction& transaction);
    /**
    */

	long downloadId() {return m_dlId;}
	void setDownloadId(long id) { m_dlId = id; }

    public: // from MHTTPTransactionCallback
	void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent &aEvent) {}
	TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent &aEvent) {return ETrue;}

private: // data
	long m_dlId;
	HttpSessionManager* m_httpSessionMgr;
};

#endif // __HTTPDLCONNECTION_H__
// end of file
