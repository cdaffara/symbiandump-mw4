/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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



#include "senuser.h"

CSenUser::CSenUser(TBool aActive)
    : iActive(aActive)
{
}


CSenUser::~CSenUser()
{
    iBaseIdentities.ResetAndDestroy();
}

CSenUser* CSenUser::NewLC(const TDesC8& aUserName, const TDesC8& aPIN, TBool aActive)
{
	CSenUser* self = new (ELeave)CSenUser(aActive);
	CleanupStack::PushL(self);
	self->ConstructL(aUserName, aPIN);
	return self;
}

CSenUser* CSenUser::NewL(const TDesC8& aUserName, const TDesC8& aPIN, TBool aActive)
{
	CSenUser* self=CSenUser::NewLC(aUserName, aPIN, aActive);
	CleanupStack::Pop(); // self;
	return self;
}

void CSenUser::ConstructL(const TDesC8& aUserName, const TDesC8& aPIN)
{
    iUserName.Close();
    iUserName.CreateL(aUserName);
    iPIN.Close();
    iPIN.CreateL(aPIN);
}
