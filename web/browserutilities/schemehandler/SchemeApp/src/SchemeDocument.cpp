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
*      Implementation of class CSchemeDocument.   
*      
*
*/


// INCLUDE FILES

#include "SchemeDocument.h"
#include "SchemeAppUi.h"
#include "SchemeHandler.h"
#include "SchemeAppLogger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSchemeDocument::NewL()
// ---------------------------------------------------------
//
CSchemeDocument* CSchemeDocument::NewL( CEikApplication& aApp )
    {
    CLOG_ENTERFN( "CSchemeDocument::NewL" );

    CSchemeDocument* doc = new (ELeave) CSchemeDocument( aApp );
    CleanupStack::PushL( doc );
    doc->ConstructL();
    CleanupStack::Pop(); // doc

    CLOG_LEAVEFN( "CSchemeDocument::NewL" );

    return doc;
    }
    
// ---------------------------------------------------------
// CSchemeDocument::~CSchemeDocument()
// ---------------------------------------------------------
//
CSchemeDocument::~CSchemeDocument()
    {
    CLOG_ENTERFN( "CSchemeDocument::~CSchemeDocument" );

    if( iShutdown )
        {
        delete iShutdown;
        }

    CLOG_LEAVEFN( "CSchemeDocument::~CSchemeDocument" );
    }

// ---------------------------------------------------------
// CSchemeDocument::CSchemeDocument()
// ---------------------------------------------------------
//
CSchemeDocument::CSchemeDocument( CEikApplication& aApp )
: CEikDocument( aApp )
    {
    }

// ---------------------------------------------------------
// CSchemeDocument::ConstructL()
// ---------------------------------------------------------
//
void CSchemeDocument::ConstructL()
    {
    }

// ---------------------------------------------------------
// CSchemeDocument::CreateAppUiL()
// ---------------------------------------------------------
//
CEikAppUi* CSchemeDocument::CreateAppUiL()
    {
    CLOG_ENTERFN( "CSchemeDocument::CreateAppUiL" );
	iSchemeAppUi = new (ELeave) CSchemeAppUi( this );
    CLOG_LEAVEFN( "CSchemeDocument::CreateAppUiL" );
    return iSchemeAppUi;
    }

// ---------------------------------------------------------
// CSchemeDocument::NotifyExit()
// ---------------------------------------------------------
//
void CSchemeDocument::HandleServerAppExit(TInt /*aReason*/ )
	{
    CLOG_ENTERFN( "CSchemeDocument::NotifyExit" );

    iShutdown = new CSchemeShutdown( iSchemeAppUi );
    if( iShutdown )
        {
        iShutdown->Start();
        }
    else
        {
        iSchemeAppUi->HandleCommandL( EEikCmdExit );
        }

    CLOG_LEAVEFN( "CSchemeDocument::NotifyExit" );
	}

// ---------------------------------------------------------
// CSchemeShutdown::CSchemeShutdown()
// ---------------------------------------------------------
//
CSchemeShutdown::CSchemeShutdown( CSchemeAppUi* aAppUi )
: CActive( EPriorityStandard )
    {
    CLOG_ENTERFN( "CSchemeDocument::CSchemeShutdown" );
    iAppUi = aAppUi;
    CActiveScheduler::Add( this );
    CLOG_LEAVEFN( "CSchemeDocument::CSchemeShutdown" );
    }

// ---------------------------------------------------------
// CSchemeShutdown::~CSchemeShutdown()
// ---------------------------------------------------------
//
CSchemeShutdown::~CSchemeShutdown()
    {
    CLOG_ENTERFN( "CSchemeDocument::~CSchemeShutdown" );
    Cancel();
    CLOG_LEAVEFN( "CSchemeDocument::~CSchemeShutdown" );
    }

// ---------------------------------------------------------
// CSchemeShutdown::Start()
// ---------------------------------------------------------
//
void CSchemeShutdown::Start()
    {
    CLOG_ENTERFN( "CSchemeDocument::Start" );
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    CLOG_LEAVEFN( "CSchemeDocument::Start" );
    }

// ---------------------------------------------------------
// CSchemeShutdown::RunL()
// ---------------------------------------------------------
//
void CSchemeShutdown::RunL()
    {
    CLOG_ENTERFN( "CSchemeDocument::RunL" );
    iAppUi->HandleCommandL( EEikCmdExit );
    CLOG_LEAVEFN( "CSchemeDocument::RunL" );
    }

// ---------------------------------------------------------
// CSchemeShutdown::DoCancel()
// ---------------------------------------------------------
//
void CSchemeShutdown::DoCancel()
    {
    }
