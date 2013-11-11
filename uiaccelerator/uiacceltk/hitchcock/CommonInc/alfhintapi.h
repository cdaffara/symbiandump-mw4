/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef _ALFHINTAPI_H_
#define _ALFHINTAPI_H_

#include <graphics/wsgraphicdrawerinterface.h>
#include <alfhintpluginuids.h>

const TInt KAlfHintObjectInterface = 0x200242A5;

class MAlfHintObserver : public MWsObjectProvider
    {
public: 
    DECLARE_WS_TYPE_ID(KAlfHintObjectInterface)
    
    virtual void InsertTagL( TInt aTag, TInt32 aParameter = 0 ) = 0;
    virtual void InsertTagL( TInt aTag, TRect aBoundingRect, TInt aLayerId ) =0;
    };

#endif // _ALFHINTAPI_H_
