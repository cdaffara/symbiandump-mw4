/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#include "wsstarhandlercontext.h"
CWSStarHandlerContext* CWSStarHandlerContext::NewL()
    {
    CWSStarHandlerContext* self = new (ELeave) CWSStarHandlerContext();
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSStarHandlerContext::CWSStarHandlerContext():
    iCoreServiceManager(NULL),
    iWSStarPlugin(NULL),
    iLogger(NULL)
    {
    }

// Constructor
void CWSStarHandlerContext::ConstructL()
    {
    }


// Destructor
CWSStarHandlerContext::~CWSStarHandlerContext()
    {
    delete iVersion;
    }

//from HandlerContext        
TInt CWSStarHandlerContext::Add(MSenCoreServiceManager& aValue)
    {
    iCoreServiceManager = &aValue;
    return KErrNone;
    }

MSenCoreServiceManager* CWSStarHandlerContext::GetSenCoreServiceManager()
    {
    return iCoreServiceManager;
    }



//from MContext      
SenContext::TContextType CWSStarHandlerContext::Type() const
    {
    return SenContext::EHandlerContext;
    }
SenContext::TContextDirection CWSStarHandlerContext::Direction() const
    {
    return SenContext::EBoth;
    }
TInt CWSStarHandlerContext::Add(const TDesC8& /*aKey*/, const TDesC8& /*aValue*/)
    {
    return KErrNotSupported;
    }
TInt CWSStarHandlerContext::Update(const TDesC8& aKey, const TDesC8& aValue)
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
const TDesC8* CWSStarHandlerContext::GetDesC8L(const TDesC8& aKey)
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
TInt CWSStarHandlerContext::Add(const TDesC8& /*aKey*/, TInt /*aValue*/)
    {
    return KErrNotSupported;
    }
TInt CWSStarHandlerContext::Update(const TDesC8& /*aKey*/, TInt /*aValue*/)
    {
    return KErrNotSupported;
    }
const TInt* CWSStarHandlerContext::GetIntL(const TDesC8& /*aKey*/)
    {
    return NULL;
    }
TInt CWSStarHandlerContext::Add(const TDesC8& /*aKey*/, CSenElement* /*aValue*/)
    {
    return KErrNotSupported;
    }
TInt CWSStarHandlerContext::Update(const TDesC8& /*aKey*/, CSenElement* /*aValue*/)
    {
    return KErrNotSupported;
    }
const CSenElement* CWSStarHandlerContext::GetSenElementL(const TDesC8& /*aKey*/)
    {
    return NULL;
    }
TInt CWSStarHandlerContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    if (aKey == HandlerContextKey::KSIF())
        {
        iWSStarPlugin = (CWSStarPlugin*)aValue;
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

TInt CWSStarHandlerContext::Update(const TDesC8& /*aKey*/, TAny* /*aValue*/)
    {
    return KErrNotSupported;
    }
TAny* CWSStarHandlerContext::GetAnyL(const TDesC8& aKey)	//codescannerwarnings
    {
     if (aKey == HandlerContextKey::KSIF())
        {
        return (TAny*)iWSStarPlugin;
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

const CSenXmlReader* CWSStarHandlerContext::GetParser()
    {
    return NULL;
    }
TInt CWSStarHandlerContext::Remove(const TDesC8& /*aKey*/)  
    {
    return KErrNotSupported;
    }
TInt CWSStarHandlerContext::Count() const
    {
    return KErrNotSupported;
    }
TPtrC8 CWSStarHandlerContext::KeyAtL(TInt /*aIndex*/)   
    {
    return KNullDesC8();
    }
void CWSStarHandlerContext::Reset() 
    {
    }
    
   
// END OF FILE
