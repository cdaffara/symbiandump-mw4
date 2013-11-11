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
* Description:   
*
*/



#ifndef C_ALFGENCOMPONENT_H
#define C_ALFGENCOMPONENT_H

#include <e32std.h>
#include <e32base.h>

class CAlfEnv;
class CAlfMessageObserver;

class MAlfAsyncOpObserver
    {
    public:
        virtual void AlfAsyncOpCompleted(TInt aCommandId, TInt aStatus) = 0;
    };

/**
 *  Generalisation of component command interface
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfGenComponent ): public CBase 
    {
public:

    /**
    *  
    *
    */
    IMPORT_C static CAlfGenComponent* NewL(
            CAlfEnv& aEnv,
            TInt aImplementationId, 
            TInt aImplementationUid, 
            const TDesC8& aConstructionParams);
    
    /**
    * Destructor  
    */
    IMPORT_C ~CAlfGenComponent();
    
    /**
    *  Performs syncronous command and returns value of success, can be any system wide error code 
    */
    IMPORT_C TInt DoSynchronousCmd( TUint aOp, 
                               const TDesC8& aInputBuf, TDes8& aOutBuf);
                               
    /**
     * Sends a command without waiting for the reply from the server.
     * Commands sent through this API can be batched as well. If the batching
     * fails (or disabled) the DoSynchronousCmd will be called automatically.
     * 
     * @param aOp Command indentifier
     * @param aInputBuf Input buffer. The function will take copy of the 
     *        buffer content and delete it when the batch buffer is 
     *        flushed.
     *
     * @return Symbian OS error code.
     */
    IMPORT_C TInt DoCmdNoReply( TUint aOp, const TDesC8& aInputBuf );
    
    /**
    * Performs syncronous command. Given observer will be notified on success of operation via call back
    * Caller must ensure that buffers exist when command is under progress.
    *
    * @return command id which can be used for cancelling the command
    */
    IMPORT_C  TInt DoAsynchronousCmdL(TUint aOp, const TDesC8& aInputBuf, 
                               TDes8& aOutBuf, MAlfAsyncOpObserver* aObserver);
    /**
    * Cancel specificied asynchronous command. 
    * To cancel all pending requests, use Cancel() from base class    
    *
    * @param aCmdId a command to be cancelled
    */
    IMPORT_C void CancelAsynchCmd(TInt aCmdId);

    /**
    *  Unique identifier of this component, returns zero if not generated yet
    */
    IMPORT_C TInt Identifier();
    
    /**
     * Called when asynchronous command completed. This call deletes 
     * the object.
     * @param aMessageObserver Message which was completed.
     */ 
    void CommandCompleted( CAlfMessageObserver* aMessageObserver );


private:
    CAlfGenComponent(CAlfEnv& aEnv);    
    void ConstructL( 
            TInt aImplementationId, 
            TInt aImplementationUid, 
            const TDesC8& aConstructionParams);

private: // data
    CAlfEnv& iEnv;
    class TPrivateData;
    TPrivateData* iData;
    };


#endif // C_ALFGENCOMPONENT_H
