// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 

#include "bulkonlytransport.h"
#include "mscfileserver.h"
#include "usbmscfileshared.h"
#include "debug.h"
#define InEndpoint EEndpoint1
#define OutEndpoint EEndpoint2
//This value defined in USB Mass Storage Bulk Only Transrt spec and not supposed to be changed
LOCAL_D const TInt KRequiredNumberOfEndpoints = 2; // in addition to endpoint 0. 


//CBW offsets
LOCAL_D const TInt KCbwSignatureOffset = 0;
LOCAL_D const TInt KCbwTagOffset = 4;
LOCAL_D const TInt KCbwDataTransferLengthOffset = 8;
LOCAL_D const TInt KCbwFlagOffset = 12;
LOCAL_D const TInt KCbwLunOffset = 13;
LOCAL_D const TInt KCbwCbLengthOffset = 14;
LOCAL_D const TInt KMaxCbwcbLength = 16;
// CSW offsets
LOCAL_D const TInt KCswSingnatureOffset = 0;
LOCAL_D const TInt KCswTagOffset = 4;
LOCAL_D const TInt KCswDataResidueOffset = 8;
LOCAL_D const TInt KCswStatusOffset = 12;
LOCAL_D const TInt KCswLength = 13;
LOCAL_D const TInt KUsbNumInterfacesOffset = 4;

/**
 This function unpacks into the TUsbRequestHdr class from a descriptor with 
 the alignment that would be introduced on the USB bus.
 
 @param aBuffer Input buffer
 @param aTarget Unpacked header.
 @return Error.
 */
TInt TUsbRequestHdr::Decode(const TDesC8& aBuffer)

    {
    if (aBuffer.Length() < static_cast<TInt> (KRequestHdrSize))
        {
        TRACE_ERROR((_L("TUsbRequestHdr::Decode buffer invalid length %d"),aBuffer.Length()))
        return KErrGeneral;
        }

    // Decode as SPEC 1.3.3
    iRequestType = aBuffer[0];
    iRequest = static_cast<TEp0Request>(aBuffer[1]);
    iValue = static_cast<TUint16>(aBuffer[2] + (aBuffer[3] << 8));
    iIndex = static_cast<TUint16>(aBuffer[4] + (aBuffer[5] << 8));
    iLength = static_cast<TUint16>(aBuffer[6] + (aBuffer[7] << 8));
    TRACE_INFO((_L("type=%d request=%d value=%d index=%d length=%d"), iRequestType,iRequest,iValue,iIndex,iLength))

    return KErrNone;
    }

/**
 This function determines whether data is required by the host in response 
 to a message header.

 @return TBool	Flag indicating whether a data response required.
 */
TBool TUsbRequestHdr::IsDataResponseRequired() const

    {
    return (iRequestType & 0x80) ? ETrue : EFalse;
    }

//
/**
 Called by CBulkOnlyTransport to create an instance of CControlInterface

 @param aParent reference to the CBulkOnlyTransport
 */
CControlInterface* CControlInterface::NewL(CBulkOnlyTransport& aParent)
    {
    CControlInterface* self = new (ELeave) CControlInterface(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    CActiveScheduler::Add(self);
    CleanupStack::Pop();
    return self;
    }

void CControlInterface::ConstructL()
    {
    }

/**
 c'tor

 @param aParent reference to the CBulkOnlyTransport
 */
CControlInterface::CControlInterface(CBulkOnlyTransport& aParent) :
	CActive(EPriorityStandard),
	iParent(aParent),
	iCurrentState(ENone)
    {
    }

/**
 d'tor
 */
CControlInterface::~CControlInterface()
    {
    TRACE_FUNC
	Cancel();
    }

/**
 Called by  CBulkOnlyTransportStart to start control interface
 */
TInt CControlInterface::Start()
    {
    TRACE_FUNC
    TInt res = ReadEp0Data();
    return (res);
    }

/**
 Called by CBulkOnlyTransportStart to stop control interface
 */
void CControlInterface::Stop()
    {
    TRACE_FUNC
    // Don't need to be any thing if we're not in active state
    if (!IsActive())
        {
        TRACE_INFO((_L("Not active")))
        return;
        }

    TRACE_INFO((_L("Stopping...")))

    // Stop by cancel
    Cancel();
    iCurrentState = ENone;
    }

/**
 Cancel outstanding request (if any)
 */
void CControlInterface::DoCancel()
    {
    TRACE_FUNC
    switch(iCurrentState)
        {
        case EReadEp0Data:
        iParent.Ldd().ReadCancel(EEndpoint0);
        break;
        case ESendMaxLun:
        iParent.Ldd().WriteCancel(EEndpoint0);
        break;
        default:
        TRACE_ERROR((_L("\nWrong state !")))
        __ASSERT_DEBUG(EFalse, User::Panic(KUsbMsSvrPncCat, EMsControlInterfaceBadState));
        }
    }

/**
 Implement CControlInterface state machine
 */
void CControlInterface::RunL()
    {
    TRACE_FUNC
    if (iStatus != KErrNone)
        {
        TRACE_ERROR(( _L( "Error %d in RunL" ), iStatus.Int() ))

        //read EP0  again 
        ReadEp0Data();
        return;
        }

    switch (iCurrentState)
        {
        case ESendMaxLun:
            ReadEp0Data();
            break;

        case EReadEp0Data:
            DecodeEp0Data();
            break;

        default:
            TRACE_ERROR(( _L( "error: (Shouldn't end up here...)" ) ))
            __ASSERT_DEBUG(EFalse, User::Panic(KUsbMsSvrPncCat, EMsControlInterfaceBadState));
            break;
        }
    return;
    }

/**
 Post a read request to EEndpoint0 to read request header
 */
TInt CControlInterface::ReadEp0Data()
    {
    TRACE_FUNC
    if ( IsActive() )
        {
        TRACE_ERROR(( _L( "Still active" ) ))
        return KErrServerBusy;
        }

    iParent.Ldd().Read(iStatus, EEndpoint0, iData, KRequestHdrSize);

    iCurrentState = EReadEp0Data;

    SetActive();
    return KErrNone;
    }

/**
 Decode request header and do appropriate action - get max LUN info or post a reset request
 */
void CControlInterface::DecodeEp0Data()
    {
    TRACE_FUNC
    if ( IsActive() )
        {
        TRACE_ERROR(( _L( "Still active" ) ))
        __ASSERT_DEBUG(EFalse, User::Panic(KUsbMsSvrPncCat, EMsControlInterfaceStillActive));
        return;
        }

    TInt err = iRequestHeader.Decode(iData);

    if (err != KErrNone)
        {
        TRACE_WARNING((_L("Decode header error : err=%d"), err))
        return;
        }

    switch(iRequestHeader.iRequest)
        {
        // GET MAX LUN (0xFE)
        case TUsbRequestHdr::EReqGetMaxLun:
            {
            TRACE_INFO((_L("DecodeEp0Data : 'Get Max LUN' Request MaxLun = %d" ),
                            iParent.MaxLun() ))

            if ( iRequestHeader.iRequestType != 0xA1 //value from USB MS BOT spec
                    || iRequestHeader.iIndex> 15 || iRequestHeader.iValue != 0 || iRequestHeader.iLength != 1)
                {
                TRACE_ERROR((_L("GetMaxLun command packet check error")))
                iParent.Ldd().EndpointZeroRequestError();
                break;
                }

            iData.FillZ(1); //Return only 1 byte to host 
            iData[0] = static_cast<TUint8>(iParent.MaxLun()); // Supported Units
            iParent.Ldd().Write(iStatus, EEndpoint0, iData, 1);

            iCurrentState = ESendMaxLun;
            SetActive();

            return;
            }

        // RESET (0xFF)
        case TUsbRequestHdr::EReqReset:
            {
            TRACE_INFO((_L("DecodeEp0Data : 'Mass Storage Reset' Request")))

            if ( iRequestHeader.iRequestType != 0x21 //value from USB MS BOT spec
                    || iRequestHeader.iIndex> 15 || iRequestHeader.iValue != 0 || iRequestHeader.iLength != 0)
                {
                TRACE_ERROR((_L("MSC Reset command packet check error")))
                iParent.Ldd().EndpointZeroRequestError();
                break;
                }

            iParent.HwStop();
            iParent.Controller().Reset();
            iParent.HwStart(ETrue);

            err = iParent.Ldd().SendEp0StatusPacket();
            TRACE_INFO((_L("SendEp0StatusPacket: err=%d"), err))

            return;
            }
        // Unknown?
        default:
            {
            TRACE_ERROR((_L("DecodeEp0Data : Unknown Request")))
            }
        }
    ReadEp0Data(); //try to get another request 
    }

//-------------------------------------
/**
 Create CBulkOnlyTransport object
 @param aNumDrives - The number of drives available for MS
 @param aController - reference to the parent
 @return pointer to newly created object
 */
CBulkOnlyTransport* CBulkOnlyTransport::NewL(TInt aNumDrives,CMscFileController& aController)
    {
	CBulkOnlyTransport* self = new(ELeave) CBulkOnlyTransport(aNumDrives, aController);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 c'tor 
 @param aNumDrives - The number of drives available for MS
 @param aController - reference to the parent
 */
CBulkOnlyTransport::CBulkOnlyTransport(TInt aNumDrives,CMscFileController& aController):
	CActive(EPriorityStandard),
	iMaxLun(aNumDrives-1),
	iController(aController),
	iReadBuf(NULL,0),
	iWriteBuf(NULL,0),
	iStallAllowed(ETrue)
    {
    }

/**
 Constructs the CBulkOnlyTranspor object
 */
void CBulkOnlyTransport::ConstructL()
    {
    TRACE_FUNC
    iControlInterface = CControlInterface::NewL(*this);
    iDeviceStateNotifier = CActiveDeviceStateNotifier::NewL(*this);
    CActiveScheduler::Add(this);
    }

/**
 Destructor
 */
CBulkOnlyTransport::~CBulkOnlyTransport()
    {
    TRACE_FUNC
    if (iInterfaceConfigured)
        {
        Stop();
        }
    delete iControlInterface;
    delete iDeviceStateNotifier;
    }

/**
 Set or unset configuration descriptor for USB MassStorage Bulk Only transport

 @param aUnset indicate whether set or unset descriptor
 @return KErrNone if operation was completed successfully, errorcode otherwise
 */
TInt CBulkOnlyTransport::SetupConfigurationDescriptor(TBool aUnset)
    {
    TRACE_FUNC
    TInt ret(KErrNone);
    TInt configDescriptorSize(0);

    iLdd.GetConfigurationDescriptorSize(configDescriptorSize);
    if (static_cast<TUint> (configDescriptorSize) != KUsbDescSize_Config)
        {
        return KErrCorrupt;
        }

    TBuf8<KUsbDescSize_Config> configDescriptor;
    ret = iLdd.GetConfigurationDescriptor(configDescriptor);
    if (ret != KErrNone)
        {
        return ret;
        }

    // I beleive that other fields setted up during LDD initialisation
    if (aUnset)
        {
        --configDescriptor[KUsbNumInterfacesOffset];
        }
    else
        {
        ++configDescriptor[KUsbNumInterfacesOffset];
        }
    ret = iLdd.SetConfigurationDescriptor(configDescriptor);

    return ret;
    }

/**
 Set up interface descriptor

 @return KErrNone if operation was completed successfully, errorcode otherwise
 */
TInt CBulkOnlyTransport::SetupInterfaceDescriptors()
    {
    TRACE_FUNC
    // Device caps
    TUsbDeviceCaps d_caps;
    TInt ret = iLdd.DeviceCaps(d_caps);
    if (ret != KErrNone)
        {
        return ret;
        }
    TInt totalEndpoints = d_caps().iTotalEndpoints;
    if (totalEndpoints < KRequiredNumberOfEndpoints)
        {
        return KErrHardwareNotAvailable;
        }

    // Endpoint caps
    TUsbcEndpointData data[KUsbcMaxEndpoints];
	TPtr8 dataptr(reinterpret_cast<TUint8*>(data), sizeof(data), sizeof(data));
    ret = iLdd.EndpointCaps(dataptr);
    if (ret != KErrNone)
        {
        return ret;
        }

    // Set the active interface
    TUsbcInterfaceInfoBuf ifc;
    TInt ep_found = 0;
    TBool foundBulkIN = EFalse;
    TBool foundBulkOUT = EFalse;

    for (TInt i = 0; i < totalEndpoints; i++)
        {
        const TUsbcEndpointCaps* caps = &data[i].iCaps;
        const TInt maxPacketSize = caps->MaxPacketSize();
		if (!foundBulkIN &&
			(caps->iTypesAndDir & (KUsbEpTypeBulk | KUsbEpDirIn)) == (KUsbEpTypeBulk | KUsbEpDirIn))
            {
            // InEndpoint is going to be our TX (IN, write) endpoint
            ifc().iEndpointData[0].iType = KUsbEpTypeBulk;
            ifc().iEndpointData[0].iDir = KUsbEpDirIn;
            ifc().iEndpointData[0].iSize = maxPacketSize;
            ifc().iEndpointData[0].iInterval_Hs = 0;
			ifc().iEndpointData[0].iFeatureWord1 = KUsbcEndpointInfoFeatureWord1_DMA;
            foundBulkIN = ETrue;
            if (++ep_found == KRequiredNumberOfEndpoints)
                {
                break;
                }
            continue;
            }
		if (!foundBulkOUT &&
			(caps->iTypesAndDir & (KUsbEpTypeBulk | KUsbEpDirOut)) == (KUsbEpTypeBulk | KUsbEpDirOut))
            {
            // OutEndpoint is going to be our RX (OUT, read) endpoint
            ifc().iEndpointData[1].iType = KUsbEpTypeBulk;
            ifc().iEndpointData[1].iDir = KUsbEpDirOut;
            ifc().iEndpointData[1].iSize = maxPacketSize;
            ifc().iEndpointData[1].iInterval_Hs = 0;
			ifc().iEndpointData[1].iFeatureWord1 = KUsbcEndpointInfoFeatureWord1_DMA;
            foundBulkOUT = ETrue;
            if (++ep_found == KRequiredNumberOfEndpoints)
                {
                break;
                }
            continue;
            }
        }
    if (ep_found != KRequiredNumberOfEndpoints)
        {
        return KErrHardwareNotAvailable;
        }
    _LIT16(string, "USB Mass Storage Interface");
    ifc().iString = const_cast<TDesC16*> (&string);
    ifc().iTotalEndpointsUsed = KRequiredNumberOfEndpoints;
    ifc().iClass.iClassNum = 0x08; // Mass Storage
    ifc().iClass.iSubClassNum = 0x06; // SCSI Transparent Command Set
    ifc().iClass.iProtocolNum = 0x50; // Bulk Only Transport

	TUint bandwidth_priority = (EUsbcBandwidthOUTDefault | EUsbcBandwidthINDefault);
    if (d_caps().iHighSpeed)
        {
        // If this device supports USB High-speed, then we request 64KB buffers
        // (otherwise the default 4KB ones will do).
        bandwidth_priority = (EUsbcBandwidthOUTPlus2 | EUsbcBandwidthINPlus2);
        // Also, tell the Protocol about it, because it might want to do some
        // optimizing too.
        iProtocol->ReportHighSpeedDevice();
        }
    ret = iLdd.SetInterface(0, ifc, bandwidth_priority);
    return ret;
    }

/**
 Called by the protocol after processing the packet to indicate that more data is required.  

 @param aData reference to the data buffer. 
 */
void CBulkOnlyTransport::SetupReadData(TPtr8& aData)
    {
    TRACE_FUNC
    TRACE_INFO((_L("Length = %d  (bytes)"), aData.Length()))
    iReadBuf.Set(aData);
    iReadSetUp = ETrue;
    }

/**
 Called by the protocol after processing the packet to indicate that data should be written to the host.  

 @param aData reference to the data buffer. 
 */
void CBulkOnlyTransport::SetupWriteData(TPtrC8& aData)
    {
    TRACE_FUNC
    TRACE_INFO((_L("Length = %d  (bytes)"), aData.Length()))
    iWriteBuf.Set(aData);
    iWriteSetUp = ETrue;
    }

TInt CBulkOnlyTransport::Start()
    {
    TRACE_FUNC_ENTRY

    TInt err = KErrNone;

    if (!iProtocol)
        {
        return KErrBadHandle; //protocol should be set up before start
        }

    if (IsActive())
        {
        TRACE_ERROR((_L("Active before start!")))
        return KErrInUse;
        }

    if ((err = iLdd.Open(0)) != KErrNone )
        {
        TRACE_ERROR((_L("Error during open ldd!")))
        return err;
        }

    if ((err = SetupConfigurationDescriptor()) != KErrNone ||
            (err = SetupInterfaceDescriptors()) != KErrNone )
        {
        iLdd.Close();
        TRACE_ERROR((_L("Error during descriptors setup!")))
        return err;
        }

    iDeviceStateNotifier->Activate(); // activate notifier wich will wait until USB became configured
    TUsbcDeviceState deviceStatus = EUsbcDeviceStateDefault;
    err = iLdd.DeviceStatus(deviceStatus);
    TRACE_INFO((_L("Device status = %d"), deviceStatus))
    if (err == KErrNone && deviceStatus == EUsbcDeviceStateConfigured)
        {
        TRACE_INFO(_L("Starting bulk only transport\n"));
        err = HwStart();
        }
    iInterfaceConfigured = ETrue;
    TRACE_FUNC_EXIT
    return err;
    }

TInt CBulkOnlyTransport::HwStart(TBool aDiscard)
    {
    TRACE_FUNC_ENTRY

    TInt res = iControlInterface->Start();

    iCurrentState = ENone;
    iWriteSetUp = EFalse;
    iReadSetUp = EFalse;
    iStarted = ETrue;

    if (aDiscard)
        {
        TInt bytes;
        const TInt err = iLdd.QueryReceiveBuffer(OutEndpoint, bytes);
        if (err != KErrNone || bytes <= 0)
            {
            TRACE_ERROR((_L("Error: err=%d bytes=%d"), bytes))
            }
        else
            {
            TRACE_ERROR((_L("RxBuffer has %d bytes"), bytes))
            ReadAndDiscardData(bytes);
            }
        }

    ReadCBW();
    TRACE_FUNC_EXIT
    return res;
    }

TInt CBulkOnlyTransport::HwStop()
    {
    TRACE_FUNC

    if (iStarted)
        {
        Cancel();
        iControlInterface->Cancel();
        iProtocol->Cancel();
        iStarted = EFalse;
        }
    return KErrNone;
    }

TInt CBulkOnlyTransport::HwSuspend()
    {
    TRACE_FUNC
    return KErrNone;
    }

TInt CBulkOnlyTransport::HwResume()
    {
    TRACE_FUNC
    return KErrNone;
    }

/**
 Stops the Bulk Only Transport
 */
TInt CBulkOnlyTransport::Stop()
    {
    TRACE_FUNC
    iControlInterface->Cancel();
    iDeviceStateNotifier->Cancel();
    Cancel();
    if (iInterfaceConfigured)
        {
        iLdd.ReleaseInterface(0);
        SetupConfigurationDescriptor(ETrue);
        iLdd.Close();
        }
    iCurrentState = ENone;
    iInterfaceConfigured = EFalse;

    return KErrNone;
    }

/**
 Read aLength bytes of data from the host into the read buffer.
 @param aLength The number of bytes to read from the host.
 */
void CBulkOnlyTransport::ReadCBW()
    {
    TRACE_FUNC
    if (IsActive())
        {
        TRACE_ERROR(( _L( "Still active" ) ))
        __ASSERT_DEBUG(EFalse, User::Panic(KUsbMsSvrPncCat, EMsBulkOnlyStillActive));
        return;
        }

    iCbwBuf.SetMax();
    iLdd.ReadUntilShort(iStatus, OutEndpoint, iCbwBuf, iCbwBuf.Length());

    iCurrentState = EWaitForCBW;
    SetActive();
    }

void CBulkOnlyTransport::DoCancel()
    {
    TRACE_FUNC
    iLdd.WriteCancel(InEndpoint);
    iLdd.ReadCancel(OutEndpoint);
    }

void CBulkOnlyTransport::Activate(TInt aReason)
    {
    SetActive();
    TRequestStatus* r = &iStatus;
    User::RequestComplete(r, aReason);
    }

void CBulkOnlyTransport::RunL()
    {
    TRACE_FUNC
    if (iStatus != KErrNone)
        {
        TRACE_ERROR((_L("Error %d in RunL, halt endpoints \n"), iStatus.Int()))
        SetPermError(); //halt endpoints for reset recovery
        return;
        }

    switch (iCurrentState)
        {
        case EWaitForCBW:
            TRACE_INFO((_L("EWaitForCBW")))
            DecodeCBW();
            break;

        case EWritingData:
            TRACE_INFO((_L("EWritingData")))
            iWriteSetUp = EFalse; //the buffer was used

            if (iDataResidue && iStallAllowed)
                {
                StallEndpointAndWaitForClear(InEndpoint);
                }

            SendCSW(iCbwTag, iDataResidue, iCmdStatus);
            break;

        case EReadingData:
            {
            TRACE_INFO((_L("EReadingData")))

            TInt ret = KErrNone;
            FOREVER
                {
                if (iReadSetUp)
                    {
                    ret = iProtocol->ReadComplete(KErrNone);
                    }

                TUint deviceDataLength = static_cast<TUint>(iReadBuf.Length());
                if(ret == KErrCompletion)
                    {
                    // The protocol has indicated with KErrCompletion that sufficient
                    // data is available in the buffer to process the transfer immediately.
                    iDataResidue -= deviceDataLength;
                    iLdd.Read(iStatus, OutEndpoint, iReadBuf, deviceDataLength);
                    User::WaitForRequest(iStatus);
                    if (iStatus != KErrNone)
                        {
                        // An error occurred - halt endpoints for reset recovery
                        TRACE_ERROR((_L("Error %d in EReadingData, halt endpoints"),
                                        iStatus.Int()))
                        SetPermError();
                        return;
                        }
                    }
                else if(ret == KErrNotReady)
                    {
                    // The protocol has indicated with KErrNotReady that insufficient
                    // data is available in the buffer, so should wait for it to arrive
                    ReadData(deviceDataLength);
                    break;
                    }
                else
                    {
                    // The protocol has indicated that transfer is 
                    // complete, so send the CSW response to the host.
                    iReadSetUp = EFalse;

                    if (ret != KErrNone)
                        {
                        iCmdStatus = ECommandFailed;
                        }

                    if (iDataResidue)
                        {
                        TRACE_INFO((_L("Discarding residue")))
                        // we have to read as much data as available that host PC sends;
                        // otherwise, bulk-out endpoint will need to keep sending NAK back.
                        ReadAndDiscardData(iDataResidue);
                        }
                    SendCSW(iCbwTag, iDataResidue, iCmdStatus);
                    break;
                    }
                }
            }
        break;

        case ESendingCSW:
        TRACE_INFO((_L("ESendingCSW")))
        ReadCBW();
        break;

        case EPermErr:
        TRACE_INFO((_L("EPermErr")))
        StallEndpointAndWaitForClear(InEndpoint);
        break;

        default:
        SetPermError(); // unexpected state
        }
    }

/**
 Prepare incase we need to read data from host
 Called in DecodeCBW(...)
 @param aHostDataLength The number of bytes to read from the host.
 */
void CBulkOnlyTransport::PrepareReadFromHost(TUint aHostDataLength)
    {
    if (!iReadSetUp)
        {
			iDataResidue =aHostDataLength;
			TRACE_INFO((_L("Read buffer was not setup\n")))
            //Use next block instead of StallEndpointAndWaitForClear(OutEndpoint); 
            {
            iBuf.SetLength(KBOTMaxBufSize);
            TUint c =0;
            TRequestStatus status;
            while (c<aHostDataLength)
                {
                TInt len;
                if (aHostDataLength - c> KBOTMaxBufSize)
                    {
                    len = KBOTMaxBufSize;
                    }
                else
                    {
                    len = aHostDataLength - c;
                    }

                iLdd.Read(status, OutEndpoint, iBuf, len);
                User::WaitForRequest(status);
                c += KBOTMaxBufSize;
                }
            }

        if (iWriteSetUp) //case (10)
            {
            TRACE_INFO((_L("case 10\n")))
            SendCSW(iCbwTag, aHostDataLength, EPhaseError);
            }
        else // case (9)

            {
            TRACE_INFO((_L("Case 9\n")))
            SendCSW(iCbwTag, aHostDataLength, iCmdStatus);
            }

        return;
        }
    else
        {
        TUint deviceDataLength = static_cast<TUint>(iReadBuf.Length());
        iDataResidue =aHostDataLength - deviceDataLength;
        TRACE_INFO((_L("deviceDataLength=%d, DataResidue (read from host) =%d\n"),
                        deviceDataLength,iDataResidue))

        if (deviceDataLength <= aHostDataLength) // case (11) and (12)

            {
            TRACE_INFO((_L("Case 11 or 12\n")))
            ReadData(deviceDataLength);
            return;
            }

        if (deviceDataLength> aHostDataLength) // case  (13)

            {
            TRACE_INFO((_L("Case 13\n")))
            /**
             * Comment following line in order to pass compliant test.
             * As spec said in case 13:"The device may receive data up to a
             * total of dCBWDataTransferLength."
             * Here we choose to ignore incoming data.
             */
            //StallEndpointAndWaitForClear(OutEndpoint); //Stall Out endpoint
            if (iReadSetUp)
                {
                iLdd.Read(iStatus, OutEndpoint, iReadBuf, aHostDataLength);
                User::WaitForRequest(iStatus);
                iProtocol->ReadComplete(KErrGeneral);
                iReadSetUp = EFalse;
                }
            SendCSW(iCbwTag, aHostDataLength, EPhaseError);
            return;
            }
        }
    }

/**
 Prepare incase we need to read data from host
 Called in DecodeCBW(...)
 @param aHostDataLength The number of bytes to write to the host.
 */
void CBulkOnlyTransport::PrepareWriteToHost(TUint aHostDataLength)
    {
    if (!iWriteSetUp) //write buffer was not set up 
        {
        TRACE_INFO((_L("Write buffer was not setup")))
        iDataResidue =aHostDataLength;
        TRACE_INFO((_L("DataResidue (write to host)=%d"),iDataResidue))

        //------------------------------------
        if (aHostDataLength <= KBOTMaxBufSize)
            {
            TRACE_INFO((_L("Case 4 or 8\n")))
            iBuf.FillZ(aHostDataLength);
            iLdd.Write(iStatus, InEndpoint, iBuf, aHostDataLength);
            SetActive();
            iCurrentState = EWritingData;
            iStallAllowed = EFalse;
            if (iReadSetUp) //read buffer WAS set up - case (8)

                {
                TRACE_INFO((_L("It is Case 8")))
                iCmdStatus = EPhaseError;
                }
            return;
            }
        else
        //------------------------------------
        //					Use next block instead of StallEndpointAndWaitForClear(InEndpoint);

            {
            iBuf.FillZ(KBOTMaxBufSize);
            TUint c =0;
            TRequestStatus status;
            while (c<aHostDataLength)
                {
                TInt len;
                if (aHostDataLength - c> KBOTMaxBufSize)
                    {
                    len = KBOTMaxBufSize;
                    }
                else
                    {
                    len = aHostDataLength - c;
                    }

                iLdd.Write(status, InEndpoint, iBuf, len);
                User::WaitForRequest(status);
                c += KBOTMaxBufSize;
                }
            }

        if (iReadSetUp) //read buffer WAS set up - case (8)
            {
            TRACE_INFO(_L("Case 8"));
            SendCSW(iCbwTag, aHostDataLength, EPhaseError);
            //don't care to reset any flag - should get reset recovery
            }
        else // case (4)
            {
            TRACE_INFO((_L("Case 4")))
            SendCSW(iCbwTag, aHostDataLength, iCmdStatus);
            }
        return;
        }
    else
        {
        //================== 
        TUint deviceDataLength = static_cast<TUint>(iWriteBuf.Length());
        iDataResidue =aHostDataLength - deviceDataLength;
        TRACE_INFO((_L("Device data length = %d, DataResidue (write to host)=%d"),
                        deviceDataLength, iDataResidue))

        if (deviceDataLength < aHostDataLength &&
                aHostDataLength < KBOTMaxBufSize )
            {
            TRACE_INFO((_L("Case 5 (padding)\n")))
            iBuf.Zero();
            iBuf.Append(iWriteBuf);
            iBuf.SetLength(aHostDataLength);
            iStallAllowed = EFalse;
            TRACE_INFO((_L("iBuf.Length=%d\n"),iBuf.Length()))
            iLdd.Write(iStatus, InEndpoint, iBuf, aHostDataLength);
            SetActive();
            iCurrentState = EWritingData;
            return;
            }

        //===================

        if (deviceDataLength == aHostDataLength) //case (6)[==]

            {
            TRACE_INFO((_L("Case 6\n")))
            WriteData(deviceDataLength);
            return;
            }
        else if (deviceDataLength < aHostDataLength) //case (5)[<]

            {
            TRACE_INFO((_L("Case 5\n")))
            WriteData(deviceDataLength, ETrue); // Send ZLP
            return;
            }
        else // deviceDataLength > aHostDataLength - case (7)

            {
            TRACE_INFO((_L("Case 7\n")))
            iCmdStatus = EPhaseError;
            iDataResidue = 0;
            WriteData(aHostDataLength);
            return;
            }
        }
    }

/**
 Decode the CBW received from the host via OutEndpoint

 - If the header is valid, the data content is passed to the parser.
 - Depending on the command, more data may be transmitted/received.
 - ...or the CSW is sent (if not a data command).

 */
void CBulkOnlyTransport::DecodeCBW()
    {
    TRACE_FUNC
    if (!CheckCBW()) //check if CBW valid and meaningful

        {
        // CBW not valid or meaningful
        // Specification says: "If the CBW is not valid, the device shall STALL
        // the Bulk-In pipe. Also, the device shall either STALL the Bulk-Out pipe,
        // or the device shall accept and discard any Bulk-Out data. The device
        // shall maintain this state until a Reset Recovery."
        // Here we keep bulk-in ep stalled and ignore bulk-out ep.
        SetPermError();
        return;
        }

    TPtrC8 aData;
    aData.Set(&iCbwBuf[KCbwCbLengthOffset], KMaxCbwcbLength + 1); //prepare data for protocol starting form Length 
    TUint8 lun = static_cast<TUint8> (iCbwBuf[13] & 0x0f);

	iCbwTag  =	static_cast<TUint32>(iCbwBuf[KCbwTagOffset])		| 
				static_cast<TUint32>(iCbwBuf[KCbwTagOffset+1])	<<8 | 
				static_cast<TUint32>(iCbwBuf[KCbwTagOffset+2])	<<16| 
				static_cast<TUint32>(iCbwBuf[KCbwTagOffset+3])	<<24;

    TInt i = KCbwDataTransferLengthOffset;
	TUint hostDataLength = 	static_cast<TUint32>(iCbwBuf[i  ])		| 
							static_cast<TUint32>(iCbwBuf[i+1]) <<8 	| 
							static_cast<TUint32>(iCbwBuf[i+2]) <<16	| 
							static_cast<TUint32>(iCbwBuf[i+3]) <<24;

    TBool dataToHost = iCbwBuf[KCbwFlagOffset] & 0x80;

    TRACE_INFO((_L("lun =%d, hostDataLength=%d, CBWtag = 0x%X\n, dataToHost=%d"),
                    lun, hostDataLength, iCbwTag, dataToHost))
    //
    TBool ret = iProtocol->DecodePacket(aData, lun);
    //

    iStallAllowed = ETrue;

    if (!ret)
        {
        TRACE_INFO((_L("Command Failed")))
        iCmdStatus = ECommandFailed;
        }
    else
        {
        TRACE_INFO((_L("Command Passed")))
        iCmdStatus = ECommandPassed;
        }

    if (hostDataLength == 0)
        {
        TRACE_INFO((_L("No data transfer expected\n")))
        iDataResidue = 0;
        if (iWriteSetUp || iReadSetUp) // case (2) and (3)

            {
            TRACE_INFO((_L("Case 2 or 3\n")))
            SendCSW(iCbwTag, 0, EPhaseError);
            }
        else
            {
            TRACE_INFO((_L("Case 1\n")))
            SendCSW(iCbwTag, 0, iCmdStatus); //case (1)
            }

        return;
        }
    else
        {
        // Data Direction, To or From Host
        if (dataToHost)
            {
            PrepareWriteToHost(hostDataLength);
            }
        else
            {
            PrepareReadFromHost(hostDataLength);
            }
        }
    }

/**
 Check if CBW Valid and Meaningful.  

 @return ETrue if CBW is Valid and Meaningful, EFalse otherwise
 */
TBool CBulkOnlyTransport::CheckCBW()
    {
    TRACE_FUNC

    // Check valid

    // Check length
    if (iCbwBuf.Length() != KCbwLength)
        {
        TRACE_INFO((_L("Bad length: %d"), iCbwBuf.Length()))
        return EFalse;
        }

    // Check signature
    TInt i = KCbwSignatureOffset;
	if (iCbwBuf[i  ] != 0x55 ||         // CBW Singature from USB Bulk-Only Transport spec
		iCbwBuf[i+1] != 0x53 ||
		iCbwBuf[i+2] != 0x42 ||
		iCbwBuf[i+3] != 0x43)
        {
        TRACE_INFO((_L("Bad signature")))
        TRACE_INFO((_L(" 0x%x, 0x%x, 0x%x, 0x%x"),
                        iCbwBuf[i], iCbwBuf[i+1], iCbwBuf[i+2],iCbwBuf[i+3]))
        return EFalse;
        }

    // Check meaningful

    // Check reserved bits ( must be zero )
    if ((iCbwBuf[KCbwLunOffset] & 0xF0) || (iCbwBuf[KCbwCbLengthOffset] & 0xE0))
        {
        TRACE_INFO((_L("Reserved bits not zero\n")))
        return EFalse;
        }

    // check command block length
    TInt cbwcbLength = iCbwBuf[KCbwCbLengthOffset] & 0x1F;
    if (cbwcbLength>KMaxCbwcbLength)
        {
        TRACE_INFO((_L("Incorrect block length\n")))
        return EFalse;
        }

    //check LUN
    TInt8 lun = static_cast<TUint8>(iCbwBuf[KCbwLunOffset] & 0x0f);
    if (iMaxLun < lun)
        {
        TRACE_INFO((_L("bad lun: %d"), lun))
        return EFalse;
        }

    return ETrue;
    }

/**
 Initiate stalling of bulk IN endpoint.
 Used when protocol wants to force host to initiate a reset recovery.
 */
void CBulkOnlyTransport::SetPermError()
    {
    TRACE_FUNC
    iCurrentState = EPermErr;
    Activate(KErrNone);
    }

/**
 Send data provided by protocol to the host

 @param aLength amount of data (in bytes) to be send to host
 */
void CBulkOnlyTransport::WriteData(TUint aLength, TBool aZlpRequired)
    {
    TRACE_FUNC

    if (IsActive())
        {
        TRACE_INFO((_L("Still active")))
        __ASSERT_DEBUG(EFalse, User::Panic(KUsbMsSvrPncCat, EMsBulkOnlyStillActive));
        return;
        }
    iLdd.Write(iStatus, InEndpoint, iWriteBuf, aLength, aZlpRequired);
    iCurrentState = EWritingData;
    SetActive();
    }

/**
 Request data form the host for the protocol

 @param aLength amount of data (in bytes) to be received from the host
 */
void CBulkOnlyTransport::ReadData(TUint aLength)
    {
    TRACE_FUNC
    if (IsActive())
        {
        TRACE_INFO((_L("Still active")))
        __ASSERT_DEBUG(EFalse, User::Panic(KUsbMsSvrPncCat, EMsBulkOnlyStillActive));
        return;
        }
    iLdd.Read(iStatus, OutEndpoint, iReadBuf, aLength);
    SetActive();
    iCurrentState = EReadingData;
    }

/**
 Send Command Status Wrapper to the host

 @param aTag Echo of Command Block Tag sent by the host.
 @param aDataResidue the difference between the amount of data expected by the
 host, and the actual amount of data processed by the device.
 @param aStatus indicates the success or failure of the command.
 */
void CBulkOnlyTransport::SendCSW(TUint aTag, TUint aDataResidue, TCswStatus aStatus)
    {
    TRACE_FUNC
    TRACE_INFO((_L("DataResidue = %d, Status = %d \n"), aDataResidue, aStatus))

    if (IsActive())
        {
        TRACE_INFO((_L("Still active")))
        __ASSERT_DEBUG(EFalse, User::Panic(KUsbMsSvrPncCat, EMsBulkOnlyStillActive));
        return;
        }
    TBuf8<KCswLength> csw(KCswLength);

    TInt i = KCswSingnatureOffset;
    csw[i] = 0x55; // CSW Singature from USB Bulk-Only Transport spec
    csw[i + 1] = 0x53;
    csw[i + 2] = 0x42;
    csw[i + 3] = 0x53;

    i = KCswTagOffset;

    csw[i] = static_cast<TUint8> ((aTag & 0x000000FF));
    csw[i + 1] = static_cast<TUint8> ((aTag & 0x0000FF00) >> 8);
    csw[i + 2] = static_cast<TUint8> ((aTag & 0x00FF0000) >> 16);
    csw[i + 3] = static_cast<TUint8> ((aTag & 0xFF000000) >> 24);

    i = KCswDataResidueOffset;
    csw[i] = static_cast<TUint8> ((aDataResidue & 0x000000FF));
    csw[i + 1] = static_cast<TUint8> ((aDataResidue & 0x0000FF00) >> 8);
    csw[i + 2] = static_cast<TUint8> ((aDataResidue & 0x00FF0000) >> 16);
    csw[i + 3] = static_cast<TUint8> ((aDataResidue & 0xFF000000) >> 24);

    csw[KCswStatusOffset] = static_cast<TUint8> (aStatus);

    iLdd.Write(iStatus, InEndpoint, csw, KCswLength);

    iCurrentState = ESendingCSW;

    SetActive();
    }

/**
 Associates the transport with the protocol.  Called during initialization of the controller.

 @param aProtocol reference to the protocol
 */
void CBulkOnlyTransport::RegisterProtocol(MProtocolBase& aProtocol)
    {
    TRACE_FUNC
    iProtocol = &aProtocol;
    }

/**
 Used by CControlInterface

 @return reference to the controller which instantiate the CBulkOnlyTransport
 */
CMscFileController& CBulkOnlyTransport::Controller()
    {
    return iController;
    }

/**
 @return the number of logical units supported by the device. 
 Logical Unit Numbers on the device shall be numbered contiguously starting from LUN
 0 to a maximum LUN of 15 (Fh).
 */
TInt CBulkOnlyTransport::MaxLun()
    {
    return iMaxLun;
    }

/**
 Used by CControlInterface
 @return reference to USB logical driver
 */
RDevUsbcClient& CBulkOnlyTransport::Ldd()
    {
    return iLdd;
    }


void CBulkOnlyTransport::StallEndpointAndWaitForClear(TEndpointNumber aEndpoint)
    {
    TRACE_FUNC
    __ASSERT_DEBUG(aEndpoint != EEndpoint0, User::Panic(KUsbMsSvrPncCat, EMsWrongEndpoint));

    // Now stall this endpoint
    TRACE_INFO((_L("Stalling endpoint %d"), aEndpoint))
    TInt r = iLdd.HaltEndpoint(aEndpoint);
    if (r != KErrNone)
        {
        TRACE_ERROR((_L("Error: stalling ep %d failed: %d"), aEndpoint, r))
        }
    TEndpointState ep_state;
    TInt i = 0;
    do
        {
        // Wait for 10ms before checking the ep status
        User::After(10000);
        iLdd.EndpointStatus(aEndpoint, ep_state);
        if (++i >= 1000)
            {
            // 10 secs should be enough
            TRACE_ERROR((_L("Error: Checked for ep %d de-stall %d seconds - giving up now..."),
                            aEndpoint, i / 100))
            // We can only hope for a Reset Recovery
            return;
            }
        }while ((ep_state == EEndpointStateStalled) && iStarted);
    TRACE_INFO((_L("Checked for ep %d de-stall: %d time(s)"), aEndpoint, i))
    }

/**
 * Read out rest data from OutEndpoint and discard them
 */
void CBulkOnlyTransport::ReadAndDiscardData(TInt aBytes)
    {
    TRACE_FUNC
    const TUint bufsize = static_cast<TUint>(iReadBuf.Length());
    __ASSERT_ALWAYS(bufsize> 0, User::Panic(_L("Buffer size is zero"), bufsize));
    TRequestStatus status;
    while (aBytes> 0)
        {
        TRACE_INFO((_L("Bytes still to be read: %d\n"), aBytes))
        iLdd.ReadOneOrMore(status, OutEndpoint, iReadBuf, bufsize);
        User::WaitForRequest(status);
        TInt err = status.Int();
        if (err != KErrNone)
            {
            // Bad.
            break;
            }
        aBytes -= iReadBuf.Length();
        }
    iReadBuf.SetLength(bufsize);
    }

/**
 Called by the protocol to determine how many bytes of data are available in the read buffer. 

 @return The number of bytes available in the read buffer
 */
TInt CBulkOnlyTransport::BytesAvailable()
    {
    TInt bytes = 0;
    TInt err = iLdd.QueryReceiveBuffer(OutEndpoint, bytes);
    if (err != KErrNone)
        bytes = 0;
    return bytes;
    }

//
// --- class CActiveDeviceStateNotifier ---------------------------------------------------------
//
CActiveDeviceStateNotifier::CActiveDeviceStateNotifier(CBulkOnlyTransport& aParent)
/**
 *
 */
	: CActive(EPriorityStandard),
	  iParent(aParent),
	  iDeviceState(EUsbcNoState),
            iOldDeviceState(EUsbcNoState)
    {

    }


CActiveDeviceStateNotifier* CActiveDeviceStateNotifier::NewL(CBulkOnlyTransport& aParent)
/**
 *
 */
    {
	CActiveDeviceStateNotifier* self = new (ELeave) CActiveDeviceStateNotifier(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    CActiveScheduler::Add(self);
    CleanupStack::Pop(); // self
    return (self);
    }

void CActiveDeviceStateNotifier::ConstructL()
/**
 *
 */
    {
    TRACE_FUNC
	}

CActiveDeviceStateNotifier::~CActiveDeviceStateNotifier()
/**
 *
 */
    {
    TRACE_FUNC
    Cancel();												// base class
    }

void CActiveDeviceStateNotifier::DoCancel()
/**
 *
 */
    {
    TRACE_FUNC
    iParent.Ldd().AlternateDeviceStatusNotifyCancel();
    }

void CActiveDeviceStateNotifier::RunL()
/**
 *
 */
    {
    TRACE_FUNC
    // This displays the device state.
    // In a real world program, the user could take here appropriate action (cancel a
    // transfer request or whatever).
    if (!(iDeviceState & KUsbAlternateSetting))
        {

        switch (iDeviceState)
            {
            case EUsbcDeviceStateUndefined:
            TRACE_INFO((_L("Device State notifier: Undefined\n")))
            iParent.HwStop();
            break;
            case EUsbcDeviceStateAttached:
            TRACE_INFO((_L("Device State notifier: Attached\n")))
            iParent.HwStop();
            break;
            case EUsbcDeviceStatePowered:
            TRACE_INFO((_L("Device State notifier: Powered\n")))
            iParent.HwStop();
            break;
            case EUsbcDeviceStateDefault:
            TRACE_INFO((_L("Device State notifier: Default\n")))
            iParent.HwStop();
            break;
            case EUsbcDeviceStateAddress:
            TRACE_INFO((_L("Device State notifier: Address\n")))
            iParent.HwStop();
            break;
            case EUsbcDeviceStateConfigured:
            TRACE_INFO((_L("Device State notifier: Configured\n")))
            if (iOldDeviceState == EUsbcDeviceStateSuspended)
                {
                iParent.HwResume();
                }
            else
                {
                iParent.HwStart();
                }
            break;
            case EUsbcDeviceStateSuspended:
            TRACE_INFO((_L("Device State notifier: Suspended\n")))
            if (iOldDeviceState == EUsbcDeviceStateConfigured)
                {
                iParent.HwSuspend();
                }
            break;
            default:
            TRACE_INFO((_L("Device State notifier: ***BAD***\n")))
            iParent.HwStop();
            break;
            }
        iOldDeviceState = iDeviceState;
        }
    else if (iDeviceState & KUsbAlternateSetting)
        {
        TRACE_INFO((_L("Device State notifier: Alternate interface setting has changed: now %d\n"),
                        iDeviceState & ~KUsbAlternateSetting))
        }
    Activate();
    }

void CActiveDeviceStateNotifier::Activate()
/**
 *
 */
    {
    TRACE_FUNC
    if (IsActive())
        {
        TRACE_INFO((_L("Still active")))
        return;
        }
    iParent.Ldd().AlternateDeviceStatusNotify(iStatus, iDeviceState);
    SetActive();
    }

