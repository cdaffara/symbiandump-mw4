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

	
 /*	 DataStoreManager table description
	 --------------------------------------------
	
	 contact (contactId INTEGER PRIMARY KEY AUTOINCREMENT,
			  userId TEXT,	 
			  contactUrl TEXT,
			  localId TEXT,
			  managerUri TEXT,
			  snsName TEXT,
			  snsDesc TEXT,
			  snsUrl TEXT),
			  presenceState INTEGER,
			  presenceText TEXT,
			  StatusText TEXT,
			  timeStamp INTEGER");			
			  
--------------------------------------------********-------------------------------------------------	
		relation table constitue of relationId, contactId and contactIndex fields.
		
		relation (relationId INTEGER ,
			   	  contactId INTEGER,
			   	  contactIndex INTEGER);	
		
		if more than one contactId exist for a relation,
		contactIndex will be assigned in incremental order to the contactId's (0, 1, 2, 3........).
	
	Eg.			
		Row	|	relationId | contactId | contactIndex
		-----------------------------------------------
		R1	|	1000	   |  1234     |    0
		R2	|	1000       |  1235     |    1
		R3	|	1001       |  4567     |    0
		R4	|	1000       |  1236     |    2
				
				
		On deletion Row2: Deleting Relation Contact
		Row	|	relationId | contactId | contactIndex
		-----------------------------------------------
		R1	|	1000	   |  1234     |    0		
		R2	|	1001       |  4567     |    0
		R3	|	1000       |  1236     |    1
				
			  
*/

#ifndef DATASTOREMANAGER_H
#define DATASTOREMANAGER_H

#define socialProfileBaseID 1000

#include <QString>
#include <QObject>
#include <QtSql>

#include "smfprovider.h"
#include "smfcontact.h"
#include "smfrelationmgr.h"
#include "smfglobal.h"

enum DataStoreManagerState{READY, BUSY, CLOSED, ERROR};

struct DSMContactPckg
{
public:
	 QString userId; 
	 QString contactUrl;
	 QString localId;
	 QString managerUri;
	 QString snsName;
	 QString snsDesc; 
	 QString snsUrl;
	 QString presenceState;
	 QString presenceText;
	 QString statusText;
	 Int64 timeStamp;
	 int contactIndex;
	 int relationId;
};

class DataStoreManager : public QObject
{
	Q_OBJECT
	
	public:
	
		static DataStoreManager* getDataStoreManager();
		~DataStoreManager();			
		DataStoreManagerState getState() const;
		QString getErrorText() const;
    
	public slots:
        SmfRelationId create(SmfProvider *aProvider=NULL, SmfContact *aContact=NULL);

		/** assign contact to a relation */
		SmfError associate( SmfRelationId aRelation,
							const SmfContact* aContact,
							SmfProvider* aProvider);				

		/** remove contact from a relation */
		SmfError remove(SmfRelationId aRelation,
						const SmfContact* aConact);			

		/** returns first relation item in the relation when exists, NULL otherwise */
		SmfRelationItem* searchById(const SmfRelationId aRelation);

		/** returns relation Id for a given contacts if exists, NULL otherwise */
		SmfRelationId searchByContact( SmfContact aContact);		

		/** list of contacts and their provider */
		QList<SmfRelationItem> getAll(SmfRelationId aRelation);
		
		/**contacts and their provider */
		SmfRelationItem* getContact(SmfRelationId aRelation, quint32 aIndex);

		/** delete a particular relation*/
		SmfError deleteRelation(SmfRelationId aRelation);
		
		/** list of all relations */
		QList<SmfRelationId> getAllRelations();
		
		/** returns number of contacts in a relation*/
		uint count(SmfRelationId aRelation);
		
		/** get DSMContactPckg */
		DSMContactPckg* get(SmfRelationId aRelation, quint32 aIndex);
      
 			
	private:
		int openDB();
		int executeQuery(QSqlQuery&);
		DataStoreManager(const QString& dbName, QObject* parent = 0); 
		bool InitializeDataBase();	// Initialization code to be called once while lazy construction of the instance
		        
	private:
        /*create and initialize a SmfRelationItem*/
		void createRelationItem(QSqlQuery & aQry, int aIndex);
		
		enum{
			ENewRelation = -1
		};
        /*------------------Operation on Contact Table-------------------*/
        /**insert a row in the contact table */        
        int addContactToTable( const QString &localId, 
									const QString &managerUri,
									const QString &snsName,
									const QString &snsDesc,
									const QString &snsUrl);
        
        int addContactToTable( const QString &userId, 
									const QString &contactUrl,
									const QString &localId,
									const QString &managerUri,
									const QString &snsName,
									const QString &snsDesc, 
									const QString &snsUrl,
									const QString &presenceState,
									const QString &presenceText,
									const QString &statusText,
									const Int64 timeStamp	
									);

  
        
        /**delete a row in the contact table */
        SmfError removeContactFromTable(const int aContactId);
        
        /**search for relation item ( contact ) */
        SmfRelationItem* searchContact(const int aContactId);
        
        /**search for contact if exist, return True/False*/
        TBool contactIfExist( const int aContactId);
        
        
        /*------------------Operation on Relation Table-------------------*/ 
        /**insert a row in relation table and return relationId */
        int addRelationToTable(const int aContactId, int aRelationId = ENewRelation);
        
        /**delete a relation table*/
        SmfError deleteRelationFromTable(const int aRelationId);
        
        /**delete a row in relation table*/
		SmfError deleteContactFromTable(const int aRelationId, const int aContactId);	
        
        /**search for list of contactId */
        QList<int> searchContactIdList(const int aRelationId);
        
        /**search for a contactId */
        int searchContactId(const int aRelationId, const int aIndex);
        
        /**search for relation id*/
        int searchRelationId(const int aContactId);
        
        /**search for a reation contact having highest index value*/
        int findMaxIndexValue(const int aRelationId );
        
        /**search for a reationId highest value*/
        int findMaxRelationId();
        
        /**On deletion of a contact it decrease index of all contacts 
         * having index greater than it in that relation.
         * */
        void manageContactIndex(const int aRelationId, const int aContactIndex );
        
        /** returns number of contacts in a relation*/
        uint count(const int aRelationId);
                
        /**search for relation if exist, return True/False*/
        TBool relationIfExist( const int aRelationId);
                
	private:
               
        static DataStoreManager* dsmInstance;		// Unique Instance for DSM (Singleton Implementation)
        static const QString dbName;
        int dsmInstanceCount; 						// Track references to the DSM
        static DataStoreManagerState state;			// Current state of DSM
        QString lastMsg;							// Last message/error generated by the database.
        QSqlDatabase db;
        SmfRelationItem * iSmsfRelationItem;
        DSMContactPckg	*iDSMContactPckgItem;
       
};

#endif
