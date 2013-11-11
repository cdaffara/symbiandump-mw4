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
* Description:   Stub header definition for a dynamically handled terminal control interface
*
*/



#ifndef __VPNTCSTUB_H__
#define __VPNTCSTUB_H__

class CVpnTcStub : public CBase
    {
public:
    virtual void        SetTCAttributeL(const TInt aAttribId, 
                                       const TDesC8& aVal) = 0;
    virtual HBufC8*     GetTCAttributeL(const TInt aAttribId) = 0;
    virtual void        ConnectToDevLockL() = 0;


private:
    };
    
#endif

