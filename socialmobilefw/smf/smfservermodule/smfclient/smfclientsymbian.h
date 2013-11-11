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
 * 
 * Describes the classes that implement the symbian client part of Smf server.
 * Private client implementation for Symbian.
 * 
 */

#ifndef SMFCLIENTSYMBIAN_H_
#define SMFCLIENTSYMBIAN_H_

#include <e32base.h>
#include <e32cmn.h>
#include <QStringList>
#include <smfglobal.h>
#include <smfobserver.h>
#include <smfprovider.h>

#include "smfclientsymbiansession.h"

/**
 * Private client implementation for Symbian.
 */
class CSmfClientSymbian : public CActive
{

public: // Constructors and destructors
	/**
	 * Two phase contruction - NewL
	 * @param aObserver For callback
	 */
	static CSmfClientSymbian* NewL(smfObserver* aObserver=NULL);
	
	/**
	 * Two phase contruction - NewLC
	 * @param aObserver For callback
	 */
	static CSmfClientSymbian* NewLC(smfObserver* aObserver);
	
	/**
	 * Destructor.
	 * Destroys the object and release all memory objects.
	 */
	virtual ~CSmfClientSymbian();
	
public: 
	/**
	* This overloaded API is for Synchronous calls to plugin APIs
	* @param aSerializedData serialized by the caller.
	* @param aInterfaceName Interface name
	* @param requestType Opcode
	* @param aMaxAllocation Max data size that is allocated in the client side for
	* receiving this data
	* @return QByteArray
	*/
	QByteArray sendRequest(QString aInterfaceName, SmfRequestTypeID requestType,
	TInt aMaxAllocation, QByteArray& aSerializedData);
	
	/**
	* This API is for Synchronous calls to server
	* @param aInterfaceName Interface name
	* @param requestType Opcode
	* @param maxSize Max data size that is allocated in the client side for
	* receiving this data
	* @param aSerializedData serialized by the caller.
	* @return QByteArray
	*/
	QByteArray sendSyncRequest ( QString aInterfaceName, 
			SmfRequestTypeID requestType, TInt maxSize, 
			QByteArray &aSerializedData );
	
	/**
	* For sending request specific to DSM. These are used by SmfRelationMngr API
	* @param requestType Opcode
	* @param aSerializedData Data to be passed to DSM through server, interpretation
	* depends on Relation manager and DSM
	* @maxSize Size to be allocated in the client side
	* @return Data received from server, interpretation depends on SmfRelationMngr and DSM
	*/
	QByteArray sendDSMSyncRequest ( SmfRequestTypeID requestType, 
			QByteArray& aSerializedData, SmfError& aErr, TInt maxSize = 10 );
	
	/**
	 * Calls SendReceive (Asynchronous) of the RSessionBase object.
	 * @param aSerializedData serialized by the caller.
	 * @param aInterfaceName Interface name
	 * @param requestType Opcode
	 * @param aMaxAllocation Max data size that is allocated in the 
	 * client side for receiving this data
	 * @param xtraInfo XtraInfo when required by server other than smfProvider
	 */
	TInt sendRequest( QByteArray& aSerializedData, QString aInterfaceName, 
			SmfRequestTypeID requestType, TInt aMaxAllocation );
		
	/**
	* CancelRequest.
	* Cancels an outstanding request.
	*/
	void CancelRequest() ;
	
protected: // Functions from the base class CActive
	/**
	 * From CActive, RunL.
	 * Callback function.
	 * Invoked to handle responses from the server.
	 */
	void RunL();
	
	/**
	 * From CActive, DoCancel.
	 * Cancels any outstanding operation.
	 */
	void DoCancel();
	
	/**
	 * From CActive, RunError.
	 */
	TInt RunError(TInt aError);
	
private: // Constructors
	/**
	 * CSmfClientSymbian.
	 * Performs the first phase of two phase construction.
	 */
	CSmfClientSymbian(smfObserver* aObserver);
	
	/**
	 * ConstructL.
	 * Performs the second phase construction of a
	 * CSmfClientSymbian object.
	 */
	void ConstructL();
	
private: // Data
	/**
	 * iSession, the server session.
	 */
	RSmfClientSymbianSession iSession;
	
	/**
	 * For callback, when the client gets serviced by the server
	 */
	smfObserver* iObserver;
	
	//Interface name, to be passed to the server
	TBuf<128> iInterfaceName;
	QString iInterfaceNameQt;
	//Data ptr
	TPtr iDataPtr;
	//Bytearray
	QByteArray* iBytearray;
	HBufC* iBuffer;
	TInt iMaxMessageSize;

	};

#endif /* SMFCLIENTSYMBIAN_H_ */
