/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* A server side session object. Owns instances of  the CPKIService and 
* CPKIWrapper classes.  All requests to the service objects go through 
* this object.
*
*/



#ifndef __PKISESSION__
#define __PKISESSION__

#include <e32base.h>
#include <unifiedkeystore.h>
#include "pkidefs.h"
#include "keyoperationprovider.h"

class CPKIWrapper;
class CPKIService;
class CPKIMapper;
class CKeyOperationQueue;

class CPKISession: public CSession2//, public MKeyOperationObserver

{
    public: 
        static CPKISession* NewL(CPKIService& aServer, 
                                 CPKIMapper& aMapper,
                                 CKeyOperationQueue& aKeyOperationQueue);
        virtual ~CPKISession(void);

        void ServiceL(const RMessage2& aMessage);
        
        void SetRequiredBufferSize(TInt aSize);
        void SetKeyList(CArrayFixFlat<TKeyListEntry> *aKeyList);
        void InitializeWrapperL(const RMessage2& aMessage);
        
        
private:
        CPKISession(CPKIService& aServer, CPKIMapper& aMapper, CKeyOperationQueue& aKeyOperationQueue);
        void ConstructL();
        //void InitializeL(const RMessage2& aMessage);
            
        /**
         * Set the used key store.
         */        
        void SetKeyStoreL(TPkiServiceStoreType aStoreType);
        TPkiServiceStoreType KeyStore() const;

        CPKIService        &iServer;
        CPKIMapper         &iMapper;
        CKeyOperationQueue &iKeyOperationQueue;
        
        CPKIWrapper        *iWrapper;
		RArray<TUid>        iUidArray;
		TInt        	    iApplCount;
		
		TInt                iUsedKeyStore;				
		CArrayFixFlat<TKeyListEntry> *iKeyList;		
		TInt                iRequiredBufferSize;
};

#endif