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
* Description:  Widget installer info file parsing.
*
*/


#include <e32base.h>
#include <f32file.h>
#include <centralrepository.h>
#include <libxml2_globals.h>
#include <libxml2_parser.h>
#include "WidgetConfigHandler.h"
#include "SWInstWidgetUid.h"
#include "WidgetInstallerInternalCRKeys.h"
#include <charconv.h>
#include <WidgetRegistryConstants.h>


// DTD

_LIT8( KNokiaId, "-//Nokia//DTD PLIST" );  // only the first part without version
_LIT8( KAppleId, "-//Apple Computer//DTD PLIST" ); // only the first part without version

// parsed elements in the DTD (XML element names are case sensitive)

_LIT8( KKey,"key" );
_LIT8( KString,"string" );
_LIT8( KInt,"integer" );
_LIT8( KTrue,"true" );
_LIT8( KFalse,"false" );

// properties from widget metadata file

// NOTE: Comments "required" and "optional" apply to our requirements
// for registering a widget.

// NOTE: These property names are not XML element names, they are text
// content for the <key> element, so we have made the match case
// insensitive and capitalization here is just for readability.

// Apple unique key names
_LIT( KAppleBundleIdentifier,      "CFBundleIdentifier" );      // required
_LIT( KAppleDisplayName,           "CFBundleDisplayName" );     // required
_LIT( KAppleBundleVersion,         "CFBundleVersion" );         // optional
_LIT( KAppleAllowFullAccess,       "AllowFullAccess" );         // optional
// For Apple, AllowFullAccess is mapped to AllowNetworkAccess unless
// AllowNetworkAccess appears too.

// Nokia unique key names
_LIT( KNokiaIdentifier,            "Identifier" );              // required
_LIT( KNokiaDisplayName,           "DisplayName" );             // required
_LIT( KNokiaVersion,               "Version" );                 // optional
_LIT( KNokiaMiniViewEnabled,       "MiniViewEnabled" );         // optional
_LIT( KBlanketPermGranted,          "BlanketPermissionGranted" );// optional 
_LIT( KPreInstalled,               "PreInstalled" );            // optional

// Apple/Nokia shared key names
_LIT( KMainHTML,                   "MainHTML" );                // required
_LIT( KAllowNetworkAccess,         "AllowNetworkAccess" );      // optional
_LIT( KHeight,                     "Height" );                  // optional
_LIT( KWidth,                      "Width" );                   // optional


enum TWidgetPropertyFlag
    {
    EMandatory = 1,
    EConfig = 2,
    EApple = 4,
    ENokia = 8,
    ESpecial = 16
    };

#define CONFIG_MUST ( EMandatory | EConfig )
#define CONFIG_APPLE_MUST ( EMandatory | EConfig | EApple )
#define CONFIG_NOKIA_MUST ( EMandatory | EConfig | ENokia )

#define CONFIG_MAY ( EConfig )
#define CONFIG_APPLE_MAY ( EConfig | EApple )
#define CONFIG_NOKIA_MAY ( EConfig | ENokia )

#define CONFIG_APPLE_SPECIAL ( EConfig | EApple | ESpecial )

// ============================================================================
// Traverse to the next Node
//
// @param aNode: current node
// @since 3.1
// @return next node
// ============================================================================
//
xmlNode* CWidgetConfigHandler::TraverseNextNode( xmlNode* n )
    {
    // depth first
    if ( n->children )
        {
        n = n->children;
        }
    else
        {
        // go up while no sibling
        while ( n->parent && !n->next )
            {
            n = n->parent;
            }
        // sibling?
        if ( n->next )
            {
            n = n->next;
            }
        else // done
            {
            n = NULL;
            }
        }
    return n;
    }

// ============================================================================
// CWidgetConfigHandler::NewL()
// two-phase constructor
//
// @since 3.1
// ============================================================================
//
CWidgetConfigHandler* CWidgetConfigHandler::NewL()
    {
    CWidgetConfigHandler *self = new (ELeave) CWidgetConfigHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ============================================================================
// CWidgetConfigHandler::CWidgetConfigHandler()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetConfigHandler::CWidgetConfigHandler()
    {
    iProperties[EPropertyDescriptionAppleBundleIdentifier].id = EBundleIdentifier;
    iProperties[EPropertyDescriptionAppleBundleIdentifier].name.Set( KAppleBundleIdentifier );
    iProperties[EPropertyDescriptionAppleBundleIdentifier].type = EWidgetPropTypeString;
    iProperties[EPropertyDescriptionAppleBundleIdentifier].flags = CONFIG_APPLE_MUST;
    //
    iProperties[EPropertyDescriptionAppleBundleDisplayName].id = EBundleDisplayName;
    iProperties[EPropertyDescriptionAppleBundleDisplayName].name.Set( KAppleDisplayName );
    iProperties[EPropertyDescriptionAppleBundleDisplayName].type = EWidgetPropTypeString;
    iProperties[EPropertyDescriptionAppleBundleDisplayName].flags = CONFIG_APPLE_MUST;
    //
    iProperties[EPropertyDescriptionAppleBundleVersion].id = EBundleVersion;
    iProperties[EPropertyDescriptionAppleBundleVersion].name.Set( KAppleBundleVersion );
    iProperties[EPropertyDescriptionAppleBundleVersion].type = EWidgetPropTypeString;
    iProperties[EPropertyDescriptionAppleBundleVersion].flags = CONFIG_APPLE_MAY;
    //
    iProperties[EPropertyDescriptionAppleAllowFullAccess].id = EWidgetPropertyIdInvalid;
    iProperties[EPropertyDescriptionAppleAllowFullAccess].name.Set( KAppleAllowFullAccess );
    iProperties[EPropertyDescriptionAppleAllowFullAccess].type = EWidgetPropTypeBool;
    iProperties[EPropertyDescriptionAppleAllowFullAccess].flags = CONFIG_APPLE_SPECIAL;
    //
    iProperties[EPropertyDescriptionNokiaIdentifier].id = EBundleIdentifier;
    iProperties[EPropertyDescriptionNokiaIdentifier].name.Set( KNokiaIdentifier );
    iProperties[EPropertyDescriptionNokiaIdentifier].type = EWidgetPropTypeString;
    iProperties[EPropertyDescriptionNokiaIdentifier].flags = CONFIG_NOKIA_MUST;
    //
    iProperties[EPropertyDescriptionNokiaDisplayName].id = EBundleDisplayName;
    iProperties[EPropertyDescriptionNokiaDisplayName].name.Set( KNokiaDisplayName );
    iProperties[EPropertyDescriptionNokiaDisplayName].type = EWidgetPropTypeString;
    iProperties[EPropertyDescriptionNokiaDisplayName].flags = CONFIG_NOKIA_MUST;
    //
    iProperties[EPropertyDescriptionNokiaVersion].id = EBundleVersion;
    iProperties[EPropertyDescriptionNokiaVersion].name.Set( KNokiaVersion );
    iProperties[EPropertyDescriptionNokiaVersion].type = EWidgetPropTypeString;
    iProperties[EPropertyDescriptionNokiaVersion].flags = CONFIG_NOKIA_MAY;
    //
    iProperties[EPropertyDescriptionMainHTML].id = EMainHTML;
    iProperties[EPropertyDescriptionMainHTML].name.Set( KMainHTML );
    iProperties[EPropertyDescriptionMainHTML].type = EWidgetPropTypeString;
    iProperties[EPropertyDescriptionMainHTML].flags = CONFIG_MUST;
    //
    iProperties[EPropertyDescriptionAllowNetworkAccess].id = EAllowNetworkAccess;
    iProperties[EPropertyDescriptionAllowNetworkAccess].name.Set( KAllowNetworkAccess );
    iProperties[EPropertyDescriptionAllowNetworkAccess].type = EWidgetPropTypeBool;
    iProperties[EPropertyDescriptionAllowNetworkAccess].flags = CONFIG_MAY;
    //
    iProperties[EPropertyDescriptionHeight].id = EHeight;
    iProperties[EPropertyDescriptionHeight].name.Set( KHeight );
    iProperties[EPropertyDescriptionHeight].type = EWidgetPropTypeInt;
    iProperties[EPropertyDescriptionHeight].flags = CONFIG_MAY;
    //
    iProperties[EPropertyDescriptionWidth].id = EWidth;
    iProperties[EPropertyDescriptionWidth].name.Set( KWidth );
    iProperties[EPropertyDescriptionWidth].type = EWidgetPropTypeInt;
    iProperties[EPropertyDescriptionWidth].flags = CONFIG_MAY;
	//
	iProperties[EPropertyDescriptionNokiaMiniViewEnable].id = EMiniViewEnable;
    iProperties[EPropertyDescriptionNokiaMiniViewEnable].name.Set( KNokiaMiniViewEnabled );
    iProperties[EPropertyDescriptionNokiaMiniViewEnable].type = EWidgetPropTypeBool;
    iProperties[EPropertyDescriptionNokiaMiniViewEnable].flags = CONFIG_NOKIA_MAY;
    //
    iProperties[EPropertyDescriptionNokiaPromptNeeded].id = EBlanketPermGranted;
    iProperties[EPropertyDescriptionNokiaPromptNeeded].name.Set( KBlanketPermGranted );
    iProperties[EPropertyDescriptionNokiaPromptNeeded].type = EWidgetPropTypeBool;
    iProperties[EPropertyDescriptionNokiaPromptNeeded].flags = CONFIG_NOKIA_MAY;
    //
    iProperties[EPropertyDescriptionNokiaPreInstalled].id = EPreInstalled;
    iProperties[EPropertyDescriptionNokiaPreInstalled].name.Set( KPreInstalled );
    iProperties[EPropertyDescriptionNokiaPreInstalled].type = EWidgetPropTypeBool;
    iProperties[EPropertyDescriptionNokiaPreInstalled].flags = CONFIG_NOKIA_MAY;

    }

// ============================================================================
// CWidgetConfigHandler::ConstructL()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
void CWidgetConfigHandler::ConstructL()
    {
    }

// ============================================================================
// CWidgetConfigHandler::~CWidgetConfigHandler()
// destructor
//
// @since 3.1
// ============================================================================
//
CWidgetConfigHandler::~CWidgetConfigHandler()
    {
    }

// ============================================================================
// Get the key type out of the keyname
//
// @param aKeyName The name of the key: <key>KeyName</key>
// @since 3.1
// @return Key type.
// ============================================================================
//
TWidgetPropertyId CWidgetConfigHandler::GetPropertyId(
    const TDesC& aKeyName,
    DtdType aDtdType,
    TWidgetPropertyDescriptionId& aPropertyDescriptionId )
    {
    aPropertyDescriptionId = EPropertyDescriptionIdInvalid;
    TInt i = 0;
    for (; i < EPropertyDescriptionIdCount; ++i )
        {
        if ( (EDtdTypeApple == aDtdType)
             && ( iProperties[i].flags & ENokia ) )
            {
            continue;
            }
        if ( (EDtdTypeNokia == aDtdType)
             && ( iProperties[i].flags & EApple ) )
            {
            continue;
            }
        // we use case insensitive match for property names
        if ( 0 == aKeyName.CompareF( iProperties[i].name ) )
            {
            aPropertyDescriptionId = static_cast<TWidgetPropertyDescriptionId>(i);
            if ( iProperties[i].flags & ESpecial )
                {
                return EWidgetPropertyIdInvalid;
                }
            return iProperties[i].id;
            }
        }
    return EWidgetPropertyIdInvalid;
    }

// ============================================================================
// CWidgetConfigHandler::ToUnicodeL
// Utility to bundle transcoding to unicode steps.
//
// @since 3.1
// @param aEncoding input buffer encoding
// @param aUnicodeSizeMultiplier how many bytes of input make one unicode char
// @param aInBuf input data in encoding
// @param aOutBuf malloc'ed output buf, caller takes ownership
// @param aFileSession CCnvCharacterSetConverter requires it
// ============================================================================
//
void CWidgetConfigHandler::ToUnicodeL( TInt aEncoding,
                                       TInt aUnicodeSizeMultiplier,
                                       TPtrC8 aInBuf, HBufC16** aOutBuf,
                                       RFs& aFileSession )
    {
    *aOutBuf = NULL;

    // outbuf sizing and alloction
    HBufC16* outBuf = HBufC16::NewLC(aUnicodeSizeMultiplier * aInBuf.Length());
    TPtr16 outPtr = outBuf->Des();

    // convert to unicode
    CCnvCharacterSetConverter* charConv = CCnvCharacterSetConverter::NewLC();
    charConv->PrepareToConvertToOrFromL( aEncoding, aFileSession );
    TInt state = CCnvCharacterSetConverter::KStateDefault;
    TInt rep = 0; // number of unconvertible characters
    TInt rIndx = 0; // index of first unconvertible character
    User::LeaveIfError(
        charConv->ConvertToUnicode( outPtr, aInBuf, state, rep, rIndx ) );
    CleanupStack::PopAndDestroy( charConv );

    CleanupStack::Pop(); // outBuf
    *aOutBuf = outBuf;
    }

// ============================================================================
// CWidgetConfigHandler::GetContentL
// Utility to bundle extraction of XML text content
//
// @since 3.1
// @param aEncoding input buffer encoding
// @param aUnicodeSizeMultiplier how many bytes of input make one unicode char
// @param aInBuf input data in encoding
// @param aOutBuf malloc'ed output buf, caller takes ownership
// @param aFileSession CCnvCharacterSetConverter requires it
// ============================================================================
//
void CWidgetConfigHandler::GetContentL( RFs& aFileSession,
                                        xmlDocPtr aDoc,
                                        xmlNode* aNode,
                                        HBufC** aContent )
    {
    // xml uses UTF-8 for the internal representation
    xmlChar* xmlContent =
        xmlNodeListGetString( aDoc, aNode, 1 /* expand entities inline */);
    if ( NULL == xmlContent )
        {
        User::Leave( OOM_FLAG ? KErrNoMemory : KErrCorrupt );
        }
    // we must transcode UTF-8 to UCS-2 (historical
    // and now inaccurate name "unicode")
    CleanupStack::PushL( xmlContent );
    TPtrC8 content( xmlContent );
    ToUnicodeL( KCharacterSetIdentifierUtf8, 2,
                content, aContent, aFileSession );
    CleanupStack::PopAndDestroy(); // xmlContent equivalent to xmlFree()
    if ( NULL == *aContent )
        {
        User::Leave( KErrCorrupt );
        }
    }

// ============================================================================
// CWidgetConfigHandler::ParseValidateBundleMetadataL
// Parse the widget info file and create widget entry
// check for required keys and values
//
// @since 3.1
// @param aBuffer The buffer contains widget info file content.
// @param aPropertyValues output filled with parsed values from buf
// ============================================================================
//
void CWidgetConfigHandler::ParseValidateBundleMetadataL(
    TPtr8 aBuffer,
    RPointerArray<CWidgetPropertyValue>& aPropertyValues,
    RFs& aFileSession )
    {
    /*
      steps: 1. parse bundle metadata (ex., info.plist) and put
      results in aPropertyValues; 2. validate the metadata 2a. are
      required keys present? 2b. are values sane?

      leaves: 1. doesn't parse -> KErrCorrupt; 2. DTD not Nokia and
      cenrep key KWidgetInstallerStrictMode is 1 -> KErrNotSupported;
      3. key type bool but child element not true or false ->
      KErrCorrupt; 4. key type integer but child element not integer
      -> KErrCorrupt; 5. key type integer and child element integer
      but unparsable integer value -> KErrCorrupt; 6. key type string
      and child element not string -> KErrCorrupt; 7. key type string
      and child element string but does not contain text ->
      KErrCorrupt; 8. required keys not present -> KErrNotSupported
      9. key values not sane -> KErrNotSupported; 10. Heap allocation
      failure -> KErrNoMem
    */

    TInt nokiaOnly = 0;
    TRAP_IGNORE(
        CRepository* rep = CRepository::NewL( TUid::Uid( KSWInstWidgetUIUid ) );
        rep->Get( KWidgetInstallerStrictMode, nokiaOnly );
        delete rep; );

    // initialize the parser and check compiled code matches lib version

#if 0
// xmllib has a memory leak, so in order to detect mem leaks outside
// of the xmllib, this code fills in the values that the parse step
// would and returns.  you have to fill in the values for the specific
// Info.plist you are installing

    _LIT( KIdentifier, "com.aws.widget.beta1" );
    _LIT( KName, "WeatherBug" );
    _LIT( KHtml, "index.html" );
    *(aPropertyValues[EBundleIdentifier]) = KIdentifier;
    *(aPropertyValues[EBundleDisplayName]) = KName();
    *(aPropertyValues[EMainHTML]) = KHtml;
    *(aPropertyValues[EAllowNetworkAccess]) = 1;
    *(aPropertyValues[ENokiaWidget]) = 0;

    // TODO: We decided to drop BundleName and just use
    // DisplayName but the registry code has errors in it and uses
    // BundleName when it should use DisplayName so as a workaround,
    // set BundleName to DisplayName.  Should eventually remove
    // BundleName from set of registry values.
    const TDesC& name = *(aPropertyValues[EBundleDisplayName]);
    *(aPropertyValues[EBundleName]) = name;

#else
    LIBXML_TEST_VERSION

    xmlDocPtr doc; // resulting document tree

    doc = xmlReadMemory( (const char *)aBuffer.Ptr(), aBuffer.Length(),
                         NULL, // no base URL
                         NULL, // get encoding from doc
                         XML_PARSE_NOWARNING | XML_PARSE_NONET ); // options
    // parse failed check
    if ( !doc )
      {
      xmlCleanupParser();
      User::Leave( KErrCorrupt );
      }

    // determine doctype
    xmlNode* n;
    xmlDtd* dtd = NULL;
    for ( n = doc->children; n; n = n->next )
        {
        if ( XML_DTD_NODE == n->type )
            {
            dtd = (xmlDtd*)n;
            break;
            }
        }
    iDtdType = EDtdTypeUnknown;
    if ( dtd )
        {
        TPtrC8 id( dtd->ExternalID );
        if ( 0 == id.Left(KNokiaId().Length()).Compare(KNokiaId()) )
            {
            iDtdType = EDtdTypeNokia;
            }
        else if ( 0 == id.Left(KAppleId().Length()).Compare(KAppleId()) )
            {
            iDtdType = EDtdTypeApple;
            }
        }

    // save for registry so non-nokia installed on memory cards can be blocked
    // when inserted in nokia-only configured device
    *(aPropertyValues[ENokiaWidget]) = ( (EDtdTypeNokia == iDtdType) ? 1 : 0 );

    // handle cenrep nokia only setting
    if ( (EDtdTypeUnknown == iDtdType)
         || (nokiaOnly && ( EDtdTypeNokia != iDtdType )) )
        {
        User::Leave( KErrNotSupported );
        }

    xmlNode* rootElement = xmlDocGetRootElement( doc );
    TWidgetPropertyId valId( EWidgetPropertyIdInvalid );
    TWidgetPropertyDescriptionId propertyDescriptionId( EPropertyDescriptionIdInvalid );

    for ( n = rootElement; n; n = TraverseNextNode( n ) )
        {
        if ( XML_ELEMENT_NODE == n->type )
            {
            TPtrC8 element( n->name );

            if ( 0 == element.Compare( KKey() ) )
                {
                valId = EWidgetPropertyIdInvalid;
                HBufC* keyName;
                GetContentL( aFileSession, doc, n->children, &keyName );
                CleanupStack::PushL( keyName );
                TPtr name( keyName->Des() );
                name.Trim(); // remove surrounding whitespace
                valId = GetPropertyId( name, iDtdType, propertyDescriptionId );
                CleanupStack::PopAndDestroy( keyName );

                // reject duplicate keys based on value already being
                // set (unset values have type unknown)
                if ( ( EWidgetPropertyIdInvalid != valId )
                     && ( EWidgetPropTypeUnknown
                          != aPropertyValues[valId]->iType ) )
                    {
                    User::Leave( KErrCorrupt );
                    }
                }
            else if ( EWidgetPropertyIdInvalid != valId )
                {
                switch ( iProperties[propertyDescriptionId].type )
                    {
                case EWidgetPropTypeBool:
                    // map true to 1 and false to 0
                    if ( 0 == element.Compare( KTrue ) )
                        {
                        *(aPropertyValues[valId]) = 1;
                        }
                    else if ( 0 == element.Compare( KFalse ) )
                        {
                        *(aPropertyValues[valId]) = 0;
                        }
                    else
                        {
                        User::Leave( KErrCorrupt );
                        }
                    break;
                case EWidgetPropTypeInt:
                    {
                    if ( 0 == element.Compare( KInt() ) )
                        {
                        HBufC* keyVal;
                        GetContentL( aFileSession, doc, n->children, &keyVal );
                        CleanupStack::PushL( keyVal );
                        TPtr value( keyVal->Des() );
                        value.Trim(); // remove surrounding whitespace
                        TLex tlex;
                        tlex.Assign( value );
                        TInt x;
                        TInt e = tlex.Val( x );
                        CleanupStack::PopAndDestroy( keyVal );
                        if ( !e && tlex.Eos() )
                            {
                            *(aPropertyValues[valId]) = x;
                            }
                        else
                            {
                            User::Leave( KErrCorrupt );
                            }
                        }
                    else
                        {
                        User::Leave( KErrCorrupt );
                        }
                    }
                    break;
                case EWidgetPropTypeString:
                    if ( 0 == element.Compare( KString() ) )
                        {
                        HBufC* keyVal;
                        GetContentL( aFileSession, doc, n->children, &keyVal );
                        CleanupStack::PushL( keyVal );
                        TPtr value( keyVal->Des() );
                        value.Trim(); // remove surrounding whitespace
                        *(aPropertyValues[valId]) = value;
                        CleanupStack::PopAndDestroy( keyVal );
                        }
                    else
                        {
                        User::Leave( KErrCorrupt );
                        }
                    break;
                default:
                    // ignore other things
                    break;
                    }
                }
            else if ( EPropertyDescriptionIdInvalid != propertyDescriptionId )
                {
                if ( EPropertyDescriptionAppleAllowFullAccess == propertyDescriptionId )
                    {
                    // only set if AllowNetworkAccess is not yet set
                    if ( aPropertyValues[EAllowNetworkAccess]->iType != EWidgetPropTypeInt )
                        {
                        // map true to 1 and false to 0
                        if ( 0 == element.Compare( KTrue ) )
                            {
                            *(aPropertyValues[EAllowNetworkAccess]) = 1;
                            }
                        else if ( 0 == element.Compare( KFalse ) )
                            {
                            *(aPropertyValues[EAllowNetworkAccess]) = 0;
                            }
                        else
                            {
                            User::Leave( KErrCorrupt );
                            }
                        }
					   // only set if MiniViewEnable is not yet set
                    else if ( aPropertyValues[EMiniViewEnable]->iType != EWidgetPropTypeInt )
                        {
                        // map true to 1 and false to 0
                        if ( 0 == element.Compare( KTrue ) )
                            {
                            *(aPropertyValues[EMiniViewEnable]) = 1;
                            }
                        else if ( 0 == element.Compare( KFalse ) )
                            {
                            *(aPropertyValues[EMiniViewEnable]) = 0;
                            }
                        else
                            {
                            User::Leave( KErrCorrupt );
                            }
                        }
                    }				                  
                }
            }   // if n is element
        }   // for

    // validate: all required keys are present
    TInt i = 0;
    for ( ; i < EPropertyDescriptionIdCount; ++i )
        {
        if ( (iProperties[i].flags & EMandatory)
             && (iProperties[i].flags & EConfig)
             // a prop name maps to some value id
             && ((EWidgetPropertyIdInvalid != iProperties[i].id)
                 && (aPropertyValues[iProperties[i].id]->iType
                     != iProperties[i].type)) )
            {
            // if cause of mismatch is bool mapped to int, then continue check
            if ( (EWidgetPropTypeInt != aPropertyValues[iProperties[i].id]->iType)
                 || (EWidgetPropTypeBool != iProperties[i].type) )
                {
                User::Leave( KErrNotSupported );
                }
            }
        }

    // TODO: We decided to drop BundleName and just use
    // DisplayName but the registry code has errors in it and uses
    // BundleName when it should use DisplayName so as a workaround,
    // set BundleName to DisplayName.  Should eventually remove
    // BundleName from set of registry values.
    const TDesC& name = *(aPropertyValues[EBundleDisplayName]);
    *(aPropertyValues[EBundleName]) = name;

    // validate values are sane
    ValidateL( aPropertyValues );

    xmlFreeDoc(doc);
    xmlCleanupParser();
#endif
    }

// ============================================================================
// CWidgetConfigHandler::ValidateL
// Check values.
//
// @since 3.1
// @param aPropertyValues output filled with parsed values from buf
// ============================================================================
//
void CWidgetConfigHandler::ValidateL(
    RPointerArray<CWidgetPropertyValue>& aPropertyValues )
    {
    // leaves with KErrCorrupt if any values are rejected

    // The limits and their justification:
    //
    // Strings are limited to KMaxFileName and must not be empty.
    //
    // For values like "Identifier" that will be used in filenames the
    // fundamental limit in Symbian is KMaxFileName (from e32const.h
    // and included by essentially all higher level e32*.h).  Since
    // these values will be concatenated with other directory
    // components, we might think to use a heuristic limit less than
    // KMaxFileName, but this introduces a needless arbitrary limit
    // and it will be cleaner to check length against KMaxFileName
    // after concatenation.  Checking here just means detecting a
    // limit violation earlier and maybe giving a better error
    // message.  Also, the KErrBadName error value from filesystem
    // operations will signal a pathname problem and MUST be checked.
    //
    // For strings like "DisplayName" that will be used in menus there
    // is no clear predefined value given that displays come in
    // various sizes and various fonts are used.  I'm going to impose
    // the same limit as for values used in filenames on the belief
    // that this is useful and not harmfull.

    // Also, check that values are not empty.

    TInt i = 0;
    for (; i < EPropertyDescriptionIdCount; ++i )
        {
        if ( EWidgetPropTypeString == aPropertyValues[i]->iType )
            {
            const TDesC& s = *(aPropertyValues[i]);
            if ( !s.Length() || ( s.Length() > KMaxFileName ) )
                {
                User::Leave( KErrCorrupt );
                }
            }
        }
    }

// ============================================================================
// CWidgetConfigHandler::ParseInfoLocL()
//
// @since 3.1
// ============================================================================
//
void CWidgetConfigHandler::ParseInfoLocL(
    TPtrC8 aBuffer,
    RFs& aFileSession,
    CWidgetPropertyValue& aBundleDisplayName )
    {
    // This logic is for localization of the display name.  The
    // display name appears in a localization file in the form:
    // DisplayName = "something"; (for Nokia DTD) or
    // CFBundleDisplayName = "something"; (for Apple DTD).

    // The encoding of the localization file is not given so it must
    // be automatically recognized.  The default is UTF-8 but if a
    // byte-order-mark is the first character then the BOM determines
    // the encoding.

    // get key name ID for display name depending on DTD
    TWidgetPropertyDescriptionId displayNameId = EPropertyDescriptionIdInvalid;
    if ( iDtdType == EDtdTypeNokia )
      {
        displayNameId = EPropertyDescriptionNokiaDisplayName;
      }
    else if ( iDtdType == EDtdTypeApple )
      {
        displayNameId = EPropertyDescriptionAppleBundleDisplayName;
      }
    if ( EPropertyDescriptionIdInvalid == displayNameId )
        {
        User::Leave( KErrCorrupt );
        }

    TPtrC8 inBuf;

    // default input encoding, no BOM
    TInt encoding = KCharacterSetIdentifierUtf8;
    TInt unicodeSizeMultiplier = 2; // a safe value which may waste some space
    inBuf.Set( aBuffer );

    // check for BOM, we only recognize UTF-16(BE/LE) and UTF-8,
    // remove BOM if found
    if ( aBuffer[0] == 0xFE && aBuffer[1] == 0xFF )
        {
        encoding = KCharacterSetIdentifierUnicodeBig;
        unicodeSizeMultiplier = 1;
        inBuf.Set( aBuffer.Right( aBuffer.Length() - 2 ) );
        }
    else if ( aBuffer[0] == 0xFF && aBuffer[1] == 0xFE )
        {
        encoding = KCharacterSetIdentifierUnicodeLittle;
        unicodeSizeMultiplier = 1;
        inBuf.Set( aBuffer.Right( aBuffer.Length() - 2 ) );
        }
    else if ( aBuffer[0] == 0xEF && aBuffer[1] == 0xBB && aBuffer[2] == 0xBF )
        {
        encoding = KCharacterSetIdentifierUtf8;
        unicodeSizeMultiplier = 2; // a safe value which may waste some space
        inBuf.Set( aBuffer.Right( aBuffer.Length() - 3 ) );
        }

    // convert to unicode
    HBufC16* outBuf;
    ToUnicodeL( encoding, unicodeSizeMultiplier,
                inBuf, &outBuf, aFileSession );
    CleanupStack::PushL( outBuf );
    TPtr16 outPtr = outBuf->Des();
    // convert the display name key name to unicode since it is just
    // _LIT() and so might be 8 bit and we want 16 bit "unicode"
    TPtrC keyName = iProperties[displayNameId].name;
    HBufC16* keyNameUnicode = HBufC16::NewLC( keyName.Length() );
    TPtr16 keyNameUnicodePtr( keyNameUnicode->Des() );
    keyNameUnicodePtr.Copy( keyName );

    // parse the display name
    outPtr.TrimLeft(); // remove leading whitespace
    TInt pos = outPtr.Find( keyNameUnicodePtr );
    if ( (KErrNotFound == pos) || (0 != pos) )
        {
        User::Leave( KErrCorrupt );
        }

    // rest contains buffer after the DisplayName,
    // i.e. = "some localized text"
    TPtr16 rest =
        outPtr.RightTPtr(
            outPtr.Length()
            - pos
            - keyNameUnicode->Length() );
    const TUint16* data = rest.Ptr();
    TBool hasEqual = EFalse;    // already pass the = sign?

    // start pos and end pos of localized displayname
    TUint8 start = 0;
    TUint8 end = 0;
    for ( TInt i = 0; i < rest.Length(); i++ )
        {
        if ( data[i] == ' ' || data[i] == '\t' )
            {
            continue;
            }
        if ( data[i] == '=' )
            {
            hasEqual = ETrue;
            continue;
            }
        if ( data[i] == '\"' )
            {
            if ( !start )
                {
                start = i + 1;
                continue;
                }
            // else already inquote, so this is close quote
            end = i;
            break;
            }
        if ( !hasEqual || !start )
            {
            User::Leave( KErrCorrupt );
            }
        }

    if ( end <= start )
        {
        User::Leave( KErrCorrupt );
        }
    TInt left = pos + keyNameUnicode->Length() + start;
    aBundleDisplayName = outPtr.MidTPtr( left, end - start);
    CleanupStack::PopAndDestroy( 2 ); // outBuf, keyNameUnicode
    }

// End of File
