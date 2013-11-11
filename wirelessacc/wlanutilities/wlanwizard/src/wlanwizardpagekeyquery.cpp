/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* WLAN Wizard Page: Key query page for WEP and WPA (2).
*/

// System includes

#include <HbDocumentLoader>
#include <HbMainWindow>
#include <HbWidget>
#include <HbLabel>
#include <HbLineEdit>
#include <HbEditorInterface>
#include <HbParameterLengthLimiter>

// User includes

#include "wlanwizard.h"
#include "wlanwizard_p.h"
#include "wlanwizardpagekeyquery.h"
#include "wlanwizardutils.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpagekeyqueryTraces.h"
#endif


/*!
   \class WlanWizardPageKeyQuery
   \brief Implements Key Query page for WEP and WPA. 
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor.
   
   @param [in] parent pointer to private implementation of wizard.
 */
WlanWizardPageKeyQuery::WlanWizardPageKeyQuery(WlanWizardPrivate* parent) :
    WlanWizardPageInternal(parent), 
    mWidget(NULL), 
    mLabelTitle(NULL),
    mLineEdit(NULL), 
    mLabelError(NULL), 
    mDocLoader(NULL)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEKEYQUERY_WLANWIZARDPAGEKEYQUERY_ENTRY);
    OstTraceFunctionExit0(WLANWIZARDPAGEKEYQUERY_WLANWIZARDPAGEKEYQUERY_EXIT);
}

/*!
   Destructor.
 */
WlanWizardPageKeyQuery::~WlanWizardPageKeyQuery()
{
    OstTraceFunctionEntry0(DUP1_WLANWIZARDPAGEKEYQUERY_WLANWIZARDPAGEKEYQUERY_ENTRY);
    
    delete mDocLoader;
    
    // Wizard framework deletes the visualization (owns mWidget).
    OstTraceFunctionExit0(DUP1_WLANWIZARDPAGEKEYQUERY_WLANWIZARDPAGEKEYQUERY_EXIT);
}

/*!
   See WlanWizardPage::initializePage()
   
   Initializes the key query page and initializes objects based on the security
   mode.
 */
HbWidget* WlanWizardPageKeyQuery::initializePage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEKEYQUERY_INITIALIZEPAGE_ENTRY);
    
    if (!mWidget) {
        bool ok;
        
        mDocLoader = new HbDocumentLoader(mWizard->mainWindow());
        
        // Load document common part
        mDocLoader->load(":/docml/occ_add_wlan_01_04.docml", &ok);
        Q_ASSERT(ok);
              
        // Initialize orientation
        loadDocmlSection(mWizard->mainWindow()->orientation());
        
        // Load widgets
        mWidget = 
            qobject_cast<HbWidget*> (mDocLoader->findWidget("occ_add_wlan_01"));
        Q_ASSERT(mWidget != NULL);

        mLabelTitle = qobject_cast<HbLabel*> (mDocLoader->findWidget("dialog"));
        Q_ASSERT(mLabelTitle != NULL);

        mLineEdit = qobject_cast<HbLineEdit*> (mDocLoader->findWidget("lineEditKey"));
        Q_ASSERT(mLineEdit != NULL);

        mLabelError = 
            qobject_cast<HbLabel*> (mDocLoader->findWidget("labelErrorNote"));
        Q_ASSERT(mLabelError != NULL);
        
        ok = connect(
            mWizard->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);
        
        HbEditorInterface editInterface(mLineEdit);
        editInterface.setInputConstraints(HbEditorConstraintLatinAlphabetOnly);
        editInterface.setSmileyTheme(HbSmileyTheme());
        mLineEdit->setInputMethodHints(
            Qt::ImhNoPredictiveText | Qt::ImhPreferLowercase);
        mLineEdit->installEventFilter(this);
    }

    if (mWizard->configuration(WlanWizardHelper::ConfSecurityMode) ==
        CMManagerShim::WlanSecModeWep) {
        mLineEdit->setMaxLength(WlanWizardUtils::WepHex128BitMaxLength);    
    } else {
        mLineEdit->setMaxLength(WlanWizardUtils::WpaMaxLength);
    }
    
    mLabelTitle->setPlainText(
        HbParameterLengthLimiter("txt_occ_dialog_enter_key_for_1").arg(
            mWizard->configuration(WlanWizardPrivate::ConfName).toString()));
    
    OstTraceFunctionExit0(WLANWIZARDPAGEKEYQUERY_INITIALIZEPAGE_EXIT);
    return mWidget;
}

/*!
   Loads docml at initialization phase and when HbMainWindow sends orientation()
   signal.
   
   @param [in] orientation to be loaded.
 */
void WlanWizardPageKeyQuery::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEKEYQUERY_LOADDOCMLSECTION_ENTRY);
    
    WlanWizardPageInternal::loadDocmlSection(
        mDocLoader,
        orientation,
        ":/docml/occ_add_wlan_01_04.docml", 
        "portrait_section",
        "landscape_section");
    
    OstTraceFunctionExit0(WLANWIZARDPAGEKEYQUERY_LOADDOCMLSECTION_EXIT);
}

/*!
   See WlanWizardPage::nextId()
   
   @param [in,out] removeFromStack return value is always false
   
   @return In case user has entered incorrect key an appropriate error text is
   displayed and WlanWizardPage::PageNone is returned.
   
   In case user has entered a valid WEP or WPA key, the key is stored into
   internal configuration and WlanWizardPage::PageProcessSettings is returned.
   
 */
int WlanWizardPageKeyQuery::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEKEYQUERY_NEXTID_ENTRY);
    
    WlanWizardUtils::KeyStatus status(WlanWizardUtils::KeyStatusOk);
    int pageId = WlanWizardPage::nextId(removeFromStack);
    int secMode = mWizard->configuration(
        WlanWizardPrivate::ConfSecurityMode).toInt();
    
    if (secMode == CMManagerShim::WlanSecModeWep) {
        status = WlanWizardUtils::validateWepKey(mLineEdit->text());   
    }
    else {
        Q_ASSERT(
            secMode == CMManagerShim::WlanSecModeWpa ||
            secMode == CMManagerShim::WlanSecModeWpa2);
        
        status = WlanWizardUtils::validateWpaKey(mLineEdit->text());
    }
    QString errorString = keyStatusToErrorString(status);
        
    if (errorString.length() == 0 ) {
        if (secMode == CMManagerShim::WlanSecModeWep) {
            mWizard->setConfiguration(WlanWizardPrivate::ConfKeyWep1, mLineEdit->text());
            mWizard->setConfiguration(WlanWizardPrivate::ConfKeyWep2, mLineEdit->text());
            mWizard->setConfiguration(WlanWizardPrivate::ConfKeyWep3, mLineEdit->text());
            mWizard->setConfiguration(WlanWizardPrivate::ConfKeyWep4, mLineEdit->text());
            mWizard->setConfiguration(WlanWizardPrivate::ConfKeyWepDefault, 
                CMManagerShim::WepKeyIndex1);
        } else {
            mWizard->setConfiguration(WlanWizardPrivate::ConfKeyWpa, mLineEdit->text());   
        }
        pageId = WlanWizardPage::PageProcessSettings;
    }
    else {
        mLabelError->setPlainText(errorString);
    }

    removeFromStack = false;
    
    OstTraceFunctionExit0(WLANWIZARDPAGEKEYQUERY_NEXTID_EXIT);
    return pageId;
}

/*!
   See WlanWizardPage::showPage()
 */
bool WlanWizardPageKeyQuery::showPage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEKEYQUERY_SHOWPAGE_ENTRY);
    
    // Open virtual keyboard by setting focus to line edit
    mLineEdit->setFocus();
    
    OstTraceFunctionExit0(WLANWIZARDPAGEKEYQUERY_SHOWPAGE_EXIT);
    return true;
}

/*!
   When focus moves into the Key editor and there is visible error text, it
   is cleared.
   
   @param [in] obj Pointer to the object the event was sent to
   @param [in] event Pointer to the sent event.
   
   @return false. do not eat the event.
 */
bool WlanWizardPageKeyQuery::eventFilter(QObject *obj, QEvent *event)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEKEYQUERY_EVENTFILTER_ENTRY);
    
    if (obj == mLineEdit && event->type() == QEvent::FocusIn) {
        mLabelError->setPlainText("");
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGEKEYQUERY_EVENTFILTER_EXIT);
    return false;
}

/*!
   Maps provided error code to error text.
   
   @param [in] status status of key validation
   
   @return An error text in case of error, empty string is returned on 
   successful case.
 */
QString WlanWizardPageKeyQuery::keyStatusToErrorString(
    WlanWizardUtils::KeyStatus status) const
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEKEYQUERY_KEYSTATUSTOERRORSTRING_ENTRY);
    
    QString errorString;
    switch (status) {
    case WlanWizardUtils::KeyStatusIllegalCharacters:
        errorString = hbTrId("txt_occ_dialog_illegal_characters_in_key_please_c");
        break;

    case WlanWizardUtils::KeyStatusWpaTooShort:
        errorString = hbTrId("txt_occ_dialog_preshared_key_too_short_at_least");
        break;

    case WlanWizardUtils::KeyStatusWepInvalidLength:
        errorString = hbTrId("txt_occ_dialog_key_is_of_incorrect_length_please");
        break;

    default:
        Q_ASSERT(WlanWizardUtils::KeyStatusOk == status);
        // nothing to do here.
        break;
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGEKEYQUERY_KEYSTATUSTOERRORSTRING_EXIT);
    return errorString;
}
