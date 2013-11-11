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
*       Methods for CConnManKeyAbsorber class. Absorbs all the key presses.
*       
*
*/


#include <eikenv.h>
#include <eikappui.h>
#include "ConnManKeyAbsorber.h"

CConnManKeyAbsorber::CConnManKeyAbsorber()
	{
	}

CConnManKeyAbsorber::~CConnManKeyAbsorber()
	{
	if (iCoeEnv && iAppUi)
	iAppUi->RemoveFromStack(this);
	}

CConnManKeyAbsorber* CConnManKeyAbsorber::NewLC()
	{
	CConnManKeyAbsorber* self=new(ELeave) CConnManKeyAbsorber();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


void CConnManKeyAbsorber::ConstructL()
	{
	CreateWindowL();
	SetExtent( TPoint( 0, 0 ), TSize( 0, 0 ) );
	ActivateL();
	SetPointerCapture( ETrue );
	ClaimPointerGrab( ETrue );
	iAppUi=iEikonEnv->EikAppUi();
	}

TKeyResponse CConnManKeyAbsorber::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
	{
	return EKeyWasConsumed;
	}


void CConnManKeyAbsorber::AddToStackL()
	{
	if ( !iAddedToStack )
		{
		iAppUi->AddToStackL( this, ECoeStackPriorityCba );
		iAddedToStack = ETrue;
		}
	}


void CConnManKeyAbsorber::RemoveFromStack()
	{
	if ( iAddedToStack )
		{
		iAppUi->RemoveFromStack( this );
		}
	iAddedToStack = EFalse;
	}


void CConnManKeyAbsorber::CleanupOnLeave( TAny* aSelf )
	{
	( static_cast< CConnManKeyAbsorber* >( aSelf ) )->RemoveFromStack( );
	}
// End of File
