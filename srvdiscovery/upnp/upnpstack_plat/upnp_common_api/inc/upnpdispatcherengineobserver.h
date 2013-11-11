/** @file
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  MDispatcherEngineObserver
*    
*
*/

#ifndef C_MUPNPDISPATCHERENGINEOBSERVER_H
#define C_MUPNPDISPATCHERENGINEOBSERVER_H

// INCLUDES
#include <e32base.h>




// CLASS DECLARATION

class MDispatcherEngineObserver
{
public:
    virtual void DeviceListReceivedL(TInt aUpdateId) = 0;
};
#endif // C_MUPNPDISPATCHERENGINEOBSERVER_H
