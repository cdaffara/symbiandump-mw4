/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/


#ifndef LOCODBEARERPLUGINOBS_H_
#define LOCODBEARERPLUGINOBS_H_

#include <e32base.h>
#include <e32debug.h>
#include <StifLogger.h>
#include <testclassassert.h>

#include <locodbearerplugin.h>
#include <locodbearerpluginobserver.h>
#include <locodbearer.h>

NONSHARABLE_CLASS( CLocodBearerPluginObs ) : public MLocodBearerPluginObserver
{
public:
    static CLocodBearerPluginObs* NewL( CStifLogger* aLog );
    virtual ~CLocodBearerPluginObs();
    
    void ActivateObserver( TBool& aLocodBearerStatus );
    
private:
    CLocodBearerPluginObs( CStifLogger* aLog );
    
    void ConstructL();
    void NotifyBearerStatus( TLocodBearer aBearer, TBool aStatus );
    
private:
    CStifLogger* iLog;
    CActiveSchedulerWait* iWait;
    TBool iLocodBearerStatus;
};

#endif /*LOCODBEARERPLUGINOBS_H_*/
