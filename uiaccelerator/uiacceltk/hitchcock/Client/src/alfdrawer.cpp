/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   CAlfDrawer implementation.
*
*/




#include <alf/alfdrawer.h>
#include <alf/alfdirectclient.h>
#include "alfcrppluginclient.h"
#include "alflogger.h"
#include "alfdrawerinternal.h"

#include <coemain.h>
#include <w32std.h>
#include <fbs.h>

// Class to hold internal alf drawer data.
NONSHARABLE_CLASS( CAlfDrawer::TAlfDrawerData )
    {
public:
    TAlfDrawerData();
        
public:
    CAlfCrpPluginClient* iClient;
    };

// Creates CAlfCrpPluginClient instance, returns error code.
static TInt CreateAlfCrpClient(CAlfCrpPluginClient*& aClient);
// Creates CAlfCrpPluginClient instance.
static CAlfCrpPluginClient* CreateAlfCrpClientL();

NONSHARABLE_CLASS( TAlfDrawerDirectClient ) : public MAlfDrawerScreenInterface
    {
public:
    TAlfDrawerDirectClient( RAlfDirectClient& aClient );
    TInt GetSizeAndRotation(TSize& aSize, TInt& aRotation);
    TInt ReadPixels(CFbsBitmap* aBitmap);

private:
    RAlfDirectClient iClient;
    };
   
/**
 * Misc utility methods for CAlfDrawer.
 */
NONSHARABLE_CLASS(AlfDrawerUtils)
    {
public:

    enum TRotation
        {
	    ERotationNone,
	    ERotation90,
	    ERotation180,
	    ERotation270
	    };

    /**
     * Inverse rotate.
     */
    static void RotateBack(TInt aVirtualRotation, TRect& aRect, TSize aScreenSize)
        {
        if( aVirtualRotation == ERotation90 || aVirtualRotation == ERotation270 )
            {
            RotateRect( aVirtualRotation, aRect, aScreenSize, ETrue );  
            }
        else if( aVirtualRotation == ERotation180 )
            {
            RotateRect( aVirtualRotation, aRect, aScreenSize, EFalse );  
            }    
        }

    /**
     * Rotate rect
     */
    static void RotateRect(TInt aDirection, TRect& aRect, TSize aScreenSize, TBool aMirror)
        {
        if (aMirror) aDirection = (aDirection+2)%4; // magic    
        for (;aDirection > 0; aDirection--) 
            {
            TSize rotatedSize(aRect.Size().iHeight, aRect.Size().iWidth);     
            aRect = TRect(TPoint(aRect.iTl.iY, aScreenSize.iWidth - aRect.iTl.iX - rotatedSize.iHeight ), rotatedSize);
            aScreenSize = TSize(aScreenSize.iHeight, aScreenSize.iWidth);
            }
        }
        
    /**
     * Copies screen to bitmap using 'read pixels' operation.
     */
    static void DoCopyScreenToBitmapL( 
        CWsScreenDevice& aDevice,
        CFbsBitmap* aBitmap, 
        const TRect& aRect );

    /**
     * Copies screen to bitmap using 'read pixels' operation.
     */
    static void DoCopyScreenToBitmap2L( 
        MAlfDrawerScreenInterface* aInterface,
        CFbsBitmap* aBitmap, 
        const TRect& aRect );
        
    };

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfDrawer* CAlfDrawer::NewL()
    {
    CAlfDrawer* self = new (ELeave) CAlfDrawer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfDrawer::~CAlfDrawer()
    {
    if ( iData )
        {
        delete iData->iClient;
        delete iData;
        }
    }

// ---------------------------------------------------------------------------
// Finish
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfDrawer::Finish()
    {
    // First perform RWsSession finish operation to ensure
    // that all drawing commands have been issued to renderstage.
    TInt err = CCoeEnv::Static()->WsSession().Finish();

    // Then create CRP instance (if needed).
    if ( err == KErrNone && !iData->iClient ) 
        {
        err = CreateAlfCrpClient( iData->iClient );
        }

    // Finally, wait until alf side has finished rendering.
    if ( err == KErrNone && iData->iClient )
        {
        iData->iClient->Synchronize();
        }
        
    return err;
    }

// ---------------------------------------------------------------------------
// FallbackCopyScreenToBitmap
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfDrawer::FallbackCopyScreenToBitmap(
        CWsScreenDevice& aDevice,
        CFbsBitmap* aBitmap, 
        const TRect& aRect)
    {
    TRAPD(err, AlfDrawerUtils::DoCopyScreenToBitmapL(aDevice, aBitmap, aRect));
    return err;    
    }

// ---------------------------------------------------------------------------
// DoCopyScreenToBitmapL
// ---------------------------------------------------------------------------
//
void AlfDrawerUtils::DoCopyScreenToBitmapL( 
    CWsScreenDevice& aDevice,
    CFbsBitmap* aBitmap, 
    const TRect& aRect )
    {
    if (aDevice.GetScreenNumber() != 0) // only screen 0 supported for now
        {
        User::Leave(KErrNotSupported);
        }
                
    if ( !aBitmap || 
         !aBitmap->Handle() || 
         aBitmap->IsCompressedInRAM() || 
         aBitmap->ExtendedBitmapType() != KNullUid )
        {
        User::Leave(KErrArgument);
        }            

    __ALFLOGSTRING("DoCopyScreenToBitmapL begin");
    
    RAlfDirectClient client;
    CleanupClosePushL(client);

    TAlfDrawerDirectClient clientWrapper(client);
    DoCopyScreenToBitmap2L(&clientWrapper, aBitmap, aRect);   
    
    CleanupStack::PopAndDestroy(); // CleanupClosePushL
    
    __ALFLOGSTRING("DoCopyScreenToBitmapL - Done");    
    }

// ---------------------------------------------------------------------------
// DoCopyScreenToBitmapL
// ---------------------------------------------------------------------------
//
void AlfDrawerUtils::DoCopyScreenToBitmap2L( 
        MAlfDrawerScreenInterface* aInterface,
        CFbsBitmap* aBitmap, 
        const TRect& aRect )
    {
    // Get size & virtual rotation from ALF side.
    // GetSizeAndRotation will also create session to server.

    TInt rotation = 0;
    TSize size;
    User::LeaveIfError(aInterface->GetSizeAndRotation(size, rotation));

    // Calculate device size in pixels (same as aDevice.SizeInPixels())
    TSize deviceSize = size;
    if (rotation == AlfDrawerUtils::ERotation90 ||
        rotation == AlfDrawerUtils::ERotation270 )
        {
        deviceSize.iWidth = size.iHeight;
        deviceSize.iHeight = size.iWidth;
        }

    // Clip aRect to screen boundaries
    TRect actualRect(deviceSize);
    actualRect.Intersection(aRect);
    
    __ALFLOGSTRING2("DoCopyScreenToBitmapL - requested rect origin %d x %d", actualRect.iTl.iX, actualRect.iTl.iY ) ;
    __ALFLOGSTRING2("DoCopyScreenToBitmapL - requested rect %d x %d", actualRect.Size().iWidth, actualRect.Size().iHeight ) ;
    __ALFLOGSTRING2("DoCopyScreenToBitmapL - bitmap rect %d x %d", aBitmap->SizeInPixels().iWidth, aBitmap->SizeInPixels().iHeight );
    __ALFLOGSTRING1("DoCopyScreenToBitmapL - bitmap mode %d", aBitmap->DisplayMode() );

    if ( TRect( aBitmap->SizeInPixels() ).IsEmpty() || 
         actualRect.IsEmpty() )
        {
        __ALFLOGSTRING("DoCopyScreenToBitmapL - empty rect or zero bitmap size");
        return;
        }

    // Select display mode.
    
    TDisplayMode surfaceDisplayMode = EColor64K;
    TDisplayMode bitmapDisplayMode = aBitmap->DisplayMode();
    if ( bitmapDisplayMode == EColor16MU || 
         bitmapDisplayMode == EColor16MA || 
         bitmapDisplayMode == EColor16MAP )
        {
        surfaceDisplayMode = bitmapDisplayMode;
        }

    __ALFLOGSTRING3("DoCopyScreenToBitmapL - surface size %d x %d, mode %d", 
        size.iWidth, size.iHeight, surfaceDisplayMode ) ;

    // Read pixels to temporary bitmap
    CFbsBitmap* surfaceBitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL( surfaceBitmap );
    User::LeaveIfError( surfaceBitmap->Create( size, surfaceDisplayMode ) );
    
    TInt err = aInterface->ReadPixels( surfaceBitmap );              
    __ALFLOGSTRING1("DoCopyScreenToBitmapL - ReadPixels returned %d", err);
    User::LeaveIfError( err );  

    // Copy data
    TInt surfaceStride = CFbsBitmap::ScanLineLength(size.iWidth, surfaceDisplayMode);
    __ALFLOGSTRING2("DoCopyScreenToBitmapL - DisplayMode %d, Stride %d", surfaceDisplayMode, surfaceStride);  

    TDisplayMode displayMode = surfaceDisplayMode;
    
    TInt8 bytesPerPixel = 2;
    switch( displayMode )
        {
        case EColor64K:
            {
            bytesPerPixel = 2;
            break;
            }
        case EColor16MU:
        case EColor16MA:
        case EColor16MAP:
            {
            bytesPerPixel = 4;
            break;
            }
        
        default:
            {
            __ALFLOGSTRING1("DoCopyScreenToBitmapL - display mode not supported %d", displayMode);
            User::Leave( KErrNotSupported );
            }
        }
    
    // actualRect (/aRect) must be converted to correct coordinate system.
    TRect surfaceRect(actualRect);
    AlfDrawerUtils::RotateBack(rotation, surfaceRect, deviceSize);
    surfaceRect.Intersection(TRect(size));
    
    __ALFLOGSTRING2("DoCopyScreenToBitmapL - surface rect tl %d x %d", surfaceRect.iTl.iX, surfaceRect.iTl.iY ) ;
    __ALFLOGSTRING2("DoCopyScreenToBitmapL - surface rect size %d x %d", surfaceRect.Size().iWidth, surfaceRect.Size().iHeight ) ;

    CFbsBitmap* bitmap = NULL;
    TBool needTempBitmap = ETrue;

    if( aBitmap->DisplayMode() == displayMode && actualRect == aRect && aBitmap->SizeInPixels() == aRect.Size() )
        {
        // Copy data direcly to given bitmap
        bitmap = aBitmap;
        needTempBitmap = EFalse;
        }
    else 
        {
        // Create temporary bitmap which maches the sufrace's pixel format and source rect size
        bitmap = new (ELeave) CFbsBitmap;
        CleanupStack::PushL( bitmap );
        TInt err = bitmap->Create( actualRect.Size(), displayMode );       
        __ALFLOGSTRING1("DoCopyScreenToBitmapL - Create returned %d", err );
        User::LeaveIfError( err );
        __ALFLOGSTRING("DoCopyScreenToBitmapL - display mode or size don't mach -> using temp bitmap" );            
        }

    surfaceBitmap->BeginDataAccess();
    bitmap->BeginDataAccess();
    
    TUint8* surfacePtr = (TUint8*)surfaceBitmap->DataAddress() + surfaceStride * surfaceRect.iTl.iY;
    
    TUint8* bitmapPtr = (TUint8*)bitmap->DataAddress();
    TInt rowBegin = surfaceRect.iTl.iX * bytesPerPixel;
    TInt captureBitmapWidth = surfaceRect.Width() * bytesPerPixel;              

    const TSize bitmapSize = bitmap->SizeInPixels();
    
    // Initialize with direct copy case parameters
    TInt bitmapPtrColumnDelta = bytesPerPixel;
    TInt bitmapPtrRowDelta = bitmap->DataStride();
    
    const TSize surfaceRectSize(surfaceRect.Size());
    if ( rotation == AlfDrawerUtils::ERotationNone )
        {
        __ALFLOGSTRING("DoCopyScreenToBitmapL - direct copy") ;
        
        // Direct copy case
        for ( TInt row = surfaceRectSize.iHeight - 1 ; row >= 0  ; --row )
            {
            // Copy only the desired part of the bitmap
            memcpy( bitmapPtr, 
                    surfacePtr + rowBegin, 
                    captureBitmapWidth );
            surfacePtr += surfaceStride;
            bitmapPtr += bitmap->DataStride();
            }
        }
    else
        {
        // Handle rotation cases
        //
        // 0 degree case
        //    A B C
        //    1 2 3
        // 90 degrees anti-clockwise
        //    C 3
        //    B 2 
        //    A 1
        // 180 degrees anti-clockwise
        //    3 2 1
        //    C B A
        // 270 degrees anti-clockwise
        //    1 A
        //    2 B
        //    3 C
        // 
        switch (rotation)
            {
            case AlfDrawerUtils::ERotation90:
                bitmapPtrColumnDelta = -bitmap->DataStride();
                bitmapPtrRowDelta = bytesPerPixel;
                bitmapPtr += ( bitmapSize.iHeight - 1 ) * bitmap->DataStride();
                break;
            case AlfDrawerUtils::ERotation180:
                bitmapPtrColumnDelta = -bytesPerPixel;
                bitmapPtrRowDelta = -bitmap->DataStride();
                bitmapPtr += ( bitmapSize.iHeight - 1 ) * bitmap->DataStride() + ( bitmapSize.iWidth - 1 ) * bytesPerPixel;
                break;
            case AlfDrawerUtils::ERotation270:
                bitmapPtrColumnDelta = bitmap->DataStride();
                bitmapPtrRowDelta = -bytesPerPixel;
                bitmapPtr += ( bitmapSize.iWidth - 1 ) * bytesPerPixel;
                break;
            default:
                break;
            }
    
        // We go through surface row by row, column by column and
        // copy pixel data to appropriate place to bitmap
        if ( bytesPerPixel == 4 )
            {
            __ALFLOGSTRING("DoCopyScreenToBitmapL - four bytes per pixel, rotated copy") ;
            
            for ( TInt row = surfaceRectSize.iHeight - 1 ; row >= 0 ; --row )
                {
                TUint8* rowBitmapPtr = bitmapPtr;
                TUint8* rowSurfacePtr = surfacePtr + rowBegin;
                for ( TInt column = surfaceRectSize.iWidth - 1 ; column >= 0 ; --column )
                    {
                    *((TUint32*)rowBitmapPtr) = *((TUint32*)rowSurfacePtr);
                    
                    rowBitmapPtr += bitmapPtrColumnDelta;
                    rowSurfacePtr += bytesPerPixel;
                    }
        
                surfacePtr += surfaceStride;
                bitmapPtr += bitmapPtrRowDelta;
                }
            }
        else
            {
            // Now bytesPerPixel == 2
            __ALFLOGSTRING("DoCopyScreenToBitmapL - two bytes per pixel, rotated copy") ;
            for ( TInt row = surfaceRectSize.iHeight - 1 ; row >= 0 ; --row )
                {
                TUint8* rowBitmapPtr = bitmapPtr;
                TUint8* rowSurfacePtr = surfacePtr + rowBegin;
                for ( TInt column = surfaceRectSize.iWidth - 1 ; column >= 0 ; --column )
                    {
                    *((TUint16*)rowBitmapPtr) = *((TUint16*)rowSurfacePtr);
                    
                    rowBitmapPtr += bitmapPtrColumnDelta;
                    rowSurfacePtr += bytesPerPixel;
                    }
        
                surfacePtr += surfaceStride;
                bitmapPtr += bitmapPtrRowDelta;
                } 
            }
        }
    __ALFLOGSTRING("DoCopyScreenToBitmapL - copy finished" );
    surfaceBitmap->EndDataAccess(ETrue);
    bitmap->EndDataAccess();

    if( !needTempBitmap )
        {
        bitmap = NULL;
        }
    else // bitblit the temporary bitmap to given bitmap
        {
        CFbsBitmapDevice* tempBitmapDevice = CFbsBitmapDevice::NewL( aBitmap );
        CleanupStack::PushL( tempBitmapDevice );
        CFbsBitGc* tempBitmapGc = NULL;
        User::LeaveIfError( tempBitmapDevice->CreateContext( tempBitmapGc ) );             
        CleanupStack::PushL( tempBitmapGc );

        tempBitmapGc->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        tempBitmapGc->BitBlt( TPoint(), bitmap );
        
        CleanupStack::PopAndDestroy( tempBitmapGc );
        CleanupStack::PopAndDestroy( tempBitmapDevice );
        CleanupStack::PopAndDestroy( bitmap );
        }
    
    CleanupStack::PopAndDestroy( surfaceBitmap );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfDrawer::CAlfDrawer()
    {
    }
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfDrawer::ConstructL()
    {
    iData = new (ELeave) TAlfDrawerData;
    CreateAlfCrpClient( iData->iClient );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfDrawer::TAlfDrawerData::TAlfDrawerData()
    : iClient( NULL )
    {
    }

// ---------------------------------------------------------------------------
// CopyScreenToBitmap
// ---------------------------------------------------------------------------
//
EXPORT_C TInt AlfDrawerInternal::CopyScreenToBitmap(
        MAlfDrawerScreenInterface* aInterface, 
        CFbsBitmap* aBitmap, 
        const TRect& aRect )
    {
    TRAPD(err, AlfDrawerUtils::DoCopyScreenToBitmap2L(aInterface, aBitmap, aRect));
    return err;
    }

// ---------------------------------------------------------------------------
// CreateAlfCrpClient
// ---------------------------------------------------------------------------
//
static TInt CreateAlfCrpClient(CAlfCrpPluginClient*& aClient)
    {
    TRAPD(err, aClient = CreateAlfCrpClientL());
    return err;
    }

// ---------------------------------------------------------------------------
// CreateAlfCrpClientL
// ---------------------------------------------------------------------------
//
static CAlfCrpPluginClient* CreateAlfCrpClientL()
    {
    CAlfCrpPluginClient* client = new (ELeave) CAlfCrpPluginClient;
    CleanupStack::PushL( client );
    client->ConstructL();
    CleanupStack::Pop( client );
    return client;
    }

// ---------------------------------------------------------------------------
// TAlfDrawerDirectClient
// ---------------------------------------------------------------------------
//
TAlfDrawerDirectClient::TAlfDrawerDirectClient( RAlfDirectClient& aClient )
    : iClient( aClient )
    {
    }
    
// ---------------------------------------------------------------------------
// GetSizeAndRotation
// ---------------------------------------------------------------------------
//
TInt TAlfDrawerDirectClient::GetSizeAndRotation(TSize& aSize, TInt& aRotation)
    {
    return iClient.GetSizeAndRotation(aSize, aRotation);
    }
    
// ---------------------------------------------------------------------------
// ReadPixels
// ---------------------------------------------------------------------------
//
TInt TAlfDrawerDirectClient::ReadPixels(CFbsBitmap* aBitmap)
    {
    return iClient.ReadPixels(aBitmap->Handle());
    }

