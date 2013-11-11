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



#ifndef __HUISCHEDULER_H__
#define __HUISCHEDULER_H__


#include <e32base.h>
#include <uiacceltk/HuiCommand.h>
#include <uiacceltk/HuiObserverArray.h>


/* Forward declaration. */
class CHuiEnv;


/**
 * The scheduler executes commands at certain points in time. There is one
 * scheduler instance created for each environment. Everyone has access to 
 * their environment's scheduler, and can send commands with it. Commands
 * include, for instance, visual animation commands and action commands sent
 * by controls. An action command would be sent, for example, when a menu 
 * item is selected or a button is pressed.
 */
NONSHARABLE_CLASS(CHuiScheduler) : public CBase
	{
public:

	/* Constructors and destructor. */

	/**
	 * Constructor. This is only done internally in the toolkit. Applications
	 * cannot construct schedulers.
	 *
	 * @param aEnv  Environment that owns the scheduler.
	 */
	CHuiScheduler(CHuiEnv& aEnv);
	
	/**
	 * Destructor.
	 */
	virtual ~CHuiScheduler();


	/* Methods. */

    /**
     * Called to notify the scheduler that time has passed and it may be 
     * necessary to trigger new animations and actions.
     *
     * @param aElapsedTime  Number of elapsed seconds.
     */
    void AdvanceTime(TReal32 aElapsedTime);	

    /**
     * Post a new command that will be executed immediately or after a
     * period of time.
     *
     * @param aCommand  Command to send.
     * @param aDelayMilliSeconds  Time to wait before executing.
     */
    void PostCommandL(const THuiCommand& aCommand, TInt aDelayMilliSeconds);
    
    /** 
     * Cancel all commands related to an object.
     *
     * @param aVisual  Visual.
     */     
    void CancelCommands(const TAny* aObject);
    
    /** 
     * Cancel commands related to an object.
     *
     * @param aVisual       Visual.
     & @param aCommandType  Type of command to cancel.
     */     
    void CancelCommands(const TAny* aObject, THuiOp aCommandOperation);
    
    /** 
     * Cancel commands related to a specific object.
     *
     * @param aControl      Control.
     * @param aCommandType  Type of command.
     * @param aParam        Parameter for custom event.
     */     
    void CancelCommands(const TAny* aObject, THuiCommandType aCommandType, TInt aParam);
    
    /**
     * Determines how much time is remaining until a command is to be
     * executed.
     *
     * @param aObject       Object.
     * @param aCommandType  Type of command.
     *     
     * @return  Time interval in milliseconds. -1, if no commands found.
     */
    TInt TimeUntilCommand(const TAny* aObject, THuiCommandType aCommandType);
    
    /**
     * Counts the number of pending commands.
     *
     * @return  Number of commands waiting to be executed.
     */
    TInt PendingCount() const;
    

private:
    
    /* Private methods */
        
    /**
     * Removes a pending command from the queue.
     *
     * @param aIndex  Index of the pending command.
     */
    void RemovePendingCommand(TInt aIndex);


private:

	/**
	 * A command to be executed at a certain point in time. A list of these delayed
	 * command events is stored in the HuiScheduler.
	 * 
	 */
	 
    class TDelayedCommand
        {
    public:
    	/** Constructor. A command may be passed in the initialiser. */
        TDelayedCommand(const THuiCommand* aCommand)
                : iCommand(aCommand)
            {
            }
        
        /** The time when this command will be executed. */
        TTime iExecutionTime;

        /** 
         * The command that will be executed.
         * 
         * @see THuiCommand
         * */        
        const THuiCommand* iCommand;
        };

    /** Scheduler's environment. */
    CHuiEnv& iEnv;
    
    /** Pending commands waiting for execution. */
    RArray<TDelayedCommand> iPendingCommands;
    
	};

#endif  // __HUISCHEDULER_H__
