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


#ifndef _INC_CSVGANIMATIONBASE_
#define _INC_CSVGANIMATIONBASE_

#include <e32math.h>
#include "SVGElementImpl.h"
#include "SVGEventReceiver.h"
#include "SVGEvent.h"
#include "SVGSchemaData.h"
#include "SVGAnimTimeController.h"

#include "GfxFloatFixPt.h"
#include "GfxRectangle2D.h"
#include "GfxGeneralPath.h"

#include "SVGSvgElementImpl.h"



const TUint32       KRmask                  = 0xff0000;
const TUint32       KGmask                  = 0x00ff00;
const TUint32       KBmask                  = 0x0000ff;

const TInt          KBeginElementOffsetRatio = 66;

//
typedef TUint8      TAnimStatus;
const TAnimStatus   KAnimNotActive          = 0;
const TAnimStatus   KAnimActive             = 1;
const TAnimStatus   KAnimEnd                = 2;
const TAnimStatus   KAnimFinished           = 3;

typedef TUint8      TAnimFill;
const TAnimFill     KAnimFillRemove         = 0;
const TAnimFill     KAnimFillFreeze         = 1;

typedef TUint8      TAccumulate;
const TAccumulate   KAccumNone              = 0;
const TAccumulate   KAccumSum               = 1;

typedef TUint8      TAdditive;
const TAdditive     KAdditiveReplace        = 0;
const TAdditive     KAdditiveSum            = 1;
const TAdditive     KAdditivePass           = 2;

typedef TUint8      TRestartMode;
const TAccumulate   KRestartAlways          = 0;
const TAccumulate   KRestartWhenNotActive   = 1;
const TAccumulate   KRestartNever           = 2;


const TUint16       KRepeatCountMax         = 0xffff;

class               CSvgDocumentImpl;
class               MXmlElementOpt;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgAnimationBase : public CSvgElementImpl
    {
    public:
        friend class                    CSvgAnimTimeController;
	// function used by decoder to set the begin time list.
    void SaveBeginTimeToList(TInt aTimeInMilliseconds);
   
    void				AddEndTime( TInt32 aEndTime );
	void                SetBeginTimeList(RArray<TInt32>*& aList);

	void				SetBeginTime(TInt32 aTime);

	void				SetAbsoluteBeginTime(TInt32 aTime);

	void				SetDurationTime(TInt32 aTime);

	TBool   GetFromFlag();
	TUint16 GetAttributeId();

	void	SetFromFlag();
	void    SetFromInt(TInt32 aValue);
	void    SetFromFloat(TFloatFixPt aValue);
	void    SetFromPathL(CGfxGeneralPath*& aPath);
	void    SetFromViewBox(TGfxRectangle2D aValue);

	void	SetToFlag();
	void    SetToInt(TInt32 aValue);
	void    SetToFloat(TFloatFixPt aValue);
	void    SetToPath(CGfxGeneralPath*& aPath);
	void    SetToViewBox(TGfxRectangle2D aValue);

	void	SetByFlag();

	CSvgElementImpl*&		GetTargetElem();

	void    SetAnimFreeze();

   	void	SetValuesFlag();
	void    SetIntValuesArray( RArray<TInt32>*& aArray);
	void    SetFloatValuesArray( RArray<TFloatFixPt>*& aArray);
	void    SetPathValuesArray( RPointerArray<CGfxGeneralPath>*& aArray);
    void    SetViewBoxValuesArray( RArray<TGfxRectangle2D>*& aArray);
	void    ResetFloatValuesArray();

	void    SetRepeatCount(TReal32 aCount);
	void    SetRepeatDuration(TUint32 aDur);

	void   SetEndTime(TInt32 aTime);

	void   SetRestartMode(TRestartMode aMode);
    void   SetAccumulate(TAccumulate aValue);
	void   SetAdditive(TAdditive aValue);

	void  SetCalcMode(TAnimCalcMode aMode);

	void   SetKeyTimeArray(RArray<CSvgAnimTimeController::TKeyTime>*& aArray);
	void   SetAnimTimeArray(RArray<TUint32>*& aArray);
    void   SetKeyTimeFlag();
	CGfxGeneralPath*  GetPathAttribute(TInt aAttributeId);
	void SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle);

	/*Function added for Forward reference support*/
	TInt	SetRefElemById(const TDesC& aName);
    void   CheckBeginTime();
	// functions added for JSR-226



/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
	TInt                GetAttributeFloat( const TInt aNameId,
                                                       TFloatFixPt& aValue );

	/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
	TInt                 SetAttributeFloatL( const TInt aNameId,
                                                        const TFloatFixPt aValue );

	/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
	TInt                SetAttributeIntL( const TInt aNameId,
                                                      const TInt32 aValue );
	/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
	TInt                GetAttributeIntL( const TInt aNameId,
                                                     TInt32& aValue );


// End of functions for Decoder


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                         ~CSvgAnimationBase();

         class TEventListItem
            {
            public:

                TInt32                                      iTime;
                TInt32                                      iOffset;
                TReal32                        				iRepeatValue;
                CSvgElementImpl*                            iTargetElement;
                HBufC*										iReferenceElemeId;
                TUint16                                     iAccessKeyCode;
				TUint16										iDummy; // Series 60 3.0 Byte Alignment
                TSvgEvent                                   iEvent;

            };
           CArrayFixFlat<TEventListItem>*           iEventList;
           CArrayFixFlat<TEventListItem>*			iEndTimeList;

		// for decoder
	   void				SetEventList(CArrayFixFlat<TEventListItem>*& aEventList);

	   void				SetEndTimeList(CArrayFixFlat<TEventListItem>*& aTimeList);
	   TBool 			IsSelfDependentForBegin();


		void			SetFromValuesL();
        // From SVG DOM / MSvgAnimationElement

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TUint32                         SimpleDuration();
        TUint32                         CompleteDuration();
        // From SVG DOM / MElementTimeControl

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           BeginElementL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           BeginElementAtL( TFloatFixPt aOffset );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           EndElement();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           EndElementAt( TFloatFixPt aOffset );


        // From MXmlElement API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                    SetAttributeL( const TDesC& aName,
                                                       const TDesC& aValue );
        // From CSvgElementImpl


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
        virtual void		 					ReInitializeAnimation();

		     /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            GetAttrValueL();

		  /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                      SetTarget( CSvgElementImpl* );

		void                      SetAttributeId(const TUint16 aAtrId);

		// this is made a leaving function
        void					  ResetL(MSvgEvent* aEvent,CSvgAnimationBase* aElement);

		void                      SetAttributeType(const TUint8 aAtrType);
		virtual void              SetOriginalValues_DOMReuse();

		TAnimStatus				  GetAnimStatus();
		void DeactivateAnimation(CSvgAnimationBase* aElement);
		void CheckForEndTimesAndFreezeL(CSvgAnimationBase* aElement);
		virtual  void SetAccumulateValuesForSetMediaTime();

        TInt32 GetAbsoluteBeginTime();
        TInt32 GetEndTime();
        void ResetAttrValueL();
        
          /**
         * Need method description
         *returns if the animation element has fill value freeze or not
         * @since 3.2
         * @param none
         * @return return True if animation element has fill value as freeze
         */
		TBool IsFillFreeze();
		
		void ResetTimes();
		

    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CSvgAnimationBase( CSvgDocumentImpl* aDoc );


		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		virtual void                    ConstructL(  const TUint8 aElemID );



    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static TFloatFixPt                   BlendFloat( TInt32 aAlpha,
                                                    TFloatFixPt aV1,
                                                    TFloatFixPt aV2 );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static TInt32                   BlendInt( TInt32 aAlpha,
                                                  TInt32 aV1,
                                                  TInt32 aV2 );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static TUint32                  BlendColor( TInt32 aAlpha,
                                                    TUint32 aV1,
                                                    TUint32 aV2 );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static TUint32                  AddColor( TUint32 aV1, TUint32 aV2 );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static TUint32                  SubtractColor( TUint32 aV1,
                                                       TUint32 aV2 );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           ReceiveEventProcL( MSvgEvent* aEvent,
                                                          CSvgAnimationBase* aAnimElement );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool                   AnimProcL( MSvgTimerEvent* aEvent ) = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    SetToOriginalL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    SetFillValueL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    SetToInitialValueL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    SetToEndValueL();

	    /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    InitAnimationL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    ResetAnimationL() = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    SetAccumulateValues();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           IsElementAttrs( const TDesC& aAttrName );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           IsPresentationAttrs( const TDesC& aAttrName );

				public:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetBeginByEventL( CSvgAnimationBase* aAnimElement,
                                                         TInt32 aOffset, TBool aAddTime = EFalse);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetEndByEvent( CSvgAnimationBase* aAnimElement,
                                                       TInt32 aOffset );
		
		void		 StoreRepeatId( const TDesC& aValue, TBool RepeatWasInBeginAtt );
		
        void        NotifyAnimationsRepeatingOnThisOneL( const TDesC* aId );
		
				protected:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt32                          GetNumberOfEvents(const TDesC& aValue);

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt32                          Duration();

        void CopyL(CSvgAnimationBase* aAnimationBase);


        // to parse a viewBox attribute value.
		TBool        ParseViewBoxValue(const TDesC& aValue, TGfxRectangle2D& aRect);
		
    private: 
        /**
         * This function computes the distance between colours for the purpose 
         * of animateColor element. Colours are treated as a 3D 
         * point with r, g, b acting as axes. It modifies the iValuesFloat 
         * array and each index contains the cummulative length(from start 
         * colour) of the segment with same index
         *
         * @since s60 v3.1
         * @param none.
         * @return none.
         */
        void FindColorDistanceL(); 

	public:
		TAnimStatus                     iAnimStatus; // before start, during animation, or end

		HBufC*                      iRepeatId;
		TBool						iRepeatInBegin;
		
    protected:

		// basic animation attributes
        CSvgAnimTimeController*         iAnimTime;
        CSvgElementImpl*                iTargetElement;    // default is parent element

        CGfxGeneralPath*                iEndPath;
        RArray<TFloatFixPt>*                 iValuesFloat;
        CGfxGeneralPath*                iOrgPath;
        CGfxGeneralPath*                iFromPath;
        CGfxGeneralPath*                iToPath;
        CGfxGeneralPath*                iOrgFromPath;
        RPointerArray<CGfxGeneralPath>* iValuesPath;
        RArray<TInt32>*                 iValuesInt;

		TInt32                          iEndOffset;
        TReal32                         iRepeatCount;

         TReal32                          iRepeatEndValue;

        TInt32                          iAbsoluteBeginTime;

        // properties that will be modified over time
        TInt32                          iPropInt;
        TFloatFixPt                          iPropFloat;

         // origianl value and animation value

        TInt32                          iAttributeFlag;
        TInt32                          iEndInt;

        TInt32                          iOrgInt;
        TInt32                          iFromInt;
        TInt32                          iToInt;
        TInt32                          iOrgFromInt;

		// for DOM Reuse
        TBool                           iIsBeginSet;

        TFloatFixPt                          iOrgFloat;
        TFloatFixPt                          iFromFloat;
        TFloatFixPt                          iToFloat;
        TFloatFixPt                          iOrgFromFloat;
        TFloatFixPt                          iEndFloat;

      //viewBox Animation

		TGfxRectangle2D                iOrgViewBox;
        TGfxRectangle2D                iFromViewBox;
        TGfxRectangle2D                iToViewBox;
        TGfxRectangle2D                iOrgFromViewBox;
		RArray<TGfxRectangle2D>*                iValuesViewBox;
		TGfxRectangle2D                iEndViewBox;
    ////////

        TInt                            iBeginTimeIndex;
       


    //    TUint16                         iEndAccessKeyCode;

        TUint16                         iCurrentRepeatCount;
         // target attribute
        TUint16                         iAttrId;
        TUint16                         iDataType;

     /*   CSvgElementImpl*                iEndSyncElementId;*/



        TUint8                           iInitDone;
        TUint8                           iTargetSet;
        TUint8                           iNoFrom;
        TUint8                           iHaveTo;
        TUint8                           iHaveBy;
		TUint8                           iHaveValues;



    /*    TSvgEvent                       iEndReferenceEvent;*/


        TAnimFill                       iFill;
        TUint8                           iHadBegun;
        TRestartMode                    iAnimRestart;
        TAccumulate                     iAccumulate;
        TAdditive                       iAdditive;
        TAdditive                       iAdditiveOrg;





		//deleted by memory manager so dont do it yourself
		CPaintCssValueImpl*				iFromPaint;
		CPaintCssValueImpl*				iToPaint;

        TUint8                           iOverrideTime;


        TUint8                           iRepeatCountSet;

        TBool                            iDoFreeze;

		//DOM Reusage

		TBool                            iCheckFirstAnim;



		TBool							iKeyTimesPresent;

		// these are kept only up to the loading time. after that this memory is freed.


		/*HBufC*							iEndReferenceElemeId;*/
		TBool 							iIsUserSeek;
		TInt                         iEndTimeIndex;
	//	TBool 							iHasEnded;

		TBool                           iIsEndEventSent;
		TInt                            iNegativeBeginTime; // Himanshu: stores negative begin value;
		static const TInt KIndefiniteAnimationDuration = 2147483647;
    };


#endif /* _INC_CSvgAnimationBase_ */
