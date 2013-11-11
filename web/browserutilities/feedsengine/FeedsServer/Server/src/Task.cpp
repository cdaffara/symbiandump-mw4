/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Task base class.
*
*/



#include "Task.h"

// -----------------------------------------------------------------------------
// CTask::CTask
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTask::CTask(CFeedsServer& aFeedsServer):
        iFeedsServer(aFeedsServer)
    {
    }
        

// -----------------------------------------------------------------------------
// CTask::~CTask
// Deconstructor.
// -----------------------------------------------------------------------------
//
CTask::~CTask()
    {
    delete iAutoDelete;
    }


// -----------------------------------------------------------------------------
// CTask::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTask::BaseConstructL(TBool aAutoDelete)
    {
    if (aAutoDelete)
        {        
        iAutoDelete = CIdle::NewL(CActive::EPriorityIdle);
        }
    }        


// -----------------------------------------------------------------------------
// CTask::AutoDelete
//
// Starts an CIdle to delete the instance after the callstack has unrolled.
// -----------------------------------------------------------------------------
//    
void CTask::AutoDelete()
    {
    if (iAutoDelete != NULL)
        {
        iAutoDelete->Start(TCallBack(DelayedDelete, this));
        }
    else
        {
        // TODO: Panic.
        }
    }


// -----------------------------------------------------------------------------
// CTask::DelayedDelete
// 
// Deletes the instance after the callstack has unrolled.
// -----------------------------------------------------------------------------
//
TInt CTask::DelayedDelete(TAny* aPtr)
    {
    CTask*  self = static_cast<CTask*>(aPtr);
    
    delete self;    
    return EFalse;
    }
