// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "displayrenderstage.h"
#include <graphics/wselement.h>
#include <hal.h>
#include "openwfcwrapper.h"
#include "panic.h"
#include "utils.h"
#include "rsdisplaychangeao.h"
#include <graphics/wsdisplaycontrol.h>
#include <graphics/wsdisplaymapping.h>
#include <graphics/wsdisplaypolicy.h>
#include "screencaptureutil.h"
#include "elementwrapper.h"
#include "alfdecoderserverclient.h"
#include "huiwscanvascommands.h"

#include "alflogger.h"

CDisplayRenderStage* CDisplayRenderStage::NewL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen, TInt aScreenNumber)
    {
    CDisplayRenderStage* stage = new(ELeave) CDisplayRenderStage(aScreenNumber);
    CleanupStack::PushL(stage);
    stage->ConstructL(aEnv, aScreen, aScreenNumber);
    CleanupStack::Pop(stage);
    return stage;
    }

CDisplayRenderStage::CDisplayRenderStage(TInt aScreenNumber)
    : iScreenNumber(aScreenNumber)
    {
    }
    
CDisplayRenderStage::~CDisplayRenderStage()
    {
    iSurfaceUpdateSession.Close();
    iUpdateRegion = NULL;
    delete iDispNotifier;
    delete iConfigNotifier;
    delete iDisplayPolicy;
    delete iOpenWfcWrapper;
    delete iScreenCaptureUtil;
    }

void CDisplayRenderStage::ConstructL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber)
    {
    BaseConstructL();
    
    MWsIniFile* iniFile = aEnv->ObjectInterface<MWsIniFile>();
    STD_ASSERT_ALWAYS(iniFile, EPluginPanicIniFileMissing);

    //Initialise display policy and strategy segments
    iDisplayPolicy = CDisplayPolicy::NewL(aEnv,aScreen, aScreenNumber);
    iDisplayPolicyEnabled = iDisplayPolicy->PolicyEnabled();
    // initialize compositor
    iOpenWfcWrapper = COpenWfcWrapper::NewL(aScreenNumber, iDisplayPolicy);
    iOpenWfcWrapperDisplayControl = iOpenWfcWrapper->GetInterface<MWsDisplayControl>();
    if(iOpenWfcWrapperDisplayControl)
        {
        iDispNotifier = CRsDisplayChangeNotifier::NewL(iOpenWfcWrapperDisplayControl, iDisplayPolicy);  
        iDispNotifier->IssueNotificationRequest();
        iConfigNotifier = CRsConfigChangeNotifier::NewL(iOpenWfcWrapperDisplayControl);
        iConfigNotifier->IssueNotificationRequest();
        
        //calculate min UI buffer size depending on physical resolutions
        TInt ret = iOpenWfcWrapperDisplayControl->NumberOfResolutions();
        RArray<MDisplayControlBase::TResolution> resolutions;
        CleanupClosePushL(resolutions);
        if(ret > 0)
            {
            User::LeaveIfError(resolutions.Reserve(ret));
            User::LeaveIfError(iOpenWfcWrapperDisplayControl->GetResolutions(resolutions));
            }
        iDisplayPolicy->CalculateMinBufferSize(resolutions, ret);               
        CleanupStack::PopAndDestroy(&resolutions);
        }
        
    TSize screenSize = iOpenWfcWrapper->ScreenSize();
    iDisplayPolicy->AdjustStageBufferSize(TSize(0,0), screenSize);
    
    User::LeaveIfError(iSurfaceUpdateSession.Connect());
    
    TDisplayConfiguration displayConfig;
    if(iOpenWfcWrapperDisplayControl)
        {
        iOpenWfcWrapper->GetConfiguration(displayConfig);
        }
    else
        {
        displayConfig.SetResolution(SizeInPixels());    // Both getters eventually reach RDisplayChannel
        displayConfig.SetResolutionTwips(SizeInTwips());// as iOpenWfcWrapper->GetConfiguration does
        const MWsScene::TSceneRotation rotation = iOpenWfcWrapper->ScreenRotation();
        displayConfig.SetRotation(static_cast<TDisplayConfiguration::TRotation>(rotation));
        }
    iDisplayPolicy->SetCompositionInfo(displayConfig,displayConfig);    //1 to 1 mapping
    }

TAny* CDisplayRenderStage::ResolveObjectInterface(TUint aTypeId)
    {
    switch(aTypeId)
        {
    case MWsScreenDevice::EWsObjectInterfaceId:
        return static_cast<MWsScreenDevice*>(this);
    case MWsScene::EWsObjectInterfaceId:
        return static_cast<MWsScene*>(this);
    case MWsDisplayControl::EWsObjectInterfaceId:
//#ifndef FAKE_DISPLAY_CONFIGURATION_FOR_SINGLE_DISPLAY
//      if (iOpenWfcWrapperDisplayControl)
//#endif
//          {   //only support the interface if the compositor does
                //otherwise we need to fake single-element support.
            return static_cast<MWsDisplayControl*>(this);
//          }
//      break;
    case MWsDisplayMapping::EWsObjectInterfaceId:
        return static_cast<MWsDisplayMapping*>(this);
    case MWsWindowTreeObserver::EWsObjectInterfaceId:
        return static_cast<MWsWindowTreeObserver*>(this);
    case KAlfCompositionControllerIfUid:
        return static_cast<MAlfCompositionController*>(this);    
    case MWsDisplayPolicy::EWsObjectInterfaceId:
            {
            if (iDisplayPolicyEnabled)
                {
                return static_cast<MWsDisplayPolicy*>(this);
                }
            else
                {
                return NULL;
                }
            }   
        }

    return CWsRenderStage::ResolveObjectInterface(aTypeId);
    }

void CDisplayRenderStage::Begin(const TRegion* aRegion)
    {
    STD_ASSERT_DEBUG(!aRegion || !aRegion->CheckError(), EPluginPanicRegionError);
    iUpdateRegion = aRegion;
    iOpenWfcWrapper->Begin(aRegion);
    }
    
void CDisplayRenderStage::End(TRequestStatus* aFinished)
    {
  if (!iOpenWfcWrapper->CompositionDue() && aFinished) 
      {
        // No re-composition required and the update region is empty.
      // Signal that we are ready for more updates.
        *aFinished = KRequestPending;
        User::RequestComplete(aFinished, KErrNone);
        }
   else
      {
      iOpenWfcWrapper->Compose(aFinished);
      }
    
    iOpenWfcWrapper->End();
    iUpdateRegion = NULL;
    }

TInt CDisplayRenderStage::ScreenNumber() const
    {
    return iScreenNumber;
    }

TDisplayMode CDisplayRenderStage::DisplayMode() const
    {
    return EColor16MAP;//iRenderTarget->DisplayMode();
    }

TSize CDisplayRenderStage::SizeInPixels() const
    {
    TSize size(0, 0);
    if (iOpenWfcWrapperDisplayControl)
        {
        size=iDisplayPolicy->GetUiResolution();
        }
    return size;
    }

TSize CDisplayRenderStage::SizeInTwips() const
    {  
    if (iOpenWfcWrapperDisplayControl)
        {
        TSize size(0, 0);
        size = iDisplayPolicy->GetUiResolutionAsTwips();
        if (size.iWidth)
            {
            return size;
            }     
        }
    const TSize size = SizeInPixels();
    const MWsScene::TSceneRotation screenRotation = iOpenWfcWrapper->ScreenRotation();
    const TBool rotated = (screenRotation == ESceneAntiClockwise90 || screenRotation == ESceneAntiClockwise270);
    
    TInt twips = 0;
    TInt res = HAL::Get(iScreenNumber, rotated ? HALData::EDisplayYTwips : HALData::EDisplayXTwips,twips);
    __ASSERT_DEBUG(res==KErrNone && twips!=0, Panic(EPluginPanicSizeInTwipsFailed));
    const TInt horzTwipsPerThousandPixels = twips * 1000 / size.iWidth; 
    const TInt horzSizeInTwips = (horzTwipsPerThousandPixels * size.iWidth + 500) / 1000;
    
    twips = 0;
    res = HAL::Get(iScreenNumber, rotated ? HALData::EDisplayXTwips : HALData::EDisplayYTwips,twips);
    __ASSERT_DEBUG(res==KErrNone && twips!=0, Panic(EPluginPanicSizeInTwipsFailed));
    const TInt vertTwipsPerThousandPixels = twips * 1000 / size.iHeight; 
    const TInt vertSizeInTwips = (vertTwipsPerThousandPixels * size.iHeight + 500) / 1000;

    return TSize (horzSizeInTwips, vertSizeInTwips);
    }

TRect CDisplayRenderStage::PointerRect() const
    {
    if (iScreenNumber != 0)
        {
        return TRect();  // Only screen 0 supports a digitizer
        }

    TMachineInfoV1Buf macInfo;
    UserHal::MachineInfo(macInfo);
    return TRect(-macInfo().iOffsetToDisplayInPixels, macInfo().iXYInputSizeInPixels);    
    }

void CDisplayRenderStage::GetPixel(TRgb& /*aColor*/, const TPoint& /*aPixel*/) const
    {
    //iRenderTarget->GetPixel(aColor, aPixel);
    }

void CDisplayRenderStage::GetScanLine(TDes8& /*aScanLine*/, const TPoint& /*aStartPixel*/, TInt /*aPixelLength*/, TDisplayMode /*aDispMode*/) const
    {
    //iRenderTarget->GetScanLine(aScanLine, aStartPixel, aPixelLength, aDispMode);
    }

TBool CDisplayRenderStage::RectCompare(const TRect& /*aRect1*/, const TRect& /*aRect2*/) const
    {
    return EFalse;//iRenderTarget->RectCompare(aRect1, aRect2);
    }

void doDynamicCaptureL(CDisplayRenderStage* aThis, CFbsBitmap* aBitmap)
    {
    if (!aThis->iScreenCaptureUtil)
        {
        aThis->iScreenCaptureUtil = CScreenCaptureUtil::NewL(aThis);
        }   
    aThis->iScreenCaptureUtil->ComposeScreenL(aBitmap->Handle());
    }
    
void CDisplayRenderStage::CopyScreenToBitmapL(CFbsBitmap* aBitmap, const TRect& /*aRect*/) const
    {
  if (iRunningOnLowMem)
        {
    User::Leave(KErrNoMemory);
    }
    
  if ( aBitmap->ExtendedBitmapType() != KNullUid )
    {
    User::Leave(KErrArgument);
    }            

    doDynamicCaptureL((CDisplayRenderStage*)this, aBitmap);
    }

MWsElement* CDisplayRenderStage::CreateSceneElementL()
    {
    return iOpenWfcWrapper->CreateElementL();
    }

void CDisplayRenderStage::DestroySceneElement(MWsElement* aElement)
    { 
    iOpenWfcWrapper->DestroyElement(aElement);
    }

TInt CDisplayRenderStage::InsertSceneElement(MWsElement* aInsertElement, MWsElement* aSubordinateElement)
    {
	if (aInsertElement)
	    {
#ifdef USE_UI_SURFACE_FOR_ALF
        TUint32 flags(0);
        aInsertElement->GetRenderStageFlags(flags);
        if ( flags & MWsElement::EElementIsIndirectlyRenderedUserInterface)
            {
            iUiElement = aInsertElement;
	        if (!iAlfElement)
	            {
	            __ALFLOGSTRING("CDisplayRenderStage: Skipping UI element for now"); 
	            return KErrNone;
	            }
            }
         }
#endif        
    return iOpenWfcWrapper->AddElement(aInsertElement, aSubordinateElement);
    }

TInt CDisplayRenderStage::RemoveSceneElement(MWsElement* aRemoveElement)
    {
    iOpenWfcWrapper->RemoveElement(aRemoveElement);
    return KErrNone;
    }

void CDisplayRenderStage::ComposePendingScene(TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted)
    {
    /*
     * If the supplied off-screen target is NULL and If the optionally provided TRequestStatus
     * is present, signal immediately and return.
     */
    if (aOffScreenTarget.IsNull())
        {
        if (aCompleted)
            {           
            User::RequestComplete(aCompleted, KErrArgument);
            return;
            }
        }

    iOpenWfcWrapper->RegisterSurface(aOffScreenTarget);
    iOpenWfcWrapper->Compose(aOffScreenTarget, aCompleted);
    iOpenWfcWrapper->UnregisterSurface(aOffScreenTarget);
    }

TInt CDisplayRenderStage::SetSceneRotation(const MWsScene::TSceneRotation aSceneRotation)
    {
    const TSceneRotation oldRotation = iOpenWfcWrapper->ScreenRotation();

    //rotate in compositor 
    iOpenWfcWrapper->SetScreenRotation(aSceneRotation);

/*  switch (aSceneRotation)
        {
        case ESceneAntiClockwise0:
        case ESceneAntiClockwise180:
            iRenderTarget->SetAspectRatio(CRenderTarget::ENormalAspectRatio);
            break;
        default:
            iRenderTarget->SetAspectRatio(CRenderTarget::EInvertedAspectRatio);
        }
*/      
    TDisplayConfiguration rotate;
    rotate.SetRotation((TDisplayConfiguration::TRotation)aSceneRotation);
    iDisplayPolicy->SetCompositionInfo(rotate,rotate);
    //update ui element
    //const TSurfaceId& uiSurface = iRenderTarget->SurfaceId();
    //return iOpenWfcWrapper->SetUiSurface(uiSurface);
    return KErrNone;
    }

MWsScene::TSceneRotation CDisplayRenderStage::SceneRotation() const
    {
    return iOpenWfcWrapper->ScreenRotation();
    }

void CDisplayRenderStage::ClearDisplay(TRgb /*aBackColor*/)
    {
    }
//
//
// Implementation of MWsDisplayConfiguration/MDisplayConfiguration interface
//
//


void CDisplayRenderStage::NotifyOnDisplayChange(TRequestStatus& aStatus)
    {
    aStatus = KRequestPending;
    if(iDispNotifier)
        {
        iDispNotifier->RegisterActiveStatus(aStatus);
        }
    else
        {
        TRequestStatus* reqPtr=&aStatus;    //indirection is required
        User::RequestComplete(reqPtr, KErrNotSupported);
        }
    }

void CDisplayRenderStage::NotifyOnDisplayChangeCancel()
    {
    if(iDispNotifier)
        {
        iDispNotifier->CancelNotificationRequest();
        }
    }

void CDisplayRenderStage::NotifyOnConfigChange(TRequestStatus& aStatus)
    {
    aStatus = KRequestPending;
    if(iConfigNotifier)
        {
        iConfigNotifier->RegisterActiveStatus(aStatus);
        }
    else
        {
        TRequestStatus* reqPtr=&aStatus;    //indirection is required
        User::RequestComplete(reqPtr, KErrNotSupported);
        }
    }

void CDisplayRenderStage::NotifyOnConfigChangeCancel()
    {
    if(iConfigNotifier)
        {
        iConfigNotifier->CancelNotificationRequest();
        }
    }

/** Determine the display configuration and application rendering
    offset to use for a given screen size mode.

    The Window Server uses this function when a request to change to a
    new screen size mode or new display configuration is made by a
    client. The display configuration passed in may be partially filled,
    and the combination of the defined entries in it and the requested
    screen size mode values shall be used to define the rest of the
    configuration and new application offset to be used.

    Only undefined entries in the configuration may be set by this
    function. Entries already defined must retain the same values. If
    the combination of defined entries and size mode values is not
    acceptable to the implementer, the function shall return EFalse.

    If the function returns ETrue, the Window Server shall set the
    returned display configuration, notify render stages of the new
    application offset and change to use the screen size mode. If
    either this function returns EFalse or the call to set the display
    configuration fails for any reason (such as KErrNoMemory, for
    example), then no further changes are made.

    @see MWsScreenConfigList

    @param aScreenSizeMode    The requested screen size mode. If static,
    the size in pixels, size in twips, and the rotation must be
    respected. If dynamic, only the rotation must be respected.
    @param aConfig    The requested display configuration on input, the
    required configuration on return. Only undefined entries may be set.

    @param aSizeModePosition    The screen size mode offset and size to be filled in
    
    @return KErrNone if no error, otherwise a system wide error code
*/
TInt CDisplayRenderStage::GetSizeModeConfiguration(TInt aScreenSizeMode, TDisplayConfiguration& aConfig, TRect& aSizeModePosition) const
    {
    TInt reply;
    RArray<TResolution> resolutions;
    reply = iOpenWfcWrapperDisplayControl->GetResolutions(resolutions);
    if (reply < KErrNone &&
            reply != KErrDisconnected)
        {
        resolutions.Close();
        return reply;
        }
    TBool isDisconnected=EFalse;
    if (reply == KErrDisconnected || resolutions.Count() == 0)
        {
        isDisconnected=ETrue;
        }
    else if (resolutions.Count() == 1 && resolutions[0].iPixelSize==TSize(0,0))
        {
        isDisconnected=ETrue;
        }
    
    if (isDisconnected)
        {//in this case aScreenSizeMode should be the smallest app mode
        reply = iDisplayPolicy->GetSizeModeConfiguration(aScreenSizeMode, aConfig, aSizeModePosition);
        }
    else
        {
        reply = iDisplayPolicy->GetSizeModeConfiguration(resolutions,aScreenSizeMode,aConfig,aSizeModePosition);
        }

    resolutions.Close();
    return reply;
    }

void CDisplayRenderStage::SetSizeModeExtent(TRect& aExtent, TBitFlags32 aContext)
    {
    if (aContext.iFlags&KOffMap)
        {
        iDisplayPolicy->SetSizeModeExtent(aExtent,aContext);
        aContext.iFlags&=~KOffMap;
        }
/*  if (aContext.iFlags&KOffGc)
        {
        iRenderTarget->SetDrawDeviceOffset(aExtent.iTl);
        aContext.iFlags&=~KOffGc;
        }*/
    //Could chain through again if required?
    }

TInt CDisplayRenderStage::NumberOfResolutions() const
    {
    if(iOpenWfcWrapperDisplayControl)
        {
        TInt returnValue = iOpenWfcWrapperDisplayControl->NumberOfResolutions();
        iDisplayPolicy->AddVirtualResolutionCount(returnValue);
        return returnValue;
        }
#ifdef FAKE_DISPLAY_CONFIGURATION_FOR_SINGLE_DISPLAY    
    return 1;
#else
    return KErrNotSupported;
#endif
    }

TInt CDisplayRenderStage::GetResolutions(RArray<TResolution>& aResolutions) const   
    {
    if(iOpenWfcWrapperDisplayControl)
        {
        TInt returnValue = iOpenWfcWrapperDisplayControl->GetResolutions(aResolutions);
        if (returnValue >= KErrNone)
            {
            //add virtual resolutions
            returnValue = iDisplayPolicy->AddVirtualResolutions(aResolutions);
            }
        return returnValue;
        }
    return KErrNotSupported;
    }


void CDisplayRenderStage::GetConfiguration(TDisplayConfiguration& aConfig) const    
    {
    aConfig.ClearAll();
    if(iOpenWfcWrapperDisplayControl)
        {
        iOpenWfcWrapperDisplayControl->GetConfiguration(aConfig);
        if (aConfig.IsDefined(aConfig.EResolution))
            {
            TSize uiRes=iDisplayPolicy->GetUiResolution();
            TDisplayConfiguration::TRotation rotation;
            if(aConfig.GetRotation(rotation))
                {
                if(rotation&TDisplayConfiguration::ERotation90CW)
                    {
                    TInt height = uiRes.iHeight;
                    uiRes.iHeight = uiRes.iWidth;
                    uiRes.iWidth = height;
                    }
                }
            if (uiRes.iWidth)
                {
                aConfig.SetResolution(uiRes);
                }
            }
        }
    }

TInt CDisplayRenderStage::SetConfigurationAndBuffer(const TDisplayConfiguration& /*aConfig*/,const TSize* /*aUiSize*/)
    {
/*  //May need to resize the FB buffer
    TInt result = KErrNone;
    TSize reqUiSize;
    TBool reallocBuffer=EFalse;
    TSize newFbSize;
        
    RWsOffScreenImageTarget newTarget;
    if (aUiSize)
        {
        newFbSize=*aUiSize;

        iDisplayPolicy->AdjustStageBufferSize(iRenderTarget->SizeInPixels(),newFbSize);

        //if realloc needed 
        //if (!(newFbSize == iRenderTarget->SizeInPixels()))
        //  { 
        //  reallocBuffer = ETrue;
        //  result = iRenderTarget->AllocNewTarget(newTarget,newFbSize);    //newFbSize should not be zero here.
        //  if (result < KErrNone)
        //      {
        //      newTarget.Close();
        //      return result;
        //      }
        //  }
        }
    TDisplayConfiguration displayCopy;
    iOpenWfcWrapper->GetConfiguration(displayCopy);
    if (aConfig.IsDefined(aConfig.EResolution)||aConfig.IsDefined(aConfig.ERotation))
        {
        result=iOpenWfcWrapper->FlushedSetConfiguration(aConfig);
        }
    if (result<KErrNone && result != KErrDisconnected)
        {
        TInt tempResult=iOpenWfcWrapper->FlushedSetConfiguration(displayCopy);
        STD_ASSERT_DEBUG((tempResult==KErrNone)||(tempResult==KErrDisconnected),EPluginPanicTemp);
        if (reallocBuffer)
            {
            newTarget.Close();
            }
        }
    else
        {
        if (reallocBuffer)
            {
            const TSurfaceId& uiSurface = newTarget.iImageCollections
                    [iRenderTarget->AspectRatio()].SurfaceId();
            result = iOpenWfcWrapper->SetUiSurface(uiSurface);
            if (result == KErrNone)
                {
                iRenderTarget->SwitchTarget(newTarget); //now newTarget shouldn't be closed
                }
            else
                {
                TInt tempResult = iOpenWfcWrapper->FlushedSetConfiguration(displayCopy);
                STD_ASSERT_DEBUG((tempResult==KErrNone)||(result==KErrDisconnected),EPluginPanicTemp);
                newTarget.Close();
                }
            }
        }
    return result;
*/
    return KErrNone;
    }

TInt CDisplayRenderStage::SetConfiguration(const TDisplayConfiguration& aConfig)    
    {
    if(iOpenWfcWrapperDisplayControl)
        {

        TInt returnValue = iOpenWfcWrapperDisplayControl->NumberOfResolutions();
        TBool success=EFalse;
        //Both "no resolution" and KErrDisconnected are treated as disconnected
        if(returnValue == KErrDisconnected || returnValue == 0)
            {
            //Ui Res is set to same as smallest appmode
            //compsotion res is set to zero

            TSize uiSize;
            TDisplayConfiguration emptyConvertedConfig;
            emptyConvertedConfig.SetResolution(TSize(0,0));
            aConfig.GetResolution(uiSize);
            returnValue=SetConfigurationAndBuffer(emptyConvertedConfig,&uiSize);
            iDisplayPolicy->SetCompositionInfo(emptyConvertedConfig,aConfig);
            return returnValue;
            }
        
        TDisplayConfiguration convertedConfig(aConfig);
        RArray<TResolution> resolutions;
        returnValue = resolutions.Reserve(returnValue);
        
        if (returnValue < KErrNone)
            {
            resolutions.Close();
            return returnValue;
            }
        returnValue = iOpenWfcWrapperDisplayControl->GetResolutions(resolutions);
        
        if (returnValue < KErrNone)
            {
            resolutions.Close();
            return returnValue;
            }
        success = iDisplayPolicy->SettingConfiguration(resolutions,convertedConfig);    //if virtual, converts to real
        resolutions.Close();

        TSize uiSize;
        TBool isUiSize = aConfig.GetResolution(uiSize);
        if (success)
            {
            TInt reply;
            if (isUiSize)
                {
                reply=SetConfigurationAndBuffer(convertedConfig,&uiSize);
                }
            else
                {
                reply=SetConfigurationAndBuffer(convertedConfig,NULL);
                }
            if (reply>=KErrNone)
                {
                iDisplayPolicy->SetCompositionInfo(convertedConfig,aConfig);
//              iBitGcWrapper->BitGc().Activate(iScreenDevice);
                }
            return reply;
            }
        else
            {
            TSize tempSize;
            
            if (!convertedConfig.GetResolution(tempSize))
                {   //signals the config is totally invalid and should not be used
                return KErrArgument;
                }
            TInt reply;
            if (isUiSize)
                {
                reply=SetConfigurationAndBuffer(convertedConfig,&uiSize);
                }
            else
                {
                reply=SetConfigurationAndBuffer(convertedConfig,NULL);
                }
            if (reply>=KErrNone)
                {
                iDisplayPolicy->SetCompositionInfo(aConfig,aConfig);
//              iBitGcWrapper->BitGc().Activate(iScreenDevice);
                }
            return reply;
            }
        }
    else
        {   //This will never work when faked
        return KErrNotSupported;
        }
    }

TInt CDisplayRenderStage::PreferredDisplayVersion() const   
    {
    if(iOpenWfcWrapperDisplayControl)
        {
        return iOpenWfcWrapperDisplayControl->PreferredDisplayVersion();
        }
    else
        {
#ifdef FAKE_DISPLAY_CONFIGURATION_FOR_SINGLE_DISPLAY    
        return TDisplayConfiguration().Version();
#else
        return KErrNotSupported;
#endif
        }
    }
TInt CDisplayRenderStage::MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const
    {
    return iDisplayPolicy->MapCoordinates(aSourceSpace, aSource, aTargetSpace, aTarget);
    }

void CDisplayRenderStage::NewAppModesAvailable()
    {
    iDisplayPolicy->NewAppModesAvailable();
    }

TInt CDisplayRenderStage::SuitableAppMode(TDisplayStatus aSituation)
    {
    return iDisplayPolicy->SuitableAppMode(aSituation);
    }

void CDisplayRenderStage::SetLastAppMode(TInt aMode)
    {
    iDisplayPolicy->SetLastAppMode(aMode);
    }

TInt CDisplayRenderStage::RegisterSurface(const TSurfaceId& aSurface)
    {
    return iOpenWfcWrapper->RegisterSurface(aSurface);
    }

TInt CDisplayRenderStage::UnregisterSurface(const TSurfaceId& aSurface)
    {
    return iOpenWfcWrapper->UnregisterSurface(aSurface);
    }

// ---------------------------------------------------------------------------
// EnableAlpha
// ---------------------------------------------------------------------------
//
TInt CDisplayRenderStage::EnableAlpha(TBool aEnable)
    {
    __ALFLOGSTRING2("CDisplayRenderStage::EnableAlpha - iTargetElement: %d, aEnable: %d", iTargetElement, aEnable);
    if ( iTargetElement )
        {
	    TUint32 flags = 0;
	    iTargetElement->GetTargetRendererFlags(flags);
        if ( aEnable )
            {
            flags |= MWsElement::EElementTransparencySource;
            }
        else
            {
            flags ^= MWsElement::EElementTransparencySource;
            }
	    iTargetElement->SetTargetRendererFlags(flags);
        TRAP_IGNORE(ElementUseAlphaFlagChangedL(*iTargetElement, aEnable));
        }
    return KErrNone;
    }


void CDisplayRenderStage::ElementUseAlphaFlagChangedL(MWsElement& aElement, TBool aEnabled)
    {
    TElementData elementData(&aElement, TSurfaceId::CreateNullId(), 0, 0, 0);
    TInt index = iElementArray.Find(elementData);
    if ( index != KErrNotFound && iElementArray[index].iWindowTreeNodeId!=0 )
        {      
        TInt array[] = {
          iElementArray[index].iWindowTreeNodeId,
          aEnabled };    
        iAlfBridge->WriteIntsL( 
            EAlfNodeLayerUsesAlphaFlagChanged,
            2,
            array
            );
        iAlfBridge->CommitL();    
        __ALFLOGSTRING1("CDisplayRenderStage::ElementUseAlphaFlagChanged - aEnabled %d", aEnabled);
        }
    }


// ---------------------------------------------------------------------------
// SetOpacity
// ---------------------------------------------------------------------------
//
TInt CDisplayRenderStage::SetOpacity(TReal32 aOpacity)  __SOFTFP
    {
    TInt error = KErrNone;
    TInt opacity = 255*aOpacity;
    __ALFLOGSTRING3("CDisplayRenderStage::SetOpacity - iTargetElement: %d, aOpacity %f, opacity %d", iTargetElement, aOpacity, opacity );
    if (iTargetElement)  
	    {
	    TUint32 flags = 0;
	    iTargetElement->GetTargetRendererFlags(flags);
	    flags |= MWsElement::EElementTransparencyGlobalAlpha;
	    iTargetElement->SetTargetRendererFlags(MWsElement::EElementTransparencyGlobalAlpha);
	    iTargetElement->SetGlobalAlpha(opacity);
        }
    // Todo: Could do some real magic based for 0..    
    return error;
    }

// ---------------------------------------------------------------------------
// SetRotation, Not in degrees !!!
// ---------------------------------------------------------------------------
//
TInt CDisplayRenderStage::SetRotation(TInt aRotation)
    {
    __ALFLOGSTRING2("CDisplayRenderStage::SetRotation - iTargetElement: %d, aRotation: %d ", iTargetElement, aRotation);

    MWsElement::TElementRotation rotation =
            static_cast<MWsElement::TElementRotation> (aRotation);

    if (iTargetElement)
        {
        iTargetElement->SetSourceRotation(rotation);
        }
        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// SetExtent
// ---------------------------------------------------------------------------
//
TInt CDisplayRenderStage::SetExtent(
        const TRect& aRect, 
#ifdef _ALF_LOGGING
        TInt aScreen)
#else
		TInt)
#endif				
    {
    __ALFLOGSTRING2("CDisplayRenderStage::SetExtent - %d %d  ",aRect.iTl.iX,aRect.iTl.iY );
    __ALFLOGSTRING2("CDisplayRenderStage::SetExtent - %d %d  ",aRect.iBr.iX,aRect.iBr.iY );
#ifdef _ALF_LOGGING
    __ALFLOGSTRING2("CDisplayRenderStage::SetExtent - iTargetElement: %d, aScreen: %d  ", iTargetElement, aScreen );
#endif                  
    __ALFLOGSTRING1("CDisplayRenderStage::SetExtent - iScreenNumber %d  ", iScreenNumber);

    TInt error = KErrNone;
    TBool composeNeeded = EFalse;
    if (iTargetElement)
        {
        iTargetElement->SetDestinationRectangle(aRect);
        iTargetElement->SetDestinationClippingRect(aRect);
        iExtentSurfaceId.CreateNullId(); // was not used -> reset
        }
    // window is not found from this screen, handle as extent without window if extent surface is set
    // Todo.    
    return error;
    }

// ---------------------------------------------------------------------------
// SetIsBackgroundAnim
// ---------------------------------------------------------------------------
//
void CDisplayRenderStage::SetIsBackgroundAnim(TBool aValue )
    {
    if( iTargetElement )
        {
        iTargetElement->SetIsBackgroundAnim(aValue);
        }
    }


TInt CDisplayRenderStage::SetSourceRect(const TRect& aRect)
    {
    if( iTargetElement )
        {
        iTargetElement->SetSourceRectangle(aRect);
        }
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// CreateTargetL
// ---------------------------------------------------------------------------
//
TInt CDisplayRenderStage::CreateTargetL(TInt aTarget, TInt aWsHandle, TInt aGroupHandle)
    {
    __ALFLOGSTRING2("CDisplayRenderStage::CreateTargetL - : %d, %d", aWsHandle, aGroupHandle );
    TInt result(KErrNotFound);
    // find the element for this window
    CElementWrapper* element = NULL;
    
    for (TInt i = 0; i < iElementArray.Count(); i++)
        {
        RDebug::Print(_L("CDisplayRenderStage::CreateTargetL - %d - %d - %d"), iElementArray[i].iWindowHandle, iElementArray[i].iWindowGrpId, iElementArray[i].iWindowTreeNodeId );

        if (iElementArray[i].iWindowHandle == aWsHandle
            && iElementArray[i].iWindowGrpId == aGroupHandle)
            {
            element = dynamic_cast<CElementWrapper*>(iElementArray[i].iElement);
            break;
            }
        }
        
    if (element)
        {
        SElementTarget elementtarget;
        elementtarget.iTarget = aTarget;
        elementtarget.iElement = element;
        result = iElementTargets.Append(elementtarget);

        // todo: is there need to lock target so that wserv does not accidentally overwrite the state from client

        __ALFLOGSTRING1("CDisplayRenderStage::CreateTargetL - client composition element count %d ", iElementTargets.Count() );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// SetTargetL
// ---------------------------------------------------------------------------
//
void CDisplayRenderStage::SetTargetL(TInt aTarget)
    {
    __ALFLOGSTRING1("CDisplayRenderStage::SetTargetL - %d ", aTarget );
    CElementWrapper* tmp = FindElementTarget(aTarget);
    if (tmp)
        {
        __ALFLOGSTRING1("CDisplayRenderStage::SetTargetL - Found target ", aTarget );
        iTargetElement = tmp;
        }
    else
        {
        if (iTargetElement)
            { // flush changes to composition when target is released
            iTargetElement->FlushSceneChanges();
            }
        iTargetElement = NULL;
        __ALFLOGSTRING1("CDisplayRenderStage::SetTargetL - Target was not found!", aTarget );
        }
    }

void CDisplayRenderStage::SetTarget(MWsElement* aElement)
  {
  if (iTargetElement && !aElement)
    { // flush changes when target is "freed"
    iTargetElement->FlushSceneChanges();
    }

  //__ASSERT_DEBUG(!iTargetElement, USER_INVARIANT())
  iTargetElement = dynamic_cast<CElementWrapper*>(aElement);
  }  

// ---------------------------------------------------------------------------
// SetExtentSurfaceId
// ---------------------------------------------------------------------------
//
void CDisplayRenderStage::SetExtentSurfaceId(const TSurfaceId& aSurfaceId)
    {
    iExtentSurfaceId = aSurfaceId;
    }
// ---------------------------------------------------------------------------
// SetOrder
// ---------------------------------------------------------------------------
//
TInt CDisplayRenderStage::SetOrder(TInt /*aTarget*/, TInt /*aTop*/, TBool /*aAbove*/, TBool /*aCombineTargets*/)
    {
    TInt newKey = KErrNone;
    // ToDo  
    return newKey;
    }

// ---------------------------------------------------------------------------
// DeleteTarget
// ---------------------------------------------------------------------------
//
void CDisplayRenderStage::DeleteTarget(TInt aTarget)
    {
    __ALFLOGSTRING1("CDisplayRenderStage::DeleteTarget - %d ", aTarget );
    // try to remove from the element targets
    MWsElement* element = FindElementTarget(aTarget);
    if(element)
        {
        // Todo: Should we restore some flags / state?
        }
    
    for( TInt x = 0; x < iElementTargets.Count() ; x++)
        {
        if( iElementTargets[x].iTarget == aTarget )
            {
            iElementTargets.Remove(x);
            break;
            }
        }

    __ALFLOGSTRING1("CDisplayRenderStage::DeleteTarget - client composition element count %d ", iElementTargets.Count() );
    }

void CDisplayRenderStage::LowOnGraphicsMemory()
    {
    __ALFLOGSTRING("void CDisplayRenderStage::LowOnGraphicsMemory()");    
    iRunningOnLowMem = ETrue;
    delete iScreenCaptureUtil;
    iScreenCaptureUtil = 0;
    }

void CDisplayRenderStage::GraphicsMemoryGood()
    {
    __ALFLOGSTRING("void CDisplayRenderStage::GraphicsMemoryGood()");    
    iRunningOnLowMem = EFalse;
    }

void CDisplayRenderStage::AlfBridgeCallback(TInt aEvent, TAny* aData)
    {
    switch(aEvent)
      {
      case MAlfBridge::EAlfBridgeCreated:
        iAlfBridge = (MAlfBridge*) aData;
        return;
      case MAlfBridge::EAlfNativeWindowCreated:
        AlfWindowListenerCallback((TAlfNativeWindowData*)aData);
        return;
      case MAlfBridge::ESetWindowTreeObserver:
        iWindowTreeObserver = (MAlfCompositionAgnosticWindowTreeObserver*)aData;
        return;
    default:
      break;
      }
    }
// ---------------------------------------------------------------------------
// AlfWindowListenerCallBack
// ---------------------------------------------------------------------------
//
#ifdef USE_UI_SURFACE_FOR_ALF
void CDisplayRenderStage::AlfWindowListenerCallback(TAlfNativeWindowData*)
    {
#else
void CDisplayRenderStage::AlfWindowListenerCallback(TAlfNativeWindowData* aData)
    {
    // Got window and group id from Alf. 
	iAlfElement = FindElement(aData->iAlfWindowGrpId, aData->iAlfWindowHandle);
	if (iAlfElement)
	    {
	    if (!(iAlfElement->ConnectedSurface().IsNull()))
	        {
            // Found Alf Egl surface. Enable alpha on it to be able to create transparent holes.
            SetTarget(iAlfElement);
            EnableAlpha(); 
            SetOpacity(1.0f);
            SetTarget(0);
            iOpenWfcWrapper->SetUiSurface(iAlfElement->ConnectedSurface())
            }
	    }
#endif
    }    

MWsElement* CDisplayRenderStage::FindElement(TInt aWindowGrpId, TInt aWindowHandle)
    {
    for(TInt index = 0; index < iElementArray.Count(); ++index)
        {
        if( (iElementArray[index].iWindowGrpId == aWindowGrpId)
        	&& (iElementArray[index].iWindowHandle == aWindowHandle))
            {
            return iElementArray[index].iElement;
            }
        }
    return NULL;
    }
      
void CDisplayRenderStage::ElementAdded(const MWsWindowTreeNode& aWindowTreeNode, const MWsElement& aElement)
    {
    __ALFLOGSTRING1("CDisplayRenderStage::ElementAdded. iScreenNumber=%d", iScreenNumber);        

   	const MWsWindow* window = aWindowTreeNode.Window();
    TUint32 windowId = window->Handle();
    TInt windowGroupId = 0;             
    const MWsWindowTreeNode* parentWindowGroup = aWindowTreeNode.ParentNode();
    while (parentWindowGroup)
        {
        if (parentWindowGroup->NodeType() == MWsWindowTreeNode::EWinTreeNodeGroup)
            {
            windowGroupId = parentWindowGroup->WindowGroup()->Identifier();
#ifdef USE_UI_SURFACE_FOR_ALF
            if (iUiElement->ConnectedSurface() == TSurfaceId::CreateNullId() && 
                parentWindowGroup->WindowGroup()->Client() && 
                parentWindowGroup->WindowGroup()->Client()->SecureId() == RProcess().SecureId()) // ALF
                {
                iAlfElement = (MWsElement*) &aElement;
	            if (!(iAlfElement->ConnectedSurface().IsNull()))
	                {
                    // Found Alf Egl surface. Enable alpha on it to be able to create transparent holes.
                    SetTarget(iAlfElement);
                    EnableAlpha();
                    SetOpacity(1.0f);
                    SetTarget(0);
                    iOpenWfcWrapper->SetUiSurface(iAlfElement->ConnectedSurface());
                    if (iUiElement)
                        {
                        iUiElement->ConnectSurface(iAlfElement->ConnectedSurface());
                        RemoveSceneElement(iAlfElement);
                        InsertSceneElement(iUiElement,0);
                        iAlfElement = iUiElement;
                        // Todo: do we need to compose manually to make change happen immediately
                        return;                     
                        }
                     }
                }                
#endif // #ifdef USE_UI_SURFACE_FOR_ALF

            break;    
            }
        parentWindowGroup = parentWindowGroup->ParentNode();
        }                        
    
    // Store window handle and surface Id in an array
    TInt index = KErrNotFound;
    TUint32 nodeId = (TUint32)&aWindowTreeNode;
    for(TInt i = 0; i < iElementArray.Count(); ++i)
        {
        if(iElementArray[i].iSurfaceId == aElement.ConnectedSurface())
            {
            iElementArray[i].iWindowGrpId = windowGroupId;
            iElementArray[i].iWindowHandle = windowId;
            iElementArray[i].iWindowTreeNodeId = nodeId;
            index = i;
            break;
            }
        }

    __ALFLOGSTRING1("DisplayRenderStage::ElementAdded. index=%d", index);
	if (index == KErrNotFound)
		{
		ASSERT(EFalse);
		return; // error
		}
	

	//SetTarget(iElementArray[index].iElement); // Usefull for debugging
    //SetOpacity(0.5f);
    //SetTarget(0);

    __ALFLOGSTRING1("--aElement: %x", iElementArray[index].iElement);
    __ALFLOGSTRING1("--iSurfaceId: %d", iElementArray[index].iSurfaceId);
    __ALFLOGSTRING1("--iWindowHandle: %d", iElementArray[index].iWindowHandle);
    __ALFLOGSTRING1("--iWindowTreeNodeId: %d", iElementArray[index].iWindowTreeNodeId);

	// Send EAlfNodeLayerAdded command that will set the surface extent to the visual
	// Note : it is not necessary to send EAlfNodeLayerAdded  command in Alf EGL surface Element case
	//        but it should not do any harm either.
    TRect extent;
    aElement.GetDestinationRectangle( extent );
    TInt array[] = {        
            nodeId,
            extent.iTl.iX,
            extent.iTl.iY,
            extent.iBr.iX,
            extent.iBr.iY
            }; 
    iAlfBridge->WriteIntsL( 
    	EAlfNodeLayerAdded,
        5,
        array );
	iAlfBridge->CommitL();

	// Do compose. This is required to get newly created surface in composition right away.
	// Otherwise, if the surface would not be updated to the composition scene here all surface updates would fail 
	// until next normal compose call. 
    //(void)CAlfComposeObject::CreateAndStartL(*iGceWrapper);
	
    __ALFLOGSTRING("CDisplayRenderStage::ElementAdded - End");  
    }
    
CElementWrapper* CDisplayRenderStage::FindElementTarget( TInt aTarget )
    {
    TInt i = 0;
    for(  ; i < iElementTargets.Count() ; i++ )
        {
        if( iElementTargets[i].iTarget == aTarget )
            {
            break;
            }
        }
    if( i < iElementTargets.Count())
        {
        return iElementTargets[i].iElement;
        }
    return NULL;
    }
