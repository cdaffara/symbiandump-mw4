/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDE FILES
#include "sencontext.h" 
#include "sencontextitem.h"
#include "SenElement.h"
#include "SenXmlReader.h"

//#include "SenPointerMap.h" // internal: Utils\inc

EXPORT_C CSenContext* CSenContext::NewL(SenContext::TContextDirection aDirection)
    {
    CSenContext* pNew = CSenContext::NewLC(aDirection);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenContext* CSenContext::NewLC(SenContext::TContextDirection aDirection)
    {
    CSenContext* pNew = new (ELeave) CSenContext(aDirection);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenContext::CSenContext(SenContext::TContextDirection aDirection)
:  iItems( EFalse, ETrue ),
   iDirection( aDirection )
    {
    }

EXPORT_C CSenContext::~CSenContext()
    {
    iItems.Reset();
    }
       
EXPORT_C void CSenContext::BaseConstructL()
    {
    }

// From MSenContext:
SenContext::TContextType CSenContext::Type() const
    {
    return SenContext::EContextBase;
    }
       
SenContext::TContextDirection CSenContext::Direction() const
    {
    return iDirection;
    }
       
TInt CSenContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {
    if( iItems.Count() > 0 && iItems.Find(aKey) != KErrNotFound )
        {
        return KErrAlreadyExists;
        }

    CSenContextItem* pItem = NULL;
    TInt leaveCode(KErrNone);
    TRAP( leaveCode, pItem = CSenContextItem::NewL(aValue); )
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.Append(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
       
TInt CSenContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = NULL;
    TRAP( leaveCode, pItem = CSenContextItem::NewL(aValue); )
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.UpdateValue(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
       
const TDesC8* CSenContext::GetDesC8L(const TDesC8& aKey)
    {
    TInt index = iItems.Find(aKey);
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        if( pItem )
            {
            return pItem->ToDesC8L();
            }
        }
    return NULL;
    }
       
TInt CSenContext::Add(const TDesC8& aKey, TInt aValue)
    {
    if( iItems.Count() > 0 && iItems.Find(aKey) != KErrNotFound )
        {
        return KErrAlreadyExists;
        }

    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = NULL;
    TRAP( leaveCode, pItem = CSenContextItem::NewL(aValue); )
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.Append(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
       
TInt CSenContext::Update(const TDesC8& aKey, TInt aValue)
    {
    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = NULL;
    TRAP( leaveCode, pItem = CSenContextItem::NewL(aValue); )
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.UpdateValue(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
       
const TInt* CSenContext::GetIntL(const TDesC8& aKey)
    {
    TInt index = iItems.Find(aKey);
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        if(pItem)
            {
            return pItem->ToIntL();
            }
        }
    return NULL;
    }
       
TInt CSenContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    if( iItems.Count() > 0 && iItems.Find(aKey) != KErrNotFound )
        {
        return KErrAlreadyExists;
        }

    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = NULL;
    TRAP( leaveCode, pItem = CSenContextItem::NewL(aValue); )
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.Append(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
       
TInt CSenContext::Update(const TDesC8& aKey, CSenElement* aValue)
    {
    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = NULL;
    TRAP( leaveCode, pItem = CSenContextItem::NewL(aValue); )
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }
        
    TInt retCode(iItems.UpdateValue(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
       
const CSenElement* CSenContext::GetSenElementL(const TDesC8& aKey)
    {
    CSenElement* pElement = NULL;
    TInt index = iItems.Find(aKey);
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        pElement = &(pItem->ToSenElementL());
        }
    return pElement;
    }
       
TInt CSenContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    if( iItems.Count() > 0 && iItems.Find(aKey) != KErrNotFound )
        {
        return KErrAlreadyExists;
        }

    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = NULL;
    TRAP( leaveCode, pItem = CSenContextItem::NewL(aValue); )
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.Append(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
       
TInt CSenContext::Update(const TDesC8& aKey, TAny* aValue)
    {
    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = NULL;
    TRAP( leaveCode, pItem = CSenContextItem::NewL(aValue); )
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.UpdateValue(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
       
TAny* CSenContext::GetAnyL(const TDesC8& aKey)	//codescannerwarnings
    {
    TAny* retVal = NULL;

    TInt index = iItems.Find(aKey);
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        retVal = pItem->ToAnyL();
        }
    return retVal;
    }
     
TInt CSenContext::Remove(const TDesC8& aKey) 
    {
    return iItems.RemoveByKey(aKey);
    }

TInt CSenContext::Count() const
    {
    return iItems.Count();
    }

TPtrC8 CSenContext::KeyAtL(TInt aIndex)
    {
    return *iItems.KeyAt(aIndex);
    }

void CSenContext::Reset()
    {
    iItems.Reset();
    }
    
TInt CSenContext::SetDirection( SenContext::TContextDirection aDirection )
    {
    iDirection = aDirection;
    return KErrNone;
    }
    
const CSenXmlReader* CSenContext::GetParser()
    {
    return NULL; 
    }
    

// End of file

