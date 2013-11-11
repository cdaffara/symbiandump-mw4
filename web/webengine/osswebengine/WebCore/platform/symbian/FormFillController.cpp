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

#include "config.h"
#include "FormFillController.h"
#include "Event.h"
#include "Timer.h"
#include "string.h"
#include "HTMLInputElement.h"
#include "HTMLFormElement.h"
#include "FontCache.h"
#include "FontPlatformData.h"
#include "FormDataStore.h"
#include "FormLoginStore.h"
#include "KURL.h"
#include "HTMLNames.h"
#include "DeprecatedString.h"
#include "Document.h"
#include "IntRect.h"
#include "Settings.h"
#include "FormFillCallback.h"
#include "FormFillPopup.h"
#include "PlatformFontCache.h"
#include "StaticObjectsContainer.h"
namespace WebCore {

static const double kSearchStartTimeout = 0.5f;

FormFillController::FormFillController() 
                    : m_popup(0), m_callback(0), m_inputElement(0), m_formDB(0), m_passwdDB(0)
{
    m_searchTimer = new Timer<FormFillController>(this, &FormFillController::fireSearch);
}

FormFillController::~FormFillController()
{
    delete m_searchTimer;
    delete m_formDB;
    delete m_passwdDB;
    if (m_popup) {
        delete m_popup;
        m_popup = NULL;
    }
}

void FormFillController::fireSearch(Timer<FormFillController>*)
{
    // start searching
    if(!m_formDB) {
        m_formDB = new FormDataStore(this);
    }

    // search is synchrous
    m_formDB->search(m_searchParam, m_searchValue, m_results);

    updatePopupView();
}

void FormFillController::updatePopupView()
{
    if(m_results.size() > 0) {
        // lazily create the popup view
        PlatformFontCache* cache = StaticObjectsContainer::instance()->fontCache();
        float zoomFactor =  (float)cache->fontZoomFactor();
        if(!m_popup) {
            // system font to be used by popup

            float newFont = 12.0f * zoomFactor /100.0f;
            FontPlatformData* font = FontCache::getDeviceDefaultFont(newFont);
            m_popup = m_callback->createFormFillPopup(font->Font());
        }
        if (!m_popup) {
            return;
        }
        m_popup->clear();
        // fill the popup
        for(int i=0; i<m_results.size(); ++i)
            m_popup->addRow(m_results.at(i));

        TRect inputRect = m_inputElement->getRect();
        TInt  widthOffset = (zoomFactor - 100)*inputRect.Width()/100;
        TInt  heightOffset = (zoomFactor - 100)*inputRect.Height()/100;

        inputRect.Resize(widthOffset,heightOffset);
        m_popup->setLocationHintInDoc(inputRect, m_inputElement->document()->frame());
        m_popup->invalidate();
    } else if(m_popup) {
        m_popup->clear();
    }    
}

void FormFillController::setInputElement(MFormFillCallback* callback, HTMLInputElement* input)
{
    clearInputElement();
    m_inputElement = input;
    m_callback = callback;
                            
    startSearch(m_inputElement->name(), m_inputElement->value() );
}

void FormFillController::clearInputElement()
{
    stopSearch();
    m_inputElement = NULL;
    m_callback = NULL;
    m_results.clear();
    m_searchParam = String();
    m_searchValue = String();

    if (m_popup) {
        m_popup->clear();
    }
}

void FormFillController::startSearch()
{
    startSearch(m_inputElement->name(), m_inputElement->value() );
}

void FormFillController::startSearch(const String& name_, const String& value_)
{
    if(m_searchTimer->isActive() && m_searchParam == name_ && m_searchValue == value_) {
        return;
    }

    // we don't need to fire search timer if we already covered this string
    // in the previous successful search.
    if (m_searchParam == name_ && value_.startsWith(m_searchValue) && !m_searchTimer->isActive()) {
        for (int i=0; i<m_results.size(); ++i) {
            if (!m_results[i].startsWith(value_)) {
                m_results.remove(i--);
            }
        }

        m_searchValue = value_;
        updatePopupView();
        return;
    }

    // stop ongoing search
    stopSearch();
    m_searchParam = name_;
    m_searchValue = value_;
    m_searchTimer->startOneShot(kSearchStartTimeout);
}

void FormFillController::stopSearch()
{
    if(m_searchTimer->isActive())
        m_searchTimer->stop();
}

void FormFillController::insertInputNameAndValue(const String& name_, const String& value_)
{
    if (value_.isEmpty())
        return;

    if(!m_formDB)
        m_formDB = new FormDataStore(this);

    m_formDB->add(name_, value_);
}

void FormFillController::fillPasswordIfExists(HTMLInputElement* uNode)
{   
    HTMLFormElement* form = uNode->form();
    if (!m_passwdDB)
        m_passwdDB = new FormLoginStore();

    if (form) {
        int passwds = 0;
        HTMLInputElement* passwd = searchPasswordFields(form, uNode, passwds);
        if (passwds == 1) {
            // search if we have already stored the password
            String passwdValue;
            String realm = passwdRealm(uNode->document()->URL());
            if (!realm.isEmpty() && m_passwdDB->search(realm, uNode->name(), 
                                    uNode->value(), passwd->name(), passwdValue)) {
                passwd->setValue(passwdValue);
            }
        }
    }
}

void FormFillController::saveFormData(HTMLFormElement* form, MFormFillCallback* callback, bool passwdFillEnabled)
{
    // should delete the popup somewhere else, however, FepManger crashes
    // if we delete the popup while editing - weird
    if (m_popup) {
        delete m_popup;
        m_popup = NULL;
    }

    // form has "autocomplete=off"
    if (!form->autoComplete())
        return;

    bool onePasswdSaved = false;
    // step through elements and ignore input element with "autocomplete=off",
    for (unsigned i = 0; i < form->formElements.size(); ++i) {
        HTMLGenericFormElement* current = form->formElements[i];
        if (current->hasTagName(HTMLNames::inputTag)) {
            HTMLInputElement* input = static_cast<HTMLInputElement*>(current);
            if (input->inputType() == HTMLInputElement::TEXT) {
                if (input->autoComplete()) { 
                
                    bool ignorefield = false;    

                    // find the matching password and save login data
                    int passwdFields = 0;
                    HTMLInputElement* passwd = NULL;
                    if (passwdFillEnabled) 
                        passwd = searchPasswordFields(form, input, passwdFields);
                    String realm = passwdRealm(input->document()->URL());
                    if (passwdFields == 1) {
                        if (!m_passwdDB)
                            m_passwdDB = new FormLoginStore();

                        // we first check the black list, don't bother the user with
                        // queries if the realm is in black list
                        if (!m_passwdDB->saveAllowed(realm))
                            ignorefield = true;

                        // if login info is already saved, don't save again
                        if (!ignorefield && m_passwdDB->contains(realm, input->name(), input->value(), passwd->name(), passwd->value())) {
                            ignorefield = true;
                            onePasswdSaved = true;
                        }
                        // ask user if she want to save login info now, later or never.
                        bool saveNow = false, neverSave = false;
                        if (!ignorefield && !onePasswdSaved)
                            TRAP_IGNORE(callback->queryUserAboutAutoFillL(saveNow, neverSave));

                        if (saveNow) {
                            m_passwdDB->add(realm, input->name(), input->value(), passwd->name(), passwd->value());
                            m_passwdDB->commit();
                            onePasswdSaved = true;
                        } else if (neverSave) {
                            // if there is already login info associated with this realm, remove it.
                            m_passwdDB->removeLoginsForRealm(realm);
                            m_passwdDB->setSaveNotAllowed(realm);
                            m_passwdDB->commit();
                            ignorefield = true;
                        } else if (!ignorefield) {
                            // don't save this time, should delete previously-save 
                            m_passwdDB->removePartial(realm, input->name(), input->value(), passwd->name());
                            m_passwdDB->commit();
                        } 

                       
                    }
                    else if (passwdFields > 1) {
                        // most likely this is a password change form, need to purge
                        // the saved login for this form.
                      if (m_passwdDB) {
                        m_passwdDB->remove(realm, input->name(), input->value(), passwd->name(), passwd->value());
                        m_passwdDB->commit();
                      }
                    }
                    if (!ignorefield)
                        insertInputNameAndValue(input->name(), input->value());
                }
            }
        }        
    }
}

HTMLInputElement* FormFillController::searchPasswordFields(HTMLFormElement* form, HTMLInputElement* uNode, int& passwds)
{
    // search the password field, we assume there is only one password field in
    // a form and it is adjacent to the username field. If the form has more than
    // one passwd field, we assume it is a password change form and will not save it.
    HTMLInputElement* passwd = NULL;
    int firstPasswdIdx = -1, usernameIdx = -1;

    // HTMLFormElement doesn't encapsulate formElements, weird!!!
    for (unsigned i = 0; i < form->formElements.size(); ++i) {
        HTMLGenericFormElement* current = form->formElements[i];
        if (current->hasTagName(HTMLNames::inputTag)) {
            HTMLInputElement* input = static_cast<HTMLInputElement*>(current);
            if (input == uNode) {
                usernameIdx = i;
                continue;
            }
            else if (usernameIdx != -1 && input->inputType() == HTMLInputElement::TEXT) {
                // current element is a text input element positioning between uNode and 
                // any possible successing password field.  So uNode can't be a
                // usename field.
                break;
            }
            else if (input->inputType() == HTMLInputElement::PASSWORD) {
                passwds++;
                if (firstPasswdIdx == -1) {
                    passwd = input;
                    firstPasswdIdx = i;
                }
                else {
                    break;
                }
            }
        }
    }
    
    return passwd;
}

String FormFillController::passwdRealm(const DeprecatedString& str)
{
    KURL url(str);
    String realm;
    if(!url.host().isEmpty()) {
        realm.append(url.protocol());
        realm.append("://");
        realm.append(url.host());
        realm.append(":");
        realm.append(DeprecatedString().setNum(url.port()));
    }
    return realm;
}

void FormFillController::clearFormData()
{
    stopSearch();
    if (!m_formDB)
        m_formDB = new FormDataStore(this);
    m_formDB->deleteAll();
    delete m_formDB;
    m_formDB = NULL;
}

void FormFillController::clearLoginData()
{
    if (!m_passwdDB)
        m_passwdDB = new FormLoginStore();
    m_passwdDB->deleteAll();
}
void FormFillController::destroyPopup()
{
    if (m_popup) {
        m_popup->clear();
        delete m_popup;
        m_popup = NULL;
    }
}

} // END OF FILE
