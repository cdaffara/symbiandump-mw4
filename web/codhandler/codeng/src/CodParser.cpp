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
*      Implementation of class TCodParser.   
*      
*
*/


// INCLUDE FILES

#include "CodParser.h"
#include "CodData.h"
#include "CodError.h"
#include "CodPanic.h"
#include "CodLogger.h"

// ================= CONSTANTS =======================

// Characters.
LOCAL_D const TText KCodSpace = 0x0020;         ///< Space.
LOCAL_D const TText KCodHorizTab = 0x0009;      ///< Horizontal tab.
LOCAL_D const TText KCodCarriageRet = 0x000d;   ///< Carriage return.
LOCAL_D const TText KCodLineFeed = 0x000a;      ///< Line feed.
LOCAL_D const TText KCodColon = ':';            ///< Colon.
LOCAL_D const TText KCodLowControl = 0x001F;    ///< Lower Control characters.
LOCAL_D const TText KCodUpControl = 0x007F;     ///< Upper Control character.

// Attribute names.
_LIT( KCodName, "COD-Name" );                       ///< COD-Name.
_LIT( KCodVendor, "COD-Vendor" );                   ///< COD-Vendor.
_LIT( KCodDescription, "COD-Description" );         ///< COD-Description.
_LIT( KCodUrl, "COD-URL" );                         ///< COD-URL.
_LIT( KCodSize, "COD-Size" );                       ///< COD-Size.
_LIT( KCodType, "COD-Type" );                       ///< COD-Type.
_LIT( KCodInstallNotify, "COD-Install-Notify" );    ///< COD-Install-Notify.
_LIT( KCodNextUrl, "COD-Next-URL" );                ///< COD-Next-URL.
_LIT( KCodNextUrlAtError, "COD-Next-URLatError" );  ///< COD-Next_URLatError.
_LIT( KCodInfoUrl, "COD-Info-URL" );                ///< COD-Info-URL.
_LIT( KCodPrice, "COD-Price" );                     ///< COD-Price.
_LIT( KCodIcon, "COD-Icon" );                       ///< COD-Icon.

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TCodParser::ParseL()
// ---------------------------------------------------------
//
void TCodParser::ParseL( const TDesC& aBuf, CCodData& aData )
    {
    CLOG(( EParse, 2, _L("-> TCodParser::ParseL") ));
    CDUMP(( EParse, 2, _S("Buf:"), _S("    "), \
        (const TUint8*)aBuf.Ptr(), aBuf.Size() ));
    iError = KErrNone;
    iData = &aData;
    iBuf = &aBuf;
    iCurP = iBuf->Ptr();
    iEndP = iCurP + iBuf->Length();
    CMediaObjectData *mediaObject = CMediaObjectData::NewL();
    // Processing lines (attribute and value) until there is more lines to read.
    while ( AttrLineL(mediaObject) )
        {
        // Some compilers require empty controlled statement block instead of
        // just a semicolon.
        }
    iData->AppendMediaObjectL( mediaObject );   
#ifdef __TEST_COD_LOG
    TPtrC ptr16;
    TPtrC8 ptr8;
    CLOG(( EParse, 3, _L("TCodParser::ParseL data:") ));
    ptr16.Set( aData.Name() );
    CLOG(( EParse, 3, _L("  Name<%S>"), &ptr16 ));
    ptr16.Set( aData.Vendor() );
    CLOG(( EParse, 3, _L("  Vendor<%S>"), &ptr16 ));
    ptr16.Set( aData.Description() );
    CLOG(( EParse, 3, _L("  Desc<%S>"), &ptr16 ));
    CLOG(( EParse, 3, _L("  Size(%d)"), aData.Size() ));
    ptr8.Set( aData.InstallNotify() );
    CLOG(( EParse, 3, _L8("  InstNotif<%S>"), &ptr8 ));
    ptr8.Set( aData.NextUrl() );
    CLOG(( EParse, 3, _L8("  NextUrl<%S>"), &ptr8 ));
    ptr8.Set( aData.NextUrlAtError() );
    CLOG(( EParse, 3, _L8("  NextUrlAtErr<%S>"), &ptr8 ));
    ptr8.Set( aData.InfoUrl() );
    CLOG(( EParse, 3, _L8("  InfoUrl<%S>"), &ptr8 ));
    ptr16.Set( aData.Price() );
    CLOG(( EParse, 3, _L("  Price<%S>"), &ptr16 ));
    ptr8.Set( aData.Icon() );
    CLOG(( EParse, 3, _L8("  Icon<%S>"), &ptr8 ));
#endif /* def __TEST_COD_LOG */

    // NULL data for clarity. These are never used later, but don't keep
    // pointers to objects which are out of reach.
    iBuf = NULL;
    iData = NULL;
    iCurP = NULL;
    iEndP = NULL;
    User::LeaveIfError( iError );
    CLOG(( EParse, 2, _L("<- TCodParser::ParseL") ));
    }

// ---------------------------------------------------------
// TCodParser::AttrLineL()
// ---------------------------------------------------------
//
TBool TCodParser::AttrLineL(CMediaObjectData *& aMediaObject)
    {
    SkipWhiteSpace();  // Skip lines which contain only WS and LF at the end.
    while ( IsEndOfLine() )
        {
        NextLine();
        SkipWhiteSpace();
        }
    TBool ok( ETrue );
    if ( iCurP < iEndP )
        {
        // Still has something to read.
        switch( AttrName() )
            {
            case ECodName:
                {
                if ( Colon() )
                    {
                    ok = aMediaObject->SetNameL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodVendor:
                {
                if ( Colon() )
                    {
                    ok = iData->SetVendorL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodDescription:
                {
                if ( Colon() )
                    {
                    ok = aMediaObject->SetDescriptionL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodSize:
                {
                if ( Colon() )
                    {
                    // Parse as TUint - negative not allowed.
                    TUint size;
                    TLex lex( AttrValue() );
                    if ( !lex.Val( size ) )
                        {
                        aMediaObject->SetSize( size );
                        }
                    else
                        {
                        ok = EFalse;
                        }
                    EndOfLine();
                    }
                break;
                }

            case ECodInstallNotify:
                {
                if ( Colon() )
                    {
                    ok = aMediaObject->SetInstallNotifyL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodNextUrl:
                {
                if ( Colon() )
                    {
                    ok = iData->SetNextUrlL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodNextUrlAtError:
                {
                if ( Colon() )
                    {
                    ok = iData->SetNextUrlAtErrorL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodInfoUrl:
                {
                if ( Colon() )
                    {
                    ok = aMediaObject->SetInfoUrlL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodPrice:
                {
                if ( Colon() )
                    {
                    ok = aMediaObject->SetPriceL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodIcon:
                {
                if ( Colon() )
                    {
                    ok = aMediaObject->SetIconL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }
            case ECodType:
                {
                if ( Colon() )
                    {
                    ok = aMediaObject->SetTypeL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }
            case ECodUrl:
                {
                if ( Colon() )
                    {
                    ok = aMediaObject->SetUrlL( AttrValue() );
                    EndOfLine();
                    }
                break;
                }

            case ECodUnknownAttr:
                {
                // Name unknown; check colon anyway (syntax check).
                ok = Colon();
                // Rest of the line goes unchecked.
                break;
                }

            default:
                {
                // Unexpected value.
                CodPanic( ECodInternal );
                }

            }
        if ( !ok )
            {
            Error( KErrCodInvalidDescriptor );
            }
        NextLine();     // Step past LF.
        return ETrue;   // More lines to go.
        }
    else
        {
        // EOF reached; done.
        // Note: not expecting EOF in any other place than here (well-formed
        // COD has complete attrlines. If EOF is found some other place, it
        // is a syntax error.
        return EFalse;
        }
    }

// ---------------------------------------------------------
// TCodParser::AttrName()
// ---------------------------------------------------------
//

TCodParser::TCodAttr TCodParser::AttrName()
    {
    TCodAttr attr( ECodUnknownAttr );

    const TText* start = iCurP;
    while
        (
            iCurP < iEndP &&
            !IsControl() &&
            !IsSeparator() &&
            *iCurP != KCodCarriageRet &&
            *iCurP != KCodLineFeed
        )
        {
        iCurP++;
        }

    TPtrC token( start, iCurP - start );
    if ( !token.Length() )
        {
        Error( KErrCodInvalidDescriptor );
        }
    else if ( !token.Compare( KCodName ) )
        {
        attr = ECodName;
        }
    else if ( !token.Compare( KCodVendor ) )
        {
        attr = ECodVendor;
        }
    else if ( !token.Compare( KCodDescription ) )
        {
        attr = ECodDescription;
        }
    else if ( !token.Compare( KCodUrl ) )
        {
        attr = ECodUrl;
        }
    else if ( !token.Compare( KCodSize ) )
        {
        attr = ECodSize;
        }
    else if ( !token.Compare( KCodType ) )
        {
        attr = ECodType;
        }
    else if ( !token.Compare( KCodInstallNotify ) )
        {
        attr = ECodInstallNotify;
        }
    else if ( !token.Compare( KCodNextUrl ) )
        {
        attr = ECodNextUrl;
        }
    else if ( !token.Compare( KCodNextUrlAtError ) )
        {
        attr = ECodNextUrlAtError;
        }
    else if ( !token.Compare( KCodInfoUrl ) )
        {
        attr = ECodInfoUrl;
        }
    else if ( !token.Compare( KCodPrice ) )
        {
        attr = ECodPrice;
        }
    else if ( !token.Compare( KCodIcon ) )
        {
        attr = ECodIcon;
        }

    CLOG(( EParse, 4, _L("TCodParser::AttrName token<%S> attr(%d)"), \
        &token, attr ));
    return attr;
    }

// ---------------------------------------------------------
// TCodParser::AttrValue()
// ---------------------------------------------------------
//
TPtrC TCodParser::AttrValue()
    {
    const TText* start = iCurP;
    const TText* trailingWs = NULL;
    while ( iCurP < iEndP && (IsValueChar() || IsWhiteSpace()) )
        {
        if ( IsWhiteSpace() && !trailingWs )
            {
            // Whitespace starts here; may be trailing WS.
            trailingWs = iCurP;
            }
        else if ( IsValueChar() )
            {
            // Possible previous WS is not trailing: value chars follow.
            trailingWs = NULL;
            }
        iCurP++;
        }
    if ( trailingWs )
        {
        // There was trailing WS. Back up to that position.
        iCurP = trailingWs;
        // Trailing WS should be trailing, not leading. This method expects
        // to be called with WS skipped first!
        __ASSERT_DEBUG( trailingWs > start, CodPanic( ECodInternal ) );
        }
    TPtrC token( start, iCurP - start );
    CLOG(( EParse, 4, _L("TCodParser::AttrValue token<%S>"), &token ));
    return token;
    }

// ---------------------------------------------------------
// TCodParser::SkipWhiteSpace()
// ---------------------------------------------------------
//
void TCodParser::SkipWhiteSpace()
    {
    while ( iCurP < iEndP && IsWhiteSpace() )
        {
        iCurP++;
        }
    }

// ---------------------------------------------------------
// TCodParser::NextLine()
// ---------------------------------------------------------
//
void TCodParser::NextLine()
    {
    while ( iCurP < iEndP && *iCurP != KCodLineFeed )
        {
        iCurP++;
        }
    if ( iCurP < iEndP ) // skip last LF.
        {
        iCurP++;
        }
    }

// ---------------------------------------------------------
// TCodParser::EndOfLine()
// ---------------------------------------------------------
//
void TCodParser::EndOfLine()
    {
    while ( iCurP < iEndP )
        {
        if ( *iCurP == KCodLineFeed )
            {
            // LF found -> done. This is the lookahead.
            break;
            }
        else if ( *iCurP == KCodCarriageRet )
            {
            // CR found -> may be CR LF (that's OK) or CR alone (error).
            if ( iCurP + 1 < iEndP && *(iCurP + 1) == KCodLineFeed )
                {
                // CR LF found-> done. Lookahead is the LF.
                iCurP ++;
                break;
                }
            // CR without LF is unexpected.
            Error( KErrCodInvalidDescriptor );
            }
        if ( IsValueChar() || IsControl() )
            {
            // Valuechar or CTL unexpected.
            Error( KErrCodInvalidDescriptor );
            }
        iCurP++;
        }
    }

// ---------------------------------------------------------
// TCodParser::Colon()
// ---------------------------------------------------------
//
TBool TCodParser::Colon()
    {
    TBool colon( EFalse );
    SkipWhiteSpace();
    if ( iCurP < iEndP && *iCurP == KCodColon )
        {
        // OK it's a colon.
        colon = ETrue;
        iCurP++;
        }
    else
        {
        // Expected a colon.
        Error( KErrCodInvalidDescriptor );
        }
    SkipWhiteSpace();
    return colon;
    }

// ---------------------------------------------------------
// TCodParser::IsValueChar()
// ---------------------------------------------------------
//
TBool TCodParser::IsValueChar() const
    {
    __ASSERT_DEBUG( iCurP < iEndP, CodPanic( ECodBufferOverread ) );
    return ( !(IsControl() || IsWhiteSpace()) );
    }

// ---------------------------------------------------------
// TCodParser::IsControl()
// ---------------------------------------------------------
//
TBool TCodParser::IsControl() const
    {
    __ASSERT_DEBUG( iCurP < iEndP, CodPanic( ECodBufferOverread ) );
    return ( (*iCurP <= KCodLowControl) || (*iCurP == KCodUpControl) );
    }

// ---------------------------------------------------------
// TCodParser::IsWhiteSpace()
// ---------------------------------------------------------
//
TBool TCodParser::IsWhiteSpace() const
    {
    __ASSERT_DEBUG( iCurP < iEndP, CodPanic( ECodBufferOverread ) );
    return ( (*iCurP == KCodSpace) || (*iCurP == KCodHorizTab) );
    }

// ---------------------------------------------------------
// TCodParser::IsSeparator()
// ---------------------------------------------------------
//
TBool TCodParser::IsSeparator() const
    {
    __ASSERT_DEBUG( iCurP < iEndP, CodPanic( ECodBufferOverread ) );
    return
        (
            IsWhiteSpace()  ||
            *iCurP == '('   ||
            *iCurP == ')'   ||
            *iCurP == '<'   ||
            *iCurP == '>'   ||
            *iCurP == '@'   ||
            *iCurP == ','   ||
            *iCurP == ';'   ||
            *iCurP == ':'   ||
            *iCurP == '\''  ||
            *iCurP == '\"'  ||
            *iCurP == '/'   ||
            *iCurP == '['   ||
            *iCurP == ']'   ||
            *iCurP == '?'   ||
            *iCurP == '='   ||
            *iCurP == '{'   ||
            *iCurP == '}'
        );
    }

// ---------------------------------------------------------
// TCodParser::IsEndOfLine()
// ---------------------------------------------------------
//
TBool TCodParser::IsEndOfLine() const
    {
    const TText* CurP = iCurP;
    if ( CurP < iEndP  && *CurP == KCodCarriageRet )
        {
        CurP++;
        }
    if ( CurP < iEndP  && *CurP == KCodLineFeed )
        {
        return ETrue;
        }
    return EFalse;
    }
