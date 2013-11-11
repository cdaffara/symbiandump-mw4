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
 * 
 */

#include <dsm.h>
#include <QDebug>

// Static data initialization
DataStoreManager* DataStoreManager::dsmInstance = NULL;
const QString DataStoreManager::dbName = "dsm.db";
DataStoreManagerState DataStoreManager::state = CLOSED;

//! \fn getDataStoreManager()
/*!
    \brief  To get a handle on the datastore instantiation.
    \brief  This function returns a pointer to the existing DataStoreManager instantiation(object).
            If no object exists, a new one is created.

    \return Pointer to the current instantiation of DataStoreManager.
*/
DataStoreManager* DataStoreManager::getDataStoreManager(){
    if(dsmInstance == NULL){
    dsmInstance = new DataStoreManager(DataStoreManager::dbName);
        if(!(dsmInstance->InitializeDataBase())){
            return NULL;
        }
    }
    return  dsmInstance;
}

//! \fn ~DataStoreManager()
/*!
    \brief  Destructor for DataStoreManager class.

    \note   Should this be public?
*/
DataStoreManager::~DataStoreManager(){
    db.close();
    state = CLOSED;
    delete dsmInstance;
}

//! \fn getState()
/*!
    \brief  Get the current state of the DataStoreManager object.
    \return The state of the DSM object.
*/
DataStoreManagerState DataStoreManager::getState() const{
    return state;
}

//! \fn getError()
/*!
    \brief  Get the last error message from the DataStoreManager object.
    \return The last error message string of the DSM object.
*/
QString DataStoreManager::getErrorText() const{
    return lastMsg;
}

SmfRelationId DataStoreManager::create(SmfProvider *aProvider, SmfContact *aContact)
	{
	if(NULL == aProvider || NULL == aContact)
		return QString::number(-1);//SmfUnknownError 
	
	QString userId, contactUrl, localId, managerUri, presenceState, presenceText, statusText;
	QDateTime dateTime;
	Int64 timeStampInSeconds;
	
	QContactGuid guid = aContact->value("Guid").value<QContactGuid>();
	userId = guid.guid();
	qDebug()<<"Inside DSM: userID "<<userId;
	
	QContactUrl url = aContact->value("Url").value<QContactUrl>();
	contactUrl = url.url();
	
	qDebug()<<"Inside Test DSM: contactUrl "<<contactUrl;
	
	QContactId contactId = aContact->value("ContactId").value<QContactId>();
	localId =  contactId.localId() ;
	managerUri = contactId.managerUri();
	
	qDebug()<<"Inside Test DSM: localId"<<localId;
	qDebug()<<"Inside Test DSM: managerUri"<<managerUri;
	
	QContactTimestamp time = aContact->value("Timestamp").value<QContactTimestamp>();
	dateTime = time.created();
	//Returns the datetime as the number of seconds that have passed since 1970-01-01T00:00:00, Coordinated Universal Time (Qt::UTC).
	timeStampInSeconds =  dateTime.toTime_t() ;
		
	QContactPresence presence = aContact->value("Presence").value<QContactPresence>();	
	presenceState = QString::number( (int)presence.presenceState() );
	presenceText = presence.presenceStateText();
	statusText = presence.customMessage();
	
	qDebug()<<"Inside Test DSM: presenceState"<<presenceState;
	qDebug()<<"Inside Test DSM: presenceText"<<presenceText;
	qDebug()<<"Inside Test DSM: statusText"<<statusText;
	
	QString snsName = aProvider->serviceName();
	QString snsUrl = (aProvider->serviceUrl()).toString();
	QString snsDesc = aProvider->description();
		
	qDebug()<<"Inside Test DSM Create: snsName"<<snsName;
	qDebug()<<"Inside Test DSM Create: snsUrl"<<snsUrl;
	qDebug()<<"Inside Test DSM Create: snsDesc"<<snsDesc;
	
	
	
	const int contactID = addContactToTable(userId, contactUrl, localId, managerUri, snsName, snsDesc, snsUrl, presenceState, presenceText , statusText, timeStampInSeconds);
	if(!(contactIfExist(contactID)))
		return QString::number(-1);
	return QString::number( addRelationToTable( contactID ) );
}

SmfError DataStoreManager::associate( 	SmfRelationId aPhoneId,	
										const SmfContact* aContact, 
										SmfProvider* aProvider){
	if(NULL == aProvider || NULL == aContact)
			return SmfError(-1);//SmfUnknownError
	
	if( ! relationIfExist( aPhoneId.toInt()) )
		return SmfErrInvalidRelation;
		
	QString userId, contactUrl, localId, managerUri, presenceState, presenceText, statusText, name;
	QDateTime dateTime;
	Int64 timeStampInSeconds;
	
	QContactGuid guid = aContact->value("Guid").value<QContactGuid>();
	userId = guid.guid();
	
	QContactUrl url = aContact->value("Url").value<QContactUrl>();
	contactUrl = url.url();
	
	QContactId contactId = aContact->value("ContactId").value<QContactId>();
	localId =  contactId.localId() ;
	managerUri = contactId.managerUri();
	
	QContactTimestamp time = aContact->value("Timestamp").value<QContactTimestamp>();
	dateTime = time.created();
	//Returns the datetime as the number of seconds that have passed since 1970-01-01T00:00:00, Coordinated Universal Time (Qt::UTC).
	timeStampInSeconds =  dateTime.toTime_t() ;
		
	QContactPresence presence = aContact->value("Presence").value<QContactPresence>();	
	presenceState = presence.presenceState();
	presenceText = presence.presenceStateText();
	statusText = presence.customMessage();
	
	QString snsName = aProvider->serviceName();
	QString snsUrl = (aProvider->serviceUrl()).toString();
	QString snsDesc = aProvider->description();
		
	qDebug()<<"Inside Test DSM Associate: snsName"<<snsName;
	qDebug()<<"Inside Test DSM Associate: snsUrl"<<snsUrl;
	qDebug()<<"Inside Test DSM Associate: snsDesc"<<snsDesc;
	
	const int contactID = addContactToTable(userId, contactUrl, localId, managerUri, snsName, snsDesc, snsUrl, presenceState, presenceText , statusText, timeStampInSeconds);
	TInt Id = addRelationToTable( contactID,  aPhoneId.toInt()); 
	if(Id == aPhoneId.toInt())
		return  SmfNoError ;
	else
		return SmfDbOpeningError;
}

/** remove contact from a relation */
SmfError DataStoreManager::remove(SmfRelationId aRelation, const SmfContact* aContact){
	
	int contactId, relationId;
	QString userId, contactUrl, localId, managerUri;
	
	//to check validity of relationId
	if(aRelation.size() == 0)
			return SmfErrInvalidRelation;
	relationId = aRelation.toInt();
	
	//to check validity of SmfContact
	if(NULL == aContact)
		return SmfDbContactNotExist;
	
	if(SmfDbOpeningError == openDB())
			return SmfDbOpeningError;

	QSqlQuery qry;
			
	QContactGuid guid = aContact->value("Guid").value<QContactGuid>();
	userId = guid.guid();
	
	if ( ! userId.isNull() )
		{
		QContactUrl url = aContact->value("Url").value<QContactUrl>();
		contactUrl = url.url();				
		qry.prepare("SELECT contactId FROM contact where userId=:userId AND contactUrl=:contactUrl");	
		qry.bindValue(":userId", userId);
		qry.bindValue(":contactUrl", contactUrl);
		}
	else
		{
		//if userId is invalid then it shoudl return SmfError
		return SmfDbContactNotExist;
		
		/*QContactId contactId = aContact->value("ContactId").value<QContactId>();
		localId =  contactId.localId() ;
		managerUri = contactId.managerUri();
		
		qry.prepare("SELECT contactId FROM contact where localId=:localId AND managerUri=:managerUri");	
		qry.bindValue(":localId", localId);
		qry.bindValue(":managerUri", managerUri);*/
		}
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	if(qry.next())
		contactId = qry.value(0).toInt();
		
	deleteContactFromTable(relationId, contactId);
	
	qry.finish();
	db.close();
	return 	SmfNoError;		
		
}

/** returns first relation item in the relation when exists, NULL otherwise */
SmfRelationItem* DataStoreManager::searchById(const SmfRelationId aRelation){

	const int relationId = aRelation.toInt();
	int contactId, contactIndex;
	
	if(0 == aRelation.size())
		return NULL;
	
	if(SmfDbOpeningError == openDB())
			return NULL;

	QSqlQuery qry;
	qry.prepare("SELECT contactId, contactIndex FROM relation where relationId=:relationId LIMIT 1");	
	qry.bindValue(":relationId", relationId);
		
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return NULL; 

	if(qry.next()){
		contactId = qry.value(0).toInt();
		contactIndex = qry.value(1).toInt();
	}
	
	qry.prepare("SELECT * FROM contact where contactId=:contactId");
	qry.bindValue(":contactId", contactId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
			return NULL; 

	if(qry.next())
		createRelationItem(qry, contactIndex);	
		
	qry.finish();
	db.close();
	return iSmsfRelationItem;
	
}

SmfRelationItem* DataStoreManager::getContact(SmfRelationId aRelation, quint32 aIndex){
	
	const int relationId = aRelation.toInt();
	int contactId;
	
	if(SmfDbOpeningError == openDB())
			return NULL;

	QSqlQuery qry;
	qry.prepare("SELECT contactId FROM relation where relationId=:relationId AND contactIndex=:contactIndex");	
	qry.bindValue(":relationId", relationId);
	qry.bindValue(":contactIndex", aIndex);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return NULL; 

	if(qry.next()){
		contactId = qry.value(0).toInt();
	}
	
	qry.prepare("SELECT * FROM contact where contactId=:contactId");
	qry.bindValue(":contactId", contactId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
			return NULL; 

	if(qry.next())
		createRelationItem(qry, aIndex);	
		
	qry.finish();
	db.close();
	return iSmsfRelationItem;	
}

/** returns relation Id for a given contacts if exists, NULL otherwise */
SmfRelationId DataStoreManager::searchByContact( SmfContact aContact){

	int contId,relationId;/* */
	QString userId, contactUrl, localId, managerUri;
	 
	if(SmfDbOpeningError == openDB())
			return QString();

	QSqlQuery qry;
	
	QContactGuid guid = aContact.value("Guid").value<QContactGuid>();
	userId = guid.guid();
	

	if (userId.isNull() )
		{

		QContactId contactId = aContact.value("ContactId").value<QContactId>();
		localId =  contactId.localId() ;
		managerUri = contactId.managerUri();
		
		qry.prepare("SELECT contactId FROM contact where localId=:localId AND managerUri=:managerUri");	
		qry.bindValue(":localId", localId);
		qry.bindValue(":managerUri", managerUri);	
		}
	else
		{
		QContactUrl url = aContact.value("Url").value<QContactUrl>();
		contactUrl = url.url();				
			
		qry.prepare("SELECT contactId FROM contact where userId=:userId ");	/*AND contactUrl=:contactUrl*/
		qry.bindValue(":userId", userId);
		//qry.bindValue(":contactUrl", contactUrl);
	
		}
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return QString(); 

	if(qry.next()){
		contId = qry.value(0).toInt();
	}
	
	qry.prepare("SELECT relationId FROM relation where contactId=:contactId");	
	qry.bindValue(":contactId", contId);	
		
	if(SmfDbQueryExecutonError == executeQuery(qry))
			return QString(); 

	if(qry.next()){
		relationId = qry.value(0).toInt();	
	}
	
	qry.finish();
	db.close();
	return QString::number( relationId );			
	
}

/** contacts and their provider */
DSMContactPckg* DataStoreManager::get(SmfRelationId aRelation, quint32 aIndex){
	
	const int relationId = aRelation.toInt();
	int contactId;
	
	if(SmfDbOpeningError == openDB())
			return NULL;

	QSqlQuery qry;
	qry.prepare("SELECT contactId FROM relation where relationId=:relationId AND contactIndex=:contactIndex");	
	qry.bindValue(":relationId", relationId);
	qry.bindValue(":contactIndex", aIndex);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return NULL; 

	if(qry.next()){
		contactId = qry.value(0).toInt();
	}
	
	qry.prepare("SELECT * FROM contact where contactId=:contactId");
	qry.bindValue(":contactId", contactId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
			return NULL; 

	if(qry.next()){
		iDSMContactPckgItem = new DSMContactPckg;
		
		iDSMContactPckgItem->userId = qry.value(0).toString(); 
		iDSMContactPckgItem->contactUrl = qry.value(1).toString(); 
		iDSMContactPckgItem->localId = qry.value(2).toString(); 
		iDSMContactPckgItem->managerUri = qry.value(3).toString(); 
		iDSMContactPckgItem->snsName = qry.value(4).toString(); 
		iDSMContactPckgItem->snsDesc = qry.value(5).toString();  
		iDSMContactPckgItem->snsUrl = qry.value(6).toString(); 
		iDSMContactPckgItem->presenceState = qry.value(7).toString(); 
		iDSMContactPckgItem->presenceText = qry.value(8).toString(); 
		iDSMContactPckgItem->statusText = qry.value(9).toString(); 
		iDSMContactPckgItem->timeStamp = qry.value(10).toLongLong(); 
		iDSMContactPckgItem->contactIndex = relationId;
		iDSMContactPckgItem->relationId = aIndex;
	}
	
	qry.finish();
	db.close();
	return iDSMContactPckgItem;
}

/** list of contacts and their provider */
QList<SmfRelationItem> DataStoreManager::getAll(SmfRelationId aRelation){
	QList<SmfRelationItem> relationItemList;
	if(aRelation.size() == 0)
		return relationItemList;
	
	const int relationId = aRelation.toInt();
	int contactId, contactIndex;
	
	if(SmfDbOpeningError == openDB())
			return relationItemList;

	QSqlQuery qry;
	qry.prepare("SELECT contactId, contactIndex FROM relation where relationId=:relationId");	
	qry.bindValue(":relationId", relationId);
		
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return relationItemList; 

	while(qry.next()){
		contactId = qry.value(0).toInt();
		contactIndex = qry.value(1).toInt();
		
		QSqlQuery innerqry;
		innerqry.prepare("SELECT * FROM contact where contactId=:contactId");
		innerqry.bindValue(":contactId", contactId);
		
		if(SmfDbQueryExecutonError == executeQuery(innerqry))
				return relationItemList; 

		if(innerqry.next()){
			createRelationItem(innerqry, contactIndex);
			relationItemList << *iSmsfRelationItem;
		}
		innerqry.finish();
	}// end while
	
	qry.finish();
	db.close();
	return relationItemList;	
}

void DataStoreManager::createRelationItem( QSqlQuery & aQry, int aIndex){
			
	SmfContact * smfContact;
	SmfProvider * smfProvider;
	QString snsName, snsDesc;
	QUrl snsUrl;
	
	
	QVariant userId = QVariant::fromValue( aQry.value(1).toString() );
	QVariant contactUrl = QVariant::fromValue(aQry.value(2).toString() ); 
	
	QString StrLocalId =  aQry.value(3).toString() ;
	QString StrManagerUri = aQry.value(4).toString() ;	
	QContactId qContactId;
	qContactId.setLocalId( StrLocalId.toInt() );
	qContactId.setManagerUri( StrManagerUri );	    
	QVariant contactId = QVariant::fromValue(qContactId);	
	
	int presenceState = aQry.value(8).toInt() ; 	
	QString presenceStateText = aQry.value(9).toString() ;
	QString statusText = aQry.value(10).toString();
	QContactPresence qContactPresence;
	qContactPresence.setPresenceState(static_cast<QContactPresence::PresenceState>(presenceState));
	qContactPresence.setPresenceStateText(presenceStateText);
	qContactPresence.setCustomMessage(statusText);
	QVariant contactPresence  = QVariant::fromValue(qContactPresence);
	
	QDateTime dateTime;	
	dateTime.setTime_t ( aQry.value(11).toLongLong() ) ;
	QContactTimestamp qContactTimestamp;
	qContactTimestamp.setCreated(dateTime);
	QVariant timeStamp = QVariant::fromValue( qContactTimestamp );
	
	smfContact = new SmfContact;
	smfContact->setValue("Guid", userId);
	smfContact->setValue("Url", contactUrl);
	smfContact->setValue("ContactId", contactId);
	smfContact->setValue("Presence", contactPresence);	
	smfContact->setValue("Timestamp", timeStamp);
	
	smfProvider = new SmfProvider;
	snsName = aQry.value(5).toString();
	snsDesc = aQry.value(6).toString();
	snsUrl = aQry.value(7).toUrl();
	
	smfProvider->setServiceName( snsName );
	smfProvider->setDescription( snsDesc );
	smfProvider->setServiceUrl( snsUrl );
	
	iSmsfRelationItem = new SmfRelationItem();	
	iSmsfRelationItem->setIndex( aIndex );
	iSmsfRelationItem->setProvider(*(smfProvider) );
	iSmsfRelationItem->setContact(*( smfContact ));	
}
uint DataStoreManager::count(SmfRelationId relation){
	
	const int relationId = relation.toInt();
	return count( relationId );
}

SmfError DataStoreManager::deleteRelation(SmfRelationId relation){
	const int relationId = relation.toInt();
	return deleteRelationFromTable( relationId );
}

QList<SmfRelationId> DataStoreManager::getAllRelations(){
	QList<SmfRelationId> relationList;
					
	if(SmfDbOpeningError == openDB())
		return relationList;

	QSqlQuery qry;
	qry.prepare("SELECT DISTINCT relationId FROM relation");	
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return relationList; 

	while(qry.next()){
		relationList << qry.value(0).toString();
	}

	qry.finish();
	db.close();
	return relationList;			
}

int DataStoreManager::openDB(){
	if (!(db.isOpen())){
		if(!(db.open())){
			state = ERROR;
			lastMsg = db.lastError().text();
			return SmfDbOpeningError;
		}
	}
	return SmfNoError;	
}

int DataStoreManager::executeQuery(QSqlQuery& qry){
	 if(!(qry.exec())){
		lastMsg = qry.lastError().text();
		qry.finish();
		return SmfDbQueryExecutonError;
	}
	return SmfNoError;
}



DataStoreManager::DataStoreManager(const QString& dbName, QObject* parent):QObject(parent){
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
}

bool DataStoreManager::InitializeDataBase(){

    if(!db.open()){
        state = ERROR;
        lastMsg = db.lastError().text();
        return FALSE;
    }
    else
        state = READY;
    
    const int tableCount = 2;

    QSqlQuery smfDsmTables[tableCount];
    
    smfDsmTables[0].prepare("CREATE TABLE IF NOT EXISTS contact (contactId  INTEGER PRIMARY KEY AUTOINCREMENT, userId TEXT, contactUrl TEXT, localId TEXT, managerUri TEXT, snsName TEXT, snsDesc TEXT, snsUrl TEXT, presenceState TEXT, presenceText TEXT, StatusText TEXT, timeStamp INTEGER )");
        
    smfDsmTables[1].prepare("CREATE TABLE IF NOT EXISTS relation (relationId INTEGER,  contactId INTEGER,  contactIndex INTEGER)" );
        


    /*smfDsmTables[0].prepare("CREATE TABLE IF NOT EXISTS contact (contactId INTEGER PRIMARY KEY AUTOINCREMENT,"
																  "contactGuid TEXT,"	 
																  "contactUrl TEXT,"
																  "localId TEXT,"
																  "managerUri TEXT,"
																  "snsName TEXT,"
																  "snsDesc TEXT,"
																  "snsUrl TEXT),"
																  "presenceState INTEGER,"
																  "presenceText TEXT,"
																  "timeStamp INTEGER )");
    
    smfDsmTables[1].prepare("CREATE TABLE IF NOT EXISTS relation (relationId INTEGER,"
																 " contactId INTEGER,"
																 " contactIndex INTEGER)" );
    
*/
    for(int i = 0; i < tableCount; i++){
        if(!smfDsmTables[i].exec()){
            state = ERROR;
            lastMsg = smfDsmTables[i].lastError().text();
            return FALSE;
        }
        smfDsmTables[i].finish();
    }

    db.close();
    state = CLOSED;
    return TRUE;
}


int DataStoreManager::addContactToTable( 	const QString &aLocalId, 
												const QString &aManagerUri,
												const QString &aSnsName,
												const QString &aSnsDesc,
												const QString &aSnsUrl){
	
	int contactId;
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("INSERT INTO contact (userId, contactUrl, localId , managerUri, snsName, snsDesc, snsUrl, presenceState, presenceText, statusText, timeStamp ) VALUES (:userId, :contactUrl, :localId , :managerUri, :snsName, :snsDesc, :snsUrl, :presenceState, :presenceText, :statusText, :timeStamp )");
	qry.bindValue(":userId", NULL);
	qry.bindValue(":contactUrl", NULL);
	qry.bindValue(":localId", aLocalId);
	qry.bindValue(":managerUri", aManagerUri);
	qry.bindValue(":snsName", aSnsName);
	qry.bindValue(":snsDesc", aSnsDesc);
	qry.bindValue(":snsUrl", aSnsUrl);
	qry.bindValue(":presenceState", NULL);
	qry.bindValue(":presenceText", NULL);
	qry.bindValue(":statusText", NULL);
	qry.bindValue(":timeStamp", 0);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	qry.prepare("SELECT contactId FROM contact WHERE localId=:localId AND managerUri=:managerUri");
	qry.bindValue(":localId", aLocalId);
	qry.bindValue(":managerUri", aManagerUri);

	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	if(qry.next()){
		contactId = qry.value(0).toInt();
	}

	qry.finish();
	db.close();
	return contactId;
	
}

int DataStoreManager::addContactToTable(const QString &aUserId, 
									const QString &aContactUrl,
									const QString &aLocalId,
									const QString &aManagerUri,
									const QString &aSnsName,
									const QString &aSnsDesc, 
									const QString &aSnsUrl,
									const QString &aPresenceState,
									const QString &aPresenceText,
									const QString &aStatusText,
									const Int64 aTimeStamp ){
	
	int contactId;
		
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("INSERT INTO contact (userId, contactUrl, localId , managerUri, snsName, snsDesc, snsUrl, PresenceState, PresenceText, statusText, timeStamp ) VALUES (:userId, :contactUrl, :localId , :managerUri, :snsName, :snsDesc, :snsUrl, :PresenceState, :PresenceText, :statusText, :timeStamp )");

	qDebug()<<"DSM :: aUserId :"<<aUserId;
	qDebug()<<"DSM :: aContactUrl :"<<aContactUrl;
	
	
	/*qry.prepare("INSERT INTO contact (userId, contactUrl, localId , managerUri, snsName,"
									" snsDesc, snsUrl, PresenceState, PresenceText, statusText, timeStamp )"
									" VALUES (:userId, :contactUrl, :localId , :managerUri, :snsName,"
									" :snsDesc, :snsUrl, :PresenceState, :PresenceText, statusText, :timeStamp )");*/
	
	qry.bindValue(":userId", aUserId);		qry.bindValue(":contactUrl", aContactUrl);
	qry.bindValue(":localId", aLocalId);				qry.bindValue(":managerUri", aManagerUri);
	qry.bindValue(":snsName", aSnsName);				qry.bindValue(":snsDesc", aSnsDesc);
	qry.bindValue(":snsUrl", aSnsUrl);					qry.bindValue(":PresenceState", aPresenceState);
	qry.bindValue(":PresenceText", aPresenceText);		qry.bindValue(":statusText", aStatusText);
	qry.bindValue(":timeStamp", aTimeStamp);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	qry.prepare("SELECT contactId FROM contact WHERE  (localId=:localId AND managerUri=:managerUri) OR (userId=:userId AND contactUrl=:contactUrl)");

	qry.bindValue(":localId", aLocalId);				qry.bindValue(":managerUri", aManagerUri);
	qry.bindValue(":userId", aUserId);		qry.bindValue(":aContactUrl", aContactUrl);

	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	if(qry.next()){
		contactId = qry.value(0).toInt();
	}

	qry.finish();
	db.close();
	return contactId;	
}

SmfError DataStoreManager::removeContactFromTable(const int aContactId){
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("DELETE FROM contact WHERE contactId=:contactId");
	qry.bindValue(":contactId", aContactId);

	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	qry.finish();
	db.close();

	return SmfNoError;
}



TBool DataStoreManager::contactIfExist( const int aContactId){
	
	TBool contactExist = EFalse;
	if(SmfDbOpeningError == openDB())
		return EFalse;

	QSqlQuery qry;
	TBool queryPep = qry.prepare("SELECT contactId FROM contact WHERE contactId=:contactId");
	if(!queryPep)
		return EFalse;
		
	qry.bindValue(":contactId", aContactId);

	if(SmfDbQueryExecutonError == executeQuery(qry))
		return EFalse; 

	if( qry.first() )
		contactExist = ETrue;
		
	qry.finish();
	db.close();

	return contactExist;
}

TBool DataStoreManager::relationIfExist( const int aRelationId){
	
	TBool contactExist = EFalse;
	if(SmfDbOpeningError == openDB())
		return EFalse;

	QSqlQuery qry;
	qry.prepare("SELECT relationId FROM relation WHERE relationId=:relationId");
	qry.bindValue(":relationId", aRelationId);

	if(SmfDbQueryExecutonError == executeQuery(qry))
		return EFalse; 

	if( qry.first() )
		contactExist = ETrue;
		
	qry.finish();
	db.close();

	return contactExist;
}


int DataStoreManager::findMaxIndexValue(const int aRelationId ){
	
	int contactIndex = 0;
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("SELECT MAX( contactIndex ) FROM (SELECT contactIndex FROM relation WHERE (relationId=:relationId ))");
	qry.bindValue(":relationId", aRelationId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	if(qry.next()){
		contactIndex = qry.value(0).toInt();
	}

	qry.finish();
	db.close();
	return contactIndex;	
}

int DataStoreManager::findMaxRelationId( ){
	
	int maxRelationId = socialProfileBaseID;
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("SELECT MAX( relationId ) FROM relation");
		
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	if(qry.next()){
		maxRelationId = qry.value(0).toInt();
	}
	if(0==maxRelationId)
		maxRelationId = 1000;//socialProfileBaseID;
	
	qry.finish();
	db.close();
	return maxRelationId;	
}

int DataStoreManager::addRelationToTable(const int aContactId, int aRelationId ){
	
	int relationId=0;
	int contactIndex;
	
	//In case of invalid contactId
	if(aContactId <= 0)
		return relationId;
	
	if( aRelationId == ENewRelation ){
			relationId = findMaxRelationId() + 1;
			contactIndex = 0;
		}
	else{
			relationId = aRelationId;
			contactIndex = 1 + ( findMaxIndexValue( aRelationId ) ) ;
			//contactIndex = ( findMaxIndexValue( aRelationId ) ) ;
		}
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("INSERT INTO relation (relationId, contactId, contactIndex ) VALUES (:relationId, :contactId, :contactIndex )");
	qry.bindValue(":relationId", relationId);
	qry.bindValue(":contactId", aContactId);
	qry.bindValue(":contactIndex", contactIndex);
		
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	qry.finish();
	db.close();
	return relationId;	
}

int DataStoreManager::searchContactId(const int aRelationId, const int aIndex){
	
	int contactId = 0; //removed : contactId=SmfDbQueryExecutonError
		
	if(aRelationId <= 0 || aIndex <0)
		return contactId;
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("SELECT contactId FROM relation WHERE (relationId=:relationId AND contactIndex = :contactIndex)");
	
	qry.bindValue(":relationId", aRelationId);
	qry.bindValue(":contactIndex", aIndex);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	if(qry.next()){
		contactId = qry.value(0).toInt();
	}

	qry.finish();
	db.close();
	return contactId;		
}


int DataStoreManager::searchRelationId(const int aContactId){

	int relationId = 0; //SmfDbQueryExecutonError;
	
	if(aContactId == 0)
		return relationId;
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("SELECT relationId FROM relation WHERE ( contactId = :contactId)");	
	qry.bindValue(":contactId", aContactId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	if(qry.next()){
		relationId = qry.value(0).toInt();
	}

	qry.finish();
	db.close();
	return relationId;		
}


QList<int> DataStoreManager::searchContactIdList(const int aRelationId){
	
	QList<int> contactList;
				
	if(SmfDbOpeningError == openDB())
		return contactList;

	QSqlQuery qry;
	qry.prepare("SELECT contactId FROM relation WHERE ( relationId = :relationId)");	
	qry.bindValue(":relationId", aRelationId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return contactList; 

	while(qry.next()){
		contactList << qry.value(0).toInt();
	}

	qry.finish();
	db.close();
	return contactList;		
}


SmfError DataStoreManager::deleteRelationFromTable(const int aRelationId){

	int contactIndex;
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("SELECT contactId FROM relation WHERE relationId = :relationId ");	
	qry.bindValue(":relationId", aRelationId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 
	
	while(qry.next()){
		contactIndex = qry.value(0).toInt();
		QSqlQuery innerQry;
		innerQry.prepare("DELETE FROM contact WHERE  contactId=:contactId ");	
		innerQry.bindValue(":contactId", contactIndex);
		
		if(SmfDbQueryExecutonError == executeQuery(innerQry))
			return SmfDbQueryExecutonError; 
		innerQry.finish();
	}
	
	qry.prepare("DELETE FROM relation WHERE ( relationId = :relationId)");	
	qry.bindValue(":relationId", aRelationId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 

	qry.finish();
	db.close();
	return SmfNoError;		
}

SmfError DataStoreManager::deleteContactFromTable(const int aRelationId, const int aContactId){
	
	int contactIndex = SmfDbContactNotExist;
	
	if(SmfDbOpeningError == openDB())
		return SmfDbOpeningError;

	QSqlQuery qry;
	qry.prepare("SELECT contactIndex FROM relation WHERE ( relationId = :relationId AND contactId=:contactId)");	
	qry.bindValue(":relationId", aRelationId);
	qry.bindValue(":contactId", aContactId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return SmfDbQueryExecutonError; 
	
	if(qry.next()){
		contactIndex = qry.value(0).toInt();
		
		qry.prepare("DELETE FROM relation WHERE ( relationId = :relationId AND contactId=:contactId)");	
		qry.bindValue(":relationId", aRelationId);
		qry.bindValue(":contactId", aContactId);
		
		if(SmfDbQueryExecutonError == executeQuery(qry))
			return SmfDbQueryExecutonError;
		
		qry.prepare("DELETE FROM contact WHERE  contactId=:contactId ");	
		qry.bindValue(":contactId", aContactId);
		
		if(SmfDbQueryExecutonError == executeQuery(qry))
			return SmfDbQueryExecutonError; 
	}
	
	manageContactIndex(aRelationId, contactIndex );
	
	qry.finish();
	db.close();
	return SmfNoError;	
}


void DataStoreManager::manageContactIndex(const int aRelationId, const int aContactIndex ){
	
	QSqlQuery qry;
	qry.prepare("UPDATE relation SET contactIndex = contactIndex - 1 WHERE ( relationId = :relationId AND contactIndex > :contactIndex )");
	if(aRelationId <=0 || aContactIndex <0)
		{
		qDebug()<<"ManageContactIndex: Invalid Argument";
		qry.finish();
		return;
		}
	qry.bindValue(":relationId", aRelationId);
	qry.bindValue(":contactIndex", aContactIndex);	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		qDebug()<<"ManageContactIndex: executeQuery() fails"; 
	qry.finish();		
}

uint DataStoreManager::count(const int aRelationId){
	uint count = 0;
	
	if(SmfDbOpeningError == openDB())
		return count;

	QSqlQuery qry;
	qry.prepare("SELECT count(*) FROM relation WHERE ( relationId = :relationId)");	
	qry.bindValue(":relationId", aRelationId);
	
	if(SmfDbQueryExecutonError == executeQuery(qry))
		return count; 

	if(qry.next()){
		count = qry.value(0).toInt();
	}

	qry.finish();
	db.close();
	return count;			
}
