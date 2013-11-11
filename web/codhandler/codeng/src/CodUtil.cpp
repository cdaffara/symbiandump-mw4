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
*      Implementation of class CodUtil.   
*      
*
*/


// INCLUDE FILES

#include "CodUtil.h"
#include "CodLogger.h"
#include "CodPanic.h"
#include <uri8.h>
#include <utf.h>
#include <charconv.h>
#include <f32file.h>
#include <commdb.h>
#include <ApSelect.h>
#include <bodypart.h>
#include <AiwGenericParam.h>
#include <caf/caf.h>

// ================= CONSTANTS =======================

/// Conversion buffer size.
LOCAL_D const TInt KCodConversionBufferSize = 256;
/// Blank IP address.
_LIT( KCodBlankIpAdress, "0.0.0.0" );
/// Zero width non-breaking space character.
LOCAL_D const TUint16 KZeroWidthNbsp = 0xfeff;
/// cid: scheme.
_LIT8( KCodCidScheme, "cid" );

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CodUtil::CanOpenL()
// ----------------------------------------------------------
//
EXPORT_C TBool CodUtil::CanOpenL( const TDesC& aFname )
    {
    CLOG(( ECodEng, 4, _L("-> CodUtil::CanOpenL <%S>"), &aFname ));
    __ASSERT_DEBUG( aFname.Length(), CodPanic( ECodInternal ) );
    TBool canOpen( EFalse );
    using namespace ContentAccess;
    CVirtualPath* vPath = CVirtualPath::NewL( aFname, KDefaultContentObject );
    CleanupStack::PushL( vPath );
    TInt err;
    CData* data = NULL;
    // 'No right' style errors leave, need to TRAP.
    TRAP( err, data = CData::NewL( *vPath, EPeek, EContentShareReadOnly ) );
    // Note, 'data' not pushed. No leaving calls below.
    if ( !err &&
                 (
                 !data->EvaluateIntent( EPlay ) ||
                 !data->EvaluateIntent( EView ) ||
                 !data->EvaluateIntent( EExecute )
                 )
       )
        {
        canOpen = ETrue;
        }
    delete data;
    CleanupStack::PopAndDestroy( vPath );
    CLOG(( ECodEng, 4, _L("<- CodUtil::CanOpenL (%d)"), canOpen ));
    return canOpen;
    }

// ----------------------------------------------------------
// CodUtil::CanOpenL()
// ----------------------------------------------------------
//
EXPORT_C TBool CodUtil::CanOpenL( RFile& aFile )
    {
    CLOG(( ECodEng, 4, _L("-> CodUtil::CanOpenL (RFile)") ));
    TBool canOpen( EFalse );
    using namespace ContentAccess;
    TInt err;
    CData* data = NULL;
    // 'No right' style errors leave, need to TRAP.
    TRAP( err, data = CData::NewL( aFile, KDefaultContentObject, EPeek ) );
    // Note, 'data' not pushed. No leaving calls below.
    if ( !err &&
                 (
                 !data->EvaluateIntent( EPlay ) ||
                 !data->EvaluateIntent( EView ) ||
                 !data->EvaluateIntent( EExecute )
                 )
       )
        {
        canOpen = ETrue;
        }
    delete data;
    CLOG(( ECodEng, 4, _L("<- CodUtil::CanOpenL (%d)"), canOpen ));
    return canOpen;
    }

// ---------------------------------------------------------
// CodUtil::GetIntParam()
// ---------------------------------------------------------
//
EXPORT_C void CodUtil::GetIntParam
        (
        TInt& aParam,
        TGenericParamId aSemanticId,
        const CAiwGenericParamList& aParams
        )
    {
    TInt index = 0;
    const TAiwGenericParam* param =
        aParams.FindFirst( index, aSemanticId, EVariantTypeTInt32 );
    if ( param )
        {
        aParam = param->Value().AsTInt32();
        CLOG(( ECodEng, 4, _L("CodUtil::GetIntParam id(%d) value(%d)"), \
            aSemanticId, aParam ));
        }
    }

// ---------------------------------------------------------
// CodUtil::GetUint32Param()
// ---------------------------------------------------------
//
EXPORT_C void CodUtil::GetUint32Param
        (
        TUint32& aParam,
        TGenericParamId aSemanticId,
        const CAiwGenericParamList& aParams
        )
    {
    TInt index = 0;
    const TAiwGenericParam* param =
        aParams.FindFirst( index, aSemanticId, EVariantTypeTInt32 );
    if ( param )
        {
        aParam = STATIC_CAST( TUint32, param->Value().AsTInt32() );
        CLOG(( ECodEng, 4, _L("CodUtil::GetUint32Param id(%d) value(%d)"), \
            aSemanticId, aParam ));
        }
    }

// ---------------------------------------------------------
// CodUtil::GetBoolParam()
// ---------------------------------------------------------
//
EXPORT_C void CodUtil::GetBoolParam
        (
        TBool& aParam,
        TGenericParamId aSemanticId,
        const CAiwGenericParamList& aParams
        )
    {
    TInt index = 0;
    const TAiwGenericParam* param =
        aParams.FindFirst( index, aSemanticId, EVariantTypeTInt32 );
    if ( param )
        {
        aParam = STATIC_CAST( TBool, param->Value().AsTInt32() );
        CLOG(( ECodEng, 4, _L("CodUtil::GetBoolParam id(%d) value(%d)"), \
            aSemanticId, aParam ));
        }
    }

// ---------------------------------------------------------
// CodUtil::GetDesParamLC()
// ---------------------------------------------------------
//
EXPORT_C void CodUtil::GetDesParamLC
        (
        TPtrC8& aParam,
        TGenericParamId aSemanticId,
        const CAiwGenericParamList& aParams
        )
    {
    TInt index = 0;
    HBufC8* buf = NULL;
    // First check 8 bit variant.
    const TAiwGenericParam* param =
        aParams.FindFirst( index, aSemanticId, EVariantTypeDesC8 );
    if ( param )
        {
        aParam.Set( param->Value().AsData() );
        CLOG(( ECodEng, 4, \
            _L8("CodUtil::GetDesParamLC 8-bit id(%d) value<%S>)"), \
            aSemanticId, &aParam ));
        }
    else
        {
        // No 8-bit, check 16-bit.
        param = aParams.FindFirst( index, aSemanticId, EVariantTypeDesC );
        if ( param )
            {
            buf = ConvertL( param->Value().AsDes() );   // No leave until push.
            aParam.Set( *buf );
            CLOG(( ECodEng, 4, \
                _L8("CodUtil::GetDesParamLC 16-bit id(%d) value<%S>)"), \
                aSemanticId, &aParam ));
            }
        }
    CleanupStack::PushL( buf ); // May be NULL.
    }

// ---------------------------------------------------------
// CodUtil::GetDesParam()
// ---------------------------------------------------------
//
EXPORT_C void CodUtil::GetDesParam
        (
        TPtrC& aParam,
        TGenericParamId aSemanticId,
        const CAiwGenericParamList& aParams
        )
    {
    TInt index = 0;
    const TAiwGenericParam* param =
        aParams.FindFirst( index, aSemanticId, EVariantTypeDesC );
    if ( param )
        {
        aParam.Set( param->Value().AsDes() );
        CLOG(( ECodEng, 4, \
            _L8("CodUtil::GetDesParam 16-bit id(%d) value<%S>)"), \
            aSemanticId, &aParam ));
        }
    }

// ---------------------------------------------------------
// CodUtil::IsCidSchemeL()
// ---------------------------------------------------------
//
TBool CodUtil::IsCidSchemeL( const TDesC8& aUrl )
    {
    TUriParser8 uri;
    User::LeaveIfError( uri.Parse( aUrl ) );
    return uri.Extract( EUriScheme ).Compare( KCodCidScheme ) == 0;
    }

// ---------------------------------------------------------
// CodUtil::PartByCidL()
// ---------------------------------------------------------
//
CBodyPart* CodUtil::PartByCidL
( const RPointerArray<CBodyPart>& aParts, const TDesC8& aUrl )
    {
    CBodyPart* part = NULL;
    TUriParser8 uri;
    User::LeaveIfError( uri.Parse( aUrl ) );
    __ASSERT_DEBUG( uri.Extract( EUriScheme ).Compare( KCodCidScheme ) == 0,
        CodPanic( ECodInternal ) ); // Expecting CID scheme!
    CUri8* cUri = CUri8::NewLC( uri );
    cUri->RemoveComponentL( EUriScheme );
    TPtrC8 cid( cUri->Uri().UriDes() );
    for ( TInt i = 0; i < aParts.Count(); i++ )
        {
        if ( !aParts[i]->ContentID().Compare( cid ) )
            {
            part = aParts[i];
            break;
            }
        }
    CleanupStack::PopAndDestroy( cUri );
    return part;
    }

// ---------------------------------------------------------
// CodUtil::FileNameFromUrl()
// ---------------------------------------------------------
//
TPtrC8 CodUtil::FileNameFromUrl( const TDesC8& aUrl )
    {
    TPtrC8 ptr;
    TUriParser8 uri; 
    if ( uri.Parse( aUrl ) == KErrNone )
        {
        ptr.Set( uri.Extract( EUriPath ) );
        TInt slash = ptr.LocateReverse( TChar('/') );
        if( slash >= 0 )
            {
            // We have a slash - keep only what is after it.
            // Position 'slash + 1' can always be passed to Mid(); length is
            // one greater than any resulting index. (If last character is
            // a slash, we get an empty string as expected.)
            ptr.Set( ptr.Mid( slash + 1 ) );
            }
        }
    return ptr;
    }

// ---------------------------------------------------------
// CodUtil::AbsoluteUrlLC()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CodUtil::AbsoluteUrlLC
( const TDesC8& aBase, const TDesC8& aUrl )
    {
    HBufC8* absUrl;
    TUriParser8 url;
    User::LeaveIfError( url.Parse( aUrl ) );
    if ( url.IsPresent( EUriScheme ) )
        {
        // aUrl is already absolute, use it "as-is".
        absUrl = aUrl.AllocLC();
        }
    else
        {
        // aUrl is relative, resolve it against the aBase.
        TUriParser8 base;
        User::LeaveIfError( base.Parse( aBase ) );
        CUri8* resolved = CUri8::ResolveL( base, url );
        CleanupStack::PushL( resolved );
        absUrl = resolved->Uri().UriDes().AllocL();     // Not pushed here...
        CleanupStack::PopAndDestroy( resolved );        // (no leaving here)
        CleanupStack::PushL( absUrl );                  // ...only here.
        }
    return absUrl;
    }

// ---------------------------------------------------------
// CodUtil::ConvertToUcs2L()
// ---------------------------------------------------------
//
HBufC16* CodUtil::ConvertToUcs2L
( const TDesC8& aSrc, const TDesC8& aEncoding, RFs& aFs )
    {
    CLOG(( ECodEng | ECharset, 2, _L("-> CodUtil::ConvertToUcs2L") ));
    CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewLC();
    CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* charsets =
        CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC( aFs );
    TUint id = 0;
    if ( aEncoding.Length() )
        {
        // Encoding is specified, use it.
        CLOG(( ECodEng | ECharset, 4, _L8("charset name given <%S>"), \
            &aEncoding ));
        id = conv->ConvertStandardNameOfCharacterSetToIdentifierL
            ( aEncoding, aFs );
        CLOG(( ECodEng | ECharset, 4, _L("charset id 0x%x"), id ));
        }
    else 
        {
        // Autodetect.
        TInt confidence = 0;
        conv->AutoDetectCharacterSetL( confidence, id, *charsets, aSrc );
        CLOG(( ECodEng | ECharset, 4, \
            _L("detected charset id 0x%x confidence 0x%x"), id, confidence ));
        if ( confidence == 0 || id == 0 )
            {
            // Failed to detect.
            User::Leave( KErrNotSupported );
            }
        }

    HBufC16* ucs2buffer = NULL;
    TBool resultOnStack = EFalse;

    // If id is 0 then we don't know what encoding the file is or it is
    // already UCS2.
    if ( id == 0 )
        {
        // If the source is too small we don't even have a unicode header or
        // have a header with no data so just leave.
        if ( aSrc.Length() < 3 )
            {
            User::Leave( KErrCorrupt );
            }
        
        const TUint8* srcData = aSrc.Ptr();
        if ( srcData[0] == 0xFF && srcData[1] == 0xFE )
            {
            // Little endian, good, just drop the unicode header and return
            // the rest.
            TPtrC result( (TUint16*)(srcData + 2) );
            ucs2buffer = result.AllocL();
            }
        else if ( srcData[0] == 0xFE && srcData[1] == 0xFF )
            {
            // Big endian, we don't support it.
            User::Leave( KErrNotSupported );
            }
        else
            {
            // No Unicode header, assume little endian-ness.
            TPtrC result( (TUint16*)srcData );
            ucs2buffer = result.AllocL();
            }
        }
    else 
        {
        HBufC16* buffer = HBufC::NewLC( KCodConversionBufferSize );
        TPtr16 ptr( buffer->Des() );
        // Prepare conversion for the given charset ID.
        conv->PrepareToConvertToOrFromL( id, *charsets, aFs );

        TInt state = 0;
        TInt remaining = conv->ConvertToUnicode( ptr, aSrc, state );
        while ( remaining >= 0 )
            {
            if ( ucs2buffer == NULL )
                {
                ucs2buffer = HBufC::NewLC( ptr.Length() );
                resultOnStack = ETrue;
                }
            else
                {
                __ASSERT_DEBUG( resultOnStack, CodPanic( ECodInternal ));
                // This may change the address of ucs2buffer so we need to put
                // it on the cleanup stack again!!
                ucs2buffer = ucs2buffer->ReAllocL
                    ( ucs2buffer->Length() + ptr.Length() );
                CleanupStack::Pop();    // old ucs2buffer
                CleanupStack::PushL( ucs2buffer );  // possibly new copy
                }
            TPtr16 ucs2ptr( ucs2buffer->Des() );
            ucs2ptr.Append( ptr );
            if ( remaining > 0 )
                {
                // Try to convert all remaining characters.
                ptr.Zero();
                remaining = conv->ConvertToUnicode
                    ( ptr, aSrc.Right( remaining ), state );
                }
            else
                {
                break;
                }
            }

        if ( resultOnStack )
            {
            CleanupStack::Pop();    // ucs2buffer
            resultOnStack = EFalse;
            }

        CleanupStack::PopAndDestroy( buffer );
        }

    if ( resultOnStack )
        {
        CleanupStack::Pop();        // ucs2buffer
        }

    CleanupStack::PopAndDestroy( 2 );   // charsets, conv

    // Check if first character is a Zero-width nbsp.
    TPtrC16 ptr( ucs2buffer->Des() );
    if ( ptr.Length() >= 1 && ptr[0] == KZeroWidthNbsp )
        {
        // First character is a Zero-width NBSP. This character is used as
        // BOM in some encodings and should not be present at this point.
        // But we are tolerant and remove it.
        // (Not expecting big-endianness here.)
        CLOG(( ECodEng | ECharset, 4, _L("dropping BOM"), id ));
        HBufC16* temp = ucs2buffer;
        CleanupStack::PushL( temp );
        ucs2buffer = ptr.Mid( 1 ).AllocL();
        CleanupStack::PopAndDestroy();  // temp
        }

    CLOG(( ECodEng | ECharset, 2, _L("<- CodUtil::ConvertToUcs2L") ));
    return ucs2buffer;
    }

// ---------------------------------------------------------
// CodUtil::ConvertUcs2ToUtf8L()
// ---------------------------------------------------------
//
HBufC8* CodUtil::ConvertUcs2ToUtf8L( const TDesC& aUcs2Src )
    {
    HBufC8* utf8buffer = NULL;
    TBool resultOnStack = EFalse;
    HBufC8* buffer = HBufC8::NewLC( KCodConversionBufferSize );
    TPtr8 ptr( buffer->Des() );
    TInt remaining =
        CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr, aUcs2Src );
    while ( remaining >= 0 )
        {
        if ( utf8buffer == NULL )
            {
            utf8buffer = HBufC8::NewLC( ptr.Length() );
            resultOnStack = ETrue;
            }
        else
            {
            __ASSERT_DEBUG( resultOnStack, CodPanic( ECodInternal ) );
            // This may change the address of utf8buffer so we need to put it
            // on the cleanup stack again!!
            utf8buffer = utf8buffer->ReAllocL
                ( utf8buffer->Length() + ptr.Length() );
            CleanupStack::Pop();    // old utf8buffer
            CleanupStack::PushL( utf8buffer );  // possibly new copy
            }
        TPtr8 utf8ptr( utf8buffer->Des() );
        utf8ptr.Append( ptr );
        if ( remaining > 0 )
            {
            // Try to convert all remaining characters.
            ptr.Zero();
            remaining = CnvUtfConverter::ConvertFromUnicodeToUtf8
                ( ptr, aUcs2Src.Right( remaining ) );
            }
        else
            {
            break;
            }
        }
    
    if ( resultOnStack )
        {
        CleanupStack::Pop();    // utf8buffer
        }
    CleanupStack::PopAndDestroy( buffer );
    return utf8buffer;
    }

// ---------------------------------------------------------
// CodUtil::ConvertLC()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CodUtil::ConvertLC( const TDesC& aDes )
    {
    HBufC8* buf = HBufC8::NewLC( aDes.Length() );
    buf->Des().Copy( aDes );
    return buf;
    }

// ---------------------------------------------------------
// CodUtil::ConvertLC()
// ---------------------------------------------------------
//
EXPORT_C HBufC* CodUtil::ConvertLC( const TDesC8& aDes )
    {
    HBufC* buf = HBufC::NewLC( aDes.Length() );
    buf->Des().Copy( aDes );
    return buf;
    }

// ---------------------------------------------------------
// CodUtil::ConvertL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CodUtil::ConvertL( const TDesC& aDes )
    {
    HBufC8* buf = HBufC8::NewL( aDes.Length() );
    buf->Des().Copy( aDes );
    return buf;
    }

// ---------------------------------------------------------
// CodUtil::ConvertL()
// ---------------------------------------------------------
//
EXPORT_C HBufC* CodUtil::ConvertL( const TDesC8& aDes )
    {
    HBufC* buf = HBufC::NewL( aDes.Length() );
    buf->Des().Copy( aDes );
    return buf;
    }

// ---------------------------------------------------------
// CodUtil::ApCountL()
// ---------------------------------------------------------
//
TUint32 CodUtil::ApCountL()
    {
    TUint32 count( 0 );
    CCommsDatabase* cdb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( cdb );
    CApSelect* apSel = CApSelect::NewLC
        (
        *cdb,
        KEApIspTypeAll,
        EApBearerTypeAll,
        KEApSortUidAscending
        );
    count = apSel->Count();
    CleanupStack::PopAndDestroy( 2 );   // apSel, cdb
    return count;
    }

// ---------------------------------------------------------
// CodUtil::WapGatewayL()
// ---------------------------------------------------------
//
HBufC8* CodUtil::WapGatewayL( TUint32 aIap )
    {
    HBufC8* gateway8 = NULL;

    // Open a CommDB database server session.
    // This has to be local and initialised every time or we get
    // strange access denied leaves happening.
    CCommsDatabase* cdb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( cdb );

    // Now access the commdb to see whether we have an associated
    // WAP AP and if so determine the gateway that we are using.
    TPtrC wapApTable( WAP_IP_BEARER );
    TPtrC wapApIapCol( WAP_IAP );
    CCommsDbTableView* view =
        cdb->OpenViewMatchingUintLC( wapApTable, wapApIapCol, aIap );
    TInt err = view->GotoFirstRecord();
    if ( !err )
        {
        // There is a matching WAP AP table entry, does it have a
        // valid gateway?
        TPtrC wapApGwCol( WAP_GATEWAY_ADDRESS );
        HBufC* gateway16 = view->ReadLongTextLC( wapApGwCol );
        if ( gateway16->Compare( KCodBlankIpAdress ) )
            {
            // There is a valid WAP gateway.
            gateway8 = ConvertL( *gateway16 );  // Not pushed, no leave below.
            }
        CleanupStack::PopAndDestroy( gateway16 );
        }
    CleanupStack::PopAndDestroy( 2 );   // view, cdb
    return gateway8;
    }
