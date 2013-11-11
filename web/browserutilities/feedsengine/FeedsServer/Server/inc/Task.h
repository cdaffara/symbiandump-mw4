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


#ifndef TASK_H
#define TASK_H


// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIdle;
class CFeedsServer;

// CLASS DECLARATION


/**
*  Task base class.  To cancel a task just simply delete it.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CTask: public CBase
    {
    public: // Destructor
        /**
        * Destructor.
        */        
        virtual ~CTask();
        
        
    public: // New Methods
        /**
        * Starts the task.
        *
        * @since 3.0
        * @return Void
        */
        virtual void StartTaskL() = 0;
        
        /**
        * Starts an CIdle to delete the instance after the callstack has unrolled.
        *
        * @since 3.0
        * @return Void
        */
        void AutoDelete();
        

    protected:
        /**
        * C++ default constructor.
        */
        CTask(CFeedsServer& aFeedsServer);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL(TBool aAutoDelete);

        
    private:
        /**
        * Deletes the instance after the callstack has unrolled..
        *
        * @since 3.0
        * @return Void
        */
        static TInt DelayedDelete(TAny* aPtr);
               
        
    protected:
        CFeedsServer&  iFeedsServer;


    private:
        CIdle*         iAutoDelete;    
    };

#endif      // TASK_H
            
// End of File