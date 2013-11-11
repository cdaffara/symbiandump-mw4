/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/



#ifndef __MENUITEMCALLBACKS_H
#define __MENUITEMCALLBACKS_H

//  INCLUDES
#include <e32base.h>
#include "WidgetEventHandler.h"

class MJSMenuItemCallbacks
{
public:
    virtual TInt createOptionsMenuItem( const TDesC& aText, TInt aCmdId, TInt aInternalId, void* aObj ) = 0;                
    virtual TInt addOptionsMenuItem( TInt aInternalId, TInt aParentInternalId, TBool aShow=ETrue ) = 0;
    virtual void deleteMenuItem( TInt aInternalId ) = 0;       
    virtual void setDimmed( TInt aInternalId, TBool aValue ) = 0;        
    virtual KJS::WidgetEventHandler* menuItemObserver( TInt aInternalId, TInt& aCmdId ) = 0;            
    virtual void setMenuItemObserver( TInt aInternalId, KJS::WidgetEventHandler* aOnSelectCallback ) = 0;                

};

#endif
