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


#ifndef _INC_CSVGANIMATEELEMENTIMPL_
#define _INC_CSVGANIMATEELEMENTIMPL_

#include "SVGAnimationBase.h"




class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgAnimateElementImpl : public CSvgAnimationBase
    {
    public:

        // Constructor/deconstructor

		TBool                           iIsInheritedAttribute;
		
		void InitAnimationL();
		
		void Reset(MSvgEvent* aEvent);
		void DeactivateAnimation();
		void SetToOriginalL();

    // for calculation of interpolated values for viewBox animation.
    void    DoViewBoxAnimProcL(MSvgTimerEvent*  aEvent); 
			TBool DoAnimProcL(MSvgEvent* aEvent);

		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		static CSvgAnimateElementImpl*  NewL(  const TUint8 aElemID,
											  CSvgDocumentImpl* aDoc );


		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		static CSvgAnimateElementImpl*  NewLC(  const TUint8 aElemID,
											   CSvgDocumentImpl* aDoc );




        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                         ~CSvgAnimateElementImpl();

		// Dom Reuse
		 void		 					ReInitializeAnimation();

		 void                         SetOriginalValues_DOMReuse();
		 CGfxGeneralPath*				GetPathAttribute(TInt aAttributeId);
		 void  SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle);

    protected:


		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		void                            ConstructL(  const TUint8 aElemID );



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CSvgAnimateElementImpl( CSvgDocumentImpl* aDoc );


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

        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           ReceiveEventL( MSvgEvent* aEvent );
        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ResetReferenceElementL();
		
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		MXmlElement* CloneL(MXmlElement* aParentElement);

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		void CopyL(CSvgAnimateElementImpl* aDestElement );

	    void Print( TBool aIsEncodeOn );
	    
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
        void                            ResetAnimationL();

        // Utility method to blend between two paths and give a current path

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            BlendPathL( TInt32 aAlpha,
                                                    CGfxGeneralPath* aFromPath,
                                                    CGfxGeneralPath* aToPath,
                                                    CGfxGeneralPath*& aBlendPath );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            AddPathsL( CGfxGeneralPath*& aPathResult,
                                                   CGfxGeneralPath* aPath1 );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            CopyPathsL( CGfxGeneralPath*& aPathResult,
                                                    CGfxGeneralPath* aPath1 );
    };


#endif /* _INC_CSvgAnimateElementImpl_ */
