/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   PKI server main module
*
*/



#ifndef __PKISERVICE_H__
#define __PKISERVICE_H__

#include <e32base.h>

class CPKIMapper;
class CSuspendedShutdown;
class CCertificateRequestStore;
class CKeyOperationQueue;

class CPKIService:public CPolicyServer
{
    public:
        static CPKIService* NewL(void);
        virtual ~CPKIService(void);

        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
        void SessionDeleted();

        CCertificateRequestStore& CertificateRequestStore();    
        
    private:
        CPKIService();
        void ConstructL();
        
        static const TUint iRangeCount;
        static const TInt iRanges[];
        static const TUint8 iElementIndex[];

        static const CPolicyServer::TPolicyElement iElements[];
        static const CPolicyServer::TPolicy iPolicy;
        
        mutable TInt        iSessionCount;
    
        CCertificateRequestStore *iCertificateRequestStore;
        CSuspendedShutdown *iShutdown;
        TBool               iStopImmediately;                
        CPKIMapper         *iMapper; 
        CKeyOperationQueue *iKeyOperationQueue;                       
};

class CSuspendedShutdown : public CActive
{
    public:
        CSuspendedShutdown();
        void Construct();
        ~CSuspendedShutdown();
        void ArmShutdown();
    private:
        void DoCancel();
        void RunL();
    private:
        RTimer iTimer;
};


#endif
