/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PTP tranport plugin interface API
*
*/



#ifndef PTP_TRANSPORT_H
#define PTP_TRANSPORT_H


// INCLUDES
#include <e32base.h>

class MNPtpCallback;
const TUid KPtpTransportInterfaceUid = { 0x1020E46C };

// Dataset to send/receive operations, responses and events.
class TNPtpContainer
	{
	public:
	TUint16 iOperationCode;
	TUint16 iResponseCode;
	TUint32 iSessionId; 
	TUint32 iTransactionId;
	TUint32 iParam1;
	TUint32 iParam2;
	TUint32 iParam3;
	// events can only have three parameters.
	TUint32 iParam4;
	TUint32 iParam5;
	}; 

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// PTPTransport ECOM plugin interface.
// This is the plugin interface that each plugin module must implement to provide a PTP transport
// layer interface. Currently only USB media is supported, but each created plugin module must 
// implement this interface.
// 
//  @lib ptptransport.lib
// ------------------------------------------------------------------------------------------------

class CNPtpTransport : public CBase 
    {

public:	// Constructors and destructor

	/**
    * C++ Consructor.
    */
	IMPORT_C CNPtpTransport();
    /**
    * Destructor.
    */
	IMPORT_C ~CNPtpTransport();
		
	/**
    * Two-phased constructor.
    */	
	IMPORT_C static CNPtpTransport* NewL( const TUid aTransport, MNPtpCallback* aCallback );		
				
public:	// API

	/**
	* Connects the client transport interface with the "Host".
	* Can be used with Bluetooth or TCP/IP transport plugin adapters in the future,
	* USB plugin (SICD) does not need a separate connection because it is created
	* when the USB cable is attached to the device.
	* @since S60 3.2
	* @return None.
	*/	
	IMPORT_C void ConnectL( ); 

	/**
	* Disconnects the client transport interface with the "Host".
	* Can be used with Bluetooth or TCP/IP transport plugin adapters in the future,
	* USB media disconnection is not needed. The connection may exists until
	* SICD component is deleted or cable is unattached.
	* @since S60 3.2
	* @return None.
	*/	
	IMPORT_C void DisconnectL();
	
	/**
	* Sends an PTP event to the connected "Host". Particulary used with USB media.
	* @param aStatus, Asynchronous completion word for the operation.
	* @param aEventData, PTP container containing all the information for the event.
	* @since S60 3.2
	* @return None.
	*/
	IMPORT_C void SendEventL( TRequestStatus& aStatus, const TNPtpContainer& aEventData );
	
    /**
   	* Cancels the asynchronous event sending operation.
	* @since S60 3.2
	* @return none    	
    */	
	IMPORT_C void CancelEvent();

protected:

	// hide the virtual methods from the public interface to enable future extension without breaking BC
	virtual void DoConnectL() = 0;
	virtual void DoDisconnectL() = 0;
	virtual void DoSendEventL( TRequestStatus& aStatus, const TNPtpContainer& aEventData ) = 0;
	virtual void DoCancelEvent() = 0;
	
private:
	TUid iEComTag;
    };


void CleanupEComArray(TAny* aArray);

#endif // PTP_TRANSPORT_H