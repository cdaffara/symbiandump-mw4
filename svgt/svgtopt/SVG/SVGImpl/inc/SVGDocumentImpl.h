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


#ifndef SVGDOCUMENTIMPL_H
#define SVGDOCUMENTIMPL_H


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include <gdi.h>
class CSvgEngineImpl;
class   CSvgEventHandler;
class CSvgErrorImpl;
class CSvgElementImpl;
class MSvgEventReceiver;
#include "SVGXmlDocument.h"
#include "SVGEvent.h"

#include "SVGEngineInterfaceImpl.h"
#include "SVGImageLoadingObserver.h" // MSVGImageLoadingObserver
#include "SVGErrorImpl.h"
#include "Gfx2dGcOpenVG.h"

//#include "SVGImageHashMap.h"
#include "SVGFontHashMap.h"
#include "SVGMemoryManager.h"
#include "SVGTimedEntityInterface.h"

#include "SvgBitmapFontProvider.h"
class   MXmlElement;
class   CSvgSchemaData;
class   CSvgContentHandler;

class CSvgAnimationBase;
class CSvgTimeContainer;

/**
 * Enumeration to define search query constraints for FindAllElements
 */
enum TSvgSearchFlags
    {
    ESvgSearchAllElements           = 0,
    ESvgSearchExcludeUseSubtree     = 0x2,
    };
/**
 * An SVG document that represents a SVG element tree..
 */
class CSvgDocumentImpl : public CBase,
                         public MXmlDocument,
                         public MSVGImageLoadingObserver,
                         public MSvgTimedEntityInterface
    {
    public:
        /**
         * Two Phase constructor for SVG document
         * @since 1.0
         * @param aHasParent - Indicates whether document has a 
         *                              parent document or not
         *        aSyncBehavDefault - Indicates default synchronised behaviour
         *                            for runtime synchronisation.
         *        aSyncTolDefault - Indicates the synchronised tolerance in 
         *                          msecs for runtime synchronisation.
         * @return CSvgDocumentImpl* 
         */
        void Reset(MSvgEvent* aEvent);

        static CSvgDocumentImpl* NewL(CSvgBitmapFontProvider* aSvgBitmapFontProvider, 
            const TBool aHasParent = EFalse,
            const TSvgSyncBehaviour aSyncBehavDefault = KSvgDefaultSyncBehaviour,
            const TInt32 aSyncTolDefault = KSvgDefaultSyncTolerance );

        /**
         * Need method description
         *
         * @since 1.0
         * @param aHasParent - Indicates whether document has a 
         *                              parent document or not
         *        aSyncBehavDefault - Indicates default synchronised behaviour
         *                            for runtime synchronisation.
         *        aSyncTolDefault - Indicates the synchronised tolerance in 
         *                          msecs for runtime synchronisation.
         * @return
         */
        static CSvgDocumentImpl* NewLC(CSvgBitmapFontProvider* aSvgBitmapFontProvider, 
            const TBool aHasParent = EFalse,
            const TSvgSyncBehaviour aSyncBehavDefault = KSvgDefaultSyncBehaviour,
            const TInt32 aSyncTolDefault = KSvgDefaultSyncTolerance );
            
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                 ~CSvgDocumentImpl();

    public:
        /**
         * ImageLoadingObserver interface method.
         * Called from Image loading class to notify image loading
         * completion.
         *
         * @since 1.0
         * @param : aErrorStatus -- error status, if any
         * @return : none
         */
        virtual void ImageLoadingCompleted( TInt aErrorStatus );

        // Pointer/Hyperlink related
        void FindAllElements( CSvgElementImpl* aStartElement, TInt aElementId,
                              RPointerArray<CSvgElementImpl>& aList,
                              TSvgSearchFlags aFlags = ESvgSearchAllElements );

        TBool SvgElementPresent(CSvgElementImpl* aElement);
        TBool SearchByPointer(CSvgElementImpl* aParent, CSvgElementImpl* aElement);
        CXmlElementImpl* GetFocusElement();
        void SetFocusElement(CXmlElementImpl* aElement );
        // From MXmlDocument
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*            CreateElementL( const TDesC& aTagName );

        MXmlElement*            CreateElementL( const TUint8 aTagName );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                    CreateAttribute( const TDesC& aName );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*            GetElementById( const TDesC& aElementId );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt           GetNumberOfIds(MXmlElement* aElement);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TDesC*            GetId( TInt index );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void            FindAllIds( CSvgElementImpl* aStartElement, RPointerArray<TDesC>& aList );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*            AppendChildL( MXmlElement* aAppendChild );

        // From MSvgDocument

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

        /**
         * Get URL
         */
        TDesC&         GetUrl();       // method of CSvgDocumentImpl

        /**
         * get root element
         */
        IMPORT_C MXmlElement*   RootElement();  // method of CSvgDocumentImpl

        // Class functions

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        CSvgSchemaData*         SchemaData();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                    SetUri( const TDesC& aUri );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                    GetUri( TDes& aUri );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                    SetEngine( CSvgEngineImpl* aEngine );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        CSvgEngineImpl*         Engine();

        /**
         * Add an object to the list of objects that are notified of an event
         *
         * @since 1.0
         * @param aElement - An SVG element that wants to be registered for an event (implements MSvgEventReceiver interface)
         * @param aEventMask - A bit mask that indicates the types of events an object listens for
         * @return None
         */
        void                    AddToEventReceiverListL( MSvgEventReceiver* aElement,
                                                        TUint8 aEventMask );

        /**
         * Add an object to the list of objects that are notified of an event
         * Has an additional parameter aEvent which is used to check for interactive events 
         *
         * @since v3.2
         * @param aElement - An SVG element that wants to be registered for an event (implements MSvgEventReceiver interface)
         * @param aEvent - An SVG Event that the target element needs to generate 
         * @param aEventMask - A bit mask that indicates the types of events an object listens for
         * @return None
         */
        void                    AddToEventReceiverListL( MSvgEventReceiver* aElement,
                                                         TSvgEvent aEvent,
                                                        TUint8 aEventMask );

         /**
         * Removed an object to the list of objects that are notified of an event
         *
         * @since 1.0
         * @param aElement - An SVG element that wants to be registered for an event (implements MSvgEventReceiver interface)
         * @return None
         */
        void                    RemoveFromEventReceiverList( MSvgEventReceiver* aElement );

          /**
         * Provide the event end time information to the Event Handler
         *
         * @since 1.0
         * @param aElement - An element that, ultimately, processes the event (eg. an animation element)
         * @param aTime - The end time for an event on the target element
         * @param aTargetElement - The element for which the event is processed
         * @return
         */
        void                    AddEventBeginTime(MSvgEventReceiver* aElement, TUint32 aTime, MSvgEventReceiver* aTargetElement );



        /**
         * An internal method that sorts the events in a time scale
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void                    SortEventList();

         /**
         * returns the next (or prev) focussable object starting with a given index
         *
         * @since 1.0
         * @param aIndex - search for a focussable object with this index
         * @param aNext - Etrue (next) EFalse (prev) (go forward or backward for search)
         * @param aEventMask - Event mask for event types the client is interested in
         * @param aNewIndex - Return the actual index where the object was found
         * @return The focussable object or NULL if not found
         */
        CSvgElementImpl*        GetEventReceiverElement(TInt32 aIndex, TBool aNext, TUint8 aEventMask, TInt32& aNewIndex);

             /**
         * Checks whether the current loaded file is animatable or static
         *
         *
         *
         * @param : None
         * @return: TBool boolean
         */

        TBool IsAnimationFile();
        
         /**
         * Checks whether the aSubEventMask is has a valid value set
         * @since v3.2
         * @param : aSubEventMask - The bitmap mask which contains the list
         *                          of interactive events if any with the element.  
         * @return: Etrue - if aSubEventMask has some valid bits set else EFalsee
         */
        TBool IsValidSubEventMask(TUint16 aSubEventMask);

           /**
         * Sets DRM Check Enable/Disable
         *
         *
         * @param : TBool : aEnable Boolean
         * @return: None
         */
        void SetDRMMode(TBool aEnable);

        void ReInitialize();

        /**
         * Load a svg content from the given filename
         *
         * @param : aFileName : filename of content (binary or text content)
         * @param : aError : error object to store error-info
         * @return: None
         */
        void Load( const TDesC& aFileName, CSvgErrorImpl& aError );

        /**
         * Load a svg content from the given byte-array
         *
         * @param : aByteData : byte-array content (binary or text content)
         * @param : aError : error object to store error-info
         * @return: None
         */
        void Load( const TDesC8& aByteData, CSvgErrorImpl& aError, TBool aRemoveFalseSwitchElements = ETrue );

		/**
         * Load a svg content from JSR226 that is already TDesc16
         *
         * @param : aSvgString
         * @param : aError : error object to store error-info
         * @return: None
         */
        void Load16BitString( const TDesC16& aSvgString, CSvgErrorImpl& aError, TBool aRemoveFalseSwitchElements = ETrue );

        /**
         * Load a svg content from the given file-handle
         *
         * @param : aFileHandle : file-handle content (binary or text content)
         * @param : aError : error object to store error-info
         * @return: None
         */
        void Load( RFile& aFileHandle, CSvgErrorImpl& aError );

        /**
         * Set the document to be in thumbnailmode, which will not consume the DRM right
         * if the Svg content is DRM encrypted.
         *
         * @param : aThumbNailMode : thumbnail mode flag.
         * @return:
         */
        void SetThumbNailMode( TBool aThumbNailMode );


        /**
        * Set the document to disable/enable consuming drm rights
        * @param aEnable: ETrue for disabling consumption of drm rights
        * @return : none
        */
        void SetDRMRights(TBool aEnable);
        /**
         * Return whether the loaded document is a thumbnail only.  ETrue is returned only
         * when the loaded content is DRM protected AND SetThumbNailMode is called with ETrue.
         *
         * @return:
         */
        TBool IsThumbNailOnly();

        /**
         * Set the flag to indicate this document was prepared (PrepareDom) or loaded (Load).
         *
         * @return: None
         */
        void SetWasPrepared( TBool aBool );

        /**
         * Return whether this document was prepared (PrepareDom) or loaded (Load).
         *
         * @return: Prepared or Loaded.
         */
        TBool WasPrepared();

        /**
         * Set the list of MSvgLoadingListener for this document to send
         * back notification of parsing progress.  NULL is also valid.
         * @param : aList : Pointer to list
         * @return : none
         */
        void SetLoadingListeners( const RPointerArray<MSvgLoadingListener>* aList );

        /**
         * Set the list of MSvgLoadingListener for this document to send
         * back notification of parsing progress.  NULL is also valid.
         * @param : aList : Pointer to list
         * @return : none
         */
        const RPointerArray<MSvgLoadingListener>* GetLoadingListeners();

        /**
         * Add a MSvgMouseListener to this SvgEngine
         *
         * @param : aListener -- listener to add to listener list
         * @return: None
         */
        void AddInternalMouseListener( const MSvgMouseListener* aListener );

        /**
         * Remove a MSvgMouseListener to this SvgEngine
         *
         * @param : aListener -- listener to remove from listener list
         * @return: None
         */
        void RemoveInternalMouseListener( const MSvgMouseListener* aListener );

        /**
         * Return number of mouselisteners.
         *
         * @return: mouse-listener count
         */
        TInt MouseInternalListenerCount();

        /**
         * Send to mouse listeners of a mouse-pressed event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyInternalMousePressed( RPointerArray<CSvgElementImpl>& aElements,
                                 TInt aX, TInt aY );

        /**
         * Send to mouse listeners of a mouse-released event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyInternalMouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                  TInt aX, TInt aY );

        /**
         * Send to mouse listeners of a mouse-entered event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyInternalMouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                 TInt aX, TInt aY );
        /**
         * Send to mouse listeners of a mouse-exited event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyInternalMouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                TInt aX, TInt aY );


        /**
         * Send to mouse listeners of a mouse-moved event.
         *
         * @param : aElements -- elements affected by mouse-event.
         * @param : aX -- X-cooordinate of mouse-event.
         * @param : aY -- X-cooordinate of mouse-event.
         * @return: None
         */
        void NotifyInternalMouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                                       TInt aX, TInt aY );

        // LoadingListeners notification
        /**
         * Send to loading listeners of a document start event.
         *
         * @return: None
         */
        void NotifyDocumentStart();

        /**
         * Send to loading listeners of a document end event.
         *
         * @return: None
         */
        void NotifyDocumentEnd();

        /**
         * Send to loading listeners of an element start event.
         *
         * @param : aTagName -- name of element tag.
         * @param : aIsSvgChild -- flag to indicate element is <svg> or child
         * of <svg> tag.
         * @return: None
         */
        void NotifyElementStart( const TDesC& aTagName,
                                 MSvgAttributeList& aAttributeList,
                                 TBool aIsSvgChild );

        /**
         * Send to loading listeners of an element end event.
         *
         * @param : aTagName -- name of element tag.
         * @param : aIsSvgChild -- flag to indicate element is <svg> or child
         * of <svg> tag.
         * @return: None
         */
        void NotifyElementEnd( const TDesC& aTagName, TBool aIsSvgChild );

        /**
         * Send to loading listeners of an unsupported element event.
         *
         * @param : aTagName -- name of element tag.
         * @return: None
         */
        void NotifyUnsupportedElement( const TDesC& aTagName,
                                       MSvgAttributeList& aAttributeList );

        /**
         * Send to loading listeners of an external data request.
         *
         * @param : aUri -- URI of data request.
         * @return: None
         */
        void NotifyExternalDataRequested( const TDesC& aUri );

        /**
         * Send to loading listeners of a succesfful external data reception.
         *
         * @param : aUri -- URI of data request.
         * @return: None
         */
        void NotifyExternalDataReceived( const TDesC& aUri );

        /**
         * Send to loading listeners of a failed external data request.
         *
         * @param : aUri -- URI of data request.
         * @return: None
         */
        void NotifyExternalDataRequestFailed( const TDesC& aUri );


        /**
         * return the animation duration in milliseconds.
         * @return : duration of animation
         */
        TUint32 AnimationDuration();

        /**
         * return wether animation is indefinite.
         */
        TUint32 IsIndefiniteAnimation();

        /**
         * Called by AppendChild/RemoveChild to notify owner document
         * of changes.
         */
        void ElementAppendedOrRemoved();

        /**
         * Cancel loading of document if it is currently parsing.
         */
        void CancelParsing();

        /**
         * Return whether document is currently parsing.
         */
        TBool IsParsing();

        /**
         * return whether this document has an error set.
         */
        TBool HasError();

        /**
         * return the error reference.  May be NULL.  Call HasError first.
         */
        MSvgError* GetError();

        /**
         * Set the error type and error message for this document.
         * Will note override existing error.
         */
        void SetError( TInt aErrorType, const TDesC& aMsg1, const TDesC& aMsg2 );

		void SetIsWarning( TBool aIsWarning = ETrue );
    //-------------------------------------------------------------------
    // Utility Functions
    //-------------------------------------------------------------------
        /**
         * Fill the given error object with the appropriate error codes, messages.
         *
         * @param : aError : Error object to fill code/message.
         * @param : aSvgErrorCode : Svg related error code.
         * @param : aSystemError : Symbian error code.
         * @param : aMsg1 : Message string
         * @param : aMsg2 : Second message string, if needed, appended to first string.
         * @return: None
         */
    static void PrepareError( CSvgErrorImpl& aError,
                              TSvgErrorCode aSvgErrorCode, TInt aSystemError,
                              const TDesC& aMsg1, const TDesC& aMsg2 );

    static void OpenSession( RFs& aSession, CSvgErrorImpl& aError );

	// ------------------------------------------------------------------
	// Opacity
	// ------------------------------------------------------------------
    TBool isGroupOpacity( CSvgElementImpl* aElement );

	void	PrintAllElements( CSvgElementImpl* aStartElement);

	void QueueFontReleaseReq(CFont* aFont);


        /**
         * Accessor funciton for SyncBehaviorDefault value
         * @since S60 3.2
         * @param none
         * @return TSvgSyncBehavior indicating default value.
         */
        TSvgSyncBehaviour SyncBehaviorDefault();

        /**
         * Accessor funciton for SyncToleranceDefault value
         * @since S60 3.2
         * @param none
         * @return TUint32 indicating default value in msecs.
         */
        TUint32 SyncToleranceDefault();

        /**
        * Set Function for the document's current time
        * @since S60 3.2
        * @param aCurTime Current Time to be set in msecs
        * @return none.
        */
        void SetCurrentTime( const TInt32 aCurTime );

        /**
        * Accessor Function for the document's current time
        * @since S60 3.2
        * @param none.
        * @return TInt32 Current Time to be set in msecs.
        */
        TInt32 CurrentTime();

        void DocumentTransform(TGfxAffineTransform& aTr);
        TGfxAffineTransform GetDocumentTransform();

        /**
        * Set the Synchronised behaviour for the time container
        * @since S60 3.2
        * @param aValue canSlip | locked | independent 
        * @return none.
        */
        void SetTCSyncBehavior( const TSvgSyncBehaviour aValue );
        
        /**
        * Set the Synchronised Tolerance for the time container
        * @since S60 3.2
        * @param aValue Clock-value 
        * @return none.
        */        
        void SetTCSyncTolerance( const TUint32 aValue );
        
        /**
        * Set the time container as Synchronised Master
        * @since S60 3.2
        * @param aValue true | false
        * @return none.
        */        
        void SetTCSyncMaster( const TBool aValue );
        
        /**
        * Accessor for the time container.
        * @since Series 3.2
        * @param none.
        * @return CSvgTimeContainer* 
        */        
        CSvgTimeContainer* TimeContainer();
        
        
		//Get function(s)
		CSvgBitmapFontProvider* GetBitmapFontProvider() ;
    
        void SetImageElementsCount(TInt aCnt);
        
        TInt GetImageElementsCount();

    /* Return the value of the current focus index.
		@return: current focus index    
    */
    TInt32 GetCurFocusIndex();
    
    /* Set the value fo the current focus index
    	@param: aVal : Value that has to be set as the focus index.
    	@return: none.
    */
    void SetCurFocusIndex(TInt32 aVal);
    
    /* Decrement the value of the current focus index
    	@return: decremented focus index value.
    */
    TInt32 DecCurFocusIndex();
    
    /* Increment the value of the current focus index
    	@return: Incremented focus index value.
    */
    TInt32 IncCurFocusIndex();

    /* Return the pointer to the current focus element
		@return: current focus element    
    */
    CSvgElementImpl* GetCurFocusObject();
    
    /* Set the value of the current focus element to aElement.
    	@param: aElement : Element that has to be set as the focus element
    	@return: none.
    */
    void SetCurFocusObject(CSvgElementImpl* aElement);

        /*
        From MSvgTimedEntityInterface
        Would return the type of object
        @param none
        @return type of object
        */
        TSvgObjectType ObjectType();
        
        
        
        /**
        * Prepare a list of media elements in DOM tree in 
        * postorder
        * @since Series 3.2
        * @param aRoot Root Node of tree
        * @param aPostOrderList List of nodes which 
        *         will contain the elements.
        * @return None. 
        */        
        void ParsePostOrderMediaElements( CSvgElementImpl* aRoot, 
            RPointerArray<CSvgElementImpl>& aPostOrderList);
        
        /**
        * Finds if Animation element is there.
        * 
        * @since Series 3.2
        * @param none
        * @return True if animation element is present. 
        */        
        TBool SvgHasAnimationElement();
        
          /**
        * Finds if Document content is Interactive
        * 
        * @since Series 3.2
        * @param none
        * @return True if Document content is Interactive
        */ 
        TBool IsDocumentContentInteractive();
        
    private:
        /**
         * Constructor
         * @param aHasParent - Indicates whether document has a parent or not
         *        aSyncBehavDefault - Indicates default synchronised behaviour
         *                            for runtime synchronisation.
         *        aSyncTolDefault - Indicates the synchronised tolerance in 
         *                          msecs for runtime synchronisation.
         */
         CSvgDocumentImpl( CSvgBitmapFontProvider* aSvgBitmapFontProvider, const TBool aHasParent,
                           const TSvgSyncBehaviour aSyncBehavDefault,
                           const TInt32 aSyncTolDefault );

        /**
         * 2-phase construction
         */
        void                    ConstructL();

        /**
         * search element by ID
         */
        MXmlElement*            SearchElementById( MXmlElement* aElement,
                                                   const TDesC& aId );

    public:
        TBool                   iInitSortList;
        TBool                   iReqExReqFtrSysLTested;
        TBool                   iInitialDrawFlag;
        CSvgEventHandler        *iEventHandler;
        TBool                   iIsInteractive;
        TBool                   iFinishedParsing;
        TBool                   iHasGroupOpacity;
        //for debugging and performance checking
        CSvgElementImpl*                    iPerfText;

				TUint32 								iTimeForJSR226;

//		CSvgImageHashMap* iImageHashMap;
		CSvgFontHashMap* iFontHashMap;
		CSvgMemoryManager* iMemoryManager;

		RPointerArray<CSvgAnimationBase>    iSvgAnimations;

        TInt          iImageElementCnt;

    private: // From MSvgTimedEntityInterface
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
        * Returns the sync behavior of the entity.
        * @since Series 3.2
        * @param none. 
        * @return TSvgSyncBehaviour Element's Sync Behaviour.
        */
        TSvgSyncBehaviour GetEntitySyncBehavior();
        
        /**
        * From MSvgTimedEntityInterface
        * When the timed entity acts as timing master in the time container, 
        * the time container gets the timed entity clock using this method 
        * and feeds to rest of timed entities.
        * @since Series 3.2
        * @param aEntityCurTime Current Entity Time in msecs. 
        * @return none.
        */
        void GetEntityCurrentTime( TUint32& aEntityCurTime );

        /**
        * From MSvgTimedEntityInterface
        * Returns the configured sync master value(as per DOM tree) as specified 
        * in the SVG content.
        * @since Series 3.2
        * @param aIsSyncMaster Indicates whether the element is configured as 
        *                      Sync Master. 
        * @return none.
        */
        void GetCnfSyncMasterStatus( TBool& aIsSyncMaster );

        /**
        * From MSvgTimedEntityInterface
        * Check if timed entity is going to act as timing master in the 
        * time container. This behavior could change dynamically.
        * @since Series 3.2
        * @param aIsSyncMaster Indicates whether the element is currrently Sync Master. 
        * @return none.
        */
        void GetCurSyncMasterStatus( TBool& isSyncMaster );

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
        * Check if timed entity can use its parent's tick. Usually only the
        * parent document should return true for this function.
        * All other elements return false
        * @since Series 3.2
        * @param none.
        * @return TBool True if can use parent's timing tick.
        */
        TBool CanUseParentTick();

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
        * From MSvgTimedEntityInterface
        * This would be used for pausing the timed entity while other locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void PauseTimedEntity();

        /**
        * From MSvgTimedEntityInterface
        * This would be used for resuming the timed entity once all locked
        * timed entities get loaded.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void ResumeTimedEntity();
        
        /**
        * From MSvgTimedEntityInterface
        * This would be used for stopping the timed entity.
        * @since S60 3.2
        * @param none.
        * @return none.
        */
        void StopTimedEntity();
        
        
       CSvgTimeContainer* GetChildTimeContainer();
       
       
    private:
        void Load( RFs& aSession, RFile& aFileHandle, CSvgErrorImpl& aError );

        void ProcessDRMError( TInt errorCode, CSvgErrorImpl& aError );

        TBool IsGzipContent( RFile& aFile );
        TBool IsGzipContent( const TDesC8& aByteArray );

        TBool IsDRMContent( RFile& aFileHandle );
        TBool IsDRMContent( const TDesC8& aByteData );

        TBool IsBinarySvg( const TDesC8& aByteArray );

        TBool ProcessGZipContent( RFs& aSession,
                                  RFile& aFileHandle,
                                  TFileName& aFileName,
                                  CSvgErrorImpl& aError );

        TBool ProcessGZipContent( RFs& aSession,
                                  TFileName& aZippedFileName,
                                  TFileName& aUnzippedFileName ,
                                  CSvgErrorImpl& aError );

        HBufC8* ProcessDRMContentL( RFile& aFileHandle, CSvgErrorImpl& aError );

        void ProcessSvgContentL( const TDesC8& aByteArray, CSvgErrorImpl& aError, TBool aRemoveFalseSwitchElements = ETrue );

		void Process16BitSvgContentL( const TDesC16& aByteArray, CSvgErrorImpl& aError, TBool aRemoveFalseSwitchElements = ETrue );

        TInt GetProcessPrivatePath( RFs& aSession, TFileName& aPath );

        TInt CreateProcessPrivateDirectory( RFs& aSession, TFileName& aPath );

        TInt WriteToTempFile( RFs& aSession, const TDesC8& aByteArray,
                              TFileName& aFilename, CSvgErrorImpl& aError );

        TInt WriteToTempFile( RFs& aSession, RFile& aFileHandle,
                              TFileName& aFilename, CSvgErrorImpl& aError );

        void LoadingCompleted();

        TUint32 SubtreeDuration( CSvgElementImpl* aRootElement );

    public:
        TBool iAnimationResetNeeded;

        // Flag set to ETrue when content-handler is removing false switch statements
        TBool                   iIsPruningTree;
        TBool                   iIsRendering;

    private:
        CSvgEngineImpl*         iEngine;

        TBuf<256>              iUri;
        CSvgElementImpl*       iRootElement;

        CSvgSchemaData*         iSchemaData;

        TBool                   iDrmEnable;

         //flag used for DOM_Reuse
        TBool                   iMultipleRendering;

        TBool                   iHasGradientElement;

        TBool                   iIsThumbNailMode;
        TBool                   iIsDRMProtected;
        //The value that holds the index of the current focus element.
	    TInt32                  iCurObjIdx;
	    // pointer to the current focus element object.
        CSvgElementImpl*        iCurrentFocusObject;
         // DRM Rights consumption
        TBool         iDrmRightsConsumptionEnabled;

        // Flag to indicate this dom was Loaded or Prepared.
        TBool                   iWasPrepared;

        const RPointerArray<MSvgLoadingListener>* iLoadingListeners;

        // MSvgMouseListener list
        RPointerArray<MSvgMouseListener>    iSvgMouseListeners;

        TBool                   iCheckedAnimationDuration;
        TUint                   iAnimationDuration;

        CSvgContentHandler*     iXmlHandler;

        CSvgErrorImpl*          iError;
        
        // Time container for document for run time sync
        CSvgTimeContainer*      iTimeContainer;
        // Indicates if this document a top-level document
        TBool iHasParent;

        // Default run time sync behaviour 
        TSvgSyncBehaviour           iSyncBehaviorDefault;
        
        // Default synchronised tolerance value in msecs
        TUint32                     iSyncToleranceDefault;
        
        
        static const TInt KIndefiniteAnimationDuration = 2147483647;
        TGfxAffineTransform iTransformMatrix;
        
        //Stores Font Type information as passed by CSvgEngineInterfaceImpl
        CSvgBitmapFontProvider *iSvgBitmapFontProvider;
    };

#endif      // SVGDOCUMENTIMPL_H
