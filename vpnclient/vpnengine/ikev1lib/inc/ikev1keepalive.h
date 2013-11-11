/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKEv1 keep alive object
*
*/


#ifndef C_IKEV1KEEPALIVE_H
#define C_IKEV1KEEPALIVE_H

#include <in_sock.h> // TInetAddr

class CIkev1PluginSession;
class CEchoSender;

/** 
* IKE keepalive event handler
* @internalComponent
*/
class MDpdHeartBeatEventHandler
{
	public:
		/** 
         * IKE PKI service operation completed 
         * @internalComponent
         * @param aStatus completion status of operation
         * @param aObject pointer to CIkePkiService object
         * 
         */
		virtual void EventHandlerL()=0; 
};


NONSHARABLE_CLASS(CIkeV1KeepAlive) : public CTimer
{
	public:
		static CIkeV1KeepAlive* NewL( CIkev1PluginSession& aPluginSession,
		                              TInt aPort,
		                              TInetAddr& aDestAddr, 
		                              TInt NatKeepAlive,
		                              TInt DpdKeepAlive,
		                              MDpdHeartBeatEventHandler* aHandler,
		                              TUint8 aDscp = 0 );
		~CIkeV1KeepAlive();

	protected:
	//
	// CActive methods
	//
		void DoCancel();
		void RunL();
		TInt RunError(TInt aError);

	private: // implementation
	    CIkeV1KeepAlive( CIkev1PluginSession& aPluginSession,
	                     TInt aPort,
	                     TInetAddr& aDestAddr, 
	                     TInt NatKeepAlive,
	                     TInt DpdKeepAlive, 
	                     MDpdHeartBeatEventHandler* aHandler,
	                     TUint8 aDscp );
		void ConstructL();		
		void StartTimer();

	private: // data
		MDpdHeartBeatEventHandler* iCallback;
		CIkev1PluginSession&       iPluginSession;
		
		TInt                       iPort;

	    /** Data to send in keepalive packet */
	    TBuf8<1>                   iMsg;		

		TInt                       iNatKeepAlive;
		TInt                       iDpdKeepAlive;

		TInt                       iCurrDPDTimeLeft;
		TInt                       iRemainingTime;

		TInetAddr                  iDestAddr; 
		
		TUint8                     iDscp;
};

#endif // C_IKEV1KEEPALIVE_H
