/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available 
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  CUpnpHttpServerTransactionDescription implementation.
 *
 */
#include <uri8.h> 
#include "upnphttpservertransactiondescription.h"
#include "upnphttpservertransactionhandler.h"
#include "upnperrors.h"
#include "upnphttpmessage.h"
#include "upnpstring.h"


// ---------------------------------------------------------------------------
// CUpnpHttpServerTransactionDescription::NewL
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpServerTransactionDescription* CUpnpHttpServerTransactionDescription::NewL( 
        CUpnpHttpServerTransactionHandler& aClientContext, const TInetAddr& aSender,
        const TDesC8& aUri )
    {
    CUpnpHttpServerTransactionDescription* self = 
        new (ELeave) CUpnpHttpServerTransactionDescription( aClientContext );
    CleanupStack::PushL( self );
    self->ConstructL( aSender, aUri );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransactionDescription::CUpnpHttpServerTransactionDescription
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpServerTransactionDescription::CUpnpHttpServerTransactionDescription( 
        CUpnpHttpServerTransactionHandler& aClientContext )
    : iClientContext( aClientContext )    
    {    
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransactionDescription::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CUpnpHttpServerTransactionDescription::ConstructL( 
	const TInetAddr& aSender, const TDesC8& aUri )
    {
    iSender = aSender;
    iSenderUri = aUri.AllocL();	
    iDecodedUri.CreateL( aUri );
    }
        
// ---------------------------------------------------------------------------
// CUpnpHttpServerTransactionDescription::~CUpnpHttpServerTransactionDescription
// 
// ---------------------------------------------------------------------------
//    
CUpnpHttpServerTransactionDescription::~CUpnpHttpServerTransactionDescription()
    {
    delete iSenderUri;
    iDecodedUri.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransactionDescription::OnCallbackL
// 
// ---------------------------------------------------------------------------
//    
void CUpnpHttpServerTransactionDescription::OnCallbackL( TUpnpHttpServerEvent aEvent )
    {
    TRAPD( err, DoCallbackL(aEvent) );
    if ( err )
        {
        SetHttpCode( err );
        }
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransactionDescription::OnCallbackL
// 
// ---------------------------------------------------------------------------
//    
void CUpnpHttpServerTransactionDescription::DoCallbackL( TUpnpHttpServerEvent aEvent )
    {
    switch ( aEvent )
        {
        case EOnRequestStart:
            {            
            TInt error = DecodeUri();      
            if ( error )
                {
                SetHttpCode( error );
                }
            else
                {
                RFile file;                                                
                TInt error = iClientContext.GetFileL( file, *iSenderUri, iSender );
                if ( error )
                    {
                    SetHttpCode( error );
                    }
                else
                    {
                    SetDataSourceL( file );
                    }
                }    
            }
            break;
        case EOnComplete:
            break;  
        case EOnResponseStart:
            break;
        default:
            break;
        }            
    }


TInt CUpnpHttpServerTransactionDescription::DecodeUri( )
    {       
    TInt error = KErrNone;
    UpnpString::ReplaceHttpCharacters( iDecodedUri );    
    TUriParser8 up;
    TInt parseError = up.Parse( iDecodedUri );
    if ( parseError )
        {
        error = -EHttpBadRequest;
        }    
    return error;
    }


