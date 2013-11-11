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



// INCLUDE FILES
#include "WidgetEngineBridge.h"
#include "WidgetEventHandler.h"
#include <object.h>
#include <eikamnt.h>
#include <eikmenub.h>
#include <gdi.h>
#include <bitdev.h>
#include "brctlinterface.h"
#include "WidgetEngineCallbacks.h"

#include "WidgetClient.h"
#include "MenuClient.h"
#include "Renderer.h"
#include "Preferences.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMenuItemCommandIdBase = 20000;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================
using namespace KJS;

// ----------------------------------------------------------------------------
// CreateWidgetEngineBridge
// creates WidgetEngineBridge object
//
//
// ----------------------------------------------------------------------------
EXPORT_C MWidgetEngineBridge* CreateWidgetEngineBridge()
    {
    return new WidgetEngineBridge();
    }


// ----------------------------------------------------------------------------
// WidgetEngineBridge::WidgetEngineBridge
// Default constructor
//
//
// ----------------------------------------------------------------------------
WidgetEngineBridge::WidgetEngineBridge() : m_menuclient(0), m_widgetclient(0), m_preferences(0)
{
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::~WidgetEngineBridge
// Destructor
//
//
// ----------------------------------------------------------------------------
WidgetEngineBridge::~WidgetEngineBridge()
{
    Clear();
    delete m_preferences;
    m_preferences = NULL;
}
// ----------------------------------------------------------------------------
// WidgetEngineBridge::Clear
// 
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::Clear()
{    
	// unprotect objects
	HashSet<JSValue*>::iterator end = m_protectedObjects.end();
	for (HashSet<JSValue*>::iterator it = m_protectedObjects.begin(); it != end; ++it) {
		Collector::unprotect(*it);
	}
	m_protectedObjects.clear();

    delete m_menuclient;
    m_menuclient = NULL;
    delete m_widgetclient;    
    m_widgetclient = NULL;
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::Widget
// creates Widget object if it does not exist, otherwise returns the already
// created instance
//
//
// ----------------------------------------------------------------------------
void* WidgetEngineBridge::Widget(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback)
{
    if (!m_widgetclient) {
        
        if (!m_preferences)
            m_preferences = new (ELeave) WidgetPreferences();
        
        m_widgetclient = CWidgetClient::NewL(aWidgetCallback, this, aWidgetEngineCallback,m_preferences);            
    }    
    return m_widgetclient->jswidget();  
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::Menu
// creates Menu object if it does not exist, otherwise returns the already
// created instance
//
//
// ----------------------------------------------------------------------------
void* WidgetEngineBridge::Menu(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback)
{
    if (!m_menuclient){
        m_menuclient = CMenuClient::NewL(aWidgetCallback,aWidgetEngineCallback);      
    }    
    return m_menuclient->jsmenu(this);  
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::MenuItem
// creates MenuItem object if it does not exist, otherwise returns the already
// created instance
//
//
// ----------------------------------------------------------------------------
void* WidgetEngineBridge::MenuItem(MWidgetCallback& aWidgetCallback, MWidgetEngineCallbacks& aWidgetEngineCallback)
{
    if (!m_menuclient){
        m_menuclient = CMenuClient::NewL(aWidgetCallback,aWidgetEngineCallback);      
    }    
    return m_menuclient->jsmenuitem(this);      
}

// ----------------------------------------------------------------------------
//WidgetEngineBridge::SetParamL
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, const TDesC& aValue)
{   
    if (!m_preferences)
        m_preferences = new (ELeave) WidgetPreferences();

    switch( aParam ) {
        case TBrCtlDefs::EWidgetBasePath: {
            m_preferences->setBasePathL(aValue);
            m_preferences->loadL();            
            break;
        }
        case TBrCtlDefs::EWidgetBundleId: {
            m_preferences->setWidgetBundleId(aValue);
            break;
        }
        default:
        break;
    }
    
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::SetParamL
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::SetParamL(TBrCtlDefs::TBrCtlWidgetParams aParam, TUint aValue)
{   
  
    if (!m_preferences)
        m_preferences = new (ELeave) WidgetPreferences();

    switch( aParam ) {
        case TBrCtlDefs::EWidgetIdentifier: {
            m_preferences->setWidgetId(aValue);
            break;
        }
        default:
        break;
    }

}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::HandleCommandL 
//
//
// ----------------------------------------------------------------------------
TBool WidgetEngineBridge::HandleCommandL( TInt aCommandId )
{
    TBool handled(EFalse);
    
    if (!handled && m_menuclient) {
        handled = m_menuclient->HandleCommandL(aCommandId);
    }
    if (!handled && m_widgetclient) {
        handled = m_widgetclient->HandleCommandL(aCommandId);
    }
    return handled;
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::OnShowSelected
// Invokes callback function when the menu item is selected
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::OnShowSelected()
{
    if (m_widgetclient) {
        m_widgetclient->onShowSelected();
    }
}


// ----------------------------------------------------------------------------
// WidgetEngineBridge::OnHideSelected
// Invokes callback function when the menu item is selected
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::OnHideSelected()
{
    if (m_widgetclient) {
        m_widgetclient->onHideSelected();
    }
}


// ----------------------------------------------------------------------------
// WidgetEngineBridge::AddOptionMenuItemsL
// 
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::AddOptionMenuItemsL(CEikMenuPane& aMenuPane, TInt aResourceId)
{
    if (m_menuclient) {
        m_menuclient->addOptionMenuItemsL(aMenuPane, aResourceId);
    }    
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::MenuItemSelected
// Invokes callback function when the menu item is selected
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::MenuItemSelected( TInt aInternalId )
{
    if (m_menuclient) {
        m_menuclient->menuItemSelected(aInternalId-KMenuItemCommandIdBase);
    }
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::MenuShowed
// Invokes callback function when Menu is displayed
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::MenuShowed()
{
    if (m_menuclient) {
        m_menuclient->menuShowed();
    }
}

// ----------------------------------------------------------------------------
// WidgetEngineBridge::DrawTransition
// 
//
//
// ----------------------------------------------------------------------------
void WidgetEngineBridge::DrawTransition(CWindowGc& gc, CFbsBitmap* fbsBm)
{
    if (m_widgetclient && m_widgetclient->renderer()) {
        m_widgetclient->renderer()->drawTransition(gc,fbsBm);
    }
}


void WidgetEngineBridge::Protect(JSValue* obj)
{
	m_protectedObjects.add(obj);
	Collector::protect(obj);
}

void WidgetEngineBridge::Unprotect(JSValue* obj)
{
	m_protectedObjects.remove(obj);
	Collector::unprotect(obj);
}

//END OF FILE


