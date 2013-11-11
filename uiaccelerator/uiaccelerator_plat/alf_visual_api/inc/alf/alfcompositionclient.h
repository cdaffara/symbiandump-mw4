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
#ifndef __ALFCOMPOSITIONCLIENT_H__
#define __ALFCOMPOSITIONCLIENT_H__

#include <e32hashtab.h>
#include <alf/alfcompositionutility.h>

const TInt KAlfCompositionControllerIfUid(0x2002bcfc);

/** Interface for class that actually controls the composition singleton **/
class MAlfCompositionController: public MAlfCompositionControl
    {
public:
    virtual TInt CreateTargetL(TInt aTarget, TInt aClientHandle, TInt aGrpHandle ) = 0;    
    virtual void SetTargetL(TInt aTarget) = 0;    
    virtual void SetExtentSurfaceId(const TSurfaceId& aSurfaceId) = 0;
    virtual TInt SetOrder(TInt aTarget, TInt aTop, TBool aAbove, TBool aCombineTargets) =0;
    virtual void DeleteTarget(TInt aTarget)=0;
    virtual void LowOnGraphicsMemory()=0;
    virtual void GraphicsMemoryGood(){}
    
    virtual void AlfBridgeCallback(TInt, TAny*){}
    virtual void HideTarget(TInt,TBool){};
    }; 

/** Wrapper class IPC <-> MAlfCompositionController **/
NONSHARABLE_CLASS(CAlfCompositionCntrlClient): public CAlfCompositionClientBase
    {
public:
    CAlfCompositionCntrlClient(MAlfCompositionController* aController):iController(aController){}
    IMPORT_C static CAlfCompositionCntrlClient* NewL(RAlfBridgerClient* aClient, MAlfCompositionController* aController);
    IMPORT_C ~CAlfCompositionCntrlClient();
    
    // From base
    void HandleEventL(TInt aEventType, TAny* aEventData);
    // from CActive
    TInt RunError(TInt aError);
private:
    MAlfCompositionController* iController;
    
    RHashMap<TInt, TInt> iHostBindingsHash;
    RHashMap<TInt, TInt32> iHostPermittedOpsHash;
    
    };
    

/**
Example1:
* // to set target from other app semitransparent 
* CAlfCompositionHost* me = CAlfCompositionHost::NewL(tokenFromSource, myCompositionKey);
* me->EnableAlpha(); 
* me->SetOpacity(0.5f);
* //...
**/
class CAlfCompositionHost: public CAlfCompositionSource
    {
public:
    /** 
    * Constructor for application that wishes modify 
    * composition information for some other process, see CreatePermissionToken()
    *
    * Leaves using KErrNotFound, if token is not valid
    * 
    * If the binded target layer is destroyed by its owner, any Set operations 
    * will have no effect.
    */
    IMPORT_C static CAlfCompositionHost* NewL(TInt aToken, TInt aKey);

    void ConstructL(TInt aToken, TInt aKey);
    
//private: // data
//    class CAlfCompositionHostData;
//    CAlfCompositionHostData* iData;
    };


// ================ Posting like SW interface ============================================
// 
// =======================================================================================

/** Inferface for the pixel buffer provider to provide information, frames and get events concerning context events **/
class MAlfBufferProvider
    {
public:
    
    enum TBufferFormat
        {
        ESourceFormatRGB_565 = 0,
        ESourceFormatXRGB_8888 = 1, 
        ESourceFormatARGB_8888 = 2,
        ESourceFormatARGB_8888_PRE = 3
        };

    struct TBufferCreationAttributes
        {
        /*
         * Buffer size
         */
        TInt iWidth;
        TInt iHeight;
        /**
         * iStride should be at least bytes * width.
         * iStride may still be adjusted by CAlfCompositionPixelSource for the target HW driver implementation
         * Client should always use iStride value available after CAlfCompositionPixelSource::ActivateL call.
         **/
        TInt iStride;
        
        TInt iAlignment;
        TBufferFormat iFormat;
        };

    /**
     * Called by framework when the next buffer is ready and available for the client to draw.
     * @param aVisibleRegion is currently visible surface region
     * 
     * @return ETrue to continue drawing of frames. EFalse to pause drawing.
     *          After pausing drawing client needs to call CAlfCompositionPixelSource::ActivateL to
     *          continue drawing of frames 
     **/
    virtual TBool ProduceNewFrameL(const TRegion& aVisibleRegion, TUint8*& aBuffer) = 0;

    /**
     * Called by framework when new buffer is been created. Client should provide desired creation attributes.
     **/
    virtual TBufferCreationAttributes& BufferAttributes() = 0;
    
    /**
     * Notification from FW 
     **/
    virtual void ContextAboutToSuspend() =0;
    virtual void OnActivation() = 0;
    };

/**
* Example1:
* // to draw frames with CAlfCompositionPixelSource API
* class CMyClass: public CBase, public MAlfBufferProvider
*     { 
*     private:
*         CAlfCompositionPixelSource* iAlfCompositionPixelSource;
*         MAlfBufferProvider::TBufferCreationAttributes iAttributes;
*     public:
*     
*     CMyClass() {}
*     
*     ConstructL()
*       {
*       // ...
*       iAttributes.iWidth = width;
*       iAttributes.iHeight = height;
*       iAttributes.iFormat = MAlfBufferProvider::ESourceFormatARGB_8888;
*       iAttributes.iStride =  iAttributes.iWidth * 4;
*       iAttributes.iAlignment = 32;
*       
*       iAlfCompositionPixelSource = CAlfCompositionPixelSource::NewL(*this, iMyDrawableWindow);
*       iAlfCompositionPixelSource->iActivateL()
*       }
*     
*    TBool ProduceNewFrameL(const TRegion& aVisibleRegion, TUint8*& aBuffer)
*       {
*       // ...
*       // copy pixel data to aBuffer
*       TUint8* from = (TUint8*)bitmap->DataAddress();
*       TUint bytes = bitmap->SizeInPixels()*4;
*       for (TInt y = bitmap->SizeInPixels().iHeight; y > 0; y--)
*           {
*           Mem::Copy(aBuffer, from, bytes);
*           aBuffer += iAttributes.iStride;
*           from += bitmap->DataStride();
*           }
*       // ...
*       return ETrue; // to continue getting ProduceNewFrameL callbacks
*       };
*
*    MAlfBufferProvider::TBufferCreationAttributes& BufferAttributes() { return iAttributes };
*   
*    void ContextAboutToSuspend() {};
*    void OnActivation() {};
*    // ...
*    }
*
* Example2:
* // client may want or need to adjust ProduceNewFrameL-callbacks inverval
* class CMyClass: public CActive, public MAlfBufferProvider
*     { 
*     private:
*         CAlfCompositionPixelSource* iAlfCompositionPixelSource;
*         MAlfBufferProvider::TBufferCreationAttributes iAttributes;
*         TBool iDrawNext;
*         TTimeIntervalMicroSeconds32 iWaitTime;
*         RTimer iTimer;
*     public:
*     
*     CMyClass() {}
*     
*     ConstructL()
*       {
*       // ...
*       iDrawNext = ETrue;
*       iWaitTime = 25*1000; 
*       // ...
*       }
*
*    void RunL()
*       {
*       if( iStatus.Int() != KErrNone )
*           {
*           iDrawNext = ETrue;
*           iAlfCompositionPixelSource->ActivateL();
*           }
*       }
*    // ... 
*    TBool ProduceNewFrameL(const TRegion& aVisibleRegion, TUint8*& aBuffer)
*       {
*       if( iDrawNext )
*           {
*           iDrawNext = EFalse;
*           }
*       else
*           {
*           iTimer.After( iStatus, iWaitTime );
*           return EFalse;
*           }
*       // ...
*       // copy pixel data to aBuffer
*       // ...
*       return ETrue; // to continue getting ProduceNewFrameL callbacks
*       };
*    // ...
*    }
*    
* Example3:
* // API can be used withou the actual window
* class CMyClass: public CBase, public MAlfBufferProvider
*     { 
*     private:
*         CAlfCompositionPixelSource* iAlfCompositionPixelSource;
*         MAlfBufferProvider::TBufferCreationAttributes iAttributes;
*     public:
*     
*     CMyClass() {}
*     
*     ConstructL()
*       {
*       // ...
*       iAttributes.iWidth = width;
*       iAttributes.iHeight = height;
*       iAttributes.iFormat = MAlfBufferProvider::ESourceFormatARGB_8888;
*       iAttributes.iStride =  iAttributes.iWidth * 4;
*       iAttributes.iAlignment = 32;
*       
*       iAlfCompositionPixelSource = CAlfCompositionPixelSource::NewL(*this, NULL);
*       iAlfCompositionPixelSource->SetExtent(TRect(0,0,600,300),0,1);
*       iAlfCompositionPixelSource->iActivateL();
*       }
*    // ... 
*    TBool ProduceNewFrameL(const TRegion& aVisibleRegion, TUint8*& aBuffer)
*       {
*       // ...
*       // copy pixel data to aBuffer
*       // ...
*       return ETrue; // to continue getting ProduceNewFrameL callbacks
*       };
*    // ...
*    
*    // Notice: When activating Composition pixel source after being suspended, SetExtent needs to be called again.
*    void ContinueAfterSuspendL()
*       {
*       iAlfCompositionPixelSource->SetExtent(TRect(0,0,600,300),0,1);
*       iAlfCompositionPixelSource->iActivateL();
*       }
*    }
**/
class CAlfCompositionPixelSource: public CAlfCompositionSource
    {
public:
    /** 
     * Constructor 
     **/
    IMPORT_C static CAlfCompositionPixelSource* NewL(MAlfBufferProvider& aProvider, RWindow* aWindow = NULL);
    

    /**
     * Constructor for application to decide AO priority
     */
    IMPORT_C static CAlfCompositionPixelSource* NewL(MAlfBufferProvider& aProvider, TInt aPriority = CActive::EPriorityIdle, RWindow* aWindow = NULL );

    
    /**
     * Activate drawing for the first time or continue drawing if it is being paused or suspended
     **/
    IMPORT_C void ActivateL();
    
    
    /*
     * An other option to activate/continue drawing. This activate method ensures that no saw-edged black area is seen if client
     * only wants to draw one frame. MAlfBufferProvider::ProduceNewFrameL is called from inside ActivateSyncL so that 
     * surface buffer can be updated before making surface visible.
     */
    IMPORT_C void ActivateSyncL();
    
    /**
     * Suspend drawing 
     **/
    IMPORT_C void Suspend();

    IMPORT_C ~CAlfCompositionPixelSource();

    /** 
    * From base class 
    * Defines extent of the composition target on a defined screen
    * Does not affect on hit testing, keyboard focus etc. but is just additional information for composition
    **/
    IMPORT_C TInt SetExtent(const TRect& aRect, TInt aScreen);
    
    /**
     * Internal
     * For drawing one frame
     **/
    TBool DrawFrameL(TRequestStatus& aStatus, TInt aBufferNumber = 0);

protected:

    /**
     * Internal
     * From base class 
     **/
    void HandleEventL(TInt aEventType, TAny* aEventData);                                          

private: // Internal / utils
    
    void ConstructL(MAlfBufferProvider& aProvider, TInt aPriority, RWindow* aWindow);
    void ConstructSurfaceL(MAlfBufferProvider::TBufferCreationAttributes& aCreationAttributes);
    void FreeSurface();
    TUint8* Buffer(TInt aBufferNumber);

    
private: // data
    
    class CAlfCompositionPixelSourceData;
    CAlfCompositionPixelSourceData* iData;
    };


#endif // #define __ALFCOMPOSITIONCLIENT_H__
