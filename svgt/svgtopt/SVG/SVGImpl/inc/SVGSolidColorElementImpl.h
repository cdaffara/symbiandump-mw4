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


#ifndef SVGSOLIDCOLORELEMENTIMPL_H
#define SVGSOLIDCOLORELEMENTIMPL_H

//  INCLUDES
#include "SVGElementImpl.h"

// FORWARD DECLARATIONS
class   CSvgDocumentImpl;

/**
 * The solidColor element is a paint server that provides a single color with opacity. 
 * It can be referenced like the other paint servers 
 * 
 * The class does not export any function to other non SVG compoents.
 *
 *  @lib SVGEngine.lib
 *  @since 1.2
 */
class CSvgSolidColorElementImpl : public CSvgElementImpl
    {
    public:
    
        // Constructor/deconstructor
      
        /**
        * Two-phased constructor.
        */
        static CSvgSolidColorElementImpl* NewL( const TUint8 aElemID,
                                          CSvgDocumentImpl* aDoc );



        /**
        * Two-phased constructor.
        */
        static CSvgSolidColorElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

        /**
        * C++ default constructor.
        */
        CSvgSolidColorElementImpl( CSvgDocumentImpl* aDoc );


        /**
         * SetAttributeL: Attribute can only be solid-color which is the same as
         * the "fill" attribute
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param aName Name of the attribute
         *        aValue conent of the attribute
         * @return KErrNone if no error.
         */

         TInt   SetAttributeL( const TDesC& aName,
                               const TDesC& aValue );

        /**
         * SetAttributeFloatL: Attribute can only be solid-opacity which is the same as
         * the "fill-opacity" attribute
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param aName Name of the attribute
         *        aValue conent of the attribute
         * @return KErrNone if no error.
         */
        TInt     SetAttributeFloatL( const TInt aNameId,
                                      const TFloatFixPt aValue );
                                      
        TInt GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue);
                                      
        /**
         * ApplyOpacitytoElement: Applies solid-opacity to appropriate element
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param aElement element pointer
         *        
         * @return void.
         */
       void     ApplyOpacitytoElement(CSvgElementImpl* aElement);

        /**
        * Destructor.
        */
        virtual    ~CSvgSolidColorElementImpl();

		void		Print( TBool aIsEncodeOn );
		
		TFloatFixPt iSolidOpacity;
		
    private:
        /**
         * Symbian 2nd phase constructor
         *
         * Mainly initialize the element and subscribe events to be listen.
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param
         * @return
         */
        void ConstructL(const TUint8 aElemID);

    };

#endif /* SVGSOLIDCOLORELEMENTIMPL_H */