/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef ATESTSOCKET_H_
#define ATESTSOCKET_H_

NONSHARABLE_CLASS(ATestSocket):public MSessionControlNotify, 
								public MSessionDataNotify
	{
	protected:
		//MSessionDataNotify
		virtual void NewData(TUint /*aCount*/)
		{};
		virtual void CanSend()
		{};
		//MSessionControlNotify
		virtual void ConnectComplete()
		{};
		virtual void ConnectComplete(const TDesC8& /*aConnectData*/)
		{};
		virtual void ConnectComplete(CSubConnectionFlowBase& /*aSSP*/)
		{};
		virtual void ConnectComplete(CSubConnectionFlowBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
		{};
		virtual void CanClose(MSessionControlNotify::TDelete /*aDelete=MSessionControlNotify::EDelete*/)
		{};
		virtual void CanClose(const TDesC8& /*aDisconnectData*/,MSessionControlNotify::TDelete /*aDelete=MSessionControlNotify::EDelete*/)
		{};
		virtual TInt Error(TInt /*anError*/,TUint /*anOperationMask=MSessionControlNotify::EErrorAllOperations*/)
		{return KErrNone;};
		virtual void Disconnect(void)
		{};
		virtual void Disconnect(TDesC8& /*aDisconnectData*/)
		{};
		virtual void IoctlComplete(TDesC8* /*aBuf*/)
		{};
		virtual void DisconnectFromListener(CSubConnectionFlowBase& /*aSSP*/)
		{};
		 
		
	};


#endif /*ATESTSOCKET_H_*/
