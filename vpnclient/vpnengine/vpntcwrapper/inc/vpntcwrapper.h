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
* Description:   PKCS#12 data handler
*
*/



#ifndef __VPNTCWRAPPER_H__
#define __VPNTCWRAPPER_H__

#include <f32file.h>
#include <TerminalControl3rdPartyAPI.h>
#include <TerminalControl.h>

#include "vpntcstub.h"

class CVpnTcWrapper : public CVpnTcStub
    {
/////////////////////////////
// Public functions
/////////////////////////////
public:
    virtual void     SetTCAttributeL(const TInt aAttribId, 
                                    const TDesC8& aVal);
    virtual HBufC8*  GetTCAttributeL(TInt aAttribId);
    virtual void     ConnectToDevLockL();
    
    virtual ~CVpnTcWrapper();

private:
    RTerminalControl                iTc;
    RTerminalControl3rdPartySession iTs;
    TBool                           iConnected;

    };
    
#endif

