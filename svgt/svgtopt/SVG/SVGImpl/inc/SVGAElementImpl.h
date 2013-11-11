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


#ifndef _INC_CSVGAELEMENTIMPL_
#define _INC_CSVGAELEMENTIMPL_

//
#include "GfxFloatFixPt.h"
#include "SVGElementImpl.h"
#include "SVGListener.h"

class   CSvgDocumentImpl;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgAElementImpl : public CSvgElementImpl,
						 public MSvgMouseListener
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
		static CSvgAElementImpl*NewL( const TUint8 aElemID,
									  CSvgDocumentImpl* aDoc );


		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		static CSvgAElementImpl*NewLC(  const TUint8 aElemID,
									   CSvgDocumentImpl* aDoc );




        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                 ~CSvgAElementImpl();

    private:


		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		void                    ConstructL(  const TUint8 aElemID );



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                CSvgAElementImpl( CSvgDocumentImpl* aDoc );

    public:

        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                    SetAttributeL( const TDesC& aName,
                                               const TDesC& aValue );
        
        TBool 							SetTargetL( const TDesC& aName, const TDesC& aValue );
        
        const TDesC&        Target();
                                            
		void					GetBBox( TGfxRectangle2D& aBbox );
		void 					GetUnscaledBBox( TGfxRectangle2D& aBbox );
		
        // From CSvgElementImpl


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*        CloneL( MXmlElement* aParentElement);


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                   DrawL( CGfx2dGc* aGc,
                                       CSvgElementImpl* aElement );

        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                   ReceiveEventL( MSvgEvent* aEvent );
		void Reset(MSvgEvent* aEvent);
		TInt SetAttributeDesL( const TInt aNameId,
                                             const TDesC& aValue );
		TInt GetAttributeDes( const TInt aNameId, TPtrC16& aValue );
                                                     
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

        /**
         * Notified when the mouse pointer is released on on visible svg element.
         *
         * @since 1.0
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        TBool MouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                          TInt aX, TInt aY );

		void	Print( TBool aIsEncodeOn );
		
    private:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                    AddEventReceiverSubtreeL( CSvgElementImpl* aElement );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                   IsChild( CSvgElementImpl* aSubtree,
                                         CSvgElementImpl* aElement );
    private:
        TBool                   iInitDone;
        HBufC*									iTarget;
    };

#endif
