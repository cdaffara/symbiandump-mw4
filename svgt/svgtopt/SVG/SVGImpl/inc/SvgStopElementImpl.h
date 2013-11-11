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
* Description:  SVG Implementation source file
 *
*/


#ifndef CSVGSTOPELEMENTIMPL_H
#define CSVGSTOPELEMENTIMPL_H

#include "SVGElementImpl.h"

class   CSvgDocumentImpl;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */

class CSvgStopElementImpl : public CSvgElementImpl
                        
    {
    public:

        // Constructor/deconstructor


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgStopElementImpl* NewL( const TUint8 aElemID,
                                          CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgStopElementImpl* NewLC( const TUint8 aElemID,
                                           CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual                     ~CSvgStopElementImpl();

        // From SVG DOM

        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt SetAttributeL( const TDesC& aName, const TDesC& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void GetStopColor(TUint32 &aVal){aVal=iStopColor;}
        void GetOffset(TFloatFixPt &aOffset){aOffset = iOffset;}
		void GetStopOpacity(TFloatFixPt &aStopOpacity){aStopOpacity = iStopOpacity;}
		void SetStopColorL(TUint32 &aVal);
				TInt GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue);
		TInt SetAttributeFloatL(const TInt aNameId,const TFloatFixPt aValue);
		TInt GetAttributeIntL( const TInt aNameId, TInt32& aValue );
		TInt SetAttributeIntL( const TInt aNameId,
                                        const TInt32 aValue );

		void Print( TBool aIsEncodeOn );
		TBool IsValidValue(const TDesC& aValue);
		TFloatFixPt iOffset;
		
		TBool IsValidElement() const;
    private:
        
		TFloatFixPt iStopOpacity;
        TUint32 iStopColor;
        TBool iValidOffsetValue; // Will be set to false if the value is improper.
        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                        ConstructL( const TUint8 aElemID );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                    CSvgStopElementImpl( CSvgDocumentImpl* aDoc );

    public:

    protected:

    };

#endif /* CSVGSTOPELEMENTIMPL_H */
