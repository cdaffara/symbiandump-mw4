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
*      Implementation of class CDdRecog.   
*      
*
*/


// INCLUDE FILES

#include <s32std.h>
#include <apmrec.h>
#include <apmstd.h>
#include <f32file.h>
#include <CodDefs.h>
#include <charconv.h>
#include <implementationproxy.h>
#include "DdRecog.h"
#include "DdRecogLogger.h"

// ================= CONSTANTS =======================

/// UID of DD Viewer Application.
LOCAL_D const TUid KDdUidDdViewerApp = { DD_VIEWER_UID };
/// Preferred recognition buffer size.
const TInt KPreferedBufferSize = 1024;
/// DD MIME type string.
_LIT8( KDdMimeType, DD_MIME_TYPE );
/// DD file extension.
_LIT( KDdExt,".dd" );
/// DD content recognition pattern (8 bit).
_LIT8( KDd8,"<media");
/// DD content recognition pattern (16 bit).
_LIT( KDd16,"<media");

/// DD recog ecom plugin implementation UID.
LOCAL_D const TInt KDdRecogImplementationUid = 0x10008d55;
/// ECOM implementation table.
LOCAL_D const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY\
        ( KDdRecogImplementationUid, CDdRecog::CreateRecognizerL )
    };


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDdRecog::CDdRecog()
// ---------------------------------------------------------
//
CDdRecog::CDdRecog()
: CApaDataRecognizerType( KDdUidDdViewerApp, CApaDataRecognizerType::ENormal )
    {
    iCountDataTypes = 1;
    }

// ---------------------------------------------------------
// CDdRecog::PreferredBufSize()
// ---------------------------------------------------------
//
TUint CDdRecog::PreferredBufSize()
    {
    return KPreferedBufferSize;
    }

// ---------------------------------------------------------
// CDdRecog::SupportedDataTypeL()
// ---------------------------------------------------------
//
TDataType CDdRecog::SupportedDataTypeL( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return TDataType( KDdMimeType );
            }

        default:
            {
            return TDataType();
            }
        }
    }

// ---------------------------------------------------------
// CDdRecog::CreateRecognizerL()
// ---------------------------------------------------------
//
CApaDataRecognizerType* CDdRecog::CreateRecognizerL()
    {
    return new (ELeave) CDdRecog();
    }

// ---------------------------------------------------------
// CDdRecog::DoRecognizeL()
// ---------------------------------------------------------
//
void CDdRecog::DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer )
    {
    CLOG_ENTERFN( "CDdRecog::DoRecognizeL" );
    CLOG_WRITE_FORMAT( "DoRecognize: name: %S", &aName );

    iConfidence = ENotRecognized;

    // Check extension. We cannot use TParse* classes, as the recog is
    // called with generated filenames which contain invalid chars and
    // panic TParsePtrC.
    TInt len( KDdExt().Length() );
    if( aName.Length() >= len && !aName.Right( len ).CompareF( KDdExt ) )
        {
        CLOG_WRITE( "DoRecognize: extension is '.dd'" );
        // Named ".dd"
        iDataType = TDataType( KDdMimeType );
        iConfidence = ECertain;
        }
    else
        {
        CLOG_WRITE( "DoRecognize: pattern search" );
        // Not named ".dd". Recognition by pattern-matching.
        // Prepare pointers to buffer (both 8 and 16 bits).
        TPtrC8 buf8( aBuffer );
        if ( aBuffer.Length() > KPreferedBufferSize )
            {
            buf8.Set( aBuffer.Left( KPreferedBufferSize ) );
            }
        TPtrC buf16( (TText*)buf8.Ptr(), buf8.Length() / 2 );
        // Find DD-specific pattern in buffer (both 8 and 16 bits).
        if ( buf8.FindF( KDd8 ) >= 0 || buf16.FindF( KDd16 ) >= 0 )
            {
            iDataType = TDataType( KDdMimeType );
            iConfidence = ECertain;
            }
        }
    CLOG_WRITE_FORMAT( "DoRecognize: confidence: %d", iConfidence );
    CLOG_LEAVEFN( "CDdRecog::DoRecognizeL" );
    }

/**
* Implementation group proxy.
* @param aTableCount Table count is returned here.
* @return Implementation table is returned here.
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
( TInt& aTableCount )
    {
    aTableCount =
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
