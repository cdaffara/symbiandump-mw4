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
* Description:  An interface for starting PTP Server and connecting/
*                disconnecting PTP Stack.
*
*/


#ifndef  RPTP_H
#define  RPTP_H

#include <e32base.h>

const TInt KFileExtLength = 8;
enum TDpsPrinterState
    {
    EPrinterNotAvailable,
    EPrinterAvailable
    };
        

/**
*  RPtp class is used to create a PTP Server session and a connection to the
*  selected transport medium.
*
*  @lib rptp.lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS(RPtp) : public RSessionBase
	{
	
public:

	/**
	* C++ default constructor.
	*/
	
	IMPORT_C RPtp();
	
	/**
	* Creates new session to PTP Server. This also register the observer
	*  to the ptp server. This should be only used by the client other than
	*   the Ptp CC
	* @since S60 3.2
	* @return KErrNone if success or system error if failed.
	*/	
	IMPORT_C TInt Connect();
	
	/**
    * Creates connection to the server and initializes the ptp stack based on
    * specified transport medium.
    * @since S60 3.2
    * @param aTransport, Implementation UID of selected transport medium.
    * 
    * @return KErrNone if success or system error if failed.
    */
	IMPORT_C TInt CreatePtpStack( const TUid aTransport );
	
	/**
    * Performs DPS discovery.
    * 
    * @since S60 3.2
    * @param aStatus the result of the Dps discovery, either EAvailable or
    *   NotAvailable from TDpsPrinterState enum
    */
	IMPORT_C void IsDpsPrinter(TRequestStatus& aStatus);
	
	/**
	* Cancels previously issued Asynch request IsDpsPrinter
	* @since S60 3.2
	*/
	IMPORT_C void CancelIsDpsPrinter();
		
	/**
	* Client sends object and get informed when the object is received by
	* the host. 
	* @since S60 3.2
	* @param aNameAndPath, the name and full path of the file.
	* @param aAdd  Whether the ObjectAdded event should be sent out for this
	*   object (DPS operation may require this)
	* @param aTimeout whether the client needs the reply for this request. If
	*   it needs the timeout will be true, which means if timout happened, 
	*   the reply is KErrTimedout
	* @param aStatus, Asynchronous completion word for the operation.
	*/
	IMPORT_C void SendObject(const TDesC& aNameAndPath, 
	                         TRequestStatus& aStatus, TBool aTimeout,
	                         TInt aSize, TBool aAdd = EFalse);
	
	/**
	* Cancel object sending request.
	* @since S60 3.2
	*/
	IMPORT_C void CancelSendObject();
		
	/**
	*   Registers the certain files arrival (from the host) notification. 
	* @since S60 3.2
	* @param aFileExtension.  file extension
	* @param aNameAndPath  name and path of the file.
	* @param aDelete whether this object should be deleted after the 
	*  notification. If it is, a ObjectRemoved event should be sent also.
	* (Dps operation may need this)
	* @param aStatus, Asynchronous completion word for the operation.
	* @return None.
	*/
	IMPORT_C void ObjectReceivedNotify(const TDesC& aFileExtension,
	                                   TDes& aNameAndPath, 
	                                   TRequestStatus& aStatus,
	                                   TBool aDelete = EFalse);
	
	/**
	* Gets the object handle (whether the object exists).
	* The client may use this function to delete certain file 
	* (mainly for dps now)  
	* @since S60 3.2
	* @param aNameAndPath, A name and full path of the file.
	* @param aHandle , 32bit handle of the object
	* @param aAdd if this object is not find whether add it to the list
	* @return KErrNone if object is found or KErrNotFound if not.
	*/
	IMPORT_C TInt GetObjectHandleByName(const TDesC& aNameAndPath, 
	                                    TUint32& aHandle, 
	                                    TBool aAdd = EFalse);
	
	/**
	* Function 
	* @since S60 3.2
	* @param aNameAndPath, A name and full path of the file.
	* @param aHandle , 32bit handle of the objcet
	* @return KErrNone if successfull or system error if failed
	*/
	IMPORT_C TInt GetNameByObjectHandle(TDes& aNameAndPath, 
	                                    const TUint32 aHandle );
	
	
	/**
	* Cancel Object received notification 
	* @since S60 3.2
	*/
	IMPORT_C void CancelObjectReceivedNotify();
	
	/**
	* Disconnects PTP from transport module and deletes the PTP stack.
	* @since S60 3.2
	* Must be only used by PTP Class Controller
	*/		
	IMPORT_C void DestroyPtpStatck();
	
	/**
	* Returns name and path of folder
	* @since S60 3.2 
	* @return Folder Name and Path
	*/
	IMPORT_C const TDesC& PtpFolder();
    
private:
    
    /**
    * only used by the client other than PTP Class Controller. There are only two clients
    * connects to the server at the same time. PTP Class Controller only starts and closes
    * the PTP stack.As the result not DeRegisterObserver is needed.
    * @since S60 3.2
    *   @return KErrNone if success or system error if failed
    */
    TInt RegisterObserver();    
    
private:
    
    TBuf<KFileExtLength> iExt;
    TFileName iFile;
	};
	
#endif // RPTP_H

