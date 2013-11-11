/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition USB bearer implementation of EComm Interface. 
*
*/


#ifndef USBLCDPLUGIN_H
#define USBLCDPLUGIN_H

#include <e32property.h>
#include <locodbearerplugin.h>
#include "usblcdactive.h"


/**
 *  Class of EComm interface implementation
 */
NONSHARABLE_CLASS(CUsbLcdPlugin) : public CLocodBearerPlugin
    {

public:

    static CUsbLcdPlugin* NewL(TLocodBearerPluginParams& aParams);
    ~CUsbLcdPlugin(); 
   
private:

    CUsbLcdPlugin(TLocodBearerPluginParams& aParams);
    void ConstructL();

private: // data
    
    /**
     * Array of Active Object, own.
     */
    CUsbLcdActive*  iUsbLcdActive;
       
    };


#endif // USBLCDPLUGIN_H
