/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implemetation of JSWidgetFunc
*
*/
#include "WidgetFuncs.h"
#include "Widget.h"
#include "WidgetCallbacks.h"
#include "WidgetEventHandler.h"

#include <interpreter.h>

const int INTBUFSIZE = 256;

// ============================ MEMBER FUNCTIONS ===============================
using namespace KJS;


// ----------------------------------------------------------------------------
// JSWidgetFunc::JSWidgetFunc
//
//
//
// ----------------------------------------------------------------------------
JSWidgetFunc::JSWidgetFunc(ExecState* exec, int fType,
                        MJSWidgetCallbacks* aWidgetCallbacks) : 
                        JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()), funcType(fType),                        
                        m_callbacks(aWidgetCallbacks)
{
}

// ----------------------------------------------------------------------------
// JSWidgetFunc::~JSWidgetFunc
//
//
//
// ----------------------------------------------------------------------------
JSWidgetFunc::~JSWidgetFunc()
{
}

// ----------------------------------------------------------------------------
// JSWidgetFunc::implementsCall
//
//
//
// ----------------------------------------------------------------------------
bool JSWidgetFunc::implementsCall() const
{
    return true;
}

// ----------------------------------------------------------------------------
// JSWidgetFunc::call
//
//
//
// ----------------------------------------------------------------------------

JSValue* JSWidgetFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
{

    switch (funcType) {
        case openApplication: {
            if ( args[0]->type() == NumberType &&
                 args[0]->toString(exec).size() > 0 ) {
                
                TPtrC tstrParam(KNullDesC);
                if ( !args[1]->isNull() && ( args[1]->type() == StringType ) ) {
                    tstrParam.Set((const TUint16 *)args[1]->toString(exec).data(), args[1]->toString(exec).size());
                }
                
                m_callbacks->openApplication(TUid::Uid(args[0]->toInt32(exec)),tstrParam);
            }
        break;
        }
        case openURL: {
            if ( args[0]->type() == StringType &&
                 args[0]->toString(exec).size() > 0 ) {
                TPtrC tstrUrl((const TUint16 *)args[0]->toString(exec).data(), args[0]->toString(exec).size());
                m_callbacks->openUrl(tstrUrl);
            }
        break;
        }
        case preferenceForKey: {
            //the key must be a string or number type
            if ( !args[0]->isNull() &&
                (args[0]->type() == StringType || args[0]->type() == NumberType) &&
                args[0]->toString(exec).size() >= 0 ) {
                
                TPtrC tstrKey(KNullDesC);
                HBufC* tstrValue(NULL);
                TInt retCode = KErrNone;

                if ( args[0]->type() == NumberType ) {
                    TBuf<INTBUFSIZE> intKey;
                    intKey.Num( args[0]->toNumber(exec) );
                    retCode = m_callbacks->preferenceForKey(intKey,tstrValue);
                }
                else {
                    tstrKey.Set((const TUint16 *)args[0]->toString(exec).data(),args[0]->toString(exec).size());
                    retCode = m_callbacks->preferenceForKey(tstrKey,tstrValue);
                }


                if (retCode == KErrNone) {
                    UString u((const UChar *)tstrValue->Ptr(),tstrValue->Length());
                    JSCell* rtn=jsString(u);
                    delete tstrValue;
                    return rtn;
                }
                else {
                    delete tstrValue;
                }

        }
        break;
        }
        case prepareForTransition: {
            
            if (  args[0]->type() == StringType && args[0]->toString(exec).size() > 0 ) {
                TPtrC tstrTrans((const TUint16 *)args[0]->toString(exec).data(), args[0]->toString(exec).size());
                m_callbacks->prepareForTransition(tstrTrans);
            }
        break;
        }
        case performTransition: {
            m_callbacks->performTransition();
        break;
        }
        case setPreferenceForKey: {
                
            if ( !args[1]->isNull() && 
                    args[1]->toString(exec).size() >= 0 ) {
                
                TPtrC tstrValue( KNullDesC );
                TPtrC tstrKey( KNullDesC );

                TBuf<INTBUFSIZE> intKey;
                TBuf<INTBUFSIZE> intVal;

                if ( args[1]->type() == NumberType ) {
                    intKey.Num( args[1]->toNumber(exec) );
                    tstrKey.Set( intKey );
                }
                else {
                    tstrKey.Set((const TUint16 *)args[1]->toString(exec).data(), args[1]->toString(exec).size());
                }

                if ( args[0]->isNull() ) {
                    m_callbacks->removePreferenceForKey(tstrKey,tstrValue);
                }
                else if ( args[0]->type() == NumberType ) {
                    intVal.Num( args[0]->toNumber(exec) );
                    tstrValue.Set( intVal );

                    m_callbacks->setPreferenceForKey(tstrKey,tstrValue);
                }
                else if ( args[0]->toString(exec).size() >= 0 ) {
                    tstrValue.Set((const TUint16 *)args[0]->toString(exec).data(), args[0]->toString(exec).size());

                    m_callbacks->setPreferenceForKey(tstrKey,tstrValue);
                }
            }
        break;
        }
        case setNavigationMode: {
            if ( args[0]->type() == BooleanType ) {
                m_callbacks->setNavigationEnabled(args[0]->toBoolean(exec));
            }
        break;
        }
        case setNavigationType: {
            if (  args[0]->type() == StringType && args[0]->toString(exec).size() > 0 ) {
                TPtrC type((const TUint16 *)args[0]->toString(exec).data(), args[0]->toString(exec).size());
                m_callbacks->setNavigationType(type);
            }
        break;
        }
        case setDisplayLandscape: {
            m_callbacks->setDisplayLandscape();
        break;
        }
        case setDisplayPortrait: {
            m_callbacks->setDisplayPortrait();
        break;
        }
        default:
        break;
    }
            
    return jsUndefined();
}

