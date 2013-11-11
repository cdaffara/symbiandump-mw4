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

void CHttpClientFlow::ActiveOpen ( const TDesC8& /*aConnectionData*/ )
	{
	}

TInt CHttpClientFlow::PassiveOpen ( TUint /*aQueueSize*/ )
	{
	return KErrNone;
	}

TInt CHttpClientFlow::PassiveOpen ( TUint /*aQueueSize*/, const TDesC8& /*aConnectionData*/ )
	{
	return KErrNone;
	}

void CHttpClientFlow::AutoBind ( )
	{
	}

void CHttpClientFlow::LocalName ( TSockAddr& /*anAddr*/ ) const
	{
	}

TInt CHttpClientFlow::SetLocalName ( TSockAddr& /*anAddr*/ )
	{
	return KErrNone;
	}

void CHttpClientFlow::RemName ( TSockAddr& /*anAddr*/ ) const
	{
	}

TInt CHttpClientFlow::SecurityCheck ( MProvdSecurityChecker* /*aSecurityChecker */ )
	{
	return KErrNone;
	}

void CHttpClientFlow::Ioctl ( TUint /*aLevel*/, TUint /*aName*/, TDes8* /*anOption*/ )
	{
	}

void CHttpClientFlow::CancelIoctl ( TUint /*aLevel*/, TUint /*aName*/ )
	{
	}

void CHttpClientFlow::Start ( )
	{
	// Ideally, start should Stack like TCP...
	}

void CHttpClientFlow::Shutdown ( MSessionControl::TCloseType /*option*/, const TDesC8& /*aDisconnectionData*/ )
	{
	}

// MSessionData
TUint CHttpClientFlow::Write ( const TDesC8& /*aDesc*/, TUint /*aOptions*/, TSockAddr* /*anAddr*/ )
	{
	return KErrNone;
	}

void CHttpClientFlow::GetData ( TDes8& /*aDesc*/, TUint /*aOptions*/, TSockAddr* /*anAddr*/ )
	{
	}

