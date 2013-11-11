/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles remote personality - related requests
*
*/


#ifndef CREMOTEPERSONALITYHANDLER_H
#define CREMOTEPERSONALITYHANDLER_H

#include <cusbdevicecontrolplugin.h>

class RUsbWatcher;
class RUsb;
class RDevUsbcClient;
class CSetPersonality;

struct TPersonality
    {
        
    TUint8 iId;     // Personality id
    TUint8 iIndex;  // Index of a string descriptor
    
    };
    
/**
 *  Setup packet interpretation
 *
 *
 *  @lib usbdevcon.lib
 *  @since S60 v.5.0 
 */
 class TSetupPacket
 {
 public:
    
    /**
     * Decodes buffer as a SetupPacket
     *
     * @since S60 v.5.0
     * @param aSetupPacket incoming buffer to be decoded
     */
    void Decode(const RBuf8& aSetupPacket);
 
 public:
 
    TUint8       iType;      // control message type
    TInt            iRequest;   // request; has type TInt because can be negative: CRemotePersonalityHandler::EUnknown;
    TUint16     iValue;     // value
    TUint16     iIndex;     // index
    TUint16     iLength;    // length
 } ;
 
/**
 *  Remote Personality Requests handler
 *
 *  @lib usbdevcon.lib
 *  @since S60 v.5.0 
 */
 
class CRemotePersonalityHandler : public CUsbCMHandler
    {

public:
    
    /**  Last result */
    enum TLastResult
    {
        ESuccess =              0x00,
        EUndefinedError =       0x01,
        EInvalidRequest=        0x02,
        ERequestIsNotSupported= 0x03,
        EFeatureIsNotSupported= 0x04,
        ENonExistingPersonality=0x10,
        EDataTransferInProgress=0x11,
        EEntityIsLocked=        0x12,
        ENonExistingEntity=     0x20,
        EInvalidLockState=      0x21,
        EInvalidPassword=       0x22,
        ENoAttemptsLeft=        0x23
        
        // 0x80 ..0xFE reserved for vendor usage
        
    };
    
    /**  Requests */
    enum TRequest
    {
        EUnknown                    = -1,
        EGetAllPersonalities        = 0x82,
        EGetLastResult              = 0x84,
        EGetPersonalityDescriptor   = 0x86,
        EGetPersonality             = 0x88,
        ESetPersonality             = 0x89,
        EGetLockState               = 0x8A,
        ESetLockState               = 0x8B
    };
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     * @return Constructed instance
     */ 
    static CRemotePersonalityHandler* NewL();
    
    /**
    * Destructor.
    *
    * @since S60 v.5.0
    */
    virtual ~CRemotePersonalityHandler();
    
    /**
     * Called back when SetPersonality request is completed
     * @param aResult Result of the SetPersonality request
     * @since S60 v.5.0
     */
    void SetPersonalityCallBack(TLastResult aResult);
    
    // From MUsbCMHandler
    
    /**
     * Handler's Handle
     *
     * @since S60 v.5.0
     * @param aSetupPacket request to be handled
     * @param aData Either data from host to device, or data from device to host
     * @return Errorcode
     */
    TInt Handle(const RBuf8& aSetupPacket, RBuf8& aData);
    
    /**
     * Initializer
     *
     * @since S60 v.5.0
     * @param aLdd Link to LDD services
     * @param aUsbWatcher Link to USB Watcher services
     * @param aUsbManager Link to USB Manager services
     */
    void Initialize(RDevUsbcClient& aLdd, RUsbWatcher& aUsbWatcher, RUsb& aUsbManager);
        
private:
    
    /**
     * Default constructor.
     *
     * @since S60 v.5.0
     */ 
    CRemotePersonalityHandler();

    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     */ 
    void ConstructL();
   
   /**
     * Internal handler - leaves if error
     *
     * @since S60 v.5.0
     * @param aSetupPacket request to be handled
     * @param aData Either data from host to device, or data from device to host
     */
    void DoHandleL(const RBuf8& aSetupPacket, RBuf8& aData);
     
    /**
     * Handles GET_ALL_PERSONALITIES request
     *
     * @since S60 v.5.0
     * @param aData Contains result on exit
     */
    void GetPersonalitiesL(RBuf8& aData);
    
    /**
     * Handles SET_PERSONALITY request
     *
     * @since S60 v.5.0
     */
    void SetPersonalityL();
    
    /**
     * Handles GET_LAST_RESULT request
     * @param aData Last result
     * @since S60 v.5.0
     */
    void GetLastResultL(RBuf8& aData);
    
    /**
     * Read personalities from USB Manager to iPersonalities
     *
     * @since S60 v.5.0
     */
    void ReadPersonalitiesL();
    
    /**
     * Saves personalities descriptions to USB standard strings descriptors
     *
     * @since S60 v.5.0
     */
    void SavePersonalitiesStringsL();
    
    /**
     * Check if mapping of ids is needed
     *
     * @since S60 v.5.0
     */
    TBool IsMappingNeededL();
    
    /**
     * Maps old personality ids (3.2) to new ones (5.0)
     *
     * @since S60 v.5.0
     */
    TInt MapPersonalityIdFromDeviceToHostL(TInt aPersonalityId);
    
    /**
     * Maps old personality ids (3.2) to new ones (5.0)
     *
     * @since S60 v.5.0
     */
    TInt MapPersonalityIdFromHostToDeviceL(TInt aPersonalityId);
        
private: // data
    
    /**
     * Result of the last operation
     *   
     */
    TLastResult iLastResult;
    
    /**
     * USB watcher
     * Not owns.  
     */
    RUsbWatcher* iUsbWatcher;
    
    /**
     * USB Manager
     * Not owns.  
     */
    RUsb* iUsbManager;
    
    /**
     * USB client
     * Not owns. 
     */
    RDevUsbcClient* iLdd;
    
    /**
     * Request, being processed currently
     *   
     */
    TSetupPacket iSetupPacket;
    
    /**
     * Supported personalities
     *   
     */
    RArray<TPersonality> iPersonalities;
    
     /**
     * SetPersonality request handler
     *   
     */
    CSetPersonality* iSetPersonalityHandler;
    
    /**
     * ETrue if mapping is needed from old ids set to new one and vice versa 
     *   
     */
    TBool iMappingIsNeeded;
    
    };

#endif // CREMOTEPERSONALITYHANDLER_H
