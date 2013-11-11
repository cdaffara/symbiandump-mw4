/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: IKEv2 Mobility and Multihoming Protocol.
*
*/
#ifndef _IKEV2MOBIKE_H_
#define _IKEV2MOBIKE_H_

class CIkev2Negotiation;
class TNotifPayloadIkev2;

class Ikev2MobIke
    {
public:
    static TBool ProcessNotifysL(CIkev2Negotiation* aNegotiation, 
                                 const CArrayFixFlat<TNotifPayloadIkev2*>& aNotifys,
								 TBool aRequest, TInt Exchange);
	static TBool SendUpdateSaAddrNotifyL(CIkev2Negotiation* aNegotiation);	
    
    };

#endif
