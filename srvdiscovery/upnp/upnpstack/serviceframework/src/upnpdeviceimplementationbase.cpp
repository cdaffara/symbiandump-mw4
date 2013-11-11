/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Defines the CUpnpDeviceImplementationBase class
*
*/


// INCLUDE FILES

#include <e32math.h>
#include <xml/dom/xmlengdom.h> 

#include "upnpfileutils.h"
#include "upnpicon.h"
#include "upnpdeviceimplementationbase.h"
#include "upnpcommonstructs.h"
#include "upnpstring.h"
#include "upnpdispatcher.h"
#include "upnpcons.h"
#include "upnpcommonupnplits.h"
#include "upnpcustomlog.h"
#include "upnpserviceimplementation.h"
#include <upnpdevicedescriptionstore.h>
#include "upnphttpservertransactionhandler.h"


#ifdef _DEBUG
#define KLogFile _L("UPnPStack.txt")
#endif

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::BaseConstructL( const TDesC8& aDeviceDescriptioUri,
                                            CUpnpDeviceDescriptionStore& aDescriptionStore,
                                            MUpnpDeviceDescriptionProvider& aProvider )
    {
    iDescriptionProvider = &aProvider;
    iDescriptionStore = &aDescriptionStore;
    
    iPath = DeviceType().AllocL();

    SetDescriptionUrlL( aDeviceDescriptioUri );

    CreateDispatcherL();
    RegisterEmbeddedDevices();

    iIsRootDevice = ETrue;

    Init();
    
    iServerTransactionHandler = CUpnpHttpServerTransactionHandler::NewL( *this );
    iDispatcher->SetTransactionCreator( iServerTransactionHandler );    
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::RegisterEmbeddedDevices
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::RegisterEmbeddedDevices()
    {
    RPointerArray<CUpnpDevice> devices;
    GetAllDevices(devices);
    for (TInt i = 0; i < iDeviceList.Count(); i++ )
    {
        ((CUpnpDeviceImplementationBase*) iDeviceList[i])->SetDispatcher(iDispatcher);
    }

    devices.Reset();
    devices.Close();
    }


// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::~CUpnpDeviceImplementationBase
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceImplementationBase::~CUpnpDeviceImplementationBase()
    {
    for ( TInt i = ServiceList().Count();  i > 0; i-- )
        {
        if ( ServiceList()[i-1]->IsComplete() )
            {
            ServiceList().Remove( i-1 );
            }
        }
    if ( iIsRootDevice )
        {
        delete iDispatcher;
        }
    delete iPath;        
    delete iServerTransactionHandler;    
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::CUpnpDeviceImplementationBase
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUpnpDeviceImplementationBase::CUpnpDeviceImplementationBase()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::SetDescriptionPropertyL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementationBase::SetDescriptionPropertyL( const TDesC8& aPropertyName, 
                                                                  const TDesC8& aPropertyValue )
    {
    TInt index = 0;

    while ( index < iProperties.Count() )
        {
        CUpnpDescriptionProperty* var = iProperties[index];

        if ( aPropertyName.Compare( var->Name() ) == KErrNone )
            {
            var->SetValueL( aPropertyValue );
            iDescriptionStore->SetPropertyL( aPropertyName, aPropertyValue );
            return;
            }

        index++;
        }
    
    CUpnpDescriptionProperty* newProperty = CUpnpDescriptionProperty::NewL( aPropertyName,
                                                                            aPropertyValue );
    CleanupStack::PushL( newProperty );        
    iProperties.AppendL( newProperty );
    CleanupStack::Pop( newProperty );
    iDescriptionStore->SetPropertyL( aPropertyName, aPropertyValue );
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::RemoveDescriptionPropertyL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementationBase::RemoveDescriptionPropertyL( 
                                                      const TDesC8& aPropertyName )
    {
    TInt index = 0;

    while ( index < iProperties.Count() )
        {
        if ( aPropertyName.Compare( iProperties[index]->Name() ) == KErrNone )
            {
            iDescriptionStore->RemoveTagL( aPropertyName );    
            delete iProperties[index];
            iProperties.Remove( index );
            return;
            }
        index++;
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::RemoveIconL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementationBase::RemoveIconL( const TDesC8& aUrl )
    {    
    iDescriptionStore->RemoveIconL( aUrl );
    
    TInt amount = iIcons.Count();
    
    for(TInt i = 0; i < amount; i++)
        {
        if(!aUrl.Compare(iIcons[i]->Url()))
            {
            delete iIcons[i];
            iIcons.Remove(i);
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::RemoveIconsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementationBase::RemoveIconsL()
    {    
    iDescriptionStore->RemoveIconsL();
    TInt amount = iIcons.Count();
    
    for(TInt i = amount - 1; i >= 0; i--)
        {
        delete iIcons[i];
        iIcons.Remove(i);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::AddIconL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementationBase::AddIconL( const CUpnpIcon& aIcon )
    {    
    iDescriptionStore->AddIconL( aIcon );
    iIcons.Append( &aIcon );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::UpdateIconL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementationBase::UpdateIconL( const CUpnpIcon& aIcon )
    {    
    iDescriptionStore->UpdateIconL( aIcon );
    TInt amount = iIcons.Count();
    
    for(TInt i = 0; i < amount; i++)
        {
        if(!aIcon.Url().Compare(iIcons[i]->Url()))
            {
            delete iIcons[i];
            iIcons.Remove(i);
            iIcons.Insert(&aIcon, i);
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::SaveDescriptionStoreL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::SaveDescriptionStoreL()
    {
    iDescriptionStore->SaveL();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::StartHttpServerL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::StartHttpServerL( const TInt aPort )
    {
    if ( iIsRootDevice ) 
        {
    	iDispatcher->StartHttpServerL( ETrue , aPort );
        }   
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::Dispatcher
// -----------------------------------------------------------------------------
//
CUpnpDispatcher* CUpnpDeviceImplementationBase::Dispatcher()
    {
    return iDispatcher;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::StopHttpServer
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::StopHttpServer()
    {
    iDispatcher->StopHttpServer();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::CreateDispatcherL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::CreateDispatcherL()
    {
    iDispatcher = CUpnpDispatcher::NewL( NULL );
    }

// -----------------------------------------------------------------------------
//  CUpnpDeviceImplementationBase::GenerateUuidL
//  Generates Universally Unique Identifier using good algorithm
//
//  UUID  =  Universally Unique Identifier
//  128 bit UUID (HEX)
//  
//  01234567   - 0123       - 0123                  - 0123       - 012345678901
//  <time_low> - <time_mid> - <time_high & version> - 
//  <clock_seq & reserved><clock_seq_low> - <node> 
//  8"-"4"-"4"-"4"-"12
//  (other items were commented in a header).
// -----------------------------------------------------------------------------
HBufC8* CUpnpDeviceImplementationBase::GenerateUuidL()
    {    
    const TInt smallBlockSize = 4;
    const TInt largeBlockSize = 8;    
    
    //Reserves enough space for the UUID (128bits)
    HBufC8* uuid = HBufC8::NewLC( UpnpString::KMaxUuidLength );  
    
    // setting time
    TTime timex;
    timex.UniversalTime();
    
    HBufC8* currTime = HBufC8::NewLC( largeBlockSize );
    
    TInt timeMaskHi = 0x0FFFFFFF;
    TInt timeMaskVer = 0x10000000;
    
    TInt joinMask = 0x8000;
    TInt rndTimeMask = 0xBFFF;
    TInt bitChange = 0x0000FFFF;
    
    //Returns a 64bit interpretation of the current time
    TInt64 timeValue = timex.Int64();

    TInt32 timeValLow = I64LOW(timeValue);
    TInt32 timeValHi = I64HIGH(timeValue);

    //Joining the Time and Version Elements    
    timeValHi = timeValHi & timeMaskHi;
    timeValHi = timeValHi | timeMaskVer;
    
    currTime->Des().Format( KMask8, timeValHi );
    
    // larger buffer required ... +1
    HBufC8* timeLow = HBufC8::NewLC( largeBlockSize + 1 );
    HBufC8* timeMid = HBufC8::NewLC( smallBlockSize + 1 );
    HBufC8* timeHi  = HBufC8::NewLC( smallBlockSize + 1 );
    
    //Dividing Time element to the wanted parts. 
    timeLow->Des().Format( KMask8, timeValLow );
    
    timeMid->Des().Append( currTime->Des().Right(smallBlockSize) );
    timeHi->Des().Append( currTime->Des().Left(smallBlockSize) );
    
    if ( iClockSeq == NULL )
        {
        //Clock Sequence creation ( random time plus variant )
        iClockSeq = HBufC8::NewL( smallBlockSize + 1 );
        iClockSeq->Des().Zero();
        
        TInt64 seed = timex.Int64();
        TUint32 rndTimeTInt = Math::Rand( seed );
        rndTimeTInt = rndTimeTInt & bitChange;
        rndTimeTInt = rndTimeTInt >> 2; 
        rndTimeTInt = rndTimeTInt | joinMask;
        rndTimeTInt = rndTimeTInt & rndTimeMask;
        
        iClockSeq->Des().Num( (TUint16) rndTimeTInt,EHex );
        HBufC8* buf = iClockSeq->Des().Right( smallBlockSize ).AllocL();
        
        delete iClockSeq;
        iClockSeq = buf;
        }
    else if ( iTimeValue>timeValue )
        {
        TInt rndTimeTInt;  
        if ( UpnpString::StringToInt( iClockSeq->Des(),&rndTimeTInt ) == KErrNone )
            {
            rndTimeTInt++;
            iClockSeq->Des().Num( (TUint16) rndTimeTInt,EHex );
            HBufC8* buf = iClockSeq->Des().Right(smallBlockSize).AllocL();
            delete iClockSeq;
            iClockSeq = buf;
            }
        }
    
    //reading the NodeAddress. 
    iTimeValue = timeValue;
    
    // setting UUID 
    uuid->Des().Zero();
    uuid->Des().Append( KUuid() );
    uuid->Des().Append( timeLow->Des() );
    uuid->Des().Append( KLine );
    uuid->Des().Append( timeMid->Des() );
    uuid->Des().Append( KLine );
    uuid->Des().Append( timeHi->Des() );
    uuid->Des().Append( KLine );
    uuid->Des().Append( iClockSeq->Des() );
    uuid->Des().Append( KLine );
    
    HBufC8* mac=NULL;
    TRAPD(err, mac = const_cast<HBufC8*>( iDispatcher->MacAddressL() ));
    if(!err)
        {    
        // Calculate remaining lenght, MAC can be corrupted and too long for Append
        TInt freeLen = UpnpString::KMaxUuidLength - uuid->Length();
        // If less free lenght than mac lenght, Append only what fits
        if ( freeLen < mac->Length() )
            {
            uuid->Des().Append( mac->Des().Left( freeLen ) );
            }
        // Mac fits, Append all
        else
            {
            uuid->Des().Append( mac->Des() );        
            }
        }
    else
        {
        TInt freeLen = UpnpString::KMaxUuidLength - uuid->Length();
        for(TInt i = 0; i < freeLen; i++)
            {
            TUint32 r = Math::Random();
            const TUint32 KOneHexFromInt32Mask = 0x0000000f;
            r &= KOneHexFromInt32Mask;
            TBuf<1> d;
            d.Num(r,EHex);
            uuid->Des().Append(d);
            }
        }
    
    delete mac;
    mac = NULL;
    
    // PopAndDestroy: timeLow, timeMid, timeHi, currTime;
    CleanupStack::PopAndDestroy( 4 );
    
    CleanupStack::Pop( uuid );
    return uuid;
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::GenerateUDNs
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::GenerateUdnsL( CUpnpDeviceImplementationBase* aDevice )
    {
    
    if ( aDevice->Uuid().Length() <= 0 )
        {
        HBufC8* udnPtr = aDevice->GenerateUuidL();    
        CleanupStack::PushL( udnPtr );

        aDevice->iDescriptionStore->SetPropertyL( KUdn, *udnPtr );
        
        TPtrC8 uuid(*udnPtr);    
        if ( !uuid.Find( UpnpSSDP::KUPnPUuid() ) )
            {
            uuid.Set( uuid.Mid( UpnpSSDP::KUPnPUuid().Size() ) );         
            }                                   
        aDevice->SetUuidL( uuid );
        CleanupStack::PopAndDestroy( udnPtr );                        
        }
              
     RPointerArray<CUpnpDevice>& devices = aDevice->DeviceList();
    
     for ( TInt i(0); i< devices.Count(); i++ )
        {
        GenerateUdnsL( (CUpnpDeviceImplementationBase*) devices[i] );
        }
    }
    

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::AttachServiceL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::AttachServiceL( CUpnpService *aService )
    {
    CUpnpDevice::AttachServiceL( aService );
    if ( iDispatcher )
        {
        ((CUpnpServiceImplementation*) aService)->SetDispatcher( iDispatcher );
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::Address
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpDeviceImplementationBase::Address()
    {
    iAddress = iDispatcher->HttpServerAddress();
    return iAddress;
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::SetDispatcher
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementationBase::SetDispatcher( CUpnpDispatcher* aDispatcher )
    {
    iDispatcher = aDispatcher;    
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::DescritptionProvider
// -----------------------------------------------------------------------------
//
MUpnpDeviceDescriptionProvider& CUpnpDeviceImplementationBase::DescritptionProvider()
    {
    return *iDescriptionProvider;
    }


EXPORT_C void CUpnpDeviceImplementationBase::StartIPFilteringL()
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    iDispatcher->StartIPFilteringL();
    #endif
    }
    
EXPORT_C void CUpnpDeviceImplementationBase::StopIPFiltering()
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    iDispatcher->StopIPFiltering();
    #endif
    }
// End of File



