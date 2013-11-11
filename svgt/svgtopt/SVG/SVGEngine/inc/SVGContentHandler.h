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
* Description:  SVG Engine header file
 *
*/


#ifndef CSVGCONTENTHANDLER_H
#define CSVGCONTENTHANDLER_H


#include <e32base.h>
#include <s32strm.h>

#include "GfxColor.h"
#include "SVGEngineInterfaceImpl.h"
#include "SVGAttributeVerifier.h"
#include "SVGErrorImpl.h"

#include "xml/mxmlattributes.h"
#include "xml/mxmlcontenthandler.h"

#include "SVGSchemaData.h"
#include "SVGDocumentImpl.h"

/**
 * SMIL2 parser class
 */
class   CSvgElementImpl;
class CSvgScriptElementImpl;
class CSvgImageElementImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgContentHandler : public CBase, public MXMLContentHandler
    {
    public:
        /**
        * Parser phase one constructor
        *
        * Takes the player interface object as a parameter. This interface
        * is used by the parser for reporting parse errors and for getting values
        * of the content control attributes. It is also passed to the created
        * presentation object.
        */

        /**
         * Two phase constructor
         *
         * @since 1.0
         * @param None
         * @return An isntance of this class
         */
        static CSvgContentHandler*  NewL( TBool aRemoveFalseSwitchElements = ETrue );

        /**
         * Two phase constructor
         *
         * @since 1.0
         * @param None
         * @return An instance of this class
         */
        static CSvgContentHandler*  NewLC();


        /**
         * Part of two phase construction
         *
         * @since 1.0
         * @param None
         * @return None
         */
        virtual void            ConstructL( TBool aRemoveFalseSwitchElements = ETrue );

        /**
        * Destructor.
        */

        /**
         * Destructor
         *
         * @since 1.0
         * @param None
         * @return None
         */
        virtual                 ~CSvgContentHandler();




        /**
         *
         * Parses the given encrypted SVG source, constructing the presentation objects.
         * This is a variant of ParseL and processes encrypted SVG sources.
         *
         * @since 1.0
         * @param aDocument - The document object to which this document is constructed
         * @param aSvgFileName - The URI to the encrypted SVG source
         * @param aSuccess - An error code that signals successful/unsuccessful parsing
         * @return Instance of the root element of the SVG document.
         */
        CSvgElementImpl* ParseByteData( CSvgDocumentImpl* aDocument,
                                        const TDesC8& aByteData,
                                        CSvgErrorImpl& aError );

		/*
		* Parse the given 16 bit string of SVG
		* This method is used primarily for JSR226 to avoid conversion of strings
		*
		*/
		CSvgElementImpl* Parse16BitData( CSvgDocumentImpl* aDocument,
                                        const TDesC16& aSvgString,
                                        CSvgErrorImpl& aError );
                                        
		
		
        CSvgElementImpl* ParseFile( CSvgDocumentImpl* aDocument,
                                    const TDesC& aFileName,
                                    CSvgErrorImpl& aError );
        /*************** MXMLContentHandler interface methods **************/

        /**
         * A callback that signals start of a document
         *
         * @since 1.0
         * @param
         * @return A value that indicates success/failure
         */
        TInt StartDocument();

        /**
         * A callback that signals end of a document
         *
         * @since 1.0
         * @param
         * @return A value that indicates success/failure
         */
        TInt EndDocument();

        /**
        * Returns reader index
        * @since Series60 3.1
        * @param aIndex - reader index
        */
	    void ReaderIndex(NW_Uint32 aIndex);

        /**
         * A callback that signals beginning of an element
         *
         * @since 1.0
         * @param aURI -
         * @param aLocalName -
         * @param aName - Name of the element
         * @param aAttributeList - a list that keeps track of the attributes found for this element
         * @return A value that indicates success/failure
         */
        TInt StartElement( TDesC& aURI, TDesC& aLocalName,
                           TDesC& aName, MXMLAttributes* aAttributeList );
        /**
         * A callback that signals end of an element
         *
         * @since 1.0
         * @param aURI -
         * @param aLocalName -
         * @param aName - Name of the element
         * @return A value that indicates success/failure
         */
        TInt EndElement( TDesC& aURI, TDesC& aLocalName, TDesC& aName );

        /**
         * Callback that signals characters inside an XML document
         *
         * @since 1.0
         * @param aBuf - A string buffer that will contain the characters
         * @param aStart - An index indicating the starting position of the characters
         * @param aLenght - AN offset indicating the terminal position of the characters
         * @return A value that indicates success/failure
         */
        TInt Charecters( TDesC& aBuf, TInt aStart, TInt aLength );

        /**
         * A callback that signals a XML comment
         *
         * @since 1.0
         * @param aComment - A string buffer that cintains the comment
         * @return A value that indicates success/failure
         */

        TInt Comment( TDesC& aComment );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return A value that indicates success/failure
         */
        TInt ProcessingInstructions( TDesC& aTarget, TDesC& aData );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return A value that indicates success/failure
         */
        TInt IgnoreWhiteSpace( TDesC& aString );

        /**
         * A callback that signals beginning of an entity element
         *
         * @since 1.0
         * @param aName - A string buffer that will contain the entity name
         * @return A value that indicates success/failure
         */
        TInt StartEntity( TDesC& aName );

        /**
         * A callback that signals end of an entity element
         *
         * @since 1.0
         * @param aName - A string buffer that will contain the entity name
         * @return A value that indicates success/failure
         */
        TInt EndEntity( TDesC& aName );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return A value that indicates success/failure
         */
        TInt SkippedEntity( TDesC& aName );

        /**
         * A callback that signals beginning of a CDATA section
         *
         * @since 1.0
         * @param
         * @return A value that indicates success/failure
         */
        TInt StartCDATA();

        /**
         * A callback that signals end of a CDATA section
         *
         * @since 1.0
         * @param
         * @return A value that indicates success/failure
         */
        TInt EndCDATA();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return A value that indicates success/failure
         */
        TInt Error( TInt aErrorCode, TInt aSeverity );
        
        void DataBuffer(const TDesC16& aBuf);

        /**
         * Request to cancel parsing.
         */
        void CancelParsing();

    protected:


        /**
         * Method to verify the value of the attribute based on its type. If the value is
         * valid then this method sets the value to the corresponding attribtue in the
         * current element
         * @since 1.0
         * @aAttrName   - name of the attribute
         * @aAttrValue  - value of the attribute
         * @return
         */

        TBool ProcessNConvertAttrL( const TDesC& aName, const TDesC& aAttrName, const TDesC& aAttrValue );



        /**
         * Parse for a color from the given string.
         *
         * @since 1.0
         * @param aString : color string to parse for color value
         * @param aTransparent : transparent flag.
         * @return parsed color.
         */
        TGfxColor               ParseColor( const TDesC& aString,
                                            TBool& aTransparent );
        /**
         * Process for a color from the given attribute name and value.
         *
         * @since 1.0
         * @param aName : attribute name
         * @param aValue : attribute value.
         * @return true if attribute is processes as a color attribute.
         */
        TBool                   ProcessColorAttributeL( const TDesC& aName,
             const TDesC& aValue );


        /**
         * Process a for style info.
         *
         * @since 1.0
         * @param aName : attribute name
         * @param aValue : attribute value
         * @return true if a style info is processed.
         */
        TBool                   ProcessStyleL( const TDesC& aValue );

        /**
         * Process a for transform info.
         *
         * @since 1.0
         * @param aName : attribute name
         * @param aValue : attribute value
         * @return true if a transform info is processed.
         */
        TBool                   ProcessTransformL( const TDesC& aName,
                                                  const TDesC& aValue );

        /**
         * Process a for unit info.
         *
         * @since 1.0
         * @param aName : attribute name
         * @param aValue : attribute value
         * @return true if a unit info is processed.
         */
        TBool                   ProcessUnitsL( const TDesC& aName,
                                               const TDesC& aValue );

        /**
         * Convert any xml entity characters in the given string list to unicode characters.
         *
         * @since 1.0
         * @param aArray : array of strings to search for xml entities characters.
         * @return
         */
        void   ConvertEntitiesToCharsL( CDesCArrayFlat& aArray );

        /**
         * Convert any xml entity characters in the given string to unicode characters.
         *
         * @since 1.0
         * @param aString : string to search for xml entities characters.
         * @return non-NULL if at least one entity character is encountered.
         */
        HBufC* ConvertEntitiesToCharsL( const TDesC& aString );

        /**
         * Find the given character in the given string at the starting index.
         *
         * @since 1.0
         * @param aString : string to find character.
         * @param aStartIndex : starting index.
         * @param aChar : character to search.
         * @return index of first character encounterd, if found, -1 otherwise.
         */
        TInt   Find( const TDesC& aString, TInt aStartIndex, TChar aChar );

        /**
         * Create a substring from the given string.
         *
         * @since 1.0
         * @param aString : string to create a substring.
         * @param aStartIndex : starting index.
         * @param aEndIndex : endind index.
         * @return substring from starting and ending indices.
         */
        HBufC* SubstringL( const TDesC& aString, TInt aStartIndex, TInt aEndIndex );

        /**
         * Convert the given string (xml entity character) to an unicode character.
         *
         * @since 1.0
         * @param aChar : character object to store unicode character.
         * @param aString : xml entity character string.
         * @return true if there was a conversion.
         */
        TBool  ConvertEntityToChar( TChar& aChar, const TDesC& aString );

        /**
         * Append a substring to the given string at given indices.
         *
         * @since 1.0
         * @param aDest : string to append substring to.
         * @param aSource : string to append substring from.
         * @param aStartIndex : starting index for substring.
         * @param aEndIndex : ending index for substring.
         * @return
         */
        void   Append( TDes& aDest, const TDesC& aSource, TInt aStartIndex, TInt aEndIndex );

        /**
         * Convert the given string (xml entity character) an unicode character.
         *
         * @since 1.0
         * @param aChar : character object to store unicode character.
         * @param aString : xml decimal entity character string.
         * @return true if a conversion was made.
         */
        TBool  ConvertDecimalStringToChar( TChar& aChar, const TDesC& aString );

        /**
         * Convert the given string (xml entity character) an unicode character.
         *
         * @since 1.0
         * @param aChar : character object to store unicode character.
         * @param aString : xml hexadecimal entity character string.
         * @return true if a conversion was made.
         */
        TBool  ConvertHexStringToChar( TChar& aChar, const TDesC& aString );


        void FilterCharactersL( TDesC& aString );

        // Leaveable StartElementL function called by StartElement
        TInt StartElementL( TDesC& aURI, TDesC& aLocalName,
                           TDesC& aName, MXMLAttributes* aAttributeList );


        void AppendToCData( const TDesC& aText );

        TBool ExtractStyleValuePair( const TDesC& aString, TPtrC& aName, TPtrC& aValue );

        void SetForwardReferences(CSvgErrorImpl& aError);

        void RemoveFalseSwitchCasesOrMakeInvisible();
        void RemoveFalseElementsOrMakeInvisible();
        TBool VerifyReqExReqFtrSysL( CSvgElementImpl* aElement );

        // Initiate image loading for <image> elements
        void LoadImages();

        /**
         * Find out the system language for the client environment  based on
         * SVG source hint
         *
         * @since 1.0
         * @param aValue - A string buffer that contains the System Language hint
         * @return  None
         */
        void SystemLanguage( TPtr aValue );

        /**
         * Private constructor
         *
         * @since 1.0
         * @param
         * @return
         */
                        CSvgContentHandler();

        /**
         * Check for possible svg elements placed inside entity references.
         * If so, attempt to parse as part of the document.
         * If parsing was successful, return ETrue, otherwise return EFalse.
         *
         * @since 1.0
         * @param
         * @return If parsing was successful
         */
        TBool ParseEntity( const TDesC& aEntity );

        /**
         * Post parsing processing, such as removing false switch elements.
         *
         * @since 1.0
         * @param
         * @return none
         */
        void PostParseProcessing();

        /**
         * Removes references of element. Usually called before the element is
         * deleted.
         *
         * @since s60 3.2
         * @param aElement Pointer of element that is about to be deleted.
         * @return none
         */
        void RemoveInternalReferences( CSvgElementImpl* aElement );

    private:

        CSvgElementImpl*iRootElement; // SVG Element
        CSvgElementImpl*iCurrentElement; // Latest discovered element
        CSvgElementImpl*iCurrentParentElement; // Parent element for the current depth
        CSvgDocumentImpl* iDocument; // The document object to which this SVG source is processed



        // Various booleans that indicate success/failure or true/false for various attributes
        TBool       iIsColorAnim;
        // Indicating begin or end attribute.
        TBool       iBeginEndAtr;


        // Various buffers that hold animation time values
        TPtrC       iFromVal;
        TPtrC       iToVal;
        TPtrC       iByVal;
        TPtrC       iValues;
        TPtrC       iTimes;
        TPtrC       iSplines;

        HBufC*      iSystemLanguage;

        // Various attributes that hold document "depth" information
        TInt        iIgnoreDepth;
        TBool       iIsSvgElement;

        CSvgErrorImpl*  iSvgError;

        /*Added to support forward references in Use and animtion elements */
        RPointerArray<CSvgElementImpl>*    iUseElementArray;
        RPointerArray<CSvgElementImpl>*     iAnimRefElementArray;

        //array to keep track of switch elements
        //so we can go back at the end of parsing and remove the elements
        //that dont evaluate to true from memory
        RPointerArray<CSvgElementImpl>*    iSwitchElementArray;
        RPointerArray<CSvgElementImpl>*    iReqFetAttSysArray;
        RPointerArray<CSvgElementImpl>*    iAnimationElementArray;
        // CData container -- used to append pieces of cdata from XML parser
        // callbacks: Charecters & StartEntity
        HBufC* iCData;

        // Reference to loading listeners
        const RPointerArray<MSvgLoadingListener>* iLoadingListeners;

        TBool iCancelRequested;

        RPointerArray<CSvgImageElementImpl>    iImageElements;
				TBool iRemoveFalseSwitchElements;
    };

#endif
