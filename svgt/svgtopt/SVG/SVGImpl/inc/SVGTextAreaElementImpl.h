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


#ifndef _INC_CSVGTEXTAREAELEMENTIMPL_
#define _INC_CSVGTEXTAREAELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxFloatFixPt.h"
#include "GfxRoundRectangle2D.h"
#include "SVGListener.h"

class   CSvgDocumentImpl;
class CSvgRectElementImpl;
class CSvgTextElementImpl;
class CSvgLineElementImpl;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgTextAreaElementImpl : public CSvgElementImpl,
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
        static CSvgTextAreaElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgTextAreaElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual     ~CSvgTextAreaElementImpl();

        // From SVG DOM
                 /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*    CloneL(MXmlElement* aParentElement);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt  X();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt  Y();
        TInt GetLength();
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt  Width();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt  Height();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt  Rx();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt  Ry();

        // SVG Implementation
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt    GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt    SetAttributeFloatL( const TInt aNameId, const TFloatFixPt aValue );

        TInt    SetAttributeDesL( const TInt aNameId, const TDesC& aValue );

        TInt    GetAttributeDes( const TInt aNameId, TPtrC16& aValue );

        // From MXmlElement API
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt   SetAttributeL( const TDesC& aName, const TDesC& aValue );
		
		void SetEditable( const TDesC& aValue );
		
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool   DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement );


        TBool   DrawTextElements( CGfx2dGc* aGc, CSvgElementImpl* aElement );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void    GetBBox( TGfxRectangle2D& aBbox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

         /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
       void SetTextAreaDimensions(TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight);

       void SetTextL( const TDesC& aText ); // This is temporary API until CDATA is implemented

       void GetText( TDes& allText );

	   TBool IsEditable( );
       
	   void LocalizedArrangeTextL( CGfx2dGc* aGc );
	   
       void RemoveText( const TDesC& aString );

       void InitializeL( );

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
        inline TInt FileOffset() const
            {
            return iOffset;
            }
            
        inline void SetFileOffset(TInt aOffset)
            {
            if( !FileOffset() )
                iOffset = aOffset;
            };
            
        inline void SetOrgLength(TInt aOrgLength)
            {
            if(! GetOrgLength())
            iOrgLength = aOrgLength;
            };
            
        inline TInt GetOrgLength()            
            {
            return iOrgLength;
            }
                                    
        void Print( TBool aIsEncodeOn );
        
    private:

                /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void ConstructL(  const TUint8 aElemID  );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
         CSvgTextAreaElementImpl( CSvgDocumentImpl* aDoc );

    private:
        CSvgRectElementImpl*        iInnerRectangle;
        CSvgRectElementImpl*        iBorderRectangle;

        HBufC*                      iAllText;
        RPointerArray<CSvgTextElementImpl>* iTextElementsArray;

		TBool 						iEditable;
        TBool                       iInitialized;
        TBool                       iNeedTextRearrange;

        TInt                        iCursorPosition;
        CSvgLineElementImpl*        iCursorElement;
		TGfxRectangle2D             iLastBBox;
        //TBool                     iScrollBar;
        //TInt                      iScrollLocation;
public:	
		TInt                        iOffset;
		TInt                        iOrgLength;	

    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void CopyL( CSvgTextAreaElementImpl* aDestElement );

    };

#endif /* _INC_CSVGLINEELEMENT_3C61A6080051_INCLUDED */
