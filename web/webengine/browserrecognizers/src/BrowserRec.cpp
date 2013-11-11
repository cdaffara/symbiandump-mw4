/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Recognizer for the bowser supported MIME types  
*
*/

#include "BrowserRec.h"

#include "implementationproxy.h"

//
// CBrowserRecognizer
//
CBrowserRecognizer::CBrowserRecognizer()
:CApaDataRecognizerType( KUidMimeBrowserRecognizer, CApaDataRecognizerType::EHigh )
    {
    iCountDataTypes = KSupportedMimetypes;
    }

TUint CBrowserRecognizer::PreferredBufSize()
    {
    return 0x80;
    }

TDataType CBrowserRecognizer::SupportedDataTypeL( TInt aIndex ) const
    {
    __ASSERT_DEBUG( aIndex>=0 && aIndex < KSupportedMimetypes, User::Invariant() );
    switch ( aIndex )
        {
        case 0:
            return TDataType( KHTMLMimeType );
        case 1:
            return TDataType( KXHTMLMimeType1 );
        case 2:
            return TDataType( KXHTMLMimeType2 );
        case 3:
            return TDataType( KXHTMLMimeType2 );
        case 4:           
            return TDataType( KCssMimeType );
        case 5:
            return TDataType( KJavaSCMimeType);
        case 6:
            return TDataType( KJavaSCMimeType);
        case 7:
            return TDataType( KJavaEcmaMimeType);
        case 8:
            return TDataType( KOPMLMimeType);
#ifndef BRDO_WML_DISABLED_FF                
        case 9:
             return TDataType( KWMLCMimeType ); 
        case 10:
            return TDataType( KWMLMimeType );
        case 11:
            return TDataType( KWBXMLMimeType );
        case 12:
            return TDataType( KWMLSCMimeType );
#endif                          
        default:
            return TDataType( KHTMLMimeType );
        } 
    }

void CBrowserRecognizer::DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer )
    {
    iConfidence = ENotRecognized;
    if ( aBuffer.Length() < 3 )
        return;

    // First try the name extension
    if ( aName.Length() >= 3 )
        {
        TInt dotPos = aName.LocateReverse( '.' );
        if ( dotPos != KErrNotFound )
            {
            TInt extLength = aName.Length() - dotPos;
            HBufC* ext = aName.Right( extLength ).AllocL();
            CleanupStack::PushL( ext );
            
#ifndef BRDO_WML_DISABLED_FF
            // application/vnd.wap.wmlc or application/vnd.wap.wbxml
            if ( ext->CompareF( KDotWMLC )  == 0 ||
                 ext->CompareF( KDotWBXML ) == 0 )
                {
                iDataType = TDataType( KWMLCMimeType );
                if ( CheckWbxmlVersion( aBuffer[0] ) &&
                     CheckWbxmlPubId( aBuffer[1] ) )
                    {
                    iConfidence = ECertain;
                    }
                else
                    {
                    iConfidence = EPossible;
                    }
                }
            // application/vnd.wap.wmlscriptc
            else if ( ext->CompareF(KDotWMLSC) == 0 )
                {
                iDataType = TDataType( KWMLSCMimeType );
                iConfidence = ECertain;
                }
            else if ( ext->CompareF(KDotJavaScript) == 0 )
                {
                iDataType = TDataType( KJavaSCMimeType );
                iConfidence = ECertain;
                }
#else
            if ( ext->CompareF(KDotJavaScript) == 0 )
                {
                iDataType = TDataType( KJavaSCMimeType );
                iConfidence = ECertain;
                }
#endif     // BRDO_WML_DISABLED_FF
            else if ( ext->CompareF(KDotCss) == 0 )
                {
                iDataType = TDataType( KCssMimeType );
                iConfidence = ECertain;
                }

            // text/html
            else if ( ext->CompareF(KDotHTML) == 0 ||
                      ext->CompareF(KDotHTM)  == 0 )
                {
                iDataType = TDataType( KHTMLMimeType );
                iConfidence = ECertain;
                }
            // application/vnd.wap.html+xml and application/xhtml+xml
            else if ( ext->CompareF(KDotXHTML)  == 0 ||
                      ext->CompareF(KDotXHTML2) == 0 )
                {
                iDataType = TDataType( KHTMLMimeType );
                iConfidence = ECertain;
                }
            // OPML file
            else if ( ext->CompareF(KDotOPML) == 0 )
                {
                iDataType = TDataType( KOPMLMimeType );
                iConfidence = ECertain;
                }
            CleanupStack::PopAndDestroy();    // ext
            }
        }

        // No recognized file name extensions, we have to look into the buffer
        if ( iConfidence == ENotRecognized )
            {
#ifndef BRDO_WML_DISABLED_FF
            if ( CheckForWMLC( aBuffer ) )
                {
                iDataType = TDataType( KWMLCMimeType );
                iConfidence = ECertain;
                }
            else if ( CheckForOPML( aBuffer ) )
                {
                iDataType = TDataType( KOPMLMimeType );
                iConfidence = ECertain;
                }
            else if ( CheckForWML( aBuffer ) )
                {
                iDataType = TDataType( KWMLMimeType );
                iConfidence = ECertain;
                }
#else
            if ( CheckForOPML( aBuffer ) )
                {
                iDataType = TDataType( KOPMLMimeType );
                iConfidence = ECertain;
                }
#endif // BRDO_WML_DISABLED_FF                 
            else if ( CheckForHtml( aBuffer ) )
                {
                if ( CheckForXhtml( aBuffer ) )
                    {
                    iDataType = TDataType( KXHTMLMimeType2 );
                    }
                else
                    {
                    iDataType = TDataType( KHTMLMimeType );
                    }
                iConfidence = ECertain;
                }
            }
    }


// We support only wml version 1.1, 1.2 and 1.3
TBool CBrowserRecognizer::CheckWbxmlVersion( TUint8 byte )
    {
    switch( byte )
        {
        case 0x01: // wml version 1.1
        case 0x02: // wml version 1.2
        case 0x03: // wml version 1.3
            return ETrue;
        default:
            return EFalse;
        }
    }

// We support only wml version 1.1, 1.2 and 1.3
TBool CBrowserRecognizer::CheckWbxmlPubId( TUint8 byte )
    {
    switch( byte )
        {
        case 0x00: // String table follows.
        case 0x04: // "-//WAPFORUM//DTD WML 1.1//EN (WML 1.1)
        case 0x09: // "-//WAPFORUM//DTD WML 1.2//EN (WML 1.2)
        case 0x0A: // "-//WAPFORUM//DTD WML 1.2//EN (WML 1.3)
            return ETrue;
        default:
            return EFalse;
        }
    }

//
// Check that the content has certain bytes is correct order
// according to the wmlc spec.
//
TBool CBrowserRecognizer::CheckForWMLC( const TDesC8& aBuffer )
    {
    TInt IndexOfxmlVer       = 0;
    TInt IndexOfpubId        = 1;
    TInt IndexOfstrTblLen    = 3;
    TInt IndexOfstrTblStart  = 4;
    TInt root                = -1;

    if ( aBuffer.Length() <=  IndexOfstrTblStart )
        return EFalse;

    if ( !CheckWbxmlVersion( aBuffer[IndexOfxmlVer] ) ||
        !CheckWbxmlPubId( aBuffer[IndexOfpubId] ) )
        return EFalse;

    // Skip chararcter set

    // if the string table length is less than zero or more that the
    // length of the buffer we cannot find a root element.
    if ( aBuffer[IndexOfstrTblLen] > aBuffer.Length() - IndexOfstrTblLen )
        return EFalse;

    root = aBuffer[IndexOfstrTblStart + aBuffer[IndexOfstrTblLen]];

    // Check if the root element is <wml>
    if ( root == 0x7F )
        return ETrue;

    return EFalse;
    }

//
// From RFC 3236 "5. Recognizing XHTML files"
//
TBool CBrowserRecognizer::CheckForHtml( const TDesC8& aBuffer )
    {
    _LIT8( html, "<html" );
    _LIT8( script, "<script" );
    if ( aBuffer.FindF( html ) != KErrNotFound )
        return ETrue;
    if ( aBuffer.FindF( script ) != KErrNotFound )
        return ETrue;
    return EFalse;
    }

//
// From RFC 3236 "5. Recognizing XHTML files"
//
TBool CBrowserRecognizer::CheckForXhtml( const TDesC8& aBuffer )
    {
    _LIT8( dtd, "//DTD XHTML" );
    if ( aBuffer.FindF( dtd ) != KErrNotFound )
        return ETrue;
    return EFalse;
    }

TBool CBrowserRecognizer::CheckForWML( const TDesC8& aBuffer )
    {
    _LIT8( dtd, "-//WAPFORUM//DTD WML" );
    if ( aBuffer.FindF( dtd ) != KErrNotFound )
        return ETrue;
    return EFalse;
    }

TBool CBrowserRecognizer::CheckForOPML( const TDesC8& aBuffer )
    {
    _LIT8( dtd, "<opml" );
    if( aBuffer.FindF( dtd ) != KErrNotFound )
        return ETrue;
    return EFalse;
    }


CApaDataRecognizerType* CBrowserRecognizer::CreateRecognizerL()
    {
    return new (ELeave) CBrowserRecognizer();
    }

const TImplementationProxy ImplementationTable[] =
    {
//        { { KBrowserRecognizerImplUIDValue}, CBrowserRecognizer::CreateRecognizerL}

        IMPLEMENTATION_PROXY_ENTRY(KBrowserRecognizerImplUIDValue, CBrowserRecognizer::CreateRecognizerL)
    };


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
