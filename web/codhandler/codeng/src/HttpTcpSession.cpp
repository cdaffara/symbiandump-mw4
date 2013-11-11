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
*      Implementation of class CHttpTcpSession.   
*      
*
*/


// INCLUDE FILES

#include <httpfiltercommonstringsext.h>
#include "HttpTcpSession.h"
#include "CodLogger.h"
#include "CodPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CHttpTcpSession::NewL()
// ---------------------------------------------------------
//
CHttpTcpSession* CHttpTcpSession::NewL()
    {
    CHttpTcpSession* sess = new( ELeave ) CHttpTcpSession();
    CleanupStack::PushL( sess );
    sess->ConstructL();
    CleanupStack::Pop( sess );
    return sess;
    }

// ---------------------------------------------------------
// CHttpTcpSession::~CHttpTcpSession()
// ---------------------------------------------------------
//
CHttpTcpSession::~CHttpTcpSession()
    {
    CLOG(( ETcpConn, 2, _L("CHttpTcpSession::~CHttpTcpSession") ));
    iSess.Close();
    }

// ---------------------------------------------------------
// CHttpTcpSession::ConnectL()
// ---------------------------------------------------------
//
void CHttpTcpSession::ConnectL( TRequestStatus* aStatus )
    {
    CLOG(( ETcpConn, 2, _L("CHttpTcpSession::ConnectL") ));
    *aStatus = KRequestPending;
    User::RequestComplete( aStatus, KErrNone );
    }

// ---------------------------------------------------------
// CHttpTcpSession::Disconnect()
// ---------------------------------------------------------
//
void CHttpTcpSession::Disconnect()
    {
    CLOG(( ETcpConn, 2, _L("CHttpTcpSession::Disconnect") ));
    }

// ---------------------------------------------------------
// CHttpTcpSession::CHttpTcpSession()
// ---------------------------------------------------------
//
CHttpTcpSession::CHttpTcpSession(): CHttpSessionBase()
    {
    CLOG(( ETcpConn, 2, _L("CHttpTcpSession::CHttpTcpSession") ));
    }

// ---------------------------------------------------------
// CHttpTcpSession::ConstructL()
// ---------------------------------------------------------
//
void CHttpTcpSession::ConstructL() 
    {
    iSess.OpenL();
    iSess.StringPool().OpenL( HttpFilterCommonStringsExt::GetTable() );
    }

