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


#ifndef _INC_CSVGSETELEMENTIMPL_
#define _INC_CSVGSETELEMENTIMPL_

#include "SVGAnimationBase.h"




class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgSetElementImpl : public CSvgAnimationBase
    {
    public:
		TBool DoAnimProcL(MSvgEvent* aEvent);
        // Constructor/deconstructor

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgSetElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgSetElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

		 void		 					ReInitializeAnimation();

		 void					Reset(MSvgEvent* aEvent);
		 void DeactivateAnimation();

	private:
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ConstructL(  const TUint8 aElemID  );


	public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                     ~CSvgSetElementImpl();

    private:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgSetElementImpl( CSvgDocumentImpl* aDoc );

    public:

			/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt SetAttributeIntL( const TInt aNameId, const TInt32 aValue);
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt GetAttributeIntL( const TInt aNameId, TInt32& aValue );
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt SetAttributeFloatL(const TInt aNameId, TFloatFixPt aValue );
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue );
        // From MXmlElement API

        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       ReceiveEventL( MSvgEvent* aEvent );
        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        ResetReferenceElementL();
		CGfxGeneralPath* GetPathAttribute(TInt aAttributeId);
		void SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle);


       void 						InitAnimationL();
       void 							SetToOriginalL();

	   void Print( TBool aIsEncodeOn );
	   
    private:

        
        
        // this tells that the style attribute is inherited one.
		// this will be used by the SetMediaTime to repoint it to the parent.

		TBool                           iIsInheritedAttribute;
		
		

    private:
        // From CSvgAnimationBase

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

        virtual TBool                   AnimProcL( MSvgTimerEvent* aEvent );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        ResetAnimationL();
    };


#endif /* _INC_CSvgSetElementImpl_ */
