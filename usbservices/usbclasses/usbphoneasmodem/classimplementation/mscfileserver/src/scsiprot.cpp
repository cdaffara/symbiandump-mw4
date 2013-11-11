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
// Description: SCSI protocol
// 
// 

#include "scsiprot.h"
#include "debug.h"
#include "mscfileserver.h"

// Helper macros
#define LBA(x) static_cast<TUint32>((x[3] << 24) | (x[4] << 16) | (x[5] << 8) | x[6])
#define LEN(x) static_cast<TUint16>((x[8] << 8) | x[9])


LOCAL_D const TUint KDefaultBlockSize = 0x800;  //default block size for MM

LOCAL_D const TUint KUndefinedLun = 0xFFFF;

LOCAL_D const TUint8 KAllPages = 0x3F;

LOCAL_D const TUint8 KChangeableValues = 0x1;
LOCAL_D const TUint8 KDefaultValues = 0x2;

/**
Default constructor for TSenseInfo
*/
TSenseInfo::TSenseInfo()
	: iSenseCode(ENoSense),
	  iAdditional(EAscNull),
	  iQualifier(EAscqNull)
	{}


/**
Set sense with no additional info.

@param aSenseCode sense key
*/
void TSenseInfo::SetSense(TSenseCode aSenseCode)
	{
	iSenseCode	= static_cast<TUint8>(aSenseCode);
	iAdditional = EAscNull;
	iQualifier = EAscqNull;
	}


/**
Set sense with additional info.

@param aSenseCode sense key
@param aAdditional additional sense code (ASC) 
*/
void TSenseInfo::SetSense(TSenseCode aSenseCode, TAdditionalCode aAdditional)

	{
	iSenseCode = static_cast<TUint8>(aSenseCode);
    iAdditional = static_cast<TUint8>(aAdditional);
	iQualifier = EAscqNull;
	}


/**
Set sense with additional info and qualifier.

@param aSenseCode sense key 
@param aAdditional additional sense code (ASC) 
@param aQualifier additional sense code qualifier (ASCQ)
*/
void TSenseInfo::SetSense(TSenseCode aSenseCode,
						  TAdditionalCode aAdditional,
						  TAdditionalSenseCodeQualifier aQualifier)
	{
	iSenseCode = static_cast<TUint8>(aSenseCode);
    iAdditional = static_cast<TUint8>(aAdditional);
	iQualifier = static_cast<TUint8>(aQualifier);
	}


//-----------------------------------------------

/**
Creates the CScsiProtocol object.  Called during controller initialisation.

@param aFsImage reference to the file system image
*/
CScsiProtocol* CScsiProtocol::NewL(CMscFileController& aController)
	{
	CScsiProtocol* self = new (ELeave) CScsiProtocol(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
c'tor

@param aFsImage reference to the file system image
*/
CScsiProtocol::CScsiProtocol(CMscFileController& aController):
	iController(aController),
	iLastCommand(EUndefinedCommand),
	iLastLun(KUndefinedLun),
	iMediaWriteSize(KDefaultMediaWriteSize)
	{
	}


CScsiProtocol::~CScsiProtocol()
	{
	}


void CScsiProtocol::ConstructL()
	{
	TRACE_FUNC
	}


/**
Associates the transport with the protocol. Called during initialisation of the controller.

@param aTransport pointer to the transport object
*/
void CScsiProtocol::RegisterTransport(MTransportBase* aTransport)
	{
	TRACE_FUNC
	iTransport = aTransport;
	}


/**
Called by the Transport when it detects that the USB device is either running
at High Speed or is at least capable of HS operation. The Protocol can use this
information (for instance) to select the optimal write block size to use.

This function is preferably called before actual MS data transfer operation
starts, and usually only once.

*/
void CScsiProtocol::ReportHighSpeedDevice()
	{
	TRACE_FUNC
	iMediaWriteSize = KHsMediaWriteSize;
	TRACE_INFO(( _L( "HS Device reported: SCSI will use %d bytes disk write size"), 
	        iMediaWriteSize ))
	}


TInt CScsiProtocol::SetScsiParameters(TMassStorageConfig aConfig)
	{
	TRACE_FUNC
	iConfig = aConfig;
	return KErrNone;
	}


/**
Called by the transport layer when a packet is available for decoding.
If an error occurs, the sense code is updated and EFalse is returned.

@param aData

@return  ETrue if command was decoded and executed successfully
*/
TBool CScsiProtocol::DecodePacket(TPtrC8& aData, TUint aLun)
	{
	TRACE_FUNC_ENTRY

	TUint8 command = aData[1];

	if (command != ERequestSense)
		{
		iSenseInfo.SetSense(TSenseInfo::ENoSense);
		}

	TRACE_INFO(( _L( "command = 0x%x lun=%d"), command, aLun ))
	switch (command)
		{
        case ETestUnitReady:
            HandleUnitReady(aLun);
            break;

        case ERequestSense:
            HandleRequestSense(aData);
            break;
            
        case EInquiry:
            HandleInquiry(aData, aLun);
            break;
                
        case EModeSense:
            HandleModeSense(aData, aLun);
            break;
            
        case EModeSense10:
            HandleModeSense10(aData, aLun);
            break;
        
        case EStartStopUnit:
            HandleStartStopUnit( aData,aLun);
            break;
    
        case EPreventMediaRemoval:
            HandlePreventMediaRemoval(aData, aLun);
            break;
    
        case EReadCapacity:
            HandleReadCapacity(aData, aLun);
            break;
            
        case ERead10:
            HandleRead10(aData, aLun);
            break;
            
        case ERead12:
            HandleRead12(aData, aLun);
            break;
        
        case EReadTOC:
            HandleReadTOC(aData, aLun);
            break;
        
        case EGetConfiguration:
            HandleGetConfiguration(aData, aLun);
            break;
    
		default:
			iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::EInvalidCmdCode);
		}
	TRACE_INFO(( _L( "result = %d" ), iSenseInfo.SenseOk() ))
	TRACE_FUNC_EXIT
	return ( iSenseInfo.SenseOk() );
	}


/**
Checks if drive ready

@param aLun Logic unit number 
@return pointer to drive correspondent to LUN if drive mounted and ready, NULL otherwise
*/
CFileSystemImage* CScsiProtocol::GetCheckFs(TUint aLun)
	{
	TRACE_FUNC
	TInt err = KErrNone;
	CFileSystemImage* image = iController.FsImage( aLun );
	
	if ( image == NULL )
		{
		TRACE_ERROR(( _L( "Illegal LUN %d" ), aLun ))
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::ELuNotSupported);
		return NULL;
		}
	
	// Image exists, ensure it is opened for access
    err = image->Open();
    if ( err == KErrNone )
        {
        // Image is now open, if it wasn't already
        TRACE_INFO(( _L( "Image opened successfully" ) ))
        return image;
        }
    else
        {
        // Either file doesn't exist or was removed
        TRACE_ERROR(( _L( "Error %d opening image" ), err ))
        iSenseInfo.SetSense( TSenseInfo::ENotReady, TSenseInfo::EMediaNotPresent );
        }
    return NULL;
	}


/**
Command Parser for the UNIT READY command (0x00)

@param aLun Logic unit number 
@return ETrue if successful, 
*/
TBool CScsiProtocol::HandleUnitReady(TUint aLun)
	{
	TRACE_FUNC
	return ( GetCheckFs(aLun) != NULL );
	}


/**
Command Parser for the REQUEST SENSE command (0x03)

@return ETrue if successful, 
*/
TBool CScsiProtocol::HandleRequestSense(TPtrC8& aData)
	{
	TRACE_FUNC
	TUint length = aData[5];
	TRACE_INFO(( _L( "length = %d"), length ))
	iCommandBuf.FillZ(KRequestSenseCommandLength);

	TSenseInfo* senseInfo;
	senseInfo = &iSenseInfo;
	iCommandBuf[00] = 0x70; //(current errors)

	iCommandBuf[02] = static_cast<TUint8>(senseInfo->iSenseCode & 0x0F);
	
	iCommandBuf[12] = senseInfo->iAdditional;
	iCommandBuf[13] = senseInfo->iQualifier;
	if (length<18 && length >=8) 
		{
		iCommandBuf.SetLength(length);  //length of response code data
		iCommandBuf[07] = TUint8(length - 8);  //additional sence length
		}
	else if (length >= KRequestSenseCommandLength)
		{
		iCommandBuf[07] = KRequestSenseCommandLength - 8;	// we have max 18 byte to send
		}

	TRACE_INFO(( _L( "Response=0x%x Sense=0x%x, Additional=0x%x, Qualifier=0x%x\n"),
				iCommandBuf[0], iCommandBuf[02], iCommandBuf[12], iCommandBuf[13] ))
 
	TPtrC8 writeBuf = iCommandBuf.Left(length);
	iTransport->SetupWriteData(writeBuf);

	// clear the sense info
	iSenseInfo.SetSense(TSenseInfo::ENoSense);
	
	return ETrue;
	}


/**
Command Parser for the INQUIRY command (0x12)

@param aLun Logic unit number 
@return ETrue if successful, 
*/
TBool CScsiProtocol::HandleInquiry(TPtrC8& aData, TUint /*aLun*/ )
	{
	TRACE_FUNC
	
	TBool cmdDt = aData[2] & 0x2;
	TBool evpd  = aData[2] & 0x1;
	TUint8 page = aData[3];
	if (cmdDt || evpd || page /*|| aLun >= KUsbMsMaxDrives*/)
		{
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::EInvalidFieldInCdb); 
		return EFalse;
		}

	iCommandBuf.FillZ(KInquiryCommandLength);

    iCommandBuf[0] = 0x05; // CD-ROM
    iCommandBuf[1] = 0x80; // MSB: RMB : Removable
    iCommandBuf[2] = 0x02; // Version SPC-3
    iCommandBuf[3] = 0x02; // AERC, TrmTsk, NormACA, Response Data Format
    iCommandBuf[4] = 0x1F; // Additional Length

	TPtr8 vendorId(&iCommandBuf[8], 8, 8);		// Vendor ID (Vendor Specific/Logged by T10)
	vendorId.Fill(' ', 8);
	vendorId.Copy(iConfig.iVendorId);

	TPtr8 productId(&iCommandBuf[16], 16, 16);	// Product ID (Vendor Specific)
	productId.Fill(' ', 16);
	productId.Copy(iConfig.iProductId);

	TPtr8 productRev(&iCommandBuf[32], 4, 4);		// Product Revision Level (Vendor Specific)
	productRev.Fill(' ', 4);
	productRev.Copy(iConfig.iProductRev);

    TUint length = aData[5];
	TPtrC8 writeBuf = iCommandBuf.Left(length);
	iTransport->SetupWriteData(writeBuf);

	iSenseInfo.SetSense(TSenseInfo::ENoSense); 
	return ETrue;
	}


/**
 Command Parser for the START STOP UNIT command (0x1B)
 
 @param aData command data (started form position 1)
 @param aLun Logic unit number 
 @return ETrue if successful, TFalse otherwise
 */
TBool CScsiProtocol::HandleStartStopUnit(TPtrC8& aData, TUint aLun)
	{
	TRACE_FUNC
    CFileSystemImage* image = GetCheckFs(aLun); 
    if ( image == NULL )
        {
        return EFalse;
        }

	const TUint8 KStartMask = 0x01;
	const TUint8 KImmedMask = 0x01;
	const TUint8 KLoejMask = 0x02;

	TBool immed = aData[2] & KImmedMask ? ETrue : EFalse;
	TBool start = aData[5] & KStartMask ? ETrue : EFalse;
	TBool loej = aData[5] & KLoejMask ? ETrue : EFalse;

	TRACE_INFO(( _L( "Data %X %X" ), aData[2], aData[5] ))
	TRACE_INFO(( _L( "IMMED = %d" ), immed ))
	TRACE_INFO(( _L( "START = %d"), start ))
	TRACE_INFO(( _L( "LOEJ  = %d" ), loej ))

	TInt err = KErrNone;
	if (loej)
		{
		if (start)	//Start unit
			{
			// GetCheckFs has already opened the image file
			TRACE_INFO(( _L( "Load media" ) ))
			}
		else		//Stop unit 
			{
			err = image->Close();
			TRACE_INFO(( _L( "Unload media" ) ))
			}
		}

	if (err != KErrNone)  //actually we have error here only if the LUN is incorrect 
		{
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::ELuNotSupported);
		return EFalse;
		}

	return ETrue;
	}


/**
Command Parser for the PREVENT/ALLOW MEDIA REMOVAL command (0x1E)

@param aData command data (started form position 1)
@param aLun Logic unit number 
@return ETrue if successful.
*/
TBool CScsiProtocol::HandlePreventMediaRemoval(TPtrC8& aData, TUint aLun)
	{
	TRACE_FUNC
    if ( GetCheckFs(aLun) == NULL )
        {
        return EFalse;
        }

	TInt prevent = aData[5] & 0x01;

	// locking is not supported
	if (prevent)
		{
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::EInvalidFieldInCdb);
		}
	return ETrue;
	}

/** Cancel active state, Invoked by transnport when it stops */
TInt CScsiProtocol::Cancel()
	{
	TRACE_FUNC
	return KErrNone;
	}

/**
Command Parser for the READ CAPACITY(10) command (0x25)

@param aData command data (started form position 1)
@param aLun Logic unit number 
@return ETrue if successful.
*/
TBool CScsiProtocol::HandleReadCapacity(TPtrC8& aData, TUint aLun)
	{
	TRACE_FUNC
	CFileSystemImage* image = GetCheckFs(aLun); 
    if ( image == NULL )
        {
        return EFalse;
        }

	TInt pmi = aData[9] & 0x01;
	TInt lba = aData[3] | aData[4] | aData[5] | aData[6];

	if (pmi || lba)   //do not support partial medium indicator
		{
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::EInvalidFieldInCdb);
		return EFalse;
		} 

	TInt64 driveBlocks = image->Size() / MAKE_TINT64(0, KDefaultBlockSize) - 1;
	iCommandBuf.FillZ(KReadCapacityCommandLength);
	if (I64HIGH(driveBlocks) == 0)
		{
		TUint32 numBlocks = I64LOW(driveBlocks);

		TRACE_INFO(( _L( "Block size=%d, NumBlocks=%d" ), 
		        KDefaultBlockSize, numBlocks ))
		iCommandBuf[0] = static_cast<TUint8>((numBlocks & 0xFF000000) >> 24);	// Number of blocks
		iCommandBuf[1] = static_cast<TUint8>((numBlocks & 0x00FF0000) >> 16);
		iCommandBuf[2] = static_cast<TUint8>((numBlocks & 0x0000FF00) >> 8);
		iCommandBuf[3] = static_cast<TUint8>((numBlocks & 0x000000FF));
		}
	else   
		{
		iCommandBuf[0] = iCommandBuf[1] = iCommandBuf[2] = iCommandBuf[3] = 0xFF;  // indicate that size more then )0xFFFFFFFF
		}

	iCommandBuf[4] = static_cast<TUint8>((KDefaultBlockSize & 0xFF000000) >> 24);	// Block Size
	iCommandBuf[5] = static_cast<TUint8>((KDefaultBlockSize & 0x00FF0000) >> 16);
	iCommandBuf[6] = static_cast<TUint8>((KDefaultBlockSize & 0x0000FF00) >> 8);
	iCommandBuf[7] = static_cast<TUint8>((KDefaultBlockSize & 0x000000FF));

	TPtrC8 writeBuf = iCommandBuf;
	iTransport->SetupWriteData(writeBuf);

	return ETrue;
	}


/**
Command Parser for the READ10 command (0x28)

@param aData command data (started form position 1)
@param aLun Logic unit number
@return ETrue if successful.
*/
TBool CScsiProtocol::HandleRead10(TPtrC8& aData, TUint aLun)
	{
	TRACE_FUNC
    CFileSystemImage* image = GetCheckFs(aLun); 
    if ( image == NULL )
        {
        return EFalse;
        }
    
	TInt rdProtect = aData[2] >> 5;
	if (rdProtect)
		{
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::EInvalidFieldInCdb);
		return EFalse;
		}

	const TUint32 lba = LBA(aData);
	const TUint16 len = LEN(aData);

	TRACE_INFO(( _L( "READ(10) : LBA = %d, Length = %d (blocks)" ), lba, len))

	if (!len)
		{
		return ETrue; // do nothing - this is not an error
		}

	const TInt64 bOffset = MAKE_TINT64(0, lba) * KDefaultBlockSize;
	const TInt bLength = len * KDefaultBlockSize;
	const TInt64 theEnd = bOffset + MAKE_TINT64(0, bLength);

    if (theEnd > image->Size())  //check if media big enough for this request
		{
		TRACE_ERROR(( _L( "err - Request ends out of media" ) ))
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::ELbaOutOfRange);
		return EFalse;
		}

	// check if our buffer can hold requested data
	if (iCommandBuf.MaxLength() < bLength)
		{
		TRACE_ERROR(( _L( "err - Buffer too small" ) ))
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::EInvalidFieldInCdb);
		return EFalse;
		}

	TInt err = image->Read(bOffset, bLength, iCommandBuf);
	if (err != KErrNone)
		{
		TRACE_ERROR(( _L( "Read failed, err=%d" ), err ))
		iSenseInfo.SetSense(TSenseInfo::ENotReady, TSenseInfo::EMediaNotPresent);
		return EFalse;
		}

	TPtrC8 writeBuf = iCommandBuf;

	// Set up data write to the host
	iTransport->SetupWriteData(writeBuf);

	return ETrue;
	}


/**
Command Parser for the READ12 command (0xA8)

@param aData command data (started form position 1)
@param aLun Logic unit number
@return ETrue if successful.
*/
TBool CScsiProtocol::HandleRead12(TPtrC8& aData, TUint aLun)
    {
    TRACE_FUNC
    CFileSystemImage* image = GetCheckFs(aLun); 
    if ( image == NULL )
        {
        return EFalse;
        }
    TInt rdProtect = aData[2] >> 5;
    if (rdProtect)
        {
        iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::EInvalidFieldInCdb);
        return EFalse;
        }

    const TUint32 lba = static_cast<TUint32>((aData[3] << 24) | (aData[4] << 16) | (aData[5] << 8) | aData[6]);
    const TUint32 len = static_cast<TUint32>((aData[7] << 24) | (aData[8] << 16) | (aData[9] << 8) | aData[10]);

    TRACE_INFO(( _L( "READ(12) : LBA = %d, Length = %d  (blocks)" ), lba, len ))

    if (!len)
        {
        return ETrue; // do nothing - this is not an error
        }

    const TInt64 bOffset = MAKE_TINT64(0, lba) * KDefaultBlockSize;
    const TUint32 bLength = len * KDefaultBlockSize;
    const TInt64 theEnd = bOffset + MAKE_TINT64(0, bLength);

    if (theEnd > image->Size())  //check if media big enough for this request
        {
        TRACE_ERROR(( _L( "err - Request ends out of media" ) ))
        iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::ELbaOutOfRange);
        return EFalse;
        }

    // check if our buffer can hold requested data
    if (iCommandBuf.MaxLength() < bLength)
        {
        TRACE_ERROR(( _L( "err - Buffer too small" ) ))
        iSenseInfo.SetSense(TSenseInfo::EIllegalRequest, TSenseInfo::EInvalidFieldInCdb);
        return EFalse;
        }

    TInt err = image->Read(bOffset, bLength, iCommandBuf);
    if (err != KErrNone)
        {
        TRACE_ERROR(( _L( "Read failed, err=%d" ), err ))
        iSenseInfo.SetSense(TSenseInfo::ENotReady, TSenseInfo::EMediaNotPresent);
        return EFalse;
        }

    TPtrC8 writeBuf = iCommandBuf;

    // Set up data write to the host
    iTransport->SetupWriteData(writeBuf);

    return ETrue;
    }

/**
Called by the transport when the requested data has been read or an error has
occurred during the read.

@param aError Indicate if an error occurs during reading data by transport.
@return KErrAbort if command processing is complete but has failed,
        KErrCompletion if sufficient data is available in the buffer to process
        the transfer immediately, KErrNotReady if insufficient data is
        available in the buffer so the transport should wait for it to arrive,
        KErrNone if command processing is complete and was successful.
*/
TInt CScsiProtocol::ReadComplete(TInt aError)
	{
	TRACE_FUNC
	TRACE_INFO(( _L( "Error = 0x%X" ), aError ))
//	const TInt64 bOffset = iOffset;
	TUint8 lastCommand = iLastCommand;
	TUint lastLun = iLastLun;

	iOffset = 0;
	iLastCommand = EUndefinedCommand;
	iLastLun = KUndefinedLun;

	TRACE_INFO(( _L( "Last command was: %s" ),
			 (lastCommand == EUndefinedCommand) ? _S("Undefined") :
			 ((lastCommand == EWrite10) ? _S("EWrite10") :
			  ((lastCommand == EVerify10) ? _S("EVerify10") :
			   _S("Unknown"))) ))

	if (aError != KErrNone ||
		lastCommand == EUndefinedCommand ||
		lastLun == KUndefinedLun)
		{
		iSenseInfo.SetSense(TSenseInfo::EAbortedCommand);
		return KErrAbort;
		}

    CFileSystemImage* image = GetCheckFs(lastLun); 
    if ( image == NULL )
        {
        return KErrAbort;
        }
    else
        {
        iSenseInfo.SetSense(TSenseInfo::EAbortedCommand);
        }
	return iSenseInfo.SenseOk() ? KErrNone : KErrAbort;
	}


/**
Command Parser for the MODE SENSE(06) command (0x1A)

@return ETrue if successful.
*/
TBool CScsiProtocol::HandleModeSense(TPtrC8& aData, TUint /*aLun*/)
	{
	TRACE_FUNC

	TInt pageCode = aData[3] & 0x3F;
	TUint8 pageControl= static_cast<TUint8>(aData[3] >>6);

	// reserve 4 bytes for Length, Media type, Device-specific parameter and Block descriptor length
	iCommandBuf.FillZ(KModeSenseCommandLength);

	if (pageCode != KAllPages || pageControl == KChangeableValues) 
		{
		TRACE_ERROR(( _L( "TSenseInfo::EIllegalRequest,TSenseInfo::EInvalidFieldInCdb")))
		iSenseInfo.SetSense(TSenseInfo::EIllegalRequest,TSenseInfo::EInvalidFieldInCdb);
		return EFalse;
		}
	if (pageControl != KDefaultValues)
		{
	    iCommandBuf[2] = 1<<7;  // set SWP bit at the Device Specific parameters
		}

	iCommandBuf[0]=3;  //Sending only Mode parameter header

	TPtrC8 writeBuf = iCommandBuf;
	iTransport->SetupWriteData(writeBuf);

	return (iSenseInfo.SenseOk());
	}


const TUint16 KMaxModeRespLen = 58;
/**
Command Parser for the MODE SENSE(10) command (0x5A)

@return ETrue if successful.
*/  
TBool CScsiProtocol::HandleModeSense10(TPtrC8& aData, TUint /*aLun*/)
    {
    TRACE_FUNC
    
    TInt pageCode   = aData[3] & 0x3F;
    TUint8 pageControl= static_cast<TUint8>(aData[3] >>6);
    
    iCommandBuf.FillZ(KMaxModeRespLen);
    
    TBool allPages = EFalse;
    TUint16 len = static_cast<TUint16>((aData[8] << 8) | aData[9]);
    __ASSERT_DEBUG(len > 1, User::Panic(KUsbMsSvrPncCat, EMsWrongLength));

    iCommandBuf[2] = 0x71; // medium type
    
    TInt i = 8;
    
    switch (pageCode)
        {
        case 0x3F: // All mode pages
            allPages = ETrue;
        case 0x1A: // Power condition mode page
            // byte 0
            iCommandBuf[i++] = 0x1A; // page code
            iCommandBuf[i++] = 0x0A; // length
            iCommandBuf[i++] = 0x00; // reserved
            iCommandBuf[i++] = 0x00; // IDLE = 0, STANDBY = 0
            iCommandBuf[i++] = 0x00; 
            iCommandBuf[i++] = 0x00; 
            iCommandBuf[i++] = 0x00; 
            iCommandBuf[i++] = 0x00; // idle timer
            // byte 8
            iCommandBuf[i++] = 0x00; 
            iCommandBuf[i++] = 0x00;
            iCommandBuf[i++] = 0x00; 
            iCommandBuf[i++] = 0x00; // standby timer
            
            if (!allPages)
                {
                break;
                }
        case 0x1D: // Timeout and protect mode page
            // byte 0
            iCommandBuf[i++] = 0x1D; // page code
            iCommandBuf[i++] = 0x08; // length
            iCommandBuf[i++] = 0x00; // reserved
            iCommandBuf[i++] = 0x00; // reserver
            iCommandBuf[i++] = 0x00; // G3, TMOE, DISP, SWPP = 0, 0, 0, 0
            iCommandBuf[i++] = 0x00; // reserved
            iCommandBuf[i++] = 0x00; 
            iCommandBuf[i++] = 0x00; // group 1 timeout
            // byte 8
            iCommandBuf[i++] = 0x00; 
            iCommandBuf[i++] = 0x00; // group 2 timeout
            
            if (!allPages)
                {
                break;
                }
        case 0x2A: // MM capabilities and mechanical status page
            // byte 0
            iCommandBuf[i++] = 0x2A; // page code
            iCommandBuf[i++] = 0x1A; // length
            iCommandBuf[i++] = 0x00; // !CD-R
            iCommandBuf[i++] = 0x00;
            iCommandBuf[i++] = 0x30; // mode (mode 2, form 1,2)
            iCommandBuf[i++] = 0x00;
            iCommandBuf[i++] = 0x29; // tray, eject, lock
            iCommandBuf[i++] = 0x00;
            // byte 8
            iCommandBuf[i++] = 0x00; //
            iCommandBuf[i++] = 0x00; // obsolete
            iCommandBuf[i++] = 0x00; //
            iCommandBuf[i++] = 0x00; // volume levels
            iCommandBuf[i++] = 0x00; //
            iCommandBuf[i++] = 0x00; // buffer size
            iCommandBuf[i++] = 0x00; //
            iCommandBuf[i++] = 0x00; // obsolete
            // byte 16
            iCommandBuf[i++] = 0x00; // reserved
            iCommandBuf[i++] = 0x00; // unspecified
            iCommandBuf[i++] = 0x00; // obsolete
            iCommandBuf[i++] = 0x00; // obsolete
            iCommandBuf[i++] = 0x00; // obsolete
            iCommandBuf[i++] = 0x00; // obsolete
            iCommandBuf[i++] = 0x00; //
            iCommandBuf[i++] = 0x00; // copy management revision
            // byte 24
            iCommandBuf[i++] = 0x00; // reserved
            iCommandBuf[i++] = 0x00; // reserved
            iCommandBuf[i++] = 0x00; // reserved
            iCommandBuf[i++] = 0x00; // reserved
            
            break;
        default:
            // Unknown page code
            iSenseInfo.SetSense(TSenseInfo::EIllegalRequest,TSenseInfo::EInvalidFieldInCdb);
            return EFalse;
        }
   
    if (i > len)
        {
        // don't send more data than the host will accept
        i = len;
        }
    iCommandBuf.SetLength(i);
    iCommandBuf[1] = (TUint8)(i-2); // length will not exceed 255, so LSB is enough
    
    TPtrC8 writeBuf = iCommandBuf;
    iTransport->SetupWriteData(writeBuf);

    return (iSenseInfo.SenseOk());
    }

/**
Command Parser for the READ TOC/PMA/ATIP command (0x43)

@return ETrue if successful.
*/  
TBool CScsiProtocol::HandleReadTOC(TPtrC8& aData, TUint aLun)
    {
    TRACE_FUNC
    if ( GetCheckFs(aLun) == NULL )
        {
        return EFalse;
        }    
    
    TUint16 len = static_cast<TUint16>((aData[8] << 8) | aData[9]);
    
    if (len == 0)
        {
        return ETrue; // allocation length = 0 is not an error
        }
    else if (len > 20)
        {
        len = 20;
        }
    
    iCommandBuf.FillZ(len);  
    
    // TOC header
    iCommandBuf[0] = 0x00; // length MSB
    iCommandBuf[1] = len-2; // length LSB
    iCommandBuf[2] = 0x01; // first track
    iCommandBuf[3] = 0x01; // last track
    if (len >= 12)
        {
        // Track descriptor, track 1
        iCommandBuf[5] = 0x14; // ADR | CTRL
        iCommandBuf[6] = 0x01; // track
        // Track start address
        if (aData[2] & 0x02)
            {
            // TIME address = 0x00 00 02 00
            iCommandBuf[10] = 0x02;
            }
        }
    if (len >= 20)
        {
        // Track descriptor, lead-out
        iCommandBuf[13] = 0x14;
        iCommandBuf[14] = 0xaa;
        }
    
    TPtrC8 writeBuf = iCommandBuf;
    iTransport->SetupWriteData(writeBuf);

    return (iSenseInfo.SenseOk());
    }

const TUint16 KFeatureNums[] = { 0x0000, 0x0001, 0x0002, 0x0003,
                                 0x0010, 0x001E, 0x0100, 0x0105 };
const TInt KNumFeatures = sizeof(KFeatureNums) / sizeof(TUint16);
const TInt KMaxConfRespLen = 76;

/**
Command Parser for the GET CONFIGURATION command (0x46)

@return ETrue if successful.
*/  
TBool CScsiProtocol::HandleGetConfiguration(TPtrC8& aData, TUint aLun)
    {
    TRACE_FUNC
    if ( GetCheckFs(aLun) == NULL )
        {
        return EFalse;
        }    
    TUint blockSize = KDefaultBlockSize;

    TUint8 rt = aData[2] & 0x03;
    TUint16 feature = static_cast<TUint16>((aData[3] << 8) | aData[4]);
    TUint16 len =     static_cast<TUint16>((aData[8] << 8) | aData[9]);
    
    if (len == 0)
        {
        return ETrue; // allocation length = 0 is not an error
        }
    
    iCommandBuf.FillZ(KMaxConfRespLen);  
    
    // Feature header
    iCommandBuf[0] = 0x00;  // length 
    iCommandBuf[1] = 0x00;  // length 
    iCommandBuf[2] = 0x00;  // length 
    iCommandBuf[6] = 0x00;
    iCommandBuf[7] = 0x08;  // CD-ROM Profile 0x0008
    
    TInt i = 8;
    
    for (TInt f = 0; f < KNumFeatures; f++)
        {
        if ( ( ( rt == 2 ) && ( KFeatureNums[f] == feature ) ) ||
             ( ( rt != 2 ) && ( KFeatureNums[f] >= feature ) ) )
            {
            switch (KFeatureNums[f])
                {
                case 0x0000:
                    // Profile list 
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x00; // feature code = 0x0000
                    iCommandBuf[i++] = 0x03; // persistent = 1, current = 1
                    iCommandBuf[i++] = 0x04; // additional length (1 profile desc.)
                    // Profile descriptor
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x08; // profile 0x0008
                    iCommandBuf[i++] = 0x01; // current
                    iCommandBuf[i++] = 0x00; // reserved
                    break;
                case 0x0001:
                    // Core feature descriptor
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x01; // feature code = 0x0001
                    iCommandBuf[i++] = 0x07; // version = 0001b, persistent = 1, current = 1
                    iCommandBuf[i++] = 0x08; // additional length
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x08; // physical interface = 0x00000008 (USB)
                    iCommandBuf[i++] = 0x01; // DBE = 1
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00; // reserved
                    break;
                case 0x0002:
                    // Morphing feature descriptor
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x02; // feature code = 0x0002
                    iCommandBuf[i++] = 0x07; // version = 0001b, persistent = 1, current = 1
                    iCommandBuf[i++] = 0x04; // additional length
                    iCommandBuf[i++] = 0x02; // OCEvent = 1, ASYNC = 0
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00; // reserved
                    break;
                case 0x0003:
                    // Removable medium feature descriptor
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x03; // feature code = 0x0003
                    iCommandBuf[i++] = 0x03; // persistent = 1, current = 1
                    iCommandBuf[i++] = 0x04; // additional length
                    iCommandBuf[i++] = 0x29; // tray, eject, lock
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00; // reserved
                    break;
                case 0x0010:
                    // Random readable feature descriptor
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x10; // feature code = 0x0010
                    iCommandBuf[i++] = 0x03; // persistent = 1, current = 1
                    iCommandBuf[i++] = 0x08; // additional length
                    // Block Size
                    iCommandBuf[i++] = static_cast<TUint8>((blockSize & 0xFF000000) >> 24);
                    iCommandBuf[i++] = static_cast<TUint8>((blockSize & 0x00FF0000) >> 16);  
                    iCommandBuf[i++] = static_cast<TUint8>((blockSize & 0x0000FF00) >> 8);   
                    iCommandBuf[i++] = static_cast<TUint8>((blockSize & 0x000000FF));        
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x01; // blocking = 1
                    iCommandBuf[i++] = 0x00; // PP = 0
                    iCommandBuf[i++] = 0x00; // reserved
                    break;
                case 0x001E:
                    // CD Read feature descriptor
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x1E; // feature code = 0x001E
                    iCommandBuf[i++] = 0x0B; // version = 2, persistent = 1, current = 1
                    iCommandBuf[i++] = 0x04; // additional length
                    iCommandBuf[i++] = 0x00; // DAP = 0, C2 flags = 0, CD-Text = 0
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00; // reserved
                    break;
                case 0x0100:
                    // Power management feature descriptor
                    iCommandBuf[i++] = 0x01;
                    iCommandBuf[i++] = 0x00; // feature code = 0x0100
                    iCommandBuf[i++] = 0x03; // persistent = 1, current = 1
                    iCommandBuf[i++] = 0x00; // additional length = 0
                    break;    
                case 0x0105:
                    // Timeout feature descriptor
                    iCommandBuf[i++] = 0x01;
                    iCommandBuf[i++] = 0x05; // feature code = 0x0105
                    iCommandBuf[i++] = 0x07; // version = 1, persistent = 1, current = 1
                    iCommandBuf[i++] = 0x04; // additional length
                    iCommandBuf[i++] = 0x00; // G3 = 0
                    iCommandBuf[i++] = 0x00; // reserved
                    iCommandBuf[i++] = 0x00;
                    iCommandBuf[i++] = 0x00; // unit lenght = undefined
                    break;
                default:
                    break;
                }                
            }
        }
    iCommandBuf[3] = (TUint8)(i-4); // length LSB
    if (i > len)
        {
        // don't send more data than the host will accept
        i = len;
        }
    
    TPtrC8 writeBuf = iCommandBuf.Left(i);
    iTransport->SetupWriteData(writeBuf);

    return (iSenseInfo.SenseOk());
    }

