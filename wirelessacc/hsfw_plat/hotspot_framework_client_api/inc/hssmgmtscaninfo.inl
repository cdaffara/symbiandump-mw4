/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Inline functions of CHssMgmtScanInfo class.
*
*/



#ifndef HSSMGMTSCANINFO_INL
#define HSSMGMTSCANINFO_INL


// Static constructor.
inline CHssMgmtScanInfo* CHssMgmtScanInfo::NewL()
    {
    const TUid KCHssMgmtScanInfoUid = { 0x10282ed0 };

    TAny* interface = REComSession::CreateImplementationL(
        KCHssMgmtScanInfoUid,
        _FOFF( CHssMgmtScanInfo,
        iInstanceIdentifier ) );
    return reinterpret_cast<CHssMgmtScanInfo*>( interface );
    }
    
// Destructor
inline CHssMgmtScanInfo::~CHssMgmtScanInfo()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }

#endif // HSSMGMTSCANINFO_INL
            
// End of File
