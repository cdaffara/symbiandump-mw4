/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Command scheduler.
*
*/



#ifndef C_ALFCOMMANDSCHEDULER_H
#define C_ALFCOMMANDSCHEDULER_H

#include <e32base.h>
#include <alf/alfcommand.h>
#include <hwrmlight.h>

class CAlfEnv;

/**
 *  Command scheduler.
 *
 *  Schedules the commands (see alfcommand.h) which are send to 
 *  to the environment class with a timeout != 0.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfCommandScheduler ) : 
    public CBase, 
    public MHWRMLightObserver
    {

public:

    /**
     * Constructor
     */
    static CAlfCommandScheduler* NewL( CAlfEnv& aEnv );

    /**
     * Destructor
     */
    virtual ~CAlfCommandScheduler();

    /**
     * Executes given command in given time.
     *
     * @param aCommand Command to execute.
     * @param aTimeInMilliSeconds Time to execution.
     */
    void ScheduleCommandL( const TAlfCommand& aCommand, 
                           TInt aTimeInMilliSeconds );
    
    /**
     * Cancel all commands for given object.
     *
     * @param aObject All commands associated to this object are cancelled.
     */
    void CancelCommands( TAny* aObject );
    
    /**
     * Cancels all commands for the given object with given operation.
     *
     * @param aObject Associated object.
     * @param aCommandOperation Operation which is cancelled.
     */
    void CancelCommands( TAny* aObject, TAlfOp aCommandOperation);
    
    /**
     * Cancels all commands for the given object with given type and paramter.
     *
     * @param aObject Associated object.
     * @param aCommandType Command type.
     * @param aParam Custom parater (only checked if 
     *               aCommandType equals EAlfCommandTypeCustomEvent)
     */
    void CancelCommands( TAny* aObject, 
                         TAlfCommandType aCommandType, 
                         TInt aParam );
    
    /**
     * Calculates the milliseconds until the command.
     * 
     * @param aObject Associated object.
     * @return Time left in milliseconds to the first found command, which 
     *         matched the criteria. KErrNotFound if the none found. The return
     *         value is set to 0 if the command timer has expired and waits
     *         execution from the active scheduler.
     */ 
    TInt MilliSecondsUntilCommand( 
        TAny* aObject );
                             
    /**
     * Calculates the milliseconds until the command.
     * 
     * @param aObject Associated object.
     * @param aCommandOperation Operation which is checked.
     * @return Time left in milliseconds to the first found command, which 
     *         matched the criteria. KErrNotFound if the none found. The return
     *         value is set to 0 if the command timer has expired and waits
     *         execution from the active scheduler.
     */ 
    TInt MilliSecondsUntilCommand( 
        TAny* aObject, 
        TAlfOp aCommandOperation );
      
    /**
     * Calculates the milliseconds until the command.
     * 
     * @param aObject Associated object.
     * @param aCommandType Command type.
     * @param aParam Custom parater (only checked if 
     *               aCommandType equals EAlfCommandTypeCustomEvent)
     * @return Time left in milliseconds to the first found command, which 
     *         matched the criteria. KErrNotFound if the none found. The return
     *         value is set to 0 if the command timer has expired and waits
     *         execution from the active scheduler.
     */   
    TInt MilliSecondsUntilCommand( 
        TAny* aObject, 
        TAlfCommandType aCommandType, 
        TInt aParam );
    
    /**
     * Executes the command. Called when timer triggers.
     *
     * @param aCommand Command to execute.
     */
    void ExecuteEventL( TAlfCommand& aCommand );
    
    /**
     * Called when the application foreground status changes.
     *
     * @param aForeground ETrue if the application is (partially) on foreground.
     */
    void AppicationOnForeground( TBool aForeground );
    
    /**
     * From MHWRMLightObserver
     *
     * Called when display light status changes.
     */
    void LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus);
 
private:

    CAlfCommandScheduler( CAlfEnv& aEnv);

    void ConstructL();
    
    void UpdateSchedulerState();
    void Run();
    void Pause();

private: // data

    // Timed command class.
    class CTimedEvent : public CTimer
        {
    public:
        static CTimedEvent* NewLC( 
            const TAlfCommand& aCommand,
            CAlfCommandScheduler& aScheduler );
        ~CTimedEvent();
        
        void ExecuteAfter( TInt aIntervalInMilliSeconds, TBool aStartPaused );
        void Pause();
        void Continue();
        TInt TimeLeftInMilliSeconds() const;
        
    protected:
        CTimedEvent( CAlfCommandScheduler& aScheduler );
        void ConstructL(const TAlfCommand& aCommand);
        void RunL();
        TInt RunError(TInt aError);
    public:
        TAlfCommand* iCommand;
    private:
        CAlfCommandScheduler& iScheduler;
        TTime iLastStartTime;
        TInt iTimeLeftInMilliSeconds;
        };
      
    // Timed commands.  
    RPointerArray<CTimedEvent> iEvents;
    
    // Environment.
    CAlfEnv& iEnv;
        
    // Internal flags.
    TUint iFlags;
    
    // Light controller/observer. Own.
    CHWRMLight* iLight; 
    };


#endif // C_ALFCOMMANDSCHEDULER_H
