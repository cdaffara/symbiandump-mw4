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


#ifndef _INC_CSVGFOREIGNOBJECTELEMENTIMPL_
#define _INC_CSVGFOREIGNOBJECTELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"

class   CSvgDocumentImpl;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgForeignObjectElementImpl : public CSvgElementImpl
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
        static CSvgForeignObjectElementImpl*NewL( const TUint8 aElemID,
                                                  CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgForeignObjectElementImpl*NewLC( const TUint8 aElemID,
                                                   CSvgDocumentImpl* aDoc );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                             ~CSvgForeignObjectElementImpl();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        CSvgElementImpl*                    SvgElementImpl();

        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                                SetAttributeL( const TDesC& aName,
                                                           const TDesC& aValue );
        // From CSvgElementImpl
		TInt SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue );
                                              
		TInt SetAttributeDesL( const TInt aNameId,
                                            const TDesC& aValue );

		void SetCdataL( const TDesC& aCdata );
		
		TFloatFixPt	X();
		TFloatFixPt	Y();
		TFloatFixPt	Width();
		TFloatFixPt	Height();
		
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*        CloneL(MXmlElement* aParentElement);


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                               DrawL( CGfx2dGc* aGc,
                                                   CSvgElementImpl* aElement );
    	
    	/**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable() { return ETrue; }

        /**
         * Notified when the mouse pointer enters a visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY );

        /**
         * Notified when the mouse pointer exits a visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt aX, TInt aY );

        /**
         * Notified when the mouse pointer exits a visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt aX, TInt aY );

        /**
         * Notified when the mouse pointer is pressed down on visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MousePressed( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY );

        /**
         * Notified when the mouse pointer is released on on visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY );
                                    
    private:

		HBufC*                      iCdata;
		
		TFloatFixPt						iX;
		TFloatFixPt						iY;
		TFloatFixPt						iWidth;
		TFloatFixPt						iHeight;
		
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                                ConstructL( const TUint8 aElemID );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                            CSvgForeignObjectElementImpl( CSvgDocumentImpl* aDoc );
    
    	void Print( TBool aIsEncodeOn );
    };

#endif
