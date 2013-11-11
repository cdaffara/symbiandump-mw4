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



#ifndef __ALFRENDERSTAGEFACTORY_H__
#define __ALFRENDERSTAGEFACTORY_H__

#include "graphics/wsplugin.h"
#include "graphics/wsrenderstagefactory.h"

class CWsRenderStage;
class MWsGraphicDrawerEnvironment;

/**
This is an implementation of MWsRenderStageFactory, in the form of a CWsPlugin.
The window server will use this to create standard render stages for each screen,
if the ini file specifies them.
*/
NONSHARABLE_CLASS(CAlfRenderStageFactory) : public CWsPlugin, public MWsRenderStageFactory
	{
public:
	enum { EImplUid = 0x200184B6 };
	
public: // New
    
    /**
     * CreateL
     * 
     * Create the class
     * 
     * @return New instance of the class
     */
	static CAlfRenderStageFactory* CreateL();
	
    /**
     * C++ destructor
     */
	~CAlfRenderStageFactory();
	
public: // From CWsPlugin

	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
	const TDesC& PluginName() const;

public: // implementation of MWsObjectProvider
	
    TAny* ResolveObjectInterface(TUint aTypeId);

public: // implementation of MWsRenderStageFactory
    
    CWsRenderStage* CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, TInt aScreenNumber);
    CWsRenderStage* CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);
    CWsRenderStage * CreateStageL(MWsScreen * aScreen, MWsScreenRedraw * aScreenRedraw)
        {
        return CreateRenderStageL(aScreen, aScreenRedraw, 0);
        }

private: // New
    
    /**
     * C++ constructor
     */
	CAlfRenderStageFactory();
	
	};

#endif //__STDRENDERSTAGEFACTORY_H__
