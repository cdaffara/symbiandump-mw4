/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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



#ifndef __WIDGETENGINEINTERFACE
#define __WIDGETENGINEINTERFACE

//  INCLUDES
#include "config.h"
#include <e32base.h>
#include <eikenv.h>
#include "BrCtlDefs.h"
#include "WidgetEngineCallbacks.h"
#include "WidgetJSObjectProtector.h"
#include "wtf/HashSet.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATION
class CMenuClient;
class CWidgetClient;
class MWidgetCallback;
class WidgetPreferences;

namespace KJS {
	class JSValue;
}

// CLASS DECLARATION
class MWidgetEngineBridge
{
public:
    inline virtual ~MWidgetEngineBridge() {};       
    virtual void* Widget(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback) = 0;
    virtual void* Menu(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback) = 0;
    virtual void* MenuItem(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback) = 0;    
    virtual void SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, const TDesC& aValue) = 0;
    virtual void SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, TUint aValue) = 0;
    virtual TBool HandleCommandL( TInt aCommandId ) = 0;        
    virtual void OnShowSelected() = 0;
    virtual void OnHideSelected() = 0;
    virtual void AddOptionMenuItemsL(CEikMenuPane& aMenuPane, TInt aResourceId) = 0;
    virtual void MenuItemSelected( TInt aInternalId ) = 0;
    virtual void MenuShowed() = 0;
    virtual void DrawTransition(CWindowGc& gc, CFbsBitmap* fbsBm) = 0;
    virtual void Clear() = 0;

};

/**
*  WidgetEngineBridge
*
*  @lib widgetengine.dll
*  @since 3.1
*/

NONSHARABLE_CLASS( WidgetEngineBridge ): public MWidgetEngineBridge, public MJSObjectProtector
{
public:
    WidgetEngineBridge();
    virtual ~WidgetEngineBridge();
    
public: //From MWidgetEngineBridge    
    void* Widget(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback);
    void* Menu(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback);
    void* MenuItem(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback);    
    
    void SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, const TDesC& aValue);
    void SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, TUint aValue);
    TBool HandleCommandL( TInt aCommandId );
    void OnShowSelected();
    void OnHideSelected();
    void AddOptionMenuItemsL(CEikMenuPane& aMenuPane, TInt aResourceId);
    void MenuItemSelected( TInt aInternalId );
    void MenuShowed();
    void DrawTransition(CWindowGc& gc, CFbsBitmap* fbsBm);
    void Clear();
    
	void Protect(KJS::JSValue* obj);
	void Unprotect(KJS::JSValue* obj);

private:    
    CMenuClient*			m_menuclient;
    CWidgetClient*			m_widgetclient;                
    WidgetPreferences*		m_preferences;
	WTF::HashSet<KJS::JSValue*>	m_protectedObjects;
 };


#endif
