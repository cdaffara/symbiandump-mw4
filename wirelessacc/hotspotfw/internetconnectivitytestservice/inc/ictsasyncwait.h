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
* Description:   Asynchronic waiting possibility
*
*/




#ifndef __ICTSASYNCWAIT_H__
#define __ICTSASYNCWAIT_H__

/**
 * CIctsAsyncWait class
 */
 
class CIctsAsyncWait : public CActive
    {
    public: 

        /**
        * C++ default constructor.
        */
        CIctsAsyncWait();
        
        /**
        * Destructor.
        */    
        ~CIctsAsyncWait() ;

    public: 

        /**
        * Nested scheduler loop
        */
        void Wait() ;

    private: // CActive

        void RunL();
        void DoCancel();


    private: // data

        CActiveSchedulerWait iActiveWait ; // nested loop for active scheduler
} ;

#endif // __ICTSASYNCWAIT_H__
