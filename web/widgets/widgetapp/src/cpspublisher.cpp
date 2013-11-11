/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Internal class definition for WidgetUI application.
*
*/


#include "cpspublisher.h"

#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <liwcommon.h>
#include <liwcommon.hrh>
#include <AknsFrameBackgroundControlContext.h>
#include <fbs.h>
#include <AknsDrawUtils.h> 


// TODO use global definitions!

_LIT8( KCPService, "Service.ContentPublishing" );
_LIT8( KCPInterface, "IDataSource" );

_LIT8( KType, "type" );
_LIT( KCpData, "cp_data" );

_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId, "content_id" );


_LIT8( KDataMap, "data_map" );

_LIT8( KItem, "item" );
_LIT8( KAdd, "Add" );

_LIT(KTemplateWidget,"ai3templatedwidget");
_LIT8( KImageKey, "image1");

_LIT8( KMask, "_mask");

_LIT(KHomescreenId,"ai3homescreen");
_LIT(KTemplateName,"wideimage");
_LIT( KImageContainer, "image_container" );
_LIT8( KWidth, "width");
_LIT8( KHeight, "height");

_LIT( KPublisherInfo, "publisher" );
_LIT( KInfoType, "network" );
_LIT( KDisConnected, "disconnected");
_LIT( KConnected, "connected");
_LIT8( KAPStatus, "apstatus");

_LIT8( KFilter, "filter" );
_LIT8( KResults,      "results");
_LIT8( KOperation, "operation" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT( KUpdate, "update" );
_LIT8( KChangeInfo, "change_info" );
_LIT( KWRTPublisher, "wrt_publisher");

const TInt KSkinGfxInnerRectShrink = 5;

static void DoResetAndDestroy( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, User::Invariant() );
    reinterpret_cast< RCriteriaArray* >( aPtr )->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// InvertBitmap
// Sets white pixels black and other ones white.
// -----------------------------------------------------------------------------
static void InvertBitmap( CFbsBitmap*& aBitmap )
    {
    TSize inSize = aBitmap->SizeInPixels();
    TBitmapUtil bitmapUtil( aBitmap );
    bitmapUtil.Begin( TPoint(0,0) );     
    TInt xPos;
    for ( TInt yPos = 0; yPos < inSize.iHeight; yPos++ )
        {
        bitmapUtil.SetPos( TPoint( 0,yPos ));
        for ( xPos = 0; xPos < inSize.iWidth; xPos++ ) 
            {
            //Inverts the pixel colors.
            TInt pix = bitmapUtil.GetPixel();
            if( pix == 0xff )
                {
                bitmapUtil.SetPixel( 0x00 );
                }
            else
                {
                bitmapUtil.SetPixel( 0xff );
                }
            bitmapUtil.IncXPos();
            }
        }
    bitmapUtil.End();
    }
// -----------------------------------------------------------------------------
// LoadMaskBitmapL
// Loads a mask bitmap from the skin.
// -----------------------------------------------------------------------------
static void LoadMaskBitmapL( 
    CFbsBitmap*& aBitmap,
    TSize aSize)
    {
    const TRect rect( 0, 0, aSize.iWidth, aSize.iHeight );
    TRect shrunkRect( rect );
    shrunkRect.Shrink( KSkinGfxInnerRectShrink, KSkinGfxInnerRectShrink );
    
    CAknsFrameBackgroundControlContext* skinContext = 
    CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnHomeBgWidget, rect, shrunkRect, EFalse );
    CleanupStack::PushL( skinContext );
    
    // Create an off-screen bitmap and context (CFbsBitGc)
     // Also remember to take care of destruction / cleanup during leaves
    aBitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( aBitmap );
    User::LeaveIfError( aBitmap->Create( aSize, EGray256 ) );

    CFbsBitGc* bitGc = NULL;
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( aBitmap );
    CleanupStack::PushL( bitmapDevice );
    User::LeaveIfError( bitmapDevice->CreateContext( bitGc ) );

    // Draw the content on the off-screen bitmap
    AknsDrawUtils::DrawBackground( AknsUtils::SkinInstance(), skinContext, 
             NULL, *bitGc, TPoint(0,0), rect, KAknsDrawParamNoClearUnderImage );
     
    CleanupStack::PopAndDestroy( bitmapDevice );
    CleanupStack::Pop( aBitmap );
    CleanupStack::PopAndDestroy( skinContext );
    }


CCpsPublisher* CCpsPublisher::NewL()
    {
    CCpsPublisher* self = new (ELeave) CCpsPublisher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CCpsPublisher::~CCpsPublisher()
    {
    
    TRAP_IGNORE(ExecuteRegistrationCommandL( KLiwOptCancel ));
    
    if( iCpsInterface )
        {
        iCpsInterface->Close();
        iCpsInterface = NULL;
        }
    if( iServiceHandler )
        {
        iServiceHandler->Reset();
        delete iServiceHandler;
        iServiceHandler = NULL;
        }
    delete iMaskBitmap;
__UHEAP_MARKEND;    
    }

CLiwDefaultMap* ElementSizeFilterLC()
    {
    //Create filter criteria for requested entries in form of LIW map:
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant( KHomescreenId ));
    filter->InsertL( KContentType, TLiwVariant( KTemplateName ));
    filter->InsertL( KContentId, TLiwVariant( KImageContainer ));
    return filter;
    }

void UpdateElementSizeL( CLiwDefaultMap *outDataMap, TSize& aSize )
    {
    TLiwVariant variant;
    variant.PushL();
    if ( outDataMap->FindL( KWidth, variant ) )
        {
        variant.Get( aSize.iWidth );
        }
    variant.Reset();
    if ( outDataMap->FindL( KHeight, variant ) )
        {
        variant.Get( aSize.iHeight );
        }
    CleanupStack::PopAndDestroy( &variant );
    }

CCpsPublisher::CCpsPublisher()
    {
    
    }

void CCpsPublisher::ConstructL()
    {
    __UHEAP_MARK;
    RDebug::Printf("CCpsPublisher::InitCPS");
    InitCpsInterfaceL();
    RDebug::Printf("CCpsPublisher::Register");
    ExecuteRegistrationCommandL( 0 );
    RDebug::Printf("CCpsPublisher::GetBitmapSize");
    GetBitmapSizeL();
    RDebug::Printf("CCpsPublisher::EndConstructL");
    }

TSize CCpsPublisher::BitmapSize()
    {
    if( iSize.iHeight == 0 || iSize.iWidth == 0 )
        {
        // TODO: This is a work around to fix the problem in 'Loading Content' feature.
        // Miniview does not have size when 'Loading Content' is showing.
        return TSize(312, 82 );
        }
    else
        {
        return iSize;
        }
    }

void CCpsPublisher::PublishBitmapL( CFbsBitmap& aBitmap, const TDesC& aBundleId )
    {
#ifdef _DEBUG
    TSize bitmapSize = aBitmap.SizeInPixels();
    RDebug::Printf("CCpsPublisher::PublishBitmapL bitmap width %d height %d", bitmapSize.iWidth, bitmapSize.iHeight);
#endif
    
    // Load the mask bitmap from skin
    if( !iMaskBitmap ||
        aBitmap.SizeInPixels() != iMaskBitmap->SizeInPixels())
        {
        RDebug::Printf("CCpsPublisher::LoadMaskBitmapL");
        delete iMaskBitmap;
        iMaskBitmap = NULL;
        LoadMaskBitmapL( iMaskBitmap, aBitmap.SizeInPixels() );
        InvertBitmap( iMaskBitmap );
        }

    TInt handle = aBitmap.Handle();
    TInt maskHandle = KErrBadHandle;
    if( iMaskBitmap )
        {
        maskHandle = iMaskBitmap->Handle();
        }    
#ifdef _DEBUG
    RDebug::Printf("CCpsPublisher::PublishBitmapL, bitmap handle 0x%x", handle);
#endif
    AddImageHandleL( aBundleId,
            handle, maskHandle, KImageKey() );
    }

void CCpsPublisher::NetworkConnectionCancelledL()
    {
    CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outparam = iServiceHandler->OutParamListL();

    TLiwGenericParam type( KType, TLiwVariant( KCpData ));
    type.PushL();
    inparam.AppendL( type );
    
    CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();
    
    // Create content map
    cpdatamap->InsertL( KPublisherId, TLiwVariant( KHomescreenId ));
    cpdatamap->InsertL( KContentType, TLiwVariant( KPublisherInfo )); 
    cpdatamap->InsertL( KContentId, TLiwVariant( KInfoType ));
    // Create the data map
    datamap->InsertL( KAPStatus, TLiwVariant( KDisConnected ));
    cpdatamap->InsertL( KDataMap, TLiwVariant(datamap) );
    
    TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));     
    item.PushL(); 
       
    inparam.AppendL( item );
    
    iCpsInterface->ExecuteCmdL( KAdd , inparam, outparam);

    CleanupStack::PopAndDestroy( &item );
    CleanupStack::PopAndDestroy( datamap );
    CleanupStack::PopAndDestroy( cpdatamap );
    CleanupStack::PopAndDestroy( &type );

    outparam.Reset();
    inparam.Reset();
    }

void CCpsPublisher::NetworkConnectionAllowedL()
    {
    CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outparam = iServiceHandler->OutParamListL();

    TLiwGenericParam type( KType, TLiwVariant( KCpData ));
    type.PushL();
    inparam.AppendL( type );
    
    CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();
    
    // Create content map
    cpdatamap->InsertL( KPublisherId, TLiwVariant( KHomescreenId ));
    cpdatamap->InsertL( KContentType, TLiwVariant( KPublisherInfo )); 
    cpdatamap->InsertL( KContentId, TLiwVariant( KInfoType ));
    // Create the data map
    datamap->InsertL( KAPStatus, TLiwVariant( KConnected ));
    cpdatamap->InsertL( KDataMap, TLiwVariant(datamap) );
    
    TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));     
    item.PushL(); 
       
    inparam.AppendL( item );
    
    iCpsInterface->ExecuteCmdL( KAdd , inparam, outparam);

    CleanupStack::PopAndDestroy( &item );
    CleanupStack::PopAndDestroy( datamap );
    CleanupStack::PopAndDestroy( cpdatamap );
    CleanupStack::PopAndDestroy( &type );

    outparam.Reset();
    inparam.Reset();
    }

void CCpsPublisher::GetBitmapSizeL()
    {
    CLiwDefaultMap *outDataMap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* filter = ElementSizeFilterLC();
    ExecuteCommandL( filter, outDataMap, KCpData  );
    CleanupStack::PopAndDestroy( filter );
    
    UpdateElementSizeL( outDataMap, iSize );
    
    CleanupStack::PopAndDestroy( outDataMap );
    }
    
void CCpsPublisher::ExecuteRegistrationCommandL( 
    TUint aOption )
    {
    CLiwDefaultMap* filter = ElementSizeFilterLC();
    filter->InsertL( KOperation, TLiwVariant( KUpdate ));
    
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
        
    // Fill in input list for RequestNotification command
    inParamList.AppendL( TLiwGenericParam( KType,TLiwVariant( KCpData )));
    inParamList.AppendL( TLiwGenericParam( KFilter ,TLiwVariant( filter )));
        
    iCpsInterface->ExecuteCmdL( 
                KRequestNotification,
                inParamList,
                outParamList,
                aOption,
                this );
    
    outParamList.Reset();
    inParamList.Reset(); 
    CleanupStack::PopAndDestroy( filter );
    }

TInt CCpsPublisher::HandleNotifyL(
    TInt /*aCmdId*/,
    TInt /*aEventId*/,
    CLiwGenericParamList& aEventParamList,
    const CLiwGenericParamList& /*aInParamList*/ )
    {
    __UHEAP_MARK;
    TInt pos(0);
    aEventParamList.FindFirst( pos, KChangeInfo );
    if (pos != KErrNotFound)
        {
        // Get list of maps
        TLiwVariant variant = ( aEventParamList )[pos].Value();
        variant.PushL();
        const CLiwList* changeMapsList = variant.AsList();

        TPtrC operation;
        
        // Iter through list content
        for (TInt i = 0; i < changeMapsList->Count(); ++i)
            {
             if ( changeMapsList->AtL( i, variant )) 
                 {
                 const CLiwMap* map  = variant.AsMap();
                 
                 // Check what triggered a notification
                 if ( map->FindL( KOperation, variant ))
                     {
                     operation.Set( variant.AsDes());
                     }
                 if ( operation == KUpdate )
                     {
                     GetBitmapSizeL();
                     }
                 }
            }
        CleanupStack::PopAndDestroy( &variant );
        }
    __UHEAP_MARKEND;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCpsPublisher::AddImageHandleL( const TDesC& aBundleId, const TInt& aHandle,
        const TInt& aMaskHandle, const TDesC8& aImageKey )
    {
    __UHEAP_MARK;
    CLiwGenericParamList& inparam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outparam = iServiceHandler->OutParamListL();

    TLiwGenericParam cptype( KType, TLiwVariant( KCpData ));
    cptype.PushL();
    
    inparam.AppendL( cptype );
    
    CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
    
    // Add bitmap handle to data map
    map->InsertL( aImageKey,  TLiwVariant( aHandle )); // key - aKey, value - map (stringsMap)
    
    HBufC8* maskKey = HBufC8::NewLC( aImageKey.Length() + KMask().Length() );
    TPtr8 maskKeyPtr = maskKey->Des();
    
    maskKeyPtr.Append(aImageKey);
    maskKeyPtr.Append(KMask);
    map->InsertL( maskKeyPtr,  TLiwVariant( aMaskHandle ));
    CleanupStack::PopAndDestroy( maskKey );

    // Create content data map
    cpdatamap->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
    cpdatamap->InsertL( KContentType, TLiwVariant( KTemplateWidget )); 
    cpdatamap->InsertL( KContentId, TLiwVariant( aBundleId ));
    cpdatamap->InsertL( KDataMap, TLiwVariant(map) );
            
    TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));     
    item.PushL(); 
       
    inparam.AppendL( item );
    
    iCpsInterface->ExecuteCmdL( KAdd , inparam, outparam);
    
 
    CleanupStack::PopAndDestroy(); // item
    CleanupStack::PopAndDestroy( map );
    CleanupStack::PopAndDestroy( cpdatamap );
    CleanupStack::PopAndDestroy(); // cptype

    outparam.Reset();
    inparam.Reset();
    __UHEAP_MARKEND;
    }

void CCpsPublisher::ExecuteCommandL(CLiwDefaultMap* aInFilter, CLiwDefaultMap* aOutDataMap, const TDesC16& aRegistry  )
    {
    CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
    CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();
    
    TLiwGenericParam type( KType, TLiwVariant( aRegistry ) );
    inParamList->AppendL( type );
    
    //append filter to input param
     TLiwGenericParam item( KFilter, TLiwVariant( aInFilter ));
     inParamList->AppendL( item );
     
    // It is assumed that interface is already initiated
    iCpsInterface->ExecuteCmdL( _L8("GetList"), *inParamList, *outParamList);

    type.Reset();
    item.Reset();
    inParamList->Reset();
    
    //extracts data map
    TInt pos = 0;
    outParamList->FindFirst( pos, KResults );
    if( pos != KErrNotFound )
        // results present
        {
        //extract iterator on results list
        TLiwVariant variant = (*outParamList)[pos].Value();
        CLiwIterable* iterable = variant.AsIterable();
        iterable->Reset();
        
        //get next result
        if( iterable->NextL( variant ) )
            {
            //extract content map
            CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
            variant.Get( *map );
            if( map->FindL( KDataMap, variant) )
                {
                 variant.Get( *aOutDataMap );
                }
            variant.Reset();
            CleanupStack::PopAndDestroy( map );
            }
        iterable->Reset();
        }
    outParamList->Reset();
    }

void CCpsPublisher::InitCpsInterfaceL()
    {
    if( !iServiceHandler )
        {
        iServiceHandler = CLiwServiceHandler::NewL();
        }
    
    CLiwGenericParamList& inParam = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParam = iServiceHandler->OutParamListL();
    
    RCriteriaArray a;
    TCleanupItem item( DoResetAndDestroy, &a );
    CleanupStack::PushL( item );
    
    CLiwCriteriaItem* crit = CLiwCriteriaItem::NewLC( KLiwCmdAsStr, KCPInterface,
        KCPService  );
    crit->SetServiceClass( TUid::Uid(KLiwClassBase) );
    
    a.AppendL(crit);    
    
    CleanupStack::Pop( crit );
    
    iServiceHandler->AttachL( a );
    iServiceHandler->ExecuteServiceCmdL( *crit, inParam, outParam ); 
    
    CleanupStack::PopAndDestroy(); // item;
        
    // find service interface
    TInt pos = 0;
    MLiwInterface* msgInterface = NULL;
    outParam.FindFirst( pos, KCPInterface );
    if ( pos != KErrNotFound ) 
        {
        msgInterface = (outParam)[pos].Value().AsInterface(); 
        }
    
    outParam.Reset();
    inParam.Reset();    
    iCpsInterface = msgInterface;
    }
// END OF FILE



