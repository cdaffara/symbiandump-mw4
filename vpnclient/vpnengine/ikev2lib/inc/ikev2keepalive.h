/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common keep alive object
*
*/

#ifndef _IKEV2KEEPALIVE_H_
#define _IKEV2KEEPALIVE_H_


#include <e32base.h>
#include <in_sock.h>

class CIkev2PluginSession;

/** 
* IKE keepalive event handler
* @internalComponent
*/
class MIkeV2DpdHeartBeatEventHandler
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


NONSHARABLE_CLASS(CIkeV2KeepAlive) : public CTimer
{
	public:
		static CIkeV2KeepAlive* NewL(TInt DpdKeepAlive,			                      
			                         MIkeV2DpdHeartBeatEventHandler& aHandler);
		~CIkeV2KeepAlive();

	protected:
	//
	// CActive methods
	//
		void DoCancel();
		void RunL();

	private: // implementation
		CIkeV2KeepAlive(TInt DpdKeepAlive, 
                        MIkeV2DpdHeartBeatEventHandler& aHandler);
		void ConstructL();		
		void StartTimer();

	private: // data
		MIkeV2DpdHeartBeatEventHandler& iCallback;

		TInt                       iDpdKeepAlive;	
		TInt                       iRemainingTime;
};

#endif //_IKEV2KEEPALIVE_H_
