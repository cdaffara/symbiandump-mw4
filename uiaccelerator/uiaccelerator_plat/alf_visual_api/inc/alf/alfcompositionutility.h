/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/
#ifndef __ALFCOMPOSITIONUTILITY_H__
#define __ALFCOMPOSITIONUTILITY_H__

#include <e32base.h>
#include <w32std.h>


class RAlfBridgerClient;
/*
 * CAlfRsSendBuffer creates in construction one CAlfCompositionClient. That client 
 * will wait for request completion. Request is completed, when an application communicaties 
 * to CAlfRenderStage via CAlfCompositionSource client. After request is completed, the 
 * CAlfCompositionClient (owned by CAlfRsSendBuffer) is again set active and it waits for 
 * the next message.
 */

/** Base classes for logistics **/
class CAlfCompositionClientBase: public CActive
    {
public:
    
    enum TAlfCompositionPermission
        {
        EAlfAllowChangeExtent           = 1,
        EAlfAllowChangeAlpha            = 2,
        EAlfAllowChangeOpacity          = 4,
        EAlfAllowChangeRotation         = 8,
        EAlfAllowChangeBackgroundAnim   = 16
        }; 
    
protected:
    CAlfCompositionClientBase(TInt aPriority = CActive::EPriorityHigh):CActive(aPriority)
        {
        CActiveScheduler::Add(this);
        }
        
    IMPORT_C ~CAlfCompositionClientBase();
    IMPORT_C void RequestEventL(TInt aBufferSize, RAlfBridgerClient* aClient = 0);        
    IMPORT_C TInt SendEvent(TInt aOp, const TAny* aEventData, TInt aEventSize);
    IMPORT_C void SetHandleL(TInt aHandle);
    
    // Virtuals
    IMPORT_C virtual void HandleEventL(TInt aEventType, TAny* aEventData);
    // from CActive
    IMPORT_C TInt RunError(TInt aError);

private: // from CActive
    void RunL();
    void DoCancel();

public: // for utils
    TInt Handle() const;
        
private: // data
    class CAlfCompositionClientBaseData;
    CAlfCompositionClientBaseData* iData;
    };

NONSHARABLE_CLASS(CAlfCompositionClientBase::CAlfCompositionClientBaseData):public CBase
    {
    public:
    static CAlfCompositionClientBaseData* NewL(TInt aBufferSize, RAlfBridgerClient* aClient);

    void ConstructL(TInt aBufferSize, RAlfBridgerClient* aClient);

    ~CAlfCompositionClientBaseData();

    RAlfBridgerClient* iClient;
    TBool iOwnsClient;
    TBuf8<60> iBuffer;
    TBool* iDeleted;
    TInt iBufferSize;
    TInt iHandle;
    };

class MAlfCompositionControl
    {
    public:
        /** 
    * Enables / disables alpha on composition target 
    **/
    virtual TInt EnableAlpha(TBool aEnable = ETrue) = 0;
    
    /** 
    * Set constant opacity (0..255) multiplier that will be applied to 
    * composition target <in practice, only 256 different shades supported> 
    **/
    virtual TInt SetOpacity(TReal32 aOpacity) __SOFTFP = 0;
    
    /** 
    * rotation of composition target
    * <in practice, only 90 degree steps supported, value given will be rounded to closest rotation> 
    **/
    virtual TInt SetRotation(TInt aRotationInDegrees) = 0;
        
    /** 
    * Defines extent of the composition target on a defined screen
    * Does not affect on hit testing, keyboard focus etc. but is just additional information for composition
    **/
    virtual TInt SetExtent(const TRect& aRect, TInt aScreen) = 0;

    /** 
    * Makes this composition target system wide background surface 
    **/
    virtual void SetIsBackgroundAnim(TBool ETrue) = 0;    

    /** 
    * Defines extent of the composition source rectangle
    **/
    virtual TInt SetSourceRect(const TRect& aRect) = 0;
    };   


/** Client interface for composition fw information **/
class MAlfCompositionObserver
    {
public:
    /**
    * Called by framework when composition framework has completed a frame and thus can 
    * efficiently handle changes and/or new content
    **/    
    virtual void FrameReady(TInt aScreenNumber) = 0;

    /**
    * Notifications from FW
    **/    
    virtual void RunningLowOnGraphicsMemory() = 0; // running low on resources, clear cahces etc.
    virtual void CompositionTargetHidden() = 0; // target is no more visible on composition scene, 
                                                // free at least slack resources 
    virtual void GraphicsMemoryGood(){}         // graphics memory status is normal again  
	virtual void CompositionTargetVisible() {}  // target is visible again after first being hidden                                  
    };

/**
* Example1:
* // to set target semitransparent 
* CAlfCompositionSource* me = CAlfCompositionSource::NewL(iMyDrawableWindow);
* me->EnableAlpha(); 
* me->SetOpacity(0.5f);
* //...
* delete me; // deletion of client will restore state to original
*
* Example2:
* // to fade in for composition target
* class CMyClass: public CBase, private MAlfCompositionObserver
*     { 
*     CMyClass():iOpacity(0)
*       {
*       }
*     ConstructL()
*       {
*       // ...
*       iAlfCompositionSource = CAlfCompositionSource::NewL(iMyDrawableWindow);
*       iAlfCompositionSource->EnableAlpha(); 
*       iAlfCompositionSource->SetOpacity(iOpacity);
*       }
*     
*     void FrameReady(TInt aScreenNumber)
*       {
*       if (iOpacity < 1)
*           { 
*           iOpacity += 0.03; // just follow refresh linearly      
*           iAlfCompositionSource->SetOpacity(iOpacity);
*           }
*        else
*           { // fade in complete, no need to have composition cntrl anymore
*           delete iAlfCompositionSource;
*           iAlfCompositionSource = 0;
*           }       
*       }    
*
* Example 3:       
* // To grant other application / process a priviledged rights to modify composition target
* const TInt mySuperSecretKeyForHost1 = 0x000FAB10;
* const TInt flagsForHost1 = EAlfAllowChangeExtent | EAlfAllowChangeAlpha; // but not rotation  
*
* CAlfCompositionSource* me = CAlfCompositionSource::NewL(iMyDrawableWindow);
* TInt generatedKey = me->CreatePermissionToken(0x000FAB10, flagsForHost1);
* 
* PassGeneratedKeyToHost(generatedKey); // it already knows the other key..
*
* TBD, will the generates key work even after composition source destroys the
* CAlfCompositionSource instance
*      
**/
class CAlfCompositionSource: public CAlfCompositionClientBase, MAlfCompositionControl
    {
public:

    /** 
    * Constructor 
    **/
    IMPORT_C static CAlfCompositionSource* NewL(RWindow& aClientWindow);

    /** 
    * Creates handle based on key
    * Flags define which kind of operations are allowed by host application
    * using the token. Same token can be used multiple times for different clients.
    *
    **/
    IMPORT_C TInt CreatePermissionToken(TInt aKey, TInt aPermissionFlags);

    /** 
    * Enables / disables alpha on composition target 
    **/
    IMPORT_C TInt EnableAlpha(TBool aEnable = ETrue);
    
    /** 
    * Set constant opacity 0.0 - 1.0f multiplier that will be applied to composition target
    * <in practice, only 256 different shades supported> 
    **/
    IMPORT_C TInt SetOpacity(TReal32 aOpacity) __SOFTFP;
    
    /** 
    * rotation of composition target
    * <in practice, only 90 degree steps supported, value given will be rounded to closest rotation> 
    **/
    IMPORT_C TInt SetRotation(TInt aRotationInDegrees);
    
    /** 
    * @prototype Consult owner about the usage and restrictions
    *
    * Defines the composition order. On success returns token
    * that can be used to create new CAlfCompositionSource object to
    * control union of theses two composition targets as single object 
    * for convenience
    *
    * Does not affect on hit testing, keyboard focus etc.
    * 
    * @return new token to control the union of composition objects (assume key different from default)
    * 
    **/
    IMPORT_C TInt SetZOrder(const CAlfCompositionClientBase& aNode, TBool aAbove, TInt aKey = -1); // todo magic
    
    /** 
    * Defines extent of the composition target on a defined screen
    * Does not affect on hit testing, keyboard focus etc. but is just additional information for composition
    **/
    IMPORT_C TInt SetExtent(const TRect& aRect, TInt aScreen);

    /** 
    * Adds composition observer for better synchronization
    **/
    IMPORT_C void AddCompositionObserverL(MAlfCompositionObserver& aObserver, TInt aScreenNumber = 0); // Todo: KMainScreen);

    /** 
    * Removes composition observer
    **/
    IMPORT_C void RemoveObserver(MAlfCompositionObserver& aObserver);

    /** 
    * Makes this composition target system wide background surface 
    * any other target that was previously used as bg anim will be discarded
    * < should this require some capability like WriteDeviceData ?>
    **/
    IMPORT_C void SetIsBackgroundAnim(TBool aIsBg);
    
    /** 
    * Defines extent of the composition source rectangle
    **/
    IMPORT_C TInt SetSourceRect(const TRect& aRect);
    
    /** 
    * Internal
    * For testing applications on extrenal screen 
    **/
    IMPORT_C void EnableKeyboard(TBool aEnable, TInt aScreen);

    IMPORT_C ~CAlfCompositionSource();

protected: // utils / baseclass
    IMPORT_C virtual void HandleEventL(TInt aEventType, TAny* aEventData);
    virtual void ConstructL(TInt aHandle1, TInt aHandle2, TInt  aScreenNumber);
    virtual void ConstructL(RWindow& aClientWindow);
    TInt Handle();

protected: // data
    class CAlfCompositionSourceData;
    CAlfCompositionSourceData* iData;
    };

NONSHARABLE_CLASS(CAlfEffectObserver): public CBase
    {
public:
    
    class MAlfEffectObserver
        {
        public:    
        /** bit field */    
        enum{
            EAlfEffectStarted = 0x1,
            EAlfEffectComplete = 0x2
            };    
        virtual void HandleEffectCallback(TInt aType, TInt aHandle, TInt aStatus) = 0;
        };
    
    IMPORT_C static CAlfEffectObserver* NewL();
    IMPORT_C ~CAlfEffectObserver();

     /**
     * Asks the number of active effects
     * @return error code or number of active effects (>= 0).
     */    
    IMPORT_C TInt ActiveEffectsCount();

     /**
     * Subscribe callback for a handle
     * @leave system wide error code
     */    
    IMPORT_C void SubscribeCallbackL(MAlfEffectObserver* aObserver, TInt aHandle, TInt aType = MAlfEffectObserver::EAlfEffectComplete );
    
	/**
	 * Define window, that should be considered transparent
	 * by fullscreen heuristic analysis. The window will
	 * be drawn normally.
	 */
    IMPORT_C void SetDistractionWindow(const RWindowTreeNode& aWindow, TInt aState = 1);
    
private:
    CAlfEffectObserver();
    class CAlfEffectObserverData;
    CAlfEffectObserverData* iData;
    };

#endif // #define __ALFCOMPOSITIONUTILITY_H__
