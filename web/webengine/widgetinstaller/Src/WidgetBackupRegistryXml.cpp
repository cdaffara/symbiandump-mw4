//
// ==============================================================================
//  Name        : WidgetBackupRegistryXml.cpp
//  Part of     : WidgetRegistry
//  Interface   :
//  Description : Processes registry persistent data in XML.
//  Version     : 5.0
//
//  Copyright © 2008 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ==============================================================================
///

// INCLUDE FILES

#include <e32base.h>
#include <f32file.h>
#include <libxml2_globals.h>
#include <libxml2_parser.h>
#include <charconv.h>
#include "WidgetBackupRegistryXml.h"

_LIT( KPropertyListVersion,  "PropertyListVersion" );
_LIT( KBundleIdentifier,     "BundleIdentifier" );
_LIT( KBundleName,           "BundleName" );
_LIT( KBundleDisplayName,    "BundleDisplayName" );
_LIT( KMainHTML,             "MainHTML" );
_LIT( KBundleShortVersion,   "BundleShortVersion" );
_LIT( KBundleVersion,        "BundleVersion" );
_LIT( KHeight,               "Height" );
_LIT( KWidth,                "Width" );
_LIT( KAllowFullAccess,      "AllowFullAccess" );
_LIT( KAllowNetworkAccess,   "AllowNetworkAccess" );
_LIT( KDriveName,            "DriveName" );
_LIT( KBasePath,             "BasePath" );
_LIT( KIconPath,             "IconPath" );
_LIT( KUrl,                  "Url" );
_LIT( KFileSize,             "FileSize" );
_LIT( KUid,                  "Uid" );
_LIT( KNokiaWidget,          "NokiaWidget" );
_LIT( KMiniViewEnabled,      "MiniViewEnabled" );
_LIT( KBlanketPermGranted,   "BlanketPermissionGranted" );         // optional
_LIT( KPreInstalled,         "PreInstalled" );                     // optional

// ============================================================================
// CWidgetBackupRegistryXml::NewL()
// two-phase constructor
//
// @since 5.0
// ============================================================================
//
CWidgetBackupRegistryXml* CWidgetBackupRegistryXml::NewL()
    {
    CWidgetBackupRegistryXml *self = new (ELeave) CWidgetBackupRegistryXml();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ============================================================================
// CWidgetBackupRegistryXml::CWidgetBackupRegistryXml()
// C++ constructor
//
// @since 5.0
// ============================================================================
//
CWidgetBackupRegistryXml::CWidgetBackupRegistryXml() : iProperties(EWidgetPropertyIdCount)
    {
    }

// ============================================================================
// CWidgetUIConfigHandler::ConstructL()
// C++ constructor
//
// @since 5.0
// ============================================================================
//
void CWidgetBackupRegistryXml::ConstructL()
    {
    TWidgetProperty property;

    property.id = EWidgetPropertyListVersion;
    property.name.Set( KPropertyListVersion );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EBundleIdentifier;
    property.name.Set( KBundleIdentifier );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EBundleName;
    property.name.Set( KBundleName );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EBundleDisplayName;
    property.name.Set( KBundleDisplayName );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EMainHTML;
    property.name.Set( KMainHTML );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EBundleShortVersion;
    property.name.Set( KBundleShortVersion );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EBundleVersion;
    property.name.Set( KBundleVersion );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EHeight;
    property.name.Set( KHeight );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EWidth;
    property.name.Set( KWidth );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EAllowFullAccess;
    property.name.Set( KAllowFullAccess );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EAllowNetworkAccess;
    property.name.Set( KAllowNetworkAccess );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EDriveName;
    property.name.Set( KDriveName );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EBasePath;
    property.name.Set( KBasePath );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EIconPath;
    property.name.Set( KIconPath );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EUrl;
    property.name.Set( KUrl );
    property.type = EWidgetPropTypeString;
    iProperties.AppendL(property);
    //
    property.id = EFileSize;
    property.name.Set( KFileSize );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EUid;
    property.name.Set( KUid );
    property.type = EWidgetPropTypeInt; // not TUid
    iProperties.AppendL(property);
    //
    property.id = ENokiaWidget;
    property.name.Set( KNokiaWidget );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EMiniViewEnable;
    property.name.Set( KMiniViewEnabled );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EBlanketPermGranted;
    property.name.Set( KBlanketPermGranted );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    //
    property.id = EPreInstalled;
    property.name.Set( KPreInstalled );
    property.type = EWidgetPropTypeInt;
    iProperties.AppendL(property);
    }

// ============================================================================
// CWidgetBackupRegistryXml::~CWidgetBackupRegistryXml()
// destructor
//
// @since 5.0
// ============================================================================
//
CWidgetBackupRegistryXml::~CWidgetBackupRegistryXml()
    {
    for (TInt i = iProperties.Count() - 1; i >= EWidgetPropertyIdCount; i--)
        {
        TUint16* name = const_cast<TUint16*>(iProperties[i].name.Ptr());
        iProperties[i].name.Set(KNullDesC);
        delete [] name;
        }
    iProperties.Reset();
    iProperties.Close();
    }

// ============================================================================
// Get the property descriptiont by finding entry for name.
//
// @param aPropName The name of the prop: <prop>propName</prop>
// @since 5.0
// @return prop type.
// ============================================================================
//
TInt CWidgetBackupRegistryXml::GetPropertyId(
    const TDesC& aPropName )
    {
    TInt i = 0;
    for (; i < iProperties.Count(); ++i )
        {
        // use case insensitive match for property names
        if ( 0 == aPropName.CompareF( iProperties[i].name ) )
            {
            return iProperties[i].id;
            }
        }
    TUint16* name = NULL;
    name = new TUint16 [aPropName.Length()];
    if (name)
        {
        TPtr namePtr(name, aPropName.Length());
        namePtr.Copy(aPropName);
        TWidgetProperty property;
        property.id = iProperties.Count();
        property.name.Set( namePtr );
        property.type = EWidgetPropTypeUnknown;
        TInt err = iProperties.Append(property);
        if (err == KErrNone)
            {
            return iProperties.Count() - 1;
            }
        delete name;
        }
    return EWidgetPropertyIdInvalid;
    }

// ============================================================================
// Get the property name from the property id. Used in entry Externalize
//
// @param aPropertyId property id
// @since 5.0
// @return property name.
// ============================================================================
//

const TPtrC& CWidgetBackupRegistryXml::XmlPropertyName(
    TInt aPropertyId )
    {
    for ( TInt i = 0; i < iProperties.Count(); i++ )
        {
        if ( iProperties[i].id == aPropertyId )
            {
            return iProperties[i].name;
            }
        }
    return iProperties[0].name; // should never get here
    }


// ============================================================================
// CWidgetBackupRegistryXml::ToUnicodeL
// Utility to bundle transcoding to unicode steps.
//
// @since 5.0
// @param aEncoding input buffer encoding
// @param aUnicodeSizeMultiplier how many bytes of input make one unicode char
// @param aInBuf input data in encoding
// @param aOutBuf malloc'ed output buf, caller takes ownership
// @param aFileSession CCnvCharacterSetConverter requires it
// ============================================================================
//
void CWidgetBackupRegistryXml::ToUnicodeL( TInt aEncoding,
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

    CleanupStack::Pop( outBuf );
    *aOutBuf = outBuf;
    }

// ============================================================================
// CWidgetBackupRegistryXml::FromUnicodeL
// Utility to bundle transcoding to unicode steps.
//
// @since 5.0
// @param aEncoding input buffer encoding
// @param aUnicodeSizeMultiplier how many bytes of input make one unicode char
// @param aInBuf input data in encoding
// @param aOutBuf malloc'ed output buf, caller takes ownership
// @param aFileSession CCnvCharacterSetConverter requires it
// ============================================================================
//
void CWidgetBackupRegistryXml::FromUnicodeL( TInt aEncoding,
                                     TInt aUnicodeSizeMultiplier,
                                     TPtrC16 aInBuf, HBufC8** aOutBuf,
                                     RFs& aFileSession )
    {
    *aOutBuf = NULL;

    // outbuf sizing and alloction
    HBufC8* outBuf = HBufC8::NewLC(aUnicodeSizeMultiplier * (aInBuf.Length() + 1));
    TPtr8 outPtr = outBuf->Des();

    // convert from unicode
    CCnvCharacterSetConverter* charConv = CCnvCharacterSetConverter::NewLC();
    charConv->PrepareToConvertToOrFromL( aEncoding, aFileSession );
    User::LeaveIfError(
        charConv->ConvertFromUnicode( outPtr, aInBuf));
    outPtr.ZeroTerminate();
    CleanupStack::PopAndDestroy( charConv );

    CleanupStack::Pop( outBuf ); 
    *aOutBuf = outBuf;
    }

// ============================================================================
// CWidgetBackupRegistryXml::GetContentL
// Utility to bundle extraction of XML text content
//
// @since 5.0
// @param aEncoding input buffer encoding
// @param aUnicodeSizeMultiplier how many bytes of input make one unicode char
// @param aInBuf input data in encoding
// @param aOutBuf malloc'ed output buf, caller takes ownership
// @param aFileSession CCnvCharacterSetConverter requires it
// ============================================================================
//
void CWidgetBackupRegistryXml::GetContentL( RFs& aFileSession,
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
// CWidgetBackupRegistryXml::GetTextContentAsStringL
//
// ============================================================================
//
void CWidgetBackupRegistryXml::GetTextContentAsStringL( RFs& aFileSession, xmlDocPtr aDoc,
                                                  xmlNode* aNode, HBufC** aContent )
    {
    HBufC* tmpBuf = NULL;
    TInt len = 0;
    GetSubtreeAsStringL (aFileSession, aDoc, aNode, NULL, len);
    tmpBuf = HBufC::NewLC(len);
    GetSubtreeAsStringL (aFileSession, aDoc, aNode, &tmpBuf, len);
    *aContent = tmpBuf;
    CleanupStack::Pop(); // tmpBuf
    }

void  CWidgetBackupRegistryXml::GetSubtreeAsStringL (RFs& aFileSession, xmlDocPtr aDoc, xmlNode* aNode, HBufC** aBuf, TInt& aLen)
    {
    xmlNode* node = aNode;
    switch (node->type)
        {
        case XML_ELEMENT_NODE:
            {
            const xmlChar* name = node->name;
            TPtrC8 tmpName(name);
            if (aBuf)
                {
                HBufC* tmpBuf = NULL;
                ToUnicodeL( KCharacterSetIdentifierUtf8, 2,
                            tmpName, &tmpBuf, aFileSession );
                CleanupStack::PushL(tmpBuf);
                (*aBuf)->Des().Append(_L("<"));
                (*aBuf)->Des().Append(*tmpBuf);
                (*aBuf)->Des().Append(_L(">"));
                if (node->children)
                    {
                    GetSubtreeAsStringL (aFileSession, aDoc, node->children, aBuf, aLen);
                    }
                (*aBuf)->Des().Append(_L("</"));
                (*aBuf)->Des().Append(*tmpBuf);
                (*aBuf)->Des().Append(_L(">"));
                CleanupStack::PopAndDestroy(tmpBuf);
                }
            else
                {
                aLen += (5 + 2 * tmpName.Length());
                if (node->children)
                    {
                    GetSubtreeAsStringL (aFileSession, aDoc, node->children, aBuf, aLen);
                    }
                }
            break;
            }
        //case XML_ATTRIBUTE_NODE:
        case XML_TEXT_NODE:
            {
            xmlChar* content = node->content;
            TPtrC8 tmpContent(content);
            if (aBuf)
                {
                HBufC* tmpBuf = NULL;
                xmlChar* encodedContent = EncodeStringL(aDoc, content);
                CleanupStack::PushL( encodedContent );
                TPtrC8 encodedContentPtr(encodedContent);
                ToUnicodeL( KCharacterSetIdentifierUtf8, 2,
                            encodedContentPtr, &tmpBuf, aFileSession );
                CleanupStack::PushL(tmpBuf);
                (*aBuf)->Des().Append(*tmpBuf);
                CleanupStack::PopAndDestroy(2); // encodedContent, tmpBuf
                }
            else
                {
                aLen += EncodedStringLength(tmpContent);
                }
            break;
            }
        case XML_CDATA_SECTION_NODE:
            {
            xmlChar* content = node->content;
            TPtrC8 tmpContent(content);
            if (aBuf)
                {
                HBufC* tmpBuf = NULL;
                ToUnicodeL( KCharacterSetIdentifierUtf8, 2,
                            content, &tmpBuf, aFileSession);
                CleanupStack::PushL(tmpBuf);
                (*aBuf)->Des().Append(_L("<![CDATA["));
                (*aBuf)->Des().Append(*tmpBuf);
                (*aBuf)->Des().Append(_L("]]>"));
                CleanupStack::PopAndDestroy(); // tmpBuf
                }
            else
                {
                aLen += (12 + tmpContent.Length());
                }
            break;
            }
        //case XML_ENTITY_REF_NODE:
        //case XML_ENTITY_NODE:
        //case XML_PI_NODE:
        //case XML_COMMENT_NODE:
        //case XML_DOCUMENT_NODE:
        //case XML_DOCUMENT_TYPE_NODE:
        //case XML_DOCUMENT_FRAG_NODE:
        //case XML_NOTATION_NODE:
        //case XML_HTML_DOCUMENT_NODE:
        //case XML_DTD_NODE:
        //case XML_ELEMENT_DECL:
        //case XML_ATTRIBUTE_DECL:
        //case XML_ENTITY_DECL:
        //case XML_NAMESPACE_DECL:
        //case XML_XINCLUDE_START:
        //case XML_XINCLUDE_END:
        }
    if (node->next)
        {
        node = node->next;
        GetSubtreeAsStringL(aFileSession, aDoc, node, aBuf, aLen);
        }
    }

xmlChar* CWidgetBackupRegistryXml::EncodeStringL(xmlDocPtr aDoc, xmlChar* aStringToConvert)
    {
    xmlChar* noEntitiesContent = xmlEncodeSpecialChars(aDoc, aStringToConvert);
    if ( NULL == noEntitiesContent )
        {
        User::Leave( OOM_FLAG ? KErrNoMemory : KErrCorrupt );
        }
    return noEntitiesContent;
    }

HBufC* CWidgetBackupRegistryXml::EncodeStringL(xmlDocPtr aDoc, TPtrC aStringToConvert, RFs& aFileSession)
    {
    HBufC8* out = NULL;
    FromUnicodeL( KCharacterSetIdentifierUtf8, 2, aStringToConvert, &out, aFileSession );
    CleanupStack::PushL(out);
    xmlChar* noEntitiesContent = xmlEncodeSpecialChars(aDoc, out->Des().Ptr());
    if ( NULL == noEntitiesContent )
        {
        User::Leave( OOM_FLAG ? KErrNoMemory : KErrCorrupt );
        }
    CleanupStack::PushL( noEntitiesContent );
    TPtrC8 noEntitiesPtr(noEntitiesContent);
    HBufC* noEntitiesBuf = NULL;
    noEntitiesBuf = HBufC::NewL(noEntitiesPtr.Length());
    noEntitiesBuf->Des().Copy(noEntitiesPtr);
    CleanupStack::PopAndDestroy(2, out); // out, noEntitiesContent

    return noEntitiesBuf;
    }


TInt CWidgetBackupRegistryXml::EncodedStringLength(TPtrC8 aStringToConvert)
    {
    _LIT(KEntity, "\"&\r<>");
    if (aStringToConvert.Length() == 0 ) return 0;
    TInt entityLength[] = {6,5,5,4,4};
    TInt i;
    TInt count = 0;
    for (i = 0; i < aStringToConvert.Length(); i++)
        {
        TInt entityIndex = KEntity().Locate(aStringToConvert[i]);
        if (entityIndex != KErrNotFound)
            {
            count+= entityLength[entityIndex];
            }
        else
            {
            count++;
            }
        }
    return count;
    }
