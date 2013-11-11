/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   AlfRenderStage provides a method for redirect avkon drawing to a graphics accelerator
*
*/

#include <graphics/wsscreendevice.h>
#include "alflogger.h"
#include "alfrenderstagefactory.h"
#include "alfrenderstage.h"

_LIT(KAlfRenderStageName, "alf");

// ---------------------------------------------------------------------------
// CreateL
// ---------------------------------------------------------------------------
//
CAlfRenderStageFactory* CAlfRenderStageFactory::CreateL()
	{
	return new(ELeave) CAlfRenderStageFactory;
	}
	
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfRenderStageFactory::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	__ALFLOGSTRING("ALF-RS: About to baseconstruct factory");
    BaseConstructL(aEnv);
	}
	
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CAlfRenderStageFactory::CAlfRenderStageFactory()
	{
	}
	
// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CAlfRenderStageFactory::~CAlfRenderStageFactory()
	{
	}

/**
Overidding MWsObjectProvider
*/

// ---------------------------------------------------------------------------
// ResolveObjectInterface
// ---------------------------------------------------------------------------
//
TAny* CAlfRenderStageFactory::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case KMWsRenderStageFactory:
			return static_cast<MWsRenderStageFactory*>(this);
		default:
		    __ALFLOGSTRING("ALF-RS: Unknown interface required, returning NULL");
		}

	return NULL;
	}

// ---------------------------------------------------------------------------
// CreateFinalRenderStageL
// ---------------------------------------------------------------------------
//
CWsRenderStage* CAlfRenderStageFactory::CreateFinalRenderStageL(MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, TInt /*aScreenNumber*/)
	{
	// this factory does not have an implementation that could be used as final render stage  
    CAlfRenderStage* stage = NULL;
	return stage;
	}
	
// ---------------------------------------------------------------------------
// CreateRenderStageL
// ---------------------------------------------------------------------------
//
CWsRenderStage* CAlfRenderStageFactory::CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* /*aScreenRedraw*/, CWsRenderStage* aNextStage)
	{
	MWsScreenDevice* wsScreenDevice = (MWsScreenDevice*)aNextStage->ResolveObjectInterface(MWsScreenDevice::EWsObjectInterfaceId);
	CAlfRenderStage* stage = 0;
	if (wsScreenDevice)  
        {	
        stage = CAlfRenderStage::NewL( &Env(), aScreen, wsScreenDevice->ScreenNumber(), aNextStage);
        }
	return stage;
	}

// ---------------------------------------------------------------------------
// PluginName
// ---------------------------------------------------------------------------
//
const TDesC& CAlfRenderStageFactory::PluginName() const
	{
	return KAlfRenderStageName; // The name _must_be_ the same as defined in wsini.ini
	}
