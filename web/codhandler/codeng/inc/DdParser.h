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
*      Declaration of class TDdParser.   
*      
*
*/


#ifndef DD_PARSER_H
#define DD_PARSER_H

// INCLUDES

#include <e32base.h>
#include <xml/cxml/nw_dom_document.h>

// FORWARD DECLARATION

class CMediaObjectData;
class CCodData;

// CLASS DECLARATION

/**
* DD Parser.
*/
NONSHARABLE_CLASS( TDdParser )
    {

    public:     // constructor

        /**
        * Constructor.
        */
        inline TDdParser();

    public:     // new methods

        /**
        * Parse the buffer into aData. Existing data in aData is lost.
        * Leaves on failure; leave code can be KErrCodInvalidDescriptor as well
        * as general errors. The resulting data is not checked semantically
        * (i.e. mandatory attributes may be missing).
        * @param aBuf DD content buffer to be parsed.
        * @param aData DD will be parsed into this.
        * @param aIsDd2 equals to ETrue for the OMA 2 dd
        * @param aIsLicenseTag equals to ETrue if a license tag is presented (for OMA 2 dd)
        */
        void ParseL( const TDesC& aBuf, CCodData& aData, const TBool aIsDd2, TBool& aIsLicenseTag );

    private:    // types

        enum TDdAttr               ///< DD Attributes.
            {
            EDdName,               ///< DD-Name.
            EDdVendor,             ///< DD-Vendor.
            EDdDescription,        ///< DD-Description.
            EDdUrl,                ///< DD-URL.
            EDdSize,               ///< DD-Size.
            EDdType,               ///< DD-Type.
            EDdInstallNotify,      ///< DD-Install-Notify.
            EDdNextUrl,            ///< DD-Netx-URL.
            EDdInfoUrl,            ///< DD-Info-URL.
            EDdIcon,               ///< DD-Icon.
            EDdVersion,            ///< DD-Version.
            EDdProduct,            ///< DD-Product (OMA 2)
            EDdMediaObject,        ///< DD-MediaObject (OMA 2)
            EDdMeta,               ///< DD-Meta (OMA 2)
            EDdProgressiveDl,      ///< DD-progressiveDowloadFlag (OMA 2)
            EDdSuppressUserConfirmation,	///< DD-suppressUserConfirmation (OMA 2)
            EDdLicense,				///< DD-license (OMA 2)
            EDdOrder,				///< DD-order (OMA 2)
            EDdText,				///< DD-text (OMA 2)
            EDdUpdatedDDURI,
            EDdUnknownAttr         ///< Future DD attributes.
            };

    private:    // parsing

        /**
        * Parse document node (top level construct: 'media').
        * @param aDocNode Document node.
        */
        void DocNodeL( NW_DOM_DocumentNode_t* aDocNode );

        /**
         * Parse document node for OMA v2
         * @param aDocNode Document node.
         */
        void ParseDd2DocNodeL( NW_DOM_DocumentNode_t* aDocNode );

        /**
         * Process child element node (below 'media').
         * @param aElmtNode Element node.
         */
        void ElementNodeL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject );

        /**
         * Parse Media elements for OMA v2
         * @param aElmtNode Document node.
         */
        void ParseMediaElementsL( NW_DOM_ElementNode_t* aElmtNode );

        /**
         * Parse Product elements for OMA v2
         * @param aElmtNode Document node.
         */
        void ParseProductElementsL( NW_DOM_ElementNode_t* aElmtNode );

        /**
         * Parse Media Object elements for OMA v2
         * @param aElmtNode Document node.
         */
        void ParseMediaObjectElementsL( NW_DOM_ElementNode_t* aElmtNode);

        /**
         * Parse Meta elements for OMA v2
         * @param aElmtNode Document node.
         */
        void ParseMetaElementsL( NW_DOM_ElementNode_t* aElmtNode );

        /**
         * Parse Meta elements for OMA v2
         * @param aElmtNode Document node.
         */
        void ParseMetaElementsL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject );

        /**
         * Parse Object Uri elements for OMA v2
         * @param aElmtNode Document node.
         */
        void ParseObjectUriElementsL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject );

        /**
         * Process child element node (below 'media') for OMA v2
         * @param aElmtNode Element node.
         */		
        void MediaElementNodeL( NW_DOM_ElementNode_t* aElmtNode);

        /**
         * Process child element node (below 'media Object') for OMA v2
         * @param aElmtNode Element node.
         */		
        void MediaObjElementNodeL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject );

        /**
         * Process child element node (below 'meta') for OMA v2
         * @param aElmtNode Element node.
         */		
        void MetaElementNodeL( NW_DOM_ElementNode_t* aElmtNode);

        /**
         * Process child element node (below 'meta') for OMA v2
         * @param aElmtNode Element node.
         */		
        void MetaElementNodeL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject );

        /**
         * Process child element node (below 'product') for OMA v2
         * @param aElmtNode Element node.
         */		
        void ProductElementNodeL( NW_DOM_ElementNode_t* aElmtNode);

        /**
         * Process child element node (below 'Object Uri') for OMA v2
         * @param aElmtNode Element node.
         */		
        void ObjUriElementNodeL( NW_DOM_ElementNode_t* aElmtNode, CMediaObjectData *& aMediaObject );
		
        /**
        * Get 'Media' attribute value from attribute name for OMA v2
        * @param aAttrName Attribute name.
        * @return Attribute value.
        */
        TDdAttr MediaAttribute( const TDesC& aAttrName ) const;

        /**
        * Get 'Media Object' attribute value from attribute name for OMA v2
        * @param aAttrName Attribute name.
        * @return Attribute value.
        */
        TDdAttr MediaObjAttribute( const TDesC& aAttrName ) const;

        /**
        * Get 'Meta' attribute value from attribute name for OMA v2
        * @param aAttrName Attribute name.
        * @return Attribute value.
        */
        TDdAttr MetaAttribute( const TDesC& aAttrName ) const;

        /**
        * Get 'Product' attribute value from attribute name for OMA v2
        * @param aAttrName Attribute name.
        * @return Attribute value.
        */
        TDdAttr ProductAttribute( const TDesC& aAttrName ) const;

        /**
        * Get 'Object Uri' attribute value from attribute name for OMA v2
        * @param aAttrName Attribute name.
        * @return Attribute value.
        */
        TDdAttr ObjUriAttribute( const TDesC& aAttrName ) const;

        /**
        * Get 'DDVersion' attribute value from attribute name for OMA v2
        * @param aAttrName Attribute name.
        * @return Attribute value.
        */
        TDdAttr DdVersionAttribute( const TDesC& aAttrName ) const;

        /**
        * Set data for "media" attribute for OMA v2
        * @param aAttr Attribute.
        * @param aValue Value.
        */		
        void SetMediaAttrL( TDdAttr aAttr, const TDesC& aValue );

        /**
        * Set data for "meta" attribute for OMA v2
        * @param aAttr Attribute.
        * @param aValue Value.
        */
        void SetMetaAttrL( TDdAttr aAttr, const TDesC& aValue );

        /**
        * Set data for "meta" attribute for OMA v2
        * @param aAttr Attribute.
        * @param aValue Value.
        */
        void SetMetaAttrL( TDdAttr aAttr, const TDesC& aValue, CMediaObjectData *& aMediaObject );

        /**
        * Set data for "mediaObject" attribute.
        * @param aAttr Attribute.
        * @param aValue Value.
        */
        void SetMediaObjAttrL( TDdAttr aAttr, const TDesC& aValue, CMediaObjectData *& aMediaObject );
				
        /**
        * Allocate and return PCDATA of element node.
        * @param aElmtNode Element node.
        * @return PCDATA or NULL. A cleanup item (or NULL) is pushed on the
        * cleanup stack.
        */
        NW_Ucs2* PcDataLC( NW_DOM_ElementNode_t* aElmtNode );

        /**
        * Get attribute value from attribute name.
        * @param aAttrName Attribute name.
        * @return Attribute value.
        */
        TDdAttr Attribute( const TDesC& aAttrName ) const;

        /**
        * Set data attribute.
        * @param aAttr Attribute.
        * @param aValue Value.
        */
        void SetAttrL( TDdAttr aAttr, const TDesC& aValue, CMediaObjectData *& aMediaObject );

        /**
         * Parse node attribute (for OMA v2) E.g. <media xmlns="urn:oma:xml:dl:dd:2.0" DDVersion="2.0"
		 * where attr1 = xmlns; attr2 = DDVersion
         * @param aAttrHandle attribute handler
         */
        void ParseNodeAttributesL( NW_DOM_AttributeHandle_t* aAttrHandle );
        
        /**
        *Parse Name from URl in case Name Tag is not present In DD
        *in that case take it from URL
        */
        void ParseNameFromUrlL( CMediaObjectData *& aMediaObject );

        /**
        * Set error code if not already set.
        * @param aError Error.
        */
        inline void Error( TInt aError );

    private:    // data

        CCodData* iData;        ///< DD Data. Not owned.
        NW_Uint32 iEncoding;    ///< Document encoding.
        TInt iError;            ///< Error code.
        TBool iIsLicenseTag;    ///< ETrue if license tag exists in the DD2
    };

#include "DdParser.inl"

#endif /* def DD_PARSER_H */
