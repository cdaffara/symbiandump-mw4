/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0" 
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Chandradeep Gandhi, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Contributors:
 * Manasij Roy, Nalina Hariharan
 */

#ifndef SMFCLIENTSYMBIANSESSION_H_
#define SMFCLIENTSYMBIANSESSION_H_

#include <e32cmn.h>
#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QList>
#include <smfglobal.h>

/**
 * Corresponds to client session in Symbian
 */
class RSmfClientSymbianSession : public RSessionBase
{
public:
	/**
	 * Constructs
	 */
	RSmfClientSymbianSession();
	
	/**
	 * Creates the server process if not already running and creates session
	 * @return Error code while creating server process/ creating session
	 */
    TInt connectToServer();
    
	/**
	 * Version
	 */
    TVersion Version() const{return TVersion(0,0,1);};
    
    void Close(){};
    
	bool isConnected();
	
	bool connected;

    /**
     * Calls SendReceive with requestType opcode and packaged data
     * depending on requestType
     */
    TInt sendRequest(SmfRequestTypeID requestType);
    
	/**
	 * Sends sync request to the Smf server - overloaded
	 */
	TPtr8 sendSyncRequest(QByteArray& aSerializedData, 
			QString aInterfaceName, 
			SmfRequestTypeID aRequestType, 
			TInt maxSize);
    
	/**
	 * Sends DSM related sync request to the Smf server
	 */
	TPtr8 sendDSMSyncRequest(SmfRequestTypeID aRequestType,	
			QByteArray& aSerializedData,
			SmfError& aErr, TInt maxSize = 0 );
    
	/**
	 * Sends synchronous requests to the Smf server
	 */
	TPtr8 sendSyncRequest( QString aInterfaceName, 
			SmfRequestTypeID aRequestType,
			TInt maxSize, QByteArray& aSerializedData );
	
	/**
	 * Sends async request to the Smf server
	 * @param aSerializedData Serialized data to be sent to the server.
	 * Consists of provider Info(SmfProvider*)+ Other common class data
	 * (when applicable)-serialized
	 * @param aInterfaceName Name of the interface
	 * @param aStatus TRequestStatus
	 * @param maxSize Size of the data to be allocated. This info is sent to the server
	 */
	void sendAsyncRequest(QByteArray& aSerializedData,
			QString aInterfaceName, SmfRequestTypeID aRequestType,
			TRequestStatus& aStatus, TInt aMaxAllocation);
	
    /**
     * Returns data,used to return back the data received from server
     */
    TPtr8 getData()
		{
		 return iDataPtr8;
		}
    
    TInt getLastRequest()
    	{
    	return iLastRequest;
    	}
	TPtr8 iDataPtr8;
	TPtr iDataPtr16;
private:
	/**
	 * Following are to form message body.
	 */
	QByteArray iBaseProvider;
    TBuf8<525> iProviderSymbian;
    TBuf<125> iInterfaceSymbian;
    TBuf8<125> iInterfaceSymbian8;
    QString iInterfaceName;
    HBufC* iIntfNameBuffer;
    HBufC* iBuffer16;
    TPtr iPtrProvider;
	TPtr8 iPtrProvider8;
	QByteArray iInterfaceNamebyte;
	TBuf<100> iDSMErr;
	HBufC8* iProviderBuf8;
    TPtr iIntfNamePtr;
	TPtr8 iIntfNamePtr8;
	HBufC8* iIntfNameBuffer8;
	HBufC8* iBuffer8;
	HBufC8* iXtraInfoBuf8;
    TPtr iPtrToXtraInfo;
    TPtr8 iPtrToXtraInfo8;
	HBufC8* iSlot0Buffer8;
	TPtr8 iPtr8ToSlot0;
	TInt iMaxMessageSize;
	TInt iMaxSize;
	/**
	 * Last request opcode
	 */
	TInt iLastRequest;
	
private:
	QList<QVariant*> result;

	};

#endif /* SMFCLIENTSYMBIANSESSION_H_ */
