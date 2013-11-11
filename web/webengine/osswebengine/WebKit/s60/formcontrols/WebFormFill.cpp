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
* Description:  FormFill
*
*/


// INCLUDE FILES
#include <../bidi.h>
#include "WebFormFill.h"
#include "WebFormFillPopup.h"
#include "WebView.h"
#include "BrCtl.h"
#include "SettingsContainer.h"
#include "StaticObjectsContainer.h"
#include "FormFillController.h"
#include "FormState.h"
#include "HTMLInputElement.h"
#include <webkit.rsg>

#include <StringLoader.h>


using namespace WebCore;

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS



// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WebFormFill::WebFormFill
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
WebFormFill::WebFormFill( WebView* view )
    : m_webView(view)
{
}

// Destructor
WebFormFill::~WebFormFill()
{
}

void WebFormFill::willSubmitForm(FormState* formState)
{
    int formDataType = m_webView->brCtl()->settings()->brctlSetting(TBrCtlDefs::ESettingsAutoFormFillEnabled);
    if (formDataType != TBrCtlDefs::EFormDataOff) {
        StaticObjectsContainer::instance()->formFillController()->saveFormData(formState->form(), this, formDataType ==  TBrCtlDefs::EFormDataPlusPassword);
    }
}

void WebFormFill::queryUserAboutAutoFillL(bool& saveNow, bool& neverSave)
{
    MBrCtlDialogsProvider* dialogProvider = m_webView->brCtl()->brCtlDialogsProvider();
    saveNow = false; 
    neverSave = false;
    if(dialogProvider) {
        HBufC* leftkey = StringLoader::LoadLC(R_TEXT_SOFTKEY_YES);
        HBufC* rightkey = StringLoader::LoadLC(R_TEXT_SOFTKEY_NO);
        HBufC* saveQuery = StringLoader::LoadLC(R_BROWSER_FORM_AUTOFILL_SAVE);
        HBufC* blockQuery = StringLoader::LoadLC(R_BROWSER_FORM_AUTOFILL_ASK_LATER);
        saveNow = dialogProvider->DialogConfirmL(_L(" "),*saveQuery,*leftkey,*rightkey);
        if (!saveNow) {
            neverSave = !dialogProvider->DialogConfirmL(_L(" "),*blockQuery,*leftkey,*rightkey);
        }
        CleanupStack::PopAndDestroy(4); // leftKey, rightKey, saveQuery, blockQuery
    }
}

MFormFillPopup* WebFormFill::createFormFillPopup(CFont* font)
{
    WebFormFillPopup* popup = NULL;
    TRAP_IGNORE(popup = WebFormFillPopup::NewL(m_webView, font, this));
    return popup;
}

void WebFormFill::autoComplete(TPtrC text)
{
    FormFillController* formfill = StaticObjectsContainer::instance()->formFillController();
    HTMLInputElement* inputNode = formfill->inputElement();
    if( inputNode ) {
        inputNode->setValue(text);
        if (m_webView->brCtl()->settings()->brctlSetting(TBrCtlDefs::ESettingsAutoFormFillEnabled) == TBrCtlDefs::EFormDataPlusPassword) {
            formfill->fillPasswordIfExists(inputNode);
        }
    }
    formfill->stopSearch();
}

void WebFormFill::cancelPopup()
{
    StaticObjectsContainer::instance()->formFillController()->clearInputElement();
    //m_webView->setEditable(ETrue);
}

void WebFormFill::textChanged(Element* inputElement)
{
    int formDataType = m_webView->brCtl()->settings()->brctlSetting(TBrCtlDefs::ESettingsAutoFormFillEnabled);
    if (formDataType != TBrCtlDefs::EFormDataOff) {
        if (StaticObjectsContainer::instance()->formFillController()->inputElement() != inputElement) {
            StaticObjectsContainer::instance()->formFillController()->setInputElement(this, static_cast<HTMLInputElement*>(inputElement));
        }
        else {
            StaticObjectsContainer::instance()->formFillController()->startSearch();
        }
    }
}

void WebFormFill::updatePopupView()
{
    StaticObjectsContainer::instance()->formFillController()->updatePopupView();
}

//  End of File
