/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CUpnpIconFileServeTransaction implementation.
*
*/


#include "upnpiconfileservetransaction.h"
#include <upnperrors.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnpiconfileservetransactionTraces.h"
#endif


// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpIconFileServeTransaction::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpIconFileServeTransaction* CUpnpIconFileServeTransaction::NewL( const TDesC& aOutFilePath,
                                                                       RFs& aIconFileSession )
       
    {
    OstTraceFunctionEntry0( CUPNPICONFILESERVETRANSACTION_NEWL_ENTRY );
    CUpnpIconFileServeTransaction* self = 
                     new (ELeave) CUpnpIconFileServeTransaction( aIconFileSession );
    CleanupStack::PushL( self );
    self->ConstructL( aOutFilePath );
    CleanupStack::Pop( self );
    OstTraceFunctionExit0( CUPNPICONFILESERVETRANSACTION_NEWL_EXIT );
    return self;
    }    
    
// ---------------------------------------------------------------------------------
// CUpnpIconFileServeTransaction::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpIconFileServeTransaction::ConstructL( const TDesC& aOutFilePath )
    {
    OstTraceFunctionEntry0( CUPNPICONFILESERVETRANSACTION_CONSTRUCTL_ENTRY );
    iOutFilePath.CreateL( aOutFilePath );
    OstTraceFunctionExit0( CUPNPICONFILESERVETRANSACTION_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpIconFileServeTransaction::CUpnpIconFileServeTransaction
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpIconFileServeTransaction::CUpnpIconFileServeTransaction( RFs& aIconFileSession )
    :iIconFileSession( aIconFileSession )
    { 
    
    }

// ---------------------------------------------------------------------------------
// CUpnpIconFileServeTransaction::~CUpnpIconFileServeTransaction
// Default Destructor
// ---------------------------------------------------------------------------------
//    
CUpnpIconFileServeTransaction::~CUpnpIconFileServeTransaction()
    {
    OstTraceFunctionEntry0( CUPNPICONFILESERVETRANSACTION_CUPNPICONFILESERVETRANSACTION_ENTRY );
    iOutFilePath.Close();
    OstTraceFunctionExit0( CUPNPICONFILESERVETRANSACTION_CUPNPICONFILESERVETRANSACTION_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpIconFileServeTransaction::OnCallbackL
// Handler for transaction event.
// Method is invoked when a particular event arrives.
// In case of leave,it traps the error and returns the http error code.
// @param aEvent Server event
// ---------------------------------------------------------------------------------
//    
void CUpnpIconFileServeTransaction::OnCallbackL( TUpnpHttpServerEvent aEvent )
    {
    OstTraceFunctionEntry0( CUPNPICONFILESERVETRANSACTION_ONCALLBACKL_ENTRY );
    TRAPD( err, DoCallbackL( aEvent ) );
    if ( err != KErrNone )
        {
        SetHttpCode( err );
        }
    OstTraceFunctionExit0( CUPNPICONFILESERVETRANSACTION_ONCALLBACKL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpIconFileServeTransaction::DoCallbackL
// Method is used to set the data source file and respective http error codes.
// @param aEvent Server event
// ---------------------------------------------------------------------------------
//  
void CUpnpIconFileServeTransaction::DoCallbackL( TUpnpHttpServerEvent aEvent )
    {
    OstTraceFunctionEntry0( CUPNPICONFILESERVETRANSACTION_DOCALLBACKL_ENTRY );
    switch ( aEvent )
        {
        case EOnRequestStart:
            { 
            RFile file;
            if ( file.Open( iIconFileSession,iOutFilePath,EFileShareReadersOnly | EFileRead ) == KErrNone ) 
                {
                CleanupClosePushL(file);
                // Sets the file which have to be served in response to http request
                SetDataSourceL(  file ); 
                CleanupStack::Pop(&file);
                }
            else
                {
                // if the error occurs set the error code in http response
                SetHttpCode( -EHttpNotFound );
                }                                  
            }break;
        case EOnComplete:
            break;  
        case EOnResponseStart:
            {
            SetHttpCode( KErrNone );
            } break;          
        default:
            break;
        }            
    OstTraceFunctionExit0( CUPNPICONFILESERVETRANSACTION_DOCALLBACKL_EXIT );
    }

// End of File
