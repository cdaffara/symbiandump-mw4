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


#ifndef __INC_CSVGDISCARDELEMENTIMPL__
#define __INC_CSVGDISCARDELEMENTIMPL__

//  INCLUDES
#include "SVGElementImpl.h"

// FORWARD DECLARATIONS
class CSvgDocumentImpl;
class CSvgEngineImpl;


/**
 * The Discard element allows content authors to specifiy the time at which particular 
 * elements are to be discarded, therefore reducing the CPU and memory resources. 
 * 
 * The class does not export any function to other non SVG compoents.
 *
 *  @lib SVGEngine.lib
 *  @since 1.2
 */
class CSvgDiscardElementImpl : public CSvgElementImpl
    {
    public:   // Constructor/deconstructor

        /**
        * Two-phased constructor.
        */
        static CSvgDiscardElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

        /**
        * Two-phased constructor.
        */
        static CSvgDiscardElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

        /**
        * Destructor.
        */
        virtual ~CSvgDiscardElementImpl();

    public: // From CSvgElementImpl


		/**
		 * ReceiveEventL: This function will be called whenever the registered
		 * event was happened.
		 *
		 * The call flow is like:
		 * 
		 * _______
		 * | UI  |------------------
		 * -------                 |
		 *                         v
		 * ________               ________               ______________
		 * | Timer|-------------> |Engine|-------------->|EventHandler|--------
		 * -------- ProcessEvenL  -------- ProcessEventL -------------- ReceiveEventL
		 *
		 *
		 *     _____________                 ________________
		 * --->|ElementImpl| --------------> |DiscardElement|
		 *     ------------- ReceiveEventL   ----------------
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param aEvent 
         * @return ETrue if redraw is needed.
		 * 
  	     */
        TBool ReceiveEventL( MSvgEvent* aEvent );
        
        /**
         * Clone the discard element
         *
         * @since 1.0
         * @param MXmlElement* new parent element
         * @return MXmlElement*
         */
        MXmlElement* CloneL( MXmlElement* aParentElement);
        

        /**
         * SetAttributeL: Attribute can only be xlink:href or begin
         * If it is xlink::href, it refers to the element that is to be discard.
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param aName Name of the attribute
         *        aValue conent of the attribute
         * @return KErrNone if no error.
         */
        TInt SetAttributeL( const TDesC& aName, const TDesC& aValue );

	public:
		// Setter        
        /**
         * Setter function for iTargetId
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TDesC& Descriptor of the target element.
         * @return void
         */
        void SetTargetId(const TDesC& aTargetId);

        /**
         * Setter function for iSyncValueDefined
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TBool flag if Sync type of begine time is defined.
         * @return void
         */
        void SetSyncValueDefined(TBool aSyncValueDefined);

        /**
         * Setter function for iEventValueDefined
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TBool flag if event type of begin time is defined.
         * @return void
         */
        void SetEventValueDefined(TBool aEventValueDefined);

        /**
         * Setter function for iHrefValueDefined
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TBool flag if xlink:href is defined.
         * @return void
         */
        void SetHrefValueDefined(TBool aHrefValueDefined);

        /**
         * Setter function for iBeginSyncElementId
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TDesC& Descriptor of the referenced target element
         * @return void
         */
        void SetBeginSyncElementId(const TDesC& aBeginSyncElementId);

        /**
         * Setter function for iAbsoluteBeginTime
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TInt the value of Absolute begin time
         * @return void
         */
        void SetAbsoluteBeginTime(TInt aAbsoluteBeginTime);

        /**
         * Setter function for iRefBeginTime
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TInt the value of reference begin time
         * @return void
         */
        void SetRefBeginTime(TInt aRefBeginTime);

        /**
         * Setter function for iKeyValue
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TInt the value of key code from user
         * @return void
         */
        void SetKeyValue(TInt aKeyValue);

        /**
         * Setter function for iBeginReferenceEvent
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param TSvgEvent the SVG event generated either by Timer
         *                  or UI.
         * @return void
         */
        void SetBeginReferenceEvent(TSvgEvent aBeginReferenceEvent);

		// Getter

        /**
         * Getter function for iTargetId
		 * 
		 * Target element to be removed. The id may be identified at
		 * parsing stage after SetAttributeL. But if the xlink:href
		 * does not specified, the value of TargetId could be NULL and 
		 * Target element will be parent. Or, if the specified target id
		 * is not valid, then the discard element will remove itself.
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TDesC& TargetId the target element's id
         */
        const TDesC& TargetId();

        /**
         * Getter function for iBeginSyncElementId
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TDesC& iBeginSyncElementId the reference element's id
         */
        const TDesC& BeginSyncElementId();

        /**
         * Getter function for iAbsoluteBeginTime
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TInt iAbsoluteBeginTime
         */
        TInt AbsoluteBeginTime();

        /**
         * Getter function for iSyncValueDefined
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TBool iSyncValueDefined
         */         
        TBool IsSyncValueDefined();
        
        /**
         * Getter function for iEventValueDefined
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TBool iEventValueDefined
         */         
        TBool IsEventValueDefined();

        /**
         * Getter function for iHrefValueDefined
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TBool iHrefValueDefined
         */         
        TBool IsHrefValueDefined();

        /**
         * Getter function for iRefBeginTime
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TInt iRefBeginTime
         */
        TInt RefBeginTime();

        /**
         * Getter function for iKeyValue
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TInt iKeyValue
         */
        TInt KeyValue();

        /**
         * Getter function for iBeginReferenceEvent
		 * 
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return TSvgEvent iBeginReferenceEvent
         */
        TSvgEvent BeginReferenceEvent();

		void Print( TBool aIsEncodeOn );
		
    protected: // From CSvgElementImpl

        /**
         * Copy content to an element
         *
         * @since 1.0
         * @param CSvgDiscardElementImpl new element
         * @return none
         */
        void CopyL( CSvgDiscardElementImpl* aDestElement );

	private:

        /**
        * C++ default constructor.
        */
        CSvgDiscardElementImpl( CSvgDocumentImpl* aDoc );

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

        /**
         * Find the target element to be remmoved.
         *
         * If no target was found, the discard element is ignored and 
         * will be removed later to save memory.
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return ETrue if target is found.
         */
		TBool FindTargetElementL();		

        /**
         * Check to see if the target element is animation element
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param CSvgElementImpl* Target element
         * @return ETrue if the target element is animation element.
         */
		//TBool IsAnimationElement(CSvgElementImpl* aElement);

        /**
         * Check to see if the target element contains any animation element
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param CSvgElementImpl* Target element
         * @return ETrue if the target element contains at least one 
         *         animation element.
         */
		//TBool HasAnimationElements(CSvgElementImpl* aElement);

        /**
         * Remove target element
         *
         * @since Series 60 3.1 SVGTiny 1.2
         * @param none
         * @return void
         */
		void RemoveTargetElement();



    private:
        
        // Target element to be removed.
        CSvgElementImpl* iTargetElement;
        
        // Time to be removed
        TInt32 iAbsoluteBeginTime;
        
        // Flag if the element needs to be removed.
        TBool iRemoveMyself;
        
		// Used in xlink:xhref to identify element to be removed.
		HBufC*  iTargetId;
		
		// Flag to identify xlink:xhref is used
		TBool iHrefValueDefined;
		
		// Used in Syncbased begin time to identify element to be removed.
		TBuf<30>   iBeginSyncElementId;
		
		// Used in Syncbased begin time to identify the event 
        TSvgEvent  iBeginReferenceEvent;

        // Flag to identity Sync-base begin time is used
		TBool iSyncValueDefined;

        // Flag to identity event-based begin time is used
		TBool iEventValueDefined;
		
		// Time to be used in Sync-based and event-based begin time
		TInt32 iRefBeginTime;
		
		// The element that is been referenced in sync-based and event-based 
		CSvgElementImpl* iRefTargetElement;
		
		// Flag to identify referenced begin time is used
		TBool iRefBeginTimeSet;
		
		// Accesskey's scancode
		TInt iKeyValue;
    };

#endif // __INC_CSVGDISCARDELEMENTIMPL__
