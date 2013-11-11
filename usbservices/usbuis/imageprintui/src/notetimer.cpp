/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles situation after timer expired
*
*/


#include "notetimer.h"
#include "imageprintuiappui.h"
#include "imageprintuidebug.h"

CNoteTimer* CNoteTimer::NewL( CImagePrintUiAppUi* aAppUi)
    {
    CNoteTimer* self = CNoteTimer::NewLC( aAppUi);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//		
CNoteTimer* CNoteTimer::NewLC( CImagePrintUiAppUi* aAppUi)
    {
    FLOG(_L("[IMAGEPRINTUI] CNoteTimer: NewLC"));
    CNoteTimer* self = new (ELeave) CNoteTimer();
    CleanupStack::PushL(self);
    self->ConstructL(aAppUi);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
CNoteTimer::CNoteTimer()
    : CTimer(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
CNoteTimer::~CNoteTimer()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CNoteTimer::Destructor start")); 
    Cancel();
    FLOG(_L("[IMAGEPRINTUI]\t CNoteTimer::Destructor end"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//		
void CNoteTimer::ConstructL(CImagePrintUiAppUi* aAppUi)
    {
    iAppUi = aAppUi;
    CTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CNoteTimer::RunL()
    {
    FLOG(_L("[IMAGEPRINTUI] CNoteTimer : RunL"));
    iAppUi->HandleTimeOutL();
    }

// -----------------------------------------------------------------------------
// CNoteTimer::RunError
// Standard active object error function.
// -----------------------------------------------------------------------------
//

TInt CNoteTimer::RunError(TInt aError)
    {
    FTRACE(FPrint(_L("[IMAGEPRINTUI] CNoteTimer::RunError: err %d from RunL"), aError));
    
    TRAPD(err,iAppUi->HandleTimeErrorL(aError));
    if(err)
    	{
    	FLOG(_L("[IMAGEPRINTUI] CNoteTimer ::  err when call HandleTimeError"));
    	}
    
    return KErrNone;
    }
