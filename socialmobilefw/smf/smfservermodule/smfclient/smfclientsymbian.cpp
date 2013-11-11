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


#include <e32cmn.h>
#include <QtGlobal>
#include <e32svr.h>
#include <e32base.h>
#include <QByteArray>
#include <qdebug.h>

#include "smfclientsymbian.h"

// Function Declaration - For starting the server process
static TInt StartServerL();
static TInt CreateServerProcessL();


CSmfClientSymbian* CSmfClientSymbian::NewL(smfObserver* aObserver )
    {
	CSmfClientSymbian* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return( self ) ;
    }

CSmfClientSymbian* CSmfClientSymbian::NewLC(smfObserver* aObserver )
    {
	CSmfClientSymbian* self = new ( ELeave ) CSmfClientSymbian( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CSmfClientSymbian::CSmfClientSymbian(smfObserver* aObserver)
		: CActive( EPriorityStandard ),
		iObserver(aObserver),
		iDataPtr(NULL, 0, 0)
	{
	CActiveScheduler::Add(this);
	}

void CSmfClientSymbian::ConstructL()
    {
    User::LeaveIfError(iSession.connectToServer());
    }

CSmfClientSymbian::~CSmfClientSymbian()
	{
	qDebug()<<"~CSmfClientSymbian";
    Cancel(); // Causes call to DoCancel()
    iSession.Close();
	}

void CSmfClientSymbian::DoCancel()
	{
	Cancel();
	}

TInt CSmfClientSymbian::RunError(TInt aError)
	{
	qDebug()<<"Inside CSmfClientSymbian::RunError(), error = "<<aError;
	return KErrNone;
	}

void CSmfClientSymbian::RunL()
	{
	qDebug()<<"Inside CSmfClientSymbian::RunL()";
	qDebug()<<"iStatus = "<<iStatus.Int();
	
    switch ( iStatus.Int() )
        {
        case KErrCancel:
            // The request was canceled
        	qDebug()<<"KErrCancel";
            break ;

        case KErrNotReady:
        	qDebug()<<"KErrNotReady";
        	break;

        default:
        	{
        	qDebug()<<"RunL :- default";
        	//This contains error followed by actual data
        	QByteArray receivedData(reinterpret_cast<const char*>(iSession.iDataPtr8.Ptr()),iSession.iDataPtr8.Length());
        	qDebug()<<"receivedData size = "<<receivedData.size();

        	SmfError errVal;
        	int errInt;
        	QByteArray data;
        	QDataStream reader(&receivedData,QIODevice::ReadOnly);
        	reader>>errInt;
        	qDebug()<<"errInt = "<<errInt;

        	errVal = (SmfError)errInt;
        	reader>>data;
        	qDebug()<<"data size = "<<data.size();

        	SmfRequestTypeID opcode = (SmfRequestTypeID)iSession.getLastRequest();
        	if(iObserver)
        		{
				iObserver->resultsAvailable(data,opcode,errVal);
        		}
        	}
        	break;
        }
	}

// Sync request for server other than getservices
QByteArray CSmfClientSymbian::sendRequest(QString aInterfaceName, 
				 SmfRequestTypeID requestType, 
				 TInt aMaxAllocation,
				 QByteArray& aSerializedData)
	{
	//Gets data synchronously from the server
    TPtr8 symbianBuf(iSession.sendSyncRequest(aInterfaceName,requestType, aMaxAllocation, aSerializedData));
    //convert this into bytearray
    QByteArray receivedData(reinterpret_cast<const char*>(symbianBuf.Ptr()),symbianBuf.Length());
    return receivedData;
	}

QByteArray CSmfClientSymbian::sendSyncRequest(QString aInterfaceName,
		 SmfRequestTypeID requestType,TInt maxSize,
		 QByteArray& aSerializedData )
	{
	//This will be a synchronous request
	//note session is opened in ctor and closed in dtor
	qDebug()<<"Inside CSmfClientSymbian::sendSyncRequest()";
	qDebug()<<"Interface name = "<<aInterfaceName;

	//Gets data synchronously from the server
    TPtr8 symbianBuf(iSession.sendSyncRequest(aInterfaceName,requestType,maxSize, aSerializedData));
    //convert this into bytearray
    QByteArray receivedData(reinterpret_cast<const char*>(symbianBuf.Ptr()),symbianBuf.Length());
    return receivedData;
	}

QByteArray CSmfClientSymbian::sendDSMSyncRequest(SmfRequestTypeID requestType,QByteArray& aSerializedData,SmfError& aErr,TInt maxSize)
	{
	Q_UNUSED(aErr)
			
	qDebug()<<"CSmfClientSymbian::sendDSMSyncRequest for : "<<requestType;
	SmfError err = SmfNoError;
	//Gets data synchronously from the server
    TPtr8 symbianBuf(iSession.sendDSMSyncRequest(requestType,aSerializedData,err,maxSize));
    //convert this into bytearray
    QByteArray receivedData(reinterpret_cast<const char*>(symbianBuf.Ptr()),symbianBuf.Length());
    qDebug()<<"receivedData size="<<receivedData.size();
    return receivedData;
	}

TInt CSmfClientSymbian::sendRequest( QByteArray& aSerializedData, QString aInterfaceName,
		 SmfRequestTypeID requestType, TInt aMaxAllocation)
	{
	//RSessionBase objects sendreceive is called
	iSession.sendAsyncRequest(aSerializedData,aInterfaceName,requestType,iStatus,aMaxAllocation);
	SetActive();
	
	return KErrNone;
	}




RSmfClientSymbianSession::RSmfClientSymbianSession()
		:iDataPtr8(NULL, 0, 0),
		 iDataPtr16(NULL,0),
		 iPtrProvider(NULL,0),
		 iPtrProvider8(NULL,0),
		 iIntfNamePtr(NULL,0),
		 iIntfNamePtr8(NULL,0),
		 iPtrToXtraInfo(NULL, 0),
		 iPtrToXtraInfo8(NULL,0),
		 iPtr8ToSlot0(NULL,0)
    {
    // No implementation required
    }

TInt RSmfClientSymbianSession::connectToServer()
    {
    TInt error = ::StartServerL();
    qDebug()<<"StartServerL = "<<error;

    if ( KErrNone == error )
        {
		error = CreateSession(KSmfServerName,
                               Version(),
                               4 );	// 4 slots used
        qDebug()<<"CreateSession = "<<error;
        }
    return error;
    }

TPtr8 RSmfClientSymbianSession::sendSyncRequest(QByteArray& aSerializedData, 
		QString aInterfaceName, SmfRequestTypeID aRequestType, TInt maxSize)
	{
	iLastRequest = aRequestType;
	/**
	 * The message body consists of.- 
	 * 1. Serialized data to server SmfProvider+page information+extra information
	 * 2. Interface name as string ("org.symbian.smf.client.gallery")
	 * 3. Data pointer to be filled by serialized data(QList<smfProvider>)
	 * 4. Xtra information if any
	 */
	if(iProviderBuf8)
		{
		delete iProviderBuf8;
		iProviderBuf8 = NULL;
		}
	iProviderBuf8 = HBufC8::NewL(aSerializedData.size());
	iPtrProvider8.Set(iProviderBuf8->Des());
	iPtrProvider8.Copy(reinterpret_cast<const TText8*>(aSerializedData.constData()),aSerializedData.length());

	//convert the QByteArray into TPtr
	TPtrC8 ptrSlot0(reinterpret_cast<const TText8*>(aSerializedData.constData()),aSerializedData.length());
	qDebug()<<"ptrSlot0 size = "<<ptrSlot0.Size();
	
	iInterfaceNamebyte.clear();
	//Pass serialized QString for interface name
	QDataStream intfNameStream(&iInterfaceNamebyte,QIODevice::WriteOnly);
	intfNameStream<<aInterfaceName;
	qDebug()<<"iInterfaceNamebyte size = "<<iInterfaceNamebyte.size();
	if(iIntfNameBuffer8)
		{
		delete iIntfNameBuffer8;
		iIntfNameBuffer8 = NULL;
		}
	iIntfNameBuffer8 = HBufC8::NewL(iInterfaceNamebyte.size());
	iIntfNamePtr8.Set(iIntfNameBuffer8->Des());
	iIntfNamePtr8.Copy(reinterpret_cast<const TText8*>(iInterfaceNamebyte.constData()),iInterfaceNamebyte.length());
	qDebug()<<"After iIntfNamePtr8.Copy";

	if(iBuffer8)
		{
		delete iBuffer8;
		iBuffer8 = NULL;
		}
    iBuffer8 = HBufC8::NewL(maxSize);
    iDataPtr8.Set(iBuffer8->Des());
    qDebug()<<"After iDataPtr8.Set";

	TIpcArgs args;

    //filling the slots
    args.Set(0, &iPtrProvider8);
    args.Set(1, &iIntfNamePtr8);
    args.Set(2, &iDataPtr8);
    qDebug()<<"After setting 0,1,2 slots";

    qDebug()<<"Before handle";
    if (Handle()) 
    	{
        qDebug()<<"Before sendreceive";
        //synchronous request
        TInt sendErr = SendReceive(aRequestType, args);
        if(sendErr)
			qDebug()<<"SendReceive error = "<<sendErr;
        }
    return iDataPtr8;
	}



TPtr8 RSmfClientSymbianSession::sendSyncRequest(QString aInterfaceName,
		SmfRequestTypeID aRequestType,
		TInt maxSize,
		QByteArray& aSerializedData)
	{
	qDebug()<<"Inside RSmfClientSymbianSession::sendSyncRequest()";
	qDebug()<<"aInterfaceName = "<<aInterfaceName;
	
	iLastRequest = aRequestType;
	/**
	 * The message body consists of.- 
	 * slot 0 = SmfProvider + argument flag + argument + etc
	 * slot 1 = Interface name serialized
	 * slot 2 = Data pointer to be filled by server
	 * slot 3 = not used now
	 */
		
	//Convert the interface name into TPtr
	//lets pass serialized QString
	iInterfaceNamebyte.clear();
	QDataStream intfNameStream(&iInterfaceNamebyte,QIODevice::WriteOnly);
	intfNameStream<<aInterfaceName;
    qDebug()<<"iInterfaceNamebyte size = "<<iInterfaceNamebyte.size();
	if(iIntfNameBuffer8)
		{
		delete iIntfNameBuffer8;
		iIntfNameBuffer8 = NULL;
		}
	iIntfNameBuffer8 = HBufC8::NewL(iInterfaceNamebyte.size());
	iIntfNamePtr8.Set(iIntfNameBuffer8->Des());
	iIntfNamePtr8.Copy(reinterpret_cast<TUint8*>(iInterfaceNamebyte.data()),iInterfaceNamebyte.length());
    qDebug()<<"iIntfNamePtr8 (1) size = "<<iIntfNamePtr8.Size();
	
	if(iBuffer8)
		{
		delete iBuffer8;
		iBuffer8 = NULL;
		}
	qDebug()<<"Allocated for output = "<<maxSize;
    iBuffer8 = HBufC8::NewL(maxSize);
    iDataPtr8.Set(iBuffer8->Des());
    
	if(iProviderBuf8)
		{
		delete iProviderBuf8;
		iProviderBuf8 = NULL;
		}
	iProviderBuf8 = HBufC8::NewL(aSerializedData.size());
	iPtrProvider8.Set(iProviderBuf8->Des());
	iPtrProvider8.Copy(reinterpret_cast<const TText8*>(aSerializedData.constData()),aSerializedData.length());

	//convert the QByteArray into TPtr
	TPtrC8 ptrSlot0(reinterpret_cast<const TText8*>(aSerializedData.constData()),aSerializedData.length());
	qDebug()<<"ptrSlot0 size = "<<ptrSlot0.Size();
        
    TIpcArgs args;
    args.Set(0, &iPtrProvider8);
    args.Set(1, &iIntfNamePtr8);
    args.Set(2, &iDataPtr8);
        
    qDebug()<<"Before handle";
    if (Handle()) 
    	{
        qDebug()<<"Before sendreceive";
        TInt sendErr = SendReceive(aRequestType, args);
        if(sendErr)
			qDebug()<<"SendReceive error = "<<sendErr;
        }
    return iDataPtr8;
	}

/**
 * Sends sync DSM request to the Smf server
 */
TPtr8 RSmfClientSymbianSession::sendDSMSyncRequest(SmfRequestTypeID aRequestType,
		QByteArray& aSerializedData, SmfError& aErr, TInt maxSize)
	{
	/**
	 * Slot 0:- Data to be passed to DSM
	 * Slot 1:- Data returned from DSM
	 * Slot 2:- Error
	 */
	qDebug()<<"Inside RSmfClientSymbianSession::sendDSMSyncRequest()";
	iLastRequest = aRequestType;
	if(iSlot0Buffer8)
		{
		delete iSlot0Buffer8;
		iSlot0Buffer8 = NULL;
		}
	iSlot0Buffer8 = HBufC8::NewL(aSerializedData.size());
	iPtr8ToSlot0.Set(iSlot0Buffer8->Des());
	iPtr8ToSlot0.Copy(reinterpret_cast<const TText8*>(aSerializedData.constData()),aSerializedData.length());
	
	if(iBuffer8)
		{
		delete iBuffer8;
		iBuffer8 = NULL;
		}
    iBuffer8 = HBufC8::NewL(maxSize);
    iDataPtr8.Set(iBuffer8->Des());
    
    TIpcArgs args;
    args.Set(0, &iPtr8ToSlot0);
    args.Set(1, &iDataPtr8);
    iDSMErr.Zero();
    args.Set(2,&iDSMErr);
    
    TInt sendErr = SendReceive(aRequestType,args);
    qDebug()<<"DSM SendReceive = "<<sendErr;
    TInt numIndex;
    TLex iLex(iDSMErr);
    
    iLex.Val(numIndex);
    aErr = (SmfError)numIndex;
    return iDataPtr8;
	}

/**
 * sendAsyncRequest()
 * Calls SendReceive() after converting into symbian descriptors
 */
void RSmfClientSymbianSession::sendAsyncRequest(QByteArray& aSerializedData,
		QString aInterfaceName,
		SmfRequestTypeID aRequestType,
		TRequestStatus& aStatus,
		TInt aMaxAllocation )
	{
	/**
	 * The message body consists of.- 
	 * slot 0 = SmfProvider + argument flag + argument + etc
	 * slot 1 = Interface name serialized
	 * slot 2 = Data pointer to be filled by server
	 * slot 3 = not used now
	 */
	qDebug()<<"Inside RSmfClientSymbianSession::sendAsyncRequest()";
	qDebug()<<"iInterfaceName = "<<aInterfaceName;
	iLastRequest = aRequestType;
	
	if(iProviderBuf8)
		{
		delete iProviderBuf8;
		iProviderBuf8 = NULL;
		}
	iProviderBuf8 = HBufC8::NewL(aSerializedData.size());
	iPtrProvider8.Set(iProviderBuf8->Des());
	iPtrProvider8.Copy(reinterpret_cast<const TText8*>(aSerializedData.constData()),aSerializedData.length());

	//convert the QByteArray into TPtr
    TPtrC8 ptrSlot0(reinterpret_cast<const TText8*>(aSerializedData.constData()),aSerializedData.length());
    qDebug()<<"ptrSlot0 size = "<<iPtrProvider8.Size();
    
	//Convert the interface name into TPtr
    //Pass serialized QString for interface name
	iInterfaceNamebyte.clear();
	QDataStream intfNameStream(&iInterfaceNamebyte,QIODevice::WriteOnly);
	intfNameStream<<aInterfaceName;
	qDebug()<<"iInterfaceNamebyte size = "<<iInterfaceNamebyte.size();
	if(iIntfNameBuffer8)
		{
		delete iIntfNameBuffer8;
		iIntfNameBuffer8 = NULL;
		}
	iIntfNameBuffer8 = HBufC8::NewL(iInterfaceNamebyte.size());
	iIntfNamePtr8.Set(iIntfNameBuffer8->Des());
	iIntfNamePtr8.Copy(reinterpret_cast<const TText8*>(iInterfaceNamebyte.constData()),iInterfaceNamebyte.length());
	qDebug()<<"ptrSlot1 size = "<<iIntfNamePtr8.Size();
	
	if(iBuffer8)
		{
		delete iBuffer8;
		iBuffer8 = NULL;
		}
    iBuffer8 = HBufC8::NewL(aMaxAllocation);
    iDataPtr8.Set(iBuffer8->Des());
    qDebug()<<"After iDataPtr.Set";
    
    TIpcArgs args;
    
    //filling the slots
    args.Set(0, &iPtrProvider8);
    args.Set(1, &iIntfNamePtr8);
    args.Set(2, &iDataPtr8);
    qDebug()<<"After setting 0,1,2 slots";
    
    qDebug()<<"Before Handle()";
    if (Handle()) 
    	{
        qDebug()<<"Before sendreceive";
        SendReceive(aRequestType, args, aStatus);
        qDebug()<<"After sendreceive";
        }
	}

// -----------------------------------------------------------------------------
// CreateServerProcessL()
// Creates a server process
// -----------------------------------------------------------------------------
//
static TInt CreateServerProcessL()
    {
    TInt result;
    TUid KSmfServerUID3 = { 0xE5027327 };
    const TUidType serverUid( KNullUid, KNullUid, KSmfServerUID3 );

    RProcess server;

    result = server.Create( KSmfServerFilename, KNullDesC, serverUid );
    qDebug()<<"server.Create() = "<<result;
    if ( result != KErrNone )
        {
        return  result;
        }

    server.Resume();
    server.Close();
    return  KErrNone;
    }

// -----------------------------------------------------------------------------
// StartServerL()
// Starts the server - Internally creates a server process
// -----------------------------------------------------------------------------
//
static TInt StartServerL()
    {
    TInt result;

    TFindServer findSmfServer( KSmfServerFilename );
    TFullName name;

    result = findSmfServer.Next( name );
    if ( result == KErrNone )
        {
        // Server already running
        return KErrNone;
        }

    RSemaphore semaphore;
    result = semaphore.CreateGlobal( KSmfServerSemaphoreName, 0 );
    
    if ( result != KErrNone )
        {
        return  result;
        }

    result = CreateServerProcessL();
    qDebug()<<"CreateServerProcessL = "<<result;
    
    if ( result != KErrNone )
        {
        return  result;
        }

    semaphore.Wait();
    semaphore.Close();

    return KErrNone;
    }
