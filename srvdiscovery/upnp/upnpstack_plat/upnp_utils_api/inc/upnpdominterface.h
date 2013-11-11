/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares UpnpDomInterface functions
*
*/


#ifndef C_UPNPDOMINTERFACE_H
#define C_UPNPDOMINTERFACE_H

// INCLUDES

#include <e32def.h>
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>
#include <utf.h>
#include <xml/dom/xmlengdom.h>

namespace UpnpDomInterface
    {
    
	//CONSTANTS
    _LIT8(KContainer, "container");
    _LIT8(KSearchable, "searchable");
    _LIT8(KTrueVal, "1");
    
	/*
	* GetAttrValueL
	* Gets value of the indicated attribut for the given tag.
	* @Param aElement reference to the Element which attribute's value 
	*	should be returned.
	* @Param aAttribute name of the attribute which value should be returned.
	* @Return attribute's value or empty descriptor if Element has no such attribute.
	*/
	IMPORT_C TPtrC8 GetAttrValueL( const TXmlEngElement& aElement,
                    	      	   const TDesC8& aAttribute );	
    
    /*
	* GetElementValueL
	* Gets value of the indicated element.
	* @Param aElement reference to the Element which value should be returned.
	* @Return value or empty descriptor if Element has no content.
	*/
	IMPORT_C TPtrC8 GetElementValueL( const TXmlEngElement& aElement );	
    
    /*
	* AllocElementValueL
	* Gets value of the indicated element and allocs it on heap.
	* @Param aElement reference to the Element which value should be returned.
	* @Return pointer to allocated descriptor eith content or NULL if Element has no content.
	*/
	IMPORT_C HBufC* AllocElementValueL( const TXmlEngElement& aElement );	
    
                    	      	    
	/*
	* CheckTagL
	* Checks if given XML element has given tag.	
	* @Param aElement reference to the Element that is being checked
	* @Param aTag LocalName of the Element which is being checked
    * @Return ETrue if element's name is equal to the passed one 
	*/
	IMPORT_C TBool CheckTagL( const TXmlEngElement& aElement,
                    	      const TDesC8& aTag );
	
	/*
	* CheckTagL
	* Checks if given XML element has given tag and contains given value.
	* @Param aElement reference to the Element that is being checked
	* @Param aTag LocalName of the Element which is being checked
    * @Param aTagValue The value that is checked againts 
    * the contents of the aTag element
    * @Return ETrue if element meets searching criteria
	*/
	IMPORT_C TBool CheckTagL( const TXmlEngElement& aElement,
                       		  const TDesC8& aTag,
                       		  const TDesC8& aTagValue );
    
    /*
    * CheckSubTagL
    * Checks if the given elements has an element whose 
    * local name is specified by the Tag
    * @Param aElement reference to the Element that is being checked
    * @Param aTag LocalName that is being searched for 
    * @Return ETrue if subtag meets search criteria
    */
    IMPORT_C TBool CheckSubTagL( const TXmlEngElement& aElement,
                                 const TDesC8& aTag );
                                                    		 
    /*
    * CheckSubTagL
    * Checks the value of the specified SubTag (Element) against 
    * the given value returns ETrue if match found else EFalse 
    * @Param aElement reference to the Element that is being checked
    * @Param aTag LocalName of the Element which is being checked
    * @Param aTagValue The value that is checked againts 
    * the contents of the Tag element
    * @Return ETrue if subtag meets search criteria
    */
    IMPORT_C TBool CheckSubTagL( const TXmlEngElement& aElement,
                                 const TDesC8& aTag,
                                 const TDesC8& aTagValue );
    
    /*
    * CheckSubTagPrefixL
    * Checks the value of the specified SubTag (Element) against 
    * the given value returns ETrue if match found else EFalse 
    * @Param aElement reference to the Element that is being checked
    * @Param aNameSpacePrefix Specifies the wanted prefix
    * @Param aTag LocalName of the Element which is being checked
    * @Param aTagValue The value that is checked againts 
    * the contents of the Tag element
    * @Return ETrue if subtag meets search criteria
    */
    IMPORT_C TBool CheckSubTagPrefixL( const TXmlEngElement& aElement,
                                       const TDesC8& aNameSpacePrefix,
                                       const TDesC8& aTag,
                                       const TDesC8& aTagValue );
    
    /*
    * CheckAttributeValueL
    * Checks the attribute value of the given element against the given value. 
    * @Param aElement reference to the Element being checked
    * @Param aAttribute The Name of the attribute which is being checked
    * @Param aValue The value that is checked against the value of the aAttribute
    * @Return ETrue if attribute meets given criteria
    */
    IMPORT_C TBool CheckAttributeValueL( const TXmlEngElement& aElement,
                                         const TDesC8& aAttribute,
                                         const TDesC8& aValue);
                                         
    /*
    * DeleteElement
    * Deletes the specified element from the DomTree
    * @param aElement reference to the Element to be deleted
    * @return the parent element
    */
    IMPORT_C TXmlEngElement DeleteElement(TXmlEngElement& aElement);      
                                   
    /*
    * GetElementL
    * Searches the DomTree for the element specified by the given specifiers.
    * @param aElement reference to a "Root" element from which the search will start
    * @param aWanted Specifies the location of the reference 
    * which will be set to point to the wanted Element
    * @param aMainTag LocalName of the Element that is being searched for 
    * @param aDepth specifies how deep into the tree the search will proceed 
    * (if set to -1 the whole tree will be searched)
    * @param aCurrentDepth How deep into the tree the search has proceeded 
    * (should be set to 0 when the search is started)
    * @Return ETrue if element is found
    */
    IMPORT_C TBool GetElementL( const TXmlEngElement& aElement, 
                                TXmlEngElement& aWanted, 
                                const TDesC8& aMainTag,
                                TInt aDepth=-1, 
                                TInt aCurrentDepth=0);
    
  	/*
    * GetElementL
    * Searches the DomTree for the element specified by the given specifiers.
    * @param aElement reference to a "Root" element from which the search will start
    * @param aWanted Specifies the location of the reference 
    * which will be set to point to the wanted Element
    * @param aMainTag LocalName of the Element that is being searched for 
    * @param aSubTag LocalName of an Element within 
    * the MainElement that is being searched for
    * @param aDepth specifies how deep into the tree the search will proceed 
    * (if set to -1 the whole tree will be searched)
    * @param aCurrentDepth How deep into the tree the search has proceeded 
    * (should be set to 0 when the search is started)
    * @Return ETrue if element is found
    */
    IMPORT_C TBool GetElementL( const TXmlEngElement& aElement, 
    						    TXmlEngElement& aWanted,
                                const TDesC8& aMainTag,
                                const TDesC8& aSubTag,
                                TInt aDepth=-1,
                                TInt aCurrentDepth=0);
    
    /**
    * GetElementL
    * Searches the DomTree for the element specified by the given specifiers.
    * @param aElement reference to a "Root" element from which the search will start
    * @param aWanted Specifies the location of the reference 
    * which will be set to point to the wanted Element
    * @param aMainTag LocalName of the Element that is being searched for 
    * @param aSubTag LocalName of an Element within 
    * the MainElement that is being searched for
    * @param aSubTagValue Contents of the Element specified by the aSubTag
    * @param aDepth specifies how deep into the tree the search will proceed 
    * (if set to -1 the whole tree will be searched)
    * @param aCurrentDepth How deep into the tree the search has proceeded 
    * (should be set to 0 when the search is started)
    * @Return ETrue if element is found 
    */
    IMPORT_C TBool GetElementL( const TXmlEngElement& aElement, 
    						    TXmlEngElement& aWanted, 
                                const TDesC8& aMainTag,
                                const TDesC8& aSubTag,
                                const TDesC8& aSubTagValue,
                                TInt aDepth=-1,
                                TInt aCurrentDepth=0);
                                   
    /*
    * GetContentElementL
    * Searches the DomTree for the element specified by the given specifiers.
    * @param aElement reference to a "Root" element from which the search will start
    * @param aWanted Specifies the location of the reference 
    * which will be set to point to the wanted Element
    * @param aSubTag LocalName of an Element within 
    * the MainElement that is being searched for
    * @param aSubTagValue Contents of the Element specified by the aSubTag
    * @param aDepth specifies how deep into the tree the search will proceed 
    * (if set to -1 the whole tree will be searched)
    * @param aCurrentDepth How deep into the tree the search has proceeded 
    * (should be set to 0 when the search is started)
    * @Return ETrue if element is found
    */
    IMPORT_C TBool GetContentElementL( const TXmlEngElement& aElement, 
    								   TXmlEngElement& aWanted,
                                       const TDesC8& aSubTag,
                                       const TDesC8& aSubTagValue,
                                       TInt aDepth=-1,
                                       TInt aCurrentDepth=0);
    
    /*
    * GetDirectoryElementL
    * Searches the DomTree for the element specified by the given parameters 
    * @param aElement reference to a "Root" element from which the search will start
    * @param aWanted Specifies the location of the reference 
    * which will be set to point to the wanted Element
    * @param aMainTag LocalName of the Element that is being searched for 
    * @param aAttribute The name of the Attribute of the MainElement 
    * that is searched for
    * @param aAttributeValue The wanted value of the Attribute 
    * that is searched for 
    * @param aDepth specifies how deep into the tree the search will proceed 
    * (if set to -1 the whole tree will be searched)
    * @param aCurrentDepth How deep into the tree the search has proceeded 
    * (should be set to 0 when the search is started)
    * @Return ETrue if element is found
    */
    IMPORT_C TBool GetDirectoryElementL( const TXmlEngElement& aElement, 
    								     TXmlEngElement& aWanted,
                                         const TDesC8& aMainTag,
                                         const TDesC8& aAttribute,
                                         const TDesC8& aAttributeValue, 
                                         TInt aDepth=-1, 
                                         TInt aCurrentDepth=0);

    /*
    * GetDirectoryElementL
    * Searches the DomTree for the element specified by the given parameters 
    * @param aElement reference to a "Root" element from which the search will start
    * @param aWanted Specifies the location of the reference 
    * which will be set to point to the wanted Element
    * @param aAttribute The name of the Attribute of the MainElement 
    * that is searched for
    * @param aAttributeValue The wanted value of the Attribute 
    * that is searched for 
    * @param aDepth specifies how deep into the tree the search will proceed 
    * (if set to -1 the whole tree will be searched)
    * @param aCurrentDepth How deep into the tree the search has proceeded 
    * (should be set to 0 when the search is started)
    * @Return ETrue if element is found
    */
    IMPORT_C TBool GetDirectoryElementL( const TXmlEngElement& aElement, 
    								     TXmlEngElement& aWanted,
                                         const TDesC8& aAttribute,
                                         const TDesC8& aAttributeValue, 
                                         TInt aDepth=-1, 
                                         TInt aCurrentDepth=0);
    
    /*
    * GetElementListL
    * Creates a RArray of Elements that fit the given specifiers 
    * @param aElement reference to a "Root" element from which the search will start
    * @param aElementList list of elements matching searching criteria
    * @param aMainTag LocalName of the Element that is being searched for 
    * @param aSearchable Specifies whether or not 
    * the searchability of container-elements is checked
    * @Return ETrue if any element has been found.
    */
    IMPORT_C TBool GetElementListL( const TXmlEngElement& aElement,
                                    RArray<TXmlEngElement>& aElementList,
                                    const TDesC8& aMainTag,
                                    TBool aSearchable=EFalse );
    
    
    
    /*
    * GetElementList
    * Creates a RArray of Elements that fit the given specifiers 
    * @param aElement reference to a "Root" element from which the search will start
    * @param aElementList list of elements matching searching criteria
    * @param aMainTag LocalName of the Element that is being searched for 
    * @param aSubTag LocalName of an Element within 
    * the MainElement that is being searched for
    * @param aSubTagValue Contents of the Element specified by the aSubTag
    * @param aSearchable Specifies whether or not 
    * the searchability of container-elements is checked 	
 	* @Return ETrue if any element has been found.
    */
    IMPORT_C TBool GetElementListL( const TXmlEngElement& aElement, 
                                    RArray<TXmlEngElement>& aElementList,
                                    const TDesC8& aMainTag, 
                                    const TDesC8& aSubTag, 
                                    const TDesC8& aSubValue,
                                    TBool aSearchable=EFalse);

    
        
    /*
    * GetElementList
    * Creates a RArray of Elements that fit the given specifiers 
    * @param aElement reference to a "Root" element from which the search will start
    * @param aElementList list of elements matching searching criteria
    * @param aMainTag LocalName of the Element that is being searched for 
    * @param aSubTag LocalName of an Element within 
    * the MainElement that is being searched for
    * @param aSubTagValue Contents of the Element specified by the aSubTag
    * @Param aSearchable Specifies whether or not 
    * the searchability of container-elements is checked
    * @Param aNSPrefix the Prefix of the wanted element
    * @Return ETrue if any element has been found. 
    */
    IMPORT_C TBool GetElementListL( const TXmlEngElement& aElement, 
                                    RArray<TXmlEngElement>& aElementList,
                                    const TDesC8& aMainTag,
                                    const TDesC8& aSubTag,
                                    const TDesC8& aSubValue,
                                    const TDesC8& aNSPrefix,
                                    TBool aSearchable=EFalse );
    
    /*
    * GetDirectoryElementList
    * Creates a RArray of Elements that fit the given specifiers 
    * @param aElement reference to a "Root" element from which the search will start
    * @param aElementList list of elements matching searching criteria
    * @param aAttrName The name of the Attribute of the Element 
    * that is searched for
    * @param aAttributeValue The wanted value of the Attribute 
    * that is searched for 
    * @param aSearchable Specifies whether or not the searchability 
    * of container-elements is checked 
    * @Return ETrue if any element has been found. 
    */
    IMPORT_C TBool GetDirectoryElementListL( const TXmlEngElement& aElement, 
                                             RArray<TXmlEngElement>& aElementList,
                                             const TDesC8& aAttrName,
                                             const TDesC8& aAttrValue,
                                             TBool aSearchable=EFalse);
    
    /*
    * GetContentElementList
    * Creates a RArray of Elements that fit the given specifiers
    * @Param aElement reference to a "Root" element from which the search will start
    * @param aElementList list of elements matching searching criteria
    * @Param aMainTag LocalName of the Element that is being searched for 
    * @Param aSubTag LocalName of an Element within the MainElement 
    * that is being searched for
    * @Param aSubTagValue Contents of the Element specified by the aSubTag
    * @Param aSearchable Specifies whether or not 
    * the searchability of container-elements is checked 
    * @Return ETrue if any element has been found.
    */
    IMPORT_C TBool GetContentElementListL( const TXmlEngElement& aElement, 
                                           RArray<TXmlEngElement>& aElementList,
                                           const TDesC8& aSubTag,
                                           const TDesC8& aSubValue,
                                           TBool aSearchable=EFalse );               
                                          
    /*
    * SaveToFile
    * Saves the contents of the aRoot element into the given file.
    * All saving should be done with ROOT element.
    * @param aFileName name of the file
    * @param aRoot reference to the first element of the DomTree
    * @return ETrue if writing succeeded 
    */
    IMPORT_C TBool SaveToFileL(const TDesC16& aFileName, const TXmlEngElement& aRoot);
    
    /*
    * SaveToFile
    * Saves the contents of the aRoot element into the given file.
    * Writes given TDesC8 string to file
    * @param aFileName name of the file
    * @param aData string that is saved to the file
    * @return ETrue if writing succeeded 
    */
    IMPORT_C TBool SaveToFileL(const TDesC16& aFileName, const TDesC8& aData);
                                         
    
    
}

#endif      // C_UPNPDOMINTERFACE_H   
            
// End of File


