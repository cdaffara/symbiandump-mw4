/*
* ==============================================================================
*  Name        : WidgetRegistryXml.h
*  Part of     : Widget Registry
*  Interface   : Widget Registry API
*  Description : This file contains the header file of the CWidgetBackupRegistryXml class.
*
*                This class processes persistent registry data in XML.
*  Version     : %version: 11 %
*
*  Copyright © 2008 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

#ifndef WIDGETBACKUPREGISTRYXML_H
#define WIDGETBACKUPREGISTRYXML_H

// INCLUDES
#include <libxml2_tree.h>
#include "WidgetRegistryConstants.h"
#include "WidgetPropertyValue.h"

class RFs;
/**
*  CWidgetBackupRegistryXml
*  @since 5.0
*/
class CWidgetBackupRegistryXml: public CBase
    {
public:

    /**
     * Two-phased constructor.
     */
//    static CWidgetBackupRegistryXml* NewL();
    static CWidgetBackupRegistryXml* NewL();

    /**
     * Destructor.
     */
    virtual ~CWidgetBackupRegistryXml();

protected:

    /**
     * C++ default constructor.
     */
    CWidgetBackupRegistryXml();

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
