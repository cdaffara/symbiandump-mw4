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
* A simple XML writer API for generating
* non-unicode XML documents.
* The API does not require its implementations
* to validate the generated XML document in any
* way. It is up to the application using the API
* to create well-formed and valid XML documents.
*
*/


#ifndef __XW_API__
#define __XW_API__

#include <e32base.h>

class MXmlWriter 
    { 
public:
    /**
     * Adds a prolog tag <?xml version='1.0' encoding='ISO-8859-1'?>
     * to the document.
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void AddPrologL() = 0;

    /**
     * Adds a prolog tag to the document.
     *
     * @param aVersionNum The version number in the prolog.
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void AddPrologL(const TDesC8& aVersionNum) = 0;

    /**
     * Adds a start tag to the document.
     *
     * @param aName The name of the start tag.
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void AddStartTagL(const TDesC8& aName) = 0;
    
    /**
     * Adds an open start tag to the document.
     *
     * @param aName The name of the element
     * 
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void OpenStartTagL(const TDesC8& aName) = 0;

    /**
     * Adds an attribute to the currently open start tag.
     *
     * @param aName The name of the attribute
     *
     * @param aValue The value of the attribute
     * Predefined entities are inserted.
     * <  ->  &lt;
     * >  ->  &gt;
     * &  ->  &amp;
     * '  ->  &apos;
     * "  ->  &quot;
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void AddAttributeL(const TDesC8& aName, const TDesC8& aValue) = 0;

    /**
     * Ends an already opened start tag in the XML document.
     *
     * @param aEndElement If ETrue, specifies that the start tag also
     * ends the element (i.e. the element is an empty element and will
     * not have a separate end tag). If EFalse, specifies that the
     * element will end with a separate end tag that will be added
     * with the AddEndTagL method.
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void CloseStartTagL(TBool aEndElement) = 0;

    /**
     * Adds text to an element whose start tag has been added to
     * the XML document.
     *
     * @param aText The element text
     * Predefined entities are inserted.
     * <  ->  &lt;
     * >  ->  &gt;
     * &  ->  &amp;
     * '  ->  &apos;
     * "  ->  &quot;
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void AddTextL(const TDesC8& aText) = 0;

    /**
     * Adds text to an element whose start tag has been added to
     * the XML document.
     *
     * @param aText The element text, that is NOT modified in any way.
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void AddXmlTextL(const TDesC8& aText) = 0;

    /**
     * Adds an end tag to the document.
     *
     * @param aName The name of the end tag.
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void AddEndTagL(const TDesC8& aName) = 0;

    /**
     * Adds an CDATA section to document.
     *
     * @param aText The text in the CDATA section.
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void AddCdataSectL(const TDesC8& aText) = 0;

    /**
     * Inserts text to the specified position in the document.
     * The text may include any kind of text, including XML.
     *
     * @param aPos The position in the document where
     * the text is to be inserted.
     *
     * @param aText The text to insert
     * Predefined entities are inserted.
     * <  ->  &lt;
     * >  ->  &gt;
     * &  ->  &amp;
     * '  ->  &apos;
     * "  ->  &quot;
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void InsertTextL(TInt aPos, const TDesC8& aText) = 0;

    /**
     * Inserts text to the specified position in the document.
     * The text may include any kind of text, including XML.
     *
     * @param aPos The position in the document where
     * the text is to be inserted.
     *
     * @param aText The text to insert
     *
     * On unexceptional conditions, the method leaves with
     * one of the system-wide error codes.
     */
    virtual void InsertXmlTextL(TInt aPos, const TDesC8& aText) = 0;

    /**
     * Returns the current length of the XML document.
     *
     * @return The current length of the XML document
     */
    virtual TInt Length() = 0;
    
    /**
     * Returns the specified part of the current
     * XML document to the caller
     *
     * @param aStartPos The position of the first character
     * to return
     * 
     * @param aEndPos The position of the last character
     * to return
     * 
     * @return A part of the current XML document
     */
    virtual TPtrC8 DocPart(TInt aStartPos, TInt aEndPos) = 0;

    /**
     * Resets the writer. A new XML-document can be written
     * using the same instance.
     */
    virtual void Reset() = 0;
    };

#endif // __XW_API__
