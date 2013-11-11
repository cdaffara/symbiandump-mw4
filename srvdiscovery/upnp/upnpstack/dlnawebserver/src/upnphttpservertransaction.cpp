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
 * Description:  CUpnpHttpServerTransaction implementation.
 *
 */

#include <f32file.h>
#include "upnpcons.h"
#include "upnphttpservertransaction.h"
#include "upnphttpmessage.h"
#include "upnpfileutils.h"
#include "upnphttpservertransactioncreator.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::CUpnpHttpServerTransaction
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CUpnpHttpServerTransaction::CUpnpHttpServerTransaction()
    {
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::~CUpnpHttpServerTransaction
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CUpnpHttpServerTransaction::~CUpnpHttpServerTransaction()
    {
    iHeaderBuffer.Close();
    iFile.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::SetDataSourceL
// used for GET 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CUpnpHttpServerTransaction::SetDataSourceL( RFile& aFile )
    {
    iFile.Close();
    iFile = aFile;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::SetDataSinkL
// used for POST 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CUpnpHttpServerTransaction::SetDataSinkL( RFile& aFile )
    {
    iFile.Close();
    iFile = aFile;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::DataSource
// used for GET 
// ---------------------------------------------------------------------------
// 
RFile& CUpnpHttpServerTransaction::DataSource()
    {
    return iFile;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::DataSink
//  used for POST 
// ---------------------------------------------------------------------------
// 
RFile& CUpnpHttpServerTransaction::DataSink()
    {
    return iFile;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::AddResponseHeaderL
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CUpnpHttpServerTransaction::AddResponseHeaderL( const TDesC8& aName, 
                                                              const TDesC8& aValue )
    {
    TInt newLength = iHeaderBuffer.Length() + aValue.Length() 
                     + UpnpString::KDoubleLineFeed().Length();
    TBool nameUsed = ( aName != KNullDesC8() );
    TInt dlfLen = UpnpString::KDoubleLineFeed().Length();
    
    if ( iHeaderBuffer.Right( dlfLen ).Compare( UpnpString::KDoubleLineFeed() ) == 0 )
        {   // if buffer ends with double line feed then remove it and append line feed
        iHeaderBuffer.Delete( iHeaderBuffer.Length() - dlfLen, dlfLen );
        iHeaderBuffer.Append( UpnpString::KLineFeed() );
        }

    if ( nameUsed )
        {
        newLength += UpnpString::KColon().Length() + UpnpString::KSpace().Length() 
                     + aName.Length();
        }

    iHeaderBuffer.ReAllocL( newLength );

    if ( nameUsed )
        {
        iHeaderBuffer.Append( aName );
        iHeaderBuffer.Append( UpnpString::KColon() );
        iHeaderBuffer.Append( UpnpString::KSpace() );
        }
    else
        {
        iHeaderBuffer.Zero();
        }

    iHeaderBuffer.Append( aValue );
    iHeaderBuffer.Append( UpnpString::KLineFeed() );
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::QueryResponseHeader
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C const TDesC8& CUpnpHttpServerTransaction::QueryResponseHeader()
    {
    if ( iHeaderBuffer.Find( UpnpString::KDoubleLineFeed() ) == KErrNotFound )
        {
        if ( iHeaderBuffer.Right( UpnpString::KLineFeed().Length() ).
                                    Compare( UpnpString::KLineFeed() ) == 0 )
            {
            if ( iHeaderBuffer.ReAlloc( iHeaderBuffer.Length() +
                             UpnpString::KLineFeed().Length() ) == KErrNone )
                {
                iHeaderBuffer.Append( UpnpString::KLineFeed() );
                }
            }
        else
            {
            if ( iHeaderBuffer.ReAlloc( iHeaderBuffer.Length() + 
                       UpnpString::KDoubleLineFeed().Length() ) == KErrNone )
                {
                iHeaderBuffer.Append( UpnpString::KDoubleLineFeed() );
                }
            }
        }
    return iHeaderBuffer;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::SetHttpCode
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CUpnpHttpServerTransaction::SetHttpCode( TInt aCode )
    {
    iHttpCode = aCode;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::QueryRequestHeader
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TDesC8& CUpnpHttpServerTransaction::QueryRequestHeader( const TDesC8& aHeaderName )
    {
    return iRequest->GetHeaderValue( aHeaderName );
    }

// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::Error
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CUpnpHttpServerTransaction::Error()
    {
    return iHttpCode;
    }
 
// ---------------------------------------------------------------------------
// CUpnpHttpServerTransaction::SetRequest
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CUpnpHttpServerTransaction::SetRequest( CUpnpHttpMessage* aRequest )
    {
    iRequest = aRequest;
    }


