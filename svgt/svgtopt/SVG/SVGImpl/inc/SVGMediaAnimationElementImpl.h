/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides the functionality for the animation 
                  element as specified in the SVG 1.2 specification
 *
*/

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#ifndef C_SVGMediaAnimationElementImpl_H
#define C_SVGMediaAnimationElementImpl_H

#include "SVGMediaElementBase.h"
#include "SVGTimeContainer.h"
#include "SVGListener.h"

class CSvgDocumentImpl;
class MXmlDocument;
class CSvgFitToViewBoxImpl;
class CSvgErrorImpl;

NONSHARABLE_CLASS( CSvgMediaAnimationElementImpl ): 
        public CSvgMediaElementBase, 
        public MSvgMouseListener,
        public MSvgTimeContainerObserver
    {
    
     public:
        /**
         * Two Phase Constructor
         *
         * since S60 3.2
         * @param  animation element id and document pointer
         * @return Animation element object
         */
        static CSvgMediaAnimationElementImpl* NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );
        /**
         * Two Phase Constructor
         *
         * since S60 3.2
         * @param  animation element id and document pointer
         * @return Animation element object
         */
         
        static CSvgMediaAnimationElementImpl* NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );
        /**
        * Set the Synchronised behaviour for the animation element
        * @since S60 3.2
        * @param aValue "canSlip" | "locked" | "independent" | "default"
        * @return none.
        */
        virtual void SetSyncBehavior( const TDesC& aValue );
        
        /**
        * Set the Synchronised Tolerance for the animation element
        * @since S60 3.2
        * @param aValue Clock-value | "default"
        * @return none.
        */        
        virtual void SetSyncTolerance( const TDesC& aValue );
        
        /**
        * Set the animation element as Synchronised Master
        * @since S60 3.2
        * @param aValue "true" | "false"
        * @return none.
        */        
        virtual void SetSyncMaster( const TDesC& aValue );
                                               

    private:
    
        /**
         * Two Phase Constructor
         *
         * since S60 3.2
         * @param  animation element id and document pointer
         * @return Animation element object
         */
         void                            ConstructL(  const TUint8 aElemID,
                                                    CSvgDocumentImpl* aDoc );
        
        /**
         * Constructor
         *
         * since S60 3.2
         * @param aDoc a document pointer
         * @return none
         */                                                        
        CSvgMediaAnimationElementImpl( CSvgDocumentImpl* aDoc );
    public:
         /**
         * A method to (deep) clone the current element
         *
         * since S60 3.2
         * @param None
         * @return An MXmlElement pointer to the object that is a copy of 
         * the current element
         */
         MXmlElement*        CloneL(MXmlElement* aParentElement);
         
        /**
         * Destructor
         *
         * since S60 3.2
         * @param none
         * @return none
         */
        virtual                         ~CSvgMediaAnimationElementImpl();

        // From MXmlElement API


        /**
         * Set attribute interface for Animation element
         * 
         * since S60 3.2
         * @param aName - A string buffer containing the name of the attribute
         * @param aValue - A string buffer containing the value of the attribute
         * @return An integer containing success/failure for this action
         */
        TInt                            SetAttributeL( const TDesC& aName,
                                                       const TDesC& aValue );

        /**
         * Set attribute interface for Animation element
         *
         * since S60 3.2
         * @param aName - A string buffer containing the name of the attribute
         * @return An integer containing success/failure for this action
         */
        TInt                            SetAttributeDesL( const TInt aNameId,
										        	   const TDesC& aValue );


        /**
         * Returns x-y or width-height attributes for Animation element
         *
         * since S60 3.2
         * @param
         * @return An integer containing success/failure for this action
         */
        TInt                            GetAttributeFloat( const TInt aNameId,
                                                           TFloatFixPt& aValue );

        /**
         * Sets x-y or width-height attributes for Animatione element
         *
         * since S60 3.2
         * @param
         * @return An integer containing success/failure for this action
         */
        TInt                            SetAttributeFloatL( const TInt aNameId,
                                                            TFloatFixPt aValue );


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
         * Resets the animation element,starts from initial position
         *
         * since S60 3.2
         * @param none
         * @return none
         */    
        void ResetAnimationL();
        
        /**
         * Need method description
         *
         * since S60 3.2
         * @param MSvgEvent
         * @return none
         */
        void Reset( MSvgEvent* aEvent );

        /**
         * Calls AnimProcL of animation element
         *
         * since S60 3.2
         * @param
         * @return TBool ETrue if child svg document is there
         */
    	TBool DoAnimProcL(MSvgEvent* aEvent);


        /**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * since S60 3.2
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable(); 
        
        /**
         * 
         * since S60 3.2
         * @param TBool
         * @return none
         */
        void Print( TBool aIsEncodeOn );
        
        /**
         * Draws the child svg in the animation element
         * since S60 3.2
         * @param GraphicsContext and SVGElement
         * @return ETrue if draw is successful else EFalse 
         */
        TBool DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement );
        
        /**
         * Sets the initial CTM on animation element
         * since S60 3.2
         * @param none
         * @return none
         */
        void SetInitialCTM();

        /**
         * Accessor function for obtaining the child SVG document
         * @since S60 3.2
         * @param none.
         * @return CSvgDocumentImpl* Pointer to the document else NULL
         */        
        CSvgDocumentImpl*                GetChildDocument();
        
        /**
         * 
         * since S60 3.2
         * @param align variabel to indicate alignment method, aMeetSlice which 
                                                is for new viewport.
         * @return none
         */
        void SetPreserveAspectRatioL( TSvgPreserveAspectAlignType aAlign,
                                      TSvgMeetOrSliceType aMeetSlice );
        
        /**
         * Sets the width of animation element
         * since S60 3.2
         * @param aValue a Width attribute value from Svg file
         * @return
         */
        void SetWidth(TReal32 aValue);
        
        /**
         * Sets the height of animation element
         * since S60 3.2
         * @param aValue a Height attribute value from Svg file
         * @return none
         */
        void SetHeight(TReal32 aValue);
       
        /**
         * prepares child svg document
         * since S60 3.2
         * @param aFileHandle for child svg
         * @return MSvgError object
         */
        MSvgError* PrepareDomChildSvg( RFile& aFileHandle, TInt& aHandle, TInt aEngine);   
        
        /**
         * prepares child svg document
         * @param aFileHandle for child svg
         * @return MSvgError object
         */
        MSvgError* PrepareDomChildSvg (const TDesC& aFileName,TInt& aHandle, TInt aEngine);  
        
        /**
         * prepares child svg document
         * since S60 3.2
         * @param aFileHandle for child svg
         * @return MSvgError object
         */
        MSvgError* PrepareDomChildSvg(const TDesC8& aByteData, TInt& aHandle, TInt aEngine );  
                                              
    public: // From CSvgMediaElementBase
        /**
        * From MSvgTimedEntityInterface
        * The parent time container provides the timing clock to 
        * the timed entity (audio, video, animation, document) 
        * using this routine.
        * @since Series 3.2
        * @param aTick Current tick information 
        * @return none.
        */
        void ParentTimeContainerTick( TSvgTick aTick );

        /**
        * From MSvgTimedEntityInterface
        * When the timed entity acts as timing master in the time container, 
        * the time container gets the timed entity clock using this method 
        * and feeds to rest of timed entities.
        * @since Series 3.2
        * @param aEntityCurTime Current Entity Time in msecs. 
        * @return none.
        */
        void GetEntityCurrentTime( TUint32& 
            aEntityCurTime );

        /**
        * From MSvgTimedEntityInterface
        * If some other element is resolved as syncMaster in the time container group, 
        * this element can not act as sync master. 
        * @since Series 3.2
        * @param aSyncMasterStatus Indicates whether the element is currrently 
        *                          Sync Master. 
        * @return none.
        */
        void SetCurSyncMasterStatus( TBool aSyncMasterStatus );

        /**
        * From MSvgTimedEntityInterface
        * Check if timed entity can provide timing ticks to rest of time 
        * container elements. This behavior could change dynamically. 
        * For example, if audio clip is over, the audio element can't generate 
        * ticks for others.
        * @since Series 3.2
        * @param none.
        * @return TBool True if can generate timing tick.
        */
        TBool CanGenerateTick();

        /**
        * From MSvgTimedEntityInterface
        * If the timed entity needs to be in sync with the time container and 
        * it has slipped beyond the sync tolerance limit, the method is called to 
        * bring the element in sync with the time container.
        * @since Series 3.2
        * @param aSynctime Time for resync in msecs.
        * @return none.
        */
        void ResyncTimedEntity( TUint32 aSynctime );
            
        /**
        * From CSvgMediaElementBase
        * This would be used for pausing the timed entity while other locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void PauseTimedEntity();

        /**
        * From CSvgMediaElementBase
        * This would be used for resuming the timed entity once all locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void ResumeTimedEntity();    

        /**
        * From CSvgMediaElementBase
        * This would be used for stoping the timed entity once all locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void StopTimedEntity();
        

        /*
        From MSvgTimedEntityInterface
        Would return the type of object
        @param none
        @return type of object
        */
        TSvgObjectType ObjectType();
        
        /*
        From MSvgTimedEntityInterface
        Return CSvgTimeContainer object
        @param none
        @return CSvgTimeContainer
        */
        CSvgTimeContainer* GetChildTimeContainer();
        
    private: // From MSvgTimeContainerObserver
        /**
        * From MSvgTimeContainerObserver
        * Indicates that all locked elements in child container are ready 
        * to render.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void TimeContainerReady();

        /**
        * From MSvgTimeContainerObserver        
        * Indicates that at least one locked element in child container 
        * is not ready to render.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void TimeContainerNotReady();
            
        private:
        // From CSvgAnimationBase

        /**
        * 
        * 
        * 
        * @since S60 3.2
        * @param MSvgTimerEvent.
        * @return TBool.
        */
        virtual TBool AnimProcL( MSvgTimerEvent* aEvent );
        
        /**
        * Creates key times and set animation time according to calcmode
        * Sets first begin time, adds target element to event listener
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        virtual void InitAnimationL();
        
          /**
        * 
        * @since S60 3.2
        * @param MSvgPreserveAspectRatio.
        * @return none.
        */
        void GetPreserveAspectRatio( MSvgPreserveAspectRatio*& aAspectRatio );

        /**
         * X co-ordinate is returned
         *
         * since S60 3.2
         * @param none
         * @return X position of Animation element
         */
                       
        TFloatFixPt                      X();


        /**
         * Y co-ordinate is returned
         *
         * since S60 3.2
         * @param none
         * @return Y position of Animation element
         */
        TFloatFixPt                      Y();


        /**
         * Width is returned
         *
         * since S60 3.2
         * @param none
         * @return Width of Animation element
         */
        TFloatFixPt                      Width();


        /**
         * Height is returned
         *
         * since S60 3.2
         * @param  none
         * @return Height of Animation element
         */
        TFloatFixPt                      Height();
        
        
        /**
         * 
         *
         * since S60 3.2
         * @param  TGfxRectangle2D
         * @return 
         */
        void GetBBox( TGfxRectangle2D& aBbox );                              
        
        /**
         * 
         *
         * since S60 3.2
         * @param  TSvgFourPointRect
         * @return none
         */
        void GetFourPointBBox(TSvgFourPointRect& aFourPointBbox);
        
        
        /**
         * 
         *
         * since S60 3.2
         * @param  none
         * @return 
         */
        void GetUnscaledBBox( TGfxRectangle2D& aBbox );
        
        
        /**
         * Notified when the mouse pointer enters a visible svg element.
         *
         * since S60 3.2
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */
        virtual TBool MouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt aX, TInt aY );
        
         /**
         * Notified when the mouse pointer exits a visible svg element.
         *
         * since S60 3.2
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */                        
        virtual TBool MouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt aX, TInt aY );
        
        
         /**
         * Notified when the mouse pointer has already entered a visible
         * svg element and just moved but remains inside its bounding-box.
         *
         * since S60 3.2
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */                      
        virtual TBool MouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt aX, TInt aY );
                                   
        /**
         * Notified when the mouse pointer is pressed down on visible svg element.
         *
         * since S60 3.2
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */                       
        virtual TBool MousePressed( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt aX, TInt aY );
                                   
        /**
         * Notified when the mouse pointer is released on on visible svg element.
         *
         * since S60 3.2
         * param : aElements -- SVG elements containing the mouse point.
         * param : aX -- x coordinate of mouse pointer.
         * param : aY -- y coordinate of mouse pointer.
         * @return : For future use.  Value is ignored.
         */                           
        virtual TBool MouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt aX, TInt aY );                                                                                                              
        
        
        /**
         * updates the CTM
         *
         * since S60 3.2
         * @param  none
         * @return none
         */                           
        virtual void   UpdateCTM();
        
        
        /**
         * 
         *
         * since S60 3.2
         * @param  none
         * @return none
         */
        void DeactivateAnimation(); 
        
      protected:  
      
         /**
         * does a deep copy of mediaanimation element
         *
         * since S60 3.2
         * @param  CSvgMediaAnimationElementImpl element
         * @return none
         */
       void                            CopyL( CSvgMediaAnimationElementImpl* aDestElement );
      private:
        // DRM related
        TBool                               iDrmEnabled;

        // Thumbnail mode
        TBool                               iIsThumbNailMode;

        // MSvgLoadingListener list
        RPointerArray<MSvgLoadingListener>  iSvgLoadingListeners;

        // Keep track of elements currently having mouse pointer inside
        // to report mouse-exit when it is moved.
        RPointerArray<CSvgElementImpl>      iSvgMouseEnteredElements;

        // Flag to indicate Load or PrepareDom call.
        TBool                               iIsLoadingRequest;

		TBool                               iMouseDownFlag;                                        
        
        TBool                               iFileIsLoaded;   //checks for parsing error                                   
                                           
         
                                              
        TFloatFixPt                     iScale;
        CSvgFitToViewBoxImpl*           iViewBoxImpl;   
        TFloatFixPt                     iX;
        TFloatFixPt                     iY;
        TBool iIsFocusable;        // To be implemented
       // TBool iInitialVisibility;  // InitialVisibility of the Animatin element
        // Referenced (target) element
        CSvgElementImpl* iReferencedElement;
        
        
        CSvgErrorImpl* iSvgError;   // error object
        CSvgDocumentImpl* iNewDocument; // Document pointer for child svg
        TGfxAffineTransform iOriginalRootTr; // For Transform
        TBool iIsOriginalRootTrSet;          
        TFloatFixPt                      iWidth;  
        TFloatFixPt                      iHeight;
        
        enum TAnimationState        // State of animation element
    	{
    		EPlayerStateIdle,
    		EPlayerStatePlaying,
    		EPlayerStatePaused,
    		EPlayerStateStop,
    		EPlayerProhibit
    	};
    	
        TAnimationState iAnimationStatus;
        
        enum TAnimationVisible        // Visibility State of animation element
    	{
    		EAnimationNotVisible,
    		EAnimationInitialised,
    		EAnimationVisible
    	};
    	
        TAnimationVisible iInitialVisibility;
        // Save the tick at the time animation starts
        TSvgTick iBeginTick;
        // Indicates whether time container issued a pause command
        TBool iIsSyncPauseIssued;      
        TBool iDurMediaTime;   // if duration is media
        HBufC*  iChildSvgName ;        
      	
        
    };

#endif // C_SVGMediaAnimationElementImpl_H
