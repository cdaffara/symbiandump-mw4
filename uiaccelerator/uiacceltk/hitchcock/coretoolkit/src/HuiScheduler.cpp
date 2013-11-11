/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include "uiacceltk/HuiScheduler.h"  // Class definition
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiCommand.h"
#include "uiacceltk/HuiEvent.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiUtil.h"

#include "uiacceltk/HuiPanic.h"


CHuiScheduler::CHuiScheduler(CHuiEnv& aEnv)
        : iEnv(aEnv)
    {
    }

CHuiScheduler::~CHuiScheduler()
    {
    for(TInt i = iPendingCommands.Count() - 1; i >= 0; --i)
        {
        RemovePendingCommand(i);
        }
    iPendingCommands.Close();
    }
    
    
void CHuiScheduler::RemovePendingCommand(TInt aIndex)
    {
    delete iPendingCommands[aIndex].iCommand;
    iPendingCommands[aIndex].iCommand = NULL;
    iPendingCommands.Remove(aIndex);
    }


void CHuiScheduler::AdvanceTime(TReal32 /*aElapsedTime*/)
    {
    TTime now = CHuiStatic::Time();

    // Check the pending commands. They are iterated in chronological order
    // so that commands with the same execution time will be executed in
    // the same order as they were posted.    

    /** @todo  Could be done more efficiently. Priority queue? */
    
    for(TInt i = 0; i < iPendingCommands.Count(); ++i)
        {
        if(iPendingCommands[i].iExecutionTime <= now)
            {
            // The execution of the command may modify the list of pending
            // commands. Take the command out of the pending array first.
            const THuiCommand* command = iPendingCommands[i].iCommand;
            iPendingCommands.Remove(i);

            // Must trap because the command is not on the cleanup stack.
            TRAPD(err, command->ExecuteL(iEnv));
            if(err != KErrNone)
                {
                HUI_DEBUG1(_L("CHuiScheduler::AdvanceTime() - Leave %i during command execution."), err);
                }

            delete command; 
            command = NULL;

            // Check through the list of pending again, in case there
            // were alterations.
            i = -1;
            }
        }
    }


void CHuiScheduler::PostCommandL(const THuiCommand& aCommand,
                                 TInt aTransitionTime)
    {
    if(aTransitionTime > 0)
        {
        // Make a copy of the command.
        const THuiCommand* data = (const THuiCommand*) new (ELeave) TUint8[aCommand.Size()];
        CleanupStack::PushL((TAny*)data);
        Mem::Copy((TAny*)data, (TAny*)&aCommand, aCommand.Size());
        
        // Won't be executed yet.
        TDelayedCommand delayed(data);
        delayed.iExecutionTime = CHuiStatic::Time() +
            TTimeIntervalMicroSeconds32(1000 * aTransitionTime);
        User::LeaveIfError( iPendingCommands.Append(delayed) ); 
        CleanupStack::Pop((TAny*)data);
        }
    else
        {
        // Execute immediately.
        aCommand.ExecuteL(iEnv);
        }
    }


void CHuiScheduler::CancelCommands(const TAny* aObject)
    {
    for(TInt i = iPendingCommands.Count() - 1; i >= 0; --i)
        {
        const THuiObjectCommand* objectCommand = iPendingCommands[i].iCommand->ObjectCommand();
        if(objectCommand && objectCommand->Object() == aObject)
            {
            // Cancel this one.
            RemovePendingCommand(i);
            }
        }
    }


void CHuiScheduler::CancelCommands(const TAny* aObject, THuiOp aCommandOperation)
    {
    for(TInt i = iPendingCommands.Count() - 1; i >= 0; --i)
        {
        const THuiObjectCommand* objectCommand = iPendingCommands[i].iCommand->ObjectCommand();
        if(objectCommand && 
           objectCommand->Object() == aObject &&
           objectCommand->Operation() == aCommandOperation)
            {
            // Cancel this one.
            RemovePendingCommand(i);
            }
        }
    }
    
    
void CHuiScheduler::CancelCommands(const TAny* aObject,
                                   THuiCommandType aCommandType,
                                   TInt aParam)
    {
    for(TInt i = iPendingCommands.Count() - 1; i >= 0; --i)
        {
        const THuiObjectCommand* objectCommand = iPendingCommands[i].iCommand->ObjectCommand();

        if(objectCommand && 
           objectCommand->Object() == aObject &&
           objectCommand->Type() == aCommandType)
            {
            if(objectCommand->Type() == EHuiCommandTypeCustomEvent)
                {
                const THuiCustomEventCommand* cec = 
                    static_cast<const THuiCustomEventCommand*>( objectCommand );
                    
                if(cec->Param() != aParam)
                    {
                    // Not this one, wrong parameter.
                    continue;
                    }
                }
            // Cancel this one.
            RemovePendingCommand(i);
            }
        }
    }


TInt CHuiScheduler::TimeUntilCommand(const TAny* aObject,
                                     THuiCommandType aCommandType)
    {
    TTime now = CHuiStatic::Time();
    TInt earliest = -1;

    for(TInt i = 0; i < iPendingCommands.Count(); ++i)
        {
        const THuiObjectCommand* objectCommand = iPendingCommands[i].iCommand->ObjectCommand();
        if(objectCommand &&
           objectCommand->Object() == aObject &&
           objectCommand->Type() == aCommandType)
            {
            TInt remaining =
#ifndef EKA2
                iPendingCommands[i].iExecutionTime.MicroSecondsFrom(now).Int64().GetTInt() / 1000;
#else
                I64LOW(iPendingCommands[i].iExecutionTime.MicroSecondsFrom(now).Int64()) / 1000;
#endif

            if(remaining < 0)
                {
                /** @todo  Should never happen? */
                remaining = 0;
                }
            if(earliest < 0 || remaining < earliest)
                {
                earliest = remaining;
                }
            }
        }

    return earliest;
    }


TInt CHuiScheduler::PendingCount() const
    {
    return iPendingCommands.Count();
    }
