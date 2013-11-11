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


#ifndef _INC_CSVGANIMATEMOTIONELEMENTIMPL_
#define _INC_CSVGANIMATEMOTIONELEMENTIMPL_

#include "SVGAnimateTransformElementImpl.h"



#include "GfxAffineTransform.h"
#include "GfxLine2D.h"





const TInt16    KAnimMotionRotateAuto           = -100;
const TInt16    KAnimMotionRotateAutoReverse    = -200;


class           CSvgDocumentImpl;
class           CGfxGeneralPath;
class           CGfxFlatteningPathIterator;



/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgAnimateMotionElementImpl : public CSvgAnimationBase
    {
    public:

        // Constructor/deconstructor

				TBool DoAnimProcL(MSvgEvent* aEvent);
	    void       SetAnimRotate(TInt16 aRotate);
	    void SetAccumulateValuesForSetMediaTime();

		TInt	   SetAttributePathRef( const TInt aNameId,
                                              CGfxGeneralPath*&  aValue  );
		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		static CSvgAnimateMotionElementImpl*NewL(  const TUint8 aElemID,
												  CSvgDocumentImpl* aDoc );


		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		static CSvgAnimateMotionElementImpl*NewLC(  const TUint8 aElemID,
												   CSvgDocumentImpl* aDoc );

		// Dom Reuse
		 void		 					ReInitializeAnimation();

		 void                          SetOriginalValues_DOMReuse();
		 CGfxGeneralPath* GetPathAttribute(TInt aAttributeId);
		 void SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle);

		private:


			/**
			 * Need method description
			 *
			 * @since 1.0
			 * @param
			 * @return
			 */
			void                                ConstructL(  const TUint8 aElemID );



	public:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                             ~CSvgAnimateMotionElementImpl();

    private:


              /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                            CSvgAnimateMotionElementImpl( CSvgDocumentImpl* aDoc );


    public:

		void								Reset(MSvgEvent* aEvent);
		void DeactivateAnimation();
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


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                                SetAttributeL( const TDesC& aName,
                                                           const TDesC& aValue );

        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                               ReceiveEventL( MSvgEvent* aEvent );
        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                                ResetReferenceElementL();

        // From CSvgAnimationBaseImpl

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                        GetAttributePath( const TInt aNameId,
                                                              CGfxGeneralPath*& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                        SetAttributePathL( const TInt aNameId,
                                                               CGfxGeneralPath* aValue );

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
		void CopyL(CSvgAnimateMotionElementImpl* aDestElement );
		
		void Print( TBool aIsEncodeOn );

    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                        SetAccumulateValues();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                                ApplyAccumulateValuesL();

        TGfxAffineTransform                 iFillMatrix;

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
        virtual void                        InitAnimationL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                        SetToOriginalL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                        SetFillValueL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                        ResetAnimationL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                        SetToInitialValueL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                        SetToEndValueL();

    private:
        CGfxGeneralPath*                    iMotionPath;
        CGfxFlatteningPathIterator*         iPathIterator;
        TGfxAffineTransform                 iIdentTransform;
        
        TFloatFixPt                              iSubLength;
        TFloatFixPt                              iCurrentPos;
        TFloatFixPt                              iSubPos;
        TGfxLine2D                          iCurrentSeg;
        TInt16                              iAnimMotionRotate;
        TGfxAffineTransform                 iEndMatrix;
        //TBool                               iIsValues;
        TFloatFixPt                              iToFloatX;
        TFloatFixPt                              iToFloatY;
        TFloatFixPt                              iFromFloatX;
        TFloatFixPt                              iFromFloatY;
        TFloatFixPt                              iOrgFromFloatX;
        TFloatFixPt                              iOrgFromFloatY;
		// for DOM Reuse
		TFloatFixPt                              iOrgToFloatX;
        TFloatFixPt                              iOrgToFloatY;
        // Used to remember that Path values have been set
        TBool                               iPathSeen;
	public:
		TBool                               iIsMotionPath;


		TReal32                            iPrevRotate; 
    };


#endif /* _INC_CSvgAnimateMotionElementImpl_ */
