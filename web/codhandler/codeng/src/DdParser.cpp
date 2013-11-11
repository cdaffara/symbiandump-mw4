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
*      Implementation of class TDdParser.   
*      
*
*/


// INCLUDE FILES

#include "DdParser.h"
#include "CodData.h"
#include "CodError.h"
#include "CodPanic.h"
#include "CodLogger.h"
#include "NwUtil.h"
#include "DdDummyDict.h"
#include "HttpDownloadData.h"
#include <xml/cxml/nw_dom_document.h>
#include <utf.h>

// ================= CONSTANTS =======================

// Attribute names.
_LIT( KDdName, "name" );                        ///< name.
_LIT( KDdVendor, "vendor" );                    ///< vendor.
_LIT( KDdDescription, "description" );          ///< description.
_LIT( KDdUrl, "objectURI" );                    ///< objectURI.
_LIT( KDdSize, "size" );                        ///< size.
_LIT( KDdType, "type" );                        ///< type.
_LIT( KDdInstallNotify, "installNotifyURI" );   ///< installNotifyURI.
_LIT( KDdNextUrl, "nextURL" );                  ///< nextURL.
_LIT( KDdInfoUrl, "infoURL" );                  ///< infoURL.
_LIT( KDdIcon, "iconURI" );                     ///< iconURI.
_LIT( KDdVersion, "DDVersion" );                ///< DDVersion.
_LIT( KDdMedia, "media" );                      ///< media.
// Attribute names for OMA v2 
_LIT( KDdProduct, "product" );                  ///< media.
_LIT( KDdMediaObject, "mediaObject" );          ///< mediaObject.
_LIT( KDdMeta, "meta" );                        ///< meta.
_LIT( KDdProgressiveDl, "progressiveDownloadFlag" );    ///< progressiveDownloadFlag.
_LIT( KDdLicense, "license" );                  ///< license.
_LIT( KDdOrder, "order" );                      ///< order.
_LIT( KDdSuppressUserConfirmation, "suppressUserConfirmation" ); ///< suppressUserConfirmation.
_LIT( KDdServer, "server" );                    ///< server.
_LIT( KDdText, "text" );						///< text.
_LIT( KDdTrue, "true" );                        ///< true.
_LIT( KDdFalse, "false" );                      ///< false.
_LIT( KDdAlways, "Always" );                    ///< Always.
_LIT( KDdUserConfirmStepOnly, "UserConfirmaStepOnly" );  ///< UserConfirmStepOnly.
_LIT( KDdNever, "Never" );                      ///< Never.
_LIT( KDdPost, "post" );                        ///< post.
_LIT( KDdAny, "any" );                          ///< any.
_LIT( KDdUpdatedDDURI, "updatedDDURI" );//updatedDDURI


#define INDENT_SPACE 2

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TDdParser::ParseL()
// ---------------------------------------------------------
//
void TDdParser::ParseL( const TDesC& aBuf, CCodData& aData, const TBool aIsDd2, TBool& aIsLicenseTag )
    {
    CLOG(( EParse, 2, _L("-> TDdParser::ParseL") ));
    CDUMP(( EParse, 2, _S("Buf:"), _S("    "), \
        (const TUint8*)aBuf.Ptr(), aBuf.Size() ));
    iError = KErrNone;
    iData = &aData;
    // XML Parser expects the buffer contain a BOM and be in
    // network byte order (this is hard-coded into cXmlParser).
    // We already have the buffer converted to UCS-2, native byte order, BOM
    // removed, and this cannot be changed without changing API.
    // So we have to re-add BOM and convert the buffer back to network byte
    // order. This is an annoying a waste, luckily a DD file is small
    // enough not to be significant.
    HBufC* buf = HBufC::NewLC( aBuf.Length() + 1 );
    buf->Des().Append( 0xFFFE );    // Add BOM, little endian as aBuf.
    buf->Des().Append( aBuf );      // Append buffer.
    // Now turn the whole buffer big-endian.
    TUint8* ptr = (TUint8*)buf->Ptr();
    for ( TInt i = 0; i < buf->Size(); i += 2 )
        {
        TUint8 tmp = ptr[i];
        ptr[i] = ptr[i + 1];
        ptr[i + 1] = tmp;
        }
    NW_WBXML_Dictionary_t* dictArray[ 1 ] = 
        { (NW_WBXML_Dictionary_t*)&NW_DdDummy_WBXMLDictionary };
    RNwWbxmlDictionary wbxmlDict;
    User::LeaveIfError( wbxmlDict.Initialize( 1, dictArray ) );
    CleanupClosePushL<RNwWbxmlDictionary>( wbxmlDict );

    NW_TinyDom_Handle_t domHandle;
    NW_Status_t stat;
    CNwDomDocumentNode* docNode = new (ELeave) CNwDomDocumentNode();
    CleanupStack::PushL( docNode );
    docNode->iDocNode = CXML_DOM_DocumentNode_BuildTree
        ( 
        &domHandle, 
        (NW_Byte*)buf->Ptr(), 
        (NW_Int32)buf->Size(), 
        /*encoded=*/NW_FALSE, 
        /*publicID=*/NW_DdDummy_PublicId, 
        /*extTNotStringTable=*/NW_FALSE,
        &stat
        );
    LeaveIfNwErrorL( stat );
    User::LeaveIfNull( docNode->iDocNode ); // Safety code.
    if (!aIsDd2)
	{
	  DocNodeL( docNode->iDocNode );
	}
    else
	{
	  iIsLicenseTag = EFalse;
	  ParseDd2DocNodeL( docNode->iDocNode );
	}
    CleanupStack::PopAndDestroy( 3, buf );  // docNode, close wbxmlDict, buf

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

//TODO add logs for OMA 2
#endif /* def __TEST_COD_LOG */
	
	//If there are no media objects present to download, 
    if( !iData->Count() )
    	{
    	User::Leave( KErrCodInvalidDescriptor );	
    	}
    	
    // NULL data for clarity. These are never used later, but don't keep
    // pointers to objects which are out of reach.
    iData = NULL;
    User::LeaveIfError( iError );
    aIsLicenseTag = iIsLicenseTag;
    CLOG(( EParse, 2, _L("<- TDdParser::ParseL") ));
    }

// ---------------------------------------------------------
// TDdParser::DocNodeL()
// ---------------------------------------------------------
//
void TDdParser::DocNodeL( NW_DOM_DocumentNode_t* aDocNode )
    {
    CLOG(( EParse, 2, _L("-> TDdParser::DocNodeL") ));
    __ASSERT_DEBUG( aDocNode, CodPanic( ECodInternal ) );
    // Validity checking of root element (media).
    NW_DOM_ElementNode_t* elmt = 
        NW_DOM_DocumentNode_getDocumentElement( aDocNode );
    if ( !elmt || NW_DOM_Node_getNodeType( elmt ) != NW_DOM_ELEMENT_NODE )
        {
        User::Leave( KErrCodInvalidDescriptor );
        }
    NW_String_t str;
    LeaveIfNwErrorL( NW_DOM_Node_getNodeName( elmt, &str ) );
    CleanupNwStringStoragePushL( &str );
    iEncoding = NW_DOM_DocumentNode_getCharacterEncoding( aDocNode );
    NW_Ucs2* name;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &name ) );
    CleanupNwUcs2PushL( name );
    if ( TPtrC( name ).Compare( KDdMedia() ) )
        {
        User::Leave( KErrCodInvalidDescriptor );
        }
    CleanupStack::PopAndDestroy( 2 );   // clean up name, str->storage

    // Process child element nodes.
    NW_DOM_Node_t* node = NW_DOM_Node_getFirstChild( elmt );
    CMediaObjectData *mediaObject = CMediaObjectData::NewL();
    while ( node )
        {
        if ( NW_DOM_Node_getNodeType( node ) == NW_DOM_ELEMENT_NODE )
            {
            ElementNodeL( STATIC_CAST( NW_DOM_ElementNode_t*, node ), mediaObject );
            }
        node = NW_DOM_Node_getNextSibling( node );
        }
	iData->AppendMediaObjectL( mediaObject );        
    CLOG(( EParse, 2, _L("<- TDdParser::DocNodeL") ));
    }

// ---------------------------------------------------------
// TDdParser::ParseDd2DocNodeL()
// ---------------------------------------------------------
//
void TDdParser::ParseDd2DocNodeL( NW_DOM_DocumentNode_t* aDocNode )
{
    CLOG(( EParse, 2, _L("-> TDdParser::ParseDd2DocNodeL") ));
    __ASSERT_DEBUG( aDocNode, CodPanic( ECodInternal ) );
    // Validity checking of root element (media).
    NW_DOM_ElementNode_t* elmt = 
        NW_DOM_DocumentNode_getDocumentElement( aDocNode );
    if ( !elmt || NW_DOM_Node_getNodeType( elmt ) != NW_DOM_ELEMENT_NODE )
        {
        User::Leave( KErrCodInvalidDescriptor );
        }
    NW_String_t str;
    LeaveIfNwErrorL( NW_DOM_Node_getNodeName( elmt, &str ) );
    CleanupNwStringStoragePushL( &str );
    iEncoding = NW_DOM_DocumentNode_getCharacterEncoding( aDocNode );
    NW_Ucs2* name;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &name ) );
    CleanupNwUcs2PushL( name );
    if ( TPtrC( name ).Compare( KDdMedia() ) )
        {
        User::Leave( KErrCodInvalidDescriptor );
        }
    CleanupStack::PopAndDestroy( 2 );   // clean up name, str->storage
    ParseMediaElementsL( elmt );
    CLOG(( EParse, 2, _L("<- TDdParser::ParseDd2DocNodeL") ));
}

// ---------------------------------------------------------
// TDdParser::ParseMediaElementsL()
// ---------------------------------------------------------
//
void TDdParser::ParseMediaElementsL( NW_DOM_ElementNode_t* aElement)
{
    CLOG(( EParse, 2, _L("-> TDdParser::ParseMediaElementsL") ));

    // Process child elements for Media Object
    NW_DOM_Node_t* node = NW_DOM_Node_getFirstChild( aElement );
    while ( node )
        {
        if ( NW_DOM_Node_getNodeType( node ) == NW_DOM_ELEMENT_NODE )
            {
            MediaElementNodeL( STATIC_CAST( NW_DOM_ElementNode_t*, node ) );
            }
        node = NW_DOM_Node_getNextSibling( node );
        }

	// Get Attributs for this node
    NW_Status_t stat = NW_STAT_SUCCESS;

    if ( NW_DOM_ElementNode_hasAttributes( aElement ) )
        {

        NW_DOM_AttributeListIterator_t attrIter;
        stat = NW_DOM_ElementNode_getAttributeListIterator
                             ( aElement, &attrIter );

        User::LeaveIfError( NwStatus2Error( stat ) );
        NW_DOM_AttributeHandle_t attrHandle;
        while( NW_DOM_AttributeListIterator_getNextAttribute( &attrIter, &attrHandle ) == NW_STAT_WBXML_ITERATE_MORE )
            {
            ParseNodeAttributesL( &attrHandle );
            }
        }

	CLOG(( EParse, 2, _L("<- TDdParser::ParseMediaElementsL") ));
}

// ---------------------------------------------------------
// TDdParser::ParseProductElementsL()
// ---------------------------------------------------------
//
void TDdParser::ParseProductElementsL( NW_DOM_ElementNode_t* aElement)
{
    CLOG(( EParse, 2, _L("-> TDdParser::ParseProductElementsL") ));
    // Process child elements for Product  Object
    NW_DOM_Node_t* node = NW_DOM_Node_getFirstChild( aElement );
    while ( node )
        {
        if ( NW_DOM_Node_getNodeType( node ) == NW_DOM_ELEMENT_NODE )
            {
            ProductElementNodeL( STATIC_CAST( NW_DOM_ElementNode_t*, node ) );
            }
        node = NW_DOM_Node_getNextSibling( node );
        }
	CLOG(( EParse, 2, _L("<- TDdParser::ParseProductElementsL") ));
}

// ---------------------------------------------------------
// TDdParser::ParseMediaObjectElementsL()
// ---------------------------------------------------------
//
void TDdParser::ParseMediaObjectElementsL( NW_DOM_ElementNode_t* aElement)
{
    CLOG(( EParse, 2, _L("-> TDdParser::ParseMediaObjectElementsL") ));
    //Create a media object and append it to Cod Data array
    CMediaObjectData *mediaObject = CMediaObjectData::NewL();

    // Process child elements for MediaObject
    NW_DOM_Node_t* node = NW_DOM_Node_getFirstChild( aElement );
    while ( node )
        {
        if ( NW_DOM_Node_getNodeType( node ) == NW_DOM_ELEMENT_NODE )
            {
            MediaObjElementNodeL( STATIC_CAST( NW_DOM_ElementNode_t*, node), mediaObject );
            }
        node = NW_DOM_Node_getNextSibling( node );
        }

    if(!mediaObject->Name().Compare(KNullDesC) && mediaObject->Url().Compare(KNullDesC8))
        {
        ParseNameFromUrlL( mediaObject );
        }
    iData->AppendMediaObjectL( mediaObject );              
    
	CLOG(( EParse, 2, _L("<- TDdParser::ParseMediaObjectElementsL") ));
}

// ---------------------------------------------------------
// TDdParser::ParseMetaElementsL()
// ---------------------------------------------------------
//
void TDdParser::ParseMetaElementsL( NW_DOM_ElementNode_t* aElement)
{
	CLOG(( EParse, 2, _L("-> TDdParser::ParseMetaElementsL") ));
    // Process child elements foe Media Object
    NW_DOM_Node_t* node = NW_DOM_Node_getFirstChild( aElement );
    while ( node )
        {
        if ( NW_DOM_Node_getNodeType( node ) == NW_DOM_ELEMENT_NODE )
            {
            MetaElementNodeL( STATIC_CAST( NW_DOM_ElementNode_t*, node ) );
            }
        node = NW_DOM_Node_getNextSibling( node );
        }
	CLOG(( EParse, 2, _L("<- TDdParser::ParseMetaElementsL") ));
}

// ---------------------------------------------------------
// TDdParser::ParseMetaElementsL()
// ---------------------------------------------------------
//
void TDdParser::ParseMetaElementsL( NW_DOM_ElementNode_t* aElement, CMediaObjectData *& aMediaObject )
{
	CLOG(( EParse, 2, _L("-> TDdParser::ParseMetaElementsL") ));
    // Process child elements foe Media Object
    NW_DOM_Node_t* node = NW_DOM_Node_getFirstChild( aElement );
    while ( node )
        {
        if ( NW_DOM_Node_getNodeType( node ) == NW_DOM_ELEMENT_NODE )
            {
            MetaElementNodeL( STATIC_CAST( NW_DOM_ElementNode_t*, node ), aMediaObject );
            }
        node = NW_DOM_Node_getNextSibling( node );
        }
	CLOG(( EParse, 2, _L("<- TDdParser::ParseMetaElementsL") ));
}

// ---------------------------------------------------------
// TDdParser::ParseObjectUriElementsL()
// ---------------------------------------------------------
//
void TDdParser::ParseObjectUriElementsL( NW_DOM_ElementNode_t* aElement, CMediaObjectData *& aMediaObject )
{
	CLOG(( EParse, 2, _L("-> TDdParser::ParseObjectUriElementsL") ));
    // Process child elements for Media Object
    NW_DOM_Node_t* node = NW_DOM_Node_getFirstChild( aElement );
    while ( node )
        {
        if ( NW_DOM_Node_getNodeType( node ) == NW_DOM_ELEMENT_NODE )
            {
            ObjUriElementNodeL( STATIC_CAST( NW_DOM_ElementNode_t*, node ), aMediaObject );
            }
        node = NW_DOM_Node_getNextSibling( node );
        }
	CLOG(( EParse, 2, _L("<- TDdParser::ParseObjectUriElementsL") ));
}

// ---------------------------------------------------------
// TDdParser::MediaElementNodeL()
// ---------------------------------------------------------
//
void TDdParser::MediaElementNodeL( NW_DOM_ElementNode_t* aElmtNode )
    {
    CLOG(( EParse, 2, _L("-> TDdParser::MediaElementNodeL") ));
    __ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );

    NW_String_t str;
    LeaveIfNwErrorL( NW_DOM_Node_getNodeName( aElmtNode, &str ) );
    CleanupNwStringStoragePushL( &str );
    NW_Ucs2* ucs2;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
    CleanupNwUcs2PushL( ucs2 );
    TPtrC attrName( ucs2 );
    TDdAttr attr = MediaAttribute( attrName );
    CLOG(( EParse, 4, _L("  attribute <%S> (%d)"), &attrName, attr ));
    CleanupStack::PopAndDestroy( 2 );   // Clean up ucs2, str->storage
	if ( attr == EDdProduct )
	{	
	    if( iData && !iData->Count() )
	        {
    		ParseProductElementsL(aElmtNode);
	        }
		return;
	}
    if ( attr != EDdUnknownAttr )   // Unknown attribute is ignored.
        {
        ucs2 = PcDataLC( aElmtNode );
        if ( ucs2 )
            {
            SetMediaAttrL( attr, TPtrC( ucs2 ) );
            }
        CleanupStack::PopAndDestroy();  // Clean up ucs2.
        }
    CLOG(( EParse, 2, _L("<- TDdParser::MediaElementNodeL") ));
    }

// ---------------------------------------------------------
// TDdParser::ParseNodeAttributesL()
// ---------------------------------------------------------
//
void TDdParser::ParseNodeAttributesL( NW_DOM_AttributeHandle_t* aAttrHandle )
{
	CLOG(( EParse, 2, _L("-> TDdParser::ParseNodeAttributesL") ));
    NW_Status_t stat = NW_STAT_SUCCESS;
    NW_String_t str;
    // Get the name of the attribute.
    stat = NW_DOM_AttributeHandle_getName( aAttrHandle, &str );
    User::LeaveIfError( NwStatus2Error( stat ) );

    CleanupNwStringStoragePushL( &str );
    NW_Ucs2* ucs2;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
    CleanupNwUcs2PushL( ucs2 );

    TPtrC attrName( ucs2 );
    TDdAttr attr = DdVersionAttribute( attrName );
	CleanupStack::PopAndDestroy( 2 );   // Clean up ucs2, str->storage
    if ( attr != EDdUnknownAttr )   // Unknown attribute is ignored.
		{
        LeaveIfNwErrorL(NW_DOM_AttributeHandle_getValue(aAttrHandle, &str) );
        LeaveIfNwErrorL( NW_String_stringToUCS2Char(&str, iEncoding, &ucs2) );
        if ( ucs2 )
            {
			if ( (attr == EDdVersion))
				{
				iData->SetVersionL( TPtrC ( ucs2 ));
				}
            }
        }
    CLOG(( EParse, 2, _L("<- TDdParser::ParseNodeAttributesL") ));
}

// ---------------------------------------------------------
// TDdParser::ObjUriElementNodeL()
// ---------------------------------------------------------
//
void TDdParser::ObjUriElementNodeL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject )
    {
	CLOG(( EParse, 2, _L("-> TDdParser::ObjUriElementNodeL") ));
    __ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );
    NW_String_t str;
    LeaveIfNwErrorL( NW_DOM_Node_getNodeName( aElmtNode, &str ) );
    CleanupNwStringStoragePushL( &str );
    NW_Ucs2* ucs2;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
    CleanupNwUcs2PushL( ucs2 );

    TPtrC attrName( ucs2 );
    TDdAttr attr = ObjUriAttribute( attrName );
	CleanupStack::PopAndDestroy( 2 );   // Clean up ucs2, str->storage
	TInt ok( EFalse );
    if ( attr != EDdUnknownAttr )   // Unknown attribute is ignored.
		{
        ucs2 = PcDataLC( aElmtNode );
        if ( ucs2 )
            {
			/* composite objects (multiple server elements) are not supported in 3.2
			   So, it takes last ObjectUri
			*/
			if ( (attr == EDdUrl))
				{
				ok = aMediaObject->SetUrlL( TPtrC ( ucs2 ));
				}
            }
        CleanupStack::PopAndDestroy();  // Clean up ucs2.
        }
		if ( !ok )
			{
			Error( KErrCodInvalidDescriptor );
			}
		CLOG(( EParse, 2, _L("<- TDdParser::ObjUriElementNodeL") ));
    }

// ---------------------------------------------------------
// TDdParser::MediaObjElementNodeL()
// ---------------------------------------------------------
//
void TDdParser::MediaObjElementNodeL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject )
    {
    CLOG(( EParse, 2, _L("-> TDdParser::MediaObjElementNodeL") ));
    __ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );
    NW_String_t str;
    LeaveIfNwErrorL( NW_DOM_Node_getNodeName( aElmtNode, &str ) );
    CleanupNwStringStoragePushL( &str );
    NW_Ucs2* ucs2;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
    CleanupNwUcs2PushL( ucs2 );
    TPtrC attrName( ucs2 );
    TDdAttr attr = MediaObjAttribute( attrName );
    CLOG(( EParse, 4, _L("  attribute <%S> (%d)"), &attrName, attr ));
    CleanupStack::PopAndDestroy( 2 );   // Clean up ucs2, str->storage
	if ( attr == EDdMeta )
	{
		ParseMetaElementsL(aElmtNode, aMediaObject );
		return;
	}
	if ( attr == EDdUrl )
	{
		ParseObjectUriElementsL(aElmtNode, aMediaObject );
		return;
	}
    if ( attr != EDdUnknownAttr )   // Unknown attribute is ignored.
        {
        ucs2 = PcDataLC( aElmtNode );
        if ( ucs2 )
            {
            SetMediaObjAttrL( attr, TPtrC( ucs2 ), aMediaObject );
            }
        CleanupStack::PopAndDestroy();  // Clean up ucs2.
        }
    CLOG(( EParse, 2, _L("<- TDdParser::MediaObjElementNodeL") ));
    }

// ---------------------------------------------------------
// TDdParser::ProductElementNodeL()
// ---------------------------------------------------------
//
void TDdParser::ProductElementNodeL( NW_DOM_ElementNode_t* aElmtNode )
    {
	CLOG(( EParse, 2, _L("-> TDdParser::ProductElementNodeL") ));
	__ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );
	NW_String_t str;
	LeaveIfNwErrorL( NW_DOM_Node_getNodeName( aElmtNode, &str ) );
	CleanupNwStringStoragePushL( &str );
	NW_Ucs2* ucs2;
	LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
	CleanupNwUcs2PushL( ucs2 );
	TPtrC attrName( ucs2 );
	TDdAttr attr = ProductAttribute( attrName );
	CLOG(( EParse, 4, _L("  attribute <%S> (%d)"), &attrName, attr ));
	CleanupStack::PopAndDestroy( 2 );   // Clean up ucs2, str->storage
	if ( attr == EDdMeta )
	{
		ParseMetaElementsL(aElmtNode);
	}
	if ( attr == EDdMediaObject )
	{
		ParseMediaObjectElementsL(aElmtNode);
	}
	CLOG(( EParse, 2, _L("<- TDdParser::ProductElementNodeL") ));
    }

// ---------------------------------------------------------
// TDdParser::MetaElementNodeL()
// ---------------------------------------------------------
//
void TDdParser::MetaElementNodeL( NW_DOM_ElementNode_t* aElmtNode )
    {
    CLOG(( EParse, 2, _L("-> TDdParser::MetaElementNodeL") ));
    __ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );
    NW_String_t str;
    LeaveIfNwErrorL( NW_DOM_Node_getNodeName( aElmtNode, &str ) );
    CleanupNwStringStoragePushL( &str );
    NW_Ucs2* ucs2;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
    CleanupNwUcs2PushL( ucs2 );
    TPtrC attrName( ucs2 );
    TDdAttr attr = MetaAttribute( attrName );
    CLOG(( EParse, 4, _L("  attribute <%S> (%d)"), &attrName, attr ));
    CleanupStack::PopAndDestroy( 2 );   // Clean up ucs2, str->storage
	if ( attr == EDdLicense )
	{
		iIsLicenseTag = ETrue;
		return;
	}
    if ( attr != EDdUnknownAttr )   // Unknown attribute is ignored.
        {
        ucs2 = PcDataLC( aElmtNode );
        if ( ucs2 )
            {
            SetMetaAttrL( attr, TPtrC( ucs2 ) );
            }
        CleanupStack::PopAndDestroy();  // Clean up ucs2.
        }
    CLOG(( EParse, 2, _L("<- TDdParser::MetaElementNodeL") ));
    }

// ---------------------------------------------------------
// TDdParser::MetaElementNodeL()
// ---------------------------------------------------------
//
void TDdParser::MetaElementNodeL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject )
    {
    CLOG(( EParse, 2, _L("-> TDdParser::MetaElementNodeL") ));
    __ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );
    NW_String_t str;
    LeaveIfNwErrorL( NW_DOM_Node_getNodeName( aElmtNode, &str ) );
    CleanupNwStringStoragePushL( &str );
    NW_Ucs2* ucs2;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
    CleanupNwUcs2PushL( ucs2 );
    TPtrC attrName( ucs2 );
    TDdAttr attr = MetaAttribute( attrName );
    CLOG(( EParse, 4, _L("  attribute <%S> (%d)"), &attrName, attr ));
    CleanupStack::PopAndDestroy( 2 );   // Clean up ucs2, str->storage
	if ( attr == EDdLicense )
	{
		iIsLicenseTag = ETrue;
		return;
	}
    if ( attr != EDdUnknownAttr )   // Unknown attribute is ignored.
        {
        ucs2 = PcDataLC( aElmtNode );
        if ( ucs2 )
            {
            SetMetaAttrL( attr, TPtrC( ucs2 ), aMediaObject );
            }
        CleanupStack::PopAndDestroy();  // Clean up ucs2.
        }
    CLOG(( EParse, 2, _L("<- TDdParser::MetaElementNodeL") ));
    }// ---------------------------------------------------------
// TDdParser::ElementNodeL()
// ---------------------------------------------------------
//
void TDdParser::ElementNodeL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject )
    {
    CLOG(( EParse, 2, _L("-> TDdParser::ElementNodeL") ));
    __ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );
    NW_String_t str;
    LeaveIfNwErrorL( NW_DOM_Node_getNodeName( aElmtNode, &str ) );
    CleanupNwStringStoragePushL( &str );
    NW_Ucs2* ucs2;
    LeaveIfNwErrorL( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
    CleanupNwUcs2PushL( ucs2 );
    TPtrC attrName( ucs2 );
    TDdAttr attr = Attribute( attrName );
    CLOG(( EParse, 4, _L("  attribute <%S> (%d)"), &attrName, attr ));
    CleanupStack::PopAndDestroy( 2 );   // Clean up ucs2, str->storage
    if ( attr != EDdUnknownAttr )   // Unknown attribute is ignored.
        {
        ucs2 = PcDataLC( aElmtNode );
        if ( ucs2 )
            {
            SetAttrL( attr, TPtrC( ucs2 ), aMediaObject );
            }
        CleanupStack::PopAndDestroy();  // Clean up ucs2.
        }
    CLOG(( EParse, 2, _L("<- TDdParser::ElementNodeL") ));
    }

// ---------------------------------------------------------
// TDdParser::PcDataLC()
// ---------------------------------------------------------
//
NW_Ucs2* TDdParser::PcDataLC( NW_DOM_ElementNode_t* aElmtNode )
    {
    __ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );
    NW_Ucs2* ucs2 = NULL;
    NW_DOM_TextNode_t* textNode = NW_DOM_Node_getFirstChild( aElmtNode );
    if ( textNode && NW_DOM_Node_getNodeType( textNode ) == NW_DOM_TEXT_NODE )
        {
        NW_String_t str;
        LeaveIfNwErrorL( NW_DOM_TextNode_getData( textNode, &str ) );
        CleanupNwStringStoragePushL( &str );
        LeaveIfNwErrorL
            ( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
        CleanupStack::PopAndDestroy();  // Clean up str->storage.
        CleanupNwUcs2PushL( ucs2 );
        }
    if ( !ucs2 )
        {
        // Push the NULL.
        CleanupStack::PushL( ucs2 );
        }
    return ucs2;
    }

// ---------------------------------------------------------
// TDdParser::GetAttrValueLC()
// ---------------------------------------------------------
//
/*
NW_Ucs2* TDdParser::GetAttrValueLC( NW_DOM_ElementNode_t* aElmtNode, NW_DOM_AttributeHandle_t* aAttrHandle )
    {
    __ASSERT_DEBUG( aElmtNode, CodPanic( ECodInternal ) );
    NW_Ucs2* ucs2 = NULL;
    NW_DOM_TextNode_t* textNode = NW_DOM_Node_getFirstChild( aElmtNode );
    if ( textNode && NW_DOM_Node_getNodeType( textNode ) == NW_DOM_TEXT_NODE )
        {
        NW_String_t str;
        LeaveIfNwErrorL( NW_DOM_TextNode_getData( textNode, &str ) );
        CleanupNwStringStoragePushL( &str );
        LeaveIfNwErrorL
            ( NW_String_stringToUCS2Char( &str, iEncoding, &ucs2 ) );
        CleanupStack::PopAndDestroy();  // Clean up str->storage.
        CleanupNwUcs2PushL( ucs2 );
        }
    if ( !ucs2 )
        {
        // Push the NULL.
        CleanupStack::PushL( ucs2 );
        }
    return ucs2;
    }
*/
// ---------------------------------------------------------
// TDdParser::Attribute() // for OMA v1
// ---------------------------------------------------------
//
TDdParser::TDdAttr TDdParser::Attribute( const TDesC& aAttrName ) const
    {
    TDdAttr attr = EDdUnknownAttr;
    if ( !aAttrName.Compare( KDdName ) )
        {
        attr = EDdName;
        }
    else if ( !aAttrName.Compare( KDdVendor ) )
        {
        attr = EDdVendor;
        }
    else if ( !aAttrName.Compare( KDdDescription ) )
        {
        attr = EDdDescription;
        }
    else if ( !aAttrName.Compare( KDdUrl ) )
        {
        attr = EDdUrl;
        }
    else if ( !aAttrName.Compare( KDdSize ) )
        {
        attr = EDdSize;
        }
    else if ( !aAttrName.Compare( KDdType ) )
        {
        attr = EDdType;
        }
    else if ( !aAttrName.Compare( KDdInstallNotify ) )
        {
        attr = EDdInstallNotify;
        }
    else if ( !aAttrName.Compare( KDdNextUrl ) )
        {
        attr = EDdNextUrl;
        }
    else if ( !aAttrName.Compare( KDdInfoUrl ) )
        {
        attr = EDdInfoUrl;
        }
    else if ( !aAttrName.Compare( KDdIcon ) )
        {
        attr = EDdIcon;
        }
    else if ( !aAttrName.Compare( KDdVersion ) )
        {
        attr = EDdVersion;
        }
    return attr;
    }

// ---------------------------------------------------------
// TDdParser::MediaAttribute() // for OMA v2
// ---------------------------------------------------------
//
TDdParser::TDdAttr TDdParser::MediaAttribute( const TDesC& aAttrName ) const
    {
    TDdAttr attr = EDdUnknownAttr;

	if ( !aAttrName.Compare( KDdVersion ) )
        {
        attr = EDdVersion;
        }
    else if ( !aAttrName.Compare( KDdVendor ) )
        {
        attr = EDdVendor;
        }
    else if ( !aAttrName.Compare( KDdName ) )
        {
        attr = EDdName;
        }
    else if ( !aAttrName.Compare( KDdNextUrl ) )
        {
        attr = EDdNextUrl;
        }
    else if ( !aAttrName.Compare( KDdProduct ) )
        {
        attr = EDdProduct;
        }
    else if ( !aAttrName.Compare( KDdUpdatedDDURI ) )
        {
        attr = EDdUpdatedDDURI;
        }        
    return attr;
    }

// ---------------------------------------------------------
// TDdParser::MediaObjAttribute() // for OMA v2
// ---------------------------------------------------------
//
TDdParser::TDdAttr TDdParser::MediaObjAttribute( const TDesC& aAttrName ) const
    {
    TDdAttr attr = EDdUnknownAttr;

	if ( !aAttrName.Compare( KDdUrl ) )
        {
        attr = EDdUrl;
        }
    else if ( !aAttrName.Compare( KDdSize ) )
        {
        attr = EDdSize;
        }
    else if ( !aAttrName.Compare( KDdType ) )
        {
        attr = EDdType;
        }
    else if ( !aAttrName.CompareF( KDdProgressiveDl ) )
        {
        attr = EDdProgressiveDl;
        }
    else if ( !aAttrName.CompareF( KDdSuppressUserConfirmation ) )
        {
        attr = EDdSuppressUserConfirmation;
        }
    else if ( !aAttrName.Compare( KDdMeta ) )
        {
        attr = EDdMeta;
        }

    return attr;
    }

// ---------------------------------------------------------
// TDdParser::ObjUriAttribute() // for OMA v2
// ---------------------------------------------------------
//
TDdParser::TDdAttr TDdParser::ObjUriAttribute( const TDesC& aAttrName ) const
    {
    TDdAttr attr = EDdUnknownAttr;

	if ( !aAttrName.Compare( KDdServer ) )
        {
        attr = EDdUrl;
        }
    return attr;
    }

// ---------------------------------------------------------
// TDdParser::DdVersionAttribute() // for OMA v2
// ---------------------------------------------------------
//
TDdParser::TDdAttr TDdParser::DdVersionAttribute( const TDesC& aAttrName ) const
    {
    TDdAttr attr = EDdUnknownAttr;

	if ( !aAttrName.Compare( KDdVersion ) )
        {
        attr = EDdVersion;
        }
    return attr;
    }

// ---------------------------------------------------------
// TDdParser::MetaAttribute() // for OMA v2
// ---------------------------------------------------------
//
TDdParser::TDdAttr TDdParser::MetaAttribute( const TDesC& aAttrName ) const
    {
    TDdAttr attr = EDdUnknownAttr;
    if ( !aAttrName.Compare( KDdName ) )
        {
        attr = EDdName;
        }
    if ( !aAttrName.Compare( KDdText ) )
        {
        attr = EDdText;
        }
    else if ( !aAttrName.Compare( KDdDescription ) )
        {
        attr = EDdDescription;
        }
    else if ( !aAttrName.Compare( KDdInstallNotify ) )
        {
        attr = EDdInstallNotify;
        }
    else if ( !aAttrName.Compare( KDdLicense ) )
        {
        attr = EDdLicense;
        }
    else if ( !aAttrName.Compare( KDdOrder ) )
        {
        attr = EDdOrder;
        }
    return attr;
    }

// ---------------------------------------------------------
// TDdParser::ProductAttribute() // for OMA v2
// ---------------------------------------------------------
//
TDdParser::TDdAttr TDdParser::ProductAttribute( const TDesC& aAttrName ) const
    {
    TDdAttr attr = EDdUnknownAttr;
    if ( !aAttrName.Compare( KDdMeta ) )
        {
        attr = EDdMeta;
        }
    if ( !aAttrName.Compare( KDdMediaObject ) )
        {
        attr = EDdMediaObject;
        }
    return attr;
    }

// ---------------------------------------------------------
// TDdParser::SetAttrL()  // for OMA v1
// ---------------------------------------------------------
//
void TDdParser::SetAttrL( TDdAttr aAttr, const TDesC& aValue, CMediaObjectData *& aMediaObject )
    {
    TInt ok( ETrue );
    switch( aAttr )
        {
        case EDdName:
            {
            if ( !aMediaObject->Name().Length() )
                {
                ok = aMediaObject->SetNameL( aValue );
                }
            break;
            }

        case EDdDescription:
            {
            if ( !aMediaObject->Description().Length() )
                {
                ok = aMediaObject->SetDescriptionL( aValue );
                }
            break;
            }

        case EDdUrl:
            {
            if ( !aMediaObject->Url().Length() )
                {
                ok = aMediaObject->SetUrlL( aValue );
                }
            break;
            }

        case EDdSize:
            {
            if ( !aMediaObject->Size() )
                {
                // Parse as TUint - negative not allowed.
                TUint size;
                TLex lex( aValue );
                if ( !lex.Val( size ) )
                    {
                    aMediaObject->SetSize( size );
                    }
                else
                    {
                    ok = EFalse;
                    }
                }
            break;
            }

        case EDdType:
            {
            ok = aMediaObject->AddTypeL( aValue );
            break;
            }

        case EDdInstallNotify:
            {
            if ( !aMediaObject->InstallNotify().Length() )
                {
                ok = aMediaObject->SetInstallNotifyL( aValue );
                }
            break;
            }

        case EDdInfoUrl:
            {
            if ( !aMediaObject->InfoUrl().Length() )
                {
                ok = aMediaObject->SetInfoUrlL( aValue );
                }
            break;
            }

        case EDdIcon:
            {
            if ( !aMediaObject->Icon().Length() )
                {
                ok = aMediaObject->SetIconL( aValue );
                }
            break;
            }
            
        case EDdVersion:
            {
            if ( !iData->Version().Length() )
                {
                ok = iData->SetVersionL( aValue );
                }
            break;
            }

        case EDdNextUrl:
            {
            if ( !iData->NextUrl().Length() )
                {
                ok = iData->SetNextUrlL( aValue );
                }
            break;
            }

        case EDdVendor:
            {
            if ( !iData->Vendor().Length() )
                {
                ok = iData->SetVendorL( aValue );
                }
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
    }


// ---------------------------------------------------------
// TDdParser::SetMediaAttrL()
// ---------------------------------------------------------
//
void TDdParser::SetMediaAttrL( TDdAttr aAttr, const TDesC& aValue )
    {
    TInt ok( ETrue );
    switch( aAttr )
        {
        case EDdName:
            {
            if ( !iData->Name().Length() )
                {
                ok = iData->SetNameL( aValue );
                }
            break;
            }
        case EDdVersion:
            {
            if ( !iData->Version().Length() )
                {
                ok = iData->SetVersionL( aValue );
                }
            break;
            }

        case EDdVendor:
            {
            if ( !iData->Vendor().Length() )
                {
                ok = iData->SetVendorL( aValue );
                }
            break;
            }

        case EDdNextUrl:
            {
            if ( !iData->NextUrl().Length() )
                {
                // Unlike COD, DD has no separate URL-s for success and error
                // service flow. Since we have a common data structore for
                // both (which contains separate URLs for those), we fill both
                // URLs with this value.
                ok = iData->SetNextUrlL( aValue ) &&
                     iData->SetNextUrlAtErrorL( aValue );
                }
            break;
            }

        case EDdUpdatedDDURI:
            {
            if(!iData->UpdatedDDUriL().Length())
                {
                iData->SetUpdatedDDURI( aValue );
                }
            
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
    }

// ---------------------------------------------------------
// TDdParser::SetMediaObjAttrL()
// ---------------------------------------------------------
//
void TDdParser::SetMediaObjAttrL( TDdAttr aAttr, const TDesC& aValue, CMediaObjectData *& aMediaObject )
    {
    TInt ok( ETrue );
    switch( aAttr )
        {
        case EDdSize:
            {
            if ( !aMediaObject->Size() )
                {
                // Parse as TUint - negative not allowed.
                TUint size;
                TLex lex( aValue );
                if ( !lex.Val( size ) )
                    {
                    aMediaObject->SetSize( size );
                    }
                else
                    {
                    ok = EFalse;
                    }
                }
            break;
            }

        case EDdType:
            {
            ok = aMediaObject->AddTypeL( aValue );
            break;
            }

        case EDdProgressiveDl:
            {
            TBool pd( EFalse );
            if ( !aValue.CompareF( KDdTrue ) )
                {
                pd = ETrue;
                }
            else if ( aValue.CompareF( KDdFalse ) )
                {
                // Expected 'true' or 'false'
                Error( KErrCodInvalidDescriptor );
                break;
                }
            aMediaObject->SetProgressiveDownload( pd );
            break;
            }

        case EDdSuppressUserConfirmation:
            {
            TInt confirm = CCodData::ENever; 
            if ( !aValue.CompareF(KDdNever) )
                {
                confirm = CCodData::ENever; 
                }
            else if ( aValue.CompareF(KDdUserConfirmStepOnly) )
                {
                confirm = CCodData::EUserConfirmStepOnly; 
                }
            else if ( aValue.CompareF(KDdAlways) )
                {
                confirm = CCodData::EAlways;      
                }
            else 
                {
                Error( KErrCodInvalidDescriptor );
                }
            iData->SetSuppressConfirm( confirm );
            break;
            }
        case EDdUpdatedDDURI:
            {
            if(!iData->UpdatedDDUriL().Length())
                {
                iData->SetUpdatedDDURI( aValue );
                }
            
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
    }


// ---------------------------------------------------------
// TDdParser::SetMetaAttrL()
// ---------------------------------------------------------
//
void TDdParser::SetMetaAttrL( TDdAttr aAttr, const TDesC& aValue )
    {
    TInt ok( ETrue );
    switch( aAttr )
        {
        case EDdName:
            {
            if ( !iData->Name().Length() )
                {
                ok = iData->SetNameL( aValue );
                }
            break;
            }

        case EDdDescription:
            {
            if ( !iData->Description().Length() )
                {
                ok = iData->SetDescriptionL( aValue );
                }
            break;
            }

        case EDdInstallNotify:
            {
            if ( !iData->InstallNotify().Length() )
                {
                ok = iData->SetInstallNotifyL( aValue );
                }
            break;
            }

        case EDdInfoUrl:
            {
            if ( !iData->InfoUrl().Length() )
                {
                ok = iData->SetInfoUrlL( aValue );
                }
            break;
            }

        case EDdIcon:
            {
            if ( !iData->Icon().Length() )
                {
                ok = iData->SetIconL( aValue );
                }
            break;
            }

        case EDdOrder:
            {
            TBool isPostOrder( EFalse );
            if ( !aValue.CompareF( KDdPost ) )
                {
                isPostOrder = ETrue;
                }
            else if ( aValue.CompareF( KDdAny ) )
                {
                // Expected 'post' or 'any'
                Error( KErrCodInvalidDescriptor );
                break;
				}
            iData->SetOrderIsPost( isPostOrder );
            break;
            }

        case EDdText:
            {

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
    }


// ---------------------------------------------------------
// TDdParser::SetMetaAttrL()
// ---------------------------------------------------------
//
void TDdParser::SetMetaAttrL( TDdAttr aAttr, const TDesC& aValue, CMediaObjectData *& aMediaObject )
    {
    TInt ok( ETrue );
    switch( aAttr )
        {
        case EDdName:
            {
            if ( !aMediaObject->Name().Length() )
                {
                ok = aMediaObject->SetNameL( aValue );
                }
            break;
            }

        case EDdDescription:
            {
            if ( !aMediaObject->Description().Length() )
                {
                ok = aMediaObject->SetDescriptionL( aValue );
                }
            break;
            }

        case EDdInstallNotify:
            {
            if ( !aMediaObject->InstallNotify().Length() )
                {
                ok = aMediaObject->SetInstallNotifyL( aValue );
                }
            break;
            }

        case EDdInfoUrl:
            {
            if ( !aMediaObject->InfoUrl().Length() )
                {
                ok = aMediaObject->SetInfoUrlL( aValue );
                }
            break;
            }

        case EDdIcon:
            {
            if ( !aMediaObject->Icon().Length() )
                {
                ok = aMediaObject->SetIconL( aValue );
                }
            break;
            }

        case EDdOrder:
            {
            TBool isPostOrder( EFalse );
            if ( !aValue.CompareF( KDdPost ) )
                {
                isPostOrder = ETrue;
                }
            else if ( aValue.CompareF( KDdAny ) )
                {
                // Expected 'post' or 'any'
                Error( KErrCodInvalidDescriptor );
                break;
				}
            aMediaObject->SetOrderIsPost( isPostOrder );
            break;
            }

        case EDdText:
            {
//TODO for OMA2
/*
            if ( !iData->Text().Length() )
                {
                ok = iData->SetTextL( aValue );
                }
*/
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
    }
    
// ---------------------------------------------------------
// TDdParser::ParseNameFromUrlL()
// ---------------------------------------------------------    
void TDdParser::ParseNameFromUrlL( CMediaObjectData *& aMediaObject )
    {
    TBuf16<COD_URL_MAX_LEN> buf16;
    CnvUtfConverter::ConvertToUnicodeFromUtf8(buf16,aMediaObject->Url());
    TInt lastSlashPos = buf16.LocateReverse('/');
    aMediaObject->SetNameL(buf16.MidTPtr( lastSlashPos + 1));
    }
    
