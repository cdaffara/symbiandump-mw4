/**
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
* Description: CUpnpTmServerDeviceInfo class implementation.
*
*/

//  Include Files  
#include "upnptmserverdeviceinfo.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmserverdeviceinfoTraces.h"
#endif

// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::NewL
// Two-phased constructor.
// @param aIapId    IAP ID.
// ---------------------------------------------------------------------------------
//
EXPORT_C CUpnpTmServerDeviceInfo* CUpnpTmServerDeviceInfo::NewL( TUint32 aIapId )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEINFO_NEWL_ENTRY );
    CUpnpTmServerDeviceInfo* self = new (ELeave) CUpnpTmServerDeviceInfo(  );  
    CleanupStack::PushL(self);
    self->ConstructL( aIapId );
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEINFO_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::CUpnpTmServerDeviceInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDeviceInfo::CUpnpTmServerDeviceInfo( ):iDeviceInfo(NULL),
    iBtAddr(NULL),iStartConnValue(EFalse)
    {

    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// @param aIapId    IAP ID. Leaves if the proper IAP ID is not set.
// ---------------------------------------------------------------------------------
//
void CUpnpTmServerDeviceInfo::ConstructL( TUint32 aIapId )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEINFO_CONSTRUCTL_ENTRY );
    OstTrace1( TRACE_NORMAL, CUPNPTMSERVERDEVICEINFO_CONSTRUCTL, "CUpnpTmServerDeviceInfo::ConstructL;aIapId=%d", aIapId );
    
    if ( aIapId <= KErrNone )
        {
        User::Leave( KErrArgument );  // Leaves in case of invalid IAP ID
        }
    iIapId = aIapId;   
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEINFO_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::~CUpnpTmServerDeviceInfo
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmServerDeviceInfo::~CUpnpTmServerDeviceInfo()
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEINFO_CUPNPTMSERVERDEVICEINFO_ENTRY );
    delete iDeviceInfo;
    delete iBtAddr;
    iDeviceIconList.ResetAndDestroy();
    iDeviceIconList.Close();
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEINFO_CUPNPTMSERVERDEVICEINFO_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::SetDeviceInfoL
// Method is used to provide Nokia specific additional info about the device
// @param aDeviceInfo    XML formatted buffer containg deviceInfo element.
// -------------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmServerDeviceInfo::SetDeviceInfoL( const TDesC8& aDeviceInfo )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEINFO_SETDEVICEINFOL_ENTRY );
    delete iDeviceInfo;
    iDeviceInfo = NULL;
    iDeviceInfo = aDeviceInfo.AllocL();   
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEINFO_SETDEVICEINFOL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::SetBtAddressL
// Method is used to set bluetooth MAC address of the device
// @param aBtAddress   BT MAC address
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmServerDeviceInfo::SetBtAddressL( const TDesC8& aBtAddress )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEINFO_SETBTADDRESSL_ENTRY );
    delete iBtAddr;
    iBtAddr = NULL;
    iBtAddr = aBtAddress.AllocL();
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEINFO_SETBTADDRESSL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmServerDeviceInfo::AddDeviceIconL
// Method is used to provide device icon information.
// @param aDeviceIcon   Pointer to the Terminal Mode Icon object.
//                      Ownership of the object is passed.
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmServerDeviceInfo::AddDeviceIconL( CUpnpTerminalModeIcon* aDeviceIcon )
    {
    OstTraceFunctionEntry0( CUPNPTMSERVERDEVICEINFO_ADDDEVICEICONL_ENTRY );
    iDeviceIconList.AppendL(aDeviceIcon);
    OstTraceFunctionExit0( CUPNPTMSERVERDEVICEINFO_ADDDEVICEICONL_EXIT );
    }

// End of File
