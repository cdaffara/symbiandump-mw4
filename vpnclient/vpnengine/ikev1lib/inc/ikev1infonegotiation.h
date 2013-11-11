/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIkev1InfoNegotiation class
*
*/

#ifndef C_IKEV1INFONEGOTIATION_H
#define C_IKEV1INFONEGOTIATION_H

#include <e32base.h>

class CIkev1Negotiation;
class CIkev1PluginSession;
class ThdrISAKMP;
class TPayloadISAKMP;
class TNotificationISAKMP;
class MIkeDebug;
class MKmdEventLoggerIf;
class TInetAddr;

class CIkev1InfoNegotiation : public CBase
{
    friend class TIkev1IsakmpStream;
    friend class CIkev1PlugIn;

public:
    CIkev1InfoNegotiation( CIkev1PluginSession& aPluginSession,
                           CIkev1Negotiation& aNegotiation,
                           MIkeDebug& aDebug );
    void ExecuteL( const ThdrISAKMP& aHdr,
                   const TInetAddr& aSrcAddr,
                   TInt aLocalPort );
    
    MKmdEventLoggerIf& EventLogger();

private:
    void InfoExchangeL( const ThdrISAKMP& aHdr );
    TBool ProcessNotificationL( const TPayloadISAKMP* aPayload, TBool aEncrypted );
    TBool ProcessDeleteL( const TPayloadISAKMP* aPayload );
	TBool ProcessDPDNotifyL( TNotificationISAKMP* aNotify );	

private:
    CIkev1PluginSession& iPluginSession;
    CIkev1Negotiation& iNegotiation;
    TUint32 iMessageId;
    MIkeDebug& iDebug;
};

#endif // C_IKEV1INFONEGOTIATION_H
