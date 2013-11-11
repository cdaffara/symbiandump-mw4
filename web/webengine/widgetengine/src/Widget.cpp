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
#include <sysutil.h>
#include "../../bidi.h"
#include <aknappui.h>

#include "Widget.h"
#include "WidgetFuncs.h"
#include "WidgetCallbacks.h"
#include "WidgetEventHandler.h"
#include "WidgetJSObjectProtector.h"
#include "Wrt.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
using namespace KJS;
const ClassInfo JSWidget::info = { "JSWidget", 0, &WidgetTable, 0 };

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin WidgetTable 17
    openApplication JSWidget::openApplication DontDelete|Function 1
    openURL JSWidget::openURL DontDelete|Function 1
    preferenceForKey JSWidget::preferenceForKey DontDelete|Function 1
    prepareForTransition JSWidget::prepareForTransition DontDelete|Function 1
    performTransition JSWidget::performTransition DontDelete|Function 0
    setPreferenceForKey JSWidget::setPreferenceForKey DontDelete|Function 2
    setNavigationEnabled JSWidget::setNavigationEnabled DontDelete|Function 1
    setNavigationType JSWidget::setNavigationType DontDelete|Function 1
    identifier JSWidget::identifier DontDelete|ReadOnly
    onhide JSWidget::onhide DontDelete|ReadOnly
    onremove JSWidget::onremove DontDelete|ReadOnly
    onshow  JSWidget::onshow DontDelete|ReadOnly
    onexit  JSWidget::onexit DontDelete|ReadOnly
    setDisplayLandscape JSWidget::setDisplayLandscape DontDelete|Function 0
    setDisplayPortrait JSWidget::setDisplayPortrait DontDelete|Function 0
    isrotationsupported JSWidget::isRotationSupported DontDelete|ReadOnly
    wrt JSWidget::wrt  DontDelete|ReadOnly
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// JSWidget::JSWidget
//
//
//
// ----------------------------------------------------------------------------
JSWidget::JSWidget(MJSWidgetCallbacks* aWidgetCallbacks, MJSObjectProtector* aProtector) :
                    JSObject(),
                    d(new WidgetPrivate(aWidgetCallbacks,0,0,0,0, aProtector))
{
}

// ----------------------------------------------------------------------------
// JSWidget::~JSWidget
//
//
//
// ----------------------------------------------------------------------------
JSWidget::~JSWidget()
{
    delete d;
}

// ----------------------------------------------------------------------------
// JSWidget::type
//
//
//
// ----------------------------------------------------------------------------
JSType JSWidget::type() const
{
    return ObjectType;
}

// ----------------------------------------------------------------------------
// JSWidget::mark
//
//
//
// ----------------------------------------------------------------------------
void JSWidget::mark()
{
	JSObject::mark();
	if(d->m_wrt)
	   d->m_wrt->mark();
}

// ----------------------------------------------------------------------------
// JSWidget::toString
//
//
//
// ----------------------------------------------------------------------------
UString JSWidget::toString(ExecState *exec) const
{
    return "[object JSWidget]";
}


// ----------------------------------------------------------------------------
// JSWidget::getOwnPropertySlot
//
//
//
// ----------------------------------------------------------------------------
bool JSWidget::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    const HashEntry* entry = Lookup::findEntry(&WidgetTable, propertyName);
    if (entry)
        {
        slot.setStaticEntry(this, entry, staticValueGetter<JSWidget>);
        return true;
        }

    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}


// ----------------------------------------------------------------------------
// JSWidget::getValueProperty
//
//
//
// ----------------------------------------------------------------------------
JSValue* JSWidget::getValueProperty(KJS::ExecState* exec, int token) const
{

    switch (token)  {
        case openApplication:
        case openURL:
        case preferenceForKey:
        case prepareForTransition:
        case performTransition:
        case setPreferenceForKey:
        case setNavigationEnabled:
        case setNavigationType:
        case setDisplayLandscape:
        case setDisplayPortrait: {
            JSWidgetFunc *wf = new JSWidgetFunc(exec,token,d->m_callbacks);
            return wf;
        }
        case identifier: {
            TPtrC tstrUrl(d->m_callbacks->getWidgetBundleId());
            return jsString(UString((const KJS::UChar*)tstrUrl.Ptr(), tstrUrl.Length()));
        }
        case isRotationSupported: {
            return jsBoolean(iAvkonAppUi->OrientationCanBeChanged());
        }
        case wrt: {
            return d->m_wrt ? d->m_wrt : d->m_wrt = new JSWrt(exec);
        }
        default:
            return throwError(exec, GeneralError);
    }

}

// ----------------------------------------------------------------------------
// JSWidget::canPut
//
//
//
// ----------------------------------------------------------------------------
bool JSWidget::canPut(ExecState *exec, const Identifier &propertyName) const
{
    return true;
}

// ----------------------------------------------------------------------------
// JSWidget::put
//
//
//
// ----------------------------------------------------------------------------
void JSWidget::put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
    const HashEntry* entry = Lookup::findEntry(&WidgetTable, propertyName);
    if (entry) {
        switch( entry->value ) {
            case onhide: {
                delete d->m_hideCallback;
                d->m_hideCallback = NULL;
                d->m_hideCallback = new WidgetEventHandler(value, exec->lexicalInterpreter()->globalExec(), d->m_protector);
                break;
            }
            case onshow: {
                delete d->m_showCallback;
                d->m_showCallback = NULL;
                d->m_showCallback = new WidgetEventHandler(value, exec->lexicalInterpreter()->globalExec(), d->m_protector);
                // fire the onShow if the SetVisibilty was called before the onShowCallback was created
                if (d->m_visibility) {
                    d->m_visibility = !d->m_visibility;
                    setVisibility(!d->m_visibility);
                }
                break;
            }
            case onexit: {
                delete d->m_exitCallback;
                d->m_exitCallback = NULL;
                d->m_exitCallback = new WidgetEventHandler(value, exec->lexicalInterpreter()->globalExec(), d->m_protector);
                break;
            }
            default:
                break;
        }
    }

}


// ----------------------------------------------------------------------------
// JSWidget::SetVisibility
// Invokes onShow or onHide callback function depending on the visibility
// status of the widget
//
//
//
// ----------------------------------------------------------------------------
void JSWidget::setVisibility(bool visibility)
{
    if (d->m_visibility != visibility) {
        d->m_visibility = visibility;
        if(visibility && d->m_showCallback)
            d->m_showCallback->InvokeCall();
        else if (d->m_hideCallback)
            d->m_hideCallback->InvokeCall();
    }
}

// ----------------------------------------------------------------------------
// JSWidget::handleExit
// Invokes onExit callback function
//
//
//
// ----------------------------------------------------------------------------
void JSWidget::handleExit()
{
    if (d->m_exitCallback)
        d->m_exitCallback->InvokeCall();
}
//END OF FILE




