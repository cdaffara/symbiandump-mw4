/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _ALFHINTPLUGINDRAWER_H_
#define _ALFHINTPLUGINDRAWER_H_

#include <e32std.h>
#include "graphics/wsgraphicdrawer.h"   // class CWsGraphicDrawer

#include <alfhintpluginuids.h>
#include <alfhintapi.h>


NONSHARABLE_CLASS(CAlfHintGraphicDrawer) : public CWsGraphicDrawer
    {
public:
    enum { EImplUid = 0x200242A1 /*KAlfHintDrawerTypeUid */ };
    
    static CAlfHintGraphicDrawer* CreateL();
    
    ~CAlfHintGraphicDrawer();
    
    void ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
    
    void HandleMessage(const TDesC8& aData);

private: // from MWsObjectProvider

    TAny* ResolveObjectInterface(TUint aTypeId);

private: // from CWsGraphicDrawer

    void DoDraw(MWsGc& aGc, const TRect& aRect,const TDesC8& aData) const;

private:

    CAlfHintGraphicDrawer();
    
private:

    
    MWsGraphicDrawerEnvironment* iEnv;
    
    MAlfHintObserver* iHintApi;
    
    /*RPointerArray<CSkin> iSkins;
    
    RArray<TInt> iCurrentTargetWindows;
    */
    };

#endif // _ALFHINTPLUGINDRAWER_H_
