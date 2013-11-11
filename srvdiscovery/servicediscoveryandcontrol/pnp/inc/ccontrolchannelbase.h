// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#ifndef __CCONTROLCHANNELBASE_H_
#define __CCONTROLCHANNELBASE_H_

#include <e32base.h>

class TControlMessage;
/**
 This class acts as a base class which technology specific control channel need to implement
 @publishedPartner
 @prototype
 */
NONSHARABLE_CLASS (CControlChannelBase) : public CBase
	{
protected:
	CControlChannelBase();
public:
	~CControlChannelBase();
public:
	/*
      Recv is used for receiving the data through control channel.
	  @param aMessage msg wrapped as TControlMessage 
      @param aStatus request status
     */  
	virtual void Recv ( TControlMessage& aMessage, TRequestStatus& aStatus ) = 0;
	 /*
      Send is used for sending the data through control channel.
	  @param aMessage msg wrapped as TControlMessage 
      @param aStatus request status
     */  
	virtual void Send ( TControlMessage& aMessage, TRequestStatus& aStatus ) = 0;
      /*
       Cancels the asynchronous recv used for receiving the data through control channel.
	  */  
	virtual void CancelRecv () = 0;
	  /*
       Cancels the asynchronous send used for sending the data through control channel.
	  */  
	virtual void CancelSend () = 0;

	};
	
inline CControlChannelBase::CControlChannelBase()	
	{
	
	}
	
inline CControlChannelBase::~CControlChannelBase()
	{
	
	}	

#endif //__CCONTROLCHANNELBASE_H__
