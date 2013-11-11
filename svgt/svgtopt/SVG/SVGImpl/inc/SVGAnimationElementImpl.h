/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __INC_CSVGANIMATIONELEMENTIMPL__
#define __INC_CSVGANIMATIONELEMENTIMPL__

//  INCLUDES
#include "SVGAnimationBase.h"
#include "SVGElementImpl.h"

// FORWARD DECLARATIONS
class CSvgDocumentImpl;


/**
 * The animation elements specifies an SVG document or an SVG document fragment
 * providing synchronized animated vector graphics.
 *
 * This element also supports all Run-time synchronization attributes and SVG
 * Timing attributes.
 *
 * @lib SVGEngine.lib
 * @since Series 60 3.1
 */
class CSvgAnimationElementImpl : public CSvgAnimationBase
    {
    public:   // Constructor/deconstructor

        /**
        * Two-phased constructor.
        */
        static CSvgAnimationElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

        /**
        * Two-phased constructor.
        */
        static CSvgAnimationElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

        /**
        * Destructor.
        */
        virtual ~CSvgAnimationElementImpl();

    public: // From CSvgAnimationBase

        /**
         * Execute animation of its children when instructed.
         *
         * @since Series 60 3.1
         * @param MSvgTimerEvent* aEvent
         * @return TBool
         */

        virtual TBool AnimProcL( MSvgTimerEvent* aEvent );

        /**
         * Cleanup animation work to its children
         *
         * @since Series 60 3.1
         * @param none
         * @return none
         */
        void ResetAnimationL();


    public:  // From MXmlElement API

		/**
         * SetAttributeL: Called by Contenthandler or decoder to set attributes that
         * are floating type.
         *
         * @since Series 60 3.1
         * @param TDesC& id of the attribute
         * @param TDesC& aValue value of the attribute
         * @return TInt error code
         */

		TInt SetAttributeFloatL( const TInt aNameId, const TFloatFixPt aValue );


        /**
         * SetAttributeFloatL: Called by Contenthandler or decoder to set attributes
         * that are string type.
         *
         * @since Series 60 3.1
         * @param TDesC& id of the attribute
         * @param TDesC& aValue value of the attribute
         * @return TInt error code
         */
        TInt SetAttributeL( const TDesC& aName, const TDesC& aValue );


        /**
         * GetAttributeFloat: returns value of the floating attribute
         * that are string type.
         *
         * @since Series 60 3.1
         * @param TDesC& id of the attribute
         * @param TDesC& aValue value of the attribute
         * @return TInt error code
         */
		TInt GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue );

    public:  // From MEventReceiver

        /**
         * ReceiveEventL: This function will be called whenever the registered
		 * event was happened.
         *
         * @since Series 60 3.1
         * @param MSvgEvent* aEvent
         * @return TBool ETrue if redraw is needed
         */
        TBool ReceiveEventL( MSvgEvent* aEvent );


		/**
         * CloneL: Clone the animation element
         *
         * @since Series 60 3.1
         * @param MXmlElement* new parent element
         * @return MXmlElement*
         */
		MXmlElement* CloneL(MXmlElement* aParentElement);

    public: // New functions

        /**
         * Getter function for iPreservRatio
		 *
		 *
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TDesC& TargetId the target element's id
         */
        const TDesC& PreservRatio();


        /**
         * X: Returns the x coordinate of the defined region
         *
         * @since Series 60 3.1
         * @param none
         * @return TFixPt
         */
        TFloatFixPt                      X();


        /**
         * Y: Returns the y coordinate of the defined region
         *
         * @since Series 60 3.1
         * @param none
         * @return TFixPt
         */
        TFloatFixPt                      Y();


        /**
         * Width: Returns the width of the defined region
         *
         * @since Series 60 3.1
         * @param none
         * @return TFixPt
         */
        TFloatFixPt                      Width();


        /**
         * Height: Returns the height of the defined region
         *
         * @since Series 60 3.1
         * @param none
         * @return TFixPt
         */
        TFloatFixPt                      Height();


		/**
         * SetReferenceElementL: Clone the referenced element.
         *
         * @since Series 60 3.1
         * @param none
         * @return none
         */
        void SetReferenceElementL();

		/**
         * SetRefElemById: Sets the referenced id.
         *
         * @since Series 60 3.1
         * @param TDesC& the referenced id
         * @return TInt error code
         */
		TInt SetRefElemById(const TDesC& aName);

		/**
         * SetRecursionVariable: Set the flag to indicate if the element
         * has been cloned or not.
         *
         * @since Series 60 3.1
         * @param TBool flag ETrue if the element has been cloned
         * @return none
         */
		void SetRecursionVariable(TBool aIsRecursion);

		/**
         * RecursionVariable: Returns the flag indicates that if the element
         * has been cloned.
         *
         * @since Series 60 3.1
         * @param none
         * @return TBool
         */
		TBool RecursionVariable();


		/**
         * ResetEventListener: Reset all event listener for it's children
         * and add to it's own event handler
         *
         * @since Series 60 3.1
         * @param none
         * @return void
         */
		void ResetEventListener();


		/**
         * RemoveEventListener: Remove all event listener for it's children.
         *
         * @since Series 60 3.1
         * @param none
         * @return void
         */
		void RemoveEventListener();



		/**
         * FindAllAnimationElements: Find and store all animation elements
         * under the element.
         *
         * @since Series 60 3.1
         * @param none
         * @return void
         */
        void FindAllAnimationElements( CSvgElementImpl* aElement,
                               RPointerArray<CSvgElementImpl>* aList );


        /**
         * Check to see if the target element is animation element
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param CSvgElementImpl* Target element
         * @return ETrue if the target element is animation element.
         */
		TBool IsAnimationElement(CSvgElementImpl* aElement);


        /**
         * Return the id of referenced target
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TDesC& id
         */
		//const TDesC&                 GetUriRefId();

        /**
         * Return the flag if the URI of the reference id is set
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return ETrue if the id of the target element is been set.
         */
		TBool                        IsUriRefSet();

		void						 Print( TBool aIsEncodeOn );
		
    protected:

        /**
         * Copy content to an element
         *
         * @since Series 60 3.1
         * @param CSvgAnimationElementImpl new element
         * @return none
         */
        void CopyL( CSvgAnimationElementImpl* aDestElement );

	private:

        /**
        * C++ default constructor.
        */
        CSvgAnimationElementImpl( CSvgDocumentImpl* aDoc );

		/**
         * Symbian 2nd phase constructor
         *
         * Mainly initialize the element and subscribe events to be listen.
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param
         * @return
         */
        void ConstructL(  const TUint8 aElemID  );


    private: //New Functions


    private:

        // The x-axis coordinate of one corner of the rectangular region into
        // which the animation is placed.
        TFloatFixPt iX;

        // The y-axis coordinate of one corner of the rectangular region into
        // which the animation is placed.
        TFloatFixPt iY;

        // The width of the rectangular region into which the animation is placed.
        TFloatFixPt iWidth;

        // The height of the rectangular region into which the animation is placed.
        TFloatFixPt iHeight;

        // Referenced (target) element
        CSvgElementImpl* iReferencedElement;

        // Flag if recursion is set
		TBool iUseInRecursion;

        // A container that holds animation elements' reference of its children
        RPointerArray<CSvgElementImpl>* iMyAnimationElementList;

        // a separated Event Handler from DocumentImpl
        CSvgEventHandler *iAnimationEventHandler;

        // flag indicates if the event list's sorting has initial state or not.
        TBool iInitSortList;

        // flag indicates if engine has started or not.
        TBool iEngineStart;

        // value of PreserveAspectRatio
        HBufC* iPreservRatio;

    };

#endif // __INC_CSVGANIMATIONELEMENTIMPL__
