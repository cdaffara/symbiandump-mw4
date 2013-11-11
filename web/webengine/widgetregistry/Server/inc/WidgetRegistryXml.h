/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the header file of the CWidgetRegistryXml class.
*
*                This class processes persistent registry data in XML.
*
*/


#ifndef WIDGETREGISTRYXML_H
#define WIDGETREGISTRYXML_H

// INCLUDES
#include <libxml2_tree.h>
#include "WidgetRegistryConstants.h"
#include "WidgetPropertyValue.h"

class RFs;
/**
*  CWidgetRegistryXml
*  @since 5.0
*/
class CWidgetRegistryXml: public CBase
    {
public:

    /**
     * Two-phased constructor.
     */
    static CWidgetRegistryXml* NewL();

    /**
     * Destructor.
     */
    virtual ~CWidgetRegistryXml();

protected:

    /**
     * C++ default constructor.
     */
    CWidgetRegistryXml();

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();

public:

    /**
     * Traverse to the next Node
     *
     * @param aNode: current node
     * @since 5.0
     * @return next node
     */
    xmlNode* TraverseNextNode( xmlNode* aNode );

    /**
     * Convert a string in some encoding to UCS2.
     *
     * @param aEncoding encoding number
     * @param aUnicodeSizeMultiplier how many input bytes per unicode char
     * @param aInBuf input data
     * @param aOutBuf output data, allocated and returned to caller
     * @param aFileSession
     * @since 5.0
     * @return void
     */
    void ToUnicodeL( TInt aEncoding,
                     TInt aUnicodeSizeMultiplier,
                     TPtrC8 aInBuf, HBufC16** aOutBuf,
                     RFs& aFileSession );

    /**
     * Convert a string to some encoding from UCS2.
     *
     * @param aEncoding encoding number
     * @param aUnicodeSizeMultiplier how many input bytes per unicode char
     * @param aInBuf input data
     * @param aOutBuf output data, allocated and returned to caller
     * @param aFileSession
     * @since 5.0
     * @return void
     */
    void FromUnicodeL( TInt aEncoding,
                     TInt aUnicodeSizeMultiplier,
                     TPtrC16 aInBuf, HBufC8** aOutBuf,
                     RFs& aFileSession );


    /**
     * Utility to bundle extraction of XML text content
     *
     * @param aEncoding input buffer encoding
     * @param aUnicodeSizeMultiplier how many bytes of input make one unicode char
     * @param aInBuf input data in encoding
     * @param aOutBuf malloc'ed output buf, caller takes ownership
     * @param aFileSession CCnvCharacterSetConverter requires it
     * @since 5.0
     * @return void
     */
    void GetContentL( RFs& aFileSession,
                      xmlDocPtr aDoc,
                      xmlNode* aNode,
                      HBufC** aContent );

    /**
     * Utility to extract XML content as a string
     *
     * @param aFileSession CCnvCharacterSetConverter requires it
     * @param aDoc
     * @param aNode
     * @param aContent
     * @since 5.0
     * @return void
     */
    void GetTextContentAsStringL( RFs& aFileSession,
                                  xmlDocPtr aDoc,
                                  xmlNode* aNode,
                                  HBufC** aContent );

    void GetSubtreeAsStringL (RFs& aFileSession, xmlDocPtr aDoc, xmlNode* aNode, HBufC** aBuf, TInt& aLen);

    xmlChar* EncodeStringL(xmlDocPtr aDoc, xmlChar* aStringToConvert);
    HBufC* EncodeStringL(xmlDocPtr aDoc, TPtrC aStringToConvert, RFs& aFileSession);

    TInt EncodedStringLength(TPtrC8 aStringToConvert);

    TInt GetPropertyId(
        const TDesC& aKeyName );

    const TPtrC& XmlPropertyName( TInt aPropertyId );

    class TWidgetProperty
        {
    public:
        TInt id; // the TWidgetPropertyId for this name
        TPtrC name;
        TWidgetPropertyType type; // the type if valid
        TInt flags;
        };

    // Table describing registry entries for XML processing.  Used by
    // parser and generator.
    RArray<TWidgetProperty> iProperties;

    };

#endif // WIDGETREGISTRYXML_H
