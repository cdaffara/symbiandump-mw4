//
// ============================================================================
//  Name     : WidgetMMCHandler.cpp
//  Part of  : SW Installer UIs / WidgetInstallerUI
//
//  Description: Handle notifications of MMC events.
//
//
//  Version     : 3.1
//
//  Copyright © 2006 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ==============================================================================
///

// INCLUDE FILES
#include "wrtusbhandler.h"
#include "wrtharvester.h"
#include "wrtusbhandler.h"
#include <driveinfo.h>

// CONSTANTS


static TInt TimerCallBack( TAny* ptr )
{    
    CWrtUsbHandler* temp = static_cast<CWrtUsbHandler*>(ptr);
    if(temp)
        {
        temp->DeleteTimer();
        temp->DoScanAndUpdate();       
        }
    return 0;    
}
// ============================================================================
// CWrtUsbHandler::NewL()
// two-phase constructor
//
// @since 3.1
// @param aRegistry - Widget registry for callback.
// @param aFs - file session
// @return pointer to CWrtUsbHandler
// ============================================================================
//
CWrtUsbHandler* CWrtUsbHandler::NewL(
    CWrtHarvester* aHarvester,
    RFs& aFs )
    {
    CWrtUsbHandler* self =
        new(ELeave) CWrtUsbHandler( aHarvester , aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ============================================================================
// CWrtUsbHandler::CWrtUsbHandler()
// C++ default constructor
//
// @since 3.1
// ============================================================================
//
CWrtUsbHandler::CWrtUsbHandler( CWrtHarvester* aHarvester,
                                      RFs& aFs )
    : CActive( CActive::EPriorityUserInput ),
      iHarvester( aHarvester ),
      iFs( aFs ),
      iDriveFlags( 0 )
    {
    CActiveScheduler::Add( this );
    }

// ============================================================================
// CWrtUsbHandler::ConstructL()
// Symbian default constructor
//
// @since 3.1
// ============================================================================
//
void CWrtUsbHandler::ConstructL()
    {
    if ( KErrNone != ScanDrives( iDriveFlags ) )
        {
        iDriveFlags = 0;
        }        
    }

// ============================================================================
// CWrtUsbHandler::~CWrtUsbHandler()
// destructor
//
// @since 3.1
// ============================================================================
CWrtUsbHandler::~CWrtUsbHandler()
    {
    Cancel();
    }

// ============================================================================
// CWrtUsbHandler::Start()
// Start monitoring.
//
// @since 5.0
// ============================================================================
void CWrtUsbHandler::Start()
    {
    iFs.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    }

// ============================================================================
// CWrtUsbHandler::RunL()
// Handle notifications of MMC events.
//
// @since 3.1
// ============================================================================
void CWrtUsbHandler::RunL()
    {
    if ( iStatus == KErrNone )
        {
        //Call back Timer
        iCallBackTimer = CPeriodic::NewL(CActive::EPriorityLow);
        iCallBackTimer->Start(10000000,0,TCallBack(&TimerCallBack,this));         
        }
    }

void CWrtUsbHandler::DoScanAndUpdate()
    {
    TInt err = 0;
    TInt driveFlags = 0;
    TInt deltaDriveFlags = 0;
    if ( KErrNone == ScanDrives( driveFlags ) )
        {
        deltaDriveFlags = iDriveFlags ^ driveFlags;
        iDriveFlags = driveFlags;
        }
    
    if ( deltaDriveFlags )
        {           
        //Unpluging USB from Mass storage . . . 
        if(iHarvester->IsInMSMode() == 1)
            {                  
            iHarvester->SetMSMode(0);
            iHarvester->ClearAllOperations();
            TRAP(err, iHarvester->UpdateL() );            
            iFs.NotifyChange( ENotifyDisk, iStatus );
            SetActive();
            return;
            }

        TVolumeInfo volInfo;
        TInt temp = deltaDriveFlags;
        TBool massMemAltered = EFalse;        
        for(TInt DriveNo = EDriveA+1 ; DriveNo<=EDriveY; DriveNo++ )
            {   
            temp =  temp >> 1;
            if( temp & 01)
                {
                TUint status(0);
                err = DriveInfo::GetDriveStatus( iFs, DriveNo , status );
                if(!err && (status & DriveInfo::EDriveExternallyMountable) && (status & DriveInfo::EDriveInternal ))
                    {
                    //Internal Memory
                    massMemAltered = ETrue;
                    }                     
                }            
            }
       if( massMemAltered )
           {           
           iHarvester->SetMSMode(1);            
           }
       
        TRAP( err, iHarvester->UpdateL() ); 
        }
    iFs.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    }


// ============================================================================
// CWrtUsbHandler::RunError()
// Ignore errors from RunL.
//
// @since 5.0
// ============================================================================
TInt CWrtUsbHandler::RunError( TInt /* aError */ )
    {
    return KErrNone; // indicates error was handled
    }

// ============================================================================
// CWrtUsbHandler::DoCancel()
// Cancel the MMC event handler
//
// @since 3.1
// ============================================================================
void CWrtUsbHandler::DoCancel()
    {
    iFs.NotifyChangeCancel();
    }

/* Scans drives and records a bit flag for those that exist and are
 * suitable for installing widgets to.
 */
TInt CWrtUsbHandler::ScanDrives( TInt& aDriveFlags )
    {
    // List all drives in the system
    TDriveList driveList;
    TInt error = iFs.DriveList( driveList );
    if ( KErrNone == error )
        {
        for ( TInt driveNumber = EDriveY;
              driveNumber >= EDriveA;
              driveNumber-- )
            {
            // The drives that will be filtered out are the same ones that
            // WidgetInstaller filters out in CWidgetUIHandler::SelectDriveL()
            if ( (EDriveD == driveNumber)
                 || !driveList[driveNumber] )
                {
                // EDriveD is a temporary drive usually a RAM disk
                continue;
                }

            TVolumeInfo volInfo;
            if ( iFs.Volume( volInfo, driveNumber ) != KErrNone )
                {
                // volume is not usable (e.g. no media card inserted)
                continue;
                }
            if ( (volInfo.iDrive.iType == EMediaNotPresent) ||
                 (volInfo.iDrive.iType == EMediaRom) ||
                 (volInfo.iDrive.iType == EMediaRemote) ||
                 (volInfo.iDrive.iDriveAtt & KDriveAttRom) ||
                 (volInfo.iDrive.iDriveAtt & KDriveAttSubsted) )
                {
                // not a suitable widget install drive
                continue;
                }

            // found a usable drive
            aDriveFlags |= (1 << driveNumber);
          
            }
        }

    return error;
    }
void CWrtUsbHandler::DeleteTimer()
    {
    iCallBackTimer->Cancel();
    delete iCallBackTimer;
    iCallBackTimer = NULL; 
    }
