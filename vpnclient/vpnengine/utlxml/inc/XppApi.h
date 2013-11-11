/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* A simple XML pull parsing interface for handling
* non-unicode XML documents.
*
*/


#ifndef __XPP_API__
#define __XPP_API__

#include <e32base.h>

/**
 * A simple XML pull parsing interface for handling
 * non-unicode XML documents.
 *
 * This interface is _conceptually_ similar to the
 * Common API for XML Pull Parsing version 1 Java
 * API specified at www.xmlpull.org.
 */
class MXmlPullParser 
    { 
public:
    /**
     * Parser states
     */
    enum TParserState
        {
        EStateStartDocument, /**< The XML input has been set */
        EStateStartTag,      /**< A start tag was read */
        EStateText,          /**< Element content was read */
        EStateEndTag,        /**< An end tag was read */
        EStateEndDocument,   /**< No more elements are available */
        EStateError          /**< The document is erroneous */
        };

    /**
     * Parser return codes (in addition to KErrNone and KErrNotFound)
     */
    enum TReturnCode
        {
        ERcWrongParserState = -101,   /**< The method is not applicable 
                                           in the current parser state */
        ERcDocumentError = -102,      /**< The document is invalid */
        ERcUnknown = -103             /**< Unknown return code */
        };

    /**
     * Sets the input(XML document) for the parser.
     *
     * @param aInput The XML document to parse
     */
    virtual void SetInput(const TDesC8& aInput) = 0;
    
    /**
     * Advances the parser to the next XML element
     * (tag or text) in the document. The State()
     * method should be used to find out the resulting
     * parser state.
     *
     * @return KErrNone if the NextL method succeeds,
     * one of TReturnCode values if an error occurs.
     * On unexceptional conditions, the method leaves
     * with one of the system-wide error codes.
     */
    virtual TInt NextL() = 0;
    
    /**
     * Returns the current state of the parser.
     *
     * @return The current state of the parser.
     * (one of the TParserState values).
     */
    virtual TParserState State() = 0;

    /**
     * Returns the name of the current element.
     * This method is only applicable if the parser
     * state is EStateStartTag, EStateEndTag or EStateText.
     * (jakovist added) or EStateEndDocument. 
     *
     * @param aName On return, contains the name
     * of the current element.
     * 
     * @return KErrNone if the method succeeds,
     * ERcWrongParserState if it fails.
     */
    virtual TInt Name(TPtrC8& aName) = 0;

    /**
     * Returns the XPath of the current element.
     * This method is only applicable if the parser
     * state is EStateStartTag, EStateEndTag or EStateText.
     *
     * @param aPath On return, contains the name
     * of the current element.
     * 
     * @return KErrNone if the method succeeds,
     * ERcWrongParserState if it fails.
     * On unexceptional conditions, the method leaves
     * with one of the system-wide error codes.
     */
    virtual TInt PathL(TPtrC8& aPath) = 0;
    
    /**
     * Returns the depth of the current element in the
     * XML document.
     * This method is only applicable if the parser
     * state is EStateStartTag, EStateEndTag or EStateText.
     *
     * @param aDepth On return, contains the depth
     * of the current element.
     * 
     * @return KErrNone if the method succeeds,
     * ERcWrongParserState if it fails.
     */
    virtual TInt Depth(TInt& aDepth) = 0;

    /**
     * Returns the number of the attributes in the
     * current start element. The method is only
     * applicable if the parser state is EStateStartTag.
     *
     * @param aCount On successful return, the number
     * of attributes.
     * 
     * @return KErrNone if the method is successful,
     * ERcWrongParserState if the method fails.
     */
    virtual TInt AttributeCount(TInt& aCount) = 0;

    /**
     * Returns the name of the specified attribute
     * of the current element. The method is only
     * applicable if the parser state is EStateStartTag.
     *
     * @param aIndex The index (between 0 and 
     * AttbuteCount() - 1) of the attribute whose
     * name is to be returned.
     * @param aValue On successful return, points
     * to the name of the specified attribute.
     * 
     * @return KErrNone if the attribute is found,
     * KErrNotFound if the specified attribute
     * cannot be found, ERcWrongParserState if 
     * the method fails.
     */
    virtual TInt AttributeName(TInt aIndex,
                               TPtrC8& aName) = 0;

    /**
     * Returns the value of the specified attribute
     * of the current element. The method is only
     * applicable if the parser state is EStateStartTag.
     *
     * @param aIndex The index (between 0 and 
     * AttbuteCount() - 1) of the attribute whose
     * value is to be returned.
     * @param aValue On successful return, points
     * to the value of the specified attribute.
     * 
     * @return KErrNone if the attribute is found,
     * KErrNotFound if the specified attribute
     * cannot be found, ERcWrongParserState if 
     * the method fails.
     */
    virtual TInt AttributeValuePredefEntitiesNotEscaped(TInt aIndex,
                                TPtrC8& aValue) = 0;

    /**
     * Returns the value of the specified attribute
     * of the current element. The method is only
     * applicable if the parser state is EStateStartTag.
     *
     * @param aName The name of the attribute whose
     * value is to be returned
     * @param aValue On successful return, points
     * to the value of the specified attribute.
     * 
     * @return KErrNone if the attribute is found,
     * KErrNotFound if the specified attribute
     * cannot be found, ERcWrongParserState if 
     * the method fails.
     */
    virtual TInt AttributeValuePredefEntitiesNotEscaped(const TDesC8& aName,
                                TPtrC8& aValue) = 0;

    /**
     * Returns the value of the specified attribute
     * of the current element. The method is only
     * applicable if the parser state is EStateStartTag.
     *
     * @param aIndex The index (between 0 and 
     * AttbuteCount() - 1) of the attribute whose
     * value is to be returned.
     * @param aValue On successful return, points
     * to the value of the specified attribute.
     * Character references have been escaped, i.e.
     * &#91;   ->  [
     * &#x5d;  ->  ]
     * etc.
     * Also predefined entities have been escaped.
     * &lt;    ->  <
     * &gt;    ->  >
     * &amp;   ->  &
     * &apos;  ->  '
     * &quot;  ->  "
     * 
     * @return KErrNone if the attribute is found,
     * KErrNotFound if the specified attribute
     * cannot be found, ERcWrongParserState if 
     * the method fails.
     */
    virtual TInt AttributeValueL(TInt aIndex, 
                                TPtrC8& aValue) = 0;

    /**
     * Returns the value of the specified attribute
     * of the current element. The method is only
     * applicable if the parser state is EStateStartTag.
     *
     * @param aName The name of the attribute whose
     * value is to be returned
     * @param aValue On successful return, points
     * to the value of the specified attribute.
     * Character references have been escaped, i.e.
     * &#91;   ->  [
     * &#x5d;  ->  ]
     * etc.
     * Also predefined entities have been escaped.
     * &lt;    ->  <
     * &gt;    ->  >
     * &amp;   ->  &
     * &apos;  ->  '
     * &quot;  ->  "
     * 
     * @return KErrNone if the attribute is found,
     * KErrNotFound if the specified attribute
     * cannot be found, ERcWrongParserState if 
     * the method fails.
     */
    virtual TInt AttributeValueL(const TDesC8& aName, 
                                TPtrC8& aValue) = 0;

    /**
     * Returns information as to whether the current
     * start tag is an empty element tag or not. The 
     * method is only applicable if the parser state 
     * is EStateStartTag.
     *
     * @param aIsEmptyElement On successful return, 
     * ETrue if the start tag is an empty element tag, 
     * EFalse otherwise.
     * 
     * @return KErrNone if the method succeeds,
     * ERcWrongParserState if the method fails.
     */
    virtual TInt IsEmptyElement(TBool& aIsEmptyElement) = 0;

    /**
     * Returns the text of the current element.
     * This method is only applicable if the parser
     * state is EStateText.
     *
     * @param aText On successful return, points
     * to the text of the current element.
     * 
     * @return KErrNone if the method succeeds,
     * ERcWrongParserState if the method fails.
     */
    virtual TInt TextPredefEntitiesNotEscaped(TPtrC8& aText) = 0;

    /**
     * Returns the text of the current element.
     * This method is only applicable if the parser
     * state is EStateText.
     *
     * @param aText On successful return, points
     * to the text of the current element.
     * Character references have been escaped, i.e.
     * &#91;   ->  [
     * &#x5d;  ->  ]
     * etc.
     * Also predefined entities have been escaped.
     * &lt;    ->  <
     * &gt;    ->  >
     * &amp;   ->  &
     * &apos;  ->  '
     * &quot;  ->  "
     * 
     * @return KErrNone if the method succeeds,
     * ERcWrongParserState if the method fails.
     */
    virtual TInt TextL(TPtrC8& aText) = 0;

    /**
     * Returns the current position of a virtual "cursor"
     * that is used to read the XML document.
     *
     * @return The cursor position
     */
    virtual TInt Pos() = 0;

    /**
     * Returns the start position of the current element.
     *
     * @return The current element start position
     */
    virtual TInt CurrentElementPos() = 0;

    /**
     * Returns the length of the XML document.
     *
     * @return The length of the XML document.
     */
    virtual TInt Length() = 0;

    /**
     * Returns the specified part of the XML document
     * being parsed to the caller
     *
     * @param aStartPos The position of the first character
     * to return
     * 
     * @param aEndPos The position of the last character
     * to return.
     * 
     * @return A part of the XML document being parsed
     */
    virtual TPtrC8 DocPart(TInt aStartPos, TInt aEndPos) = 0;
    };

#endif // __XPP_API__
