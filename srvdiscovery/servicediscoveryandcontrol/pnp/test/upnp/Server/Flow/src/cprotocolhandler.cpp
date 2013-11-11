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
// @file
// @internalComponent
// 
//

#include "cprotocolhandler.h"


CProtocolHandler::CProtocolHandler ( )
	:iSocketHandler ( *this )
	{
	}
CProtocolHandler::CProtocolHandler ( RInternalSocket& aSocket)
: iSocketHandler ( *this ),
iSocket ( aSocket )
	{
		
	}	
CProtocolHandler::~CProtocolHandler ( )
	{
	iTransactions.Reset ( );
	// Cancel all socket operations
	iSocketHandler.CancelAll( );
	iSocket.Close( );
	iTimer->StopTimer ();
	delete iTimer;
	}

void CProtocolHandler::ConstructL () 
	{
	iTimer = CUPnPTimer::NewL ( *this );
	}
	
TInt CProtocolHandler::FindTransaction ( CTransaction* aTransaction ) const
	{
    return iTransactions.Find ( aTransaction );
    }
    
TInt CProtocolHandler::FindTransaction ( const CRequest* aRequest ) const
	{
	for ( TInt i = 0; i < iTransactions.Count ( ); i++ )
		{
		if ( iTransactions[i]->Request ( ) == aRequest )
			{
			return i;
			}
		}
	return KErrNotFound;
	}



