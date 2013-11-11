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



#ifndef __MENUCALLBACKS_H
#define __MENUCALLBACKS_H

//  INCLUDES
#include <e32base.h>
#include "WidgetEventHandler.h"

class MJSMenuCallbacks
{
public:
    virtual void* getFromMenuItemCollection( TInt aCmdId ) = 0;            
    virtual void* getFromMenuItemCollection( const TDesC& aTextVal ) = 0;              
    virtual void hideSoftkeys() = 0;
    virtual void showSoftkeys() = 0;          
    virtual void setRightSoftKeyLabel( const TDesC& aText ) = 0;
    virtual void setLeftSoftKeyLabel( const TDesC& aText ) = 0;
    virtual void clearMenuItems() = 0;    
};

#endif
