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
// Description: Class declaration for CBulkOnlyTransport.
// 
// 

#ifndef BULKONLYTRANSPORT_H
#define BULKONLYTRANSPORT_H

#include <e32std.h> 

#include <d32usbc.h>
#include "protocol.h"  
#include "mscfilecontroller.h"

static const TInt KCbwLength = 31;

// for control endpoint 
static const TUint KRequestHdrSize = 8;


/** size of buffer for command padding */
static const TUint KBOTMaxBufSize 		= 1024; 

/**
Represent Endpoint0 request
*/
class TUsbRequestHdr
	{
public:
	enum TEp0Request
		{
		EReqGetMaxLun = 0xFE,
		EReqReset	  = 0xFF
		};
public:
	TInt Decode(const TDesC8& aBuffer);
	TBool IsDataResponseRequired() const;

public:
	TUint8	iRequestType;
	TEp0Request	iRequest;
	TUint16 iValue;
	TUint16 iIndex;
	TUint16 iLength;


	};


//---------------------------------------
class CBulkOnlyTransport;  //forward declaration



//
// --- class CActiveDeviceStateNotifier -----------------------------
//

class CActiveDeviceStateNotifier : public CActive
	{
public:
	// Construction
	static CActiveDeviceStateNotifier* NewL(CBulkOnlyTransport& aParent);

	// Destruction
	~CActiveDeviceStateNotifier();

	void Activate();

private:
	// Construction
	CActiveDeviceStateNotifier(CBulkOnlyTransport& aParent);
	void ConstructL();

	// Cancel request.
	// Defined as pure virtual by CActive;
	// implementation provided by this class.
	virtual void DoCancel();

	// Service completed request.
	// Defined as pure virtual by CActive;
	// implementation provided by this class,
	virtual void RunL();

private:
	CBulkOnlyTransport& iParent;
	TUint iDeviceState;
	TUint iOldDeviceState;
	
	};
//=====================




/**
Represent session with control endpoint (Ep0).
handles the control interface, and responds to the class specific commands (RESET and GET_MAX_LUN).  
*/
class CControlInterface : public CActive
	{

public:
	enum TControlState
		{
		ENone,
		EReadEp0Data,
		ESendMaxLun

		};

public:
	
	static CControlInterface* NewL(CBulkOnlyTransport& aParent);
	
	~CControlInterface();
	TInt Start();
	void Stop();
	virtual void RunL();
	virtual void DoCancel();
			

private:
	CControlInterface(CBulkOnlyTransport& aParent);
	void ConstructL();
	TInt ReadEp0Data();
	void DecodeEp0Data();

private:
	/** Buffer for request data*/
	TBuf8<KRequestHdrSize> iData;
	TUsbRequestHdr iRequestHeader;
	/** reference to the  CBulkOnlyTransport*/
	CBulkOnlyTransport& iParent;
	/** represent carrent state for state mashine */
	TControlState iCurrentState;
	};

//----------------------
/** handles the data transport and communications with the SCSI protocol */
class CBulkOnlyTransport : public CActive, public MTransportBase
	{
public:
	enum TCswStatus
		{
		ECommandPassed 	= 0,
		ECommandFailed	= 1,
		EPhaseError		= 2
		};
	
	enum TTransportState
		{
		ENone,
		EWaitForCBW,
		ESendingCSW,
		EWritingData,
		EReadingData,
        EPermErr
		};
public:
	static CBulkOnlyTransport* NewL(TInt aNumDrives,CMscFileController& aController);

	~CBulkOnlyTransport();
	void SetupReadData(TPtr8& aData);	
	void SetupWriteData(TPtrC8& aData);
	TInt Start();
	TInt Stop();
	void RegisterProtocol(MProtocolBase& aProtocol);
	TInt BytesAvailable();

	CMscFileController& Controller();
	TInt MaxLun();
	RDevUsbcClient& Ldd();
	TInt HwStart(TBool aDiscard = EFalse);
	TInt HwStop();
	TInt HwSuspend();
	TInt HwResume();
	
	virtual void RunL();
	virtual void DoCancel();

private:
	CBulkOnlyTransport(TInt aNumDrives,CMscFileController& aController);	
	void ConstructL();
	TInt SetupConfigurationDescriptor(TBool aUnset = EFalse);
	TInt SetupInterfaceDescriptors();	
	void ReadCBW();
	void DecodeCBW();
	TBool CheckCBW();
	void SetPermError();
	void SendCSW(TUint aTag, TUint aDataResidue, TCswStatus aStatus);
	void WriteData(TUint aLength, TBool aZlpRequired = EFalse);
	void ReadData(TUint aLength);
	void StallEndpointAndWaitForClear(TEndpointNumber aEndpoint);
	void ReadAndDiscardData(TInt aBytes);
	void Activate(TInt aReason);
    void PrepareReadFromHost(TUint aHostDataLength);
    void PrepareWriteToHost(TUint aHostDataLength);
	
private:
	/** maximun logic unit number supported (started from 0) */
	TInt iMaxLun;
	
	CMscFileController& iController;
	
	CControlInterface* iControlInterface;
	MProtocolBase* iProtocol;
	RDevUsbcClient iLdd;
	TTransportState  iCurrentState;
	
	/** buffer for Command Block Wrapper */
	TBuf8 <KCbwLength+1> iCbwBuf;
	
	/** Pointer to read buffer (bufer itself provided by protocol) */
	TPtr8 iReadBuf; 
	
	/** Pointer to write buffer (bufer itself provided by protocol) */
	TPtrC8 iWriteBuf;

	/** Shows how many data was not sent/reseived */
	TUint32 iDataResidue;
	TUint32 iCbwTag;
	TCswStatus iCmdStatus;
	
	/** Indicate if SCSI protocol has data to sent */
	TBool iWriteSetUp;
	
	/** Indicate if SCSI protocol expected additional data */
	TBool iReadSetUp;
	
	/** Indicate whether SCSI protocol started or not */
	
	TBool iStarted;
	
	/** internal buffer for padding */
	TBuf8<KBOTMaxBufSize> iBuf;
	
	TBool iStallAllowed;
	
	CActiveDeviceStateNotifier* iDeviceStateNotifier;
	TBool iInterfaceConfigured;
	};
	
#endif // BULKONLYTRANSPORT_H	
