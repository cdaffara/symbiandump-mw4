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
*      Implementation of class Scheme Dispatcher.   
*      
*
*/


// INCLUDE FILES

#include <SchemeDefs.hrh>
#include <ecom/ecom.h>		// For REComSession

// ================= CONSTANTS =======================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSchemeHandler::NewL()
// ---------------------------------------------------------
//
CSchemeHandler* CSchemeHandler::NewL( const TDesC& aUrl )
{
	// retreiving the scheme
	const TUid KSchemeHandlerDefinitionUid = { SCHEME_INTERFACE_DEF };

	TInt colonPos = aUrl.Locate( ':' );
	if ( colonPos <= 0 )
		{
		// It should not happen since this char was found in DocumentHandler alredy
 		User::Leave( KErrArgument );
		}

	// Creating the right implementation
	TEComResolverParams resolverParams;

    HBufC8* scheme = HBufC8::NewLC( colonPos );
    scheme->Des().Append( aUrl.Left( colonPos ) );
    scheme->Des().LowerCase();
    resolverParams.SetDataType( scheme->Des() );
    resolverParams.SetGenericMatch( ETrue ); //To enable default scheme handlers

    TAny* ptr = REComSession::CreateImplementationL(KSchemeHandlerDefinitionUid, 
												    _FOFF( CSchemeHandler, iDtor_ID_Key ), 
												    (TAny*)&aUrl,
												    resolverParams);

	CleanupStack::PopAndDestroy(); // urlCopy
	
	CSchemeHandler* schemeHandler = REINTERPRET_CAST( CSchemeHandler*, ptr );

	return schemeHandler;
	}

// ---------------------------------------------------------
// CSchemeHandler::~CSchemeHandler()
// ---------------------------------------------------------
//
CSchemeHandler::~CSchemeHandler()
    {
    if( iParamList )
        {
        delete iParamList;
        }
	REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ---------------------------------------------------------
// CSchemeHandler::SetParameterList()
// ---------------------------------------------------------
//
void CSchemeHandler::SetParameterList(CAiwGenericParamList* aParamList) // Takes ownership
    {
    iParamList = aParamList;
    }

// Reserved functions
TInt CSchemeHandler::Reserved_1( TAny* /*aAny*/ )
	{
	return 0;
	}
TInt CSchemeHandler::Reserved_2( TAny* /*aAny*/ )
	{
	return 0;
	}
