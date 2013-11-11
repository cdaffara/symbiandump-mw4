/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __UPNPTMINFOELEMENT_INL__
#define __UPNPTMINFOELEMENT_INL__


// ---------------------------------------------------------------------------------
// CUpnpTmInfoElement::TerminalModeInfoType
// Method is used to retrieve terminal mode info type
// ---------------------------------------------------------------------------------
//
inline const TDesC8& CUpnpTmInfoElement::TerminalModeInfoType( )const
    {
    return *iInfoTypeBuffer;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmInfoElement::TmInfoElementCount
// Method is used to retrieve the number of key-value pairs for each info type
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmInfoElement::TmInfoElementCount()const
    {
    return iInfoElementNameArray.Count();
    }

#endif  //__UPNPTMINFOELEMENT_INL__
