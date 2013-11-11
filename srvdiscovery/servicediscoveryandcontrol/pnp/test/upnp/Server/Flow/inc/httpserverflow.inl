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

void CHttpServerFlow::ActiveOpen(const TDesC8& /*aConnectionData*/)
	{
	}


TInt CHttpServerFlow::PassiveOpen(TUint /*aQueueSize*/)
	{
	return 0;
	}

TInt CHttpServerFlow::PassiveOpen(TUint /*aQueueSize*/,const TDesC8& /*aConnectionData*/)
	{
	return 0;
	}

void CHttpServerFlow::AutoBind()
	{
	}

void CHttpServerFlow::LocalName(TSockAddr& /*anAddr*/) const
	{
	}

TInt CHttpServerFlow::SetLocalName(TSockAddr& /*anAddr*/)
	{
	return 0;
	}

void CHttpServerFlow::RemName(TSockAddr& /*anAddr*/) const
	{
	}

TInt CHttpServerFlow::SetRemName(TSockAddr& /*anAddr*/)
	{
	return 0;
	}

TInt CHttpServerFlow::SecurityCheck(MProvdSecurityChecker* /*aSecurityChecker*/)
	{
	return 0;
	}

void CHttpServerFlow::Ioctl(TUint /*level*/, TUint /*name*/, TDes8* /*anOption*/)
	{
	}

void CHttpServerFlow::CancelIoctl(TUint /*aLevel*/, TUint /*aName*/)
	{
	}

TUint CHttpServerFlow::Write ( const TDesC8& /*aDesc*/,TUint /*aOptions*/,TSockAddr* /*anAddr*/ )
	{
	ASSERT(0);
	return KErrNone;
	}

void CHttpServerFlow::GetData ( TDes8& /*aDesc*/, TUint /*aOptions*/, TSockAddr* /*anAddr*/ )
	{
	ASSERT(0);
	}

void CHttpServerFlow::Start()
	{
	}

void CHttpServerFlow::Shutdown(MSessionControl::TCloseType /*option*/, const TDesC8& /*aDisconnectionData*/)
	{
	}

TBool CHttpServerFlow::IsValidCharset ( const CRequest& /*aRequest*/ )
	{
	THTTPHdrVal hdrVal;
//	if ( GetParamValue ( aRequest, HTTP::EContentType, HTTP::ECharset, hdrVal ) )
//		{
//		if ( hdrVal.StrF ().DesC ().Compare ( KCharset() ) == 0 )
			{
			return ETrue;
			}
//		}
//	return EFalse;
	}



