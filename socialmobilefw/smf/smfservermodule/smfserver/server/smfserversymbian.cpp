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
 * Description:
 * SMF Server private implementation for Symbian
 *
 */

#include <QDataStream>
#include <QDebug>
#include <smfrelationmgr.h>

#include "smfpluginmanager.h"
#include "smfserversymbian_p.h"


SmfServerSymbian* SmfServerSymbian::NewL(CActive::TPriority aActiveObjectPriority,SmfServer* aWrapper)
	{
	SmfServerSymbian* self = new(ELeave) SmfServerSymbian(aActiveObjectPriority,aWrapper);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void SmfServerSymbian::ConstructL()
	{
	}

SmfServerSymbian::SmfServerSymbian( CActive::TPriority aActiveObjectPriority,SmfServer* aWrapper )
: CPolicyServer(0,myPolicy, ESharableSessions), iWrapper(aWrapper)
	{
	Q_UNUSED(aActiveObjectPriority)
	}

SmfServerSymbian::~SmfServerSymbian()
	{
	}

TInt SmfServerSymbian::addToSessionMap(SmfServerSymbianSession* aSession,const RMessage2& aMsg)
	{
	qDebug()<<"Inside SmfServerSymbian::addToSessionMap()";
	
	if(iSessionCount)
		{
		//The key generation 
		TInt key = qrand();
		//in case qrand generates any duplicate key
		while(iMap.Find(key))
			{
			key = qrand();
			}

		CSessionStruct session;
		session.iMsg = aMsg;
		session.iSession = aSession;
		iMap.Insert(key,session); 
		
		return key;
		}
	
	return (-1);
	}

TInt SmfServerSymbian::removeFromSessionMap(SmfServerSymbianSession* aSession,RMessage2& aMsg)
	{
	//To be implemented
	Q_UNUSED(aSession)
	Q_UNUSED(aMsg)
	return 0;
	}


SmfServer* SmfServerSymbian::wrapper()
	{
	return iWrapper;
	}

CSession2* SmfServerSymbian::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	qDebug()<<"Inside SmfServerSymbian::NewSessionL()";
	
	Q_UNUSED(aVersion)
	// Check that the version is OK
	//    TVersion v( 0, 1, 0 );
	//    if (!User::QueryVersionSupported( v, aVersion ))
	//        User::Leave( KErrNotSupported ); 
	
	// Create the session.
	qDebug()<<"New session created";
	return new (ELeave) SmfServerSymbianSession( const_cast<SmfServerSymbian*>(this) );
	}

SmfServerSymbianSession* SmfServerSymbian::findSession(TInt id)
	{
	qDebug()<<"Inside SmfServerSymbian::findSession()";
	CSessionStruct* sessionStruct = iMap.Find(id);	
	if(sessionStruct)
		{
		qDebug()<<"Session id found";
		return sessionStruct->iSession;
		}
	else
		{
		qDebug()<<"Session id not found";
		return NULL;
		}
	}

TInt SmfServerSymbian::findAndServiceclient(TInt requestID,QByteArray* parsedData,SmfError error)
	{
	qDebug()<<"Inside SmfServerSymbian::findAndServiceclient()";
	SmfServerSymbianSession* sessionToservice = findSession(requestID);
	if(sessionToservice)
		{
		sessionToservice->resultsAvailable(parsedData,error);
		}
	return 0;
	}



SmfServerSymbianSession::SmfServerSymbianSession(SmfServerSymbian* aServer):
			iServer(aServer),iPtrToBuf(NULL,0) ,
			iIntfNameSymbian8(NULL,0), iProviderSymbian8(NULL,0),
			iIntfNameSymbian(NULL,0) ,iXtraDataPtr8(NULL,0),
			iPtrToDataForClient(NULL,0) ,iPtr8DataForDSM(NULL,0),iPtr8DataFromDSM(NULL,0)  
	{
	iServer->iSessionCount++;
	}

SmfServerSymbianSession::~SmfServerSymbianSession()
	{
	qDebug()<<"Inside SmfServerSymbianSession::~SmfServerSymbianSession()";
	//cleanup of client resources
	iServer->iSessionCount--;
	}

void SmfServerSymbianSession::clientAuthorizationFinished(bool success)
	{
	qDebug()<<"Inside SmfServerSymbianSession::clientAuthorizationFinished() = "<<success;
	
	//Client authorization failed
	if(!success)
		{
		//TODO:- Should use smf wide error instead
		iMessage.Complete(KErrPermissionDenied);
		}
	else
		{
		HandleClientMessageL(iMessage);
		}
	}

void SmfServerSymbianSession::resultsAvailable(QByteArray* parsedData,SmfError error)
	{
	qDebug()<<"Inside SmfServerSymbianSession::resultsAvailable()";
	
	//Note:- The order of serialization of parsedData - Error value followed by the data
	//parsedData is already serialized by PM
	
	//We should remove the request from the map as soon its no longer outstanding
	iServer->removeFromSessionMap(this,iMessage);
	
	//Note:- Session must take the ownership of the data being passed to the client session
	if(iDataForClient)
		{
		delete iDataForClient;
		iDataForClient = NULL;
		}
	iDataForClient = HBufC8::NewL(parsedData->size());
	iPtrToDataForClient.Set(iDataForClient->Des());
	iPtrToDataForClient.Copy(reinterpret_cast<const TText8*>(parsedData->constData()),parsedData->length());

	TInt writeErr = iMessage.Write(2,iPtrToDataForClient);
	qDebug()<<"iMessage.Write() = "<<writeErr;

	//signal completion for the last request
	iMessage.Complete(error);
	}

void SmfServerSymbianSession::ServiceL(const RMessage2& aMessage)
	{    
	qDebug()<<"Inside SmfServerSymbianSession::ServiceL() = "<<iMessage.Function();
	iMessage = aMessage;

	//construct the client auth id
	SmfClientAuthID clientAuthID;
	clientAuthID.pid = aMessage.SecureId();
	clientAuthID.session = this;
	//TODO:- No client pid checking?No capability? So why symbian client-server?
	HandleClientMessageL(iMessage);
	}

void SmfServerSymbianSession::HandleClientMessageL(const RMessage2& aMessage)
	{
	qDebug()<<"Inside SmfServerSymbianSession::HandleClientMessageL() = "<<aMessage.Function();
	
	/**Note:- Only ESmfGetService needs to be taken care separately as it doesn't involve createrequest for PM
	 *See SmfRequestTypeID for list of opcodes
	 *
	 *Two cases,-
	 *1.for ESmfGetServices we donno the provider info
	 *2. for rest of the cases, we know the provider info
	 *so it seems PM needs to provide two overloaded getPlugins API But for every client intf instanciation
	 *we would execute the same things twice.
	 *TODO:- to be changed after GetServices returns SmfProvider+pluginID 
	 * 
	 */
	if( (SmfGetService == aMessage.Function()) 			||
		(SmfPostGetMaxCharsInPost == aMessage.Function())	||
		(SmfPostGetMaxItems == aMessage.Function())		||
		(SmfPostGetSupportedFormats == aMessage.Function())||
		(SmfPostGetAppearanceSupport == aMessage.Function()))
			
		{
		HandleSyncServiceL(aMessage);
		}
	else if (SmfCancelRequest == aMessage.Function())
		{
		HandleCancelRequest(aMessage);
		}
	else if(aMessage.Function() == SmfRelationCreate ||
			aMessage.Function() == SmfRelationAssociate || 
			aMessage.Function() == SmfRelationSearchById ||
			aMessage.Function() == SmfRelationSearchByContact ||
			aMessage.Function() == SmfRelationCount ||
			aMessage.Function() == SmfRelationGet ||
			aMessage.Function() == SmfRelationGetAll ||
			aMessage.Function() == SmfRelationGetAllRelations ||
			aMessage.Function() == SmfRelationDeleteRelation ||
			aMessage.Function() == SmfRelationRemove
			)
		{
		HandleDSMServiceL(aMessage);
		}
	else
		{
		HandleCommonServiceL(aMessage);
		}
	}

void SmfServerSymbianSession::HandleCancelRequest(const RMessage2 & aMessage)
	{
	SmfError err = SmfNoError;
	// iLastRequest contains the last operations opcode, cwhich is to be cancelled.
	bool ret = SmfPluginManager::getInstance(iServer->wrapper())->cancelRequest(iLastRequest);
	
	iErrBuf.Zero();
	iErrBuf.AppendNum(err);
	iMessage.Write(2,iErrBuf);
	aMessage.Complete(iLastRequest);
	
	}

void SmfServerSymbianSession::HandleDSMServiceL(const RMessage2 & aMessage)
	{
	qDebug()<<"Inside SmfServerSymbianSession::HandleDSMServiceL()";
	iLastRequest = aMessage.Function();
	//TODO:-If DSM takes care of deserialization and formation of User and social 
	//profile from the params then switch case can be removed
	if(iData8ForDSM)
		{
		delete iData8ForDSM;
		iData8ForDSM = NULL;
		}
	
	switch(iLastRequest)
		{
		case SmfRelationCreate:
			{
			iData8ForDSM = HBufC8::New(maxSmfRelationIdSize);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			}
			break;
		case SmfRelationAssociate:
			{
			int maxAlloc = 1000;
			iData8ForDSM = HBufC8::New(maxAlloc);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			}
			break;
		case SmfRelationRemove:
			{
			iData8ForDSM = HBufC8::New(100);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			break;
			}
		case SmfRelationSearchById:
			{
			int maxAlloc = MaxSmfContactSize;
			iData8ForDSM = HBufC8::New(maxAlloc);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			}
			break;
		case SmfRelationSearchByContact:
			{
			int maxAlloc = 500; // hard coded in relation manager
			iData8ForDSM = HBufC8::New(maxAlloc);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			}
			break;
		case SmfRelationCount:
			{
			int maxAlloc = 100;
			iData8ForDSM = HBufC8::New(maxAlloc);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM);
			}
			break;
		case SmfRelationGet:
			{
			int maxAlloc = maxSmfRelationItemSize*maxRelationItems;
			iData8ForDSM = HBufC8::New(maxAlloc);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			}
			break;
		case SmfRelationGetAll:
			{
			int maxAlloc = maxSmfRelationItemSize*maxRelationItems;
			iData8ForDSM = HBufC8::New(maxAlloc);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			}
			break;
		case SmfRelationGetAllRelations:
			{
			int maxAlloc = maxSmfRelationItemSize*maxRelationItems;
			iData8ForDSM = HBufC8::New(maxAlloc);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			break;
			}
		case SmfRelationDeleteRelation:
			{
			int maxAlloc = 100;
			iData8ForDSM = HBufC8::New(maxAlloc);
			iPtr8DataForDSM.Set(iData8ForDSM->Des());
			TInt readerr0 = aMessage.Read(0,iPtr8DataForDSM); 
			break;
			}
		default:
			break;
		}
	//Convert into QByteArray
	QByteArray qtdataForDSM(reinterpret_cast<const char*>(iPtr8DataForDSM.Ptr()),iPtr8DataForDSM.Length()) ;
	QByteArray qtdataFromDSM;
	SmfRequestTypeID opcode = (SmfRequestTypeID)iLastRequest;
	SmfError dsmErr = iServer->wrapper()->sendToDSM(qtdataForDSM,opcode,qtdataFromDSM);
	if(dsmErr == SmfNoError)
		{
		if(qtdataFromDSM.size())
			{
			if(iData8FromDSM)
				{
				delete iData8FromDSM;
				iData8FromDSM = NULL;
				}
			int siz = qtdataFromDSM.size();
			qDebug()<<"Size of Data to be sent back thru DSM Create : "<<siz;
			iData8FromDSM = HBufC8::NewL(qtdataFromDSM.size());
			iPtr8DataFromDSM.Set(iData8FromDSM->Des());
			iPtr8DataFromDSM.Copy(reinterpret_cast<const TText8*>(qtdataFromDSM.constData()),qtdataFromDSM.length());
			TInt writeErr = aMessage.Write(1,iPtr8DataFromDSM);
			}
		}
	else
		{
		iDSMErr.Zero();
		TInt errInt = dsmErr;
		iDSMErr.AppendNum(errInt);
		TInt writeErr = aMessage.Write(2,iDSMErr);
		}
	aMessage.Complete(iLastRequest);
	}


void SmfServerSymbianSession::HandleSyncServiceL(const RMessage2 & aMessage)
	{
	qDebug()<<"Inside SmfServerSymbianSession::HandleSyncServiceL()";
	
	iLastRequest = aMessage.Function();
	
	// Following is the data format sent by client
	// 1. SmfProvider +PageInfo flag+ aPageNum + aPerPage (if pageinfoflag is set) + XtraInfo flag(size of xtra data) Serialized 
	// 2. Interface name as string ("org.symbian.smf.client.gallery")
	// 3. Data pointer to be filled by serialized data(eg: QList<smfProvider>)
	// 4. Input Data if xtra flag is set
	
	TInt intfNameSize = aMessage.GetDesLength(1);
	if(iIntfNameBuf8)
		{
		delete iIntfNameBuf8;
		iIntfNameBuf8 = NULL;
		}
	iIntfNameBuf8 = HBufC8::NewL(intfNameSize);
	iIntfNameSymbian8.Set(iIntfNameBuf8->Des());

	//read it into iIntfNameSymbian8
	aMessage.ReadL(1,iIntfNameSymbian8);
	
	QByteArray intfName(reinterpret_cast<const char*>(iIntfNameSymbian8.Ptr()),iIntfNameSymbian8.Length()) ;
	QDataStream readIntfNameStream(&intfName,QIODevice::ReadOnly);
	iInterfaceID.clear();
	readIntfNameStream>>iInterfaceID;
	qDebug()<<"After de-serializing into iInterfaceID = "<<iInterfaceID;
	
	//Interface names are diff in client and plugin, replacing *.client.* with *.plugin.*
	iInterfaceID.replace(QString(".client"),QString(".plugin"));
	
	// for get services
	if(SmfGetService == aMessage.Function())
		HandleGetService(aMessage, iInterfaceID);
	
	// for other services
	else
		{
		// read provider info
		TInt providerSize = aMessage.GetDesLength(0);
		if(iProviderBuf8)
			{
			delete iProviderBuf8;
			iProviderBuf8 = NULL;
			}
		iProviderBuf8 = HBufC8::NewL(providerSize);
		iProviderSymbian8.Set(iProviderBuf8->Des());
	
		//read it into iProviderSymbian8
		aMessage.ReadL(0,iProviderSymbian8);
	
		//convert SmfProvider info from Symbian into bytearray
		QByteArray providerBufQt(reinterpret_cast<const char*>(iProviderSymbian8.Ptr()),iProviderSymbian8.Length());
		qDebug()<<"providerBufQt.size = "<<providerBufQt.size();
	
		//now de-serialize it
		QDataStream readProviderStream(&providerBufQt,QIODevice::ReadOnly);
		SmfProvider provider;
		readProviderStream>>provider;
		QByteArray XtraBufQt;
		readProviderStream>>XtraBufQt;
		
		//Get the plugin ID who matches provider info for a given intf name
		SmfPluginID pluginID = iServer->wrapper()->getPlugin(iInterfaceID,provider);
		qDebug()<<"pluginID from PM = "<<pluginID;
	
		//we need to check only this pluginID is authorized
		iPluginIDList.clear();
		iPluginIDList<<pluginID;
		
		//iAuthList will contain pluginID for a successfull case
		iAuthList.clear();
		iServer->wrapper()->getAuthorizedPlugins(iPluginIDList,iAuthList);
		
		if(iAuthList.contains(pluginID))
			{
			//Plugin ID is authorised, service the request
			//Generate request id only if the plugin ID is authorised
			//request PM to get the data
			SmfRequestTypeID opcode = (SmfRequestTypeID)aMessage.Function();
			resultData.clear();
			SmfError err = iServer->wrapper()->sendToPluginManager(pluginID,iInterfaceID,opcode,XtraBufQt, resultData);
	
			if(resultData.size())
				{
				TPtrC8 resultPtr8(reinterpret_cast<const TText8*>(resultData.constData()),resultData.length());
				
				TInt writeErr = aMessage.Write(2,resultPtr8);
				qDebug()<<"aMessage.Write(2) = "<<writeErr;
				}
			else
				{
				iErrBuf.Zero();
				iErrBuf.AppendNum(err);
				iMessage.Write(2,iErrBuf);
				}
	
			//signal completion
			aMessage.Complete(iLastRequest);
			}
		else
			{
			SmfError err = SmfNoAuthorizedPlugin;
			iErrBuf.Zero();
			iErrBuf.AppendNum(err);
			iMessage.Write(2,iErrBuf);
			aMessage.Complete(iLastRequest);
			}
		}
	}

void SmfServerSymbianSession::HandleGetService(const RMessage2 & aMessage, const SmfInterfaceID& aInterfaceID)
	{
	qDebug()<<"Inside SmfServerSymbianSession::HandleGetService()";
	
	iServer->wrapper()->getPlugins(aInterfaceID,iPluginIDMap);
	
	//form the plugin id list from the map
	iPluginIDList.clear();
	iPluginIDList = iPluginIDMap.uniqueKeys();
	
	//iAuthList will be filled by credential manager
	iServer->wrapper()->getAuthorizedPlugins(iPluginIDList,iAuthList);

	// iPluginIDMap now contains SmfProvider info to return to the client
	// No need to add this to session map, as we are not requesting PM for this
	QMap<SmfPluginID, SmfProvider> tempMap;
	QMapIterator<SmfPluginID, SmfProvider> i(iPluginIDMap);
	while(i.hasNext()) 
		{
		i.next();
		if(iAuthList.contains(i.key()))
			{
			tempMap.insert(i.key(),i.value());
			}
		}
	//now tempMap contains the info to be passed to the client
	iPluginIDMap.clear();
	iPluginIDMap = tempMap;
	
	//form list of smfprovider from this map,-
	QList<SmfProvider> providerList = iPluginIDMap.values();
	
	//now serialize this list into bytearray
	resultData.clear();
	QDataStream stream(&resultData,QIODevice::WriteOnly);
	stream<<providerList;
	qDebug()<<"providerList.count() = "<<providerList.count();

	qDebug()<<"Before providerListSymbian";
	//now convert it into TPtr8
	TPtrC8 providerListSymbian(reinterpret_cast<const TText8*>(resultData.constData()),resultData.length());
	qDebug()<<"After providerListSymbian";
	
	TInt writeErr = aMessage.Write(2,providerListSymbian);
	qDebug()<<"aMessage.Write(2) = "<<writeErr;

	//signal completion
	TInt completion = SmfGetService;
	aMessage.Complete(completion);
	}

void SmfServerSymbianSession::HandleCommonServiceL(const RMessage2& aMessage)
	{
	qDebug()<<"Inside SmfServerSymbianSession::HandleCommonServiceL() = "<<aMessage.Function();
	iLastRequest = aMessage.Function();
	/**
	 * Note:- client sends message in the following format,-
	 * Slot 0:- SmfProvider* serialized+Page info flag+page number+per page (if page info flag)+xtra info flag
	 * Slot 1:- Interface Name buffer
	 * Slot 2:- Ptr to data block to be filled
	 * Slot 3 :- Xtra Data if xtra data flag
	 */
	//TODO:- Use macro instead, shared betn client-server
	TInt providerSize = aMessage.GetDesLength(0);
	if(iProviderBuf8)
		{
		delete iProviderBuf8;
		iProviderBuf8 = NULL;
		}
	iProviderBuf8 = HBufC8::NewL(providerSize);
	iProviderSymbian8.Set(iProviderBuf8->Des());
	qDebug()<<"data info (0) size = "<<iProviderSymbian8.Size();
	
	//read it into iProviderSymbian8
	aMessage.ReadL(0,iProviderSymbian8);
	
	//convert SmfProvider info from Symbian into bytearray
	QByteArray providerBufQt(reinterpret_cast<const char*>(iProviderSymbian8.Ptr()),iProviderSymbian8.Length());
	qDebug()<<"providerBufQt.size = "<<providerBufQt.size();

	//now de-serialize it
	QDataStream stream(&providerBufQt,QIODevice::ReadOnly);
	SmfProvider provider;
	stream>>provider;
	QByteArray XtraBufQt;
	stream>>XtraBufQt;
	qDebug()<<"XtraBufQt size = "<<XtraBufQt.size();
	TInt intfNameSize = aMessage.GetDesLength(1);
	if(iIntfNameBuf8)
		{
		delete iIntfNameBuf8;
		iIntfNameBuf8 = NULL;
		}
	iIntfNameBuf8 = HBufC8::NewL(intfNameSize);
	iIntfNameSymbian8.Set(iIntfNameBuf8->Des());

	//read it into iIntfNameSymbian8
	aMessage.ReadL(1,iIntfNameSymbian8);
	qDebug()<<"iIntfNameSymbian8 (1) .Size = "<<iIntfNameSymbian8.Size();

	QByteArray bytearray(reinterpret_cast<const char*>(iIntfNameSymbian8.Ptr()),iIntfNameSymbian8.Length()) ;
	QDataStream intfNameStream(&bytearray,QIODevice::ReadOnly);
	iInterfaceID.clear();
	intfNameStream>>iInterfaceID;
	qDebug()<<"Interface Name = "<<iInterfaceID;
	
	//Interface names are diff in client and plugin, replacing *.client.* with *.plugin.*
	iInterfaceID.replace(QString(".client"),QString(".plugin"));
	
	//Get the plugin ID who matches provider info for a given intf name
	SmfPluginID pluginID = iServer->wrapper()->getPlugin(iInterfaceID,provider);
	qDebug()<<"pluginID from PM = "<<pluginID;

	//we need to check only this pluginID is authorized?
	iPluginIDList.clear();
	iPluginIDList<<pluginID;
	
	//iAuthList will contain pluginID for a successfull case
	iAuthList.clear();
	iServer->wrapper()->getAuthorizedPlugins(iPluginIDList,iAuthList);
	if(iAuthList.contains(pluginID))
		{
		//Plugin ID is authorised, service the request
		//Gnerate request id only if the plugin ID is authorised
		TInt id = iServer->addToSessionMap(this,aMessage);
		//request PM to get the data
		SmfRequestTypeID opcode = (SmfRequestTypeID)iLastRequest;
		
		SmfError err = iServer->wrapper()->sendToPluginManager(id,pluginID,iInterfaceID,opcode,XtraBufQt);
		if(SmfNoError != err)
			{
			iErrBuf.Zero();
			iErrBuf.AppendNum(err);
			iMessage.Write(2,iErrBuf);
			
			//signal completion for the last request
			iMessage.Complete(err);
			}
		}
	else
		{
		SmfError err = SmfNoAuthorizedPlugin;
		iErrBuf.Zero();
		iErrBuf.AppendNum(err);
		iMessage.Write(2,iErrBuf);
		
		//signal completion for the last request
		iMessage.Complete(err);
		}
	}
