/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     This class provides connection/session related information.
*	  Both the requested and the stored AP's are here.
*
*
*/


#ifndef ConnMan_Active_Connector_H
#define ConnMan_Active_Connection_H

#include <e32base.h>
#include <nifman.h>


class TCommDbConnPref;
class TConnSnapPref;

const TInt KStageGoingBackwardsError = 10602;
//This class plays the role of a high level state machine in the connection creation
NONSHARABLE_CLASS(  CConnManActiveConnector ): public CActive
	{
public:	

	/**
	*Constructor
	*@param aAgent an RGenerericAgent
	*@param aPriority the priority of the active object
	*/
	CConnManActiveConnector( RConnection& aConnection, TInt aPriority = EPriorityStandard );

	/**
	*Destructor
	*/
	virtual ~CConnManActiveConnector();
	
	/**
	*This will activate the active object and start the connection creation process
	*It will complete on error or when the connection has been created
	*An important precondition: the agent that was passed must be opened before calling this function
	*and must not be closed before it completes
	*@param aSettings the overridesettings containing the connection characteristics
	*@param aStatus the status of the observer Active object
	*/
    void StartConnection( TCommDbConnPref* aSettings, TRequestStatus& aStatus );
    
    /**
	*This will activate the active object and start the connection creation process
	*It will complete on error or when the connection has been created
	*An important precondition: the agent that was passed must be opened before calling this function
	*and must not be closed before it completes
	*@param aSettings the overridesettings containing the connection characteristics
	*@param aStatus the status of the observer Active object
	*/
    void StartConnection( TConnSnapPref* aSettings, TRequestStatus& aStatus);

private://from CActive	

	/**
	*This method is implemented to enable proper cancellation of the active obect
	*/
	void DoCancel();			
	
	/**
	*RunL contains the decisions in different stages of the connection progress
	*/	
	void RunL();

private:

	TRequestStatus* iExternalRequestStatus;
    RConnection& iConnection;
	};


//This is a wrapper class on top of CConnManActiveConnector enabling asynch->synch conversion
NONSHARABLE_CLASS(  CActiveConnectorSyncWrapper ): public CActive
	{
public:
	/**
	*Symbian OS constructor 
	*@param aAgent a RGenerericAgent
	*@param aPriority the priority of the active object
	*/
    static CActiveConnectorSyncWrapper * NewL( RConnection& aConnection, TInt aPriority = EPriorityStandard );
	static CActiveConnectorSyncWrapper * NewL( TInt aPriority = EPriorityStandard );

	/**
	*Symbian OS constructor 
	*@param aAgent a RGenerericAgent
	*@param aPriority the priority of the active object
	*/
//	static CActiveConnectorSyncWrapper * NewLC( RGenericAgent& aAgent, TInt aPriority = EPriorityStandard );    CHANGED
    static CActiveConnectorSyncWrapper * NewLC( RConnection& aConnection, TInt aPriority = EPriorityStandard );
	
public:	

	/**
	*This will activate the active object and start the connection creation process
	*The it will return on error or when the connection has been created
	*An important precondition: the agent that was passed must be opened before calling this function
	*and must not be closed before it completes
	*@param aSettings the overridesettings containing the connection characteristics
	*@return the error code
	*/
    TInt Connect( TCommDbConnPref* aSettings );
    
    /**
	*This will activate the active object and start the connection creation process
	*The it will return on error or when the connection has been created
	*An important precondition: the agent that was passed must be opened before calling this function
	*and must not be closed before it completes
	*@param aSettings the overridesettings containing the connection characteristics
	*@return the error code
	*/    
    TInt ConnectSnap( TConnSnapPref* aSettings );

public:
	
    /**
	*Destructor
	*/
	virtual ~CActiveConnectorSyncWrapper();

private://from CActive

    /**
	*This function will release the active scheduler loop
	*/
	void RunL();
	/**
	*This function too releases the active scheduler loop
	*/
	void DoCancel();

protected:

	/**
	*Symbian OS second phase constructor
	*@param aAgent a RGenericAgent handle
	*/
	void ConstructL( RConnection& aConnection );

	/**
	*constructor
	*@param aPiority the priority of the active object
	*/
	CActiveConnectorSyncWrapper( TInt aPriority );

private:

	CActiveSchedulerWait iWait;
	CConnManActiveConnector* iActiveConnector;
	};

#endif// End of File
