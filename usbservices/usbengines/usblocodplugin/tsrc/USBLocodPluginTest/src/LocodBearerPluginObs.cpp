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


#include "LocodBearerPluginObs.h"

CLocodBearerPluginObs::CLocodBearerPluginObs( CStifLogger* aLog )
    {
    iLog = aLog;
    }


CLocodBearerPluginObs::~CLocodBearerPluginObs()
    {
    STIF_LOG( "[STIF_LOG] >>>CLocodBearerPluginObs::~CLocodBearerPluginObs" );
    
    if ( iWait -> IsStarted() )
        iWait -> AsyncStop();
    delete iWait;
    iWait = NULL;
    
    STIF_LOG( "[STIF_LOG] <<<CLocodBearerPluginObs::~CLocodBearerPluginObs" );
    }


CLocodBearerPluginObs* CLocodBearerPluginObs::NewL( CStifLogger* aLog )
    {
    CLocodBearerPluginObs* self = new ( ELeave )CLocodBearerPluginObs( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


void CLocodBearerPluginObs::ConstructL()
    {
    STIF_LOG( "[STIF_LOG] >>>CLocodBearerPluginObs::ConstructL" );   
    iWait = new ( ELeave )CActiveSchedulerWait();    
    STIF_LOG( "[STIF_LOG] <<<CLocodBearerPluginObs::ConstructL" );
    }


void CLocodBearerPluginObs::NotifyBearerStatus( TLocodBearer aBearer, TBool aStatus )
    {
    STIF_LOG( "[STIF_LOG] >>>CLocodBearerPluginObs::NotifyBearerStatus" );
    if ( iWait -> IsStarted() )
        iWait -> AsyncStop();
    iLocodBearerStatus = aStatus;
    STIF_LOG( "[STIF_LOG] <<<CLocodBearerPluginObs::NotifyBearerStatus" );
    }


void CLocodBearerPluginObs::ActivateObserver( TBool& aLocodBearerStatus )
    {
    STIF_LOG( "[STIF_LOG] >>>CLocodBearerPluginObs::ActivateObserver" ); 
    iWait -> Start();
    aLocodBearerStatus = iLocodBearerStatus;
    STIF_LOG( "[STIF_LOG] <<<CLocodBearerPluginObs::ActivateObserver" );
    }
