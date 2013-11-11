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
*      Implementation of class CCodRecog.   
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
#include "CodRecog.h"
#include "CodRecogLogger.h"

// ================= CONSTANTS =======================

/// UID of COD Viewer Application.
LOCAL_D const TUid KCodUidCodViewerApp = { COD_VIEWER_UID };
/// Preferred recognition buffer size.
const TInt KCodPreferedBufferSize = 1024;
/// COD MIME type string.
_LIT8( KCodMimeType, COD_MIME_TYPE );
/// COD file extension.
_LIT( KCodExt,".cod" );
/// COD content recognition pattern (8 bit).
_LIT8( KCod8,"COD-");
/// COD content recognition pattern (16 bit).
_LIT( KCod16,"COD-");

/// COD recog ecom plugin implementation UID.
LOCAL_D const TInt KCodRecogImplementationUid = 0x10008d49;
/// ECOM implementation table.
LOCAL_D const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY\
        ( KCodRecogImplementationUid, CCodRecog::CreateRecognizerL )
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodRecog::CCodRecog()
// ---------------------------------------------------------
//
CCodRecog::CCodRecog()
: CApaDataRecognizerType
                    ( KCodUidCodViewerApp, CApaDataRecognizerType::ENormal )
    {
    iCountDataTypes = 1;
    }

// ---------------------------------------------------------
// CCodRecog::PreferredBufSize()
// ---------------------------------------------------------
//
TUint CCodRecog::PreferredBufSize()
    {
    return KCodPreferedBufferSize;
    }

// ---------------------------------------------------------
// CCodRecog::SupportedDataTypeL()
// ---------------------------------------------------------
//
TDataType CCodRecog::SupportedDataTypeL( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return TDataType( KCodMimeType );
            }

        default:
            {
            return TDataType();
            }
        }
    }

// ---------------------------------------------------------
// CCodRecog::CreateRecognizerL()
// ---------------------------------------------------------
//
CApaDataRecognizerType* CCodRecog::CreateRecognizerL()
    {
    return new (ELeave) CCodRecog();
    }

// ---------------------------------------------------------
// CCodRecog::DoRecognizeL()
// ---------------------------------------------------------
//
void CCodRecog::DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer )
    {
    CLOG_ENTERFN( "CCodRecog::DoRecognizeL" );
    CLOG_WRITE_FORMAT( "DoRecognize: name: %S", &aName );

    iConfidence = ENotRecognized;

    // Check extension. We cannot use TParse* classes, as the recog is
    // called with generated filenames which contain invalid chars and
    // panic TParsePtrC.
    TInt len( KCodExt().Length() );
    if( aName.Length() >= len && !aName.Right( len ).CompareF( KCodExt ) )
        {
        CLOG_WRITE( "DoRecognize: extension is '.cod'" );
        // Named ".cod"
        iDataType = TDataType( KCodMimeType );
        iConfidence = ECertain;
        }
    else
        {
        CLOG_WRITE( "DoRecognize: pattern search" );
        // Not named ".cod". Recognition by pattern-matching.
        // Prepare pointers to buffer (both 8 and 16 bits).
        TPtrC8 buf8( aBuffer );
        if ( aBuffer.Length() > KCodPreferedBufferSize )
            {
            buf8.Set( aBuffer.Left( KCodPreferedBufferSize ) );
            }
        TPtrC buf16( (TText*)buf8.Ptr(), buf8.Length() / 2 );
        // Find COD-specific pattern in buffer (both 8 and 16 bits).
        if ( buf8.FindF( KCod8 ) >= 0 || buf16.FindF( KCod16 ) >= 0 )
            {
            iDataType = TDataType( KCodMimeType );
            iConfidence = ECertain;
            }
        }
    CLOG_WRITE_FORMAT( "DoRecognize: confidence: %d", iConfidence );
    CLOG_LEAVEFN( "CCodRecog::DoRecognizeL" );
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
