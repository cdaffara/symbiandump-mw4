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
*      Implementation of class CSchemeAppUi.   
*      
*
*/

#include "SchemeDocument.h"
#include "SchemeAppLogger.h"
#include "SchemeDialog.h" 
#include <SchemeHandler.h>
#include "SchemeAppUi.h"
#include "Scheme.hrh"
#include <schemeapp.rsg>
#include <apgcli.h>
#include <avkon.hrh>
#include <apparc.h>

#include <e32std.h>
#include <s32mem.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CSchemeAppUi::CSchemeAppUi()
// ----------------------------------------------------------
//
CSchemeAppUi::CSchemeAppUi( CSchemeDocument* aSchemeDocument )
    {
    CLOG_ENTERFN( "CSchemeAppUi::CSchemeAppUi" );
    iSchemeDocument = aSchemeDocument;
    CLOG_LEAVEFN( "CSchemeAppUi::CSchemeAppUi" );
    }

// ----------------------------------------------------------
// CSchemeAppUi::ConstructL()
// ----------------------------------------------------------
//
void CSchemeAppUi::ConstructL()
    {
    CLOG_ENTERFN( "CSchemeAppUi::ConstructL" );
    // SKAI-6YUEEA EAknEnableMSK is needed, EAknEnableSkin should be given as well
    // in case the app wants to enable default skin parameters for the Avkon controls 
    BaseConstructL( EAknEnableSkin | EAknEnableMSK ); 
    iAppDialog = new (ELeave) CSchemeDialog;
    iAppDialog->SetMopParent( this );
    iAppDialog->ExecuteLD( R_SCHEME_DIALOG );
    CLOG_LEAVEFN( "CSchemeAppUi::ConstructL" );
    }

// ----------------------------------------------------------
// CSchemeAppUi::~CSchemeAppUi()
// ----------------------------------------------------------
//
CSchemeAppUi::~CSchemeAppUi()
    {
	CLOG_ENTERFN( "CSchemeAppUi::~CSchemeAppUi" );

    if( iDoorObserver )
        {
        iDoorObserver->NotifyExit( MApaEmbeddedDocObserver::ENoChanges );
        }

    if( iSchemeHandler )
        {
        delete iSchemeHandler;
        }

    delete iAppDialog;

    CLOG_LEAVEFN( "CSchemeAppUi::~CSchemeAppUi" );
    }

// ----------------------------------------------------------
// CSchemeAppUi::ProcessCommandParametersL()
// ----------------------------------------------------------
//
TBool CSchemeAppUi::ProcessCommandParametersL( TApaCommand aCommand, 
                                               TFileName& aDocumentName, 
                                               const TDesC8& aTail )
	{
    CLOG_ENTERFN( "CSchemeAppUi::ProcessCommandParametersL" );
	if ( aCommand == EApaCommandCreate )
        {
        CLOG_WRITE( "EApaCommandCreate" );
		iSchemeHandler = CSchemeHandler::NewL( aDocumentName );
	    iSchemeHandler->Observer( iSchemeDocument );
        

        if( aTail.Length() != 0 )
            {
            CAiwGenericParamList* paramsList = CAiwGenericParamList::NewLC();
            CBufFlat* store = CBufFlat::NewL( aTail.Length() );
            CleanupStack::PushL( store );
            store->InsertL( 0, aTail );

	        RBufReadStream inStream;
            inStream.Open( *store );
            CleanupClosePushL<RBufReadStream>( inStream );

            paramsList->InternalizeL( inStream );
            iSchemeHandler->SetParameterList( paramsList ); // Takes ownership
            CleanupStack::PopAndDestroy( 2 );  // store, close inStream
            CleanupStack::Pop( paramsList ); //paramsList
            }

		TRAP_IGNORE( iSchemeHandler->HandleUrlEmbeddedL() );
        }

    CLOG_LEAVEFN( "CSchemeAppUi::ProcessCommandParametersL" );

	return CEikAppUi::ProcessCommandParametersL
                      ( aCommand, aDocumentName, aTail );
	}

// ----------------------------------------------------------
// CSchemeAppUi::HandleCommandL()
// ----------------------------------------------------------
//
void CSchemeAppUi::HandleCommandL( TInt aCommand )
    {
    CLOG_ENTERFN( "CSchemeAppUi::HandleCommandL" );

    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            Exit();
            break;
            }

        default:
            {
            break;
            }
        }

    CLOG_LEAVEFN( "CSchemeAppUi::HandleCommandL" );
    }
    
// ---------------------------------------------------------
// CBrowserAppUi::HandleScreenDeviceChangedL()
// ---------------------------------------------------------
//
void CSchemeAppUi::HandleScreenDeviceChangedL()
    {
    CEikAppUi::HandleScreenDeviceChangedL();
    }

// End of File
    

// End of File  
