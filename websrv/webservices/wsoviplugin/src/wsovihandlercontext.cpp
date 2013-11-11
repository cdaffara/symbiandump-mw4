/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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








#include "wsovihandlercontext.h"
CWSOviHandlerContext* CWSOviHandlerContext::NewL()
    {
    CWSOviHandlerContext* self = new (ELeave) CWSOviHandlerContext();
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSOviHandlerContext::CWSOviHandlerContext():
    iCoreServiceManager(NULL),
    iWSOviPlugin(NULL),
    iLogger(NULL)
    {
    }

// Constructor
void CWSOviHandlerContext::ConstructL()
    {
    }


// Destructor
CWSOviHandlerContext::~CWSOviHandlerContext()
    {
    delete iVersion;
    delete iDiff;
    }

//from HandlerContext        
TInt CWSOviHandlerContext::Add(MSenCoreServiceManager& aValue)
    {
    iCoreServiceManager = &aValue;
    return KErrNone;
    }

MSenCoreServiceManager* CWSOviHandlerContext::GetSenCoreServiceManager()
    {
    return iCoreServiceManager;
    }



//from MContext      
SenContext::TContextType CWSOviHandlerContext::Type() const
    {
    return SenContext::EHandlerContext;
    }
SenContext::TContextDirection CWSOviHandlerContext::Direction() const
    {
    return SenContext::EBoth;
    }
TInt CWSOviHandlerContext::Add(const TDesC8& /*aKey*/, const TDesC8& /*aValue*/)
    {
    return KErrNotSupported;
    }
TInt CWSOviHandlerContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
    TInt retVal(KErrNone);
    //version with makeing of copy
    if (aKey == HandlerContextKey::KVersion)
        {
        delete iVersion;
        iVersion = NULL;
        TRAP(retVal,iVersion = aValue.AllocL());
        }
    else
        {
        retVal = KErrNotFound;
        }
    return retVal;    
    }
const TDesC8* CWSOviHandlerContext::GetDesC8L(const TDesC8& aKey)
    {
    if (aKey == HandlerContextKey::KVersion)
        {
        return iVersion;
        }
    else
        {
        return NULL;
        }
    }
TInt CWSOviHandlerContext::Add(const TDesC8& /*aKey*/, TInt /*aValue*/)
    {
    return KErrNotSupported;
    }
TInt CWSOviHandlerContext::Update(const TDesC8& aKey, TInt aValue)
    {
    TInt retVal(KErrNone);
    //version with makeing of copy
    if (aKey == HandlerContextKey::KDiff)
        {
        delete iDiff;
        iDiff = new TInt(aValue);
        }
    else
        {
        retVal = KErrNotFound;
        }
    return retVal;    
    }
const TInt* CWSOviHandlerContext::GetIntL(const TDesC8& aKey)
    {
    if (aKey == HandlerContextKey::KDiff)
        {
        return iDiff;
        }
    else
        {
        return NULL;
        }
    }
TInt CWSOviHandlerContext::Add(const TDesC8& /*aKey*/, CSenElement* /*aValue*/)
    {
    return KErrNotSupported;
    }
TInt CWSOviHandlerContext::Update(const TDesC8& /*aKey*/, CSenElement* /*aValue*/)
    {
    return KErrNotSupported;
    }
const CSenElement* CWSOviHandlerContext::GetSenElementL(const TDesC8& /*aKey*/)
    {
    return NULL;
    }
TInt CWSOviHandlerContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    if (aKey == HandlerContextKey::KSIF())
        {
        if(aValue)
        	{
        	iWSOviPlugin = (CWSOviPlugin*)aValue;
        	}
        return KErrNone;
        }
    else if (aKey == HandlerContextKey::KLogger())
        {
        iLogger = (RFileLogger*)aValue;
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

TInt CWSOviHandlerContext::Update(const TDesC8& /*aKey*/, TAny* /*aValue*/)
    {
    return KErrNotSupported;
    }
TAny* CWSOviHandlerContext::GetAnyL(const TDesC8& aKey)
    {
     if (aKey == HandlerContextKey::KSIF())
        {
        return (TAny*)iWSOviPlugin;
        }
     else if (aKey == HandlerContextKey::KLogger())
        {
        return (TAny*)iLogger;
        }
    else
        {
        return NULL;
        }
    }

const CSenXmlReader* CWSOviHandlerContext::GetParser()
    {
    return NULL;
    }
TInt CWSOviHandlerContext::Remove(const TDesC8& /*aKey*/)  
    {
    return KErrNotSupported;
    }
TInt CWSOviHandlerContext::Count() const
    {
    return KErrNotSupported;
    }
TPtrC8 CWSOviHandlerContext::KeyAtL(TInt /*aIndex*/)   
    {
    return KNullDesC8();
    }
void CWSOviHandlerContext::Reset() 
    {
    }
    
   
// END OF FILE

