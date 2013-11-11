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


#ifndef Connection_Observers_H
#define Connection_Observers_H

#warning The Connection Manager API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information


//System includes
#include <es_sock.h>
#include <es_enum.h> 
#include <cdbcols.h>

//You should inherit from this if you want to observe connection stages
class MConnectionStageObserver
	{
	public:

		virtual void ConnectionStageAchievedL() = 0;
	};


//You should inherit from this if you want to observe multiple connection stages
class MConnectionMultiStageObserver
	{
	public:

		virtual void ConnectionStageAchievedL( TInt aStage ) = 0;
	};

//This is the notifier class which listens to connection stage changes
NONSHARABLE_CLASS(  CConnectionStageNotifierWCB ) : public CActive
	{
public:

	/**
	*Symbian OS 2 phased constructor
	*@param aPriority the active objects priority
	*/
	IMPORT_C static CConnectionStageNotifierWCB* NewL( TInt aPriority = EPriorityStandard);
	
	/**
	*Symbian OS 2 phased constructor
	*@param aPriority the active objects priority
	*/
	IMPORT_C static CConnectionStageNotifierWCB* NewLC( TInt aPriority = EPriorityStandard );

public:

	/**
    *
    *DEPRECATED since Averell2.0
    *
	*Starts the notification service
	*after it has been started you can expect a notification
	*@param aStageToObserve the stage you want to notified about
	*@param aObserver the observer you want to register for notification
	*@param aCompleteIfAlreadyAtStage if ETrue then you get a notification even 
	* if the connection is in the stage you defined when you start the service
	*/
	IMPORT_C void StartNotificationL( TInt aStageToObserve, MConnectionStageObserver* aObserver, TBool aCompleteIfAlreadyAtStage= ETrue  );

public:

	/**
	*Destructor
	*/
	IMPORT_C virtual ~CConnectionStageNotifierWCB();

protected:

	/**
	*Constructor
	*@param aPriority the priority of the active object
	*/
	CConnectionStageNotifierWCB( TInt aPriority );

private://from CActive		

        void DoCancel();		
		void RunL();	

public:

    /**
	*Starts the notification service
	*after it has been started you can expect a notification
    *@param aConnName connection name returned by RConnection::Name()
	*@param aStageToObserve the stage you want to notified about
	*@param aObserver the observer you want to register for notification
	*@param aCompleteIfAlreadyAtStage if ETrue then you get a notification even 
	* if the connection is in the stage you defined when you start the service
	*/
	IMPORT_C void StartNotificationL( TName* aConnName, 
                                      TInt aStageToObserve, 
                                      MConnectionStageObserver* aObserver, 
                                      TBool aCompleteIfAlreadyAtStage= ETrue  );

    /**
	*Starts the notification service
	*after it has been started you can expect a notification
    *@param aConnName connection name returned by RConnection::Name()
	*@param aStagesToObserve array that contains the stages you want to notified about
	*@param aObserver the observer you want to register for notification
	*@param aCompleteIfAlreadyAtStage if ETrue then you get a notification even 
	* if the connection is in the stage you defined when you start the service
	*/
    IMPORT_C void StartNotificationL( TName* aConnName, 
                                      TInt* aStages, 
                                      TInt aNumOfStages,
                                      MConnectionMultiStageObserver* aObserver,
                                      TBool aCompleteIfAlreadyAtStage = ETrue );

protected:

    /**
    *Attach to connection, the name of which is the aConnName.
    *@param aConnName connection name returned by RConnection::Name()
    */
    void DoOpenAgentL( TName* aConnName );

    /**
    *Detach from connection.
    */
    void DoCloseAgent();

    /**
    *Checks if any of the stage passed in StartNotificationL() is reached.
    *Doesn't stop for waiting for another state.
    */
    TBool IsAnyStageReached( TInt aCurrentStage );

private:

    RSocketServ     iServer;
    RConnection     iConnection;
	TNifProgressBuf iProgressBuf;		
	TBool iCompleteIfAlreadyAtStage;	
	TInt iStageToObserve;
	MConnectionStageObserver* iObserver;
    MConnectionMultiStageObserver* iMultiObserver;
    TInt*   iStages;
    TInt    iNumOfStages;
	};






#endif
// End of File
