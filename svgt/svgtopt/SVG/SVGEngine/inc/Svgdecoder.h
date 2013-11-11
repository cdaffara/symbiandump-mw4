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
* Description:  SVG Implementation header file
 *
*/


#ifndef SVGDECODER_H
#define SVGDECODER_H

#include <stdio.h>
#include <e32std.h>
#include <string.h>
#include <e32base.h>
#include <f32file.h>
#include <s32file.h>

#include "SVGPaintCssValueImpl.h"
#include "SVGPathElementImpl.h"
#include "GfxGeneralPath.h"
#include "SVGSvgElementImpl.h"
#include "SVGImageElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGErrorImpl.h"

enum TGfxEncodedSegType
    {
    EGfxEncodedSegMoveTo,
    EGfxEncodedSegLineTo,
    EGfxEncodedSegQuadTo,
    EGfxEncodedSegCubicTo,
    EGfxEncodedSegClose
    };


class CSvgDecoder : public CBase
{
public:

	   /**
      * Two phase constructor
      *
      * @since 1.0
      * @param None
	  * @return An isntance of this class
      */
	static CSvgDecoder* NewL(const TDesC& aFileName);

	 /**
      * Two phase constructor
      *
      * @since 1.0
      * @param None
	  * @return An isntance of this class
      */
	static CSvgDecoder* NewL(const TDesC8& aByteData);

		/**
         * Reads a Descriptor's data
         *
         * @since 1.0
         * @param
         * @return the created descriptor
         */
	TDesC& DecodeTDesCLC();

    	/**
         * Reads a Descriptor's data. Used only for image element with
         *      Embedde image.       
         * @since 1.0
         * @param
         * @return the created descriptor
         */
	TDesC& DecodeImageEmbeddedTDesCLC();

		/**
         * start of the decoder
         *     
         * @since 1.0
         * @param
         * @return the root element
		 */
	CSvgElementImpl*         StartDecodingL(CSvgDocumentImpl *aDocument, CSvgErrorImpl& aError);


		/**
         * Read Data of a particular attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TInt                     DecodeAttributeL(const TUint16 aName);


		/**
         * Read Data of a presentation attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodePresentationAttributeL(const TUint16 aName);

		/**
         * Read Data of a SVGT attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeSVGTAttributeL(const TUint16 aName);

	/**
         * Read Data of a SVGT attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeAnimationAttributeL(const TUint16 aName);


		/**
         * Read Data of a SVGT attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeSvgElementAttributeL(const TUint16 aName);

		/**
         * Read Data of a SVGT attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeLangSpaceAttributeL(const TUint16 aName);

		/**
         * Read Data of an attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeTransformAttributeL(const TUint16 aName);

		/**
         * Read Data of an  attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeTestAttributeL(const TUint16 aName);

		/**
         * Read Data of an  attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeIdAndXmlAttributeL(const TUint16 aName);

		/**
         * Read Data of an  attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeUriRefAttributeL(const TUint16 aName);



	/**
     * Decode attributes for Discard element
     *
     * @since 1.2
     * @param Attribute Id
     * @return A value that indicates success/failure
     */
    TBool                    DecodeDiscardAttributeL (const TUint16& aName);


		/**
         * Read Data of an  attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */

	void                     SetPaintValueL (CCssValue*& aValue);

		/**
         * Read Data of an attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeColorCssValueL(const TUint16 aName);

		/**
         * Read Data of an  attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeFloatCssValueL(const TUint16 aName);

		/**
         * Read Data of an  attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
    TBool                    DecodeIntCssValueL(const TUint16 aName);

		/**
         * Read Data of an  attribute
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeStringCssValueL(const TUint16 aName);

		/**
         * Read a Des attribute value
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeAttributeDesL(const TUint16 aName);

		/**
         * Read a path attribute value
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeAttributePathL(const TUint16 aName);

	void                    DecodeAnimationPathL(CGfxGeneralPath*& aPath);

		/**
         * Read a float attribute value
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
	TBool                    DecodeAttributeFloatL(const TUint16 aName);

		/**
         * Read a Int attribute value
         *
         * @since 1.0
         * @param Attribute Id
         * @return A value that indicates success/failure
         */
    TBool                    DecodeAttributeIntL(const TUint16 aName);

	// checks whether required attributes are present.
	void                     CheckRequiredAttributesL(const TUint8  aName);


		 /**
         * Destructor
         *
         * @since 1.0
         * @param None
         * @return None
         */
	~CSvgDecoder();

	void RemoveFalseSwitchCases();
	void RemoveFalseElements();
	TBool VerifyReqExReqFtrSysL( CSvgElementImpl* aElement );

    /**
    * Find out the system language for the client environment  based on
    * SVG source hint
    *
    * @since 1.0
    * @param aValue - A string buffer that contains the System Language hint
    * @return  None
    */
    void SystemLanguage( TPtr aValue );

private:

	     // Default constructor
	CSvgDecoder();

		 /**
         * Part of two phase construction
         *
         * @since 1.0
         * @param None
         * @return None
         */
	void ConstructL(const TDesC& aFileName);

	// to read from a descriptor
	void ConstructL(const TDesC8& aByteData);
  
    /**
     * Removes references of element. Usually called before the element is
     * deleted.
     *
     * @since s60 3.2
     * @param aElement Pointer of element that is about to be deleted.
     * @return none
     */
    void RemoveInternalReferences( CSvgElementImpl* aElement );
        
private: // Data

	//All the functions in the class RReadStream that are used for reading data can Leave.
	RReadStream	   iStream;
	RFileReadStream iFStream;
	RDesReadStream  iDStream;
	RFs  iFs;

     CSvgElementImpl*iRootElement; // SVG Element
     CSvgElementImpl*iCurrentElement; // Latest discovered element
     CSvgElementImpl*iCurrentParentElement; // Parent element for the current depth
     CSvgDocumentImpl*   iDocument; // The document object to which this SVG source is processed

	  // Various attributes that hold document "depth" information
     TInt        iIgnoreDepth;
     TBool       iIsSvgElement;

    // Various booleans that indicate success/failure or true/false for various attributes
	 TBool       iIsAnimElement;

	 CSvgErrorImpl*  iSvgError;

	/* Added to support forward references in USE, Animation and Animate elements*/	 
     RPointerArray<CSvgElementImpl>*    iUseElementArray;
     RPointerArray<CSvgElementImpl>*    iAnimationElementArray;
	 RPointerArray<CSvgElementImpl>*	iAnimRefElementArray;

	//array to keep track of switch elements
	//so we can go back at the end of parsing and remove the elements
	//that dont evaluate to true from memory
	RPointerArray<CSvgElementImpl>*    iSwitchElementArray;
	RPointerArray<CSvgElementImpl>*	   iReqFetAttSysArray;
    
    //variable to tell if the encoder was encoding fixed points or floats   
	TBool iIsFixPt;
	TBool iIsRGB;
	
	RPointerArray<CSvgImageElementImpl>    iImageElements;
    // Embedded Images Need to be kept track cause loadimage is not called on them
    // iImageElements has only images that have uri
    // Document needs to be set with the total images so that it can perform a
    // callback to the client and indicate that all image loading is complete
    TInt iEmbeddedImagesCount;
};



#endif