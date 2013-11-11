/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Misc. libxml2 related utilities.
*
*/


#ifndef XML_UTILS_H
#define XML_UTILS_H

// INCLUDES
#include <e32std.h>
#include <xmlengdom.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CXmlEncoding;
class CXmlEntity;
class RXmlEngDocument;
class TXmlEngElement;

// CLASS DECLARATION


/**
*  Misc. libxml2 related utilities.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CXmlUtils: public CBase
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CXmlUtils* NewL();
        
        /**
        * Destructor.
        */        
        virtual ~CXmlUtils();


    public:  // New methods
        /**
        * Returns a document from the provided buffer.
        *
        * @since 3.0
        * @param aBuffer A buffer in any char-encoding.
        * @param aCharSet The char-set.
        * @return A document.
        */
        RXmlEngDocument ParseBufferL(const TDesC8& aBuffer,
                const TDesC& aCharSet) const;

        /**
        * Resolves any entities and escaped chars in the given url.
        *
        * @since 3.0
        * @param aUrl The url to clean up.
        * @return void.
        */
        void CleanupUrlL(TDes& aUrl) const;

        /**
        * Remove any markup found in the given descriptor.
        *
        * @since 3.0
        * @param aBuffer The buffer to clean up.
        * @param aNewLineChar The char used to insert a newline.
        * @return ETrue if markup was found in aOrig.
        */
        TBool CleanupMarkupL(TDes& aBuffer, TInt aNewLineChar) const;

        /**
        * Resolves any entities found in aOrig the named entity into its char-value.
        *
        * @since 3.0
        * @param aBuffer The buffer to clean up.
        * @return ETrue if entities were found in aOrig.
        */
        TBool ResolveEntitiesL(TDes& aBuffer) const;

        /**
        * Performs a deep extraction of the text children of the given node.  The result
        * is returned as a 16-bit descriptor.  The char encoding is always ucs2.
        *
        * @since 3.0
        * @param aElement A element.
        * @return The text or NULL.
        */
        HBufC* ExtractTextL(TXmlEngElement aElement) const;

        /**
        * Performs a deep extraction of the text children of the given node.  The result
        * is returned as a 16-bit descriptor.    The char encoding is always ucs2.  This
        * method is different from ExtractTextL in that it doesn't resolve entities or
        * do any other clean up.  It also has a length param.
        *
        * @since 3.1
        * @param aElement A element.
        * @param aMaxLength The max length to extract or 0 to extract the entire string.
        * @param aFromEnd If ETrue the aMaxLength chars are taken from the end of the string.
        * @return The text or NULL.
        */
        HBufC* ExtractSimpleTextL(TXmlEngElement aElement, TInt aMaxLength,
                TBool aFromEnd = EFalse) const;

        /**
        * Returns the first element in the given document.
        *
        * @since 3.0
        * @param aDocument A document.
        * @return A element or NULL-Element.
        */
        TXmlEngElement GetDocumentFirstElement(RXmlEngDocument aDocument) const;
        
        /**
        * Returns the first child of the given node.
        *
        * @since 3.0
        * @param aElement A element.
        * @return A element or NULL-Element.
        */
        TXmlEngElement GetFirstElementChild(TXmlEngElement aElement) const;

        /**
        * Returns the first child of the given node with the provided name.  aUtf8Name
        * MUST be null terminated.
        *
        * @since 3.0
        * @param aElement A element.
        * @param aName The name of the node.
        * @return A element or NULL-Element.
        */
        TXmlEngElement GetFirstNamedChild(TXmlEngElement aElement, const TDesC8& aUtf8Name) const;

        /**
        * Returns the next sibling of the given node.
        *
        * @since 3.0
        * @param aNode A element.
        * @return A element or NULL-Element.
        */
        TXmlEngElement GetNextSiblingElement(TXmlEngElement aElement);

        /**
        * Returns true if the given node has a name of aName.  This method doesn't 
        * compare the namespace.
        *
        * @since 3.0
        * @param aElement A element.
        * @param aName The name of the node.
        * @return ETrue if they match.
        */
        TBool IsNamed(TXmlEngElement aElement, const TDesC8& aUtf8Name) const;

        /**
        * Returns true if the given node has match the given namespace and name.
        *
        * @since 3.0
        * @param aElement A element.
        * @param aNameSpace The namespace of the node.
        * @param aName The name of the node.
        * @return ETrue if they match.
        */
        TBool IsNamed(TXmlEngElement aElement, const TDesC8& aUtf8NameSpace, 
                const TDesC8& aUtf8Name) const;

        /**
        * Returns the attribute's value or NULL if the attribute wasn't present.
        *
        * @since 3.0
        * @param aElement A element.
        * @param aAttribute The attribute to extract.
        * @return The value or NULL.
        */
        HBufC* AttributeL(TXmlEngElement aElement, const TDesC8& aAttribute) const;


    private:  // New methods
        /**
        * C++ default constructor.
        */
        CXmlUtils();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
                
        /**
        * Cleanup stack callback method to cleanup ParseBufferL.
        *
        * @since 3.0
        * @param aPtr A libxml2 parser context ptr.
        * @return void.
        */
        static void CleanupParseBuffer(TAny *aPtr);

        /**
        * Skip any chars before the XML-prolog.
        *
        * @since 3.0
        * @param aBuffer A xml buffer.
        * @param aLen The length of the xml buffer.
        * @return void.
        */
        void SkipCharsBeforeXmlProlog(const TUint8** aString, TInt& aLen) const;
        
        /**
        * Remove any markup found in the given descriptor.
        *
        * @since 3.0
        * @param aBuffer The string to be stripped of markup.
        * @param aNewLineChar The char used to insert a newline.
        * @return ETrue if markup was found in aBuffer.
        */
        TBool StripMarkupL(TDes& aBuffer, TInt aNewLineChar) const;        

        /**
        * Remove any CDATA markers in the given descriptor.
        *
        * @since 3.1
        * @param aBuffer The string to be stripped of markup.
        * @return ETrue if CDATA markers were found in aBuffer.
        */
        TBool StripCDataMarkers(TDes& aBuffer) const;
        
        
    private:  // Data
        TLeakTracker  iLeakTracker;
        
        RXmlEngDOMImplementation iImpl;
        CXmlEncoding*  iXmlEncoding;
        CXmlEntity*    iXmlEntity;
    };

#endif      // XML_UTILS_H
            
// End of File
