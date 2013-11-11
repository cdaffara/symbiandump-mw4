/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles Remote personalities change
*
*/


#include <e32base.h>
#include <e32std.h>
#include <versioninfo.h>
#include <d32usbc.h>
#include <usbman.h>
#include <usbwatcher.h>

#include "cremotepersonalityhandler.h"
#include "csetpersonality.h" 
#include "debug.h"

const TUint KValueLoByte = 2;
const TUint KValueHiByte = 3;
const TUint KIndexLoByte = 4;
const TUint KIndexHiByte = 5;
const TUint KLengthLoByte = 6;
const TUint KLengthHiByte = 7;

const TUint KOneByte = 8; // for shifting data to one byte

const TUint KGetPersonalitiesHeaderLen = 4;
const TUint KItemsPerPersonality = 2;

const TUint8 KStringDescriptorsBase = 0xED; // string descriptors will be written starting from this index, descendingly; 0xEE is used for OS string descriptor

const TUint KAllPersonalitiesDescriptorType = 0x12; // All Personalities Descriptor type 

const TUint KSetupPacketLength = 8; // 8 bytes

const TUint KS6032MajorNumber = 3; // 3.2 major number is 3

const TInt K32DevicePCSuite = 113;
const TInt K32DeviceMS = 114;
const TInt K32DevicePTP = 115;

const TInt KHostPCSuite = 1;
const TInt KHostMS = 2;
const TInt KHostPTP = 3;

// ---------------------------------------------------------------------------
// Decoding EP0 buffer
// ---------------------------------------------------------------------------
//
void TSetupPacket::Decode(const RBuf8& aSetupPacket)
    {
        
    if (aSetupPacket.Length() < KSetupPacketLength)
        {
        iRequest = CRemotePersonalityHandler::EUnknown; 
        return;
        }
    
    iType           = aSetupPacket[0];
    iRequest        = static_cast<CRemotePersonalityHandler::TRequest>(aSetupPacket[1]);
    iValue          = static_cast<TUint16>(aSetupPacket[KValueLoByte] | 
                                            (aSetupPacket[KValueHiByte] << KOneByte) );
    iIndex          = static_cast<TUint16>(aSetupPacket[KIndexLoByte] |
                                            (aSetupPacket[KIndexHiByte] << KOneByte) );
    iLength         = static_cast<TUint16>(aSetupPacket[KLengthLoByte] |
                                            (aSetupPacket[KLengthHiByte] << KOneByte) );
    }

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//  
CRemotePersonalityHandler* CRemotePersonalityHandler::NewL()
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::NewL" ) );
    
    CRemotePersonalityHandler* self = new (ELeave) CRemotePersonalityHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------
//
CRemotePersonalityHandler::CRemotePersonalityHandler() : 
                                        iLastResult(EUndefinedError)
    {
    iSetupPacket.iRequest = CRemotePersonalityHandler::EUnknown;
    }
        
// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
void CRemotePersonalityHandler::ConstructL()
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::ConstructL" ) );
    iSetPersonalityHandler = CSetPersonality::NewL(*this);
    
    iMappingIsNeeded = IsMappingNeededL();
    }
    
// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
CRemotePersonalityHandler::~CRemotePersonalityHandler()
    {
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::~CRemotePersonalityHandler" ) );
    
    delete iSetPersonalityHandler;
                 
    iPersonalities.Close(); // T-classes' objects in RArray do not require to be "destroyed"
            
    }   
    
// ---------------------------------------------------------------------------
// SetPersonality request has been completed
// ---------------------------------------------------------------------------
//
void CRemotePersonalityHandler::SetPersonalityCallBack(TLastResult aResult)
    {
    
    FTRACE(FPrint(
          _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::SetPersonalityCallBack aResult = %d" ), aResult));
    
    iLastResult = aResult;
    iSetupPacket.iRequest = CRemotePersonalityHandler::EUnknown;
    
    }

// ---------------------------------------------------------------------------
// Personality-related requests handler
// ---------------------------------------------------------------------------
//
TInt CRemotePersonalityHandler::Handle(const RBuf8& aSetupBuffer, RBuf8& aData)
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::Handle" ) );
    
    TRAPD(err, DoHandleL(aSetupBuffer, aData));
    
    return static_cast<TLastResult>(err);
    
    }

// ---------------------------------------------------------------------------
// Personality-related requests internal handler
// ---------------------------------------------------------------------------
//
void CRemotePersonalityHandler::DoHandleL(const RBuf8& aSetupBuffer, RBuf8& aData)
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::DoHandleL" ) );
    
    iSetupPacket.Decode(aSetupBuffer);
    
    FTRACE(FPrint(
           _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::DoHandleL: Request = %d" ), iSetupPacket.iRequest));
    
    switch(iSetupPacket.iRequest)
        {
        
        case EGetAllPersonalities       :
            {
            iLastResult = EUndefinedError; // will be updated after completing the request
            
            GetPersonalitiesL(aData);
            
            iLastResult = ESuccess; //static_cast<TLastResult>(err);
            
            break;
            }
        
        case EGetLastResult             :
            {
            GetLastResultL(aData);
            break;
            }
        
        case ESetPersonality            :
            {
            
            iLastResult = EUndefinedError; // will be updated after copmleting the request

            SetPersonalityL();
            
            iLastResult = EDataTransferInProgress; 
            
            break;
            
            }
                
        case EGetPersonalityDescriptor  :
        case EGetPersonality            :
        case EGetLockState              :
        case ESetLockState              :
        
        default:
            {
                
            FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::Handle ***Request Is Not Supported***" ) );
            
            User::Leave(KErrNotSupported); 
            
            }
        }
     
    }
    
// ---------------------------------------------------------------------------
// Set links to needed services
// ---------------------------------------------------------------------------
//
void CRemotePersonalityHandler::Initialize( RDevUsbcClient& aLdd, 
                                            RUsbWatcher& aUsbWatcher,
                                            RUsb& aUsbManager)
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::Initialize" ) );
    
    iUsbWatcher = &aUsbWatcher;
    iUsbManager = &aUsbManager;
    iLdd = &aLdd;
    
    iSetPersonalityHandler->SetUsbWatcher(iUsbWatcher);
    
    // Read personalities
    TRAPD(err, ReadPersonalitiesL());
    
    FTRACE(FPrint(
           _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::Initialize: ReadPersonalities err  = %d" ), err));
    
    // Save personalities descriptions, to enable read of them by standard GetDescriptor request 
    TRAP(err, SavePersonalitiesStringsL());
    
    FTRACE(FPrint(
           _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::Initialize: SavePersString err  = %d" ), err));
   
    }

// ---------------------------------------------------------------------------
// Process GetAllPersonalities request
// ---------------------------------------------------------------------------
//
void CRemotePersonalityHandler::GetPersonalitiesL(RBuf8& aPersonalities)
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::GetPersonalities" ) );
    
    // check the request
    if((iSetupPacket.iValue != 0) || (iSetupPacket.iIndex != 0))
        {

        FLOG( _L( "[USBREMOTEPERSONALITY]\t**** CRemotePersonalityHandler::GetPersonalities SetupPacket has wrong data *****" ) );
        User::Leave(EInvalidRequest);

        }

    TInt8 responseLength(KGetPersonalitiesHeaderLen+iPersonalities.Count()*KItemsPerPersonality); // 4 mandatory bytes for header + 2 bytes per personality

    FTRACE(FPrint(
           _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::GetPersonalities Response length is %d bytes" ), responseLength));
    
    aPersonalities.Close();
    aPersonalities.CreateL(responseLength);
    
    // Panic on Append never can be rised in this method, due to aPersonalities length exactly equal the appending data length.  
    aPersonalities.Append(responseLength);
    aPersonalities.Append(KAllPersonalitiesDescriptorType); // All Personalities Descriptor type 
    
    TInt err(ESuccess);
    TInt currentPersonalityId;
    
    err = iUsbManager->GetCurrentPersonalityId(currentPersonalityId);
    if(ESuccess != err)
        {
        User::Leave(EUndefinedError);
        }
        
    FTRACE(FPrint(
          _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::GetPersonalities Current personality Id is %d" ), currentPersonalityId));
    
    // in S60 3.2 or older, map some personality ids into newer set
    if(iMappingIsNeeded)
    	{
    	currentPersonalityId = MapPersonalityIdFromDeviceToHostL(currentPersonalityId);
    	}
    
    aPersonalities.Append(static_cast<TInt8>(currentPersonalityId));
    aPersonalities.Append(static_cast<TInt8>(iPersonalities.Count()));
    
    TUint counter(KGetPersonalitiesHeaderLen); // counter for aPersonalities descriptor, 4 bytes already written
    
    for(TUint i(0); i < iPersonalities.Count(); ++i, counter = counter + KItemsPerPersonality)
        {
        
        TPersonality personality;
        
        if(iMappingIsNeeded)
        	{
        	personality.iId = MapPersonalityIdFromDeviceToHostL(iPersonalities[i].iId);
        	}
        	else
        	{
        	personality.iId = iPersonalities[i].iId;	
        	}
        	
        aPersonalities.Append(static_cast<TInt8>(personality.iId));
        aPersonalities.Append(static_cast<TInt8>(iPersonalities[i].iIndex));        
    
        FTRACE(FPrint(
           _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::GetPersonalities Added personality id %d Index %d" ), aPersonalities[counter], aPersonalities[counter + 1]));

        }
    
    }

// ---------------------------------------------------------------------------
// Reads personalities 
// ---------------------------------------------------------------------------
//
void CRemotePersonalityHandler::ReadPersonalitiesL()
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::ReadPersonalitiesL" ) );
    
    RArray<TInt> personalityIds;
    CleanupClosePushL(personalityIds);
    
    User::LeaveIfError(iUsbManager->GetPersonalityIds(personalityIds));
    
    FTRACE(FPrint(
           _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::ReadPersonalities There are %d personalities supported" ), personalityIds.Count()));
    
    // save ids to iPersonalities           
    iPersonalities.Reset();
    TPersonality p;
    
    for(TUint i(0); i < personalityIds.Count(); ++i)
        {
    
        p.iId = personalityIds[i];
        p.iIndex = KStringDescriptorsBase - i;
        
        // iPersonalities is a dynamic array, no error handling is needed on Append    
        iPersonalities.AppendL(p);
            
        FTRACE(FPrint(
          _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::ReadPersonalities Personality id = %d Index = %d" ), iPersonalities[i].iId, iPersonalities[i].iIndex));

        }
        
    CleanupStack::PopAndDestroy(&personalityIds); // personalityIds

    }

// ---------------------------------------------------------------------------
// Saves personalities descriptions as standard usb string descriptors
// ---------------------------------------------------------------------------
//  
void CRemotePersonalityHandler::SavePersonalitiesStringsL()
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::SavePersonalitiesStringsL" ) );
    
    HBufC* description; // personality description
    for(TUint i(0); i<iPersonalities.Count(); ++i)
        {
        // gets description; data owenerships hands over to caller
        User::LeaveIfError(iUsbManager->GetDescription(iPersonalities[i].iId, description));
        
        FTRACE(FPrint(
          _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::SavePersonalitiesStrings Personality Id = %d Description length =  %d" ), iPersonalities[i].iId, description->Length()));
        
        // save string to repository
        User::LeaveIfError(iLdd->SetStringDescriptor(iPersonalities[i].iIndex, *description));
        
        FTRACE(FPrint(
          _L("[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::SavePersonalitiesStrings Personality description saved with index %d" ), iPersonalities[i].iIndex));

        delete description;
        description = 0;
        }
        
    }
    
// ---------------------------------------------------------------------------
// Process SetPersonality request
// ---------------------------------------------------------------------------
//  
void CRemotePersonalityHandler::SetPersonalityL()
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::SetPersonality" ) );
    
    // check the request
    if((iSetupPacket.iLength != 0) || (iSetupPacket.iIndex != 0))
        {

        FLOG( _L( "[USBREMOTEPERSONALITY]\t**** CRemotePersonalityHandler::SetPersonality SetupPacket has wrong data *****" ) );
        User::Leave(EInvalidRequest);

        }
    
    if(iMappingIsNeeded)
    	{
    	iSetupPacket.iValue = MapPersonalityIdFromHostToDeviceL(iSetupPacket.iValue);
    	}
    	
    // due to watcher process SetPersonality somehow strange, here is check for valid id	
    for(TUint i(0); i < iPersonalities.Count(); ++i)
    	{
    	if(iSetupPacket.iValue == iPersonalities[i].iId)
    		{
    		// set personality
    		iSetPersonalityHandler->SetPersonality(iSetupPacket.iValue);
    		return;
    		}
    	}
    	
    // did not find personality id in list of supported personalities
    iLastResult = ENonExistingPersonality;
    iSetupPacket.iRequest = CRemotePersonalityHandler::EUnknown;
    User::Leave(ENonExistingPersonality);    
    
    }

// ---------------------------------------------------------------------------
// Process GetLastResult request
// ---------------------------------------------------------------------------
//  
void CRemotePersonalityHandler::GetLastResultL(RBuf8& aLastResult)
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCRemotePersonalityHandler::GetLastResult" ) );
    
    // check the request
    if((iSetupPacket.iValue != 0) || (iSetupPacket.iIndex != 0))
        {

        FLOG( _L( "[USBREMOTEPERSONALITY]\t**** CRemotePersonalityHandler::GetLastResult SetupPacket has wrong data *****" ) );
        User::Leave(EInvalidRequest);

        }
    
    aLastResult.Close();
    aLastResult.CreateL(1); // Length of response to GetLastResult request is 1 byte always.
    
    // Panic on Append never can be rised here, due to aPersonalities length exactly equal the appending data length.  
    aLastResult.Append(static_cast<TInt8>(iLastResult));
    
}

TBool CRemotePersonalityHandler::IsMappingNeededL()
{
	VersionInfo::TPlatformVersion platformVersion;
	User::LeaveIfError( VersionInfo::GetVersion( platformVersion ) );
	
	if(platformVersion.iMajorVersion > KS6032MajorNumber) return EFalse;
	
	return ETrue;	
}

TInt CRemotePersonalityHandler::MapPersonalityIdFromDeviceToHostL(TInt aPersonality)
{
	switch(aPersonality)
	{
		case K32DevicePCSuite: return KHostPCSuite;
		case K32DeviceMS: return KHostMS;
		case K32DevicePTP: return KHostPTP;
		
		default: return aPersonality;	
	}
}

TInt CRemotePersonalityHandler::MapPersonalityIdFromHostToDeviceL(TInt aPersonality)
{
	switch(aPersonality)
	{
		case KHostPCSuite: return K32DevicePCSuite;
		case KHostMS: return K32DeviceMS;
		case KHostPTP: return K32DevicePTP;
		
		default: return aPersonality;	
	}
	
}


