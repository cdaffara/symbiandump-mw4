// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <eiknotapi.h>
#include <AknUtils.h>
#include <AknLayoutFont.h>
#include <AknBidiTextUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <ecom/implementationproxy.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknWsEventObserver.h>
#include <apgwgnam.h> 
//#include <aknappui.h> 
#include "alfappfwproxyops.h"
#include "huiskinbackroundlayout.h"

// hollow notifier implementation to get access on AppFw inside eikon server
class CAlfAppFwNotifier : public CBase, public MEikSrvNotifierBase2, public MCoeMessageMonitorObserver	
    {
public:    
    ~CAlfAppFwNotifier()
        {
        delete iBitmap;
        delete iTempSkinBmp;
        delete iTempMaskBmp;
        delete iSkinControlContext;
        }
    
    void MonitorWsMessage(const TWsEvent& aEvent)
        {
        if (aEvent.Type() == EEventWindowGroupListChanged)
            {
            UpdateWgListL();
            }
       else if (aEvent.Type() == KEikDynamicLayoutVariantSwitch ||
                aEvent.Type() == KEikInputLanguageChange)
           {
           if (iLayoutMirrored != AknLayoutUtils::LayoutMirrored())
               {
               iLayoutMirrored  = AknLayoutUtils::LayoutMirrored();
               if (iLayoutMirroredMessage.IsNull())
                    { 
                    iLayoutMirroredUpdated = ETrue;
                    }
               else
                    {
                    iLayoutMirroredMessage.Complete(iLayoutMirrored);
                    iLayoutMirroredUpdated = EFalse;
                    }      
                } 
            }
        }        
    
    CAlfAppFwNotifier(
        TUid aNotifierUid, 
        TUid aChannel, 
        TInt aPriority )
        : iWsSession(CCoeEnv::Static()->WsSession())
        {
        iInfo.iUid = aNotifierUid;
        iInfo.iChannel= aChannel;
        iInfo.iPriority = aPriority;
        *iWgIdArray = -1; // indicates array uninitialized
        iLayoutMirrored  = AknLayoutUtils::LayoutMirrored();
        iLayoutMirroredUpdated = ETrue;    
        CCoeEnv::Static()->AddMessageMonitorObserverL(*this);
        CCoeEnv::Static()->RootWin().EnableScreenChangeEvents();
        }        
   
public:
    // from MEikSrvNotifierBase2
    TPtrC8 UpdateL(const TDesC8&)
        {
        return KNullDesC8();
        }
   
   void Cancel()
        {
        }
        
    void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
        {
        TInt err = KErrNone;
        TAlfCommandParams* params =  (TAlfCommandParams*) aBuffer.Ptr();
        switch (params->iCommand)
            {
            case EAlfIsMirrorred:
                {
                if (iLayoutMirroredUpdated)
                    {
                    aMessage.Complete(iLayoutMirrored);
                    iLayoutMirroredUpdated = EFalse;
                    }    
                else
                    {            
                    iLayoutMirroredMessage = aMessage;
                    }
                return;    
                }            
            case EAlfCbaLocation:
                {
                err = AknLayoutUtils::CbaLocation();
                break;
                }            
            case EAlfLayoutMetricsRect:
                {
                TRect rect(0,0,0,0);
                AknLayoutUtils::LayoutMetricsRect((AknLayoutUtils::TAknLayoutMetrics)params->iInt1, rect);
                TPckgC<TRect> response(rect);
                err = aMessage.Write(aReplySlot, response);
                break;
                }            
            case EAlfGetCachedColor:
                {
                TAknsItemID id;
                id.iMajor = params->iInt1;
                id.iMinor = params->iInt2;
                MAknsSkinInstance* skin = AknsUtils::SkinInstance();
                TRgb color = KRgbBlack;
                err = AknsUtils::GetCachedColor(skin, color, id, params->iInt3 );
                if (!err)
                    {
                    TPckgC<TRgb> ret(color);
                    err = aMessage.Write(aReplySlot, ret);
                    }
                break;
                }
            case EAlfConvertVisualAndClip:
                {
                TUint desLength = aMessage.GetDesMaxLength(aReplySlot);
                HBufC* replyDes = HBufC::NewLC(desLength/2+1);
                TPtr ptr = replyDes->Des();                
                aMessage.Read(aReplySlot, ptr);
                // ToDo: Wo to get font from client..
                //CFont font = Use Font spec to get the font??
                //AknBidiTextUtils::ConvertToVisualAndClipL(ptr, font, params->iInt2, params->iInt3);
                err = aMessage.Write(aReplySlot, ptr);
                CleanupStack::PopAndDestroy();
                break;
                }
            case EAlfGetSkinBackgroundBitmap:
                {
                TRect dummy;
                TRect skinRect;
                TAknsItemID id;
                
                id.iMajor = params->iInt1;
                id.iMinor = params->iInt2;
                
                GetRectForItem(id, dummy, skinRect);

                if (!iBitmap)
                    {
                    iBitmap = new (ELeave) CFbsBitmap();
                    User::LeaveIfError( iBitmap->Create(skinRect.Size(), EColor16MAP) );         
                    }
                
                User::LeaveIfError(iBitmap->Resize(skinRect.Size()));
                
                if (!iSkinControlContext)
                    {
                    iSkinControlContext = CAknsBasicBackgroundControlContext::NewL(
                    KAknsIIDQsnBgScreen, TRect(TPoint(0, 0), CCoeEnv::Static()->ScreenDevice()->SizeInPixels()), ETrue);
                    }
                
                CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(iBitmap);
                CleanupStack::PushL(device);

                CFbsBitGc* gc = 0;
                User::LeaveIfError( device->CreateContext(gc) );
                CleanupStack::PushL(gc);
                iSkinControlContext->SetRect(skinRect);
                iSkinControlContext->SetBitmap(id);

                AknsDrawUtils::DrawBackground(AknsUtils::SkinInstance(), iSkinControlContext, NULL, *gc, TPoint(0,0), skinRect,
                                  KAknsDrawParamDefault);

                CleanupStack::PopAndDestroy(gc);
                CleanupStack::PopAndDestroy(device);
                TPckg<TInt> ret(iBitmap->Handle());
                err = aMessage.Write(aReplySlot, ret);
                break;
                }
            case EAlfGetSkinBitmap:
                {
                // local override input params
                TAlfCommandParams2* params =  (TAlfCommandParams2*) aBuffer.Ptr();                
                TAknsItemID id;
                
                id.iMajor = params->iInt1;
                id.iMinor = params->iInt2;
                
                // recycle ownership through these, assuming synchronized client implementation !!
                delete iTempSkinBmp;
                iTempSkinBmp = 0;
                delete iTempMaskBmp;
                iTempMaskBmp = 0;
                
                TRAP(err, AknsUtils::CreateIconL(	AknsUtils::SkinInstance(), id,
					                    iTempSkinBmp, iTempMaskBmp, 
					                    params->iFileName, params->iInt3,params->iInt4));

                if (err == KErrNotFound) // lets retry without mask
                    { 
                    delete iTempSkinBmp;
                    iTempSkinBmp = 0;
                    delete iTempMaskBmp;
                    iTempMaskBmp = 0;
                    // try if we have better luck without mask
                    AknsUtils::CreateIconL(	AknsUtils::SkinInstance(), id,
					                    iTempSkinBmp, params->iFileName, params->iInt3);

                    }
                
                if (iTempSkinBmp)
                    {
                    AknIconUtils::DisableCompression(iTempSkinBmp);
                    AknIconUtils::SetSize( iTempSkinBmp, params->iSize, params->iScaleMode );    
                    }
                
                if (iTempMaskBmp)
                    {
                    AknIconUtils::DisableCompression(iTempSkinBmp);
                    AknIconUtils::SetSize( iTempMaskBmp, params->iSize, params->iScaleMode );    
                    }
                        
                TInt2 ret;
                ret.iInt1 = iTempSkinBmp->Handle();
                ret.iInt2 = iTempMaskBmp?iTempMaskBmp->Handle():0;
                TPckg<TInt2> retp = ret;
                err = aMessage.Write(aReplySlot, retp);
                break;
                }
            case EGetCachedSkinItemData:
                {
                TAknsItemID id;
                
                id.iMajor = params->iInt1;
                id.iMinor = params->iInt2;
                
                TAlfCachedSkinItemArray array;
                array.iCount = 0;
                CAknsItemData* data = AknsUtils::SkinInstance()->CreateUncachedItemDataL( id, (TAknsItemType)params->iInt3 );
                
                if (data)
                    {
                    CAknsImageTableItemData* itdata = static_cast<CAknsImageTableItemData*>(data);    
                    array.iCount = Min(9, itdata->NumberOfImages());   
                    for(TInt i = 0; i<array.iCount;i++)
                        {
                        array.iImages[i] = itdata->ImageIID(i);
                        } 
                    delete data;        
                    }
                TPckgC<TAlfCachedSkinItemArray> retp(array);
                err = aMessage.Write(aReplySlot, retp);
                break;
                }
            
            case EGetListOfWindowGroups:
                {
                if (*iWgIdArray == -1) // uninitialized  
                    {
                    CCoeEnv::Static()->RootWin().EnableGroupListChangeEvents();
                    UpdateWgListL();
                    }
                
                if (*iWgIdArray != 0)
                    { // updated array exists, return it
                    CompleteWgListMessage(aMessage, aReplySlot);
                    }
                else
                    { // let messages wait for updates
                    iWgMessage = aMessage;
                    iWgReplySlot = aReplySlot;            
                    }
                
                return; // message completed asynchronously
                }    
                

            default:
                {
                err = KErrNotSupported;
                break;
                }
            }
        aMessage.Complete(err);
        }
            
    TPtrC8 StartL(const TDesC8&)
        { 
        return KNullDesC8();
        }
        
   MEikSrvNotifierBase2::TNotifierInfo Info() const        
        {
        return iInfo;
        }
        
   MEikSrvNotifierBase2::TNotifierInfo RegisterL()
        {
        return iInfo;
        }

   void Release()
        {
        delete this;
        }    

    void UpdateWgListL()
        {
        TInt wgId=0;
        TInt count=iWsSession.NumWindowGroups();
        if (count)
            {
            TInt* ptr = iWgIdArray;    
            Mem::FillZ(iWgIdArray,160);
            CArrayFixFlat<TInt>* wgIdArray=new(ELeave) CArrayFixFlat<TInt>(count);
            CleanupStack::PushL(wgIdArray);
            CApaWindowGroupName* wgName=CApaWindowGroupName::NewL(iWsSession);
            CleanupStack::PushL(wgName);
            User::LeaveIfError(iWsSession.WindowGroupList(wgIdArray)); // all groups
            count = wgIdArray->Count(); // count might have changed
            
            iWgEntriesWritten = 0;
            for (TInt ii=0; ii < count-1; ii++)
                { 
                wgId=(*wgIdArray)[ii];
                wgName->ConstructFromWgIdL(wgId);
                if(!wgName->Hidden() && wgName->AppUid().iUid) // lets skip the hidden groups  and groups w/o AppUid at the time being
                    {
                    *ptr = wgId;
                    ptr++;
                    *ptr = wgName->AppUid().iUid;
                    ptr++;                           
                    iWgEntriesWritten++;
                    }
                if (iWgEntriesWritten == 20) // magic, interim patch until we have better way to pair wg ids & clien app uids
                    {
                    break;
                    }    
                }
            CleanupStack::PopAndDestroy(2); // wgIdArray, wgName
            if (iWgIdArray[0] && !iWgMessage.IsNull())
                {
                CompleteWgListMessage(iWgMessage, iWgReplySlot);
                }
            }
        }
    
    void CompleteWgListMessage(const RMessagePtr2& aMessage, TInt aReplySlot)
        {
        if (Mem::Compare((TUint8*)iWgIdArray, 8*iWgEntriesWritten, (TUint8*)iWgIdArray2,8*iWgEntriesWritten))
            {
            TPtrC8 ret((TUint8*)iWgIdArray, 160);   
            if (!aMessage.IsNull())
                {
                TInt err = aMessage.Write(aReplySlot, ret);
                aMessage.Complete(err);
                }
            Mem::Copy((TUint8*)iWgIdArray2, (TUint8*)iWgIdArray,160);
            }
        Mem::FillZ(iWgIdArray, 160);
        }
        
private:
    MEikSrvNotifierBase2::TNotifierInfo iInfo;
    CFbsBitmap* iBitmap;
    CFbsBitmap* iTempSkinBmp;
    CFbsBitmap* iTempMaskBmp;
    CAknsBasicBackgroundControlContext* iSkinControlContext;
    TInt iWgIdArray[40]; // magic, cool
    TInt iWgIdArray2[40]; // magic, cool
    RMessagePtr2 iWgMessage;
    TInt iWgReplySlot;
    RWsSession& iWsSession;
    TUint iWgEntriesWritten;
    RMessagePtr2 iLayoutMirroredMessage;
    TBool iLayoutMirrored;
    TBool iLayoutMirroredUpdated;
    };

CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* array = new CArrayPtrFlat<MEikSrvNotifierBase2>(1);
    if ( array )
        {
        CAlfAppFwNotifier* notif = new CAlfAppFwNotifier(TUid::Uid(KAlfAppFwProxyUid),TUid::Uid(KAlfAppFwProxyUid), MEikSrvNotifierBase2::ENotifierPriorityHigh); 
        if ( notif )
            {
            TRAP_IGNORE(array->AppendL(notif));  // we have reserver granularity of one already
            }
        }
    return array;
    }
    

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KAlfAppFwProxyUid,NotifierArray)
    };
 
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
    return ImplementationTable;
    }
    
// end of file
