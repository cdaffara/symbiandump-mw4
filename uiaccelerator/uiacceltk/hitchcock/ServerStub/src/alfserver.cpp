/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Application definition.
*
*/

#include <alf/alfappui.h>
#include "alfuids.h"

_LIT(KAlfServerName, "alfredserver");


static void RunServerL()
	{
    CActiveScheduler* s=new(ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);

    CAlfAppUi* appUiStub = new (ELeave) CAlfAppUi();
    CleanupStack::PushL(appUiStub);
    appUiStub->ConstructL(); 
 
    RProcess::Rendezvous(KErrNone);
    
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(2);	
	}


TInt E32Main()
	{
	TInt err = User::RenameThread(KAlfServerName);
    if ( err != KErrNone )
        {
        return err;
        }
    
    // create CCleanup
    CTrapCleanup * cleanup = CTrapCleanup::New();
    if (cleanup)
        {
        TRAP(err, RunServerL());
        delete cleanup;
        }

	return err;
	}

