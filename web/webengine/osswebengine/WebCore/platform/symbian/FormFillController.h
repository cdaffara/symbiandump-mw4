/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __WEBCOREFORMFILLCONTROLLER_H__
#define __WEBCOREFORMFILLCONTROLLER_H__

#include "Timer.h"
#include "PlatformString.h"

class MFormFillPopup;
class MFormFillCallback;

namespace WebCore {

class FormDataStore;
class FormLoginStore;
class HTMLInputElement;
class HTMLFormElement;
class AtomicString;

class FormFillController
{
public:
    FormFillController();
    ~FormFillController();

    void startSearch();
    void startSearch(const String& name_, const String& value_);
    void stopSearch();
    void setInputElement(MFormFillCallback* callback, HTMLInputElement* input);
    void clearInputElement();

    // data store operations
    void insertInputNameAndValue(const String& name_, const String& value_);
    HTMLInputElement* inputElement() const              { return m_inputElement; }

    // password
    void fillPasswordIfExists(HTMLInputElement* uNode);
    void saveFormData(HTMLFormElement* form, MFormFillCallback* callback, bool passwdFillEnabled);

    void enableFormDataFill(bool aEnabled);
    void enablePasswordFill(bool aEnabled);

    void clearFormData();
    void clearLoginData();
    void updatePopupView();
    void destroyPopup();

private:
    FormFillController(const FormFillController&);              // not implemented
    FormFillController& operator=(const FormFillController&);   // not implemented

    // timer callback
    void fireSearch(Timer<FormFillController>* timer_);

    String passwdRealm(const DeprecatedString&);
    HTMLInputElement* searchPasswordFields(HTMLFormElement*, HTMLInputElement*, int&);

    MFormFillPopup* m_popup;
    MFormFillCallback* m_callback;
    HTMLInputElement* m_inputElement;
    Timer<FormFillController>* m_searchTimer;
    FormDataStore* m_formDB;
    FormLoginStore* m_passwdDB;
    String m_searchParam;
    String m_searchValue;
    Vector<String> m_results;
};

}

#endif //!__WEBCOREFORMFILLCONTROLLER_H__
